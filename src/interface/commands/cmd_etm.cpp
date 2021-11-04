#include "interface/commands/etm.h"

namespace Interface
{
	namespace ETM
	{
		void EtmExtract(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading ETM From: " + filepath + "\n");
			etm::EtmFile etmFile(filepath);
			output.log(Flags::LOG, "Extracting ETM To: " + outpath + "\n");
			etmFile.saveAll(outpath);
		}

	}
}
