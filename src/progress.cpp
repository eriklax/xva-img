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

Progress::Progress(const std::string& action)
: m_active(false) , m_curval(0) , m_error(false) , m_action(action)
{
}

Progress::~Progress()
{
	/* somehow Finish() wasn't called */
	if (m_active)
	{
		m_error = true;
		Draw();
	}
}

void Progress::Start()
{
	m_active = true;
	m_curval = 0;
	m_error = false;

	Draw();
}

void Progress::Update(float val){
	m_curval = val;
	Draw();
}

void Progress::Draw()
{
	if (!m_active) return;

	size_t cols = 80;
	printf("\r");

	printf("%s: |", m_action.c_str());
	cols -= m_action.size() + 3;	/* [Extracting: |]	*/
	cols -= 7;						/* [/ 100% ] 		*/

	size_t val = (int)(m_curval * cols/100);
	size_t i = 0;

	for(; i < cols; i++)
	{
		if (i < val)
			printf("=");
		else
			printf(" ");
	}

	char spinner[] = {
		'|',
		'/',
		'-',
		'\\',
	};
	
	if (m_error)
	{
		printf("%c ERROR\n", spinner[(int)m_curval%4], m_curval);
		m_active = false;
	} else {
		printf("%c %3.0lf%%", spinner[(int)m_curval%4], m_curval);

		if (val == cols)
		{
			m_active = false;
			printf("\n");
		}
	}

	fflush(stdout);
}

void Progress::Finish()
{
	m_curval = 100;
	Draw();
}
