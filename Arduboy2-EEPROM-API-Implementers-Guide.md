# Arduboy2 EEPROM API Implementer's Guide

## Introduction

Only the `writeByte`, `readByte`, `begin` and `commit` functions require device-specific behaviour.  
The remaining functions may be implemented as outlined by the [Class Template](#class-template).

## Implementing

### Handling Invalid Addresses

While technically attempting to write to an invalid address is considered [_undefined behaviour_](https://en.wikipedia.org/wiki/Undefined_behaviour) (i.e. anything is permitted to happen), realistically something has to happen, and since that something is not mandated, invalid address handling is effectively _implementation defined_ (i.e. implementers are free to choose how to handle invalid addresses).  

Recommended options include, in order of preference:
* Throwing an exception.
* Calling `std::abort()` from `<cstdlib>`.
* Calling `abort()` from `<stdlib.h>`.
* Setting an error flag and returning from the function.
* Ignoring the address, e.g. by returning from the function.
* Making addresses wrap around, e.g. by performing modular arithmetic on the address.
	* Note: This is what the AVR implementation does. If you can do better, you _should_, but for AVR the options are limited.

It is also recommended that you somehow log the error to identify what went wrong if it is practical to do so. Devices with fewer resources are likely to be unable to do this, but devices with more resources or more sophisticated environments may be able to do so.

### Implementing with Native EEPROM

If a device has native EEPROM, `writeByte` and `readByte` _should_ directly write to and read from the native EEPROM.

`begin` _should_ either: do nothing, or perform necessary set-up for detecting EEPROM failure.

`commit` _should_ either: always return `true`, or attempt to detect EEPROM failure.

If `writeByte` writes directly to EEPROM then `writeByte` _must_ avoid writing to EEPROM if the value at the specified address is already equal to the value specified to be written to it. E.g. if the value at address `0x200` is `0xAB`, and `writeByte` is called as `writeByte(0x200, 0xAB)`, then no write shall be performed. This behaviour is mandated to prevent the wasting of write/erase cycles, and users _must_ be able to expect and depend upon this behaviour.

#### AVR & avr-libc Example

For an AVR system using [avr-libc](https://www.nongnu.org/avr-libc/user-manual/index.html), the implementations for `writeByte`, `readByte`, `begin`, and `commit` _may_ be as thus:

```cpp
// Arduboy2EEPROM.h
#include <avr/eeprom.h>

// ...

class Arduboy2EEPROM
{
	// ...

	static void begin()
	{
		// This function is intentionally left blank
	}

	static bool commit()
	{
		return true;
	}

	static void writeByte(uintptr_t address, unsigned char byte)
	{
		eeprom_update_byte(reinterpret_cast<unsigned char *>(address), byte);
	}

	static unsigned char readByte(uintptr_t address)
	{
		return eeprom_read_byte(reinterpret_cast<const unsigned char *>(address));
	}
	
	// ...
};
```

See:
* [`<avr/eeprom.h>`](https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html)
* [`eeprom_update_byte`](https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html#ga63aee2719099e8435e8584d4b3e51991)
* [`eeprom_read_byte`](https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html#ga2d4ee8b92a592c764785fb5e4af5662b)

### Implementing with Flash Memory

When simulating EEPROM with flash memory, there are various approaches that _may_ be taken.

One approach is to create a RAM buffer to store values to (e.g. allocated when `begin` is called), and then to write the buffered data into flash only when `commit` is called. This helps reduce the number of erase cycles, and means the data is already buffered when a full page erase is required.

Another approach is to represent the value of a byte through the exclusive-oring of several (`N`) consecutive bytes of flash, thus allowing more writes before a page erasure is required, at the cost of using several (`N`) times the amount of flash needed to store the data.

Other approaches _may_ also be valid, though the two approaches outlined above are recommended.

#### ESP8266 & Arduino Example

For an ESP8266 system using [Arduino's EEPROM API](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM) as a back-end, the implementations for `writeByte`, `readByte`, `begin`, and `commit` may be as thus:

```cpp
// Arduboy2EEPROM.h
#include <EEPROM.h>

// ...

class Arduboy2EEPROM
{
	// ...

	static void begin()
	{
		// Only 1024 bytes are necessary for Arduboy
		EEPROM.begin(1024);
	}

	static bool commit()
	{
		return EEPROM.commit();
	}

	static void writeByte(uintptr_t address, unsigned char byte)
	{
		// These static_casts are not strictly necessary,
		// but are used to emphasise that a type conversion is occurring.
		EEPROM.write(static_cast<int>(address), static_cast<uint8_t>(byte));
	}

	static unsigned char readByte(uintptr_t address)
	{
		// These static_casts are not strictly necessary,
		// but are used to emphasise that a type conversion is occurring.
		return static_cast<unsigned char>(EEPROM.read(static_cast<int>(address)));
	}
	
	// ...
};
```

See:
* [`<EEPROM.h>`](https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/EEPROM.h) ([implementation](https://github.com/esp8266/Arduino/blob/master/libraries/EEPROM/EEPROM.cpp))
* [`EEPROM.begin`](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.h#L34) ([implementation](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.cpp#L46-L73))
* [`EEPROM.read`](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.h#L35) ([implementation](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.cpp#L94-L105))
* [`EEPROM.write`](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.h#L36) ([implementation](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.cpp#L107-L124))
* [`EEPROM.commit`](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.h#L37) ([implementation](https://github.com/esp8266/Arduino/blob/313b3c07ecccbe6fee24aa9fa447c4aed16ca499/libraries/EEPROM/EEPROM.cpp#L126-L139))

### Implementing with a File System

For devices that have access to a file system, there are several approaches that _may_ be taken.

One approach, much like with flash-simulated EEPROM, is to allocate a buffer to write to in `begin` and write that buffer to a file in `commit`. In this case, the file is opened only for the duration of `commit`.

Another approach is to open the file in `begin`; seek and read or write to it in `writeByte`, `readByte`, `write` and `read`; flush the file in `commit`; and only close the file when the program terminates. This can be useful for protecting the file from being written to by other processes. This may not be as performant as the first approach due to the sequential nature of file writing on most systems.

A third approach is similar to the second, but using [memory-mapped files](https://en.wikipedia.org/wiki/Memory-mapped_file) instead of a sequential file, thus avoiding the seek operations, which may improve performance.

Other approaches _may_ be viable, but the above approaches are recommended.

#### Standard C++ Example

The following example demonstrates a class that uses a 1024-byte buffer, reads the buffer from a file upon `begin` being called, and stores the buffer to a file upon `commit` being called.

```cpp
// Arduboy2EEPROM.h
#include <fstream>

// ...

class Arduboy2EEPROM
{
	// ...

private:
	static unsigned char buffer[1024];

public:
	static void begin()
	{
		// Create a file variable
		std::ifstream file;

		// Enable exceptions
		file.exceptions(ifstream::badbit);
		
		// Open a file named 'eeprom' in binary read mode
		file.open("eeprom", std::ios_base::binary);

		// Read the contents of the file
		file.read(reinterpret_cast<char *>(buffer), sizeof(buffer));
	}

	static bool commit()
	{
		// Create a file variable
		std::ofstream file;

		// Open a file named 'eeprom' in binary write mode
		file.open("eeprom", std::ios_base::binary);

		// If the file failed to open
		if(!file.is_open())
			// Return failure
			return false;

		// Write all bytes to the file
		file.write(reinterpret_cast<const char *>(buffer), sizeof(buffer));

		// Return success
		return file.good();
	}

	static void writeByte(uintptr_t address, unsigned char byte)
	{
		eeprom_update_byte(reinterpret_cast<unsigned char *>(address), byte);
	}

	static unsigned char readByte(uintptr_t address)
	{
		return eeprom_read_byte(reinterpret_cast<const unsigned char *>(address));
	}
	
	// ...
};
```
```cpp
// Arduboy2EEPROM.cpp
#include "Arduboy2EEPROM.h"

// Construct the static member variable
unsigned char Arduboy2EEPROM::buffer[1024] {};
```

See:
* [`<fstream>`](https://en.cppreference.com/w/cpp/header/fstream)
	* [`std::ifstream`](https://en.cppreference.com/w/cpp/io/basic_ifstream)
	* [`std::ofstream`](https://en.cppreference.com/w/cpp/io/basic_ofstream)
* [`std::ios_base::openmode`](https://en.cppreference.com/w/cpp/io/ios_base/openmode)

## Class Template

```cpp
class Arduboy2EEPROM
{
public:
	static void begin();

	static bool commit();

	static void writeByte(uintptr_t address, unsigned char byte);

	static unsigned char readByte(uintptr_t address);

	static void write(uintptr_t address, const unsigned char * data, size_t size)
	{
		for(size_t index = 0; index < size; ++index)
			writeByte(address + index, data[index]);
	}

	template<typename Type>
	static void write(uintptr_t address, const Type & object)
	{
		write(reinterpret_cast<const unsigned char *>(&object), sizeof(object));
	}

	static void read(uintptr_t address, const unsigned char * data, size_t size)
	{
		for(size_t index = 0; index < size; ++index)
			data[index] = readByte(address + index);
	}

	template<typename Type>
	static void read(uintptr_t address, Type & object)
	{
		read(reinterpret_cast<unsigned char *>(&object), sizeof(object));
	}

	using hash_type = uint32_t;

	static hash_type hash(const unsigned char * data, size_t size)
	{
		hash_type value = size;
		
		for(size_t index = 0; index < size; ++index)
			value = (((value << 5) ^ (value >> 27)) ^ data[index]);
			
		return value;
	}

	template<typename Type>
	static hash_type hash(const Type & object)
	{
		return hash(reinterpret_cast<const unsigned char *>(&object), sizeof(object));
	}

	template<typename Type>
	static void writeWithHash(uintptr_t address, const Type & object)
	{
		write(address, hash(object));
		write(address + sizeof(hash_type), object);
	}

	template<typename Type>
	static bool readWithHash(uintptr_t address, Type & object)
	{
		hash_type storedHash;
	
		read(address, storedHash);
		read(address + sizeof(hash_type), object);
		
		return (storedHash == hash(object));
	}

	template<typename Hash, typename Type>
	static void writeWithHash(uintptr_t address, const Type & object, Hash && hash)
	{
		using hash_type = decltype(hash(object));
		
		const hash_type hashValue = static_cast<Hash&&>(hash)(object);
		
		write(address, hashValue);
		write(address + sizeof(hash_type), object);
	}

	template<typename Hash, typename Type>
	static bool readWithHash(uintptr_t address, Type & object, Hash && hash)
	{
		using hash_type = decltype(hash(object));
	
		hash_type storedHash;
	
		read(address, storedHash);
		read(address + sizeof(hash_type), object);
		
		const hash_type hashValue = static_cast<Hash&&>(hash)(object);
		
		return (storedHash == hashValue);
	}
};
```