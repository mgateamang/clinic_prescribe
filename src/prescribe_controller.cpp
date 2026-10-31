/**
 * @file prescribe_controller.cpp
 * @brief Implementation of PrescribeController for Qt6 / QML e-Prescription client.
 *
 * Implements physician doctor lists, inventory lookup, dynamic prescription assembly,
 * JSON serialization, and submission to /api/v1/prescriptions.
 */

#include "prescribe_controller.hpp" // Include prescribe_controller.hpp for declarations and types
#include <QSettings> // Include QSettings for declarations and types
#include <QFileInfo> // Include QFileInfo for declarations and types
#include <QJsonDocument> // Include QJsonDocument for declarations and types
#include <QJsonObject> // Include QJsonObject for declarations and types
#include <QJsonArray> // Include QJsonArray for declarations and types
#include <QUrl> // Include QUrl for declarations and types
#include <QUrlQuery> // Include QUrlQuery for declarations and types

namespace aidans { // Execute clinical workflow step

/**
 * @brief Constructs the controller and reads local config.ini.
 */
PrescribeController::PrescribeController(const QString& configPath, QObject* parent) // Execute clinical workflow step
    : QObject(parent), m_configPath(configPath) { // Execute clinical workflow step
    loadConfig(); // Execute clinical workflow step
} // Close active scope block

void PrescribeController::loadConfig() { // Execute clinical workflow step
    if (QFileInfo::exists(m_configPath)) { // Evaluate conditional expression to branch execution flow
        QSettings settings(m_configPath, QSettings::IniFormat); // Execute clinical workflow step
        settings.beginGroup("Server"); // Execute clinical workflow step
        QString url = settings.value("url", m_serverUrl).toString(); // Assign value to target variable
        settings.endGroup(); // Execute clinical workflow step
        if (!url.isEmpty()) { // Evaluate conditional expression to branch execution flow
            m_serverUrl = url; // Assign value to target variable
            emit serverUrlChanged(); // Notify QML UI bindings of underlying property state update
        } // Close active scope block
    } // Close active scope block
} // Close active scope block

void PrescribeController::saveServerUrl(const QString& url) { // Execute clinical workflow step
    QString trimmed = url.trimmed(); // Assign value to target variable
    if (trimmed.isEmpty()) return; // Evaluate conditional expression to branch execution flow

    m_serverUrl = trimmed; // Assign value to target variable
    emit serverUrlChanged(); // Notify QML UI bindings of underlying property state update

    QSettings settings(m_configPath, QSettings::IniFormat); // Execute clinical workflow step
    settings.beginGroup("Server"); // Execute clinical workflow step
    settings.setValue("url", m_serverUrl); // Execute clinical workflow step
    settings.endGroup(); // Execute clinical workflow step
    settings.sync(); // Execute clinical workflow step

    setStatus("Server URL saved to " + m_configPath); // Execute clinical workflow step
} // Close active scope block

void PrescribeController::setServerUrl(const QString& url) { // Execute clinical workflow step
    m_serverUrl = url; // Assign value to target variable
    emit serverUrlChanged(); // Notify QML UI bindings of underlying property state update
} // Close active scope block

void PrescribeController::setStatus(const QString& msg) { // Execute clinical workflow step
    if (m_statusMessage != msg) { // Evaluate conditional expression to branch execution flow
        m_statusMessage = msg; // Assign value to target variable
        emit statusMessageChanged(); // Notify QML UI bindings of underlying property state update
    } // Close active scope block
} // Close active scope block

void PrescribeController::setBusy(bool busy) { // Execute clinical workflow step
    if (m_isBusy != busy) { // Evaluate conditional expression to branch execution flow
        m_isBusy = busy; // Assign value to target variable
        emit isBusyChanged(); // Notify QML UI bindings of underlying property state update
    } // Close active scope block
} // Close active scope block

void PrescribeController::setServerOnline(bool online) { // Execute clinical workflow step
    if (m_isServerOnline != online) { // Evaluate conditional expression to branch execution flow
        m_isServerOnline = online; // Assign value to target variable
        emit isServerOnlineChanged(); // Notify QML UI bindings of underlying property state update
    } // Close active scope block
} // Close active scope block

void PrescribeController::setConnectionStatus(const QString& status) { // Execute clinical workflow step
    if (m_connectionStatus != status) { // Evaluate conditional expression to branch execution flow
        m_connectionStatus = status; // Assign value to target variable
        emit connectionStatusChanged(); // Notify QML UI bindings of underlying property state update
    } // Close active scope block
} // Close active scope block

void PrescribeController::testConnection(const QString& targetUrl) { // Execute clinical workflow step
    QString urlStr = targetUrl.trimmed(); // Assign value to target variable
    if (urlStr.isEmpty()) urlStr = m_serverUrl; // Evaluate conditional expression to branch execution flow

    setBusy(true); // Execute clinical workflow step
    setConnectionStatus("Pinging " + urlStr + "..."); // Execute clinical workflow step

    QString healthUrl = urlStr; // Assign value to target variable
    if (!healthUrl.endsWith("/health") && !healthUrl.endsWith("/api/v1/health")) { // Evaluate conditional expression to branch execution flow
        healthUrl = urlStr + "/health"; // Assign value to target variable
    } // Close active scope block

    QUrl target(healthUrl); // Execute clinical workflow step
    QNetworkRequest req(target); // Construct HTTP request specification with target URL and headers
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // Construct HTTP request specification with target URL and headers

    QNetworkReply* reply = m_netManager.get(req); // Assign value to target variable
    connect(reply, &QNetworkReply::finished, this, [this, reply, urlStr]() { // Connect Qt signal to asynchronous member slot handler
        reply->deleteLater(); // Schedule network reply object for safe asynchronous deletion
        setBusy(false); // Execute clinical workflow step

        if (reply->error() == QNetworkReply::NoError) { // Evaluate conditional expression to branch execution flow
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll()); // Read incoming response byte stream into buffer
            QString service = doc.object().value("service").toString(); // Assign value to target variable
            QString version = doc.object().value("version").toString(); // Assign value to target variable
            setConnectionStatus(QString("Connected to %1 (%2 v%3)").arg(urlStr).arg(service).arg(version)); // Execute clinical workflow step
            setServerOnline(true); // Execute clinical workflow step
        } else { // Execute default fallback branch
            setConnectionStatus("Failed to connect: " + reply->errorString()); // Execute clinical workflow step
            setServerOnline(false); // Execute clinical workflow step
        } // Close active scope block
    }); // Execute clinical workflow step
} // Close active scope block

bool PrescribeController::addItem(int inventoryId, const QString& itemName, int quantity, const QString& dosage) { // Execute clinical workflow step
    if (inventoryId <= 0 || quantity <= 0 || dosage.trimmed().isEmpty()) { // Evaluate conditional expression to branch execution flow
        setStatus("Error: Invalid medication, quantity must be > 0 and dosage specified."); // Execute clinical workflow step
        return false; // Return computed result to caller
    } // Close active scope block

    QVariantMap item; // Execute clinical workflow step
    item["inventory_item_id"] = inventoryId; // Assign value to target variable
    item["item_name"] = itemName; // Assign value to target variable
    item["quantity"] = quantity; // Assign value to target variable
    item["dosage_instructions"] = dosage.trimmed(); // Assign value to target variable

    m_cart.append(item); // Execute clinical workflow step
    emit cartChanged(); // Notify QML UI bindings of underlying property state update
    setStatus(QString("Added %1 (%2x) to prescription").arg(itemName).arg(quantity)); // Execute clinical workflow step
    return true; // Return computed result to caller
} // Close active scope block

void PrescribeController::removeItem(int index) { // Execute clinical workflow step
    if (index >= 0 && index < m_cart.size()) { // Evaluate conditional expression to branch execution flow
        m_cart.removeAt(index); // Execute clinical workflow step
        emit cartChanged(); // Notify QML UI bindings of underlying property state update
        setStatus("Item removed from prescription"); // Execute clinical workflow step
    } // Close active scope block
} // Close active scope block

void PrescribeController::clearCart() { // Execute clinical workflow step
    if (!m_cart.isEmpty()) { // Evaluate conditional expression to branch execution flow
        m_cart.clear(); // Execute clinical workflow step
        emit cartChanged(); // Notify QML UI bindings of underlying property state update
        setStatus("Prescription cleared"); // Execute clinical workflow step
    } // Close active scope block
} // Close active scope block

QByteArray PrescribeController::buildPrescriptionPayload(int doctorId, int patientId, const QString& notes) const { // Execute clinical workflow step
    QJsonObject root; // Execute clinical workflow step
    root["doctor_id"] = doctorId; // Assign value to target variable
    root["patient_id"] = patientId; // Assign value to target variable
    root["notes"] = notes.trimmed(); // Assign value to target variable

    QJsonArray itemsArr; // Execute clinical workflow step
    for (const auto& itemVar : m_cart) { // Iterate through items in sequence or collection
        QVariantMap itemMap = itemVar.toMap(); // Assign value to target variable
        QJsonObject it; // Execute clinical workflow step
        it["inventory_item_id"] = itemMap["inventory_item_id"].toInt(); // Assign value to target variable
        it["quantity"] = itemMap["quantity"].toInt(); // Assign value to target variable
        it["dosage_instructions"] = itemMap["dosage_instructions"].toString(); // Assign value to target variable
        itemsArr.append(it); // Execute clinical workflow step
    } // Close active scope block
    root["items"] = itemsArr; // Assign value to target variable

    return QJsonDocument(root).toJson(QJsonDocument::Compact); // Return computed result to caller
} // Close active scope block

void PrescribeController::refreshData() { // Execute clinical workflow step
    setBusy(true); // Execute clinical workflow step
    setStatus("Fetching doctors and medical inventory..."); // Execute clinical workflow step

    // 1. Fetch Doctors
    QNetworkRequest docReq(QUrl(m_serverUrl + "/doctors")); // Construct HTTP request specification with target URL and headers
    QNetworkReply* docReply = m_netManager.get(docReq); // Assign value to target variable
    connect(docReply, &QNetworkReply::finished, this, [this, docReply]() { // Connect Qt signal to asynchronous member slot handler
        docReply->deleteLater(); // Execute clinical workflow step
        if (docReply->error() == QNetworkReply::NoError) { // Evaluate conditional expression to branch execution flow
            QJsonDocument doc = QJsonDocument::fromJson(docReply->readAll()); // Assign value to target variable
            if (doc.isArray()) { // Evaluate conditional expression to branch execution flow
                m_doctors = doc.toVariant().toList(); // Assign value to target variable
                emit doctorsChanged(); // Notify QML UI bindings of underlying property state update
                setServerOnline(true); // Execute clinical workflow step
            } // Close active scope block
        } else { // Execute default fallback branch
            setServerOnline(false); // Execute clinical workflow step
            setStatus("Could not connect to clinic server: " + docReply->errorString()); // Execute clinical workflow step
        } // Close active scope block
    }); // Execute clinical workflow step

    // 2. Fetch Inventory
    QNetworkRequest invReq(QUrl(m_serverUrl + "/inventory")); // Construct HTTP request specification with target URL and headers
    QNetworkReply* invReply = m_netManager.get(invReq); // Assign value to target variable
    connect(invReply, &QNetworkReply::finished, this, [this, invReply]() { // Connect Qt signal to asynchronous member slot handler
        invReply->deleteLater(); // Execute clinical workflow step
        setBusy(false); // Execute clinical workflow step
        if (invReply->error() == QNetworkReply::NoError) { // Evaluate conditional expression to branch execution flow
            QJsonDocument doc = QJsonDocument::fromJson(invReply->readAll()); // Assign value to target variable
            if (doc.isArray()) { // Evaluate conditional expression to branch execution flow
                m_inventory = doc.toVariant().toList(); // Assign value to target variable
                emit inventoryChanged(); // Notify QML UI bindings of underlying property state update
                setServerOnline(true); // Execute clinical workflow step
                setStatus("Clinic catalog refreshed successfully."); // Execute clinical workflow step
            } // Close active scope block
        } else { // Execute default fallback branch
            setServerOnline(false); // Execute clinical workflow step
            setStatus("Could not connect to server at " + m_serverUrl); // Execute clinical workflow step
        } // Close active scope block
    }); // Execute clinical workflow step

    // 3. Fetch Online Portal Appointments
    QNetworkRequest apptReq(QUrl(m_serverUrl + "/appointments"));
    QNetworkReply* apptReply = m_netManager.get(apptReq);
    connect(apptReply, &QNetworkReply::finished, this, [this, apptReply]() {
        apptReply->deleteLater();
        if (apptReply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(apptReply->readAll());
            if (doc.isArray()) {
                QVariantList raw = doc.toVariant().toList();
                QVariantList active;
                for (const auto& v : raw) {
                    QString st = v.toMap().value("status").toString();
                    if (st != "COMPLETED" && st != "CANCELLED") {
                        active.append(v);
                    }
                }
                m_appointments = active;
                emit appointmentsChanged();
            }
        }
    });
} // Close active scope block

void PrescribeController::setAppointments(const QVariantList& list) {
    m_appointments = list;
    emit appointmentsChanged();
}

bool PrescribeController::selectAppointment(int index) {
    if (index < 0 || index >= m_appointments.size()) {
        setStatus("Error: Invalid appointment selection.");
        return false;
    }
    m_selectedAppointment = m_appointments[index].toMap();
    emit selectedAppointmentChanged();

    int docId = m_selectedAppointment.value("doctor_id").toInt();
    int patId = m_selectedAppointment.value("patient_id").toInt();
    QString patName = m_selectedAppointment.value("patient_name").toString();
    QString reason = m_selectedAppointment.value("reason").toString();

    setStatus(QString("Selected patient %1 (ID #%2) from portal submission.").arg(patName).arg(patId));
    emit appointmentSelected(docId, patId, patName, reason);
    return true;
}

bool PrescribeController::resolveAppointment(int index) {
    if (index < 0 || index >= m_appointments.size()) {
        setStatus("Error: Invalid appointment to resolve.");
        return false;
    }
    QVariantMap appt = m_appointments[index].toMap();
    int apptId = appt.value("id").toInt();
    QString patName = appt.value("patient_name").toString();

    m_appointments.removeAt(index);
    emit appointmentsChanged();

    if (m_selectedAppointment.value("id").toInt() == apptId) {
        m_selectedAppointment.clear();
        emit selectedAppointmentChanged();
    }

    setStatus(QString("Resolved and dismissed concern for %1.").arg(patName));

    if (apptId > 0) {
        QNetworkRequest req(QUrl(QString("%1/appointments/%2/resolve").arg(m_serverUrl).arg(apptId)));
        req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QNetworkReply* reply = m_netManager.post(req, QByteArray("{}"));
        connect(reply, &QNetworkReply::finished, this, [reply]() {
            reply->deleteLater();
        });
    }
    return true;
}

QString PrescribeController::formatPrescriptionReceipt(const QString& rxCode, const QString& doctorName, int doctorId,
                                                       const QString& patientName, int patientId, const QString& notes,
                                                       const QVariantList& items) {
    QString out;
    out += "================================================\n";
    out += "       AIDANS DIGITAL PRESCRIPTION RECEIPT      \n";
    out += "================================================\n";
    out += QString("RX CODE    : %1\n").arg(rxCode.isEmpty() ? "DRAFT (UNTRANSMITTED)" : rxCode);
    out += QString("DOCTOR     : %1 (ID #%2)\n").arg(doctorName.isEmpty() ? "Physician" : doctorName).arg(doctorId);
    out += QString("PATIENT    : %1 (ID #%2)\n").arg(patientName.isEmpty() ? "Patient" : patientName).arg(patientId);
    if (!notes.trimmed().isEmpty()) {
        out += QString("NOTES      : %1\n").arg(notes.trimmed());
    }
    out += "------------------------------------------------\n";
    out += "PRESCRIBED MEDICATIONS:\n";
    int idx = 1;
    for (const auto& itVar : items) {
        QVariantMap it = itVar.toMap();
        out += QString("  %1. %2 (Qty: %3)\n     Dosage: %4\n")
                   .arg(idx++)
                   .arg(it.value("item_name").toString())
                   .arg(it.value("quantity").toInt())
                   .arg(it.value("dosage_instructions").toString());
    }
    out += "================================================\n";
    return out;
}

QString PrescribeController::renderReceipt(const QString& rxCode, int doctorId, int patientId, const QString& notes) {
    if (m_cart.isEmpty() && !m_lastReceiptText.isEmpty() && (rxCode.isEmpty() || rxCode == m_lastPrescriptionCode)) {
        return m_lastReceiptText;
    }
    QString docName;
    for (const auto& d : m_doctors) {
        if (d.toMap().value("id").toInt() == doctorId) {
            docName = d.toMap().value("name").toString();
            break;
        }
    }
    QString patName = m_selectedAppointment.value("patient_id").toInt() == patientId
                          ? m_selectedAppointment.value("patient_name").toString()
                          : QString();
    QString codeToUse = rxCode.isEmpty() ? m_lastPrescriptionCode : rxCode;
    m_lastPrescriptionCode = codeToUse;
    m_lastReceiptText = formatPrescriptionReceipt(codeToUse, docName, doctorId, patName, patientId, notes, m_cart);
    emit lastPrescriptionChanged();
    return m_lastReceiptText;
}

void PrescribeController::submitPrescription(int doctorId, int patientId, const QString& notes) { // Execute clinical workflow step
    if (doctorId <= 0) { // Evaluate conditional expression to branch execution flow
        setStatus("Error: Please select a prescribing doctor."); // Execute clinical workflow step
        return; // Exit current function execution scope
    } // Close active scope block
    if (patientId <= 0) { // Evaluate conditional expression to branch execution flow
        setStatus("Error: Please provide a valid Patient ID."); // Execute clinical workflow step
        return; // Exit current function execution scope
    } // Close active scope block
    if (m_cart.isEmpty()) { // Evaluate conditional expression to branch execution flow
        setStatus("Error: Prescription must contain at least one medication."); // Execute clinical workflow step
        return; // Exit current function execution scope
    } // Close active scope block

    setBusy(true); // Execute clinical workflow step
    setStatus("Transmitting digital prescription to clinic server..."); // Execute clinical workflow step

    QVariantList snapshotItems = m_cart;
    QByteArray payload = buildPrescriptionPayload(doctorId, patientId, notes); // Assign value to target variable

    QNetworkRequest req(QUrl(m_serverUrl + "/prescriptions")); // Construct HTTP request specification with target URL and headers
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json"); // Construct HTTP request specification with target URL and headers

    QNetworkReply* reply = m_netManager.post(req, payload); // Assign value to target variable
    connect(reply, &QNetworkReply::finished, this, [this, reply, doctorId, patientId, notes, snapshotItems]() { // Connect Qt signal to asynchronous member slot handler
        reply->deleteLater(); // Schedule network reply object for safe asynchronous deletion
        setBusy(false); // Execute clinical workflow step

        if (reply->error() == QNetworkReply::NoError) { // Evaluate conditional expression to branch execution flow
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll()); // Read incoming response byte stream into buffer
            QJsonObject obj = doc.object(); // Assign value to target variable
            QString code = obj.value("prescription_code").toString(); // Assign value to target variable
            QString docName = obj.value("doctor_name").toString();
            QString patName = obj.value("patient_name").toString();
            m_lastPrescriptionCode = code;
            m_lastReceiptText = formatPrescriptionReceipt(code, docName, doctorId, patName, patientId, notes, snapshotItems);
            emit lastPrescriptionChanged();
            clearCart(); // Execute clinical workflow step
            setServerOnline(true); // Execute clinical workflow step
            setStatus(QString("Prescription transmitted successfully! Code: %1").arg(code)); // Execute clinical workflow step
            emit prescriptionCreated(code); // Notify QML UI bindings of underlying property state update
        } else { // Execute default fallback branch
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll()); // Read incoming response byte stream into buffer
            QString errStr = doc.object().value("error").toString(); // Assign value to target variable
            if (errStr.isEmpty()) errStr = reply->errorString(); // Evaluate conditional expression to branch execution flow
            if (reply->error() == QNetworkReply::ConnectionRefusedError || // Evaluate conditional expression to branch execution flow
                reply->error() == QNetworkReply::HostNotFoundError || // Assign value to target variable
                reply->error() == QNetworkReply::TimeoutError) { // Assign value to target variable
                setServerOnline(false); // Execute clinical workflow step
            } // Close active scope block
            setStatus("Submission failed: " + errStr); // Execute clinical workflow step
        } // Close active scope block
    }); // Execute clinical workflow step
} // Close active scope block

} // namespace aidans
