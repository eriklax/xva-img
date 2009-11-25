#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <getopt.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "package.hpp"
#include "disk.hpp"

#define _VERSION_ "1.0"

std::string program_name = "";

void usage(FILE* fp, int status)
{
	fprintf(fp, 
	"xentool " _VERSION_ " build on " __DATE__
		" (c) Erik Lax <erik@datahack.se>\n"
	"Usage: %s [OPTION...] COMMAND ...\n"
	"\n"
	" Commands\n"
	"\n"
	" package      <xva-file> <file> [ ,<file>... ]\n"
	" disk-export  <xva-disk> <disk-image>\n"
	" disk-import  <xva-disk> <disk-image>\n"
	"\n"
	" Options\n"
	"\n"
	" -h,  --help                Show this help\n"
	" -v,  --version             Show version\n"
	"\n",
	program_name.c_str()
	);
	exit(status);
}

int main(int argc, char* argv[])
{
	program_name = std::string(argv[0]);

	if (argc < 2)
		usage(stderr, 2);

	static struct option longopts[] = { 
		{ "help",		no_argument,		0x0,		'h'	},
		{ "version",	no_argument,		0x0,		'v'	},
		{ 0x0,			0,					0x0,		0	}   
	}; 

	opterr = 0;
	optind = 0;
	int ch;
	while ((ch = getopt_long(argc, argv, "hv", longopts, 0x0)) != -1) {
		switch(ch)
		{
			case 'h':
				usage(stdout, 0);
				break;
			case 'v':
				printf("%s\n", __VERSION__);
				exit(0);
				break;
			case 0: break;
			default:
					usage(stderr, 2);
					break;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage(stderr, 2);

	try {
		if (strcmp(*argv, "package") == 0)
		{
			argv++;
			argc--;
			char * outfile = *argv;

			argv++;
			argc--;

			XVA::XVAPackage packet;
			while(argc > 0)
			{
				struct stat stt;
				if (stat(*argv, &stt) != 0) {
					throw std::runtime_error("cannot stat " +
						std::string(*argv));
				}

				if (stt.st_mode & S_IFREG)
					packet.AddFile(*argv);
				else if (stt.st_mode & S_IFDIR)
					packet.AddDir(*argv);
				else
					throw std::runtime_error("unknown file type " +
						std::string(*argv));

				argv++;
				argc--;
			}
			packet.Write(outfile);
			return 0;
		}

		if (strcmp(*argv, "disk-export") == 0)
		{
			argv++;
			argc--;

			if (argc != 2)
				usage(stderr, 2);

			XVA::Disk disk(*argv);
			argv++;
			argc--;
			disk.Export(*argv);
			return 0;
		}

		if (strcmp(*argv, "disk-import") == 0)
		{
			argv++;
			argc--;

			if (argc != 2)
				usage(stderr, 2);

			XVA::Disk disk(*argv);
			argv++;
			argc--;
			disk.Import(*argv);
			return 0;
		}

		usage(stderr, 2);
	} catch (std::runtime_error& e) {
		fprintf(stderr, "%s: %s\n", program_name.c_str(), e.what());
		exit(1);
	}

	return 0;
}
