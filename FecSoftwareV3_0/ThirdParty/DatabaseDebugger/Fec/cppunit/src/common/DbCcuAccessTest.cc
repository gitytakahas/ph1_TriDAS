#include "DbCcuAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbCcuAccessTest );

static const std::string ccuString("CCU ");
static const std::string partitionName1("Partition");
static const std::string partitionName2("OtherPartition");
static const std::string structureName("Structure");
static const std::string fileName("../../xml/testCcuTemplate.xml");

static const std::string fecHardwareId("0");
static const std::string dummyFecHardwareId("777");
static const unsigned int dummyRingSlot = 7;
static const unsigned int ringSlot = 0;

void DbCcuAccessTest::setUp(){
  try {
    dbAccess = new DbCcuAccess();
    uploadedCcuNumber = 0;
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbCcuAccessTest::tearDown(){
  delete dbAccess;
}

void DbCcuAccessTest::init(std::string file){
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
    while ((index = uploadedBuffer.find(ccuString, index+1))!= std::string::npos) {
      uploadedCcuNumber++;
    }
    fileStream.close();
  } else {
    std::cerr << "unable to open file named "<< fileName << std::endl;
  }
}

unsigned int DbCcuAccessTest::findStringInClob(oracle::occi::Clob *xmlClob, std::string name){
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

void DbCcuAccessTest::downloadTest(){
  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla","deNouilles",0)->length();
    clobLength = dbAccess->getXMLClob ("tireli","pin pon",1)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength);
    CPPUNIT_ASSERT( clobLength > 0 );
  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbCcuAccessTest::uploadTest(){
  try {
    init(fileName);
    CPPUNIT_ASSERT( uploadedCcuNumber  > 0);
    // upload to partitionName1, partition creation
    dbAccess->setXMLClob (buffer, partitionName1,  true);

    // upload to partitionName2, partition creation
    dbAccess->setXMLClob (buffer, partitionName2, true);

    // upload to partitionName1
    dbAccess->setXMLClob (buffer, partitionName1,  false);

    // upload to partitionName2, partition creation
    dbAccess->setXMLClob (buffer, partitionName2, false);

    emptyClobLength = dbAccess->getXMLClob ("hopla","deNouilles",0)->length();

    // download from partitionName1 with Ccu
    clobLength = dbAccess->getXMLClob (partitionName1, fecHardwareId, ringSlot)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
    // download from partitionName2 with Ccu
    clobLength = dbAccess->getXMLClob (partitionName2, fecHardwareId, ringSlot)->length();
    CPPUNIT_ASSERT( clobLength > emptyClobLength);
    
    delete[] buffer;

  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

void DbCcuAccessTest::downloadTestFromPartitionNameAndHardId(){
  init(fileName);
  CPPUNIT_ASSERT( uploadedCcuNumber  > 0);
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedCcuNumber;

  try {
    emptyClobLength = dbAccess->getXMLClob ("hopla","deNouilles",0)->length();
    
    // download from partitionName1, fecHardwareId, ringSlot
    xmlClob= dbAccess->getXMLClob (partitionName1, fecHardwareId, ringSlot);
    downloadedCcuNumber = findStringInClob(xmlClob, ccuString); 
    CPPUNIT_ASSERT( downloadedCcuNumber == uploadedCcuNumber  );

    // download from partitionName1, dummyCcuHardwareId and ringSlot
    clobLength = dbAccess->getXMLClob (partitionName1, dummyFecHardwareId, ringSlot)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
    // download from partitionName2, fecHardwareId, dummyRingSlot
    clobLength = dbAccess->getXMLClob (partitionName1, fecHardwareId, dummyRingSlot)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );

    // download from partitionName2, fecHardwareId, ringSlot
    xmlClob= dbAccess->getXMLClob (partitionName2, fecHardwareId, ringSlot);
    downloadedCcuNumber = findStringInClob(xmlClob, ccuString); 
    CPPUNIT_ASSERT( downloadedCcuNumber == uploadedCcuNumber  );
    
    // download from partitionName2,  dummyFecHardwareId, ringSlot
    clobLength = dbAccess->getXMLClob (partitionName2, dummyFecHardwareId, ringSlot)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );

    // download from partitionName2, fecHardwareId, dummyRingSlot
    clobLength = dbAccess->getXMLClob (partitionName2, fecHardwareId, dummyRingSlot)->length();
    CPPUNIT_ASSERT( clobLength == emptyClobLength );
    
    delete[] buffer;

  } catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

