import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "RaceDetailViewTests"
    
    // Mock race object for testing
    QtObject {
        id: mockRace
        property string name: "Elf"
        property string description: "Long-lived, graceful beings with magical affinity."
    }
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/RaceDetailView.qml"
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
            componentLoader.item.race = null
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
    
    function test_racePropertyBinding() {
        var detailView = componentLoader.item
        
        // Initially null
        verify(detailView.race === null, "Race should initially be null")
        
        // Set race
        detailView.race = mockRace
        waitForRendering(detailView)
        verify(detailView.race !== null, "Race should now be set")
        
        // Clear race
        detailView.race = null
        waitForRendering(detailView)
        verify(detailView.race === null, "Race should be cleared")
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
    
    function test_displaysRaceData() {
        var detailView = componentLoader.item
        detailView.race = mockRace
        
        waitForRendering(detailView)
        
        // Verify the race is set
        verify(detailView.race !== null, "Race should be set")
        compare(detailView.race.name, "Elf", "Race name should match")
        compare(detailView.race.description, "Long-lived, graceful beings with magical affinity.", 
                "Race description should match")
    }
}
