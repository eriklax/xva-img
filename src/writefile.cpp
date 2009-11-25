#include <stdio.h>
#include "writefile.hpp"

bool XVA::WriteFile(const std::string& path, const std::string& data)
{
	FILE* fp = fopen(path.c_str(), "w");
 
 	if (!fp)
		return false;

	if (fwrite(data.c_str(), 1, data.size(), fp) != data.size())
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}
