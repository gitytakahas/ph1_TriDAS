#ifndef XMLFECDCU_TEST_H
#define XMLFECDCU_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "XMLFecDcu.h"

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class XMLFecDcuTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( XMLFecDcuTest );
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest ); 
  CPPUNIT_TEST( downloadTestFromPartition );
  CPPUNIT_TEST( downloadTestFromPartitionAndTime );
  CPPUNIT_TEST( downloadTestFromPartitionId );
  CPPUNIT_TEST( downloadTestFromPartitionIdAndTime );
  CPPUNIT_TEST( downloadTestFromPartitionDcuId );
  CPPUNIT_TEST( downloadTestFromPartitionDcuIdAndTime );
  CPPUNIT_TEST( downloadTestFromDcuId );
  CPPUNIT_TEST( downloadTestFromDcuIdAndTime );
  CPPUNIT_TEST( downloadTestAll );
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int dcuNumber;
  unsigned int uploadedDcuNumber;
  char *buffer;

  DbFecAccess *dbAccess;
  XMLFecDcu *xmlFecDcu;
  XMLFecDcu *xmlFecDcuFile;
  XMLFecDcu *xmlFecDcuTime;
  XMLFecDcu *xmlFecDcuPartition;
  XMLFecDcu *xmlFecDcuPartitionTime;
  XMLFecDcu *xmlFecDcuDummyPartition;
  XMLFecDcu *xmlFecDcuPartitionFecHardId;
  XMLFecDcu *xmlFecDcuPartitionIp;
  XMLFecDcu *xmlFecDcuPartitionIpTime;
  XMLFecDcu *xmlFecDcuPartitionFecHardIdTime;
  XMLFecDcu *xmlFecDcuPartitionDcuHardId;
  XMLFecDcu *xmlFecDcuPartitionDcuHardIdTime;
  XMLFecDcu *xmlFecDcuDcuHardId;
  XMLFecDcu *xmlFecDcuDcuHardIdTime;
  void dbConfigureTest();
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartition();
  void downloadTestFromPartitionAndTime();
  void downloadTestFromPartitionId();
  void downloadTestFromPartitionIdAndTime();
  void downloadTestFromPartitionDcuId();
  void downloadTestFromPartitionDcuIdAndTime();
  void downloadTestFromDcuId();
  void downloadTestFromDcuIdAndTime();
  void downloadTestAll();
};
#endif
