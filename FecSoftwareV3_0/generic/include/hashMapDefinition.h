/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef HASHMAPDEFINITION_H
#define HASHMAPDEFINITION_H
#include <string.h>
// Compatibility with the G++ version 3.X
// Retreive from http://gcc.gnu.org/onlinedocs/libstdc++/faq/
#ifdef __GNUC__
#define GNUCV  __GNUC__*10+__GNUC_MINOR__
//#if __GNUC__ < 
#if GNUCV < 30
#include <hash_map>
namespace Sgi { using ::hash_map; }; // inherit globals
#elif GNUCV < 44
#include <ext/hash_map>
//#if __GNUC_MINOR__ == 0
#if GNUCV == 30
namespace Sgi = std;               // GCC 3.0
#else
namespace Sgi = ::__gnu_cxx;       // GCC 3.1 and later
#endif
#else
#include <unordered_map>
namespace Sgi = std;
#define hash_map unordered_map
#endif
#else      // ...  there are other compilers, right?
namespace Sgi = std;
#endif

// To have hash_map hash_map<const char *, ...>
// Please remarks that hash_map<std::string, ...> does not work at all. 
// Since hash_map is not part of the standard STL, there are certain other parts of the STL that it 
// doesn't work with (notably the string template, which is pretty much standard across all STL implementations). 
// You need to supply some extra code yourself, for the hash function to work correctly with other parts of the STL, 
// which are standard across all implementations.
#if  GNUCV < 44
struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};
#else
struct eqstr {
    inline size_t operator()(const char *s) const {
        size_t hash = 1;
        for (; *s; ++s) hash = hash * 5 + *s;
        return hash;
    }
    inline bool operator()(const char *s1, const char *s2) const {
        return strcmp(s1, s2) == 0;
    }
};

#endif


#endif
