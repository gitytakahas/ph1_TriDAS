#ifndef DBFECACCESS_TEST_H
#define DBFECACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbFecAccess.h"
#include "Utils.h"
/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbFecAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbFecAccessTest );
  CPPUNIT_TEST( downloadTest );  
  CPPUNIT_TEST( createNewStateTest );   
  //CPPUNIT_TEST( uploadTest );    
  //CPPUNIT_TEST( downloadTestFromPartitionName );     
  //CPPUNIT_TEST( downloadTestFromPartitionNameAndHardId );     
  //CPPUNIT_TEST( versionTest );     
  //CPPUNIT_TEST( uploadVersionTestInCurrentMajorDotZero );  
  //CPPUNIT_TEST( uploadVersionTestInCurrentMajorDotNextMinor );  
  //CPPUNIT_TEST( uploadVersionTestInNextMajorDotZero );  
  //CPPUNIT_TEST( uploadVersionTestInZeroDotNextMinor );  
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedPllNumber;
  unsigned int uploadedApv25Number;
  char *buffer;
  std::string *xmlBuffer;
  unsigned int clobLength;

  DbFecAccess *dbAccess;
  void downloadTest();
  void uploadTest();
  void createNewStateTest();
  void downloadTestFromPartitionName();
  void downloadTestFromPartitionNameAndHardId();
  void downloadTestFromPartitionNameAndSoftId();
  void versionTest();
  void uploadVersionTestInCurrentMajorDotZero();
  void uploadVersionTestInCurrentMajorDotNextMinor();
  void uploadVersionTestInNextMajorDotZero();
  void uploadVersionTestInZeroDotNextMinor();
};
#endif
