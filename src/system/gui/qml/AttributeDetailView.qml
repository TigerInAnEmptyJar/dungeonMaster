import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Read-only display panel showing the properties of a single Attribute.
 * 
 * Set the \c attribute property to an Attribute object to display its details.
 */
Item {
    id: root

    /// The Attribute object to display (expected type: gurps_system::Attribute*)
    property var attribute: null

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
            text: "Attribute Details"
            font.pixelSize: 18
            font.bold: true
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#cccccc"
        }

        // Display Name
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Label {
                text: "Name:"
                font.bold: true
                Layout.preferredWidth: 100
            }

            Label {
                text: root.attribute ? root.attribute.name : "-"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        // Display Description
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
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 60

                TextArea {
                    text: root.attribute ? root.attribute.description : "-"
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
        }

        // Placeholder for future properties
        Label {
            text: root.attribute ? "" : "No attribute selected"
            color: "#888888"
            font.italic: true
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            visible: !root.attribute
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
