#ifndef XMLFECDEVICE_TEST_H
#define XMLFECDEVICE_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "XMLFecDevice.h"
#include "Utils.h"

/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class XMLFecDeviceTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( XMLFecDeviceTest );
  CPPUNIT_TEST( dbConfigureTest );  
  CPPUNIT_TEST( downloadTest );
  CPPUNIT_TEST( uploadTest );  
  CPPUNIT_TEST( downloadTestFromPartition );  
  CPPUNIT_TEST( downloadTestFromPartitionAndId ); 
  CPPUNIT_TEST( uploadInVersionUpdateTest );  
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int deviceNumber;
  unsigned int uploadedPllNumber;
  unsigned int uploadedApvNumber;
  unsigned int uploadedApvMuxNumber;
  unsigned int uploadedLaserdriverNumber;
  unsigned int uploadedDcuNumber;
  unsigned int uploadedDeviceNumber;
  unsigned int uploadedDeviceNumberWithDcu;
  std::string buffer;

  DbFecAccess *dbAccess;
  XMLFecDevice *xmlFecDevice;
  XMLFecDevice *xmlFecDeviceFile;
  XMLFecDevice *xmlFecDevicePartition;
  XMLFecDevice *xmlFecDeviceDummyPartition;
  XMLFecDevice *xmlFecDevicePartitionVersion;
  XMLFecDevice *xmlFecDevicePartitionFecHardId;
  XMLFecDevice *xmlFecDevicePartitionFecHardIdVersion;
  void dbConfigureTest();
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartition();
  void downloadTestFromPartitionAndId();
  void uploadInVersionUpdateTest();
};
#endif
