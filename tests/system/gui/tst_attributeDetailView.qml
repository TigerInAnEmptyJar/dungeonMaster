import QtQuick
import QtQuick.Controls
import QtTest

TestCase {
    id: testCase
    name: "AttributeDetailViewTests"
    
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
        }
    }
    
    function test_componentLoadsSuccessfully() {
        compare(componentLoader.status, Loader.Ready, "Component should load from resources")
        verify(componentLoader.item !== null, "Component item should be created")
    }
    
    function test_detailViewCreation() {
        var detailView = componentLoader.item
        verify(detailView !== null, "Detail view should be created")
        verify(detailView.attribute === null, "Attribute should initially be null")
    }
    
    function test_detailViewWithAttribute() {
        var detailView = componentLoader.item
        detailView.attribute = mockAttribute
        
        waitForRendering(detailView)
        
        verify(detailView.attribute !== null, "Attribute should be set")
        compare(detailView.attribute.name, "Strength (ST)", "Attribute name should match")
        compare(detailView.attribute.description, "Physical power and ability to lift, carry, and damage.", 
                "Attribute description should match")
    }
    
    function test_detailViewWithNullAttribute() {
        var detailView = componentLoader.item
        detailView.attribute = null
        
        waitForRendering(detailView)
        
        verify(detailView.attribute === null, "Attribute should be null")
    }
    
    function test_attributePropertyChange() {
        var detailView = componentLoader.item
        
        // Initially null
        verify(detailView.attribute === null)
        
        // Set attribute
        detailView.attribute = mockAttribute
        waitForRendering(detailView)
        verify(detailView.attribute !== null, "Attribute should now be set")
        
        // Clear attribute
        detailView.attribute = null
        waitForRendering(detailView)
        verify(detailView.attribute === null, "Attribute should be cleared")
    }
    
    function test_implicitSizeIsSet() {
        var detailView = componentLoader.item
        verify(detailView.implicitWidth > 0, "Component should have implicit width")
        verify(detailView.implicitHeight > 0, "Component should have implicit height")
    }
}
