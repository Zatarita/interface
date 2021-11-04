#include "interface/interface.h"

#include <iostream>
#include <string>
#include <map>
#include <algorithm>

int main(int argc, char* argv[])
{
	Interface::CLI cli;
	cli.runCommand(argc, argv);

	return 0;
}

