#include "interface/commands/dat.h"

namespace Interface
{
	namespace DAT
	{
		void DatExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading DAT File: " + filepath);
			dat::DatFile datFile(filepath);
			output.log(Flags::LOG, "Extracting DAT To: " + outpath);
			datFile.extractAll(outpath);
		}

		void DatBuild(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading Entries From: " + filepath);
			dat::DatFile datFile;
			datFile.addDir(filepath);
			output.log(Flags::LOG, "Saving DAT File To: " + outpath);
			datFile.save(outpath);
		}
	}
}
