#include "interface/log.h"

namespace Interface
{
	void Log::setLevel(const Flags& level)
	{
		logLevel = level;
	}

	void Log::log(const Flags& severity, const std::string& msg) const
	{
		history.push_back(msg + "\n");
		if (severity == Flags::EXCEPTION)
		{
			saveHistory("dump.txt");
			throw std::logic_error(msg);
		}
		if (streamBuffer)
			if (severity <= logLevel) { *streamBuffer << msg << "\n"; }
	}

	void Log::saveHistory(const std::string& path) const
	{
		std::ofstream stream(path);
		if (!stream.is_open())
		{
			log(Flags::ERROR, "Unable To Open Requested File");
			return;
		}

		for (std::string& str : history)
			stream << str;
	}

	void Log::redirectBuffer(std::ostream* newBuffer)
	{
		streamBuffer = newBuffer;
	}
}
