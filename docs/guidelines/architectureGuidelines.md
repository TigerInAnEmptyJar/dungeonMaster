# Architecture Guidelines

## Guiding Principles

* SOLID Principles
* Encapsulation on all levels
* Easy to use, hard to misuse
* Testable
* Rule of 5

## SOLID Principles

### Single Responsible Principle

### Open-Close Principle

### Liskovs Substitution Principle

### Interface Seggregation Principle

### Dependency Inversion Principle

## Encapsulation

### C++ level

* No public data fields
* Interface classes
  * Pimpl idiom
  * Minimal interface  
* 

### CMake level

On CMake level we have public interfaces and package private elements. This is reflected on the directory structure:

component
  |- CMakeLists.txt
  |- include
  |    |- publicClass1.hpp
  |    |- publicClass2.hpp
  |- src
  |    |- publicClass1.cpp
  |    |- publicClass2.cpp
  |    |- packagePrivateClass.hpp
  |    |- packagePrivateClass.cpp
  |    |- resources.qrc
  |    |- form.ui
  |- qrc
  |    |- qml.qrc
  |    |- form.qml
  |    |- detail
  |    |    |- detailForm.qml

## Interfaces

To enable testing 
