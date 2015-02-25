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

#include <sys/types.h>
#include <dirent.h>
#include <set>
#include <stdio.h>
#include <string.h>

#include "writefile.hpp"
#include "readfile.hpp"
#include "progress.hpp"

#include "package.hpp"

using XVA::XVAPackage;

XVAPackage::XVAPackage()
: m_verbose(false)
{
}

/*
 * Add a file to the list 
 */
void XVAPackage::AddFile(const std::string& file)
{
	m_files.push_back(file);
}

/*
 * Add all files in a directory to the file list
 */
void XVAPackage::AddDir(const std::string& path)
	throw (std::runtime_error)
{
	DIR* dp = opendir(path.c_str());
	if (!dp)
		throw std::runtime_error("unable to open " + path);

	std::set<std::string> list;

	bool slash = true;
	if (path.substr(path.size()-1) == "/")
		slash = false;

	struct dirent* de;
	while((de=readdir(dp)) != NULL)
	{
		if (strcmp(de->d_name, ".") == 0) continue;
		if (strcmp(de->d_name, "..") == 0) continue;
		list.insert(path + (slash?"/":"") + de->d_name);
	}
	closedir(dp);

	for(std::set<std::string>::const_iterator i = list.begin();
			i != list.end(); ++i)
	{
		m_files.push_back(*i);
	}
}

/*
 * Write the XVA Package to file
 */
bool XVAPackage::Write(const std::string& file)
	throw (std::runtime_error)
{
	FILE* fp = fopen(file.c_str(), "w");
	if (!fp)
		throw std::runtime_error("unable to open " + file);

	Progress progress("Packing");
	if (m_verbose)
		progress.Start();

	size_t x = 0;
	for(std::list<std::string>::const_iterator i = m_files.begin();
			i != m_files.end(); ++i, ++x)
	{
		if (m_verbose)
			progress.Update(((float)x / m_files.size())*100);

		std::string data;
		if (!XVA::ReadFile(*i, data))
		{
			fclose(fp);
			throw std::runtime_error("unable to open file " +
						*i);
		}

		/*
		 * we should remove all paths up to ova.xml and Ref:4/ for disks 
		 * this code is subject to be changed, when new types of files
		 * are added to the xva file!
		 */
		const char* file = strrchr(i->c_str(), '/');
		if (file != NULL)
		{
			file++;
			if (strcmp(file, "ova.xml") == 0)
			{ /* ova.xml file .. */ } else {

				int len = file - i->c_str();

				char* str = new char[len];
				strncpy(str, i->c_str(), len - 1);
				str[len - 1] = '\0';

				if (strrchr(str, '/') != NULL)
				{
					int len = strrchr(str, '/') - str;
					file = i->c_str() + len + 1;
				}
				delete [] str;
			}
		}

		/*
		 * build a tar-header
		 */
		char header[512] = { 0 };
		char* ptr = header;
		snprintf(ptr, 100, "%s", file);
		ptr += 100;
		snprintf(ptr, 8, "%07d", 0);
		ptr += 8;
		snprintf(ptr, 8, "%07d", 0);
		ptr += 8;
		snprintf(ptr, 8, "%07d", 0);
		ptr += 8;
		snprintf(ptr, 12, "%011lo", data.size());
		ptr += 12;
		snprintf(ptr, 12, "%011d", 0);
		ptr += 12;

		memset(ptr, ' ', 8);
		unsigned int checksum = 0;
		for(int x=0; x < 512; x++)
			checksum += header[x];
		snprintf(ptr, 8, "%07o", checksum);
		ptr += 8;

		if (fwrite(header, 1, 512, fp) != 512)
		{
			fclose(fp);
			throw std::runtime_error("unable to write header for " +
						*i);
		}

		if (fwrite(data.c_str(), 1, data.size(), fp) != data.size())
		{
			fclose(fp);
			throw std::runtime_error("unable to write data for " +
						*i);
		}

		size_t pad = 512-(data.size()%512);
		if (pad < 512)
		{
			char null[512] = { 0 };
			if (fwrite(null, 1, pad, fp) != pad)
			{
				fclose(fp);
				throw std::runtime_error("unable to write pad for " +
						*i);
			}
		}
	}

	char null[1024] = { 0 };
	if (fwrite(null, 1, sizeof null, fp) != sizeof null)
	{
		fclose(fp);
		throw std::runtime_error("unable to footer");
	}

	if (m_verbose)
		progress.Finish();

	fclose(fp);
	return true;
}

void XVAPackage::Verbose()
{
	m_verbose = true;
}
