#pragma once

#include <string>
#include "structures/etm.h"
#include "interface/log.h"
#include "interface/argParser.h"

namespace Interface
{
	namespace ETM
	{
		static inline const std::string EtmExtract_help_text{
			"\netm-extract [file.dat] <output directory>\n"
			"etm-extract -r <directory>\n\n"
			"Extracts The Contents Of A ETM File Into A Directory.\n"
			"Optionally You Can Extract All ETM Files In A Directory With The -r Flag.\n\n"
			"Flags:\n"
			"\t-r <dir>\tRecursively Search A Directory For ETM Files And Extract Them. (threaded)\n" };
		static inline const std::string EtmBuild_help_text{
			"\netm-build <directory> [file.dat]\n\n"
			"Builds A ETM File From The Contents Of A Directory.\n"
		};

		void EtmExtract(const ArgParser& parser, const Log& output);
		void EtmBuild(const ArgParser& parser, const Log& output);

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
	}
}
