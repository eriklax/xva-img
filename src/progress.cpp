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
#include "progress.hpp"

using XVA::Progress;

Progress::Progress() : m_active(false) , m_curval(0)
{

}

Progress::~Progress()
{
	if (m_active)
		Finish(true);
}

void Progress::Start()
{
	m_active = true;
	m_curval = 0;

	printf("[");
	fflush(stdout);
}

void Progress::Update(float val)
{
	val = (int)(val * 78/100);
	for(; m_curval < val; m_curval++)
		printf("-");
	fflush(stdout);
}

void Progress::Finish(bool error)
{
	for(; m_curval < 78; m_curval++)
	{
		if (error)
			printf("x");
		else
			printf("-");
	}
	m_active = false;
	printf("]\n");
	fflush(stdout);
}
