import QtQuick // Import module dependency for runtime execution
import QtQuick.Controls // Import module dependency for runtime execution
import QtQuick.Layouts // Import module dependency for runtime execution

ApplicationWindow { // Execute clinical workflow step
    id: root // Execute clinical workflow step
    visible: true // Execute clinical workflow step
    width: 820 // Execute clinical workflow step
    height: 660 // Execute clinical workflow step
    title: "AIDANS - Digital Prescription Module (Doctor / Nurse)" // Execute clinical workflow step

    ColumnLayout { // Execute clinical workflow step
        anchors.fill: parent // Execute clinical workflow step
        anchors.margins: 16 // Execute clinical workflow step
        spacing: 12 // Execute clinical workflow step

        // Header
        RowLayout { // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            Label { // Execute clinical workflow step
                text: "Doctor e-Prescription Portal" // Execute clinical workflow step
                font.pixelSize: 20 // Execute clinical workflow step
                font.bold: true // Execute clinical workflow step
            } // Close active scope block

            Item { Layout.fillWidth: true } // Execute clinical workflow step

            Rectangle { // Execute clinical workflow step
                width: 10 // Execute clinical workflow step
                height: 10 // Execute clinical workflow step
                radius: 5 // Execute clinical workflow step
                color: controller.isServerOnline ? "#16a34a" : "#dc2626" // Execute clinical workflow step
            } // Close active scope block
            Label { // Execute clinical workflow step
                text: controller.isServerOnline ? "Online" : "Offline" // Execute clinical workflow step
                font.pixelSize: 12 // Execute clinical workflow step
                font.bold: true // Execute clinical workflow step
                color: controller.isServerOnline ? "#16a34a" : "#dc2626" // Execute clinical workflow step
            } // Close active scope block

            Button { // Execute clinical workflow step
                text: "⚙ Settings" // Execute clinical workflow step
                onClicked: settingsPopup.open() // Execute clinical workflow step
            } // Close active scope block

            Button { // Execute clinical workflow step
                text: "Refresh Data" // Execute clinical workflow step
                onClicked: controller.refreshData() // Execute clinical workflow step
            } // Close active scope block
        } // Close active scope block

        // Online Portal Patient Submissions
        GroupBox {
            title: "Online Patient Submissions (" + controller.appointments.length + ")"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 6

                ListView {
                    id: apptList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 110
                    clip: true
                    model: controller.appointments
                    delegate: Rectangle {
                        width: apptList.width
                        height: 52
                        color: controller.selectedAppointment.id === modelData.id ? "#e0f2fe" : (index % 2 === 0 ? "#f8fafc" : "#ffffff")
                        border.color: controller.selectedAppointment.id === modelData.id ? "#0284c7" : "#e2e8f0"
                        radius: 4

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 6
                            spacing: 10

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: modelData.patient_name + " (Patient ID #" + modelData.patient_id + ") • " + (modelData.patient_phone || "") + (modelData.patient_email ? " • " + modelData.patient_email : "")
                                    font.bold: true
                                    font.pixelSize: 12
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }

                                Label {
                                    text: modelData.appointment_time + " with " + modelData.doctor_name + " — Reason: " + (modelData.reason && modelData.reason.length > 0 ? modelData.reason : "No reason specified")
                                    font.pixelSize: 11
                                    color: "#475569"
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }

                            Button {
                                text: "Select to Prescribe"
                                highlighted: controller.selectedAppointment.id === modelData.id
                                onClicked: {
                                    controller.selectAppointment(index);
                                    patientIdInput.text = modelData.patient_id.toString();
                                    for (var i = 0; i < controller.doctors.length; ++i) {
                                        if (controller.doctors[i].id === modelData.doctor_id) {
                                            doctorCombo.currentIndex = i;
                                            break;
                                        }
                                    }
                                    if (modelData.reason && modelData.reason.length > 0 && notesInput.text.trim().length === 0) {
                                        notesInput.text = "Chief complaint: " + modelData.reason;
                                    }
                                }
                            }

                            Button {
                                text: "Resolved"
                                onClicked: controller.resolveAppointment(index)
                            }
                        }
                    }
                }

                Label {
                    visible: controller.selectedAppointment.patient_id !== undefined
                    text: controller.selectedAppointment.patient_id !== undefined
                          ? ("Selected: " + controller.selectedAppointment.patient_name +
                             " | Phone: " + (controller.selectedAppointment.patient_phone || "N/A") +
                             " | Email: " + (controller.selectedAppointment.patient_email || "N/A") +
                             " | Reason: " + (controller.selectedAppointment.reason || "N/A"))
                          : ""
                    font.pixelSize: 11
                    color: "#0369a1"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        // Doctor & Patient info
        RowLayout { // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            spacing: 12 // Execute clinical workflow step

            Label { text: "Doctor:" } // Execute clinical workflow step
            ComboBox { // Execute clinical workflow step
                id: doctorCombo // Execute clinical workflow step
                Layout.fillWidth: true // Execute clinical workflow step
                model: controller.doctors // Execute clinical workflow step
                textRole: "name" // Execute clinical workflow step
            } // Close active scope block

            Label { text: "Patient ID:" } // Execute clinical workflow step
            TextField { // Execute clinical workflow step
                id: patientIdInput // Execute clinical workflow step
                placeholderText: "e.g. 1" // Execute clinical workflow step
                validator: IntValidator { bottom: 1 } // Execute clinical workflow step
                Layout.preferredWidth: 90 // Execute clinical workflow step
            } // Close active scope block
        } // Close active scope block

        TextField { // Execute clinical workflow step
            id: notesInput // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            placeholderText: "Clinical consultation notes / instructions..." // Execute clinical workflow step
        } // Close active scope block

        // Add medication box
        GroupBox { // Execute clinical workflow step
            title: "Add Medication from Inventory" // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step

            RowLayout { // Execute clinical workflow step
                anchors.fill: parent // Execute clinical workflow step
                spacing: 8 // Execute clinical workflow step

                ComboBox { // Execute clinical workflow step
                    id: medCombo // Execute clinical workflow step
                    Layout.fillWidth: true // Execute clinical workflow step
                    model: controller.inventory // Execute clinical workflow step
                    textRole: "name" // Execute clinical workflow step
                } // Close active scope block

                Label { text: "Qty:" } // Execute clinical workflow step
                SpinBox { // Execute clinical workflow step
                    id: qtySpin // Execute clinical workflow step
                    from: 1 // Execute clinical workflow step
                    to: 50 // Execute clinical workflow step
                    value: 1 // Execute clinical workflow step
                    Layout.preferredWidth: 90 // Execute clinical workflow step
                } // Close active scope block

                TextField { // Execute clinical workflow step
                    id: dosageInput // Execute clinical workflow step
                    placeholderText: "Dosage (e.g. 1 tab 3x daily)" // Execute clinical workflow step
                    Layout.preferredWidth: 200 // Execute clinical workflow step
                } // Close active scope block

                Button { // Execute clinical workflow step
                    text: "+ Add" // Execute clinical workflow step
                    highlighted: true // Execute clinical workflow step
                    onClicked: { // Execute clinical workflow step
                        if (medCombo.currentIndex >= 0 && dosageInput.text.trim().length > 0) { // Evaluate conditional expression to branch execution flow
                            var item = controller.inventory[medCombo.currentIndex]; // Assign value to target variable
                            controller.addItem(item.id, item.name, qtySpin.value, dosageInput.text); // Execute clinical workflow step
                            dosageInput.text = ""; // Assign value to target variable
                            qtySpin.value = 1; // Assign value to target variable
                        } // Close active scope block
                    } // Close active scope block
                } // Close active scope block
            } // Close active scope block
        } // Close active scope block

        // Items list
        Label { // Execute clinical workflow step
            text: "Prescription Items (" + controller.cart.length + "):" // Execute clinical workflow step
            font.bold: true // Execute clinical workflow step
        } // Close active scope block

        ListView { // Execute clinical workflow step
            id: cartList // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            Layout.fillHeight: true // Execute clinical workflow step
            clip: true // Execute clinical workflow step
            model: controller.cart // Execute clinical workflow step
            delegate: Rectangle { // Execute clinical workflow step
                width: cartList.width // Execute clinical workflow step
                height: 44 // Execute clinical workflow step
                color: index % 2 === 0 ? "#f8fafc" : "#ffffff" // Assign value to target variable
                border.color: "#e2e8f0" // Execute clinical workflow step
                radius: 4 // Execute clinical workflow step

                RowLayout { // Execute clinical workflow step
                    anchors.fill: parent // Execute clinical workflow step
                    anchors.margins: 8 // Execute clinical workflow step
                    spacing: 12 // Execute clinical workflow step

                    Label { // Execute clinical workflow step
                        text: modelData.item_name // Execute clinical workflow step
                        font.bold: true // Execute clinical workflow step
                        Layout.preferredWidth: 220 // Execute clinical workflow step
                        elide: Text.ElideRight // Execute clinical workflow step
                    } // Close active scope block

                    Label { // Execute clinical workflow step
                        text: "Qty: " + modelData.quantity // Execute clinical workflow step
                        Layout.preferredWidth: 60 // Execute clinical workflow step
                    } // Close active scope block

                    Label { // Execute clinical workflow step
                        text: modelData.dosage_instructions // Execute clinical workflow step
                        Layout.fillWidth: true // Execute clinical workflow step
                        color: "#475569" // Execute clinical workflow step
                        elide: Text.ElideRight // Execute clinical workflow step
                    } // Close active scope block

                    Button { // Execute clinical workflow step
                        text: "Remove" // Execute clinical workflow step
                        onClicked: controller.removeItem(index) // Execute clinical workflow step
                    } // Close active scope block
                } // Close active scope block
            } // Close active scope block
        } // Close active scope block

        // Action row
        RowLayout { // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            spacing: 12 // Execute clinical workflow step

            Button { // Execute clinical workflow step
                text: "Clear" // Execute clinical workflow step
                onClicked: controller.clearCart() // Execute clinical workflow step
            } // Close active scope block

            Item { Layout.fillWidth: true } // Execute clinical workflow step

            Button {
                text: "Print Receipt"
                enabled: controller.lastReceiptText.length > 0 || controller.cart.length > 0
                onClicked: {
                    if (controller.cart.length > 0) {
                        var docId = doctorCombo.currentIndex >= 0 ? controller.doctors[doctorCombo.currentIndex].id : 1;
                        var patId = parseInt(patientIdInput.text, 10) || 0;
                        controller.renderReceipt("", docId, patId, notesInput.text);
                    }
                    receiptPopup.open();
                }
            }

            Button { // Execute clinical workflow step
                text: "Transmit Prescription to Pharmacy" // Execute clinical workflow step
                highlighted: true // Execute clinical workflow step
                enabled: !controller.isBusy && controller.cart.length > 0 && patientIdInput.text.length > 0 // Execute clinical workflow step
                onClicked: { // Execute clinical workflow step
                    var docId = doctorCombo.currentIndex >= 0 ? controller.doctors[doctorCombo.currentIndex].id : 1; // Assign value to target variable
                    var patId = parseInt(patientIdInput.text, 10); // Assign value to target variable
                    controller.submitPrescription(docId, patId, notesInput.text); // Execute clinical workflow step
                } // Close active scope block
            } // Close active scope block
        } // Close active scope block

        // Status bar
        Rectangle { // Execute clinical workflow step
            Layout.fillWidth: true // Execute clinical workflow step
            height: 32 // Execute clinical workflow step
            color: "#f1f5f9" // Execute clinical workflow step
            border.color: "#cbd5e1" // Execute clinical workflow step
            radius: 4 // Execute clinical workflow step

            Label { // Execute clinical workflow step
                anchors.centerIn: parent // Execute clinical workflow step
                text: controller.statusMessage.length > 0 ? controller.statusMessage : "Ready" // Execute clinical workflow step
                font.pixelSize: 12 // Execute clinical workflow step
                color: "#1e293b" // Execute clinical workflow step
            } // Close active scope block
        } // Close active scope block
    } // Close active scope block

    // Settings Popup Dialog
    Popup { // Execute clinical workflow step
        id: settingsPopup // Execute clinical workflow step
        anchors.centerIn: parent // Execute clinical workflow step
        width: 480 // Execute clinical workflow step
        height: 280 // Execute clinical workflow step
        modal: true // Execute clinical workflow step
        focus: true // Execute clinical workflow step

        contentItem: ColumnLayout { // Execute clinical workflow step
            anchors.fill: parent // Execute clinical workflow step
            anchors.margins: 16 // Execute clinical workflow step
            spacing: 12 // Execute clinical workflow step

            Label { // Execute clinical workflow step
                text: "Server Settings" // Execute clinical workflow step
                font.bold: true // Execute clinical workflow step
                font.pixelSize: 16 // Execute clinical workflow step
            } // Close active scope block

            Label { // Execute clinical workflow step
                text: "Configure Clinic Server IP / URL (saved to config.ini):" // Execute clinical workflow step
                font.pixelSize: 12 // Execute clinical workflow step
                color: "#64748b" // Execute clinical workflow step
            } // Close active scope block

            RowLayout { // Execute clinical workflow step
                Layout.fillWidth: true // Execute clinical workflow step
                spacing: 8 // Execute clinical workflow step

                TextField { // Execute clinical workflow step
                    id: serverUrlField // Execute clinical workflow step
                    text: controller.serverUrl // Execute clinical workflow step
                    Layout.fillWidth: true // Execute clinical workflow step
                } // Close active scope block

                Button { // Execute clinical workflow step
                    text: "Test" // Execute clinical workflow step
                    onClicked: controller.testConnection(serverUrlField.text) // Execute clinical workflow step
                } // Close active scope block
            } // Close active scope block

            Label { // Execute clinical workflow step
                text: controller.connectionStatus.length > 0 ? controller.connectionStatus : "Click Test to verify connection." // Execute clinical workflow step
                font.pixelSize: 11 // Execute clinical workflow step
                color: controller.connectionStatus.includes("Connected") ? "#16a34a" : "#dc2626" // Execute clinical workflow step
            } // Close active scope block

            Item { Layout.fillHeight: true } // Execute clinical workflow step

            RowLayout { // Execute clinical workflow step
                Layout.fillWidth: true // Execute clinical workflow step
                spacing: 12 // Execute clinical workflow step

                Button { // Execute clinical workflow step
                    text: "Cancel" // Execute clinical workflow step
                    onClicked: settingsPopup.close() // Execute clinical workflow step
                } // Close active scope block

                Item { Layout.fillWidth: true } // Execute clinical workflow step

                Button { // Execute clinical workflow step
                    text: "Save & Apply" // Execute clinical workflow step
                    highlighted: true // Execute clinical workflow step
                    onClicked: { // Execute clinical workflow step
                        controller.saveServerUrl(serverUrlField.text); // Execute clinical workflow step
                        controller.refreshData(); // Execute clinical workflow step
                        settingsPopup.close(); // Execute clinical workflow step
                    } // Close active scope block
                } // Close active scope block
            } // Close active scope block
        } // Close active scope block
    } // Close active scope block

    // Prescription Receipt Popup Dialog
    Popup {
        id: receiptPopup
        anchors.centerIn: parent
        width: 520
        height: 420
        modal: true
        focus: true

        contentItem: ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 10

            Label {
                text: "Prescription Receipt"
                font.bold: true
                font.pixelSize: 16
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: "RX Code:"
                    font.bold: true
                }

                TextField {
                    id: rxCodeCopyField
                    text: controller.lastPrescriptionCode.length > 0 ? controller.lastPrescriptionCode : "DRAFT"
                    readOnly: true
                    selectByMouse: true
                    font.bold: true
                    Layout.fillWidth: true
                }

                Button {
                    text: "Copy RX Code"
                    onClicked: {
                        rxCodeCopyField.selectAll();
                        rxCodeCopyField.copy();
                    }
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextArea {
                    id: receiptTextArea
                    text: controller.lastReceiptText
                    readOnly: true
                    selectByMouse: true
                    selectByKeyboard: true
                    font.family: "monospace"
                    font.pixelSize: 12
                    wrapMode: TextEdit.Wrap
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Button {
                    text: "Copy Full Receipt"
                    onClicked: {
                        receiptTextArea.selectAll();
                        receiptTextArea.copy();
                    }
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "Close"
                    highlighted: true
                    onClicked: receiptPopup.close()
                }
            }
        }
    }

    Connections {
        target: controller
        function onPrescriptionCreated(code) {
            receiptPopup.open();
        }
    }

    Component.onCompleted: { // Execute clinical workflow step
        controller.refreshData(); // Execute clinical workflow step
    } // Close active scope block
} // Close active scope block
