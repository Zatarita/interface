#include "interface/commands/smd.h"

namespace Interface
{
	namespace SMD
	{
		void SmdBinExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading SMD From: " + filepath + "\n");
			smd::SmdFile smdFile(filepath);
			output.log(Flags::LOG, "Saving SMD's TPL To: " + outpath + "\n");
			smdFile.saveTpl(outpath + "/texture.tpl");
		}

		void SmdTplExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading SMD From: " + filepath + "\n");
			smd::SmdFile smdFile(filepath);
			output.log(Flags::LOG, "Saving SMD's TPL To: " + outpath + "\n");
			smdFile.saveTpl(outpath + "/texture.tpl");
		}
	}
}
