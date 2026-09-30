import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    width: 820
    height: 660
    title: "AIDANS - Digital Prescription Module (Doctor / Nurse)"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // Header
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: "Doctor e-Prescription Portal"
                font.pixelSize: 20
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            Rectangle {
                width: 10
                height: 10
                radius: 5
                color: controller.isServerOnline ? "#16a34a" : "#dc2626"
            }
            Label {
                text: controller.isServerOnline ? "Online" : "Offline"
                font.pixelSize: 12
                font.bold: true
                color: controller.isServerOnline ? "#16a34a" : "#dc2626"
            }

            Button {
                text: "⚙ Settings"
                onClicked: settingsPopup.open()
            }

            Button {
                text: "Refresh Data"
                onClicked: controller.refreshData()
            }
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

    // Settings Popup Dialog
    Popup {
        id: settingsPopup
        anchors.centerIn: parent
        width: 480
        height: 280
        modal: true
        focus: true

        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 12

            Label {
                text: "Server Settings"
                font.bold: true
                font.pixelSize: 16
            }

            Label {
                text: "Configure Clinic Server IP / URL (saved to config.ini):"
                font.pixelSize: 12
                color: "#64748b"
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                TextField {
                    id: serverUrlField
                    text: controller.serverUrl
                    Layout.fillWidth: true
                }

                Button {
                    text: "Test"
                    onClicked: controller.testConnection(serverUrlField.text)
                }
            }

            Label {
                text: controller.connectionStatus.length > 0 ? controller.connectionStatus : "Click Test to verify connection."
                font.pixelSize: 11
                color: controller.connectionStatus.includes("Connected") ? "#16a34a" : "#dc2626"
            }

            Item { Layout.fillHeight: true }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    text: "Cancel"
                    onClicked: settingsPopup.close()
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "Save & Apply"
                    highlighted: true
                    onClicked: {
                        controller.saveServerUrl(serverUrlField.text);
                        controller.refreshData();
                        settingsPopup.close();
                    }
                }
            }
        }
    }

    // SERVER OFFLINE ERROR OVERLAY
    Rectangle {
        id: offlineOverlay
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.6)
        visible: !controller.isServerOnline
        z: 999

        Rectangle {
            anchors.centerIn: parent
            width: 460
            height: 260
            radius: 8
            color: "#ffffff"
            border.color: "#f87171"
            border.width: 2

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12

                Label {
                    text: "⚠ Server Offline or Unreachable"
                    font.bold: true
                    font.pixelSize: 18
                    color: "#dc2626"
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: "Cannot connect to clinic server at:\n" + controller.serverUrl
                    horizontalAlignment: Text.AlignHCenter
                    color: "#475569"
                    font.pixelSize: 13
                    Layout.alignment: Qt.AlignHCenter
                }

                Item { Layout.fillHeight: true }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Button {
                        text: "⚙ Settings"
                        Layout.fillWidth: true
                        onClicked: settingsPopup.open()
                    }

                    Button {
                        text: "🔄 Retry Connection"
                        highlighted: true
                        Layout.fillWidth: true
                        onClicked: controller.refreshData()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        controller.refreshData();
    }
}
