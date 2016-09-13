//: C19:stringConv.h
// Chuck Allison's string converter
#ifndef STRINGCONV_H
#define STRINGCONV_H

//util/XercesVersion.hpp:#define XERCES_VERSION_MAJOR 2
//util/XercesVersion.hpp:#define XERCES_VERSION_MINOR 7

#include <string>
#include <sstream>
#include <iostream>
#include <xercesc/util/XMLString.hpp>

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
  is >> std::hex >> t;
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
  s << std::hex << t;
  return s.str();
}

template<typename T>
std::string toHEXString(const T& t) {
  std::ostringstream s;
  s.setf(std::ios::uppercase);
  s << std::hex << t;
  return s.str();
}

template<typename T>
std::string toBINString(const T& t, unsigned int nBits) {
  std::ostringstream s;
  s.setf(std::ios::uppercase);

  if (nBits > 64)
    s << "TOO MANY BITS" ;
  else {
    int val[65] = {0} ;
    for (unsigned int i = 0 ; i < nBits ; i ++)
      if (t & (1 << i)) val[i] = 1 ;
    for (unsigned int i = (nBits-1) ; i > 0 ; i --) 
      s << std::hex << val[i];
  }

  return s.str();
}

class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XERCES_CPP_NAMESPACE::XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
      XERCES_CPP_NAMESPACE::XMLString::release(&fLocalForm);
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

    // -----------------------------------------------------------------------
    //  Transform to lower case
    // -----------------------------------------------------------------------
    /** Convert a string to lower case, indeed, myString needs to be a copy of the original string
     * \param myString - string to be converted
     * \return the string in lower case
     */
    static std::string StringToLower(std::string myString) {
      
      const int length = myString.length();
      for(int i=0; i!=length; ++i) 
	myString[i] = std::tolower(myString[i]);
      return myString;
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

#endif // ParameterDescription::STRINGCONV_H
