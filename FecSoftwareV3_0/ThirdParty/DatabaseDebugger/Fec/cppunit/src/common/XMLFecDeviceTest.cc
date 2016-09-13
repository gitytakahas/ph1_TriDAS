#include "XMLFecDeviceTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( XMLFecDeviceTest );

static const std::string pllString("PLL ");
static const std::string apvString("APV25 ");
static const std::string laserdriverString("LASERDRIVER ");
static const std::string apvMuxString("APVMUX ");
static const std::string dcuString("DCU ");
static const std::string partitionName1("NewPartition");
static const std::string partitionName2("OtherPartition");
static const std::string partitionName3("DummyPartition");
static const std::string structureName("Structure");
//static const std::string fileName("../../xml/testFecTemplateWithoutDcu.xml");
static const std::string fileName("../../xml/testFecTemplate320TOB.xml");

static const std::string fecHardwareId("AAA");
static const std::string dummyFecHardwareId("CBA");
static const unsigned int versionMajor = 123;
static const unsigned int versionMinor = 321;

void XMLFecDeviceTest::setUp(){
  try {
    dbAccess = new DbFecAccess();
    xmlFecDevice = new XMLFecDevice();
    xmlFecDevice->setDatabaseAccess(dbAccess);
    xmlFecDeviceFile = new XMLFecDevice(fileName);
    xmlFecDeviceFile->setDatabaseAccess(dbAccess);
    xmlFecDevicePartition = new XMLFecDevice(dbAccess, partitionName1);
    xmlFecDevicePartitionVersion = new XMLFecDevice(dbAccess, partitionName1, versionMajor, versionMinor);
    xmlFecDevicePartitionFecHardId = new XMLFecDevice(dbAccess, partitionName1, fecHardwareId);
    xmlFecDevicePartitionFecHardIdVersion = new XMLFecDevice(dbAccess, partitionName1, fecHardwareId, versionMajor, versionMinor);
    xmlFecDeviceDummyPartition = new XMLFecDevice(dbAccess, partitionName3);

    Utils::init(fileName, &buffer);
    uploadedPllNumber = Utils::findStringInBuffer(buffer, pllString);
    uploadedApvMuxNumber = Utils::findStringInBuffer(buffer, apvMuxString);
    uploadedLaserdriverNumber = Utils::findStringInBuffer(buffer, laserdriverString);
    uploadedApvNumber = Utils::findStringInBuffer(buffer, apvString);
    uploadedDcuNumber = Utils::findStringInBuffer(buffer, dcuString);
    uploadedDeviceNumber = uploadedPllNumber + uploadedApvNumber + uploadedApvMuxNumber + uploadedLaserdriverNumber;
    uploadedDeviceNumberWithDcu = uploadedPllNumber + uploadedApvNumber + uploadedApvMuxNumber + uploadedLaserdriverNumber + uploadedDcuNumber;

    CPPUNIT_ASSERT( uploadedPllNumber  > 0);
    CPPUNIT_ASSERT( uploadedApvMuxNumber  > 0);
    CPPUNIT_ASSERT( uploadedLaserdriverNumber  > 0);
    CPPUNIT_ASSERT( uploadedApvNumber  > 0);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDeviceTest::tearDown(){
  delete xmlFecDevice;
  delete xmlFecDeviceFile;
  delete xmlFecDevicePartition;
  delete xmlFecDevicePartitionVersion;
  delete xmlFecDevicePartitionFecHardId;
  delete xmlFecDevicePartitionFecHardIdVersion;
  delete xmlFecDeviceDummyPartition;
  delete dbAccess;
}

void XMLFecDeviceTest::dbConfigureTest(){
  unsigned int returnedVersionMajorId;
  try {
    deviceVector* dVector = xmlFecDeviceFile->getDevices();
    returnedVersionMajorId = xmlFecDeviceFile->dbConfigure(structureName, partitionName1, true);
    CPPUNIT_ASSERT( dVector->size() == uploadedDeviceNumberWithDcu );
    CPPUNIT_ASSERT( returnedVersionMajorId+1 == dbAccess->getNextMajorVersion());
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDeviceTest::downloadTest(){
  try {
    CPPUNIT_ASSERT( xmlFecDeviceDummyPartition->getDevices()->size() == 0);

    unsigned int deviceNumber;

    deviceNumber = xmlFecDevicePartition->getDevices(partitionName1)->size();
    CPPUNIT_ASSERT( deviceNumber == uploadedDeviceNumber);

    deviceNumber = xmlFecDevicePartitionFecHardId->getDevices(partitionName1, fecHardwareId)->size();
    CPPUNIT_ASSERT( deviceNumber == uploadedDeviceNumber);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDeviceTest::uploadTest(){
  try {
    unsigned int deviceNumber;

    // upload to partitionName1 in versionMajor->versionMinor
    deviceNumber = xmlFecDevicePartition->getDevices()->size();
    xmlFecDevicePartition->setDevices (partitionName1, versionMajor, versionMinor);
  } catch (FecExceptionHandler &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

void XMLFecDeviceTest::downloadTestFromPartition(){
  try {
    unsigned int downloadedDeviceNumber;

    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, versionMajor, versionMinor)->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDeviceTest::downloadTestFromPartitionAndId(){
  try {
    unsigned int downloadedDeviceNumber;

    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, fecHardwareId, versionMajor, versionMinor )->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDeviceTest::uploadInVersionUpdateTest(){
  try {
    unsigned int downloadedDeviceNumber;
    std::list<unsigned int*>*listeId = dbAccess->getDatabaseVersion(partitionName1);
    std::list<unsigned int*>::iterator it = listeId->begin();
    unsigned int *value = *it;
    int versionMajorId = value[1];
    
    // upload to partitionName1, currentMajorVersion.0
    deviceNumber = xmlFecDevicePartition->getDevices()->size();
    xmlFecDevicePartition->setDevices (partitionName1, 2);
    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, versionMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
    
    // upload to partitionName1, creation d'une partition
    int nextMinorId = dbAccess->getNextMinorVersion(versionMajorId);
    //upload to partitionName1, currentMajorVersion.nextMinorVersion
    xmlFecDevicePartition->setDevices (partitionName1, 0);
    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, versionMajorId, nextMinorId)->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
    
    int nextMajorId = dbAccess->getNextMajorVersion();
    //upload to partitionName1, majorVersion.minorVersion
    xmlFecDevicePartition->setDevices (partitionName1, 1);
    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, nextMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
    
    nextMinorId = dbAccess->getNextMinorVersion(0);
    // upload to partitionName1, 0.nextMinorVersion
    xmlFecDevicePartition->setDevices ( partitionName1, 3);
    // download from partitionName1 and version number 
    downloadedDeviceNumber = xmlFecDevicePartition->getDevices(partitionName1, nextMajorId, 0)->size();
    CPPUNIT_ASSERT( downloadedDeviceNumber == uploadedDeviceNumber);
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}
