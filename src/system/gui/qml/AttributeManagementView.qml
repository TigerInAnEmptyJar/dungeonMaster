import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Complete attribute management interface with list, detail view, and edit capabilities.
 * 
 * This component combines AttributeListView and AttributeDetailView with add/remove
 * functionality for full CRUD operations on attributes.
 * 
 * Set the \c attributeModel property to an AttributeListModel. Individual attribute
 * properties are edited directly through the Attribute object's Qt properties.
 */
Item {
    id: root
    
    /// The AttributeListModel to manage
    property var attributeModel: null
    
    /// Currently selected attribute
    property var selectedAttribute: null
    
    /// Whether currently in edit mode
    property bool editMode: false
    
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        
        // Left side: List view with toolbar
        Item {
            SplitView.preferredWidth: 300
            SplitView.minimumWidth: 200
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // Toolbar for list actions
                ToolBar {
                    Layout.fillWidth: true
                    
                    RowLayout {
                        anchors.fill: parent
                        
                        ToolButton {
                            text: "+"
                            font.pixelSize: 20
                            ToolTip.text: "Add new attribute"
                            ToolTip.visible: hovered
                            onClicked: addAttributeDialog.open()
                        }
                        
                        ToolButton {
                            text: "-"
                            font.pixelSize: 20
                            enabled: root.selectedAttribute !== null
                            ToolTip.text: "Delete selected attribute"
                            ToolTip.visible: hovered
                            onClicked: deleteAttributeDialog.open()
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Label {
                            text: root.attributeModel ? root.attributeModel.rowCount() + " attributes" : "0 attributes"
                            color: "#555555"
                        }
                    }
                }
                
                // Attribute list
                AttributeListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: root.attributeModel
                    
                    onAttributeSelected: function(attribute) {
                        root.selectedAttribute = attribute
                        root.editMode = false
                    }
                }
            }
        }
        
        // Right side: Detail/Edit view
        Item {
            SplitView.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // Toolbar for detail actions
                ToolBar {
                    Layout.fillWidth: true
                    visible: root.selectedAttribute !== null
                    
                    RowLayout {
                        anchors.fill: parent
                        
                        Label {
                            text: "Attribute Details"
                            font.pixelSize: 14
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        ToolButton {
                            text: root.editMode ? "View Mode" : "Edit Mode"
                            checkable: true
                            checked: root.editMode
                            onClicked: root.editMode = !root.editMode
                        }
                    }
                }
                
                // Detail view
                AttributeDetailView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    attribute: root.selectedAttribute
                    attributeModel: root.attributeModel
                    editable: root.editMode
                    
                    onSaveRequested: {
                        root.editMode = false
                        // Changes are saved automatically due to property bindings
                    }
                    
                    onCancelRequested: {
                        root.editMode = false
                        // Reload the attribute to discard changes
                        // (In a real app, you might want to implement undo/redo)
                    }
                }
                
                // Placeholder when nothing selected
                Label {
                    visible: root.selectedAttribute === null
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    text: "Select an attribute to view details"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: "#888888"
                    font.pixelSize: 16
                }
            }
        }
    }
    
    // Dialog for adding new attribute
    Dialog {
        id: addAttributeDialog
        title: "Add New Attribute"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        anchors.centerIn: parent
        
        width: 400
        
        ColumnLayout {
            spacing: 10
            anchors.fill: parent
            
            Label {
                text: "Name:"
            }
            
            TextField {
                id: nameField
                Layout.fillWidth: true
                placeholderText: "Attribute name (e.g., Strength)"
            }
            
            Label {
                text: "Description:"
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: 100
                
                TextArea {
                    id: descriptionField
                    placeholderText: "Attribute description"
                    wrapMode: TextArea.Wrap
                }
            }
        }
        
        onAccepted: {
            if (nameField.text && root.attributeModel) {
                root.attributeModel.addAttribute(nameField.text, descriptionField.text)
                nameField.text = ""
                descriptionField.text = ""
            }
        }
        
        onRejected: {
            nameField.text = ""
            descriptionField.text = ""
        }
    }
    
    // Confirmation dialog for deletion
    Dialog {
        id: deleteAttributeDialog
        title: "Delete Attribute"
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        
        Label {
            text: root.selectedAttribute ? 
                  "Are you sure you want to delete \"" + root.selectedAttribute.name + "\"?" :
                  ""
            wrapMode: Text.WordWrap
        }
        
        onAccepted: {
            if (root.selectedAttribute && root.attributeModel) {
                // Find the index of the selected attribute
                for (var i = 0; i < root.attributeModel.rowCount(); i++) {
                    if (root.attributeModel.attributeAt(i) === root.selectedAttribute) {
                        root.attributeModel.removeAttribute(i)
                        root.selectedAttribute = null
                        break
                    }
                }
            }
        }
    }
}
