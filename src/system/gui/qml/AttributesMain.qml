import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Main window combining AttributeListView and AttributeDetailView.
 * 
 * Displays a list of attributes on the left and detailed properties on the right.
 */
ApplicationWindow {
    id: window
    
    width: 800
    height: 600
    visible: true
    title: "GURPS Attributes"

    // The AttributeListModel should be set from C++
    // property var attributeModel: null

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Left panel: Attribute list
        AttributeListView {
            id: attributeList
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            
            model: attributeModel

            onAttributeSelected: function(attribute) {
                attributeDetail.attribute = attribute
            }
        }

        // Right panel: Attribute details
        AttributeDetailView {
            id: attributeDetail
            SplitView.fillWidth: true
            SplitView.minimumWidth: 300
        }
    }
}
