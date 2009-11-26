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
