#include "interface/commands/cmd_afs.h"

namespace Interface
{
	namespace AFS
	{
		void AfsExtract(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, AfsExtract_help_text);
				return;
			}

			std::string filepath{ *parser.arg(0) }, outpath{ *parser.arg(1) };

			if (!std::filesystem::exists(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested File!"); return; }

			output.log(Level::LOG, "Loading AFS File: " + filepath + "\n");
			afs::AfsFile afsFile(filepath, output);
			output.log(Level::LOG, "Extracting AFS To: " + outpath + "\n");
			afsFile.extractAll(outpath, parser.hasFlag("i"));
		}

		void AfsBuild(const ArgParser& parser, const Log& output)
		{
			if (parser.argc() < 2)
			{
				output.log(Level::ERROR, AfsBuild_help_text);
				return;
			}

			std::string filepath{ *parser.arg(1) }, outpath{ *parser.arg(0) };

			if (!std::filesystem::is_directory(filepath)) { output.log(Level::ERROR, "Unable To Locate Requested Directory!"); return; }

			output.log(Level::LOG, "Loading AFS File: " + filepath + "\n");
			afs::AfsFile afsFile(output);
			output.log(Level::LOG, "Extracting AFS To: " + outpath + "\n");
			afsFile.buildAfs(filepath, outpath);
		}
	}
}
