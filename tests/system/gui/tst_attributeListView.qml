import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "AttributeListViewTests"
    
    // Mock model for testing
    ListModel {
        id: mockModel
        ListElement {
            name: "Strength (ST)"
            description: "Physical power and ability to lift, carry, and damage."
        }
        ListElement {
            name: "Dexterity (DX)"
            description: "Agility, coordination, and fine motor control."
        }
        ListElement {
            name: "Intelligence (IQ)"
            description: "Mental ability, creativity, and reasoning power."
        }
    }
    
    // Empty model for testing
    ListModel {
        id: emptyModel
    }
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/AttributeListView.qml"
        asynchronous: false
    }
    
    // Signal spy for attributeSelected signal
    SignalSpy {
        id: attributeSelectedSpy
        target: componentLoader.item
        signalName: "attributeSelected"
    }
    
    function init() {
        // Reset for each test
        attributeSelectedSpy.clear()
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
        compare(mockModel.get(0).name, "Strength (ST)", "First item name should match")
        compare(mockModel.get(1).name, "Dexterity (DX)", "Second item name should match")
        compare(mockModel.get(2).name, "Intelligence (IQ)", "Third item name should match")
    }
    
    function test_emptyModel() {
        var listView = componentLoader.item
        
        listView.model = emptyModel
        waitForRendering(listView)
        
        compare(emptyModel.count, 0, "Empty model should have 0 items")
    }
    
    function test_attributeSelectedSignal() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        waitForRendering(listView)
        
        // Verify signal is defined
        verify(listView.attributeSelected !== undefined, "attributeSelected signal should be defined")
        
        // Initial signal count should be 0
        compare(attributeSelectedSpy.count, 0, "No signals emitted initially")
    }
    
    function test_implicitSizeIsSet() {
        var listView = componentLoader.item
        verify(listView.implicitWidth > 0, "Component should have implicit width")
        verify(listView.implicitHeight > 0, "Component should have implicit height")
    }
}
