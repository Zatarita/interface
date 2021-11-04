#pragma once

#include <string>
#include "structures/dat.h"
#include "interface/log.h"

namespace Interface
{
	namespace DAT
	{
		void DatExtract(int argc, char* argv[], const Log& output);
		void DatBuild(int argc, char* argv[], const Log& output);
	}
}
