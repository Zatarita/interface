#include "interface/commands/itm.h"

namespace Interface
{
	namespace ITM
	{
		void ItmExtractBins(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading ITM From: " + filepath + "\n");
			itm::ItmFile itmFile(filepath);
			output.log(Flags::LOG, "Extracting ITM To: " + outpath + "\n");
			itmFile.extractBins(outpath);
		}


		void ItmExtractTpls(int argc, char* argv[], const Log& output)
		{
			std::string filepath{ argv[0] }, outpath{ argv[1] };

			output.log(Flags::LOG, "Loading ITM From: " + filepath + "\n");
			itm::ItmFile itmFile(filepath);
			output.log(Flags::LOG, "Extracting ITM To: " + outpath + "\n");
			itmFile.extractTpls(outpath);
		}
	}
	
}
