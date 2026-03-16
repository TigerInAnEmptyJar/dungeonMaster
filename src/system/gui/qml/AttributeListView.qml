import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * List view displaying all Attribute names from an AttributeListModel.
 * 
 * On click, the selected attribute object is forwarded via the 
 * \c attributeSelected signal.
 */
Item {
    id: root

    /// The AttributeListModel to display
    property var model: null

    /// Emitted when an attribute is clicked
    /// \param attribute The Attribute object that was selected
    signal attributeSelected(var attribute)

    implicitWidth: 250
    implicitHeight: 400

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#2c3e50"

            Label {
                anchors.centerIn: parent
                text: "Attributes"
                font.pixelSize: 16
                font.bold: true
                color: "white"
            }
        }

        // List view
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: root.model
            clip: true

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            delegate: ItemDelegate {
                width: listView.width
                height: 50

                background: Rectangle {
                    color: index % 2 === 0 ? "#ffffff" : "#f8f9fa"
                    border.color: hovered ? "#3498db" : "#e0e0e0"
                    border.width: 1

                    Rectangle {
                        anchors.left: parent.left
                        width: 4
                        height: parent.height
                        color: hovered ? "#3498db" : "transparent"
                    }
                }

                contentItem: ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 2

                    Label {
                        text: model.name
                        font.pixelSize: 14
                        font.bold: true
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                    }
                }

                onClicked: {
                    listView.currentIndex = index
                    // Use the attributeObject role to get the Attribute pointer directly
                    if (model.attributeObject) {
                        root.attributeSelected(model.attributeObject)
                    }
                }

                // Highlight selected item
                Rectangle {
                    anchors.fill: parent
                    color: "#3498db"
                    opacity: listView.currentIndex === index ? 0.1 : 0
                    z: -1
                }
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                text: "No attributes available"
                color: "#888888"
                font.italic: true
                visible: listView.count === 0
            }
        }

        // Footer with count
        Rectangle {
            Layout.fillWidth: true
            height: 30
            color: "#ecf0f1"
            border.color: "#bdc3c7"
            border.width: 1

            Label {
                anchors.centerIn: parent
                text: listView.count + (listView.count === 1 ? " attribute" : " attributes")
                font.pixelSize: 11
                color: "#555555"
            }
        }
    }
}
