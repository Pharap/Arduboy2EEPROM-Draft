#pragma once

/// @file Arduboy2EEPROM.h
/// @brief The Arduboy2EEPROM Class.

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
class Arduboy2EEPROM
{
public:
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	static unsigned char readByte(uintptr_t address)
	{
		return eeprom_read_byte(reinterpret_cast<const unsigned char *>(address));
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
	/// Reads any object from EEPROM at the specified address.
	///
	/// @par Complexity
	/// `O(n)`, where `n` is `sizeof(object)`.
	///
	/// @param[in] address
	/// The address of the object to be read.
	///
	/// @param[in] object
	/// A reference to an object that shall receive the data
	/// read from EEPROM.
	///
	/// @pre
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression `(address + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
	template<typename Type>
	static void read(uintptr_t address, Type & object)
	{
		auto pointer = reinterpret_cast<unsigned char *>(object);
		
		for(size_t index = 0; index < sizeof(object); ++index)
			pointer[index] = readByte(address + index);
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression `(address + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
	///
	/// @note
	/// If the value to be written is the same as the value
	/// already stored at the specified address then this
	/// function will _not_ overwrite the already stored value.
	/// This behaviour avoids unnecessarily wasting EEPROM
	/// write-erase cycles, which are a limited resource.
	template<typename Type>
	static void write(uintptr_t address, const Type & object)
	{
		auto pointer = reinterpret_cast<const unsigned char *>(object);
		
		for(size_t index = 0; index < sizeof(object); ++index)
			writeByte(address + index, pointer[index]);
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
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** contain any `virtual` functions and
	/// **must not** use `virtual` inheritance.
	template<typename Type>
	static hash_type hash(const Type & object)
	{
		return hash(reinterpret_cast<const unsigned char *>(object), sizeof(object));
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
	/// @li `hash(object)` **must** be a valid expression.
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
	/// @li `(address <= 1023)` &mdash;
	/// `address` **must not** exceed a value of `1023`.
	/// @li `((address + sizeof(hash_type) + sizeof(object)) <= 1024)` &mdash;
	/// The value of the expression
	/// `(address + sizeof(hash_type) + sizeof(object))`
	/// **must not** exceed a value of `1024`.
	/// @li `Type` <strong>must</strong> be a
	/// <a href="https://en.cppreference.com/w/cpp/named_req/TrivialType">
	/// <em>trivial type</em></a>. &mdash;
	/// In particular, if it is a `struct` or `class` type, that type
	/// **must not** have any `virtual` functions and
	/// **must not** have any `virtual` base classes.
	/// @li `hash(object)` **must** be a valid expression.
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
};