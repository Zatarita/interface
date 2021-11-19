#include "interface/interface.h"

namespace Interface
{	
	CLI::CLI()
	{
		output.attachProbe( new stdOutput() );				   // Lifetime managed by logger
		//output.attachProbe( new fileOutput("Interface.log") ); // Lifetime managed by logger
	}

	void CLI::runCommand(int argc, char* argv[])
	{
		if (argc == 0) { output.log(Level::ERROR, help); return; }

		ArgParser parser(argc - 1, argv + 1);						// Shave off the program name from the arguments

		if(parser.hasFlag("verbose") || parser.hasFlag("v") )
			output.setLevel(Level::VERBOSE);
		else if (parser.hasFlag("silent") || parser.hasFlag("s"))
			output.setLevel(Level::SILENT);

		if ( commands.count( parser.cmd() ) )
			commands[parser.cmd()](parser, output);
		else
			output.log(Level::ERROR, help);
	}
}