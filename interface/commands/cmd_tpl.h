#pragma once

#include <string>
#include "structures/tpl.h"
#include "interface/log.h"
#include "interface/argParser.h"

namespace Interface
{
	namespace TPL
	{
		static inline const std::string TplBuild_help_text{};
		static inline const std::string TplMerge_help_text{};
		static inline const std::string TplExtract_help_text{};
		static inline const std::string TplDecompile_help_text{};

		void TplBuild(const ArgParser& parser, const Log& output);
		void TplMerge(const ArgParser& parser, const Log& output);
		void TplExtract(const ArgParser& parser, const Log& output);
		void TplDecompile(const ArgParser& parser, const Log& output);

		void DoDecompile(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
	}
}
