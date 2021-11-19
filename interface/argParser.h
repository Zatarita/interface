#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <optional>
#include <span>

namespace Interface
{
	struct Switch
	{
		std::string text;
		std::string parameter;
	};

	class ArgParser
	{
		std::string				 command;
		std::vector<std::string> flags;			// flags start with "-" and toggle behavior
		std::vector<std::string> args;			// args supply command parameters
		std::vector<Switch>		 switches;		// Switches start with "/" and toggle behavior with its own aditional parameter.

	public:
		ArgParser(int argc, char* argv[])
		{
			if (argc == 0) return;

			command = std::string(argv[0]);
			for (int i = 1; i < argc; i++)
			{
				if	    (argv[i][0] == '-') flags.push_back   (std::string(argv[i] + 1));
				else if (argv[i][0] == '/') switches.push_back({ std::string(argv[i++] + 1), std::string(argv[i]) });
				else						args.push_back    (argv[i]);
			}
		}

		bool hasFlag(const std::string& search_flag) const noexcept
		{
			for (const std::string& flag : flags)
				if (flag == search_flag) return true;
			return false;
		}

		void setFlag(const std::string& flag) noexcept
		{
			flags.push_back(flag);
		}

		std::optional<const std::string> arg(const size_t& index) const noexcept
		{
			if (index > args.size()) std::nullopt;
			return args[index];
		}

		const size_t argc() const noexcept
		{
			return args.size();
		}

		const std::string& cmd() const noexcept
		{
			return command;
		}

		std::span<const std::string> getArgs(size_t start = 0, size_t end = 0) const
		{
			if(!start && !end)
				return { args };

			if (start > args.size()) return{};		 // If the starting point is past the available args
			if (end > args.size()) return{};		 // If the end point is past the availabe args
			if (end == 0) end = args.size() - start; // If the end point is 0 it means read to end

			return { &args[start], end };
		}
	};
}
