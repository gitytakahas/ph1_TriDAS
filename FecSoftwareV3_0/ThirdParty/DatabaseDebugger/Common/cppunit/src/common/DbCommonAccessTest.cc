#include "DbCommonAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbCommonAccessTest );

void DbCommonAccessTest::setUp(){
  dbAccess1 = new DbCommonAccess();
  dbAccess2 = new DbCommonAccess();
  DbCommonAccess::getDbConfiguration(login, passwd, path);
}

void DbCommonAccessTest::tearDown(){
  delete dbAccess1;
  delete dbAccess2;
}

void DbCommonAccessTest::constructorTest(){
  // Check
  CPPUNIT_ASSERT_MESSAGE("dbAccess1 != dbAccess2", dbAccess1 != dbAccess2);
  CPPUNIT_ASSERT( dbAccess1->getDbLogin() == dbAccess2->getDbLogin());
  CPPUNIT_ASSERT( dbAccess1->getDbPassword() == dbAccess2->getDbPassword());
  CPPUNIT_ASSERT( dbAccess1->getDbPath() == dbAccess2->getDbPath());
}

void DbCommonAccessTest::connectToDummyAccountThrows(){
  //Check
  DbCommonAccess *dbDummyAccess = new DbCommonAccess("tagada","tsointsoin","db");
  delete dbDummyAccess;
}

void DbCommonAccessTest::getDbConfigurationTest(){
  // Check
  CPPUNIT_ASSERT( dbAccess1->getDbLogin() == login );
  CPPUNIT_ASSERT( dbAccess1->getDbPassword() == passwd );
  CPPUNIT_ASSERT( dbAccess1->getDbPath() == path );
}


void DbCommonAccessTest::initializeClobTest(){
  // Check
  CPPUNIT_ASSERT( dbAccess1->initializeClob().isInitialized() == true );
  CPPUNIT_ASSERT( dbAccess2->initializeClob().isInitialized() == true );
}

void DbCommonAccessTest::getErrorMessageTest(){
  // Check
  CPPUNIT_ASSERT( dbAccess1->getErrorMessage() ==  dbAccess2->getErrorMessage());
}
