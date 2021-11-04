#pragma once

#include <string>
#include "structures/tpl.h"
#include "interface/log.h"
#include "interface/generic.h"

namespace Interface
{
	namespace TPL
	{
		void TplBuild(int argc, char* argv[], const Log& output);
		void TplMerge(int argc, char* argv[], const Log& output);
		void TplExtract(int argc, char* argv[], const Log& output);
	}
}
