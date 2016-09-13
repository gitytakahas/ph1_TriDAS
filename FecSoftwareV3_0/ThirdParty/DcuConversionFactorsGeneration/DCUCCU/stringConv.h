//: C19:stringConv.h
// Chuck Allison's string converter
#ifndef STRINGCONV_H
#define STRINGCONV_H
#include <string>
#include <sstream>
#include <iostream>

#if defined XERCES_VERSION_2_3_0 || defined XERCES_VERSION_1_6_0
#include <xercesc/util/XMLString.hpp>
#endif

#ifdef XERCES_VERSION_2_3_0
using namespace XERCES_CPP_NAMESPACE;
#endif

template<typename T>
T fromString(const std::string& s) {
  std::istringstream is(s);
  T t;
  is >> t;
  return t;
}

template<typename T>
T fromHexString(const std::string& s) {
  std::istringstream is(s);
  T t;
  is >> hex >> t;
  return t;
}

template<typename T>
std::string toString(const T& t) {
  std::ostringstream s;
  s << t;
  return s.str();
}

template<typename T>
std::string toHexString(const T& t) {
  std::ostringstream s;
  s << hex << t;
  return s.str();
}

#if defined XERCES_VERSION_2_3_0 || defined XERCES_VERSION_1_6_0
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XMLString::release(&fLocalForm);
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const std::string getString() const
    {
        return std::string(fLocalForm);
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline std::ostream& operator<<(std::ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}
#endif



#endif // STRINGCONV_H
