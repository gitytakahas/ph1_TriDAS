#include "stringConv.h"

#include "DeviceFactory.h"

int main( int argc, char **argv ){

  std::string login="nil", passwd="nil", path="nil" ;
  std::string partitionName = "COUCOU" ;
  unsigned int dcuhardid = 0 ;

  if (argc > 1) { 
    partitionName = argv[1] ;
    if (argc > 2) dcuhardid = fromString<unsigned int>(argv[2]) ;
  }

  try {
    DbAccess::getDbConfiguration (login, passwd, path) ;
    
    if (login == "nil" || passwd=="nil" || path=="nil") {
      
      std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
      std::cerr << "Exiting ..." << std::endl ;
      return -1 ;
    }
    
    std::cout << "Creating a new DeviceFactory..." << std::endl ;
    DeviceFactory *deviceFactory = new DeviceFactory ( login, passwd, path ) ;
    std::cout << "--------------------------------" << std::endl ;
    std::cout << "Database version = " << deviceFactory->getDbVersion() << std::endl ;
    std::cout << "--------------------------------" << std::endl ;
    
    try {

      ////////////////////// DCU PSU MAP //////////////////////////
      /*
      std::cout << "Retrieving dcu-psu map from a file..." << std::endl ;
      ((TkDcuPsuMapFactory*)deviceFactory)->addFileName("/exports/slc4/development/FecSoftwareV3_0.dcupsu/ThirdParty/DeviceFactoryTemplate/xml/testDcuPsuMap.xml");
    
      std::cout << "Data read from file : " << std::endl ;
      Sgi::hash_map<unsigned long, TkDcuPsuMap *> map = deviceFactory->getDcuPsuMaps ( );
      Sgi::hash_map<unsigned long, TkDcuPsuMap *>::iterator i = map.begin();
      while(i!=map.end()){
	(*i).second->display();
	i++;
      }
      */

      unsigned int majorVersion;
      unsigned int minorVersion;
      
      std::cout << "Uploading the PSU names..." << std::endl ;      
      //created the list
      tkDcuPsuMapVector v ;
      TkDcuPsuMap* psu = new TkDcuPsuMap(0, "test", PSUDCUTYPE_CG);// we don't care for the dcuHardId, it's useless
      v.push_back(psu);
      psu = new TkDcuPsuMap(0, "test2", PSUDCUTYPE_CG);
      v.push_back(psu);
      psu = new TkDcuPsuMap(0, "test4", PSUDCUTYPE_PG);
      v.push_back(psu);

      //upload the PSU names
      deviceFactory->setTkPsuNames(v,partitionName,&majorVersion,&minorVersion) ;
      std::cout << "Upload of version "<< majorVersion << "." <<minorVersion << " just have been done" << std::endl ;  

      //delete the list
      TkDcuPsuMapFactory::deleteVectorI(v) ;

      std::cout << "Retrieving the PSU names (version "<<majorVersion<<"."<<minorVersion<<")..." << std::endl ;  
      
      deviceFactory->getPsuNamePartition(partitionName,majorVersion, minorVersion); // the vector v is managed by the factory
      std::cout << "Found " << deviceFactory->getControlGroupDcuPsuMaps().size() << " control groups" << std::endl ;
      std::cout << "Found " << deviceFactory->getPowerGroupDcuPsuMaps().size() << " control groups" << std::endl ;

      int psuNumber = deviceFactory->getControlGroupDcuPsuMaps().size();
      std::string psuNames[psuNumber];
      int i = 0;
      for (tkDcuPsuMapVector::iterator it = v.begin() ; it != v.end() ; it ++) {
      	psuNames[i] = (*it)->getPsuName();
	i++;
      }

      // DCU-PSU map creation
      tkDcuPsuMapVector map ;

      if (psuNumber > 0) { TkDcuPsuMap* psu1 = new TkDcuPsuMap(16001, psuNames[0], PSUDCUTYPE_PG); map.push_back(psu1); }
      if (psuNumber > 0) { TkDcuPsuMap* psu2 = new TkDcuPsuMap(16002, psuNames[0], PSUDCUTYPE_PG);map.push_back(psu2); }
      if (psuNumber > 1) { TkDcuPsuMap* psu3 = new TkDcuPsuMap(16003, psuNames[1], PSUDCUTYPE_CG);map.push_back(psu3); }
      if (psuNumber > 2) { TkDcuPsuMap* psu4 = new TkDcuPsuMap(16004, psuNames[2], PSUDCUTYPE_CG);map.push_back(psu4); }
      if (psuNumber > 2) { TkDcuPsuMap* psu5 = new TkDcuPsuMap(16005, psuNames[2], PSUDCUTYPE_CG);map.push_back(psu5); }
      
      std::cout << "Uploading the dcu-psu into the DB..." << std::endl ;
      deviceFactory->setUsingFile(false) ;      
      deviceFactory->setUsingDb(true) ;
      deviceFactory->setTkDcuPsuMap(map,partitionName); 
      
      std::cout << "Reseting the deviceFactory ..." << std::endl ;
      delete deviceFactory;
      deviceFactory = new DeviceFactory ( login, passwd, path ) ;

      std::cout << "Retrieving dcu-psu from the database ..." << std::endl ;
      deviceFactory->getPsuNamePartition(partitionName);

      std::cout << "Storing the dcuInfos into a file : /tmp/testDownload.xml ..." << std::endl ;
      deviceFactory->setUsingDb(false) ;
      deviceFactory->setUsingFile(true) ;      
      deviceFactory->setOutputFileName("/tmp/testDownload.xml") ;


      // delete the local vector
      TkDcuPsuMapFactory::deleteVectorI(map) ;
    } 
    catch (FecExceptionHandler &e) {

      cout << "Error : " <<e.what() <<endl;
      cout << e.what() <<endl;
    }
    
    delete deviceFactory;
    
  }
  catch (FecExceptionHandler &e) {

    std::cerr << "An error happends: " << e.what() << std::endl ;
    return -1 ;
  }
  catch (oracle::occi::SQLException &e) {

    std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
    std::cerr << e.what() ;
    std::cerr << "Exiting ..." << std::endl ;
    return -1 ;
  }
  return 0;
}
