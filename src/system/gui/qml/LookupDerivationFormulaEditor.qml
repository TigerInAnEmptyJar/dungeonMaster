import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Editor for LookupDerivationFormula properties.
 * 
 * LookupDerivationFormula: direct table lookup from parent level
 * Formula: result = table[parentLevel]
 * Properties: table (map<int,int>)
 */
ColumnLayout {
    id: root
    
    /// The DerivationFormula object to edit (expected type: gurps_system::LookupDerivationFormula*)
    property var formula: null
    
    spacing: 10
    
    Label {
        text: "Lookup Derivation"
        font.pixelSize: 14
        font.bold: true
        color: "#2980b9"
    }
    
    Label {
        text: "Derived from direct table lookup based on parent attribute level."
        font.italic: true
        color: "#666"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
    
    Label {
        text: "Example: Damage (Thrust/Swing) derived from ST via lookup table"
        font.italic: true
        color: "#666"
        Layout.fillWidth: true
    }
    
    // Table Editor
    GroupBox {
        title: "Derivation Lookup Table"
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
            // The table is a map<int, int> (parent level → derived level)
        }
    }
}
