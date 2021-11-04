#include "interface/commands/tpl.h"


namespace Interface
{
	namespace TPL
	{
		void TplBuild(int argc, char* argv[], const Log& output)
		{
			std::string outfile{ argv[1] }, path{ argv[0] };

			output.log(Flags::LOG, "Building TPL File From: " + path + "\n");
			tpl::TplFile tplFile;

			for (const auto& entry : getSequentialNumericFilenames(path))
			{
				tpl::TplFile concatFile(entry);
				tplFile += concatFile;
			}

			output.log(Flags::LOG, "Saving TPL File To: " + outfile + "\n");
			tplFile.save(outfile);
		}

		void TplMerge(int argc, char* argv[], const Log& output)
		{
			std::string outfile{ argv[0] };

			output.log(Flags::LOG, "Concatinating TPL Files\n");
			tpl::TplFile tplFile;
			for (int i = 1; i < argc; i++)
			{
				tpl::TplFile concatFile(argv[i]);
				tplFile += concatFile;
			}

			output.log(Flags::LOG, "Saving TPL File To: " + outfile + "\n");
			tplFile.save(outfile);
		}

		void TplExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading TPL From: " + filepath + "\n");
			tpl::TplFile tplFile(filepath);
			output.log(Flags::LOG, "Extracting TPL Files To: " + outpath + "\n");
			tplFile.extractAll(outpath);
		}
	}
}
