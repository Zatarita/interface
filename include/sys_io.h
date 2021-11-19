#ifndef SYS_IO
#define SYS_IO

#include <iostream>
#include <vector>

#include "structures/scalar_types.h"

using ByteArray = std::vector<std::byte>;				// Helper definition for prettier chunk referencing

class membuf : public std::basic_streambuf<char> {
	char* begin, * end;

public:
	membuf(const std::byte* p, size_t l) {
		begin = (char*)p;
		end   = (char*)(p + l);

		setg(begin, begin, end);
		setp(begin, begin, end);
	}

	// Make this more robust eventually
	pos_type seekoff(off_type _Off,
					 std::ios_base::seekdir _Way,
					 std::ios_base::openmode _Which = std::ios_base::in | std::ios_base::out)
	{
		
		switch (_Way)
		{
		case std::ios::beg:
			if (_Which == std::ios_base::in)
				setg(begin, begin + _Off, end);
			else
				setp(begin, begin + _Off, end);
			break;
		case std::ios::cur:
			if (_Which == std::ios_base::in)
				gbump(static_cast<int>(_Off));
			break;
		case std::ios::end:
			if (_Which == std::ios_base::in)
				setg(begin, end - _Off, end);
			else
				setp(begin, end - _Off, end);
			break;
		}

		return gptr() - eback();
	}

	pos_type seekpos(pos_type sp_, std::ios_base::openmode which_)
	{
		return seekoff(off_type(sp_), std::ios_base::beg, which_);
	}
};

class imemstream : public std::istream {
public:
	imemstream(const std::byte* p, size_t l) :
		std::istream(&_buffer),
		_buffer(p, l) {
		rdbuf(&_buffer);
	}

private:
	membuf _buffer;
};

class omemstream : public std::ostream {
public:
	omemstream(const std::byte* p, size_t l) :
		std::ostream(&_buffer),
		_buffer(p, l) {
		rdbuf(&_buffer);
	}

private:
	membuf _buffer;
};

/*-------------------------------------------------------------------------------------------------------------------------------------------------
 * Type Definitions
 -------------------------------------------------------------------------------------------------------------------------------------------------*/
// Little endian
using le_size_t   = Scalar<size_t,   std::endian::little>;

using le_uint8_t  = Scalar<uint8_t,  std::endian::little>;
using le_uint16_t = Scalar<uint16_t, std::endian::little>;
using le_uint32_t = Scalar<uint32_t, std::endian::little>;
using le_uint64_t = Scalar<uint64_t, std::endian::little>;

using le_int8_t   = Scalar<int8_t,   std::endian::little>;
using le_int16_t  = Scalar<int16_t,  std::endian::little>;
using le_int32_t  = Scalar<int32_t,  std::endian::little>;
using le_int64_t  = Scalar<int64_t,  std::endian::little>;
				  					 
using le_float    = Scalar<float,    std::endian::little>;
using le_double   = Scalar<double,   std::endian::little>;
				  				     
using le_bool     = Scalar<bool,     std::endian::little>;

// Big endian
using be_size_t   = Scalar<size_t,   std::endian::big>;

using be_uint8_t  = Scalar<uint8_t,  std::endian::big>;
using be_uint16_t = Scalar<uint16_t, std::endian::big>;
using be_uint32_t = Scalar<uint32_t, std::endian::big>;
using be_uint64_t = Scalar<uint64_t, std::endian::big>;

using be_int8_t   = Scalar<int8_t,   std::endian::big>;
using be_int16_t  = Scalar<int16_t,  std::endian::big>;
using be_int32_t  = Scalar<int32_t,  std::endian::big>;
using be_int64_t  = Scalar<int64_t,  std::endian::big>;
				  					 
using be_float    = Scalar<float,    std::endian::big>;
using be_double   = Scalar<double,   std::endian::big>;
				  				     
using be_bool     = Scalar<bool,     std::endian::big>;

#endif // !SYS_IO
