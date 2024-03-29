/*
   Copyright (C) 2009 Erik Lax 

   Sparse file support on export added by Jens-U. Mozdzen <jmozdzen@nde.ag>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   In addition, as a special exception, the copyright holders give
   permission to link the code of portions of this program with the
   OpenSSL library under certain conditions as described in each
   individual source file, and distribute linked combinations
   including the two.
   You must obey the GNU General Public License in all respects
   for all of the code used other than OpenSSL. If you modify
   file(s) with this exception, you may extend this exception to your
   version of the file(s), but you are not obligated to do so. If you
   do not wish to do so, delete this exception statement from your
   version. If you delete this exception statement from all source
   files in the program, then also delete it here.
*/

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

#define _VERSION_ "1.5.0"

std::string program_name = "";

void usage(FILE* fp, int status)
{
	fprintf(fp, 
	"xva-img " _VERSION_ " build on " __DATE__
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
	" -p,  --progress            Show progress\n"
	" -s,  --sparse              Create sparse files on export\n"
	" -i,  --ignore-checksum     Only warn on failed checksum\n"
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

	bool verbose = false;
	bool sparse = false;
	bool ignore_checksum = false;
	static struct option longopts[] = { 
		{ "help",		no_argument,		0x0,		'h'	},
		{ "version",		no_argument,		0x0,		'v'	},
		{ "progress",		no_argument,		0x0,		'p'	},
		{ "sparse",		no_argument,		0x0,		's'	},
		{ "ignore-checksum",	no_argument,		0x0,		'i'	},
		{ 0x0,			0,					0x0,		0	}   
	}; 

	opterr = 0;
	optind = 0;
	int ch;
	while ((ch = getopt_long(argc, argv, "hvpis", longopts, 0x0)) != -1) {
		switch(ch)
		{
			case 'p':
				verbose = true;
			break;
			case 's':
				sparse = true;
			break;
			case 'i':
				ignore_checksum = true;
			break;
			case 'h':
				usage(stdout, 0);
			break;
			case 'v':
				printf("%s\n", _VERSION_);
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
			char* outfile = *argv;

			argv++;
			argc--;

			XVA::XVAPackage package;
			if (verbose)
				package.Verbose();
			while(argc > 0)
			{
				struct stat stt;
				if (stat(*argv, &stt) != 0) {
					throw std::runtime_error("cannot stat " +
						std::string(*argv));
				}

				if (stt.st_mode & S_IFREG)
					package.AddFile(*argv);
				else if (stt.st_mode & S_IFDIR)
					package.AddDir(*argv);
				else
					throw std::runtime_error("unknown file type " +
						std::string(*argv));

				argv++;
				argc--;
			}
			package.Write(outfile);
			return 0;
		}

		if (strcmp(*argv, "disk-export") == 0)
		{
			argv++;
			argc--;

			if (argc != 2)
				usage(stderr, 2);

			XVA::Disk disk(*argv);
			if (verbose)
				disk.Verbose();
			if (sparse)
				disk.Sparse();
			if (ignore_checksum)
				disk.IgnoreChecksum();
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
			if (verbose)
				disk.Verbose();
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
