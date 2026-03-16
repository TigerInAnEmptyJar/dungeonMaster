import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Editor for QuadraticDerivationFormula properties.
 * 
 * QuadraticDerivationFormula: parent squared divided by divisor
 * Formula: result = v[0]² / divisor
 * Properties: divisor
 */
ColumnLayout {
    id: root
    
    /// The DerivationFormula object to edit (expected type: gurps_system::QuadraticDerivationFormula*)
    property var formula: null
    
    spacing: 10
    
    Label {
        text: "Quadratic Derivation"
        font.pixelSize: 14
        font.bold: true
        color: "#2980b9"
    }
    
    Label {
        text: "Derived from parent attribute squared, divided by a divisor."
        font.italic: true
        color: "#666"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
    
    Label {
        text: "Formula: v² / divisor"
        font.italic: true
        color: "#444"
        Layout.fillWidth: true
    }
    
    Label {
        text: "Example: Basic Lift = ST² / 5"
        font.italic: true
        color: "#666"
        Layout.fillWidth: true
    }
    
    // Divisor
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        
        Label {
            text: "Divisor:"
            Layout.preferredWidth: 120
            ToolTip.text: "Divide the squared parent value by this"
            ToolTip.visible: divisorMouseArea.containsMouse
            ToolTip.delay: 500
            
            MouseArea {
                id: divisorMouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.NoButton
            }
        }
        
        SpinBox {
            id: divisorSpinBox
            from: 1
            to: 999
            value: root.formula ? root.formula.divisor : 1
            editable: true
            onValueModified: {
                if (root.formula) {
                    root.formula.divisor = value
                }
            }
        }
        
        Item { Layout.fillWidth: true }
    }
}
