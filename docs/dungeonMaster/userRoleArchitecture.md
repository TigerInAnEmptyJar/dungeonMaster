# User Role Architecture

## Overview

The Dungeon Master application serves three distinct user roles, each with different permissions and UI requirements:

1. **Admin** - Full system configuration and editing capabilities
2. **Dungeon Master (DM)** - Read-only access for running game sessions
3. **Player** - Read-only access for playing characters

## User Roles and Permissions

### Admin Role

**Purpose:** System configuration, content creation, and rule management

**Capabilities:**
- **Full CRUD Operations**: Create, Read, Update, Delete all system entities (Attributes, Skills, etc.)
- **Formula Editing**: Define and modify calculation formulas for attributes and derived values
- **System Configuration**: Switch between different game system setups (e.g., different GURPS editions, house rules)
- **Data Management**: Import/export system definitions, manage templates
- **User Management**: Configure permissions and access control (if multi-user)

**UI Requirements:**
- Editable forms for all properties
- Formula editors with validation
- System switcher interface
- Bulk operations and data import/export tools
- Administrative dashboards

### Dungeon Master Role

**Purpose:** Run game sessions, manage NPCs, track game state

**Capabilities:**
- **Read-Only System Access**: View all attributes, skills, rules definitions (cannot modify system config)
- **Campaign Content Editing**: Create, edit, and delete Locations, NPCs, Groups, and other campaign-specific content
- **Character Management**: Create and modify NPCs and characters for the game
- **Session Management**: Track game state, rolls, events
- **Campaign Data**: Manage scenarios, encounters, story elements

**UI Requirements:**
- Read-only views of system data (uses existing QML components)
- Editable forms for campaign content (Locations, NPCs, Groups)
- Character sheets for viewing/creating NPCs
- Session management tools
- Quick reference lookups

### Player Role

**Purpose:** Play character, track character progression

**Capabilities:**
- **Read-Only System Access**: View relevant rules and definitions
- **Own Character**: View and update their character
- **Limited Scope**: Only access to their own character data

**UI Requirements:**
- Read-only views of system data (uses existing QML components)
- Character sheet for their character
- Dice rolling interface
- Simplified, focused UI with less clutter

## Current Implementation Status

### ✅ Completed: Read-Only System Views

The system currently has read-only QML components for viewing system data:

**Components:**
- `AttributeListView.qml` - Display list of attributes
- `AttributeDetailView.qml` - Display single attribute details
- `AttributesMain.qml` - Composite view combining list and details

**Backend:**
- `AttributeListModel` - QAbstractListModel exposing Attributes to QML
- `AttributeContainer` - Container for Attribute objects
- `Attribute` - Core data model with name and description

**Usage:**
- ✅ Usable by Dungeon Master role (read-only viewing)
- ✅ Usable by Player role (read-only viewing)
- ⚠️ Not editable - cannot be used by Admin role yet

### 🚧 Planned: Editable Admin Views

For the Admin role, we need to create editable counterparts:

**Required Components:**
- `AttributeEditForm.qml` - Editable form for modifying attributes
- `AttributeCreateDialog.qml` - Dialog for creating new attributes
- `AttributeListManagementView.qml` - List with add/remove/reorder operations
- `FormulaEditor.qml` - Editor for attribute calculation formulas (future)
- `SystemSwitcher.qml` - Interface to switch between system configurations

**Backend Extensions:**
- Editable model roles or separate edit model class
- Validation logic for user inputs
- Formula parsing and validation
- System configuration management
- Persistence layer for saving changes

## Architecture Patterns

### Component Reuse Strategy

```
Read-Only Components (Current)
    ↓ (composed within)
Editable Components (Admin)
```

**Pattern:** Editable admin views will **compose and extend** read-only views rather than duplicate them.

**Example:**
```qml
// AttributeEditForm.qml (Admin)
Item {
    // Reuse read-only detail view for display
    AttributeDetailView {
        id: detailView
        attribute: root.editableAttribute
        visible: !root.editing
    }
    
    // Overlay edit form when editing
    ColumnLayout {
        visible: root.editing
        
        TextField {
            text: root.editableAttribute.name
            onTextChanged: root.editableAttribute.name = text
        }
        
        TextArea {
            text: root.editableAttribute.description
            onTextChanged: root.editableAttribute.description = text
        }
    }
}
```

### Role-Based View Selection

```
User Login
    ↓
Role Detection
    ├─ Admin    → Editable Views (AttributeEditForm, etc.)
    ├─ DM       → Read-Only Views (AttributeDetailView, etc.)
    └─ Player   → Read-Only Views (AttributeDetailView, etc.)
```

**Implementation Options:**

1. **Separate Applications per Role** (simplest)
   - `adminApp` - Full editing capabilities
   - `dmApp` - Read-only system + DM tools
   - `playerApp` - Read-only system + player tools

2. **Single Application with Role Switching**
   - Login/role selection
   - Dynamic UI loading based on role
   - Permission checks in backend

3. **Hybrid Approach** (recommended)
   - Separate desktop apps for Admin vs DM/Player
   - Shared libraries for common components
   - Mobile apps for both DM (session tools) and Player (character sheet)

## Component Organization

### Directory Structure

```
src/
├── system/
│   ├── datamodel/          # Core data structures (role-agnostic)
│   │   ├── attribute.hpp
│   │   ├── attributeContainer.hpp
│   │   └── ...
│   ├── gui/                # Read-only QML views
│   │   ├── qml/
│   │   │   ├── AttributeListView.qml      (DM/Player)
│   │   │   ├── AttributeDetailView.qml    (DM/Player)
│   │   │   └── AttributesMain.qml         (DM/Player)
│   │   └── include/
│   │       └── attributeListModel.hpp
│   └── ...
├── gaming/                 # Campaign/session content (future)
│   ├── datamodel/          # Campaign entities
│   │   ├── character.hpp
│   │   ├── npc.hpp
│   │   ├── location.hpp
│   │   ├── group.hpp
│   │   └── campaign.hpp
│   ├── gui/                # Campaign content views
│   │   ├── qml/
│   │   │   ├── LocationListView.qml       (DM editable)
│   │   │   ├── LocationEditForm.qml       (DM editable)
│   │   │   ├── NPCListView.qml            (DM editable)
│   │   │   ├── NPCEditForm.qml            (DM editable)
│   │   │   └── CharacterSheet.qml         (DM/Player)
│   │   └── include/
│   │       ├── locationListModel.hpp      (DM editable)
│   │       └── npcListModel.hpp           (DM editable)
│   └── ...
├── admin/                  # Admin-specific components (future)
│   ├── datamodel/          # Editable models, validation
│   │   ├── editableAttributeModel.hpp
│   │   ├── formulaValidator.hpp
│   │   └── systemConfigManager.hpp
│   ├── gui/                # Editable QML views
│   │   ├── qml/
│   │   │   ├── AttributeEditForm.qml
│   │   │   ├── AttributeCreateDialog.qml
│   │   │   ├── FormulaEditor.qml
│   │   │   └── SystemAdminMain.qml
│   │   └── include/
│   │       └── editableAttributeListModel.hpp
│   └── ...
├── desktop_app/            # Desktop application
│   ├── admin/              # Admin desktop app
│   ├── dm/                 # Dungeon Master desktop app
│   └── player/             # Player desktop app (optional)
└── mobile_app/
    ├── dm/                 # DM mobile app (quick reference, session tools)
    └── player/             # Player mobile app (character sheet)
```

### Dependency Relationships

```
┌─────────────────────────────────────────────────┐
│            Applications Layer                   │
│  ┌──────────┐  ┌──────┐  ┌──────────┐           │
│  │ AdminApp │  │ DMApp│  │PlayerApp │           │
│  └─────┬────┘  └───┬──┘  └────┬─────┘           │
└────────┼───────────┼──────────┼─────────────────┘
         │           │          │
         ↓           ↓          ↓
┌─────────────────────────────────────────────────┐
│            GUI Components Layer                 │
│  ┌──────────┐  ┌─────────┐  ┌──────────────┐    │
│  │Admin GUI │  │Gaming   │  │ System GUI   │    │
│  │(Editable)│  │GUI      │  │ (Read-Only)  │    │
│  │(System)  │  │(DM Edit)│  │              │    │
│  └─────┬────┘  └────┬────┘  └───┬──────────┘    │
└────────┼────────────┼───────────┼───────────────┘
         │            │           │
         │            ↓           ↓
         │      ┌─────────────────────┐
         │      │  Read-Only Models   │
         │      │  (AttributeList)    │
         │      └─────────┬───────────┘
         │                │
         ↓                ↓
┌─────────────────────────────────────────────────┐
│             Data Model Layer                    │
│  ┌────────────┐  ┌──────────┐  ┌─────────────┐  │
│  │  Admin     │  │   DM     │  │   System    │  │
│  │ Editable   │  │ Editable │  │  Datamodel  │  │
│  │  Models    │  │  Models  │  │ (Core       │  │
│  │ (System    │  │(Campaign │  │  Entities)  │  │
│  │  Config)   │  │ Content) │  │             │  │
│  └──────┬─────┘  └─────┬────┘  └──────┬──────┘  │
└─────────┼──────────────┼──────────────┼─────────┘
          │              │              │
          ↓              ↓              ↓
    ┌─────────────┐  ┌──────────┐  ┌────────────┐
    │   System    │  │ Gaming   │  │   Core     │
    │ Datamodel   │  │Datamodel │  │ TreeItem   │
    │(Attributes, │  │(NPC,     │  │            │
    │ Skills)     │  │Location) │  │            │
    └─────────────┘  └──────────┘  └────────────┘
```

**Key Principles:**
- Admin GUI depends on System GUI (reuses read-only components)
- System GUI is role-agnostic (can be used by any role)
- Core datamodel is shared by all
- Each layer only depends on layers below it

## Development Roadmap

### Phase 1: ✅ Read-Only Views (Current)
- [x] Core datamodel (Attribute, AttributeContainer)
- [x] QAbstractListModel integration (AttributeListModel)
- [x] Read-only QML components (ListView, DetailView)
- [x] Qt resource system integration
- [x] Unit tests (C++ and QML)
- [x] Example application

**Status:** Complete and tested. Ready for DM/Player applications.

### Phase 2: 🎯 Editable Admin Framework (Next)
- [ ] Create `admin/` module structure
- [ ] Editable attribute model with validation
- [ ] Basic edit form QML components
- [ ] Save/cancel/validation workflow
- [ ] Integration tests for editing
- [ ] Admin example application

**Estimated Components:**
- `EditableAttributeModel` - Extends AttributeListModel with edit operations
- `AttributeEditForm.qml` - Form for editing single attribute
- `AttributeListEditor.qml` - List with CRUD operations
- Validation framework for property constraints

### Phase 2b: 🎮 DM Campaign Content Management (Parallel)
- [ ] Create `gaming/` module structure
- [ ] Gaming datamodel (Location, NPC, Group, Campaign)
- [ ] Editable models for campaign content (LocationListModel, NPCListModel)
- [ ] Campaign content edit forms (LocationEditForm, NPCEditForm)
- [ ] Integration with read-only system views
- [ ] DM example application

**Estimated Components:**
- `Location`, `NPC`, `Group` - Campaign entity classes
- `LocationListModel`, `NPCListModel` - Editable QAbstractListModel subclasses
- `LocationEditForm.qml`, `NPCEditForm.qml` - Edit forms for campaign content
- `LocationListView.qml`, `NPCListView.qml` - List views with CRUD operations

### Phase 3: 📋 Formula System (Future)
- [ ] Formula parser and evaluator
- [ ] Formula editor QML component
- [ ] Dependency graph for calculated attributes
- [ ] Formula validation and error reporting
- [ ] Visual formula builder (optional)

### Phase 4: 🔄 System Configuration (Future)
- [ ] System configuration datamodel
- [ ] System switcher UI
- [ ] Import/export functionality
- [ ] Template management
- [ ] Migration tools for system updates

### Phase 5: 🏗️ Application Assembly (Future)
- [ ] Admin desktop application
- [ ] DM desktop application
- [ ] Player desktop application
- [ ] DM mobile application (quick reference, session tools)
- [ ] Player mobile application (character sheet)
- [ ] Authentication and role management
- [ ] Network communication (if multi-user)

## Design Considerations

### Editable vs Read-Only Models

**Option 1: Single Model with Edit Mode**
```cpp
class AttributeListModel : public QAbstractListModel {
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
    
    // Override setData for editing in edit mode
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
};
```

**Pros:** Single model class, simpler architecture
**Cons:** Mixing concerns, harder to enforce read-only behavior

**Option 2: Separate Edit Model** (Recommended)
```cpp
class AttributeListModel : public QAbstractListModel {
    // Read-only implementation
};

class EditableAttributeListModel : public AttributeListModel {
    // Adds setData, validation, insert/remove
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    Q_INVOKABLE void addAttribute(const QString& name, const QString& description);
    Q_INVOKABLE void removeAttribute(int index);
};
```

**Pros:** Clear separation, enforces read-only by design, easier testing
**Cons:** More classes, potential code duplication

**Decision:** Use separate edit model for clearer separation of concerns and better alignment with role-based architecture.

### Formula System Design

Formulas will allow attributes to be calculated from other attributes:

**Example:**
```
Hit Points (HP) = ST  // HP equals Strength
Will = IQ            // Will equals Intelligence
Dodge = Speed + 3    // Dodge is Speed plus 3
```

**Requirements:**
- Formula parsing and validation
- Dependency resolution (avoid circular dependencies)
- Real-time recalculation
- Error handling for invalid formulas
- Admin-only editing

**Implementation Strategy:**
- Store formulas as strings in datamodel
- Parse and validate on admin save
- Compile to efficient calculation at runtime
- Use Qt's JavaScript engine or custom parser

### System Switching

Systems represent different rule sets (GURPS 3rd ed, 4th ed, house rules):

**Design:**
```cpp
class SystemConfiguration {
    QString name;
    QString description;
    QList<AttributeDefinition> attributes;
    QList<SkillDefinition> skills;
    // ... other system elements
};

class SystemManager {
    Q_INVOKABLE void loadSystem(const QString& systemId);
    Q_INVOKABLE SystemConfiguration* currentSystem();
    Q_INVOKABLE QList<SystemConfiguration*> availableSystems();
};
```

**Persistence:**
- JSON or XML files for system definitions
- Database for user data within a system
- Clear separation: system config vs instance data

## Testing Strategy

### Read-Only Components (Current)
- ✅ C++ model tests with GTest/GMock
- ✅ QML component tests with Qt Quick Test
- ✅ Integration tests loading real components

### Editable Components (Future)
- Unit tests for edit model validation
- QML tests for edit form behavior
- Integration tests for save/cancel workflows
- End-to-end tests for full edit cycles

### Role-Based Access (Future)
- Tests verifying Admin can edit
- Tests verifying DM/Player cannot edit
- Permission boundary tests

## References

- [QML Component Guidelines](../guidelines/qmlComponentGuidelines.md)
- [QML Testing Guidelines](../guidelines/qmlTestingGuidelines.md)
- [Architecture Guidelines](../guidelines/architectureGuidelines.md)
- [Coding Guidelines](../guidelines/codingGuidelines.md)
