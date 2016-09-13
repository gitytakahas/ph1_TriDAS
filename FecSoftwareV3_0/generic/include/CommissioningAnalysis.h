
//---------------------- includes ---------------------------- //
#include <sstream> 
#include <string> 


//------------------------ types ----------------------------- //
#ifndef COMMISSIONINGANALYSIS_H
#define COMMISSIONINGANALYSIS_H

#define PARTITIONNAMESIZE 100
#define MAXIMUMSTRINGSIZE 255

typedef char           c_string[PARTITIONNAMESIZE];
typedef char           max_string[MAXIMUMSTRINGSIZE];

typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef int             int32_t;

#include <vector>
typedef std::vector<uint16_t> VInt16;

// see PkgAnalysis.spec::StringSeparator ; must be same separator character
const char CONCATENED_STRING_CHARACTER_SEPARATOR = ',';

#endif
