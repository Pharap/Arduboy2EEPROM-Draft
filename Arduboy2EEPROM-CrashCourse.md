# `Arduboy2EEPROM` Crash Course

## Basic Usage

* Call `begin` to initialise EEPROM access.
	* The best place to call it is probably from the `setup` function, at some point after `arduboy.begin();` and some time before the first attempt to access EEPROM.
	* Call `begin` _only once_, or 'bad things' may happen.
* Use `read` or `readByte` to read data
	* `readByte` reads a single byte at the specified address
		* You must provide an address
		* The value of the byte is returned from the function
	* `read` reads an objects at the specified address
		* You must provide an address
		* You must provide a variable into which the data shall be written
* Use `write` or `writeByte` to write data
	* `writeByte` writes a single byte to the specified address
		* You must provide an address
		* You must provide a byte value to be written
	* `write` writes an object to the specified address
		* You must provide an address
		* You must provide an object to be written
* When you have finished writing data, you must call `commit` to ensure the data is saved.
	* Try to avoid calling it too often.
		* E.g. do not call it after every single write call if you have multiple calls to write occuring one after another.
	* Call it after the completion of a single 'transaction'.
		* E.g. if your game has a highscore table and you need to insert a new score into the scoreboard, call `commit` only after you have finished organising and writing all of the highscores.

### Basic Usage Example

Almost complete example of using the EEPROM functions in a game with a simple highscore table:
```cpp
// Create a type alias for score values:
// The type 'Score' will be a 32-bit unsigned integer
using Score = uint32_t;

// Number of total highscores
constexpr size_t highscoreCount = 8;

// An array of scores shall represent the highscore table
Score highscores[highscoreCount];

// A function to determine if a score is a highscore
bool isHighscore(Score score)
{
	for(size_t index = 0; index < highscoreCount; ++index)
		if(highscores[index] < score)
			return true;

	return false;
}

// A function that inserts a score if it's a new highscore
bool tryInsertHighscore(Score score)
{
	// Begin by presuming the score is not a highscore.
	// (Until proven otherwise.)
	bool isHighscore = false;

	// For every index value from 0 to (highscoreCount - 1).
	for(size_t index = 0; index < highscoreCount; ++index)
	{
		// If the score is less than or equal to
		// the highscore at the current index
		if(score <= highscores[index])
			// It's not a highscore,
			// so continue on to the next index
			continue;

		// Otherwise, if the score was greater than
		// the highscore at the current index,
		// then a highscore has been found.
		isHighscore = true;

		// Swap the highscore with the new score.
		Score oldHighscore = highscore[index];
		highscore[index] = score;
		score = oldHighscore;

		// Now 'highscore[index]' holds the old value of 'score'
		// and 'score' holds the old value of 'highscore[index]'.
	}

	// Report whether 'score' was found to be a highscore.
	return isHighscore;
}

// A function to clear all highscores
void clearHighscores()
{
	for(size_t index = 0; index < highscoreCount; ++index)
		highscores[index] = 0;
}

// Address at which the highscores are to be saved.
// Must be at least 16 and leave enough room for the full highscore table.
constexpr uintptr_t highscoreAddress = /* Value between 16 and (1023 - sizeof(highscores)) */;

// A function to load all highscores
void loadHighscores()
{
	// Load all the highscores
	Arduboy2EEPROM::read(highscoreAddress, highscores);
}

// A function to save all highscores
void saveHighscores()
{
	// Write all the highscores
	Arduboy2EEPROM::write(highscoreAddress, highscores);

	// Finalise the write transaction
	if(!Arduboy2EEPROM::commit())
		// If commit failed, signal an error with abort
		abort();
}

void setup()
{
	// Setup the Arduboy2 library
	arduboy.begin();

	// Setup EEPROM
	Arduboy2EEPROM::begin();

	// Load existing highscores
	loadHighscores();
}

void loop()
{
	if(!arduboy.nextFrame())
		return;

	arduboy.pollButtons();

	// Game logic goes here

	arduboy.display();
}
```

## Protecting Your Save Data

### General Idea

The library also provides two functions called `writeWithHash` and `readWithHash`. These enable you to detect when someone has accidentally overwritten your save data (typically it is said that the other game has 'trampled' your game's save data, or that the save data has been 'corrupted').

When you call `readWithHash`, it will return a `true` or `false` value that tells you if loading was successful. If the result is `true`, your game's data is intact and you can continue as normal. If the result is `false` then it is up to you to decide how to handle that situation. It is recommended that you offer to wipe the save data completely, allowing the player to start again with a 'blank slate'. If the player does not wish to wipe the save data, it is recommended that you refuse to launch the game (or, if the game supports different player 'profiles', refuse to launch the profile that has been corrupted).

However, these functions only work properly if you save and load _all_ of your save data at once. (The best way to do that is to keep all of your save data in a single `struct`.)

### How it Works

This works by running a special function, called a _hash function_, over the save data. The function derives a value, called a _hash value_ (or sometimes _hash code_), from that data, which it then stores along with the data. If the data is modified without properly recomputing the _hash value_ (as would happen if another game were to accidentally save over part or all of the data).

There are situations where this can fail - if another game attempts to save data of the same size to the same address, that data will have a valid _hash value_ from your game's point of view. Fortunately such scenarios are unlikely.

Note that this also won't protect your game's save data from intentional tampering. Any reasonably competent programmer could easily hack your game's data - all they would have to do is to recompute the hash after editing the save data. (And store the recomputed hash value alongside the save data, of course.)

(Not even hiding your source code or trying to create some kind of 'encryption' system would be enough to prevent a determined hacker, so you may as well just accept that possibility as inevitable and either trust people to be honest or stop caring that they can hack your game.)

### Examples

#### Single Game Save

An example of a game with a single large block of save data:
```cpp
// A struct representing a player's data in
// an RPG-style game, with various stats.
struct PlayerData
{
	// The player character's name.
	char name[8];

	// The player character's level.
	uint16_t level;

	// The player character's
	// current health and max health.
	uint16_t health;
	uint16_t maxHealth;

	// The player character's
	// attack and defence stats.
	uint16_t attack;
	uint16_t defence;

	// Maybe some other data...
};

// The size of the game's save data:
// the sum of the size of the PlayerData struct
// and the size of the type of the hash value.
constexpr size_t saveDataSize = (sizeof(PlayerData) + sizeof(Arduboy2EEPROM::HashType));

// The address at which the data is to be stored.
// It must be greater than 16, and far enough from the end that
// the whole save data (hash included) can be stored.
constexpr uintptr_t playerDataAddress = /* Value between 16 and (1023 - saveDataSize) */;

// A variable to store the player's data.
PlayerData playerData;

// A function to save the player's data.
void saveData()
{
	// Write the player data variable to EEPROM,
	// including the internal hash.
	Arduboy2EEPROM::writeWithHash(playerDataAddress, playerData);
}

void loadData()
{
	if(!Arduboy2EEPROM::readWithHash(playerDataAddress, playerData))
	{
		// If the file didn't exist or was corrupted,
		// handle the situation here.
		// E.g. by switching to a 'new game' dialogue.
	}
	else
	{
		// If the data loaded without issue, handle that situation here.
	}
}
```

#### Multiple Profiles

Aside from having a game with only a single user profile, it is also possible to save multiple player 'profiles', which has a big advantage over trying to save multiple profiles with a single [_hash value_](#how-it-works).

If multiple save profiles are saved with a single [_hash value_](#how-it-works) (i.e. one call to `saveWithHash` for _all_ the data), the corruption of one profile would render _all_ of the profiles useless. This is because the _hash value_ can only tell you whether or not the data has been corrupted, it cannot tell you where the corruption occurred.

In comparison, if each profile is saved with a separate [_hash value_](#how-it-works) (i.e. one call to `saveWithHash` _per profile_), then if only one profile is corrupted the library (via `readWithHash`) will be able to tell that the other profiles haven't been corrupted because they will still have correct [_hash values_](#how-it-works), so the player will be able to keep using the save profiles that weren't corrupted.

However, to do this properly you must properly account for the size of each profile, which, when you account for the size of the [_hash value_](#how-it-works), will be the size of the `struct` _plus_ the size of `Arduboy2EEPROM::HashType` (which can be obtained via `sizeof(Arduboy2EEPROM::HashType)`).

The code below demonstrates the calculations necessary to do this properly:
```cpp
// A struct representing the profile of a single player
struct PlayerProfile
{
	// The player's name
	char name[8];

	// Their current level
	uint8_t currentLevel;

	// How many lives they have remaining
	uint8_t livesRemaining;

	// Maybe some other data...
};

// The size of a single player profile (including the size of the hash value.)
constexpr size_t profileSaveSize = (sizeof(PlayerProfile) + sizeof(Arduboy2EEPROM::HashType));

// The maximum number of player profiles.
constexpr size_t maximumPlayerProfiles = 3;

// The total size of all player profiles (including the size of the hash values)
constexpr size_t saveDataSize = (profileSaveSize * maximumPlayerProfiles);

// The address at which the data is to be stored.
// It must be greater than 16, and far enough from the end that
// all of the save profiles (and their hashes) can be stored.
constexpr uintptr_t saveDataAddress = /* value between 16 and (1023 - saveDataSize) */;

// A variable in which to store the loaded player profile.
// (Just the one loaded, not including any others.)
PlayerProfile currentProfile;

// A function to save the specified player profile.
// 'profileNumber' must be greater than or equal to 0
// and less than 'maximumPlayerProfiles'.
// (In this case, it can be 0, 1, or 2).
void saveData(uint8_t profileNumber)
{
	// Calculate the offset of the specified profile,
	// by multiplying the profile index with the size of a single profile.
	// (Including the hashes.)
	size_t profileOffset = (profileSaveSize * profileNumber);

	// Calculate the address of the specified profile,
	// by taking the base address of the save data
	// and adding the offset calculated earlier.
	uintptr_t profileAddress = (saveDataAddress + profileOffset);

	// Write the current profile into the calculated address.
	Arduboy2EEPROM::writeWithHash(profileAddress, currentProfile);
}

// A function to load the specified player profile.
// 'profileNumber' must be greater than or equal to 0
// and less than 'maximumPlayerProfiles'.
// (In this case, it can be 0, 1, or 2).
void loadData(uint8_t profileNumber)
{
	// Calculate the offset of the specified profile,
	// by multiplying the profile index with the size of a single profile.
	// (Including the hashes.)
	size_t profileOffset = (profileSaveSize * profileNumber);

	// Calculate the address of the specified profile,
	// by taking the base address of the save data
	// and adding the offset calculated earlier.
	uintptr_t profileAddress = (saveDataAddress + profileOffset);

	// Attempt to read the profile at the calculated address
	if(!Arduboy2EEPROM::readWithHash(profileAddress, currentProfile))
	{
		// If the file didn't exist or was corrupted,
		// handle the situation here.
		// E.g. by switching to a 'new game' dialogue.
	}
	else
	{
		// If the data loaded without issue, handle that situation here.
	}
}

// A function to copy the contents of one save profile to another.
void copyData(uint8_t sourceProfile, uint8_t destinationProfile)
{
	// Load one profile.
	loadData(sourceProfile);

	// And write it into another.
	saveData(destinationProfile);
}
```

Your own code will likely look a bit different to this, this is just a simplified example to demonstrate how the calculations must be taken into account.