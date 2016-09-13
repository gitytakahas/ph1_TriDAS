#ifndef DBCCUACCESS_TEST_H
#define DBCCUACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbCcuAccess.h"

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbCcuAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbCcuAccessTest );
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest );
  CPPUNIT_TEST( downloadTestFromPartitionNameAndHardId );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedCcuNumber;
  char *buffer;
  unsigned int clobLength;

  DbCcuAccess *dbAccess;
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartitionNameAndHardId();
  void versionTest();
};
#endif
