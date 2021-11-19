#pragma once

#include <string>
#include "structures/smd.h"
#include "interface/log.h"
#include "interface/argParser.h"

namespace Interface
{
	namespace SMD
	{
		static inline const std::string SmdExtractBins_help_text{};
		static inline const std::string SmdExtractTpls_help_text{};
		static inline const std::string SmdExtract_help_text{};

		void SmdBinExtract(const ArgParser& parser, const Log& output);
		void SmdTplExtract(const ArgParser& parser, const Log& output);
		void SmdExtract(const ArgParser& parser, const Log& output);

		void DoTplExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
		void DoBinExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output);
	}
}
