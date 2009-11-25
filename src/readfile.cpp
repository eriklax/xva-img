#include <stdio.h>
#include "readfile.hpp"

bool XVA::ReadFile(const std::string& path, std::string& result)
{
	FILE* fp = fopen(path.c_str(), "r");
 
 	if (!fp)
		return false;

	char buf[1024];
	size_t r;
	while((r = fread(buf, 1, sizeof(buf), fp)) > 0)
	{
		result.append(buf, r);
	}
	fclose(fp);

	return true;
}
