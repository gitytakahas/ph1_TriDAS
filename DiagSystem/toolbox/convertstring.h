/* convertstring.h

   Giordano Cerizza: helper class to convert different
                 types into strings to reduce the overhead
                 to build an error message.
   18.5.2007  for numbers (integer, float..)
   21.5.2007  add handling of characters
*/

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

class BadConversion : public std::runtime_error {
 public:
  BadConversion(const std::string& s)
    : std::runtime_error(s)
    { }
};
 
inline std::string stringF(unsigned long x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(unsigned long)");
  return o.str();
}

inline std::string stringF(unsigned int x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(unsigned int)");
  return o.str();
}

inline std::string stringF(int x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(int)");
  return o.str();
}

inline std::string stringF(double x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(double)");
  return o.str();
}

inline std::string stringF(float x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(float)");
  return o.str();
}

inline std::string stringF(const char* x)
{
  std::ostringstream o;
  if (!(o << x))
    throw BadConversion("stringF(char)");
  return o.str();
}

