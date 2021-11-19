#include "interface/log.h"


namespace Interface
{
	Log::~Log()
	{
		
	}

	void Log::setLevel(const Level& level) const
	{
		logLevel = level;
	}

	void Log::log(const Level& severity, const std::string& msg) const
	{
		history.push_back(msg + "\n");
		for (auto probe : probes)
		{
			if (severity == Level::EXCEPTION)
			{
				probe->write("EXCEPTION HALT AT: " + msg + "\n");
				saveHistory("dump.txt");
				throw std::logic_error(msg);
			}
			if (severity <= logLevel) 
				probe->write(msg + "\n");
		}
		
	}

	void Log::saveHistory(const std::string& path) const
	{
		std::ofstream stream(path);
		if (!stream.is_open())
		{
			log(Level::ERROR, "Unable To Open Requested File");
			return;
		}

		for (std::string& str : history)
			stream << str;
	}

	void Log::attachProbe(OutputWrapper* newProbe)
	{
		probes.push_back(newProbe);
	}
}
