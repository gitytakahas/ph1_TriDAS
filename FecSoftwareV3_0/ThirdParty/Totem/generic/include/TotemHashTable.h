
#ifndef TOTEMFECHASHTABLE_H
#define TOTEMFECHASHTABLE_H

#include <vector>

#include "hashMapDefinition.h"

#include "tscTypes.h"
#include "keyType.h"

#include "vfatAccess.h"
#include "totemCChipAccess.h"
#include "totemBBAccess.h"

// All the map needed for the FecSupervisor program and threads program
// Create new type in order to store the access of each device type

// Hash table for Vfat
typedef Sgi::hash_map<keyType, vfatAccess *> vfatAccessedType ;

// Hash table for CChip
typedef Sgi::hash_map<keyType, totemCChipAccess *> cchipAccessedType ;

// Hash table for TBB
typedef Sgi::hash_map<keyType, totemBBAccess *> tbbAccessedType ;

#endif
