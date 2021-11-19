#include "interface/commands/cmd_tpl.h"


namespace Interface
{
	namespace TPL
	{
		void TplBuild(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, TplBuild_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, path{ *parser.arg(1) };

			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Building TPL File From: " + path);
			tpl::TplFile tplFile(output);

			for (const auto& entry : getSequentialNumericFilenames(path))
			{
				tpl::TplFile concatFile(entry, output);
				tplFile += concatFile;
			}

			output.log(Level::LOG, "Saving TPL File To: " + filepath);
			tplFile.save(filepath);
		}

		void TplMerge(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, TplMerge_help_text);
				return;
			}

			std::string outfile{ *parser.arg(0) };

			output.log(Level::LOG, "Concatinating TPL Files\n");
			tpl::TplFile tplFile(output);
			for (auto& arg : parser.getArgs(1))
			{
				tpl::TplFile concatFile(arg, output);
				tplFile += concatFile;
			}

			output.log(Level::LOG, "Saving TPL File To: " + outfile);
			tplFile.save(outfile);
		}

		void TplExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, TplBuild_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading TPL From: " + filepath);
			tpl::TplFile tplFile(filepath, output);
			output.log(Level::LOG, "Extracting TPL Files To: " + outpath);
			tplFile.extractAll(outpath);
		}

		void TplDecompile(const ArgParser& parser, const Log& output)
		{
			if (parser.hasFlag("r") || parser.hasFlag("recursive"))
			{
				RecursiveExtract(parser, output, ".TPL", &DoDecompile);
				return;
			}

			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, TplBuild_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			DoDecompile(filepath, outpath, output);
		}

		void DoDecompile(const std::filesystem::path& filepath, const std::filesystem::path& outpath, const Log& output)
		{
			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading TPL From: " + filepath.string());
			tpl::TplFile tplFile(filepath.string(), output);
			output.log(Level::LOG, "Extracting TPL Files To: " + outpath.string());
			tplFile.decompileAll(outpath.string() + "/");
		}
	}
}
