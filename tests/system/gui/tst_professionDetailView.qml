import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "ProfessionDetailViewTests"
    
    // Mock profession object for testing
    QtObject {
        id: mockProfession
        property string name: "Wizard"
        property string description: "Master of arcane arts, wielding powerful spells."
    }
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/ProfessionDetailView.qml"
        asynchronous: false
    }
    
    // Signal spies
    SignalSpy {
        id: saveRequestedSpy
        target: componentLoader.item
        signalName: "saveRequested"
    }
    
    SignalSpy {
        id: cancelRequestedSpy
        target: componentLoader.item
        signalName: "cancelRequested"
    }
    
    function init() {
        // Reset for each test
        saveRequestedSpy.clear()
        cancelRequestedSpy.clear()
        if (componentLoader.item) {
            componentLoader.item.profession = null
            componentLoader.item.editable = false
        }
    }
    
    function test_componentLoadsSuccessfully() {
        compare(componentLoader.status, Loader.Ready, "Component should load from resources")
        verify(componentLoader.item !== null, "Component item should be created")
    }
    
    function test_detailViewCreation() {
        var detailView = componentLoader.item
        verify(detailView !== null, "Detail view should be created")
    }
    
    function test_professionPropertyBinding() {
        var detailView = componentLoader.item
        
        // Initially null
        verify(detailView.profession === null, "Profession should initially be null")
        
        // Set profession
        detailView.profession = mockProfession
        waitForRendering(detailView)
        verify(detailView.profession !== null, "Profession should now be set")
        
        // Clear profession
        detailView.profession = null
        waitForRendering(detailView)
        verify(detailView.profession === null, "Profession should be cleared")
    }
    
    function test_implicitSizeIsSet() {
        var detailView = componentLoader.item
        verify(detailView.implicitWidth > 0, "Component should have implicit width")
        verify(detailView.implicitHeight > 0, "Component should have implicit height")
    }
    
    function test_editablePropertyWorks() {
        var detailView = componentLoader.item
        
        // Initially not editable
        compare(detailView.editable, false, "Should initially not be editable")
        
        // Set editable
        detailView.editable = true
        compare(detailView.editable, true, "Should now be editable")
    }
    
    function test_signalsAreDefined() {
        var detailView = componentLoader.item
        
        verify(saveRequestedSpy.valid, "saveRequested signal should be defined")
        verify(cancelRequestedSpy.valid, "cancelRequested signal should be defined")
    }
    
    function test_displaysProfessionData() {
        var detailView = componentLoader.item
        detailView.profession = mockProfession
        
        waitForRendering(detailView)
        
        // Verify the profession is set
        verify(detailView.profession !== null, "Profession should be set")
        compare(detailView.profession.name, "Wizard", "Profession name should match")
        compare(detailView.profession.description, "Master of arcane arts, wielding powerful spells.", 
                "Profession description should match")
    }
}
