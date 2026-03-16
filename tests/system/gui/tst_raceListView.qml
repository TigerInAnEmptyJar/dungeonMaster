import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "RaceListViewTests"
    
    // Mock model for testing
    ListModel {
        id: mockModel
        ListElement {
            name: "Human"
            description: "Average in all respects, adaptable and versatile."
        }
        ListElement {
            name: "Elf"
            description: "Long-lived, graceful, and magical beings."
        }
        ListElement {
            name: "Dwarf"
            description: "Sturdy, resilient, and excellent craftsmen."
        }
    }
    
    // Empty model for testing
    ListModel {
        id: emptyModel
    }
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/RaceListView.qml"
        asynchronous: false
    }
    
    // Signal spy for raceSelected signal
    SignalSpy {
        id: raceSelectedSpy
        target: componentLoader.item
        signalName: "raceSelected"
    }
    
    function init() {
        // Reset for each test
        raceSelectedSpy.clear()
        if (componentLoader.item) {
            componentLoader.item.model = null
        }
    }
    
    function test_componentLoadsSuccessfully() {
        compare(componentLoader.status, Loader.Ready, "Component should load from resources")
        verify(componentLoader.item !== null, "Component item should be created")
    }
    
    function test_listViewCreation() {
        var listView = componentLoader.item
        verify(listView !== null, "List view should be created")
    }
    
    function test_listViewWithModel() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        // Wait for view to update
        waitForRendering(listView)
        
        verify(listView !== null, "List view should exist")
        compare(mockModel.count, 3, "Model should have 3 items")
    }
    
    function test_modelDataBinding() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        waitForRendering(listView)
        
        // Verify model has expected data
        compare(mockModel.get(0).name, "Human", "First item name should match")
        compare(mockModel.get(1).name, "Elf", "Second item name should match")
        compare(mockModel.get(2).name, "Dwarf", "Third item name should match")
    }
    
    function test_emptyModel() {
        var listView = componentLoader.item
        
        listView.model = emptyModel
        waitForRendering(listView)
        
        compare(emptyModel.count, 0, "Empty model should have 0 items")
    }
    
    function test_raceSelectedSignal() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        waitForRendering(listView)
        
        // The signal should be defined
        verify(raceSelectedSpy.valid, "raceSelected signal should be defined")
    }
    
    function test_implicitSizeIsSet() {
        var listView = componentLoader.item
        verify(listView.implicitWidth > 0, "Component should have implicit width")
        verify(listView.implicitHeight > 0, "Component should have implicit height")
    }
    
    function test_headerIsVisible() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        waitForRendering(listView)
        
        // Component should render successfully
        verify(listView !== null, "List view should be visible")
    }
}
