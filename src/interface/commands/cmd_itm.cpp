#include "interface/commands/cmd_itm.h"

namespace Interface
{
	namespace ITM
	{
		void ItmExtractBins(const ArgParser& parser, const Log& output)
		{

			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".ITM", &DoBinExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, ItmExtractBins_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoBinExtraction(filepath, outpath, output);
		}

		void ItmExtractTpls(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".ITM", &DoTplExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, ItmExtractTpls_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoTplExtraction(filepath, outpath, output);
		}

		void ItmExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".ITM", &DoExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, ItmExtract_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoExtraction(filepath, outpath, output);
		}

		void DoTplExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }
			if (std::filesystem::file_size(filepath) == 0) return;

			output.log(Level::LOG, "Loading ITM From: " + filepath.string());
			itm::ItmFile itmFile(filepath.string(), output);
			output.log(Level::LOG, "Extracting ITM To: " + outpath.string());
			itmFile.extractTpls(outpath.string());
		}

		void DoBinExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }
			if (std::filesystem::file_size(filepath) == 0) return;

			output.log(Level::LOG, "Loading ITM From: " + filepath.string());
			itm::ItmFile itmFile(filepath.string(), output);
			output.log(Level::LOG, "Extracting ITM To: " + outpath.string());
			itmFile.extractBins(outpath.string());
		}

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }
			if (std::filesystem::file_size(filepath) == 0) return;

			output.log(Level::LOG, "Loading ITM From: " + filepath.string());
			itm::ItmFile itmFile(filepath.string(), output);
			output.log(Level::LOG, "Extracting ITM To: " + outpath.string());
			itmFile.extractBins(outpath.string());
			itmFile.extractTpls(outpath.string());
		}
	}
	
}
