import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Editor for LinearFormula properties.
 * 
 * LinearFormula: constant CP cost per level (GURPS 4e style)
 * Properties: costPerLevel, maxDirectBonus
 */
ColumnLayout {
    id: root
    
    /// The Formula object to edit (expected type: gurps_system::LinearFormula*)
    property var formula: null
    
    spacing: 10
    
    Label {
        text: "Linear Formula"
        font.pixelSize: 14
        font.bold: true
        color: "#2980b9"
    }
    
    Label {
        text: "Each level costs a constant number of character points."
        font.italic: true
        color: "#666"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
    
    // Cost Per Level
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        
        Label {
            text: "Cost Per Level:"
            Layout.preferredWidth: 120
            ToolTip.text: "Character points required per level"
            ToolTip.visible: costPerLevelMouseArea.containsMouse
            ToolTip.delay: 500
            
            MouseArea {
                id: costPerLevelMouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
            }
        }
        
        SpinBox {
            id: costPerLevelSpinBox
            from: 1
            to: 999
            value: root.formula ? root.formula.costPerLevel : 10
            editable: true
            onValueModified: {
                if (root.formula) {
                    root.formula.costPerLevel = value
                }
            }
        }
        
        Label {
            text: "CP"
            color: "#666"
        }
        
        Item { Layout.fillWidth: true }
    }
    
    // Max Direct Bonus
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        
        Label {
            text: "Max Direct Bonus:"
            Layout.preferredWidth: 120
            ToolTip.text: "Maximum levels that can be purchased (-1 = unlimited)"
            ToolTip.visible: maxBonusMouseArea.containsMouse
            ToolTip.delay: 500
            
            MouseArea {
                id: maxBonusMouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
            }
        }
        
        SpinBox {
            id: maxBonusSpinBox
            from: -1
            to: 999
            value: root.formula ? root.formula.maxDirectBonus : -1
            editable: true
            onValueModified: {
                if (root.formula) {
                    root.formula.maxDirectBonus = value
                }
            }
        }
        
        Label {
            text: "(−1 = unlimited)"
            color: "#666"
        }
        
        Item { Layout.fillWidth: true }
    }
}
