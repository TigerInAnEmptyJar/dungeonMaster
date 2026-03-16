import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * List view displaying all Race names from a RaceListModel.
 * 
 * On click, the selected race object is forwarded via the 
 * \c raceSelected signal.
 */
Item {
    id: root

    /// The RaceListModel to display
    property var model: null

    /// Emitted when a race is clicked
    /// \param race The Race object that was selected
    signal raceSelected(var race)

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
                text: "Races"
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
                    // Use the raceObject role to get the Race pointer directly
                    if (model.raceObject) {
                        root.raceSelected(model.raceObject)
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
        }
    }
}
