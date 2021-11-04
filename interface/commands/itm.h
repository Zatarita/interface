#pragma once

#include <string>
#include "structures/itm.h"
#include "interface/log.h"

namespace Interface
{
	namespace ITM
	{
		void ItmExtractBins(int argc, char* argv[], const Log& output);
		void ItmExtractTpls(int argc, char* argv[], const Log& output);
	}
}