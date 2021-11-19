#pragma once
#include <string>
#include "structures/afs.h"
#include "interface/log.h"
#include "interface/argParser.h"

namespace Interface
{
	namespace AFS
	{

		static inline const std::string AfsExtract_help_text{
			"afs-extract [file.afs] <output directory>\n"
			"Extracts The Contents Of An AFS File Into A Directory.\n\n"
			"Flags:\n"
			"\t-i\tIgnore Empty Entries.\n"
		};
		static inline const std::string AfsBuild_help_text{
			"afs-build <content directory> [output.afs]\n"
			"Build An AFS File From The Contents Of A Directory.\n"
		};

		void AfsExtract(const ArgParser& parser, const Log& output);
		void AfsBuild(const ArgParser& parser, const Log& output);
	}
}
