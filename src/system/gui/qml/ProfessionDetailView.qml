import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Display panel showing the properties of a single Profession.
 * 
 * Set the \c profession property to a Profession object to display its details.
 * Set \c editable to true to allow editing the profession properties.
 */
Item {
    id: root

    /// The Profession object to display (expected type: gurps_system::Profession*)
    property var profession: null
    
    /// Whether the profession properties are editable
    property bool editable: false
    
    /// Emitted when the user saves changes
    signal saveRequested()
    
    /// Emitted when the user cancels changes
    signal cancelRequested()

    implicitWidth: 300
    implicitHeight: contentLayout.implicitHeight + 20

    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"
        border.color: "#cccccc"
        border.width: 1
        radius: 4
    }

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Label {
            text: "Profession Details"
            font.pixelSize: 18
            font.bold: true
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#cccccc"
        }

        // Display/Edit Name
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Label {
                text: "Name:"
                font.bold: true
                Layout.preferredWidth: 100
            }

            Label {
                visible: !root.editable
                text: root.profession ? root.profession.name : "-"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            
            TextField {
                visible: root.editable
                text: root.profession ? root.profession.name : ""
                Layout.fillWidth: true
                placeholderText: "Profession name"
                onEditingFinished: {
                    if (root.profession) {
                        root.profession.name = text
                    }
                }
            }
        }

        // Display/Edit Description
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            Label {
                text: "Description:"
                font.bold: true
                Layout.preferredWidth: 100
                Layout.alignment: Qt.AlignTop
            }

            ScrollView {
                visible: !root.editable
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100

                TextArea {
                    text: root.profession ? root.profession.description : "-"
                    readOnly: true
                    wrapMode: TextArea.WordWrap
                    selectByMouse: true
                    background: Rectangle {
                        color: "white"
                        border.color: "#dddddd"
                        border.width: 1
                        radius: 2
                    }
                }
            }
            
            ScrollView {
                visible: root.editable
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 150

                TextArea {
                    text: root.profession ? root.profession.description : ""
                    wrapMode: TextArea.Wrap
                    selectByMouse: true
                    placeholderText: "Profession description"
                    onEditingFinished: {
                        if (root.profession) {
                            root.profession.description = text
                        }
                    }
                }
            }
        }

        // Edit mode buttons
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: root.editable

            Item { Layout.fillWidth: true }

            Button {
                text: "Save"
                onClicked: root.saveRequested()
            }

            Button {
                text: "Cancel"
                onClicked: root.cancelRequested()
            }
        }

        // Spacer to push content to top when not in edit mode
        Item {
            Layout.fillHeight: true
            visible: !root.editable
        }
    }
}
