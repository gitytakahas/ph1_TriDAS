#include "XMLFecPiaResetTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( XMLFecPiaResetTest );

static const std::string piaResetString("PIARESET ");
static const std::string partitionName1("NewPartition");
static const std::string partitionName2("OtherPartition");
static const std::string partitionName3("DummyPartition");
static const std::string structureName("Structure");
static const std::string fileName("../../xml/testFecTemplate320TOB.xml");

static const std::string fecHardwareId("AAA");
static const std::string dummyFecHardwareId("CBA");
static const unsigned int versionMajor = 123;
static const unsigned int versionMinor = 321;

void XMLFecPiaResetTest::setUp(){
  try {
    dbAccess = new DbPiaResetAccess();
    xmlFecPiaReset = new XMLFecPiaReset();
    xmlFecPiaReset->setDatabaseAccess(dbAccess);
    xmlFecPiaResetFile = new XMLFecPiaReset(fileName);
    xmlFecPiaResetFile->setDatabaseAccess(dbAccess);
    xmlFecPiaResetPartition = new XMLFecPiaReset(dbAccess, partitionName1);
    xmlFecPiaResetPartitionVersion = new XMLFecPiaReset(dbAccess, partitionName1, versionMajor, versionMinor);
    xmlFecPiaResetPartitionFecHardId = new XMLFecPiaReset(dbAccess, partitionName1, fecHardwareId);
    xmlFecPiaResetPartitionFecHardIdVersion = new XMLFecPiaReset(dbAccess, partitionName1, fecHardwareId, versionMajor, versionMinor);
    xmlFecPiaResetDummyPartition = new XMLFecPiaReset(dbAccess, partitionName3);

    Utils::init(fileName, &buffer);
    uploadedPiaResetNumber = Utils::findStringInBuffer(buffer, piaResetString);
    CPPUNIT_ASSERT( uploadedPiaResetNumber  > 0);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecPiaResetTest::tearDown(){
  delete xmlFecPiaReset;
  delete xmlFecPiaResetFile;
  delete xmlFecPiaResetPartition;
  delete xmlFecPiaResetPartitionVersion;
  delete xmlFecPiaResetPartitionFecHardId;
  delete xmlFecPiaResetPartitionFecHardIdVersion;
  delete xmlFecPiaResetDummyPartition;
  delete dbAccess;
}

void XMLFecPiaResetTest::dbConfigureTest(){
  unsigned int returnedVersionMajorId;
  try {
    piaResetVector *dVector = xmlFecPiaResetFile->getPiaResets();
    CPPUNIT_ASSERT( dVector->size() == uploadedPiaResetNumber );

    returnedVersionMajorId = xmlFecPiaResetFile->dbConfigure(partitionName1);

    CPPUNIT_ASSERT( returnedVersionMajorId+1 == dbAccess->getNextMajorVersion());
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecPiaResetTest::downloadTest(){
  try {
    CPPUNIT_ASSERT( xmlFecPiaResetDummyPartition->getPiaResets()->size() == 0);

    unsigned int piaResetNumber;

    piaResetNumber = xmlFecPiaResetPartition->getPiaResets()->size();
    CPPUNIT_ASSERT( piaResetNumber == uploadedPiaResetNumber );

    piaResetNumber = xmlFecPiaResetPartitionFecHardId->getPiaResets()->size();
    CPPUNIT_ASSERT( piaResetNumber == uploadedPiaResetNumber );
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecPiaResetTest::uploadTest(){
  try {
    unsigned int piaResetNumber;

    // upload to partitionName1 in versionMajor->versionMinor
    piaResetNumber = xmlFecPiaResetPartition->getPiaResets()->size();
    xmlFecPiaResetPartition->setPiaResets (partitionName1, versionMajor, versionMinor);
  } catch (FecExceptionHandler &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}
    
void XMLFecPiaResetTest::downloadTestFromPartition(){
  try {
    unsigned int downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, versionMajor, versionMinor)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber );
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}
     
void XMLFecPiaResetTest::downloadTestFromPartitionAndId(){
  try {
    unsigned int downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, fecHardwareId, versionMajor, versionMinor)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber );
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecPiaResetTest::uploadInVersionUpdateTest(){
  try {
    unsigned int downloadedPiaResetNumber;
    std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
    std::list<unsigned int*>::iterator it = listeId->begin();
    unsigned int *value = *it;
    int versionMajorId = value[1];
    
    // upload to partitionName1, currentMajorVersion.0
    piaResetNumber = xmlFecPiaResetPartition->getPiaResets()->size();
    xmlFecPiaResetPartition->setPiaResets (partitionName1, 2);
    // download from partitionName1 and version number 
    downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, versionMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
    
    // upload to partitionName1, creation d'une partition
    int nextMinorId = dbAccess->getNextMinorVersion(versionMajorId);
    //upload to partitionName1, currentMajorVersion.nextMinorVersion
    xmlFecPiaResetPartition->setPiaResets (partitionName1, 0);
    // download from partitionName1 and version number 
    downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, versionMajorId, nextMinorId)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
    
    int nextMajorId = dbAccess->getNextMajorVersion();
    //upload to partitionName1, majorVersion.minorVersion
    xmlFecPiaResetPartition->setPiaResets (partitionName1, 1);
    // download from partitionName1 and version number 
    downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, nextMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber  );
    
    nextMinorId = dbAccess->getNextMinorVersion(0);
    // upload to partitionName1, 0.nextMinorVersion
    xmlFecPiaResetPartition->setPiaResets ( partitionName1, 3);
    // download from partitionName1 and version number 
    downloadedPiaResetNumber = xmlFecPiaResetPartition->getPiaResets(partitionName1, nextMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedPiaResetNumber == uploadedPiaResetNumber);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}





