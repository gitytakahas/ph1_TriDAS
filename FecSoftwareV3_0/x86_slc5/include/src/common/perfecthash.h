/* C++ code produced by gperf version 3.0.1 */
/* Command-line: 'D:\\Application\\gperf\\bin\\gperf' -CGD -N IsValidCommandLineOption -K Option -L C++ -t CommandLineOptions.gperf  */
/* Computed positions: -k'$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "CommandLineOptions.gperf"

#include "CommandLineOptions.h"
#include <string.h>
typedef struct CommandLineOptionCode CommandLineOptionCode;
#line 6 "CommandLineOptions.gperf"
struct CommandLineOption
  {
  const char *Option;
  int OptionCode;
  };

#define TOTAL_KEYWORDS 24
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 22
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 36
/* maximum key range = 35, duplicates = 0 */

class Perfect_Hash
{
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static const struct CommandLineOption *IsValidCommandLineOption (const char *str, unsigned int len);
};

inline unsigned int
Perfect_Hash::hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      25,  0, 37, 37, 30, 18, 37, 37, 37, 37,
       0,  8, 25, 37, 15,  5,  0, 37, 10, 37,
      37,  0, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
      37, 37, 37, 37, 37, 37
    };
  return len + asso_values[(unsigned char)str[len - 1]];
}

static const struct CommandLineOption wordlist[] =
  {
#line 18 "CommandLineOptions.gperf"
    {"-t", CommandLineOptionCode::ANALYSIS_TYPE},
#line 32 "CommandLineOptions.gperf"
    {"-rn", CommandLineOptionCode::PRINTED_ROWS_NUMBER},
#line 26 "CommandLineOptions.gperf"
    {"-gct", CommandLineOptionCode::GET_ANALYSIS_TYPE},
#line 23 "CommandLineOptions.gperf"
    {"--run", CommandLineOptionCode::RUNNUMBER},
#line 29 "CommandLineOptions.gperf"
    {"--input", CommandLineOptionCode::INPUT},
#line 31 "CommandLineOptions.gperf"
    {"--output", CommandLineOptionCode::OUTPUT},
#line 25 "CommandLineOptions.gperf"
    {"--version", CommandLineOptionCode::VERSION},
#line 30 "CommandLineOptions.gperf"
    {"-o", CommandLineOptionCode::OUTPUT},
#line 21 "CommandLineOptions.gperf"
    {"--partition", CommandLineOptionCode::PARTITION},
#line 24 "CommandLineOptions.gperf"
    {"-v", CommandLineOptionCode::VERSION},
#line 34 "CommandLineOptions.gperf"
    {"-dv", CommandLineOptionCode::DOWNLOAD_ANALYSISVERSIONS},
#line 19 "CommandLineOptions.gperf"
    {"--analysis-type", CommandLineOptionCode::ANALYSIS_TYPE},
#line 22 "CommandLineOptions.gperf"
    {"-r", CommandLineOptionCode::RUNNUMBER},
#line 17 "CommandLineOptions.gperf"
    {"--download-history", CommandLineOptionCode::DOWNLOAD_HISTORY},
#line 27 "CommandLineOptions.gperf"
    {"--get-analysis-type", CommandLineOptionCode::GET_ANALYSIS_TYPE},
#line 28 "CommandLineOptions.gperf"
    {"-i", CommandLineOptionCode::INPUT},
#line 15 "CommandLineOptions.gperf"
    {"--download-description", CommandLineOptionCode::DOWNLOAD_DESCRIPTION},
#line 35 "CommandLineOptions.gperf"
    {"-download-versions", CommandLineOptionCode::DOWNLOAD_ANALYSISVERSIONS},
#line 20 "CommandLineOptions.gperf"
    {"-p", CommandLineOptionCode::PARTITION},
#line 14 "CommandLineOptions.gperf"
    {"-dd", CommandLineOptionCode::DOWNLOAD_DESCRIPTION},
#line 13 "CommandLineOptions.gperf"
    {"--help", CommandLineOptionCode::HELP},
#line 12 "CommandLineOptions.gperf"
    {"-h", CommandLineOptionCode::HELP},
#line 16 "CommandLineOptions.gperf"
    {"-dh", CommandLineOptionCode::DOWNLOAD_HISTORY},
#line 33 "CommandLineOptions.gperf"
    {"--printed_rows_number", CommandLineOptionCode::PRINTED_ROWS_NUMBER}
  };

static const signed char lookup[] =
  {
    -1, -1,  0,  1,  2,  3, -1,  4,  5,  6,  7,  8,  9, 10,
    -1, 11, -1, 12, 13, 14, 15, -1, 16, 17, -1, -1, -1, 18,
    19, -1, -1, 20, 21, 22, -1, -1, 23
  };

const struct CommandLineOption *
Perfect_Hash::IsValidCommandLineOption (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].Option;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
#line 36 "CommandLineOptions.gperf"

