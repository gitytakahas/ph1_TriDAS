#include "DbConnectionsAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbConnectionsAccessTest );

static const std::string uConnectionString("RAWCONNECTION");
static const std::string dConnectionString("<FedChannelConnection ");
static const std::string partitionName1("Partition");
static const std::string dirName("../../xml/");

static const std::string fecHardwareId("AAA");
static const std::string dummyConnectionsHardwareId("50598144");

void DbConnectionsAccessTest::setUp(){
  try {
    dbAccess = new DbConnectionsAccess();
    std::string buffer;
    std::string xmlTest = dirName + "newTestConnections.xml";

    Utils::init(xmlTest, &buffer);
    xmlBuffer = std::string("<ROWSET>") + buffer + std::string("</ROWSET>") ;
    uploadedConnectionNumber = Utils::findStringInBuffer(buffer, uConnectionString)/2;
    CPPUNIT_ASSERT( uploadedConnectionNumber  > 0);
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbConnectionsAccessTest::tearDown(){
  delete dbAccess;
}

void DbConnectionsAccessTest::downloadTest(){
  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    clobLength = dbAccess->getXMLClob ("deNouilles")->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength);
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbConnectionsAccessTest::uploadTest(){
  try {
    // upload to partitionName1, creation d'une partition
    dbAccess->setXMLClob (xmlBuffer, partitionName1, 1);

    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    CPPUNIT_ASSERT( emptyClobLength > 0 );

    // download from partitionName1 with Connection
    clobLength = dbAccess->getXMLClob (partitionName1)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
    delete[] buffer;

  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbConnectionsAccessTest::downloadTestFromPartitionName(){
  CPPUNIT_ASSERT( uploadedConnectionNumber  > 0);
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedConnectionNumber;

  try {
    // download from partitionName1 with connection
    xmlClob = dbAccess->getXMLClob (partitionName1);
    downloadedConnectionNumber = Utils::findStringInClob(xmlClob, dConnectionString); 
    CPPUNIT_ASSERT( downloadedConnectionNumber == uploadedConnectionNumber  );
    
    delete[] buffer;
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbConnectionsAccessTest::copyPartition(){
  CPPUNIT_ASSERT( uploadedConnectionNumber  > 0);

  try {
    // download from partitionName1 with connection
    std::vector<unsigned int> dcuHardIdVector(3);
    dcuHardIdVector[0]=69632;
    dcuHardIdVector[1]=69633;
    dcuHardIdVector[2]=69634;
    
    dbAccess->copyPartition(partitionName1, "NouvellePartition", "NouvelleStructure", dcuHardIdVector);
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}





