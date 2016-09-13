#include "DbTkDcuConversionAccessTest.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DbTkDcuConversionAccessTest );

static const std::string dcuConvertionString("DCUCONVERSION ");
static const std::string dcuInfoString("DCUINFO ");
static const std::string fileName("../../xml/testTkDcuConversionTemplate.xml");
static const std::string fileName2("../../xml/testTkDcuInfoTemplate.xml");
static const tscType32 dcuHardId(70144);
static const std::string partitionName("Partition");

void DbTkDcuConversionAccessTest::setUp(){
  try {
    dbAccess = new DbTkDcuConversionAccess();
    uploadedTkDcuConversionNumber = 0;
    xmlConversion = new XMLTkDcuConversion();
    xmlConversion->setDatabaseAccess(dbAccess);
    xmlConversionFile = new XMLTkDcuConversion(fileName);

    dbInfoAccess = new DbTkDcuInfoAccess();
    xmlInfo = new XMLTkDcuInfo();
    xmlInfo->setDatabaseAccess(dbInfoAccess);
    xmlInfoFile = new XMLTkDcuInfo(fileName2);

    //XMLTkDcuConversion xmlConversion;
    //xmlConversion.setDatabaseAccess(&dbAccess);
    //XMLTkDcuConversion xmlConversionFile(fileName);

    /*
      Utils::init(fileName, &buffer);
      uploadedPiaResetNumber = Utils::findStringInBuffer(buffer, piaResetString);
    CPPUNIT_ASSERT( uploadedPiaResetNumber  > 0);
    */
  } 
  catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
  catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());   
  }
}

void DbTkDcuConversionAccessTest::tearDown(){
  delete dbAccess;
  delete xmlConversion;
  delete xmlConversionFile;

  delete dbInfoAccess;
  delete xmlInfo;
  delete xmlInfoFile;
}

void DbTkDcuConversionAccessTest::init(std::string file){
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
    while ((index = uploadedBuffer.find(dcuConvertionString, index+1))!= std::string::npos) {
      uploadedTkDcuConversionNumber++;
    }
    fileStream.close();
  } else {
    std::cerr << "unable to open file named : " << fileName << std::endl;
  }
}

unsigned int DbTkDcuConversionAccessTest::findStringInClob(oracle::occi::Clob *xmlClob, std::string name){
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
	  //	 std::string::size_type newIndex  = 0;
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

void DbTkDcuConversionAccessTest::uploadTest(){
  try {
    init(fileName);
    CPPUNIT_ASSERT( uploadedTkDcuConversionNumber  > 0);
    // upload 
    dbAccess->setXMLClob (buffer);
    delete[] buffer;
    
  } catch (oracle::occi::SQLException &e) {
    std::string errorMessage = e.what();
    if ((errorMessage.find("exit called from Java code with status 0",0)) != std::string::npos || (errorMessage.find("Java call terminated by uncaught Java exception",0)) != std::string::npos) {
      errorMessage = dbAccess->getErrorMessage();
    }
    CPPUNIT_FAIL(errorMessage);
  }
}

void DbTkDcuConversionAccessTest::uploadToFileTest(){
  try {

    xmlConversionFile->getDcuConversions();
    xmlConversionFile->setDcuConversions("test_result.xml");
    tkDcuInfoVector* t = xmlInfo->getAllDcuInfos();
    /*    
    for ( tkDcuInfoVector::iterator it = t->begin() ; it != t->end() ; it ++) {
      (*it)->display();
    }
    */
    xmlInfo->setDcuInfos("test_resultInfo.xml");

    } catch (FecExceptionHandler &e) {
      CPPUNIT_FAIL(e.what());
    }
    catch (oracle::occi::SQLException &e) {
      CPPUNIT_FAIL(e.what());
    }
}

void DbTkDcuConversionAccessTest::uploadToDatabaseTest(){
  try {

    xmlConversionFile->getDcuConversions();
    xmlConversion->setDcuConversionVector(xmlConversionFile->getDcuConversionVector());
    xmlConversion->setDcuConversions();

    xmlInfoFile->getDcuInfos();
    xmlInfo->setDcuInfoVector(xmlInfoFile->getDcuInfoVector());
    xmlInfo->setDcuInfos();

    } catch (FecExceptionHandler &e) {
      CPPUNIT_FAIL(e.what());
    }
    catch (oracle::occi::SQLException &e) {
      CPPUNIT_FAIL(e.what());
    }
}

void DbTkDcuConversionAccessTest::downloadStateFromDatabaseTest(){
  init(fileName);
  CPPUNIT_ASSERT( uploadedTkDcuConversionNumber  > 0);
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedTkDcuConversionNumber;

  try {
    std::cout<<std::endl;
    tkStateVector* l = xmlInfo->getAllCurrentStates();
    CPPUNIT_ASSERT( l->size() > 0);
    for ( tkStateVector::iterator it = l->begin() ; it != l->end() ; it ++) {
      (*it)->display();
      delete(*it);
    }
    l->clear();
    delete l;

    std::cout<<std::endl;
    tkVersionVector* l2 = xmlInfo->getAllPiaResetVersions();
    CPPUNIT_ASSERT( l2->size() > 0);
    for ( tkVersionVector::iterator it = l2->begin() ; it != l2->end() ; it ++) {
      (*it)->display();
      delete(*it);
    }
    l2->clear();
    delete l2;
  } 
  catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
  catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}


void DbTkDcuConversionAccessTest::downloadTest(){
  init(fileName);
  CPPUNIT_ASSERT( uploadedTkDcuConversionNumber  > 0);
  oracle::occi::Clob *xmlClob;
  unsigned int  downloadedTkDcuConversionNumber;

  try {
    // download from partitionName1 with pll
    xmlClob = dbAccess->getXMLClob (dcuHardId);
    downloadedTkDcuConversionNumber = findStringInClob(xmlClob, dcuConvertionString); 
    CPPUNIT_ASSERT( downloadedTkDcuConversionNumber == 1 );

     /*int downloadedTkDcuInfoNumber = */findStringInClob(xmlClob, dcuInfoString); 
    //CPPUNIT_ASSERT( downloadedTkDcuInfoNumber == 2 );

    delete[] buffer;

  } 
  catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
  catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  }
}

void DbTkDcuConversionAccessTest::downloadFromDatabaseTest(){
  //init(fileName);
  //CPPUNIT_ASSERT( uploadedTkDcuConversionNumber  > 0);
  //  for(int i=0;i<150;i++){
    try {
      //tkDcuInfoVector* l = xmlInfo->getDcuInfos("Partition");
      //tkDcuInfoVector* l = xmlInfo->getAllDcuInfos();
    
      //l[0]->display();
      //dcuConversionVector* l = xmlConversion->getDcuConversions(partitionName);
      //std::cout<<l->size()<<std::endl;
      //CPPUNIT_ASSERT( l->size() == 1 );
      //delete[] buffer;
      
    } 
    catch (FecExceptionHandler &e) {
      std::cout<<e.what()<<std::endl;
      //CPPUNIT_FAIL(e.what());
    }
    catch (oracle::occi::SQLException &e) {
      //CPPUNIT_FAIL(e.what());
    } 
    //}
}

void DbTkDcuConversionAccessTest::downloadFromFileTest(){
  init(fileName);
  CPPUNIT_ASSERT( uploadedTkDcuConversionNumber  > 0);
  
  try {
    
    dcuConversionVector* l = xmlConversionFile->getDcuConversions();
    CPPUNIT_ASSERT( l->size() == 4 );

    tkDcuInfoVector* l2 = xmlInfoFile->getDcuInfos();
    CPPUNIT_ASSERT( l2->size() == 4 );

    delete[] buffer;

  } 
  catch (FecExceptionHandler &e) {
    CPPUNIT_FAIL(e.what());
  }
  catch (oracle::occi::SQLException &e) {
    CPPUNIT_FAIL(e.what());
  } 
}
    
