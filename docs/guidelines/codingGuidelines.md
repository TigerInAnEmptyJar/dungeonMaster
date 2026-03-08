# Coding Guidelines

## Directory Structure

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

## CMake definitions

Use CMake functions `my_add_library`, `my_add_executable` and `my_add_test` for declaring CMake targets in a declarative way:

```CMake
my_add_library(myLibrary
  AUTOMOC AUTOUIC AUTORCC STATIC
  ALIAS Data::Library
  INCLUDES
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DEPENDS
    PUBLIC Qt6::Core
  HEADERS
    include/myClass.hpp
    src/privateClass.hpp
  SOURCES
    src/myClass.hpp
    src/privateClass.cpp
  FORMS
    src/myWidget.ui
  RESOURCES
    src/myResource.qrc
  QMLS
    qml/myQml.qml
  DEFINES
    USE_SUBSTUFF
)
```

## Naming conventions

|Entity||
|:--|:--|
|Files|camelCase|
|Directories|camelCase|
|Namespaces|camelCase|
|Class names|PascalCase|
|Member functions|camelCase|
|Member fields|_camelCase|
|Template types|PascalCase|
|Variable names|camelCase|
|Function names|camelCase|
|Enums/Enum classes|PascalCase|
|Enum values|ALL_CAPS|
|Interface classes|IPascalCase|
|||
|||
|||

* No hungarian notation
* Use compact, speaking names
* Bind variable explicity to lifetime (could be single letter for loop variable)

### Pimpl declarations

Destructor, constructors, copy operators and move operators (if applicable) need to be declared
in header and implemented (even if defaulted) in source file.

```C++
class MyClass {
...
private:
  struct Impl;
  std::unique_ptr<Impl> _p;
}
```

### Function declaration

Use trailing return types.

```C++
auto myFunction() -> MyReturnType;
```

## Comments/Documentation

Document the "Why", not the "What".

Use Doxygen documentation in header files for classes and free functions:

```C++
/**
 * \brief My Class description short
 * 
 * My Class description full...
 */
class MyClass {
public:
  /**
   * \brief my method description short
   * 
   * my method description short
   *
   * \param param1 description 
   * \param param2 description 
   * \returns return value description 
   */
  auto myMethod(int param1, std::string param2) -> double
};
```

## Qt declarations

### Signals

Declare Qt-Signals using:

```C++
Q_SIGNALS:
```

When calling a Qt signal, use the `Q_EMIT` empty prefix.

### Slots

Do not use slot declaration unless

* slot is to be used in QML
* slot is to be invoked by access with QMetaMethod.

Declare Qt-Slots using:

```C++
Q_SLOTS:
```

### Invokables

Some functions need to be accessible via QML, use `Q_INVOKABLE` to mark the respective method:

```C++
Q_INVOKABLE auto myMethod(int param) -> void;
```

### Properties

To make fields accessible in QML, declare them Qt properties:

```C++
class MyClass: public QObject {
  Q_OBJECT
  Q_PROPERTY(Type name READ name WRITE setName NOTIFY nameChanged);
public:
  MyClass();
  //...
  auto name() const -> Type;
  auto setName(Type const& n) -> void;
Q_SIGNALS:
  auto nameChanged(Type const& n) -> void;  
}
```

### Enums

To make enums and enum classes available to the Qt type system use `Q_ENUM`:

```C++
enum class MyEnum {V1, V2, V3};
E_ENUM(MyEnum);
```

### Metatypes

Use `Q_DECLARE_METATYPE` to make it available via Qt Properties:

```C++
namespace name {
    struct MyStruct{};
}
Q_DECLARE_METATYPE(name::MyStruct)
```

Provide this in the header of the declaring class, if it is derived from QObject.

To be able to use it across different threads call `qRegisterMetatype<name::MyStruct>();' in a static variable initialization in the constructor of the class.
