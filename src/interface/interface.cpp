#include "interface/interface.h"



namespace Interface
{	
	void CLI::runCommand(int argc, char* argv[])
	{
		if (argc <= 1)
			std::cout << help;

		for (int i = 1; i < argc; i++)
		{
			if (flags.count(argv[i]))
				output.setLevel(flags[argv[i]]);
			else if (commands.count(argv[i]))
			{
				commands[argv[i++]](argc - i, argv + i, output);
				return;
			}
			else
			{
				std::cout << help;
				return;
			}
		}
	}
}