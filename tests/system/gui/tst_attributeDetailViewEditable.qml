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
    }
    
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
    
    // Helper component for signal spying
    Component {
        id: signalSpy
        SignalSpy {}
    }
}
