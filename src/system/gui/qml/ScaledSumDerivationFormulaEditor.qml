import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

/**
 * Editor for ScaledSumDerivationFormula properties.
 * 
 * ScaledSumDerivationFormula: weighted sum divided by divisor
 * Formula: result = (coefficients[0]*v[0] + ... + coefficients[n-1]*v[n-1]) / divisor
 * Properties: coefficients (QList<int>), divisor
 */
ColumnLayout {
    id: root
    
    /// The DerivationFormula object to edit (expected type: gurps_system::ScaledSumDerivationFormula*)
    property var formula: null
    
    spacing: 10
    
    Label {
        text: "Scaled Sum Derivation"
        font.pixelSize: 14
        font.bold: true
        color: "#2980b9"
    }
    
    Label {
        text: "Derived from weighted sum of parent attributes divided by a divisor."
        font.italic: true
        color: "#666"
        wrapMode: Text.WordWrap
        Layout.fillWidth: true
    }
    
    Label {
        text: "Formula: (c₀×v₀ + c₁×v₁ + ... + cₙ×vₙ) / divisor"
        font.italic: true
        color: "#444"
        Layout.fillWidth: true
    }
    
    // Divisor
    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        
        Label {
            text: "Divisor:"
            Layout.preferredWidth: 120
            ToolTip.text: "Divide the weighted sum by this value"
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
    
    // Coefficients Editor
    GroupBox {
        title: "Coefficients (per-parent weights)"
        Layout.fillWidth: true
        
        ColumnLayout {
            anchors.fill: parent
            
            Label {
                text: "Coefficient editing not yet implemented in this UI."
                font.italic: true
                color: "#999"
            }
            
            Label {
                text: "Use C++ API to set coefficients programmatically."
                font.italic: true
                color: "#999"
            }
            
            // TODO: Add ListView to display/edit coefficient list
            // Each entry corresponds to a parent attribute reference
        }
    }
}
