/*
   Copyright (C) 2009 Erik Lax 

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
*/
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <set>
#include <errno.h>

#include "readfile.hpp"
#include "writefile.hpp"
#include "sha1.hpp"
#include "progress.hpp"

#include "disk.hpp"

using XVA::Disk;

Disk::Disk(const std::string& path) : m_path(path) , m_verbose(false)
{
}

bool Disk::Export(const std::string& diskpath)
	throw (std::runtime_error)
{
	std::set<unsigned int> parts;

	DIR * dp = opendir(m_path.c_str());
	if (!dp)
		throw std::runtime_error("unable to open " + m_path);

	struct dirent * de;
	while((de=readdir(dp)) != NULL)
	{
		unsigned int part;
		if (strlen(de->d_name) == 8 && sscanf(de->d_name, "%u", &part))
		{
			parts.insert(part);
		}
	}
	closedir(dp);

	if (parts.size() == 0)
		throw std::runtime_error("directory " + m_path + " is empty");

	FILE* fp = fopen(diskpath.c_str(), "w");
	if (!fp)
		throw std::runtime_error("unable to open " + diskpath);

	Progress progress("Exporting");
	if (m_verbose)
		progress.Start();

	for(unsigned int i = 0; i <= (*parts.rbegin()); i++)
	{
		if (m_verbose)
			progress.Update(((float)i / *parts.rbegin())*100);

		if (parts.find(i) != parts.end())
		{
			char path2[9];
			snprintf(path2, sizeof(path2), "%08d", i);

			std::string chunk;
			if (!XVA::ReadFile((m_path + "/" + path2), chunk))
			{
				fclose(fp);
				throw std::runtime_error("unable to open chunk " +
						std::string(path2));
			}

			std::string checksum;
			if (!XVA::ReadFile(
					(m_path + "/" + path2 + ".checksum"),
					checksum)
				)
			{
				fclose(fp);
				throw std::runtime_error("unable to read " +
						std::string(path2) + ".checksum");
			}

			if (checksum != XVA::SHA1(chunk))
			{
				fclose(fp);
				throw std::runtime_error("invalid checksum for " +
						std::string(path2));
			}

			if (fwrite(chunk.c_str(), 1, chunk.size(), fp) != chunk.size())
			{
				fclose(fp);
				throw std::runtime_error("cannot write " +
						std::string(path2) + " to output file");
			}
		} else {
			char bufnull[1048576] = { 0 };
			if (fwrite(bufnull, 1, sizeof(bufnull), fp) != sizeof(bufnull))
			{
				fclose(fp);
				throw std::runtime_error("cannot add empty chunk to " +
						diskpath);
			}
		}
	}

	if (m_verbose)
		progress.Finish();

	fclose(fp);
	return true;
}

bool Disk::Import(const std::string& diskpath)
	throw (std::runtime_error)
{
	DIR * dp = opendir(m_path.c_str());
	if (!dp)
		throw std::runtime_error("unable to open " + m_path);

	size_t files = 0;
	struct dirent * de;
	while((de=readdir(dp)) != NULL)
	{
		if (strcmp(de->d_name, ".") == 0) continue;
		if (strcmp(de->d_name, "..") == 0) continue;
		files++;
	}
	closedir(dp);

	if (files != 0)
		throw std::runtime_error("directory " + m_path + " is not empty");

	FILE* fp = fopen(diskpath.c_str(), "r");
	if (!fp)
		throw std::runtime_error("unable to open " + diskpath);

	char buf[1048576];
	fseek(fp, 0, SEEK_END);
	unsigned long long size = ftello(fp);
	fseek(fp, 0, SEEK_SET);

	Progress progress("Importing");
	if (m_verbose)
		progress.Start();

	char bufnull[1048576] = { 0 };
	for (size_t i = 0; ; i++)
	{
		if (m_verbose)
			progress.Update(((float)ftello(fp) / size)*100);

		fread(buf, 1, sizeof(buf), fp);
		if (feof(fp)) break;

		/* skip empty chunks */
		if ((i > 0 && ftello(fp) < (off_t)size)
			&& 
			memcmp(buf, bufnull, sizeof(bufnull)) == 0)
		{
			continue;
		}

		char path2[9];
		snprintf(path2, sizeof(path2), "%08lu", i);

		std::string output;
		output.append(buf, 1048576);

		if (!XVA::WriteFile(m_path + "/" + path2, output))
		{
			fclose(fp);
			throw std::runtime_error("cannot write chunk " +
					std::string(path2));
		}

		if (!XVA::WriteFile(m_path + "/" + path2 + ".checksum",
				XVA::SHA1(output)
			))
		{
			fclose(fp);
			throw std::runtime_error("cannot write checksum for " +
					std::string(path2));
		}
	}

	if (m_verbose)
		progress.Finish();

	fclose(fp);
	return true;
}

void Disk::Verbose()
{
	m_verbose = true;
}
