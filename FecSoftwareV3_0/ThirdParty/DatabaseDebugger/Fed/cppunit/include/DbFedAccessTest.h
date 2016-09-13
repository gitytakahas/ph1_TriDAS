#ifndef DBFEDACCESS_TEST_H
#define DBFEDACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbFedAccess.h"
#include "Utils.h"
/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbFedAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbFedAccessTest );
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest );
  CPPUNIT_TEST( downloadTestFromPartitionName ); 
  CPPUNIT_TEST( downloadTestFromPartitionNameAndHardId );  
  CPPUNIT_TEST( downloadTestFromPartitionNameAndSoftId );  
  CPPUNIT_TEST( versionTest ); 
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedFedNumber;
  unsigned int uploadedTtcrxNumber;
  unsigned int uploadedApvNumber;
  unsigned int uploadedStripNumber;
  char *buffer;
  std::string *xmlBuffer;

  unsigned int clobLength;

  DbFedAccess *dbAccess;
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartitionName();
  void downloadTestFromPartitionNameAndHardId();
  void downloadTestFromPartitionNameAndSoftId();
  void versionTest();
};
#endif

