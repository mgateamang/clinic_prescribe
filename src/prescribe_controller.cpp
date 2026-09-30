#include "prescribe_controller.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>

namespace aidans {

PrescribeController::PrescribeController(QObject* parent)
    : QObject(parent) {
}

void PrescribeController::setServerUrl(const QString& url) {
    m_serverUrl = url;
}

void PrescribeController::setStatus(const QString& msg) {
    if (m_statusMessage != msg) {
        m_statusMessage = msg;
        emit statusMessageChanged();
    }
}

void PrescribeController::setBusy(bool busy) {
    if (m_isBusy != busy) {
        m_isBusy = busy;
        emit isBusyChanged();
    }
}

bool PrescribeController::addItem(int inventoryId, const QString& itemName, int quantity, const QString& dosage) {
    if (inventoryId <= 0 || quantity <= 0 || dosage.trimmed().isEmpty()) {
        setStatus("Error: Invalid medication, quantity must be > 0 and dosage specified.");
        return false;
    }

    QVariantMap item;
    item["inventory_item_id"] = inventoryId;
    item["item_name"] = itemName;
    item["quantity"] = quantity;
    item["dosage_instructions"] = dosage.trimmed();

    m_cart.append(item);
    emit cartChanged();
    setStatus(QString("Added %1 (%2x) to prescription").arg(itemName).arg(quantity));
    return true;
}

void PrescribeController::removeItem(int index) {
    if (index >= 0 && index < m_cart.size()) {
        m_cart.removeAt(index);
        emit cartChanged();
        setStatus("Item removed from prescription");
    }
}

void PrescribeController::clearCart() {
    if (!m_cart.isEmpty()) {
        m_cart.clear();
        emit cartChanged();
        setStatus("Prescription cleared");
    }
}

QByteArray PrescribeController::buildPrescriptionPayload(int doctorId, int patientId, const QString& notes) const {
    QJsonObject root;
    root["doctor_id"] = doctorId;
    root["patient_id"] = patientId;
    root["notes"] = notes.trimmed();

    QJsonArray itemsArr;
    for (const auto& itemVar : m_cart) {
        QVariantMap itemMap = itemVar.toMap();
        QJsonObject it;
        it["inventory_item_id"] = itemMap["inventory_item_id"].toInt();
        it["quantity"] = itemMap["quantity"].toInt();
        it["dosage_instructions"] = itemMap["dosage_instructions"].toString();
        itemsArr.append(it);
    }
    root["items"] = itemsArr;

    return QJsonDocument(root).toJson(QJsonDocument::Compact);
}

void PrescribeController::refreshData() {
    setBusy(true);
    setStatus("Fetching doctors and medical inventory...");

    // 1. Fetch Doctors
    QNetworkRequest docReq(QUrl(m_serverUrl + "/doctors"));
    QNetworkReply* docReply = m_netManager.get(docReq);
    connect(docReply, &QNetworkReply::finished, this, [this, docReply]() {
        docReply->deleteLater();
        if (docReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(docReply->readAll());
            if (doc.isArray()) {
                m_doctors = doc.toVariant().toList();
                emit doctorsChanged();
            }
        }
    });

    // 2. Fetch Inventory
    QNetworkRequest invReq(QUrl(m_serverUrl + "/inventory"));
    QNetworkReply* invReply = m_netManager.get(invReq);
    connect(invReply, &QNetworkReply::finished, this, [this, invReply]() {
        invReply->deleteLater();
        setBusy(false);
        if (invReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(invReply->readAll());
            if (doc.isArray()) {
                m_inventory = doc.toVariant().toList();
                emit inventoryChanged();
                setStatus("Clinic catalog refreshed successfully.");
            }
        } else {
            setStatus("Could not connect to server at " + m_serverUrl);
        }
    });
}

void PrescribeController::submitPrescription(int doctorId, int patientId, const QString& notes) {
    if (doctorId <= 0) {
        setStatus("Error: Please select a prescribing doctor.");
        return;
    }
    if (patientId <= 0) {
        setStatus("Error: Please provide a valid Patient ID.");
        return;
    }
    if (m_cart.isEmpty()) {
        setStatus("Error: Prescription must contain at least one medication.");
        return;
    }

    setBusy(true);
    setStatus("Transmitting digital prescription to clinic server...");

    QByteArray payload = buildPrescriptionPayload(doctorId, patientId, notes);

    QNetworkRequest req(QUrl(m_serverUrl + "/prescriptions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_netManager.post(req, payload);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        setBusy(false);

        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            QString code = obj.value("prescription_code").toString();
            clearCart();
            setStatus(QString("Prescription transmitted successfully! Code: %1").arg(code));
            emit prescriptionCreated(code);
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QString errStr = doc.object().value("error").toString();
            if (errStr.isEmpty()) errStr = reply->errorString();
            setStatus("Submission failed: " + errStr);
        }
    });
}

} // namespace aidans
