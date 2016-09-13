#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <occi.h>
#include <xercesc/framework/MemBufInputSource.hpp>

class Utils {
 public: 
  static void init(std::string file, std::string *fileBuffer){
    char* buffer;
    std::ifstream fileStream((const char *)file.c_str());
    if (fileStream) { 
      // get pointer to associated buffer object
      std::filebuf *pbuf = fileStream.rdbuf();
      
      // get file size using buffer's members
      unsigned int size = pbuf->pubseekoff (0,std::ios::end,std::ios::in);
      pbuf->pubseekpos (0,std::ios::in);
      
      // allocate a character buffer
      buffer=new char[size+1];
      
      // get file data  
      pbuf->sgetn (buffer,size);
      *fileBuffer = std::string(buffer, size);
      fileStream.close();
    } else {
      std::cerr << "unable to open file named " << file << std::endl;
    }
  }

  static unsigned int findStringInBuffer(std::string uploadedBuffer, std::string name){
    int number = 0;
    std::string::size_type index = 0;
    while ((index = uploadedBuffer.find(name, index+1))!= std::string::npos) {
      number++;
    }
  return number;
  }
  
  static unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name){
    unsigned int number = 0;
    XMLByte *xmlBuffer;
    if (xmlClob != NULL){
      unsigned int resultLength = (*xmlClob).length();
      if (resultLength>0) {
	if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char))) != NULL) {
	  if (resultLength == (*xmlClob).read(resultLength, xmlBuffer, resultLength+1)){
	    const std::string xmlBufferId = "myXmlBuffer";
	    std::string downloadedBuffer((const char *)xmlBuffer);
	    std::string::size_type index = 0;
	    while ((index = downloadedBuffer.find(name, index+1))!= std::string::npos) {
	      number++;
	    }
	  } else {
	    std::cerr << "failed while reading the XML Clob"  << std::endl;
	  }
	} else {
	  std::cerr << "failed during memory allocation"  << std::endl;  		  }
      } else {
	std::cerr << "no data found... " << std::endl;
      }
    } else {
    std::cerr << "failed while trying to get the XML Clob"  << std::endl;
    }
    free(xmlBuffer);
    return number;
  }
};
#endif
