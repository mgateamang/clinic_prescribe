#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <cassert>
#include "prescribe_controller.hpp"

void runPrescribeTests() {
    std::cout << "[TEST] Running PrescribeController Unit Tests...\n";

    QString testConfig = "test_prescribe_config.ini";
    if (QFile::exists(testConfig)) QFile::remove(testConfig);

    {
        aidans::PrescribeController ctrlCfg(testConfig);
        assert(ctrlCfg.serverUrl() == "http://localhost:8080/api/v1");
        ctrlCfg.saveServerUrl("http://192.168.1.88:8080/api/v1");
    }

    {
        aidans::PrescribeController ctrlReload(testConfig);
        assert(ctrlReload.serverUrl() == "http://192.168.1.88:8080/api/v1");
    }
    if (QFile::exists(testConfig)) QFile::remove(testConfig);

    aidans::PrescribeController ctrl;

    // 1. Initial State
    assert(ctrl.cart().isEmpty());
    assert(!ctrl.isBusy());
    assert(ctrl.isServerOnline());

    // 2. Reject Invalid Additions
    std::cout << "[TEST] Testing validation on invalid medication additions...\n";
    bool addedBad1 = ctrl.addItem(1, "Amoxicillin", 0, "1 tab"); // quantity <= 0
    assert(!addedBad1);
    assert(ctrl.cart().isEmpty());

    bool addedBad2 = ctrl.addItem(1, "Amoxicillin", 2, "   "); // empty dosage
    assert(!addedBad2);
    assert(ctrl.cart().isEmpty());

    bool addedBad3 = ctrl.addItem(0, "Amoxicillin", 2, "1 tab"); // id <= 0
    assert(!addedBad3);
    assert(ctrl.cart().isEmpty());

    // 3. Valid Additions
    std::cout << "[TEST] Testing valid item additions...\n";
    bool ok1 = ctrl.addItem(10, "Amoxicillin 500mg", 2, "1 cap every 8 hours");
    assert(ok1);
    assert(ctrl.cart().size() == 1);

    bool ok2 = ctrl.addItem(12, "Paracetamol 500mg", 1, "1 tab as needed for fever");
    assert(ok2);
    assert(ctrl.cart().size() == 2);

    // Verify properties of first item
    QVariantMap first = ctrl.cart()[0].toMap();
    assert(first["inventory_item_id"].toInt() == 10);
    assert(first["quantity"].toInt() == 2);
    assert(first["dosage_instructions"].toString() == "1 cap every 8 hours");

    // 4. Payload Generation & OpenAPI Compliance
    std::cout << "[TEST] Verifying OpenAPI-compliant JSON payload generation...\n";
    QByteArray payload = ctrl.buildPrescriptionPayload(3, 42, "Take after meals");
    QJsonDocument doc = QJsonDocument::fromJson(payload);
    assert(!doc.isNull());
    assert(doc.isObject());

    QJsonObject root = doc.object();
    assert(root["doctor_id"].toInt() == 3);
    assert(root["patient_id"].toInt() == 42);
    assert(root["notes"].toString() == "Take after meals");
    assert(root["items"].isArray());

    QJsonArray items = root["items"].toArray();
    assert(items.size() == 2);

    QJsonObject item0 = items[0].toObject();
    assert(item0["inventory_item_id"].toInt() == 10);
    assert(item0["quantity"].toInt() == 2);
    assert(item0["dosage_instructions"].toString() == "1 cap every 8 hours");

    // 5. Item Removal
    std::cout << "[TEST] Testing item removal from cart...\n";
    ctrl.removeItem(0);
    assert(ctrl.cart().size() == 1);
    QVariantMap remaining = ctrl.cart()[0].toMap();
    assert(remaining["inventory_item_id"].toInt() == 12);

    // 6. Clear Cart
    std::cout << "[TEST] Testing clearing cart...\n";
    ctrl.clearCart();
    assert(ctrl.cart().isEmpty());

    std::cout << "\n>>> ALL CLINIC_PRESCRIBE TESTS PASSED SUCCESSFULLY! <<<\n\n";
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    runPrescribeTests();
    return 0;
}
