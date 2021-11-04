#pragma once
#include <string>
#include "structures/afs.h"
#include "interface/log.h"

namespace Interface
{
	namespace AFS
	{
		void AfsExtract(int argc, char* argv[], const Log& output);
		void AfsBuild(int argc, char* argv[], const Log& output);
	}
}
