#pragma once

/// @file Arduboy2EEPROM.h
/// @brief The `Arduboy2EEPROM` class.
/// @details An API for manipulating EEPROM.
/// @author [Pharap](https://github.com/Pharap)

// For size_t
#include <stddef.h>

// For uintptr_t, uint32_t
#include <stdint.h>

// For eeprom_read_byte and eeprom_update_byte
#include <avr/eeprom.h>

/// @brief
/// A `class` containing EEPROM-manipulating `static` functions.
///
/// @warning
/// The Arduboy has 1KiB of EEPROM, which spans the consecutive range
/// of addresses from 0 to 1023 inclusive. Attempting to write to or
/// read from any address beyond that range that range
/// shall result in _undefined behaviour_.
///
/// @warning
/// Violation of any of the preconditions or postconditions specified
/// herein shall result in _undefined behaviour_.
///
/// @warning
/// The consequences of failing to adhere to the
/// preconditions and postconditions shall be the responsibility
/// of the programmer using the library.
class Arduboy2EEPROM
{
public:
	/// @brief
	/// Initialises EEPROM for use.
	///
	/// @par Complexity
	/// `O(1)`.
	///
	/// @pre
	/// @li `begin()` has **not** been called previously in the program.
	///
	/// @note
	/// This function exists to support devices that do not have native EEPROM.
	/// On devices that do have native EEPROM, such as the Arduboy,
	/// this function is technically unneccessary and performs no work,
	/// thus allowing it to be optimised away by the compiler.
	///
	/// @warning
	/// @li `begin()` **must** be called before attempting to perform any
	/// EEPROM read or write operations,
	/// e.g. `readByte()`, `writeByte()`, `read()`, `write()`.
	/// @li Once `begin()` has been called, it **must not** be called again.
	static void begin()
	{
		// This function is intentionally left blank
	}
	
	/// @brief
	/// Finalises the changes made by previous write operations by
	/// committing them to memory.
	///
	/// @par Complexity
	/// `O(1)`.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	///
	/// @post
	/// @li All data written via write operations
	/// (e.g. `writeByte()`, `write()`) has been committed to memory.
	///
	/// @note
	/// This function exists to support devices that do not have native EEPROM.
	/// On devices that do have native EEPROM, such as the Arduboy,
	/// this function is technically unneccessary and performs no work,
	/// thus allowing it to be optimised away by the compiler.
	///
	/// @warning
	/// @li `commit()` **must** be called to finalise the modifications made by
	/// any previous write operations, e.g. `writeByte()`, `write()`.
	/// @li Failure to call `commit()` **may** result in the discarding of
	/// any or all of the data written by previous write operations.
	/// Discarded data is **not** committed to memory.
	///
	/// @note
	/// When some data is written and other data is not, this is
	/// known as a 'partial write'.
	static void commit()
	{
		// This function is intentionally left blank
	}

	/// @brief
	/// Writes a byte to EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(1)`.
	///
	/// @param[in] address
	/// The address at which the provided byte is to be written.
	///
	/// @param[in] byte
	/// The value of the byte to be written to EEPROM.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	///
	/// @note
	/// If the value to be written is the same as the value
	/// already stored at the specified address then this
	/// function will _not_ overwrite the already stored value.
	/// This behaviour avoids unnecessarily wasting EEPROM
	/// write-erase cycles, which are a limited resource.
	static void writeByte(uintptr_t address, unsigned char byte)
	{
		eeprom_update_byte(reinterpret_cast<unsigned char *>(address), byte);
	}

	/// @brief
	/// Reads a byte from EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(1)`.
	///
	/// @param[in] address
	/// The address of the byte to be read.
	///
	/// @return
	/// The value of the byte at the specified address in EEPROM.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	static unsigned char readByte(uintptr_t address)
	{
		return eeprom_read_byte(reinterpret_cast<const unsigned char *>(address));
	}

	/// @brief
	/// Writes any object to EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `sizeof(object)`.
	///
	/// @param[in] address
	/// The address at which the provided object is to be written.
	///
	/// @param[in] object
	/// A reference to an object that is to be written to EEPROM.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression `(address + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @note
	/// If the value to be written is the same as the value
	/// already stored at the specified address then this
	/// function will _not_ overwrite the already stored value.
	/// This behaviour avoids unnecessarily wasting EEPROM
	/// write-erase cycles, which are a limited resource.
	///
	/// @warning
	/// @parblock
	/// Whilst pointers are guaranteed to retain the value they had when they
	/// were saved, there are many circumstances in which the value of a
	/// stored pointer may become invalid before its retrieval.
	///
	/// E.g. a pointer that points to a global variable may be invalidated
	/// if the program is recompiled, with or without a change to
	/// compiler settings.
	/// @endparblock
	///
	/// @details
	/// This function writes the provided `object`'s
	/// <a href="https://en.cppreference.com/w/cpp/language/object#Object_representation_and_value_representation">
	/// <em>object representation</em></a>
	/// into EEPROM by taking a pointer to the `object`,
	/// converting it to a `const unsigned char *`,
	/// and writing the derived sequence of bytes into EEPROM.
	template<typename Type>
	static void write(uintptr_t address, const Type & object)
	{
		auto pointer = reinterpret_cast<const unsigned char *>(&object);
		
		for(size_t index = 0; index < sizeof(object); ++index)
			writeByte(address + index, pointer[index]);
	}

	/// @brief
	/// Reads any object from EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is
	/// [`sizeof(object)`](https://en.cppreference.com/w/cpp/language/sizeof).
	///
	/// @param[in] address
	/// The address of the object to be read.
	///
	/// @param[in] object
	/// A reference to an object that shall receive the data
	/// read from EEPROM.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression `(address + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @warning
	/// @parblock
	/// Whilst pointers are guaranteed to retain the value they had when they
	/// were saved, there are many circumstances in which the value of a
	/// stored pointer may become invalid before its retrieval.
	///
	/// E.g. a pointer that points to a global variable may be invalidated
	/// if the program is recompiled, with or without a change to
	/// compiler settings.
	/// @endparblock
	///
	/// @details
	/// This function overwrites the provided `object`'s
	/// <a href="https://en.cppreference.com/w/cpp/language/object#Object_representation_and_value_representation">
	/// <em>object representation</em></a>
	/// with an _object representation_ stored in EEPROM (i.e. by `write()`).
	/// It does this by taking a pointer to the `object`,
	/// converting it to an `unsigned char *`,
	/// and reading a suitably-sized sequence of bytes
	/// (i.e. a sequence of `sizeof(object)` bytes) from EEPROM.
	template<typename Type>
	static void read(uintptr_t address, Type & object)
	{
		auto pointer = reinterpret_cast<unsigned char *>(&object);
		
		for(size_t index = 0; index < sizeof(object); ++index)
			pointer[index] = readByte(address + index);
	}
	
	/// @brief
	/// The type used to represent the hash code produced
	/// by the `hash` function.
	using hash_type = uint32_t;
	
	/// @brief
	/// Calculates a hash code from the specified sequence of bytes.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `size`.
	///
	/// @param[in] data
	/// A pointer to a contiguous sequence of bytes that are
	/// to be hashed to produce a hash code.
	///
	/// @param[in] size
	/// The quantity of bytes present in the sequence.
	///
	/// @return
	/// The hash code calculated from the provided sequence of bytes.
	///
	/// @pre
	/// @li `data != nullptr` &mdash;
	/// `data` **must not** have a value of `nullptr`.
	///
	/// @note
	/// If `size` is `0`, the returned hash code will also be `0`.
	static hash_type hash(const unsigned char * data, size_t size)
	{
		hash_type value = size;
		
		for(size_t index = 0; index < size; ++index)
			value = (((value << 5) ^ (value >> 27)) ^ data[index]);
			
		return value;
	}
	
	/// @brief
	/// Calculates a hash code from the bytes of the specified object.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `sizeof(object)`.
	///
	/// @param[in] object
	/// An object from which a hash code is to be calculated.
	///
	/// @return
	/// The hash code calculated from the provided sequence of bytes.
	///
	/// @pre
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @details
	/// This function calculates a hash code of the provided `object`
	/// by hashing the bytes of the `object`'s
	/// <a href="https://en.cppreference.com/w/cpp/language/object#Object_representation_and_value_representation">
	/// <em>object representation</em></a>.
	/// It does this by taking a pointer to the `object`,
	/// converting it to a `const unsigned char *`,
	/// and calculating the hash of the resulting sequence of bytes.
	template<typename Type>
	static hash_type hash(const Type & object)
	{
		return hash(reinterpret_cast<const unsigned char *>(&object), sizeof(object));
	}

	/// @brief
	/// Writes both an object and a hash code
	/// to EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `sizeof(object)`.
	///
	/// @param[in] address
	/// The address at which the provided object and its hash code
	/// are to be written.
	///
	/// @param[in] object
	/// A reference to an object that is to be written to EEPROM.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	/// **should not** have any `virtual` base classes.
	///
	/// @note
	/// If the value to be written is the same as the value
	/// already stored at the specified address then this
	/// function will not overwrite the already stored value.
	/// This behaviour avoids unnecessarily wasting EEPROM
	/// write-erase cycles, which are a limited resource.
	///
	/// @see hash() write()
	template<typename Type>
	static void writeWithHash(uintptr_t address, const Type & object)
	{
		write(address, hash(object));
		write(address + sizeof(hash_type), object);
	}

	/// @brief
	/// Reads both an object and a hash code from EEPROM
	/// at the specified address, and determines if the
	/// hash of the object matches the stored hash code.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `sizeof(object)`.
	///
	/// @param[in] address
	/// The address of the hash code and object to be read.
	///
	/// @param[out] object
	/// A reference to an object that shall receive the data
	/// read from EEPROM.
	///
	/// @returns
	/// @li `true` if the hash of the object matched the stored hash code.
	/// @li `false` if the hash code did not match.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @see hash() read()
	template<typename Type>
	static bool readWithHash(uintptr_t address, Type & object)
	{
		hash_type storedHash;
	
		read(address, storedHash);
		read(address + sizeof(hash_type), object);
		
		return (storedHash == hash(object));
	}

	/// @brief
	/// Writes both an object and a hash code
	/// to EEPROM at the specified address.
	/// This overload accepts a custom hash provider.
	///
	/// @par Complexity
	/// The complexity of this function is equivalent to
	/// the complexity of the expression `hash(object)`,
	/// where `hash` and `object` are the `hash` and `object`
	/// parameters of this function.
	///
	/// @param[in] address
	/// The address at which the provided object and its hash code
	/// are to be written.
	///
	/// @param[in] object
	/// A reference to an object that is to be written to EEPROM.
	///
	/// @param[in] hash
	/// A hash provider.
	/// Can be any type that supports an `operator()`,
	/// thus functions, function pointers, lambda expressions,
	/// and `class`es and `struct`s with `operator()`s are all valid options.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li The expression `hash(object)` **must** be a valid expression.
	/// @li The type of `hash(object)` **should** satisfy the same
	/// requirements as `Type`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @note
	/// If the value to be written is the same as the value
	/// already stored at the specified address then this
	/// function will _not_ overwrite the already stored value.
	/// This behaviour avoids unnecessarily wasting EEPROM
	/// write-erase cycles, which are a limited resource.
	///
	/// @see hash() write()
	template<typename Hash, typename Type>
	static void writeWithHash(uintptr_t address, const Type & object, Hash && hash)
	{
		using hash_type = decltype(hash(object));
		
		const hash_type hashValue = static_cast<Hash&&>(hash)(object);
		
		write(address, hashValue);
		write(address + sizeof(hash_type), object);
	}

	/// @brief
	/// Reads both an object and a hash code from EEPROM
	/// at the specified address, and determines if the
	/// hash of the object matches the stored hash code.
	/// This overload accepts a custom hash provider.
	///
	/// @par Complexity
	/// The complexity of this function is equivalent to
	/// the complexity of the expression `hash(object)`,
	/// where `hash` and `object` are the `hash` and `object`
	/// parameters of this function.
	///
	/// @param[in] address
	/// The address of the hash code and object to be read.
	///
	/// @param[out] object
	/// A reference to an object that shall receive the data
	/// read from EEPROM.
	///
	/// @param[in] hash
	/// A hash provider.
	/// Can be any type that supports an `operator()`,
	/// thus functions, function pointers, lambda expressions,
	/// and `class`es and `struct`s with `operator()`s are all valid options.
	///
	/// @returns
	/// @li `true` if the hash of the object matched the stored hash code.
	/// @li `false` if the hash code did not match.
	///
	/// @pre
	/// @li `begin()` has been called previously in the program.
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li The expression `hash(object)` **must** be a valid expression.
	/// @li The type of `hash(object)` **should** satisfy the same
	/// requirements as `Type`.
	/// @li `Type` **should not** be a pointer type.
	/// @li `Type` **should not** have any member variables of pointer type.
	/// @li `Type` **should** be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>.
	/// @li If `Type` is a `struct` or `class` type, it **should** be a 
	/// <a href="https://en.cppreference.com/w/cpp/language/classes#Standard-layout_class">
	/// <em>standard-layout class</em></a>. &mdash;
	/// In particular, that type
	/// **should not** have any `virtual` functions and
	/// **should not** have any `virtual` base classes.
	///
	/// @see hash() read()
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