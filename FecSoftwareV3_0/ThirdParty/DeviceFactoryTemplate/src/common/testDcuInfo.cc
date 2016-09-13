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

    //deviceFactory->addDetIdPartition(partitionName);

    try {

      ////////////////////// DCU INFOS //////////////////////////
      /*
      std::cout << "Retrieving dcuInfos from a file..." << std::endl ;
      ((TkDcuInfoFactory*)deviceFactory)->addFileName("/exports/xdaq/users/fdrouhin/FecSoftwareV3_0/ThirdParty/DeviceFactoryTemplate/xml/TIDDcuInfo20070621.xml");

      std::cout << "Creating a new DcuInfo version..." << std::endl ;
      deviceFactory->createNewDcuInfoVersion();
      
      std::cout << "Uploading the dcuInfos into the DB..." << std::endl ;
      deviceFactory->setUsingFile(false) ;      
      deviceFactory->setUsingDb(true) ;
      deviceFactory->setTkDcuInfo();
      
      std::cout << "Reseting the deviceFactory ..." << std::endl ;
      delete deviceFactory;
      deviceFactory = new DeviceFactory ( login, passwd, path ) ;
      
      std::cout << "Retrieving dcuInfos from the database (last version)..." << std::endl ;
      deviceFactory->addAllDetId();

      std::cout << "Storing the dcuInfos into a file : /tmp/testDownload2.xml ..." << std::endl ;
      deviceFactory->setUsingDb(false) ;
      deviceFactory->setUsingFile(true) ;      
      deviceFactory->setOutputFileName("/tmp/testDownload2.xml") ;
      deviceFactory->setTkDcuInfo();
      
      
      /////////////////////// DCU CONVERSIONS ////////////////////////////////
      
      std::cout << "Reseting the deviceFactory ..." << std::endl ;
      delete deviceFactory;
      deviceFactory = new DeviceFactory ( login, passwd, path ) ;

      std::cout << "Retrieving dcuConversions from a file..." << std::endl ;
      ((TkDcuConversionFactory*)deviceFactory)->addFileName("/exports/xdaq/users/fdrouhin/FecSoftwareV3_0/ThirdParty/DeviceFactoryTemplate/xml/testDcuConversion.xml");
      
      std::cout << "Uploading the dcuConversion into the DB..." << std::endl ;
      deviceFactory->setUsingFile(false) ;      
      deviceFactory->setUsingDb(true) ;
      deviceFactory->setTkDcuConversionFactors();

      std::cout << "Reseting the deviceFactory ..." << std::endl ;
      delete deviceFactory;
      deviceFactory = new DeviceFactory ( login, passwd, path ) ;
      
      std::cout << "Retrieving dcuConversions from the database..." << std::endl ;
      deviceFactory->addConversionPartition("PARTITION_1");

      std::cout << "Uploading the dcuInfos into a file : /tmp/testDownloadConversions.xml ..." << std::endl ;
      deviceFactory->setUsingDb(false) ;
      deviceFactory->setUsingFile(true) ;      
      deviceFactory->setOutputFileName("/tmp/testDownloadConversions.xml") ;
      deviceFactory->setTkDcuConversionFactors();
      */
      ////////////////////////// RUN //////////////////////////////////////////
      /*
      std::cout << "Creating a run (Nb 7) on partition TEST1..." << std::endl ;
      deviceFactory->setRun("TEST1", 7, 4, 0);
      std::cout<<"Updating the run comment ... "<<std::endl;
      deviceFactory->updateRunComment("TEST1",7,"Nouveau commentaire!!");
      std::cout << "Stop the run on partition TEST1" << std::endl ;
      deviceFactory->stopRun("TEST1");
      
      std::cout<<"Retrieve All the Runs from the DB "<<std::endl;
      tkRunVector *l = deviceFactory->getAllRuns();
      for ( tkRunVector::iterator it = l->begin() ; it != l->end() ; it ++) {
	(*it)->display();
      }
      delete(l);
      */

      std::cout << "Retriving an old state (coucou1)..." << std::endl ;
      unsigned int newStateId = deviceFactory->setCurrentState("coucou1");
      std::cout << "Old state retrived in current state (" << newStateId << ")" << std::endl ;

      std::cout << "Test completed." << std::endl ;
    } 
    catch (FecExceptionHandler &e) {

      cout << "Error : " <<e.what() <<endl;
      cout << e.what() <<endl;
    }

    //std::cout<<"Trying to take off one module... "<<std::endl;
    //deviceFactory->removeModule("ROD1", "ROD1UPDATED2", "ROD1UPDATED2",  6222151);

        
    //std::cout<<"Get the state names : "<<std::endl;
    //std::list<std::string>* nameList = deviceFactory->getAllStateNames();

    //for ( std::list<std::string>::iterator it = nameList->begin() ; it != nameList->end() ; it ++) {
    //  std::cout<<(*it)<<std::endl;
    //}

    //delete nameList;
    
    //std::cout<<"Changing the current state... "<<std::endl;
    //deviceFactory->setCurrentState("ALLERENCOREUNDERNIER");
    
    //std::cout<<"Delete the connections of work_crack_06... "<<std::endl;
    //deviceFactory->destroyConnections("work_crack_06");

    //std::cout<<"Creating a new run... "<<std::endl;
    //deviceFactory->setRun("testPartition1",34,6,1);
    //std::cout<<"Updating a run comment ... "<<std::endl;
    //deviceFactory->updateRunComment("YOMTEST3",26,"Yop! Nouveau commentaire!!");
    /*
    std::cout<<"Retrieve last run for YOMTEST3 "<<std::endl;
    TkRun *run = deviceFactory->getLastRun("YOMTEST3");
    if(run!=0){
      run->display();
      delete(run);
    }
    else{
      std::cout<<"No record found."<<std::endl;
    }
    */
    //std::cout<<"Tag last run as transfered by O2O"<<std::endl;
    //deviceFactory->setO2ORun("FREDTEST2",11);

    /*
    std::cout<<"Retrieve last O2Orun for FREDTEST2 "<<std::endl;
    run = deviceFactory->getLastO2ORun("FREDTEST2");
    if(run!=0){
      run->display();
      delete(run);
    }
    else{
      std::cout<<"No record found."<<std::endl;
    }
    */

    //std::cout<<"Retrieve All the Runs from the DB "<<std::endl;
    //tkRunVector *l = deviceFactory->getAllRuns();
    //for ( tkRunVector::iterator it = l->begin() ; it != l->end() ; it ++) {
    //  (*it)->display();
    //}
    //delete(l);
    

    //std::cout<<"Load a file in DcuInfo"<<std::endl;
    //((TkDcuInfoFactory*)deviceFactory)->setInputFileName("070109DcuInfo.xml");
    //deviceFactory->setUsingDb(false) ;
    //std::cout<<"Retrieve the DcuInfos from the database..."<<std::endl;
    //deviceFactory->addAllDetId();

    //std::cout<<"Write in the file 070123BackUpTOBDB.xml"<<std::endl;
    //deviceFactory->setUsingDb(false) ;
    //deviceFactory->setOutputFileName("070123BackUpTOBDB.xml") ;
    //deviceFactory->setUsingFile(true) ;
    //deviceFactory->setUsingDb(true) ;
    //deviceFactory->setTkDcuInfo();
    
    
    //std::vector <unsigned int> dcuHardIds;
    //dcuHardIds.push_back(5421);
    //dcuHardIds.push_back(13761779);
    //dcuHardIds.push_back(11795675);
    //deviceFactory->enableDevice("work_crack_07", dcuHardIds) ;

    //std::cout<<"Load a file in DcuConversion"<<std::endl;
    //((TkDcuConversionFactory*)deviceFactory)->setInputFileName("dcuconv_TIB.xml");
    
    //std::cout<<"Get the conversion factors for MTCC_TEST1"<<std::endl;

    //deviceFactory->addConversionPartition("MTCC_TEST1");
    //Sgi::hash_map<unsigned long, TkDcuConversionFactors *> map = deviceFactory->getConversionFactors ();

    //Sgi::hash_map<unsigned long, TkDcuConversionFactors *>::iterator i = map.begin();
    //while(i!=map.end()){
    //  (*i).second->display();
    //  i++;
    //}
    

    //deviceFactory->setOutputFileName("testRobOutput.xml") ;
    //deviceFactory->setUsingDb(true);
    //deviceFactory->setTkDcuConversionFactors();
 
    //std::cout<<"Get the current states..."<<std::endl;
    //tkStateVector* s = deviceFactory->getCurrentStates();
    /*
    tkStateVector *stateVector = new tkStateVector();
    TkState* t1 = new TkState("Insertion test", "TELTEST", 1, 0, 1, 0, 1, 0, 0, 0, 0);
    TkState* t2 = new TkState("Insertion test", "TELTEST1", 1, 0, 1, 0, 1, 0, 0, 0, 0);
    TkState* t3 = new TkState("Insertion test", "TELTEST2", 1, 0, 1, 0, 1, 0, 0, 0, 0);
    stateVector->push_back(t1);
    stateVector->push_back(t2);
    stateVector->push_back(t3);

    deviceFactory->setCurrentState(*stateVector);

    for ( tkStateVector::iterator it = stateVector->begin() ; it != stateVector->end() ; it ++) {
      delete(*it);
    }
    delete stateVector;
    */
    /*
    for ( tkStateVector::iterator it = s->begin() ; it != s->end() ; it ++) {
      (*it)->display();
    }

    for ( tkStateVector::iterator it = s->begin() ; it != s->end() ; it ++) {

      std::string partitionName = (*it)->getPartitionName() ;
      
      std::cout<<"\nGet the fec versions for partition " << partitionName <<std::endl;
      tkVersionVector* l = deviceFactory->getAllFecVersions(partitionName);
      
      for ( tkVersionVector::iterator it = l->begin() ; it != l->end() ; it ++) {
	(*it)->display();
      }      TkDcuInfoFactory::deleteVersionVector(*l);
      delete l;

      std::cout<<"\nGet the fed versions for partition " << partitionName <<std::endl;
      l = deviceFactory->getAllFedVersions(partitionName);
      for ( tkVersionVector::iterator it = l->begin() ; it != l->end() ; it ++) {
	(*it)->display();
      }
      TkDcuInfoFactory::deleteVersionVector(*l);
      delete l;

      std::cout<<"\nGet the Pia Reset versions for partition " << partitionName <<std::endl;
      l = deviceFactory->getAllPiaResetVersions(partitionName);
      for ( tkVersionVector::iterator it = l->begin() ; it != l->end() ; it ++) {
	(*it)->display();
      }
      TkDcuInfoFactory::deleteVersionVector(*l);
      delete l;

      std::cout<<"\n********************************************"<<std::endl;
    }
    */
    //TkDcuInfoFactory::deleteStateVector(*s);
    //delete s;
    
    //std::cout<<"Update of the channel delays..."<<std::endl;
    //deviceFactory->updateChannelDelays();

    //TkDcuConversionFactors *conversionFactors = deviceFactory->getTkDcuConversionFactors (805310153) ;

    /*
    std::cout<<"ok"<<std::endl;
    
    try {
      
      std::cout << "Get the conversion factors for each DCU in the database" << std::endl ;
      for (int i=0;i<50;i++) {
	
	try {
	  std::cout<<"Get conversion factors for dcu_id : "<<i<<std::endl;
	  TkDcuConversionFactors *conversionFactors = deviceFactory->getTkDcuConversionFactors ( i ) ;
	  std::cout<<"OK!"<<std::endl;
	  
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "No conversion factors for the DCU "<< i << ")" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	}
      }
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*********** ERROR *************" << std::endl ;
      std::cerr << "XML parsing for database error " << e.what() << std::endl ;
      std::cerr << "*******************************" << std::endl ;
    }
    */
            
    //std::cout<<"Reading a file... "<<std::endl;

    //deviceFactory->setUsingFile(true);
    //deviceFactory->setUsingDb(false) ;
    //((TkDcuInfoFactory*)deviceFactory)->setInputFileName ("AllDcuInfo061115.xml") ;
    //deviceFactory->addAllDetId();
    //deviceFactory->setUsingDb(true) ;
    //deviceFactory->setUsingFile(false) ;
    //deviceFactory->setTkDcuInfo();
    
    
    /*
    std::cout<<"Check the dcuHardId 85475..."<<std::endl;
    
    TkDcuInfo *infos = tkDcuInfoFactory.getTkDcuInfo ( 85475 ) ;
    infos->display();
    
    std::cout<<"Writing data to an output file (testOutput.xml)..."<<std::endl;

    tkDcuInfoFactory.setOutputFileName("testOutput.xml") ;
    tkDcuInfoFactory.setTkDcuInfo();

    std::cout<<"Adding a new detector..."<<std::endl;
    Sgi::hash_map<unsigned long, TkDcuInfo *> map = tkDcuInfoFactory.getInfos ( );
    TkDcuInfo *newDcu = new TkDcuInfo(123, 485284574, 5487.5, 6);
    map[newDcu->getDcuHardId()] = newDcu->clone() ;
    delete newDcu;

    std::cout<<"Writing data to the database..."<<std::endl;
    tkDcuInfoFactory.setDatabaseAccess (login, passwd, path) ;
    tkDcuInfoFactory.setUsingDb() ;
    tkDcuInfoFactory.setTkDcuInfo(map);//send the given map in the DB (does not update the factory internal map!)

    std::cout<<"Get data from the database (partition TEC+)..."<<std::endl;
    tkDcuInfoFactory.addDetIdPartition("TEC+");
    std::cout<<"Display data :"<<std::endl;
    Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator i = map.begin();
    while(i!=map.end()){
      (*i).second->display();
      i++;
    }
    */
    //std::cout<<"Test of conversion factors (extract TOB partition info and upload in the file testConversionOutput.xml)..."<<std::endl;
    //TkDcuConversionFactory tkDcuConversionFactory (false) ;
    //std::cout<<"Reading DCU Conversions file"<<std::endl;
    //((TkDcuConversionFactory*)deviceFactory)->setInputFileName ("dcuconv_TIB.xml") ;

    

    /*
    std::cout<<"Creating new conversion factors..."<<std::endl;

    dcuConversionVector vConversionFactors ;

    // Create the corresponding parameters
    TkDcuConversionFactors tkDcuConversionFactorsStatic ( 0, "TEC", DCUFEH ) ;
    tkDcuConversionFactorsStatic.setAdcGain0(2.145) ;
    tkDcuConversionFactorsStatic.setAdcOffset0(0) ;
    tkDcuConversionFactorsStatic.setAdcCal0(false) ;
    tkDcuConversionFactorsStatic.setAdcInl0(0) ;
    tkDcuConversionFactorsStatic.setAdcInl0OW(true) ;
    tkDcuConversionFactorsStatic.setI20(0.02122);
    tkDcuConversionFactorsStatic.setI10(.01061);
    tkDcuConversionFactorsStatic.setICal(false) ;
    tkDcuConversionFactorsStatic.setKDiv(0.56) ;
    tkDcuConversionFactorsStatic.setKDivCal(false) ;
    tkDcuConversionFactorsStatic.setTsGain(8.9) ;
    tkDcuConversionFactorsStatic.setTsOffset(2432) ;
    tkDcuConversionFactorsStatic.setTsCal(false) ;
    tkDcuConversionFactorsStatic.setR68(0) ;
    tkDcuConversionFactorsStatic.setR68Cal(false) ;
    tkDcuConversionFactorsStatic.setAdcGain2(0) ;
    tkDcuConversionFactorsStatic.setAdcOffset2(0) ;
    tkDcuConversionFactorsStatic.setAdcCal2(false) ;
    tkDcuConversionFactorsStatic.setAdcGain3(0) ;
    tkDcuConversionFactorsStatic.setAdcCal3(false) ;
    

    for(int i=0;i<10;i++){
      TkDcuConversionFactors *tkDcuConversionFactors = new TkDcuConversionFactors ( tkDcuConversionFactorsStatic ) ;
      tkDcuConversionFactors->setDetId(i*1000+1) ;
      tkDcuConversionFactors->setDcuHardId(i*2154) ;
      vConversionFactors.push_back(tkDcuConversionFactors) ;
    }
    
    //tkDcuConversionFactory.setTkDcuConversionFactors ( vConversionFactors ) ;

    //tkDcuConversionFactory.addConversionPartition("TOB");
    //tkDcuConversionFactory.getTkDcuConversionFactors(123,true);
    //tkDcuConversionFactory.setUsingFile();
    //tkDcuConversionFactory.setOutputFileName("testConversionOutput.xml") ;
    tkDcuConversionFactory.setTkDcuConversionFactors(vConversionFactors);

    // Delete the conversion factors
    for (dcuConversionVector::iterator iti = vConversionFactors.begin() ; iti != vConversionFactors.end() ; iti ++) {
      //(*iti)->display();
      delete *iti ;
      }
    */
    //std::cout<<"Uploading the data..."<<std::endl;
    //deviceFactory->setUsingDb(true);
    //deviceFactory->setTkDcuConversionFactors();
    
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
