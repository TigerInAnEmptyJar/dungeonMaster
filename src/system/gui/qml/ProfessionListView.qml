import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * List view displaying all Profession names from a ProfessionListModel.
 * 
 * On click, the selected profession object is forwarded via the 
 * \c professionSelected signal.
 */
Item {
    id: root

    /// The ProfessionListModel to display
    property var model: null

    /// Emitted when a profession is clicked
    /// \param profession The Profession object that was selected
    signal professionSelected(var profession)

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
                text: "Professions"
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
                    // Use the professionObject role to get the Profession pointer directly
                    if (model.professionObject) {
                        root.professionSelected(model.professionObject)
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
