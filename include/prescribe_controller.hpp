#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace aidans {

class PrescribeController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QVariantList doctors READ doctors NOTIFY doctorsChanged)
    Q_PROPERTY(QVariantList inventory READ inventory NOTIFY inventoryChanged)
    Q_PROPERTY(QVariantList cart READ cart NOTIFY cartChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)

public:
    explicit PrescribeController(QObject* parent = nullptr);
    ~PrescribeController() override = default;

    QVariantList doctors() const { return m_doctors; }
    QVariantList inventory() const { return m_inventory; }
    QVariantList cart() const { return m_cart; }
    QString statusMessage() const { return m_statusMessage; }
    bool isBusy() const { return m_isBusy; }

    Q_INVOKABLE void setServerUrl(const QString& url);
    Q_INVOKABLE void refreshData();
    Q_INVOKABLE bool addItem(int inventoryId, const QString& itemName, int quantity, const QString& dosage);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void clearCart();
    Q_INVOKABLE void submitPrescription(int doctorId, int patientId, const QString& notes);

    // Helper for payload generation (used both in network submit and tests)
    QByteArray buildPrescriptionPayload(int doctorId, int patientId, const QString& notes) const;

signals:
    void doctorsChanged();
    void inventoryChanged();
    void cartChanged();
    void statusMessageChanged();
    void isBusyChanged();
    void prescriptionCreated(const QString& prescriptionCode);

private:
    QString m_serverUrl{"http://localhost:8080/api/v1"};
    QVariantList m_doctors;
    QVariantList m_inventory;
    QVariantList m_cart;
    QString m_statusMessage;
    bool m_isBusy{false};
    QNetworkAccessManager m_netManager;

    void setStatus(const QString& msg);
    void setBusy(bool busy);
};

} // namespace aidans
