import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Editor for LookupFormula properties.
 * 
 * LookupFormula: non-linear table lookup (GURPS 3e style)
 * Properties: table (map<int,int>), maxDirectBonus
 */
ColumnLayout {
    id: root
    
    /// The Formula object to edit (expected type: gurps_system::LookupFormula*)
    property var formula: null
    
    spacing: 10
    
    Label {
        text: "Lookup Formula"
        font.pixelSize: 14
        font.bold: true
        color: "#2980b9"
    }
    
    Label {
        text: "Non-linear CP costs defined by explicit lookup table."
        font.italic: true
        color: "#666"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
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
    
    // Table Editor (simplified - could be enhanced with TableView)
    GroupBox {
        title: "CP Cost Lookup Table"
        Layout.fillWidth: true
        
        ColumnLayout {
            anchors.fill: parent
            
            Label {
                text: "Table editing not yet implemented in this UI."
                font.italic: true
                color: "#999"
            }
            
            Label {
                text: "Use C++ API to populate the table programmatically."
                font.italic: true
                color: "#999"
            }
            
            // TODO: Add TableView or ListView to display/edit table entries
            // The table is a map<int, int> (level bonus → cumulative CP cost)
        }
    }
}
