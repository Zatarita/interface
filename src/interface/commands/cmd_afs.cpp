#include "interface/commands/afs.h"

namespace Interface
{
	namespace AFS
	{
		void AfsExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading AFS File: " + filepath + "\n");
			afs::AfsFile afsFile(filepath);
			output.log(Flags::LOG, "Extracting AFS To: " + outpath + "\n");
			afsFile.saveAll(outpath, true);
		}

		void AfsBuild(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading AFS File: " + filepath + "\n");
			afs::AfsFile afsFile;
			output.log(Flags::LOG, "Extracting AFS To: " + outpath + "\n");
			afsFile.buildAfs(filepath, outpath);
		}
	}
}
