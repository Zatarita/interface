#pragma once
#include "commands/smd.h";
#include "commands/etm.h";
#include "commands/itm.h";
#include "commands/tpl.h";
#include "commands/dat.h";
#include "commands/afs.h";
#include "log.h"

#include <map>
#include <string>
#include <algorithm>

namespace Interface
{
	class CLI
	{
		using cmd =  void (*)(int, char**, const Log&);
		static inline void NO_CMD(int argc, char* argv[], const Log&) {}

		static inline std::map<std::string, cmd> commands{
			{ "afs-extract",	 Interface::AFS::AfsExtract },
			{ "afs-build",		 Interface::AFS::AfsBuild },
			{ "dat-extract",	 Interface::DAT::DatExtract },
			{ "dat-build",		 Interface::DAT::DatBuild },
			{ "tpl-build",		 Interface::TPL::TplBuild },
			{ "tpl-merge",		 Interface::TPL::TplMerge },
			{ "tpl-extract",	 Interface::TPL::TplExtract },
			{ "smd-bin-extract", Interface::SMD::SmdBinExtract },
			{ "smd-tpl-extract", Interface::SMD::SmdTplExtract },
			{ "etm-extract",	 Interface::ETM::EtmExtract },
			{ "itm-bin-extract", Interface::ITM::ItmExtractBins },
			{ "itm-tpl-extract", Interface::ITM::ItmExtractTpls }
		};

		static inline std::map<std::string, Flags> flags{
			{"-silent" , Flags::SILENT },
			{"-log"    , Flags::LOG },
			{"-verbose", Flags::VERBOSE },
		};

		static inline const std::string help{
			"(flags) command"
			"\n"
			"Available Flags:\n"
			"\t-silent\t\tSupress Console Output.\n"
			"\t-log\t\tPrint Standard Output to Console. \t(default)\n"
			"\t-trace\t\tVerbose Output to Console.\n"
			"\n"
			"Available Commands:\n"
			"\tafs-extract\t[file.afs] <outdir>\t\t\tExtract An AFS File Into Specified Directory\n"
			"\tafs-build\t<files dir> [file.afs]\t\t\tBuild An AFS File From Specified Directory\n"
			"\n"
			"\tdat-extract\t[file.dat] <outdir>\t\t\tExtract A DAT File Into Specified Directory\n"
			"\tdat-build\t<files dir> [output.dat]\t\tBuild A DAT File Using A Specified Directory\n"
			"\n"
			"\ttpl-build \t<images dir> [output.tpl]\t\tBuild TPL File From TGA Images.\n"
			"\ttpl-merge \t[output.tpl] [tpl_1.tpl] [tpl_2.tpl] <..>\tMerge TPL Files Together.\n"
			"\ttpl-extract \t[file.tpl] <outdir>\t\t\tExtract TPL File Into Specified Directory.\n"
			"\n"
			"\tsmd-bin-extract\t[file.smd] <outdir>\t\t\tExtract BIN Files From SMD\n"
			"\tsmd-tpl-extract\t[file.smd] [output.tpl]\t\t\tExtract TPL File From SMD\n"
			"\n"
			"\tetm-extract\t[file.etm] <outdir>\t\t\tExtract Contents From ETM Files\n"
			"\n"
			"\titm-bin-extract\t[file.itm] <outdir>\t\t\tExtract BINs From ITM Files\n"
			"\titm-tpl-extract\t[file.itm] <outdir>\t\t\tExtract TPLs From ITM Files\n"
			};
		Log output;

	public:
		void runCommand(int argc, char* argv[]);
	};
	
}
