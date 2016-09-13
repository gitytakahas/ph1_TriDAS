#include "DbFedAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbFedAccessTest );

static const std::string uFedString("RAWFED");
static const std::string uTtcrxString("RAWTTCRX");
static const std::string uApvString("RAWAPVFED");
static const std::string uStripString("RAWSTRIP");

static const std::string dFedString("fed ");
static const std::string dTtcrxString("ttcrx ");
static const std::string dApvString("apv ");
static const std::string dStripString("strips ");

static const std::string partitionName1("Partition");
static const std::string partitionName2("OtherPartition");
static const std::string structureName1("testStructure");
static const std::string dirName("../../xml/");

static const unsigned int fedHardwareId = 32;
static const unsigned int dummyFedHardwareId = 777;
static const unsigned int fedSoftwareId = 0;
static const unsigned int dummyFedSoftwareId = 777;

void DbFedAccessTest::setUp(){
  try {
    dbAccess = new DbFedAccess();

    std::string *xmlTest = new std::string[8];
    xmlBuffer = new std::string[8];
    std::string buffer[8];

    xmlTest[0] = dirName + "newTestFed.xml";
    xmlTest[1] = dirName + "newTestTtcrx.xml";
    xmlTest[2] = dirName + "newTestVoltageMonitor.xml";
    xmlTest[3] = dirName + "newTestFeFpga.xml";
    xmlTest[4] = dirName + "newTestChannelPair.xml";
    xmlTest[5] = dirName + "newTestChannel.xml";
    xmlTest[6] = dirName + "newTestApvFed.xml";
    xmlTest[7] = dirName + "newTestStrip.xml";


    /* lecture des fichiers */
    for (int i = 0; i< 8; i++) {
      Utils::init(xmlTest[i], &buffer[i]);
      xmlBuffer[i] = std::string("<ROWSET>") + buffer[i] + std::string("</ROWSET>") ;
    }

    /* count the <TAG> and </TAG> */
    uploadedFedNumber = Utils::findStringInBuffer(buffer[0], uFedString)/2;
    uploadedTtcrxNumber = Utils::findStringInBuffer(buffer[1], uTtcrxString)/2;
    uploadedApvNumber = Utils::findStringInBuffer(buffer[6], uApvString)/2;
    uploadedStripNumber = Utils::findStringInBuffer(buffer[7], uStripString)/2;

    CPPUNIT_ASSERT( uploadedFedNumber  > 0);
    CPPUNIT_ASSERT( uploadedTtcrxNumber  > 0);
    CPPUNIT_ASSERT( uploadedApvNumber  > 0);

    delete[] xmlTest;
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::tearDown(){
  delete dbAccess;
  delete[] buffer;
}

void DbFedAccessTest::downloadTest(){
  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    clobLength = dbAccess->getXMLClob ("deNouilles")->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength);
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::uploadTest(){
  try {
    // upload to partitionName1
    dbAccess->setXMLClob (xmlBuffer, partitionName1, 1);

    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    
    // download from partitionName1 with Strip
    clobLength = dbAccess->getXMLClob (partitionName1)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength );

  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::downloadTestFromPartitionName(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedStripNumber;
  unsigned int  downloadedApvNumber;
  try {
    // download from partitionName1 with strip
    xmlClob = dbAccess->getXMLClob (partitionName1, true);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT( downloadedStripNumber == uploadedStripNumber  );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );
    
    // download from partitionName1 without strip
    xmlClob = dbAccess->getXMLClob (partitionName1, false);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT( downloadedStripNumber == 0 );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );
    
    delete[] buffer;
  }catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::downloadTestFromPartitionNameAndHardId(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedStripNumber;
  unsigned int  downloadedApvNumber;

  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();
    
    // download from partitionName1 and fedHardwareId with strip
    xmlClob= dbAccess->getXMLClob (partitionName1, fedHardwareId, true, true);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT( downloadedStripNumber == uploadedStripNumber  );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );
    
  // download from partitionName1 and fedHardwareId without strip
    xmlClob = dbAccess->getXMLClob (partitionName1, fedHardwareId, true, false);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT(  downloadedStripNumber == 0 );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );
    
    // download from partitionName1 and dummyFedHardwareId with strip
    clobLength = dbAccess->getXMLClob (partitionName1, dummyFedHardwareId, true, true)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
    delete[] buffer;

  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::downloadTestFromPartitionNameAndSoftId(){
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedStripNumber;
  unsigned int  downloadedApvNumber;

  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla")->length();

    // download from partitionName1 and fedSoftwareId with strip  
    xmlClob= dbAccess->getXMLClob (partitionName1, fedSoftwareId, false, true);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT( downloadedStripNumber == uploadedStripNumber  );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );

    // download from partitionName1 and fedSoftwareId without strip  
    xmlClob = dbAccess->getXMLClob (partitionName1, fedSoftwareId, false, false);
    downloadedStripNumber = Utils::findStringInClob(xmlClob, dStripString); 
    CPPUNIT_ASSERT(  downloadedStripNumber == 0 );
    downloadedApvNumber = Utils::findStringInClob(xmlClob, dApvString); 
    CPPUNIT_ASSERT( downloadedApvNumber == uploadedApvNumber  );
    
    // download from partitionName1 and dummyfedSoftwareId with strip  
    clobLength = dbAccess->getXMLClob (partitionName1, dummyFedSoftwareId, false, true)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
    delete[] buffer;

  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbFedAccessTest::versionTest(){
  try {
    CPPUNIT_ASSERT(  dbAccess->getNextMinorVersion(0) ==  dbAccess->getMaxMinorVersion()+1);

    std::list<unsigned int*> *listeValues;
    listeValues = dbAccess->getDatabaseVersion(partitionName1);
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
    unsigned int *value = *it;
    CPPUNIT_ASSERT(  dbAccess->getPartitionName(value[0]) >= partitionName1 );
    CPPUNIT_ASSERT(  dbAccess->getNextMajorVersion() >= value[1] );
    CPPUNIT_ASSERT(  dbAccess->getNextMinorVersion(value[1]) >= value[2] );
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
    }
    
    for (std::list<unsigned int*>::iterator it = listeValues->begin() ; it != listeValues->end() ; it ++) {
      delete[] *it;
    }
    delete listeValues;
    
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

