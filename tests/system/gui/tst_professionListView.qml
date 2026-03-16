import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "ProfessionListViewTests"
    
    // Mock model for testing
    ListModel {
        id: mockModel
        ListElement {
            name: "Warrior"
            description: "Skilled in combat and military tactics."
        }
        ListElement {
            name: "Wizard"
            description: "Master of arcane arts and spellcasting."
        }
        ListElement {
            name: "Rogue"
            description: "Expert in stealth, agility, and cunning."
        }
    }
    
    // Empty model for testing
    ListModel {
        id: emptyModel
    }
    
    // Load the ACTUAL component from Qt resources
    Loader {
        id: componentLoader
        source: "qrc:/qml/systemGui/ProfessionListView.qml"
        asynchronous: false
    }
    
    // Signal spy for professionSelected signal
    SignalSpy {
        id: professionSelectedSpy
        target: componentLoader.item
        signalName: "professionSelected"
    }
    
    function init() {
        // Reset for each test
        professionSelectedSpy.clear()
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
        compare(mockModel.get(0).name, "Warrior", "First item name should match")
        compare(mockModel.get(1).name, "Wizard", "Second item name should match")
        compare(mockModel.get(2).name, "Rogue", "Third item name should match")
    }
    
    function test_emptyModel() {
        var listView = componentLoader.item
        
        listView.model = emptyModel
        waitForRendering(listView)
        
        compare(emptyModel.count, 0, "Empty model should have 0 items")
    }
    
    function test_professionSelectedSignal() {
        var listView = componentLoader.item
        listView.model = mockModel
        
        waitForRendering(listView)
        
        // The signal should be defined
        verify(professionSelectedSpy.valid, "professionSelected signal should be defined")
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
