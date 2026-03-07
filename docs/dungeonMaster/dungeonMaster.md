# Dungeon Master

Main functionality of the application is for the Game Master to manage non-player-characters (NPCs)
in combat and non-combat situations. For this purpose, the application has 2 different modes:
* Preparation Mode: Here the Game Master prepares and allocates the maps, locations and groups of
  NPCs.
* Game Mode: This is the mode used in the actual game-play. Here prepared NPCs can engage in various activities, like sharing knowledge, engaging in combat, providing healing, etc.

Secondary feature is for players to manage their characters including skills, inventory, knowledge about main character NPCs and diary.

## Concepts

### Character

A character describes a person inside the game play. It is characterized by a set of properties (advantages, disadvantages, attributes, skills, spells, class, race - the things a character is) and belongings (inventory, knowledge).

#### Non-Player-Character

These are characters managed by the Game Master. They could be monsters, animals, minions, a waitress in a tavern, a boss in a dungeon. Some are simpler, some more elaborate.

##### Main Character NPC

These NPCs have background and knowledge in tiers, which can be reveiled in stages.

##### Minion

Most simple NPC type. To be encountered in a dungeon or similar encounters.

##### Character Template

To have an easy creation of NPCs, a Game Master can define Character Templates and create NPCs from it that can be refined afterwards. Useful for minions or village folk.

#### Player Character

Player Characters are managed by players. Most specific here is the knowledge features, like a lookup to which Main Character NPCs are known to the character and a diary.

### Preparation Mode

#### Game Master



#### Player

Maintain the character development, spending Character Points (level up), etc.

### Game Mode

#### Dice Checks

Have the application roll on a selected property (attribute, skill, ...) of the respective character or specify a dice roll. The application provides information on how well the check succeeds.

#### Combat Actions

