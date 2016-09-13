#ifndef DBTKDCUCONVERSIONACCESS_TEST_H
#define DBTKDCUCONVERSIONACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbTkDcuConversionAccess.h"
#include "DbTkDcuInfoAccess.h"
#include "XMLTkDcuConversion.h"
#include "XMLTkDcuInfo.h"

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbTkDcuConversionAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbTkDcuConversionAccessTest );
  //CPPUNIT_TEST( uploadTest );  
  //CPPUNIT_TEST( downloadTest );  
  //CPPUNIT_TEST( downloadFromFileTest );
  //CPPUNIT_TEST( uploadToFileTest );  
  //CPPUNIT_TEST( uploadToDatabaseTest );  
  //CPPUNIT_TEST( downloadFromDatabaseTest );
  CPPUNIT_TEST( downloadStateFromDatabaseTest );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedTkDcuConversionNumber;
  char *buffer;
  unsigned int clobLength;

  DbTkDcuConversionAccess *dbAccess;
  XMLTkDcuConversion *xmlConversion;
  XMLTkDcuConversion *xmlConversionFile;
  DbTkDcuInfoAccess *dbInfoAccess;
  XMLTkDcuInfo *xmlInfo;
  XMLTkDcuInfo *xmlInfoFile;
  void uploadTest();
  void uploadToFileTest();
  void uploadToDatabaseTest();
  void downloadTest();
  void downloadFromFileTest();
  void downloadFromDatabaseTest();
  void downloadStateFromDatabaseTest();
};
#endif
