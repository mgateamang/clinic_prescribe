import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 780
    height: 640
    title: "AIDANS - Digital Prescription Module (Doctor / Nurse)"

    ColumnLayout {
        anchors.fill: parent
        anchors.margin: 16
        spacing: 12

        // Header
        Label {
            text: "Doctor e-Prescription Portal"
            font.pixelSize: 20
            font.bold: true
        }

        // Doctor & Patient info
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Label { text: "Doctor:" }
            ComboBox {
                id: doctorCombo
                Layout.fillWidth: true
                model: controller.doctors
                textRole: "name"
            }

            Label { text: "Patient ID:" }
            TextField {
                id: patientIdInput
                placeholderText: "e.g. 1"
                validator: IntValidator { bottom: 1 }
                Layout.preferredWidth: 90
            }

            Button {
                text: "Refresh Data"
                onClicked: controller.refreshData()
            }
        }

        TextField {
            id: notesInput
            Layout.fillWidth: true
            placeholderText: "Clinical consultation notes / instructions..."
        }

        // Add medication box
        GroupBox {
            title: "Add Medication from Inventory"
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 8

                ComboBox {
                    id: medCombo
                    Layout.fillWidth: true
                    model: controller.inventory
                    textRole: "name"
                }

                Label { text: "Qty:" }
                SpinBox {
                    id: qtySpin
                    from: 1
                    to: 50
                    value: 1
                    Layout.preferredWidth: 90
                }

                TextField {
                    id: dosageInput
                    placeholderText: "Dosage (e.g. 1 tab 3x daily)"
                    Layout.preferredWidth: 200
                }

                Button {
                    text: "+ Add"
                    highlighted: true
                    onClicked: {
                        if (medCombo.currentIndex >= 0 && dosageInput.text.trim().length > 0) {
                            var item = controller.inventory[medCombo.currentIndex];
                            controller.addItem(item.id, item.name, qtySpin.value, dosageInput.text);
                            dosageInput.text = "";
                            qtySpin.value = 1;
                        }
                    }
                }
            }
        }

        // Items list
        Label {
            text: "Prescription Items (" + controller.cart.length + "):"
            font.bold: true
        }

        ListView {
            id: cartList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: controller.cart
            delegate: Rectangle {
                width: cartList.width
                height: 44
                color: index % 2 === 0 ? "#f8fafc" : "#ffffff"
                border.color: "#e2e8f0"
                radius: 4

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 12

                    Label {
                        text: modelData.item_name
                        font.bold: true
                        Layout.preferredWidth: 220
                        elide: Text.ElideRight
                    }

                    Label {
                        text: "Qty: " + modelData.quantity
                        Layout.preferredWidth: 60
                    }

                    Label {
                        text: modelData.dosage_instructions
                        Layout.fillWidth: true
                        color: "#475569"
                        elide: Text.ElideRight
                    }

                    Button {
                        text: "Remove"
                        onClicked: controller.removeItem(index)
                    }
                }
            }
        }

        // Action row
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            Button {
                text: "Clear"
                onClicked: controller.clearCart()
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Transmit Prescription to Pharmacy"
                highlighted: true
                enabled: !controller.isBusy && controller.cart.length > 0 && patientIdInput.text.length > 0
                onClicked: {
                    var docId = doctorCombo.currentIndex >= 0 ? controller.doctors[doctorCombo.currentIndex].id : 1;
                    var patId = parseInt(patientIdInput.text, 10);
                    controller.submitPrescription(docId, patId, notesInput.text);
                }
            }
        }

        // Status bar
        Rectangle {
            Layout.fillWidth: true
            height: 32
            color: "#f1f5f9"
            border.color: "#cbd5e1"
            radius: 4

            Label {
                anchors.centerIn: parent
                text: controller.statusMessage.length > 0 ? controller.statusMessage : "Ready"
                font.pixelSize: 12
                color: "#1e293b"
            }
        }
    }

    Component.onCompleted: {
        controller.refreshData();
    }
}
