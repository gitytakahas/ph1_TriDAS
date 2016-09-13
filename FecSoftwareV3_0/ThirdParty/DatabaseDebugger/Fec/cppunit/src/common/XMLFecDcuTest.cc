#include "XMLFecDcuTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( XMLFecDcuTest );

static const std::string dcuString("DCU ");
static const std::string partitionName1("Partition");
static const std::string partitionName2("OtherPartition");
static const std::string partitionName3("DummyPartition");
static const std::string structureName("Structure");
static const std::string fileName("../../xml/testFecTemplate.xml");
static const std::string supervisorIp("155.166.60.239");
static const std::string className("FecSupervisor");

static const std::string fecHardwareId("ABC");
static const std::string dummyFecHardwareId("CBA");
static const unsigned int xdaqInstance = 0;
static const long dcuHardwareId = 12;

void XMLFecDcuTest::setUp(){
  try {
    dbAccess = new DbFecAccess();
    xmlFecDcu = new XMLFecDcu();
    xmlFecDcu->setDatabaseAccess(dbAccess);
    xmlFecDcuFile = new XMLFecDcu(fileName);
    xmlFecDcuFile->setDatabaseAccess(dbAccess);
    xmlFecDcuPartition = new XMLFecDcu(dbAccess, partitionName1);
    xmlFecDcuPartitionTime = new XMLFecDcu(dbAccess, partitionName1, 0, 1000000);
    xmlFecDcuPartitionIp = new XMLFecDcu(dbAccess, partitionName1, supervisorIp, xdaqInstance, className);
    xmlFecDcuPartitionIpTime = new XMLFecDcu(dbAccess, partitionName1, supervisorIp, xdaqInstance, className, 0, 1000000);
    xmlFecDcuPartitionFecHardId = new XMLFecDcu(dbAccess, partitionName1, fecHardwareId);
    xmlFecDcuPartitionFecHardIdTime = new XMLFecDcu(dbAccess, partitionName1, fecHardwareId, 0, 1000000);
    xmlFecDcuDcuHardId = new XMLFecDcu(dbAccess, dcuHardwareId);
    xmlFecDcuDcuHardIdTime = new XMLFecDcu(dbAccess, dcuHardwareId, 0, 1000000);
    xmlFecDcuPartitionDcuHardId = new XMLFecDcu(dbAccess, partitionName1, dcuHardwareId);
    xmlFecDcuPartitionDcuHardIdTime = new XMLFecDcu(dbAccess, partitionName1, dcuHardwareId, 0, 1000000);
    xmlFecDcuTime = new XMLFecDcu(dbAccess, 0, 1000000);
    xmlFecDcuDummyPartition = new XMLFecDcu(dbAccess, partitionName3);
    uploadedDcuNumber = 0;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::tearDown(){
  delete xmlFecDcu;
  delete xmlFecDcuFile;
  delete xmlFecDcuPartition;
  delete xmlFecDcuPartitionTime;
  delete xmlFecDcuPartitionFecHardId;
  delete xmlFecDcuPartitionFecHardIdTime;
  delete xmlFecDcuPartitionDcuHardId;
  delete xmlFecDcuPartitionDcuHardIdTime;
  delete xmlFecDcuDcuHardId;
  delete xmlFecDcuDcuHardIdTime;
  delete xmlFecDcuTime;
}

void XMLFecDcuTest::init(std::string file){
  std::ifstream fileStream((const char *)file.c_str());
  if (fileStream) { 
    // get pointer to associated buffer object
    std::filebuf *pbuf = fileStream.rdbuf();
    
    // get file size using buffer's members
    unsigned int size = pbuf->pubseekoff (0,std::ios::end,std::ios::in);
    pbuf->pubseekpos (0,std::ios::in);
    
    // allocate a character buffer
    buffer=new char[size+1];

    // get file data  
    pbuf->sgetn (buffer,size);

    std::string uploadedBuffer((const char *)buffer, size);
    std::string::size_type index = 0;
    while ((index = uploadedBuffer.find(dcuString, index+1))!= std::string::npos) {
      uploadedDcuNumber++;
    }

    fileStream.close();
  } else {
    std::cerr << "unable to open file named "<< fileName << std::endl;
  }
}

unsigned int XMLFecDcuTest::findStringInClob(oracle::occi::Clob *xmlClob, std::string name){
  unsigned int number = 0;
  XMLByte *xmlBuffer;
  if (xmlClob != NULL){
    unsigned int resultLength = (*xmlClob).length();
    if (resultLength>0) {
      if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char))) != NULL) {
	if (resultLength == (*xmlClob).read(resultLength, xmlBuffer, resultLength+1)){
	  const std::string xmlBufferId = "myXmlBuffer";
	  // std::cout << xmlBuffer << std::endl;
	  std::string downloadedBuffer((const char *)xmlBuffer);
	  // size_type find_first_of( const std::string &str, size_type index = 0 );
	  std::string::size_type index = 0;
	  //	  std::string::size_type newIndex  = 0;
	  while ((index = downloadedBuffer.find(name, index+1))!= std::string::npos) {
	    number++;
	  }
	} else {
	  std::cerr << "failed while reading the XML Clob"  << std::endl;
	}
      } else {
	std::cerr << "failed during memory allocation"  << std::endl;  		  }
    } else {
      std::cerr << "no data found... " << std::endl;
    }
  } else {
    std::cerr << "failed while trying to get the XML Clob"  << std::endl;
  }
  free(xmlBuffer);
  return number;
}

void XMLFecDcuTest::downloadTest(){
  init(fileName);
  try {
    CPPUNIT_ASSERT( xmlFecDcuDummyPartition->getDevices()->size() == 0);

    unsigned int dcuNumber;

    dcuNumber = xmlFecDcuPartition->getDevices()->size();
    CPPUNIT_ASSERT( dcuNumber == uploadedDcuNumber );

    dcuNumber = xmlFecDcuPartitionFecHardId->getDevices()->size();
    CPPUNIT_ASSERT( dcuNumber == uploadedDcuNumber );
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::uploadTest(){
  init(fileName);
  try {
    unsigned int dcuNumber;

    // upload to partitionName1 
    dcuNumber = xmlFecDcuPartition->getDevices()->size();
    CPPUNIT_ASSERT( dcuNumber > 0 );
    xmlFecDcuPartition->setDevices (partitionName1, 0 ,0);
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}
    
void XMLFecDcuTest::downloadTestFromPartition(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1
    downloadedDcuNumber = xmlFecDcuPartition->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber == uploadedDcuNumber );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestFromPartitionAndTime(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1 between time stamp 0 and time stamp 1 000 000 ( 0 and 999 999 )
    downloadedDcuNumber = xmlFecDcuPartitionTime->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber == uploadedDcuNumber);
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}
     
void XMLFecDcuTest::downloadTestFromPartitionId(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, fecHardId 
    downloadedDcuNumber = xmlFecDcuPartitionFecHardId->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber == uploadedDcuNumber );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestFromPartitionIdAndTime(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, fecHardId between time stamp 0 and time stamp 1 000 000 ( 0 and 999 999 )
    downloadedDcuNumber = xmlFecDcuPartitionFecHardIdTime->getDevices()->size();
    CPPUNIT_ASSERT( uploadedDcuNumber%downloadedDcuNumber == 0 );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}
     
void XMLFecDcuTest::downloadTestFromPartitionDcuId(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, dcuHardId
    downloadedDcuNumber = xmlFecDcuPartitionDcuHardId->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber > 0 );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestFromPartitionDcuIdAndTime(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, dcuHardId between time stamp 0 and time stamp 1 000 000 ( 0 and 999 999 ) 
    downloadedDcuNumber = xmlFecDcuPartitionDcuHardIdTime->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber > 0 );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestFromDcuId(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, dcuHardId
    downloadedDcuNumber = xmlFecDcuDcuHardId->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber > 0 );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestFromDcuIdAndTime(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
    // download from partitionName1, dcuHardId between time stamp 0 and time stamp 1 000 000 ( 0 and 999 999 ) 
    downloadedDcuNumber = xmlFecDcuDcuHardIdTime->getDevices()->size();
    CPPUNIT_ASSERT( downloadedDcuNumber > 0 );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void XMLFecDcuTest::downloadTestAll(){
  init(fileName);
  try {
    unsigned int downloadedDcuNumber;
     
    downloadedDcuNumber = xmlFecDcuTime->getDevices()->size();
    CPPUNIT_ASSERT( uploadedDcuNumber = downloadedDcuNumber );
    delete[] buffer;
  } catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
}



