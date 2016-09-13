#ifndef DBPIARESETACCESS_TEST_H
#define DBPIARESETACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbPiaResetAccess.h"
#include "Utils.h"
/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbPiaResetAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbPiaResetAccessTest );
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest );
  CPPUNIT_TEST( downloadTestFromPartitionName ); 
  CPPUNIT_TEST( downloadTestFromPartitionNameAndHardId );
  CPPUNIT_TEST( versionTest );
  CPPUNIT_TEST( uploadVersionTestInCurrentMajorDotZero );
  CPPUNIT_TEST( uploadVersionTestInCurrentMajorDotNextMinor );
  CPPUNIT_TEST( uploadVersionTestInNextMajorDotZero );
  CPPUNIT_TEST( uploadVersionTestInZeroDotNextMinor );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedPiaResetNumber;
  std::string xmlBuffer;
  unsigned int clobLength;

  DbPiaResetAccess *dbAccess;
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartitionName();
  void downloadTestFromPartitionNameAndHardId();
  void versionTest();
  void uploadVersionTestInCurrentMajorDotZero();
  void uploadVersionTestInCurrentMajorDotNextMinor();
  void uploadVersionTestInNextMajorDotZero();
  void uploadVersionTestInZeroDotNextMinor();
};
#endif
