#pragma once
#include "commands/cmd_smd.h"
#include "commands/cmd_etm.h"
#include "commands/cmd_itm.h"
#include "commands/cmd_tpl.h"
#include "commands/cmd_dat.h"
#include "commands/cmd_afs.h"
#include "argParser.h"
#include "log.h"

#include <map>
#include <string>
#include <algorithm>

namespace Interface
{
	class CLI
	{
		using cmd =  void (*)(const ArgParser&, const Log&);
		static inline void NO_CMD(const ArgParser&, const Log&) {}

		static inline std::map<std::string, cmd> commands{
			{ "afs-extract",	 Interface::AFS::AfsExtract },
			{ "afs-build",		 Interface::AFS::AfsBuild },
			{ "dat-extract",	 Interface::DAT::DatExtract },
			{ "dat-build",		 Interface::DAT::DatBuild },
			{ "tpl-build",		 Interface::TPL::TplBuild },
			{ "tpl-merge",		 Interface::TPL::TplMerge },
			{ "tpl-extract",	 Interface::TPL::TplExtract },
			{ "tpl-decompile",	 Interface::TPL::TplDecompile },
			{ "smd-extract",	 Interface::SMD::SmdExtract },
			{ "smd-bin-extract", Interface::SMD::SmdBinExtract },
			{ "smd-tpl-extract", Interface::SMD::SmdTplExtract },
			{ "etm-extract",	 Interface::ETM::EtmExtract },
			{ "etm-build",		 Interface::ETM::EtmBuild },
			{ "itm-extract",	 Interface::ITM::ItmExtract },
			{ "itm-bin-extract", Interface::ITM::ItmExtractBins },
			{ "itm-tpl-extract", Interface::ITM::ItmExtractTpls }
		};

		static inline std::map<std::string, Level> traceFlags{
			{"silent" , Level::SILENT },
			{"log"    , Level::LOG },
			{"verbose", Level::VERBOSE },
		};

		static inline const std::string help{
			"(flags) command"
			"\n"
			"Available Flags:\n"
			"-silent\t\tSupress Console Output.\n"
			"-log\t\tPrint Standard Output to Console. \t(default)\n"
			"-verbose\t\tVerbose Output to Console.\n"
			"\n"
			"Available Commands:\n"
			"afs-extract\t[file.afs] <outdir>\t\t\tExtract An AFS File Into Specified Directory\n"
			"afs-build\t<files dir> [file.afs]\t\t\tBuild An AFS File From Specified Directory\n"
			"\n"
			"dat-extract\t[file.dat] <outdir>\t\t\tExtract A DAT File Into Specified Directory.\n"
			"dat-build\t<files dir> [output.dat]\t\tBuild A DAT File Using A Specified Directory\n"
			"\n"
			"tpl-merge \t[output.tpl] [tpl_1.tpl] [tpl_n.tpl]\tMerge TPL Files Together.\n"
			"tpl-extract \t[file.tpl] <outdir>\t\t\tExtract TPL File Into Specified Directory.\n"
			//"tpl-build \t<images dir> (format) [output.tpl]\tBuild TPL File From TGA Images. Formats: 4-bit, 8-bit, 32-bit\n"
			"tpl-decompile \t[file.tpl] <outdir>\t\t\tConvert TPL entries into TGA Files and Save.\n"
			"\n"
			"smd-extract\t[file.smd] <outdir>\t\t\tExtract TPL and BIN Files From SMD\n"
			"smd-bin-extract\t[file.smd] <outdir>\t\t\tExtract BIN Files From SMD\n"
			"smd-tpl-extract\t[file.smd] [output.tpl]\t\t\tExtract TPL File From SMD\n"
			"\n"
			"etm-extract\t[file.etm] <outdir>\t\t\tExtract Contents From ETM Files\n"
			"etm-build\t<dir> [ouput.etm]\t\t\tBuild ETM From Files In Directory\n"
			"\n"
			"itm-extract\t[file.itm] <outdir>\t\t\tExtract TPLs And BINs From ITM Files\n"
			"itm-bin-extract\t[file.itm] <outdir>\t\t\tExtract BINs From ITM Files\n"
			"itm-tpl-extract\t[file.itm] <outdir>\t\t\tExtract TPLs From ITM Files\n"
			};
		Log output;

	public:
		CLI();

		void runCommand(int argc, char* argv[]);
	};
	
}
