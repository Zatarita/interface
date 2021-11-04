#pragma once

#include <string>
#include "structures/smd.h"
#include "interface/log.h"

namespace Interface
{
	namespace SMD
	{
		void SmdBinExtract(int argc, char* argv[], const Log& output);
		void SmdTplExtract(int argc, char* argv[], const Log& output);
	}
}
