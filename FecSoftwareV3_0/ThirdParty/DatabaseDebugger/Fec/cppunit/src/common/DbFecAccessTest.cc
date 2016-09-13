#include "DbFecAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbFecAccessTest );

static const std::string uPllString("RAWPLL");
static const std::string uApv25String("RAWAPVFEC");

static const std::string dPllString("PLL ");
static const std::string dApv25String("APV25 ");
static const std::string partitionName1("Partition");
static const std::string partitionName2("OtherPartition");
static const std::string structureName1("Structure");
static const std::string structureName2("Structure2");
static const std::string dirName("../../xml/");

static const std::string fecHardwareId("AAA");
static const std::string dummyFecHardwareId("CBA");

static const unsigned int versionMajor = 123;
static const unsigned int versionMinor = 321;

void DbFecAccessTest::setUp(){
  try {
    dbAccess = new DbFecAccess();

    std::string *xmlTest = new std::string[8];
    xmlBuffer = new std::string[8];
    std::string buffer[5];

    xmlTest[0] = dirName + "newTestPll.xml";
    xmlTest[1] = dirName + "newTestLaserdriver.xml";
    xmlTest[2] = dirName + "newTestApvFec.xml";
    xmlTest[3] = dirName + "newTestApvMux.xml";
    xmlTest[4] = dirName + "newTestDcu.xml";

    /* lecture des fichiers */
    for (int i = 0; i< 5; i++) {
      Utils::init(xmlTest[i], &buffer[i]);
      xmlBuffer[i] = std::string("<ROWSET>") + buffer[i] + std::string("</ROWSET>") ;
    }

    /* count the <TAG> and </TAG> */
    uploadedPllNumber = Utils::findStringInBuffer(buffer[0], uPllString)/2;
    uploadedApv25Number = Utils::findStringInBuffer(buffer[2], uApv25String)/2;
    CPPUNIT_ASSERT( uploadedPllNumber  > 0);
    CPPUNIT_ASSERT( uploadedApv25Number  > 0);

    delete[] xmlTest;
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::tearDown(){
  delete[] buffer;
  delete dbAccess;
}

void DbFecAccessTest::downloadTest(){
  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    clobLength = dbAccess->getXMLClob ("deNouilles")->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength);
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::createNewStateTest(){
  unsigned int structureId;
  unsigned int partitionId;
  unsigned int piaVersionMajorId;
  unsigned int fecVersionMajorId;
  try {
    dbAccess->createNewState(structureName1, &structureId, partitionName1, &partitionId, &piaVersionMajorId, &fecVersionMajorId);
  CPPUNIT_ASSERT(  dbAccess->getNextMajorVersion() == fecVersionMajorId+1 );
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::uploadTest(){
  try {
    // upload to partitionName1, creation d'une partition
    dbAccess->setXMLClob (xmlBuffer, partitionName1, true, structureName1);
    // upload to partitionName1 in versionMajor.versionMinor
    dbAccess->setXMLClobWithVersion (xmlBuffer, partitionName1, versionMajor, versionMinor);

    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    CPPUNIT_ASSERT( emptyClobLength > 0 );
    
    // download from partitionName1 with Pll
    clobLength = dbAccess->getXMLClob (partitionName1)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
    // download from partitionName1 and version number with Pll 
    clobLength = dbAccess->getXMLClobWithVersion (partitionName1, versionMajor, versionMinor )->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::downloadTestFromPartitionName(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPllNumber;
  unsigned int  downloadedApv25Number;

  try {
    // download from partitionName1 with pll
    xmlClob = dbAccess->getXMLClob (partitionName1);
    downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
    downloadedApv25Number = Utils::findStringInClob(xmlClob, dApv25String); 
    CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
    
    // download from partitionName1 and version number with pll
    xmlClob = dbAccess->getXMLClobWithVersion (partitionName1, versionMajor, versionMinor);
    downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
    CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::downloadTestFromPartitionNameAndHardId(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPllNumber;

  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    
    // download from partitionName1 and fecHardwareId with pll
    xmlClob= dbAccess->getXMLClob (partitionName1, fecHardwareId);
    downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
    CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFecAccessTest::versionTest(){
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

void DbFecAccessTest::uploadVersionTestInCurrentMajorDotZero(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPllNumber;

  try {
     std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
     std::list<unsigned int*>::iterator it = listeId->begin();
     unsigned int *value = *it;
     int versionMajorId = value[1];

     // upload to partitionName1, currentMajorVersion.0
     dbAccess->setXMLClob ( xmlBuffer, partitionName1, 2);
     // upload to partitionName1 in versionMajor.versionMinor
     xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, versionMajorId, 0);
     downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
     CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}
     
void DbFecAccessTest::uploadVersionTestInCurrentMajorDotNextMinor(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPllNumber;

  try {
     std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
     std::list<unsigned int*>::iterator it = listeId->begin();
     unsigned int *value = *it;
     int versionMajorId = value[1];
     int nextMinorId;

     nextMinorId = dbAccess->getNextMinorVersion(versionMajorId);
     // upload to partitionName1, currentMajorVersion.nextMinorVersion
     dbAccess->setXMLClob ( xmlBuffer, partitionName1, 0);
     // upload to partitionName1 in versionMajor.versionMinor
     xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, versionMajorId, nextMinorId);
     downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
     CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}
     
void DbFecAccessTest::uploadVersionTestInNextMajorDotZero(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedPllNumber;

  try {
     int nextMajorId;

     nextMajorId = dbAccess->getNextMajorVersion();
     // upload to partitionName1, nextMajorVersion.0
     dbAccess->setXMLClob ( xmlBuffer, partitionName1, 1);
     // upload to partitionName1 in versionMajor.versionMinor
     xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, nextMajorId, 0);
     downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 
     CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

void DbFecAccessTest::uploadVersionTestInZeroDotNextMinor(){
  oracle::occi::Clob *xmlClob;
  unsigned int downloadedPllNumber;

  try {
     int nextMinorId;

     nextMinorId = dbAccess->getNextMinorVersion(0);
     // upload to partitionName1, 0.nextMinorVersion
     dbAccess->setXMLClob ( xmlBuffer, partitionName1, 3);
     // upload to partitionName1 in versionMajor.versionMinor
     xmlClob = dbAccess->getXMLClobWithVersion ( partitionName1, 0, nextMinorId);
     downloadedPllNumber = Utils::findStringInClob(xmlClob, dPllString); 

     CPPUNIT_ASSERT( downloadedPllNumber == uploadedPllNumber  );
     
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

