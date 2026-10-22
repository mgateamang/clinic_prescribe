#pragma once // Ensure header is included only once per translation unit

#include <QObject> // Include base Qt object class
#include <QString> // Include Qt Unicode string class
#include <QVariantList> // Include Qt variant list collection
#include <QVariantMap> // Include Qt variant map dictionary
#include <QNetworkAccessManager> // Include Qt network access coordinator
#include <QNetworkReply> // Include Qt network reply handler

namespace aidans { // Enclosing clinical suite namespace

/**
 * @class PrescribeController
 * @brief ViewModel controller for physician e-prescription authoring and submission.
 */
class PrescribeController : public QObject { // Inherit QObject for meta-object system
    Q_OBJECT // Macro enabling signals, slots, and properties

    Q_PROPERTY(QVariantList doctors READ doctors NOTIFY doctorsChanged) // Expose doctor roster to QML
    Q_PROPERTY(QVariantList inventory READ inventory NOTIFY inventoryChanged) // Expose formulary inventory items
    Q_PROPERTY(QVariantList cart READ cart NOTIFY cartChanged) // Expose in-progress prescription medication items
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged) // Expose user status message
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY isBusyChanged) // Expose network busy state flag
    Q_PROPERTY(bool isServerOnline READ isServerOnline NOTIFY isServerOnlineChanged) // Expose server connectivity flag
    Q_PROPERTY(QString serverUrl READ serverUrl NOTIFY serverUrlChanged) // Expose active REST API server URL
    Q_PROPERTY(QString connectionStatus READ connectionStatus NOTIFY connectionStatusChanged) // Expose diagnostic text

public: // Public interface methods
    explicit PrescribeController(const QString& configPath = "config.ini", QObject* parent = nullptr); // Constructor
    ~PrescribeController() override = default; // Destructor

    QVariantList doctors() const { return m_doctors; } // Getter for doctor roster
    QVariantList inventory() const { return m_inventory; } // Getter for medical formulary items
    QVariantList cart() const { return m_cart; } // Getter for current prescription medications
    QString statusMessage() const { return m_statusMessage; } // Getter for status message text
    bool isBusy() const { return m_isBusy; } // Getter for network busy flag
    bool isServerOnline() const { return m_isServerOnline; } // Getter for server availability flag
    QString serverUrl() const { return m_serverUrl; } // Getter for active backend server URL
    QString connectionStatus() const { return m_connectionStatus; } // Getter for diagnostic connection text

    Q_INVOKABLE void loadConfig(); // Load INI configuration from disk
    Q_INVOKABLE void saveServerUrl(const QString& url); // Persist updated server URL
    Q_INVOKABLE void testConnection(const QString& targetUrl); // Test server connectivity asynchronously
    Q_INVOKABLE void setServerUrl(const QString& url); // Set server URL dynamically
    Q_INVOKABLE void refreshData(); // Refresh doctor directory and inventory items
    Q_INVOKABLE bool addItem(int inventoryId, const QString& itemName, int quantity, const QString& dosage); // Add medication
    Q_INVOKABLE void removeItem(int index); // Remove medication line by index
    Q_INVOKABLE void clearCart(); // Clear all medication lines from draft
    Q_INVOKABLE void submitPrescription(int doctorId, int patientId, const QString& notes); // Submit digital prescription

    // Helper for payload generation (used both in network submit and tests)
    QByteArray buildPrescriptionPayload(int doctorId, int patientId, const QString& notes) const; // Build JSON prescription payload

signals: // Qt notification signals
    void doctorsChanged(); // Emitted when doctor roster updates
    void inventoryChanged(); // Emitted when formulary inventory updates
    void cartChanged(); // Emitted when prescription line items update
    void statusMessageChanged(); // Emitted when status message updates
    void isBusyChanged(); // Emitted when busy state toggles
    void isServerOnlineChanged(); // Emitted when server connectivity changes
    void serverUrlChanged(); // Emitted when server URL changes
    void connectionStatusChanged(); // Emitted when connection diagnostic text updates
    void prescriptionCreated(const QString& prescriptionCode); // Emitted when prescription is successfully created

private: // Private member state and helpers
    QString m_configPath{"config.ini"}; // Path to configuration file
    QString m_serverUrl{"http://localhost:8080/api/v1"}; // Active REST API server URL
    QString m_connectionStatus; // Detailed connection diagnostic text
    bool m_isServerOnline{true}; // Server availability status flag
    QVariantList m_doctors; // Cached list of doctors
    QVariantList m_inventory; // Cached list of formulary inventory items
    QVariantList m_cart; // Active prescription line items
    QString m_statusMessage; // Active status message text
    bool m_isBusy{false}; // Busy network flag
    QNetworkAccessManager m_netManager; // Network manager instance

    void setStatus(const QString& msg); // Set status message helper
    void setBusy(bool busy); // Set busy state helper
    void setServerOnline(bool online); // Set server availability helper
    void setConnectionStatus(const QString& status); // Set diagnostic connection text helper
}; // End of PrescribeController class definition

} // namespace aidans
