#ifndef XMLFECPIARESET_TEST_H
#define XMLFECPIARESET_TEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <fstream>
#include "XMLFecPiaReset.h"
#include "Utils.h"
/* 
 * A test case that is designed to produce
 * example errors and failures
 *
 */

class XMLFecPiaResetTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( XMLFecPiaResetTest );
  CPPUNIT_TEST( dbConfigureTest );  
  CPPUNIT_TEST( downloadTest ); 
  CPPUNIT_TEST( uploadTest );   
  CPPUNIT_TEST( downloadTestFromPartition );  
  CPPUNIT_TEST( downloadTestFromPartitionAndId );  
  CPPUNIT_TEST( uploadInVersionUpdateTest); 
  CPPUNIT_TEST_SUITE_END();
  
 public:
  void setUp();
  void tearDown();

 private :
  void init(std::string fileName);
  unsigned int findStringInClob(oracle::occi::Clob *xmlClob, std::string name);

 protected:
  unsigned int piaResetNumber;
  unsigned int uploadedPiaResetNumber;
  std::string buffer;

  DbPiaResetAccess *dbAccess;
  XMLFecPiaReset *xmlFecPiaReset;
  XMLFecPiaReset *xmlFecPiaResetFile;
  XMLFecPiaReset *xmlFecPiaResetPartition;
  XMLFecPiaReset *xmlFecPiaResetDummyPartition;
  XMLFecPiaReset *xmlFecPiaResetPartitionVersion;
  XMLFecPiaReset *xmlFecPiaResetPartitionFecHardId;
  XMLFecPiaReset *xmlFecPiaResetPartitionFecHardIdVersion;
  void dbConfigureTest();
  void downloadTest();
  void uploadTest();
  void downloadTestFromPartition();
  void downloadTestFromPartitionAndId();
  void uploadInVersionUpdateTest();
};
#endif
