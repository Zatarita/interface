#pragma once

#include <string>
#include "structures/dat.h"
#include "interface/log.h"
#include "interface/argParser.h"
#include "thread_pool.hpp"
#include "interface/common.h"

namespace Interface
{
	namespace DAT
	{
		static inline const std::string DatExtract_help_text{
			"\ndat-extract [file.dat] <output directory>\n"
			"dat-extract -r <directory>\n\n"
			"Extracts The Contents Of A DAT File Into A Directory.\n"
			"Optionally You Can Extract All DAT Files In A Directory With The -r Flag.\n\n"
			"Flags:\n"
			"\t-r <dir>\tRecursively Search A Directory For DAT Files And Extract Them. (threaded)\n"
		};
		static inline const std::string DatBuild_help_text{
			"\ndat-build <directory> [file.dat]\n\n"
			"Builds A DAT File From The Contents Of A Directory.\n"
			"Filenames MUST Be Numbers, And The Extension Must Be The Format Type. (Eg 0.EFF, 1.TPL, 3.BIN)\n"
		};

		void DatExtract(const ArgParser& parser, const Log& output);
		void DatBuild(const ArgParser& parser, const Log& output);

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
	}
}
