#include "DbPiaResetAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbPiaResetAccessTest );

static const std::string uPiaResetString("RAWPIARESET");
static const std::string dPiaResetString("PIARESET ");
static const std::string partitionName1("Partition");
static const std::string structureName("Structure");
static const std::string dirName("../../xml/");

static const std::string fecHardwareId("AAA");
static const std::string dummyFecHardwareId("CBA");

static const unsigned int versionMajor = 123;
static const unsigned int versionMinor = 321;

void DbPiaResetAccessTest::setUp(){
  try {
    dbAccess = new DbPiaResetAccess();
    std::string buffer;
    std::string xmlTest = dirName + "newTestPiaReset.xml";

    Utils::init(xmlTest, &buffer);
    xmlBuffer = std::string("<ROWSET>") + buffer + std::string("</ROWSET>") ;
    uploadedPiaResetNumber = Utils::findStringInBuffer(buffer, uPiaResetString)/2;
    CPPUNIT_ASSERT( uploadedPiaResetNumber  > 0);
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::tearDown(){
  delete dbAccess;
}

void DbPiaResetAccessTest::downloadTest(){
  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    clobLength = dbAccess->getXMLClob ("deNouilles")->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength);
    CPPUNIT_ASSERT( clobLength > 0 );
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::uploadTest(){
  try {
    // upload to partitionName1, creation d'une partition
    dbAccess->setXMLClob (xmlBuffer, partitionName1);
    // upload to partitionName1 in versionMajor.versionMinor
    dbAccess->setXMLClobWithVersion (xmlBuffer, partitionName1, versionMajor, versionMinor);

    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();

    // download from partitionName1 with PiaReset
    clobLength = dbAccess->getXMLClob (partitionName1)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
    // download from partitionName1 and version number with PiaReset 
    clobLength = dbAccess->getXMLClobWithVersion (partitionName1, versionMajor, versionMinor )->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::downloadTestFromPartitionName(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;
  try {
    // download from partitionName1 with piaReset
    xmlClob = dbAccess->getXMLClob (partitionName1);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
    
    // download from partitionName1 and version number with piaReset
    xmlClob = dbAccess->getXMLClobWithVersion (partitionName1, versionMajor, versionMinor);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::downloadTestFromPartitionNameAndHardId(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;

  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    
    // download from partitionName1 and fecHardwareId with piaReset
    xmlClob= dbAccess->getXMLClob (partitionName1, fecHardwareId);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );

    // download from partitionName1 and dummyPiaResetHardwareId with piaReset
    clobLength = dbAccess->getXMLClob (partitionName1, dummyFecHardwareId)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
    // download from partitionName1, fecHardwareId and version number with piaReset
    xmlClob= dbAccess->getXMLClobWithVersion (partitionName1, fecHardwareId, versionMajor, versionMinor);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
    
    // download from partitionName1, dummyPiaResetHardwareId and version number with piaReset
    clobLength = dbAccess->getXMLClobWithVersion (partitionName1, dummyFecHardwareId, versionMajor, versionMinor)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::versionTest(){
  try {
    CPPUNIT_ASSERT(  dbAccess->getNextMinorVersion(0) ==  dbAccess->getMaxMinorVersion()+1);
    CPPUNIT_ASSERT(  dbAccess->getNextMajorVersion() > versionMajor );
    CPPUNIT_ASSERT(  dbAccess->getNextMinorVersion(versionMajor) >= versionMinor );
    
    std::list<unsigned int*> *listeValues;
    listeValues = dbAccess->getDatabaseVersion(partitionName1);
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
      unsigned int *value = *it;
      CPPUNIT_ASSERT(  dbAccess->getPartitionName(value[0]) >= partitionName1 );
      CPPUNIT_ASSERT(  dbAccess->getNextMajorVersion() >= value[1] );
      CPPUNIT_ASSERT(  dbAccess->getNextMinorVersion(value[1]) >= value[2] );
      value[1] = versionMajor;
      value[2] = versionMinor;
    }
    dbAccess->setDatabaseVersion(listeValues);
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
      delete[] *it;
    }
    delete listeValues;

    listeValues = dbAccess->getDatabaseVersion(partitionName1);
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
      unsigned int *value = *it;
      CPPUNIT_ASSERT(  dbAccess->getPartitionName(value[0]) >= partitionName1 );
      CPPUNIT_ASSERT(  value[1] == versionMajor);
      CPPUNIT_ASSERT(  value[2] == versionMinor);
    }
    
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
      delete[] *it;
    }
    delete listeValues;
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbPiaResetAccessTest::uploadVersionTestInCurrentMajorDotZero(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;

  try {
    std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
    std::list<unsigned int*>::iterator it = listeId->begin();
    unsigned int *value = *it;
    int versionMajorId = value[1];

    // upload to partitionName1, currentMajorVersion.0
    dbAccess->setXMLClob ( xmlBuffer, partitionName1, 2);
    // upload to partitionName1 in versionMajor.versionMinor
    xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, versionMajorId, 0);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );

  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}



void DbPiaResetAccessTest::uploadVersionTestInCurrentMajorDotNextMinor(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;

  try {
    std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
    std::list<unsigned int*>::iterator it = listeId->begin();
    unsigned int *value = *it;
    int versionMajorId = value[1];

    int nextMinorId = dbAccess->getNextMinorVersion(versionMajorId);
    // upload to partitionName1, currentMajorVersion.nextMinorVersion
    dbAccess->setXMLClob ( xmlBuffer, partitionName1, 0);
    // upload to partitionName1 in versionMajor.versionMinor
    xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, versionMajorId, nextMinorId);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

void DbPiaResetAccessTest::uploadVersionTestInNextMajorDotZero(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;

  try {
    int nextMajorId = dbAccess->getNextMajorVersion();
    // upload to partitionName1, nextMajorVersion.0
    dbAccess->setXMLClob ( xmlBuffer, partitionName1, 1);
    // upload to partitionName1 in versionMajor.versionMinor
    xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, nextMajorId, 0);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );

  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}


void DbPiaResetAccessTest::uploadVersionTestInZeroDotNextMinor(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPiaResetNumber;

  try {
    int nextMinorId = dbAccess->getNextMinorVersion(0);
    // upload to partitionName1, 0.nextMinorVersion
    dbAccess->setXMLClob ( xmlBuffer, partitionName1, 3);
    // upload to partitionName1 in versionMajor.versionMinor
    xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, 0, nextMinorId);
    downloadedPiaResetNumber = Utils::findStringInClob(xmlClob, dPiaResetString); 
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

