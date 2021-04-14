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

#include <stdio.h>
#include <xxhash.h>
#include "xxh64.hpp"

std::string XVA::XXH64(const std::string& input)
{
	XXH64_state_t* const state = XXH64_createState();
	XXH64_hash_t const seed = 0;
	XXH64_reset(state, seed);

	XXH64_update(state, input.c_str(), input.size());
	XXH64_hash_t const int_digest = XXH64_digest(state);

	char digest[8];
	XXH64_canonicalFromHash((XXH64_canonical_t *)digest, int_digest);
	XXH64_freeState(state);

	char digest_chars[16];
	for (int i = 0, j = 0; i < 8; i++) {
		unsigned char c;
		c = (digest[i] >> 4) & 0xf;
		c = (c > 9) ? c + 'A' - 10 : c + '0';
		digest_chars[j++] = c;
		c = (digest[i] & 0xf);
		c = (c > 9) ? c + 'A' - 10 : c + '0';
		digest_chars[j++] = c;
	}
	std::string hexdigest(digest_chars);
	hexdigest.resize(16);
	return hexdigest;
}
