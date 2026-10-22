#pragma once

/**
 * @file prescribe_controller.hpp
 * @brief ViewModel Controller for AIDANS Doctor e-Prescription Client.
 *
 * Exposes a reactive Qt C++ interface to declarative QML UI views.
 * Responsibilities:
 *  - Loads active doctors and medical formulary inventory via REST API.
 *  - Manages the in-progress prescription item cart (medicines, quantities, dosages).
 *  - Serializes prescription requests into compliant JSON payloads.
 *  - Submits digital prescriptions (`POST /api/v1/prescriptions`) and handles RX code generation.
 *  - Provides health diagnostics and local configuration persistence (config.ini).
 */

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace aidans {

class PrescribeController : public QObject {
    Q_OBJECT

    // Reactive properties bound directly to QML UI components
    Q_PROPERTY(QVariantList doctors READ doctors NOTIFY doctorsChanged)
    Q_PROPERTY(QVariantList inventory READ inventory NOTIFY inventoryChanged)
    Q_PROPERTY(QVariantList cart READ cart NOTIFY cartChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged)
    Q_PROPERTY(bool isServerOnline READ isServerOnline NOTIFY isServerOnlineChanged)
    Q_PROPERTY(QString serverUrl READ serverUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)

public:
    /**
     * @brief Constructs controller, loads INI settings, and checks backend health.
     * @param configPath Local config.ini path.
     * @param parent Qt parent object hierarchy pointer.
     */
    explicit PrescribeController(const QString& configPath = "config.ini", QObject* parent = nullptr);
    ~PrescribeController() override = default;

    // Property Getters
    QVariantList doctors() const { return m_doctors; }
    QVariantList inventory() const { return m_inventory; }
    QVariantList cart() const { return m_cart; }
    QString statusMessage() const { return m_statusMessage; }
    bool isBusy() const { return m_isBusy; }
    bool isServerOnline() const { return m_isServerOnline; }
    QString serverUrl() const { return m_serverUrl; }
    QString connectionStatus() const { return m_connectionStatus; }

    /**
     * @brief Reads server URL from config.ini.
     */
    Q_INVOKABLE void loadConfig();

    /**
     * @brief Persists server URL to config.ini.
     */
    Q_INVOKABLE void saveServerUrl(const QString& url);

    /**
     * @brief Probes a candidate server URL to verify reachability.
     */
    Q_INVOKABLE void testConnection(const QString& targetUrl);

    /**
     * @brief Dynamically updates active server base URL.
     */
    Q_INVOKABLE void setServerUrl(const QString& url);

    /**
     * @brief Refreshes doctor directory and medical inventory from backend.
     */
    Q_INVOKABLE void refreshData();

    /**
     * @brief Adds a prescribed medication line to the draft prescription.
     * @param inventoryId Inventory item ID.
     * @param itemName Name of medication.
     * @param quantity Number of units prescribed.
     * @param dosage Clinical dosage directions (e.g. "Take 1 pill bid after food").
     * @return true if added successfully.
     */
    Q_INVOKABLE bool addItem(int inventoryId, const QString& itemName, int quantity, const QString& dosage);

    /**
     * @brief Removes a medication line item from the draft prescription.
     * @param index Zero-based item index.
     */
    Q_INVOKABLE void removeItem(int index);

    /**
     * @brief Clears all medication items from the current draft.
     */
    Q_INVOKABLE void clearCart();

    /**
     * @brief Submits the completed prescription to the backend server.
     * @param doctorId Prescribing physician ID.
     * @param patientId Patient record ID.
     * @param notes Clinical notes and diagnosis summary.
     */
    Q_INVOKABLE void submitPrescription(int doctorId, int patientId, const QString& notes);

    /**
     * @brief Helper to serialize prescription payload to JSON byte array (used in network & tests).
     */
    QByteArray buildPrescriptionPayload(int doctorId, int patientId, const QString& notes) const;

signals:
    void doctorsChanged();
    void inventoryChanged();
    void cartChanged();
    void statusMessageChanged();
    void isBusyChanged();
    void isServerOnlineChanged();
    void serverUrlChanged();
    void connectionStatusChanged();
    void prescriptionCreated(const QString& prescriptionCode);
    void errorOccurred(const QString& errorMsg);

private:
    QString m_configPath{"config.ini"};
    QString m_serverUrl{"http://localhost:8080"};

    QVariantList m_doctors;
    QVariantList m_inventory;
    QVariantList m_cart;
    QString m_statusMessage{"Ready"};
    bool m_isBusy{false};
    bool m_isServerOnline{false};
    QString m_connectionStatus{"Checking..."};

    QNetworkAccessManager* m_netManager{nullptr};

    void checkServerHealth();
    void fetchDoctors();
    void fetchInventory();
};

} // namespace aidans
