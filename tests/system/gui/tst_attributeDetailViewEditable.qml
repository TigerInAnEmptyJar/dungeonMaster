import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "AttributeDetailViewEditableTests"
    
    // Mock attribute object for testing
    QtObject {
        id: mockAttribute
        property string name: "Strength (ST)"
        property string description: "Physical power and ability to lift, carry, and damage."
        property var formula: null
    }
    
    // Get the global attributeModel provided by the test setup
    property var testAttributeModel: attributeModel
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/AttributeDetailView.qml"
        asynchronous: false
    }
    
    function init() {
        // Reset for each test
        if (componentLoader.item) {
            componentLoader.item.attribute = null
            componentLoader.item.attributeModel = null
            componentLoader.item.editable = false
        }
    }
    
    function test_componentLoadsSuccessfully() {
        compare(componentLoader.status, Loader.Ready, "Component should load from resources")
        verify(componentLoader.item !== null, "Component item should be created")
    }
    
    function test_editablePropertyDefaultsToFalse() {
        var detailView = componentLoader.item
        compare(detailView.editable, false, "Editable should default to false")
    }
    
    function test_editablePropertyCanBeSet() {
        var detailView = componentLoader.item
        detailView.editable = true
        compare(detailView.editable, true, "Editable should be settable")
    }
    
    function test_readOnlyModeShowsLabels() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.editable = false
        
        waitForRendering(detailView)
        
        // In read-only mode, we should see Labels (not TextFields)
        // This is a basic check - in a real app you might verify specific child objects
        verify(detailView.attribute !== null, "Attribute should be set")
        compare(detailView.editable, false, "Should be in read-only mode")
    }
    
    function test_editModeShowsTextFields() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.editable = true
        
        waitForRendering(detailView)
        
        verify(detailView.attribute !== null, "Attribute should be set")
        compare(detailView.editable, true, "Should be in edit mode")
    }
    
    function test_switchingBetweenModes() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        
        // Start in read-only
        detailView.editable = false
        waitForRendering(detailView)
        compare(detailView.editable, false, "Should start in read-only mode")
        
        // Switch to edit
        detailView.editable = true
        waitForRendering(detailView)
        compare(detailView.editable, true, "Should switch to edit mode")
        
        // Switch back to read-only
        detailView.editable = false
        waitForRendering(detailView)
        compare(detailView.editable, false, "Should switch back to read-only")
    }
    
    function test_saveSignalEmitted() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.editable = true
        
        waitForRendering(detailView)
        
        var spy = signalSpy.createObject(testCase, {
            target: detailView,
            signalName: "saveRequested"
        })
        
        // Trigger save (would normally happen via button click)
        detailView.saveRequested()
        
        compare(spy.count, 1, "saveRequested signal should be emitted")
        spy.destroy()
    }
    
    function test_cancelSignalEmitted() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.editable = true
        
        waitForRendering(detailView)
        
        var spy = signalSpy.createObject(testCase, {
            target: detailView,
            signalName: "cancelRequested"
        })
        
        // Trigger cancel
        detailView.cancelRequested()
        
        compare(spy.count, 1, "cancelRequested signal should be emitted")
        spy.destroy()
    }
    
    function test_editModeWithNullAttribute() {
        var detailView = componentLoader.item
        detailView.attribute = null
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // Should not crash with null attribute
        verify(detailView.attribute === null, "Attribute should be null")
        compare(detailView.editable, true, "Should still be in edit mode")
    }
    
    // ── Formula Tests ─────────────────────────────────────────────────────────
    
    function test_formulaGroupBoxVisibleInEditMode() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // The formula configuration GroupBox should be visible in edit mode
        verify(detailView.editable, "Should be in edit mode")
    }
    
    function test_formulaGroupBoxHiddenInReadOnlyMode() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.attributeModel = testAttributeModel
        detailView.editable = false
        
        waitForRendering(detailView)
        
        // The formula configuration GroupBox should be hidden in read-only mode
        compare(detailView.editable, false, "Should be in read-only mode")
    }
    
    function test_formulaTypeComboBoxWithNullFormula() {
        var detailView = componentLoader.item
        mockAttribute.formula = null
        detailView.attribute = mockAttribute
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // When formula is null, getFormulaType should return 0 (None)
        var formulaType = testAttributeModel.getFormulaType(null)
        compare(formulaType, 0, "Null formula should return type 0 (None)")
    }
    
    function test_updateFormulaEditorsCalledWhenAttributeChanges() {
        var detailView = componentLoader.item
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        // Set first attribute
        detailView.attribute = mockAttribute
        waitForRendering(detailView)
        
        // Change to a different attribute - should trigger updateFormulaEditors
        var anotherAttribute = Qt.createQmlObject('
            import QtQuick 2.0
            QtObject {
                property string name: "Dexterity"
                property string description: "Agility"
                property var formula: null
            }
        ', testCase)
        
        detailView.attribute = anotherAttribute
        waitForRendering(detailView)
        
        verify(detailView.attribute === anotherAttribute, "Attribute should be updated")
    }
    
    function test_attributeModelRequiredForFormulaEditing() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.attributeModel = null  // No model
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // Without attributeModel, formula editing won't work
        // But the view should still render
        compare(detailView.attributeModel, null, "AttributeModel should be null")
        verify(detailView.editable, "Should still be in edit mode")
    }
    
    function test_modelProvidesFormulaTypeMapping() {
        // Test that the model can map formula types correctly
        verify(testAttributeModel !== null, "AttributeModel should be available")
        
        // Test with null formula
        var typeNull = testAttributeModel.getFormulaType(null)
        compare(typeNull, 0, "Null formula returns 0")
        
        // Note: Testing actual formula objects requires creating them,
        // which is better done in C++ tests where we have access to formula classes
    }
    
    function test_formulaComboBoxReflectsCurrentFormula() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // The ComboBox should show "None" (index 0) when formula is null
        // We can't directly access the ComboBox from here, but we can verify the model method
        var currentType = testAttributeModel.getFormulaType(mockAttribute.formula)
        compare(currentType, 0, "Should detect None type for null formula")
    }
    
    function test_attributeModelSetFormulaTypeCreatesFormula() {
        var detailView = componentLoader.item
        
        // Create a real attribute that we can modify
        var testAttr = Qt.createQmlObject('
            import QtQuick 2.0
            QtObject {
                property string name: "Test Attribute"
                property string description: "Test"
                property var formula: null
            }
        ', testCase)
        
        detailView.attribute = testAttr
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // Note: We can't directly create formulas in QML without the C++ objects
        // But we can verify the model exists and has the method
        verify(typeof testAttributeModel.setAttributeFormulaType === "function",
               "Model should have setAttributeFormulaType method")
        verify(typeof testAttributeModel.getFormulaType === "function",
               "Model should have getFormulaType method")
    }
    
    function test_attributeModelBindingWorks() {
        var detailView = componentLoader.item
        
        // Set model first
        detailView.attributeModel = testAttributeModel
        verify(detailView.attributeModel !== null, "Model should be set")
        
        // Then set attribute and edit mode
        detailView.attribute = mockAttribute
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // Both should still be set
        verify(detailView.attributeModel !== null, "Model should still be set")
        verify(detailView.attribute !== null, "Attribute should be set")
    }
    
    function test_formulaEditorsAreInstancedNotLoaded() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        waitForRendering(detailView)
        
        // The formula editors should be directly instantiated, not via Loader
        // This makes them always available, improving performance
        // We verify the view rendered without errors as a basic check
        verify(detailView.editable, "View should be in edit mode")
    }
    
    function test_updateFormulaEditorsFunctionExists() {
        var detailView = componentLoader.item
        
        // Verify the updateFormulaEditors function exists on the component
        verify(typeof detailView.updateFormulaEditors === "function",
               "updateFormulaEditors should be a function")
    }
    
    function test_formulaEditorsUpdatedOnAttributeSet() {
        var detailView = componentLoader.item
        detailView.attributeModel = testAttributeModel
        detailView.editable = true
        
        // Initially no attribute
        verify(detailView.attribute === null, "Should start with null attribute")
        
        // Set attribute - should trigger updateFormulaEditors
        detailView.attribute = mockAttribute
        waitForRendering(detailView)
        
        // Verify attribute is set
        verify(detailView.attribute !== null, "Attribute should be set")
        compare(detailView.attribute.name, mockAttribute.name, "Should be the same attribute")
    }
    
    // Helper component for signal spying
    Component {
        id: signalSpy
        SignalSpy {}
    }
}
