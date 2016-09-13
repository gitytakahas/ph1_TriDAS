#ifndef CPP_UNIT_EXAMPLETESTCASE_H
#define CPP_UNIT_EXAMPLETESTCASE_H

#include "DbCommonAccess.h"
#include <cppunit/extensions/HelperMacros.h>

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbCommonAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbCommonAccessTest );
  CPPUNIT_TEST( constructorTest );
  CPPUNIT_TEST_EXCEPTION( connectToDummyAccountThrows, oracle::occi::SQLException );
  CPPUNIT_TEST( getDbConfigurationTest );
  CPPUNIT_TEST(  initializeClobTest );
  CPPUNIT_TEST(  getErrorMessageTest );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();
  
 protected:
  std::string login;
  std::string passwd;
  std::string path;
  DbCommonAccess *dbAccess1;
  DbCommonAccess *dbAccess2;
  void constructorTest();
  void connectToDummyAccountThrows();
  void getDbConfigurationTest();
  void initializeClobTest();
  void getErrorMessageTest();
};
#endif
