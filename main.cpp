#include "interface/interface.h"

#include "structures/collision_geometry.h"

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

