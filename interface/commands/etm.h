#pragma once

#include <string>
#include "structures/etm.h"
#include "interface/log.h"

namespace Interface
{
	namespace ETM
	{
		void EtmExtract(int argc, char* argv[], const Log& output);
	}
}
