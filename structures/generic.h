#pragma once

#include <vector>

static int listenLevel;
namespace debug
{
	enum level : int
	{
		silent,
		log,
		verbose,
		error
	};

	static void emit(level noticeType, const std::string& msg)
	{
		if (noticeType == level::error)
			throw std::runtime_error(msg);
		if (noticeType <= listenLevel)
			std::cout << msg;
	}

	static void setListenLevel(const level& newLevel)
	{
		listenLevel = newLevel;
	}
}


struct Color
{
	unsigned char r{}, g{}, b{}, a{};
};

static std::istream& operator >> (std::istream& stream, Color& clr)
{
	stream.read((char*)&clr.r, 1);
	stream.read((char*)&clr.g, 1);
	stream.read((char*)&clr.b, 1);
	stream.read((char*)&clr.a, 1);
	return stream;
}

static std::ostream& operator << (std::ostream& stream, Color& clr)
{
	stream.write((const char*)&clr.r, 1);
	stream.write((const char*)&clr.g, 1);
	stream.write((const char*)&clr.b, 1);
	stream.write((const char*)&clr.a, 1);
	return stream;
}