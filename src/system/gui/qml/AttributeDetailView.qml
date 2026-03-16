import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Display panel showing the properties of a single Attribute.
 * 
 * Set the \c attribute property to an Attribute object to display its details.
 * Set \c editable to true to allow editing the attribute properties.
 */
Item {
    id: root

    /// The Attribute object to display (expected type: gurps_system::Attribute*)
    property var attribute: null
    
    /// The AttributeListModel for creating formulas
    property var attributeModel: null
    
    /// Whether the attribute properties are editable
    property bool editable: false
    
    /// Emitted when the user saves changes
    signal saveRequested()
    
    /// Emitted when the user cancels changes
    signal cancelRequested()

    implicitWidth: 300
    implicitHeight: contentLayout.implicitHeight + 20

    // Helper function to update all formula editors when attribute changes
    function updateFormulaEditors() {
        if (root.attribute && root.attribute.formula) {
            var formula = root.attribute.formula
            linearEditor.formula = formula
            lookupEditor.formula = formula
            scaledSumEditor.formula = formula
            quadraticEditor.formula = formula
            lookupDerivationEditor.formula = formula
        }
    }

    // Update editors when attribute changes
    onAttributeChanged: updateFormulaEditors()

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
                text: root.attribute ? root.attribute.name : "-"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
            
            TextField {
                visible: root.editable
                text: root.attribute ? root.attribute.name : ""
                Layout.fillWidth: true
                placeholderText: "Attribute name"
                onEditingFinished: {
                    if (root.attribute) {
                        root.attribute.name = text
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
            
            ScrollView {
                visible: root.editable
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumHeight: 100

                TextArea {
                    text: root.attribute ? root.attribute.description : ""
                    wrapMode: TextArea.Wrap
                    selectByMouse: true
                    placeholderText: "Attribute description"
                    onEditingFinished: {
                        if (root.attribute) {
                            root.attribute.description = text
                        }
                    }
                    background: Rectangle {
                        color: "white"
                        border.color: "#3498db"
                        border.width: 2
                        radius: 2
                    }
                }
            }
        }
        
        // Formula Editor (only visible in edit mode)
        GroupBox {
            visible: root.editable
            title: "Formula Configuration"
            Layout.fillWidth: true
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 10
                
                // Formula Type Selector
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10
                    
                    Label {
                        text: "Formula Type:"
                        font.bold: true
                        Layout.preferredWidth: 100
                    }
                    
                    ComboBox {
                        id: formulaTypeCombo
                        Layout.fillWidth: true
                        model: [
                            "None",
                            "Linear",
                            "Lookup Table",
                            "Scaled Sum Derivation",
                            "Quadratic Derivation",
                            "Lookup Derivation"
                        ]
                        
                        // Determine current formula type and set index
                        Component.onCompleted: updateFormulaType()
                        
                        Connections {
                            target: root
                            function onAttributeChanged() {
                                formulaTypeCombo.updateFormulaType()
                            }
                        }
                        
                        function updateFormulaType() {
                            if (!root.attributeModel) {
                                currentIndex = 0
                                return
                            }
                            
                            var formula = root.attribute ? root.attribute.formula : null
                            currentIndex = root.attributeModel.getFormulaType(formula)
                        }
                        
                        onCurrentIndexChanged: {
                            // Skip if we're just updating to match the current formula
                            if (!root.attribute || !root.attributeModel) {
                                return
                            }
                            
                            // Check if the index matches the current formula type to avoid unnecessary recreation
                            var formula = root.attribute.formula
                            var currentFormulaType = root.attributeModel.getFormulaType(formula)
                            
                            // Only create new formula if the type actually changed
                            if (currentIndex !== currentFormulaType) {
                                root.attributeModel.setAttributeFormulaType(root.attribute, currentIndex)
                            }
                        }
                    }
                }
                
                // Formula Editor Stack
                StackLayout {
                    id: formulaEditorStack
                    Layout.fillWidth: true
                    currentIndex: formulaTypeCombo.currentIndex
                    
                    // 0: None
                    Label {
                        text: "No formula configured. Select a type to create one."
                        font.italic: true
                        color: "#666"
                        Layout.fillWidth: true
                    }
                    
                    // 1: LinearFormula
                    LinearFormulaEditor {
                        id: linearEditor
                    }
                    
                    // 2: LookupFormula
                    LookupFormulaEditor {
                        id: lookupEditor
                    }
                    
                    // 3: ScaledSumDerivationFormula
                    ScaledSumDerivationFormulaEditor {
                        id: scaledSumEditor
                    }
                    
                    // 4: QuadraticDerivationFormula
                    QuadraticDerivationFormulaEditor {
                        id: quadraticEditor
                    }
                    
                    // 5: LookupDerivationFormula
                    LookupDerivationFormulaEditor {
                        id: lookupDerivationEditor
                    }
                }
            }
        }
        
        // Action buttons (only visible in edit mode)
        RowLayout {
            visible: root.editable
            Layout.fillWidth: true
            spacing: 10
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Cancel"
                onClicked: root.cancelRequested()
            }
            
            Button {
                text: "Save"
                highlighted: true
                onClicked: root.saveRequested()
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
