#pragma once

#include <string>
#include "structures/itm.h"
#include "interface/log.h"
#include "interface/argParser.h"

namespace Interface
{
	namespace ITM
	{

		static inline const std::string ItmExtractBins_help_text{};
		static inline const std::string ItmExtractTpls_help_text{};
		static inline const std::string ItmExtract_help_text{};

		void ItmExtractBins(const ArgParser& parser, const Log& output);
		void ItmExtractTpls(const ArgParser& parser, const Log& output);
		void ItmExtract(const ArgParser& parser, const Log& output);

		void DoTplExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
		void DoBinExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
	}
}