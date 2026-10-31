#include <QCoreApplication> // Include QCoreApplication for declarations and types
#include <QFile> // Include QFile for declarations and types
#include <QJsonDocument> // Include QJsonDocument for declarations and types
#include <QJsonObject> // Include QJsonObject for declarations and types
#include <QJsonArray> // Include QJsonArray for declarations and types
#include <iostream> // Include iostream for declarations and types
#include <cassert> // Include cassert for declarations and types
#include "prescribe_controller.hpp" // Include prescribe_controller.hpp for declarations and types

void runPrescribeTests() { // Execute clinical workflow step
    std::cout << "[TEST] Running PrescribeController Unit Tests...\n"; // Print diagnostic informational message to standard output

    QString testConfig = "test_prescribe_config.ini"; // Assign value to target variable
    if (QFile::exists(testConfig)) QFile::remove(testConfig); // Evaluate conditional expression to branch execution flow

    { // Scope delimiter
        aidans::PrescribeController ctrlCfg(testConfig); // Execute clinical workflow step
        assert(ctrlCfg.serverUrl() == "http://localhost:8080/api/v1");
        ctrlCfg.saveServerUrl("http://192.168.1.88:8080/api/v1");
    } // Close active scope block

    { // Scope delimiter
        aidans::PrescribeController ctrlReload(testConfig); // Execute clinical workflow step
        assert(ctrlReload.serverUrl() == "http://192.168.1.88:8080/api/v1");
    } // Close active scope block
    if (QFile::exists(testConfig)) QFile::remove(testConfig); // Evaluate conditional expression to branch execution flow

    aidans::PrescribeController ctrl; // Execute clinical workflow step

    // 1. Initial State
    assert(ctrl.cart().isEmpty()); // Execute clinical workflow step
    assert(!ctrl.isBusy()); // Execute clinical workflow step
    assert(ctrl.isServerOnline()); // Execute clinical workflow step

    // 2. Reject Invalid Additions
    std::cout << "[TEST] Testing validation on invalid medication additions...\n"; // Print diagnostic informational message to standard output
    bool addedBad1 = ctrl.addItem(1, "Amoxicillin", 0, "1 tab"); // quantity <= 0
    assert(!addedBad1); // Execute clinical workflow step
    assert(ctrl.cart().isEmpty()); // Execute clinical workflow step

    bool addedBad2 = ctrl.addItem(1, "Amoxicillin", 2, "   "); // empty dosage
    assert(!addedBad2); // Execute clinical workflow step
    assert(ctrl.cart().isEmpty()); // Execute clinical workflow step

    bool addedBad3 = ctrl.addItem(0, "Amoxicillin", 2, "1 tab"); // id <= 0
    assert(!addedBad3); // Execute clinical workflow step
    assert(ctrl.cart().isEmpty()); // Execute clinical workflow step

    // 3. Valid Additions
    std::cout << "[TEST] Testing valid item additions...\n"; // Print diagnostic informational message to standard output
    bool ok1 = ctrl.addItem(10, "Amoxicillin 500mg", 2, "1 cap every 8 hours"); // Assign value to target variable
    assert(ok1); // Execute clinical workflow step
    assert(ctrl.cart().size() == 1); // Assign value to target variable

    bool ok2 = ctrl.addItem(12, "Paracetamol 500mg", 1, "1 tab as needed for fever"); // Assign value to target variable
    assert(ok2); // Execute clinical workflow step
    assert(ctrl.cart().size() == 2); // Assign value to target variable

    // Verify properties of first item
    QVariantMap first = ctrl.cart()[0].toMap(); // Assign value to target variable
    assert(first["inventory_item_id"].toInt() == 10); // Assign value to target variable
    assert(first["quantity"].toInt() == 2); // Assign value to target variable
    assert(first["dosage_instructions"].toString() == "1 cap every 8 hours"); // Assign value to target variable

    // 4. Payload Generation & OpenAPI Compliance
    std::cout << "[TEST] Verifying OpenAPI-compliant JSON payload generation...\n"; // Print diagnostic informational message to standard output
    QByteArray payload = ctrl.buildPrescriptionPayload(3, 42, "Take after meals"); // Assign value to target variable
    QJsonDocument doc = QJsonDocument::fromJson(payload); // Assign value to target variable
    assert(!doc.isNull()); // Execute clinical workflow step
    assert(doc.isObject()); // Execute clinical workflow step

    QJsonObject root = doc.object(); // Assign value to target variable
    assert(root["doctor_id"].toInt() == 3); // Assign value to target variable
    assert(root["patient_id"].toInt() == 42); // Assign value to target variable
    assert(root["notes"].toString() == "Take after meals"); // Assign value to target variable
    assert(root["items"].isArray()); // Execute clinical workflow step

    QJsonArray items = root["items"].toArray(); // Assign value to target variable
    assert(items.size() == 2); // Assign value to target variable

    QJsonObject item0 = items[0].toObject(); // Assign value to target variable
    assert(item0["inventory_item_id"].toInt() == 10); // Assign value to target variable
    assert(item0["quantity"].toInt() == 2); // Assign value to target variable
    assert(item0["dosage_instructions"].toString() == "1 cap every 8 hours"); // Assign value to target variable

    // 5. Item Removal
    std::cout << "[TEST] Testing item removal from cart...\n"; // Print diagnostic informational message to standard output
    ctrl.removeItem(0); // Execute clinical workflow step
    assert(ctrl.cart().size() == 1); // Assign value to target variable
    QVariantMap remaining = ctrl.cart()[0].toMap(); // Assign value to target variable
    assert(remaining["inventory_item_id"].toInt() == 12); // Assign value to target variable

    // 6. Clear Cart
    std::cout << "[TEST] Testing clearing cart...\n"; // Print diagnostic informational message to standard output
    ctrl.clearCart(); // Execute clinical workflow step
    assert(ctrl.cart().isEmpty()); // Execute clinical workflow step

    // 7. Online Patient Appointment Selection
    std::cout << "[TEST] Testing online portal appointment selection...\n";
    QVariantList mockAppts;
    QVariantMap appt1;
    appt1["id"] = 7;
    appt1["patient_id"] = 42;
    appt1["doctor_id"] = 2;
    appt1["patient_name"] = "Erika Musterfrau";
    appt1["patient_phone"] = "+49-171-9876543";
    appt1["patient_email"] = "erika@example.de";
    appt1["appointment_time"] = "2026-10-31 10:30:00";
    appt1["doctor_name"] = "Dr. Klaus Schneider";
    appt1["reason"] = "Persistent cough and fever";
    mockAppts.append(appt1);

    ctrl.setAppointments(mockAppts);
    assert(ctrl.appointments().size() == 1);
    assert(!ctrl.selectAppointment(-1));
    assert(!ctrl.selectAppointment(5));
    assert(ctrl.selectAppointment(0));
    assert(ctrl.selectedAppointment()["patient_id"].toInt() == 42);
    assert(ctrl.selectedAppointment()["patient_name"].toString() == "Erika Musterfrau");
    assert(ctrl.selectedAppointment()["patient_email"].toString() == "erika@example.de");
    assert(ctrl.selectedAppointment()["reason"].toString() == "Persistent cough and fever");

    // 8. Prescription Receipt Rendering (Selectable RX Code & Details)
    std::cout << "[TEST] Testing prescription receipt rendering...\n";
    ctrl.addItem(10, "Amoxicillin 500mg", 2, "1 cap every 8 hours");
    QString receiptText = ctrl.renderReceipt("RX-998877-1234", 2, 42, "Persistent cough and fever");
    assert(ctrl.lastPrescriptionCode() == "RX-998877-1234");
    assert(receiptText.contains("RX-998877-1234"));
    assert(receiptText.contains("Erika Musterfrau"));
    assert(receiptText.contains("Amoxicillin 500mg"));
    assert(receiptText.contains("1 cap every 8 hours"));

    // 9. Dismiss / Resolve Patient Concern
    std::cout << "[TEST] Testing resolving/dismissing patient concern...\n";
    assert(!ctrl.resolveAppointment(-1));
    assert(ctrl.resolveAppointment(0));
    assert(ctrl.appointments().isEmpty());
    assert(ctrl.selectedAppointment().isEmpty());

    std::cout << "\n>>> ALL CLINIC_PRESCRIBE TESTS PASSED SUCCESSFULLY! <<<\n\n"; // Print diagnostic informational message to standard output
} // Close active scope block

int main(int argc, char* argv[]) { // Execute clinical workflow step
    QCoreApplication app(argc, argv); // Execute clinical workflow step
    runPrescribeTests(); // Execute clinical workflow step
    return 0; // Return computed result to caller
} // Close active scope block
