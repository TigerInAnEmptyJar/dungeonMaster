# GURPS System

The GURPS System provides the following:

## Attributes

Basic traits of a character or other entity:

|Attribute|Description||
|:--|:--|:--:|
|Strength|Basic|ST|
|Dexterity|Basic|DX|
|Intelligence|Basic|IQ|
|Health|Basic|HT|
|Hit Points|Secondary|HP|
|Will|Secondary|HI|
|Perception|Secondary|PER|
|Fatique Points|Secondary|FP|
|Basic Move|Secondary|BM|
|Damage (swing)|Secondary|DMG|
|Damage (thrust)|Secondary|DMG|
|Basic Lift|Secondary|BL|
|Basic Speed|Secondary||
|Encumbrance|Secondary||
|Move|Secondary||
|Dodge|Secondary||
|Size Modifier|Basic|SM|

The level of basic attributes (strength, dexterity, intelligence, health, size modifier) only depends on the invested character points (CP).

The level of secondary attributes depend on other attribut(s). In several variants:

* other attribute plus CP modifier: (hit points, will, perception, fatigue points, basic move)
* table lookup based on other attribute: (damage variants, encumbrance)
* subtract encumbrance: (move)
* special:
  * basic lift: (ST*ST)/5 lb
  * basic speed: (DX+HT)/4
  * dodge: basic speed + 3 - encumbrance

Some secondary attributes are maxed or the CP expenditure is maxed:

* Will: max at 14, but can be extended using CP
* Fatigue points: maximum of FP added/removed by CP is 1/3 of ST (round up)

## Advantages/Disadvantages

Advantages/disadvantages come in various flavors:

* adding bonus/malus to spells/skills/attributes:
  * Energy Reserve adds FP
  * Magery adds to Thaumaturgy and Spell levels
* allowing for skills/spells/other advantges: (Magery/Clerical Investment/Druidic Investment provide spells)
* adding bonus/malus to spells/skills/attributes in certain conditions

Some advantages/disadvantages do several of those.

Some dis/advantages are one shot buys, some can be bought in levels (Energy Reserve: +1FP/level)

## Profession and Race

Race and profession are alien to GURPS in general.

The actual implementation is that a race or profession comes with a set of advantages/disadvantages and a set of known skills/spells. Cultural aspects need to be roleplayed-

Some advantages in GURPS Dungeon Fantasy are only available to respective professions or races.

## Skills

GURPS provides a huge set of skills, for Dungeon Fantasy the list has been tuned down.

A skill has the following properties:

* name
* description
* difficulty: defining the bonus depending on CP invested
* attribute it is based on: defines the level together with the difficulty and the invested CP
* defaults: some skills can be used without a CP investment, some value can be derived from an existing skill or an attribute. Some have several possible defaults.

### Spells

Spells are a special set of skills. They can only be learned if the respective advantage has been asquired.

On top of the properties of skills, there are the following properties in spells:

* time to cast
* duration: can have some duration after which the spell can be kept up, or it has a permanent effect (cannot be maintained)
* cost: fatigue point cost, can come in several variants (spell build-up or more difficult circumstances like Shape Earth on stone)
* maintenance cost: fatigue point cost when the spell is kept up
* spell type: like area spell, missile spell, information, regular, melee, blocking
* prerequisites: things necessary to enable learning the spell (advantage, attribute or other spell(s))

## Items

The weight of carried items is used to calculate the character's encumbrance.

### Armor

Armor provides damage resistance on various hit locations. Damage resistance is substracted when the character is hit before calculating the lost hit points. Some armor types work better against different damage types.

### Shield

A shield is required to be able to block during combat and the shield skill needs to be learned. The block value is (shield-skill/2 +3)

Shields have these properties:

* Defense bonus

### Melee Weapons

Melee weapons have the following properties:

* skill to wield the weapon
* damage type
* damage calculation (could be several depending on how the weapon is used)
* reach: how close does the enemy need to be
* parry: how can be parried, with which values
* minimum strength to use

### Ranged Weapons

Ranged weapons have the following properties:

* skill to wield the weapon
* damage type
* damage calculation
* acc: aiming gives this bonus
* max range
* half damage range
* minumum strength to use
* how to reload and how long does it take:
  * x rounds (quick draw skill): bows and crossbows
  * draw new weapon: bomerang, throwing knifes
  * cast new spell

## Modifiers

Individual items can be modified giving them a bonus or malus. They have the following properties:

* Item type: Shield, armor, melee, ranged
* Item id if restricted to one item object
* Weight modifier
* Skill modifier

## Management

These objects are stored in filesystem and/or can be retrieved via webserver.

To modify these objects, the role `admin` is needed. Game Master and Players do have read-only access.

## Gui

The application provides for each of those concepts a list gui and two individual displays: the read-only display for Game Master and Players to get information about the item, and an editor for the Admin to create and change the items.
