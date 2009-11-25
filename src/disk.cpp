#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <set>
#include <errno.h>

#include "readfile.hpp"
#include "writefile.hpp"
#include "sha1.hpp"

#include "disk.hpp"

using XVA::Disk;

Disk::Disk(const std::string& path) : m_path(path)
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

	for(unsigned int i = 0; i <= (*parts.rbegin()); i++)
	{
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
			if (fseek(fp, 1048576, SEEK_CUR) != 0)
			{
				fclose(fp);
				throw std::runtime_error("cannot add empty chunk to " +
						diskpath);
			}
		}
	}

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
	for (size_t i = 0; ; i++)
	{
		size_t r = fread(buf, 1, sizeof(buf), fp);
		if (feof(fp)) break;

		char path2[9];
		snprintf(path2, sizeof(path2), "%08d", i);

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

	fclose(fp);
	return true;
}
