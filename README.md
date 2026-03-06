# Dungeon Master

This is a C++/QML application for dungeon masters and their players to manage non-player-characters as well as player characters.

It consists of 
* a desktop application (Windows 11/Linux) for dungeon masters,
* a desktop application (Windows 11/Linux) for players,
* a desktop application (Windows 11/Linux) for administration,
* a mobile application for players
* a desktop web-service

## Dependencies

### Compiler

This application is verified building using gcc (minGW) and clang other compilers might work, but you are on your own fixing issues.

Any version supporting C++23 should do fine.

Tested using:
|gcc|clang|
|:--|:--|
|15.2.1|21.1.8|

### Python

This application uses conan package manager (2.26) for managing dependencies and build tools. 

**Python >= 3.11**

```
python -m pip install -r requirements.txt
```

Using a python virtual environment is recommended.

### CMake

This application uses CMake for building, testing and packaging.

Tested using
|cmake |
|:--|
|4.2.3|

## Building

We are using CMake Presets to build.

Retrieve a list of possible presets:

```
cmake --list-presets
```

Predefined preset building desktop applications on linux:

```
cmake --workflow --preset linux_gcc_release
```
This will perform CMake configure and build the code; the unit-tests will be executed; packageing will be performed.