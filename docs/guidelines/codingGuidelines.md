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

```
class MyClass {
...
private:
  struct Impl;
  std::unique_ptr<Impl> _p;
}
```

### Function declaration

Use trailing return types.
```
auto myFunction() -> MyReturnType;
```

## Comments/Documentation

Document the "Why", not the "What".

Use Doxygen documentation in header files for classes and free functions:
```
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

