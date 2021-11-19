#include "interface/commands/cmd_dat.h"

namespace Interface
{
	namespace DAT
	{
		void DatExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".dat", &DoExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, DatExtract_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoExtraction(filepath, outpath, output);
		}

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }
			if (filepath.extension().string() != ".dat") return;
			if (std::filesystem::file_size(filepath) == 0) return;

			output.log(Level::VERBOSE, "\tFile Opened As Little Endian...");
			le_DatFile le_datFile(output);
			if (le_datFile.loadFromFile(filepath.string()))
			{
				output.log(Level::LOG, "Extracting DAT To: " + outpath.string());
				le_datFile.extractAll(outpath.string());
			}
			else
			{
				output.log(Level::ERROR, "\t\tFailed Validation, Attempting Big Endian...");
				be_DatFile be_datFile(output);

				if (!be_datFile.loadFromFile(filepath.string()))
					output.log(Level::EXCEPTION, EXCEPT_TEXT("\t\tFailed Validation. Cannot Open File"));

				output.log(Level::LOG, "Extracting DAT To: " + outpath.string());
				be_datFile.extractAll(outpath.string());
			}
		}

		void DatBuild(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, DatBuild_help_text);
				return;
			}

			std::string filepath{ *parser.arg(1) }, outpath{ *parser.arg(0) }, flag;
			if (!std::filesystem::is_directory(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested Directory!"); return; }

			output.log(Level::LOG, "Loading Entries From: " + filepath);
			dat::DatFile datFile(output);
			datFile.addDir(filepath);
			output.log(Level::LOG, "Saving DAT File To: " + outpath);
			datFile.save(outpath);
		}
	}
}
