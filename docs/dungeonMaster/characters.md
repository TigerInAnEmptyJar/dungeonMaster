# Characters

Characters are based on the elements defined in the System component.

For each of the items

* Attributes: (ST, DX, IQ, HT, HP, WI, PER, FP, BL, Basic Speed, Basic Move, Move, Encumbrance, Dodge)
* Dis/Advantages: selected ones
* Skills/Spells: selected ones
* Items: selected ones

an instance object is created of the respective type to take into account the actual values of the character based on the amount of CP the player spent (AttributeInstance, AdvantageInstance, SkillInstance, SpellInstance, ArmorInstance, ...).

There are some traits, which may vary temporarily:

* current hit point
* current fatigue points
* list of combat effects
  * stun
  * prone
  * frightened
  * ...

Those are the basic GURPS traits. Depending on type of character there are additional properties.

To help visualize the character, an image property is provided.

## NPC

NPCs have a list of lootable items.

### Minion

Nothing else needed

### Main Character NPC

This character has these additional properties:

* Background: some backgound story available to the Game Master to easily remember what this character is about
* Knowledge: this is a set of sharable information from or about the character. Flags show whether the respective set of information had been disclosed to which player character.

## Player Characters

The player characters hold a reference of the NPCs they have encountered. In addition, they have a note section and a diary.

## Management

NPCs are managed by the Game Master, Player characters by players.

The Game Master can create and manage a list of character templates that

## Gui

Depending on the mode and user role, the view is different:

### Preparation Mode

#### Game Master

The Game Master has a list of NPCs sortable by group, location, type, status. There is an editing view for the two different types of NPCs. NPCs can be created, deleted and moved between groups.

#### Player

The Player can view and modify their character, modifying attributes, skills, spells, dis/advantages, inventory, image, diary.

### Game Mode

#### Game Master

During Game Mode, the Game Master has different views available:

* an easy to navigate group view displaying the NPCs of the group as they are located.
* a list of characters sorted by their basic speed as their order in combat
* NPCs can be moved between different groups
* on tab/click the individual character can participate in actions as they are availably shown in a pop-up
* status of an NPC can be reset (resurrecting them if wanted)

#### Player

The Player obviously only has one character to control during game play.

* activating a skill w/wo automatic dice roll, showing by how much it passed
* attack selecting weapon
* getting up
* changing weapon
* cast spell (if available) and/or throw spell
* defend (dodge/parry/block), receiving damage (or not)
