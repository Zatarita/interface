#include "interface/commands/cmd_etm.h"

namespace Interface
{
	namespace ETM
	{
		void EtmExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".ETM", &DoExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, EtmExtract_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoExtraction(filepath, outpath, output);
		}

		void EtmBuild(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, EtmExtract_help_text);
				return;
			}

			std::string filepath{ *parser.arg(1) }, outpath{ *parser.arg(0) };

			if (!std::filesystem::is_directory(filepath)) output.log(Level::ERROR, "Unable To Locate Requested Directory!");

			output.log(Level::LOG, "Creating ETM");
			etm::EtmFile etmFile(output);
			output.log(Level::LOG, "Loading Entries From Dir: " + filepath);
			etmFile.addDir(filepath);
			output.log(Level::LOG, "Saving ETM To: " + outpath);
			etmFile.saveToFile(outpath);
		}

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }
			if (filepath.extension().string() != ".ETM") return;
			if (std::filesystem::file_size(filepath) == 0) return;

			output.log(Level::LOG, "Loading ETM From: " + filepath.string());
			etm::EtmFile etmFile(filepath.string(), output);
			output.log(Level::LOG, "Extracting ETM To: " + outpath.string());
			etmFile.extractAll(outpath.string());
		}
	}
}
