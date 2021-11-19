#ifndef SCALAR_T
#define SCALAR_T

#include <algorithm>
#include <bit>
#include <cstddef>

#include <istream>
#include <ostream>

 /*-------------------------------------------------------------------------------------------------------------------------------------------------
  * Scalar
  * ~ ~ This is a template class that wraps scalar types for endian sensitive content. 
 -------------------------------------------------------------------------------------------------------------------------------------------------*/
template <class scalar_t, std::endian endianness = std::endian::native>
class Scalar
{
	scalar_t value;																// Memory

public:
	Scalar(const scalar_t& init_value) { assign(init_value); }					// Constructor with initialized value
	Scalar() = default;															// Default constructor
	/*-------------------------------------------------------------------------------------------------------------------------------------------------
	 * Operators 
	-------------------------------------------------------------------------------------------------------------------------------------------------*/
	operator  scalar_t () 	  { return cast(); }						 // Convert to contained type of native endianness.
	explicit operator const size_t() { return cast(); }						// Used for fixed string conversion.

	void operator=(const scalar_t& new_value) { assign(new_value); }		    // Assignment operator for value type
	template <std::endian endian_t>
	void operator=(Scalar<scalar_t, endian_t>& new_value)					    // "Copy" Assignment for generalized endianness
	{
		if (endian_t == endianness)	value = new_value.data();						// Copy with same endianness
		else						value = endian_swap(new_value.data());			// Copy with differing endianness.
	}

	void assign(const scalar_t& new_value)										// Assign a system native endian number and cast to desired endianness
	{
		if (endianness == std::endian::native)										// If system endianness matches desired endianness
			value = new_value;															// Just copy the value to memory
		else																		// If not
			value = endian_swap(new_value);												// swap the value, and then copy it to memory.
	}
	/*-------------------------------------------------------------------------------------------------------------------------------------------------
	 * Functions
	-------------------------------------------------------------------------------------------------------------------------------------------------*/
	template <std::endian endian_t = std::endian::native> scalar_t cast()   // Cast value to specified (default system native) endianness.
	{
		if (endian_t == endianness)	return value;									// No need to swap. Variable endianness matches desired endianness
		else						return endian_swap(value);						// Need to swap.    Variable endianness inverse desired endianness
	}

	scalar_t&		data()	     { return value; }								// Return a reference to the data		  (non-const)
	const scalar_t* get()  const { return &value; }								// Return a pointer to the data's address (const)
	const size_t    size() const { return sizeof(value); }						// Return the size of the data			  (const)

	/*-------------------------------------------------------------------------------------------------------------------------------------------------
	 * Utility
	-------------------------------------------------------------------------------------------------------------------------------------------------*/
	template <class type>
	type endian_swap(type data)													// Swap a value's endianness
	{
		std::byte* rawData = new (&data) std::byte[sizeof(type)];					// Get the memory that represents our data as a byte array.
		std::reverse(rawData, rawData + sizeof(type));								// Reverse the order of that byte array. (and by extention the data)
		return data;																// Return the swapped data.
	}
};

/*-------------------------------------------------------------------------------------------------------------------------------------------------
 * Stream Operators
-------------------------------------------------------------------------------------------------------------------------------------------------*/
template <class scalar_t, std::endian endianness>
std::istream& operator>> (std::istream& is, Scalar<scalar_t, endianness>& type)
{																				// Stream input operator
	is.read((char*) type.get(), type.size());							        // Write directly to the memory
	return is;																	// Return stream object
}

template <class scalar_t, std::endian endianness>
std::ostream& operator<< (std::ostream& is, const Scalar<scalar_t, endianness>& type)
{																				// Stream output operator 
	is.write( (const char*) type.get(), type.size() );							// Write directly from the memory
	return is;																	// Return stream object
}																				// *NOTE* ostream impacts cout and cerr. Cast for actual value

#endif // !SCALAR_T
