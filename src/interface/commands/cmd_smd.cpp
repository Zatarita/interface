#include "interface/commands/cmd_smd.h"

namespace Interface
{
	namespace SMD
	{
		void SmdBinExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".SMD", &DoBinExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, SmdExtractBins_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoBinExtraction(filepath, outpath, output);
		}

		void SmdTplExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".SMD", &DoTplExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, SmdExtractTpls_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoTplExtraction(filepath, outpath, output);
		}

		void SmdExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".SMD", &DoExtraction);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, SmdExtractTpls_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoExtraction(filepath, outpath, output);
		}

		void DoTplExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output) 
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading SMD File..." + filepath.string());
			smd::SmdFile<std::endian::little> le_smdFile(output);
			if (le_smdFile.loadFromFile(filepath.string()))
			{
				output.log(Level::LOG, "Extracting SMD To: " + outpath.string());
				le_smdFile.saveTpl(outpath.string() + "/texture.tpl");
			}
			else { output.log(Level::ERROR, "Unable To Load SMD!"); return; }
		}

		void DoBinExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output) 
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading SMD File..." + filepath.string());
			smd::SmdFile<std::endian::little> le_smdFile(output);
			if (le_smdFile.loadFromFile(filepath.string()))
			{
				output.log(Level::LOG, "Extracting SMD To: " + outpath.string());
				le_smdFile.extract(outpath.string());
			}
			else { output.log(Level::ERROR, "Unable To Load SMD!"); return; }
		}

		void DoExtraction(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading SMD File..." + filepath.string());
			smd::SmdFile<std::endian::little> le_smdFile(output);
			if (le_smdFile.loadFromFile(filepath.string()))
			{
				output.log(Level::LOG, "Extracting SMD To: " + outpath.string());
				le_smdFile.extract(outpath.string());
				le_smdFile.saveTpl(outpath.string() + "/texture.tpl");
			}
			else { output.log(Level::ERROR, "Unable To Load SMD!"); return; }
		}

	}
}
