#ifndef DBCONNECTIONSACCESS_TEST_H
#define DBCONNECTIONSACCESS_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "DbConnectionsAccess.h"
#include "Utils.h"
/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class DbConnectionsAccessTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DbConnectionsAccessTest );
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest );
  CPPUNIT_TEST( downloadTestFromPartitionName );
  CPPUNIT_TEST( copyPartition );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int emptyClobLength;
  unsigned int uploadedConnectionNumber;
  char *buffer;
  std::string xmlBuffer;
  unsigned int clobLength;

  DbConnectionsAccess *dbAccess;
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartitionName();
  void downloadTestFromPartitionNameAndHardId();
  void copyPartition();
};
#endif
