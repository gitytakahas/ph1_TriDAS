
#include <unistd.h>
#include "DBCacheHandler.h"
#include "DbInterfaceDef.h"
#include <unistd.h>
#include "DbClient.h"

using namespace std;


  DBCacheHandler::DBCacheHandler(
				 std::string partition,
				 bool downloadfed, 
				 bool downloadfec, 
				 bool downloadcon,
				 std::string fedname,
				 std::string fecname,
				 std::string conname ,
				 unsigned int fedsize,
				 unsigned int fecsize,
				 unsigned int consize,
				 bool commonmem)

{
  //

  partitionName_ =partition;
  downloadFED_ = downloadfed;
  FEDShareMemoryName_ = fedname;
  FEDShareMemorySize_ = fedsize; // FED +FEC +Connection size
  downloadFEC_ = downloadfec;
  FECShareMemoryName_ = fecname;
  FECShareMemorySize_= fecsize;

  downloadCON_ = downloadcon;
  CONShareMemoryName_ = conname;
  CONShareMemorySize_ = consize;


  dbfecstart_=NULL;
  dbfedstart_=NULL;
  dbconstart_=NULL;


  dbfecmem_=NULL;
  dbfedmem_=NULL;
  dbconmem_=NULL;

  curall_=NULL;
  o2oStatus_=true;


  commonMemory_ = commonmem;

}

/** Delete all map with the dbinterface
 */
DBCacheHandler::~DBCacheHandler() {

  for (std::map<std::string, DbInterface *>::iterator it = mapDbInterface_.begin() ; it != mapDbInterface_.end() ; it ++) {
    delete it->second;
  }

  mapDbInterface_.clear() ;
}

/**
 */
void DBCacheHandler::InitialiseShareMemory()

{
  std::cout <<"Download FED ? "<< downloadFED_ <<endl;
    // Creation of the 3 Share Memories
    if (downloadFED_)
      {
	std::cout<<"Creating FED Share Memory with size"<<hex << FEDShareMemorySize_ << dec <<std::endl; 
	try 
	  {
	    dbfedmem_ =  new TShare((char*) FEDShareMemoryName_.c_str(),TShare_CREATE,FEDShareMemorySize_);
	    //dbfedstart_ = (char*) dbfedmem_->Attach();
	  }
	catch (std::string e)
	  {
	    cout << "Can't create the FED ShareMemory " << e <<endl;
	    return;
	  }
      }


    if (downloadFEC_)
      {
	try 
	  {
	    std::cout<<"Creating FEC Share Memory with size"<<hex << FECShareMemorySize_ << dec <<std::endl;
	    dbfecmem_ =  new TShare((char*) FECShareMemoryName_.c_str(),TShare_CREATE,FECShareMemorySize_);
	    //dbfecstart_ = (char*) dbfecmem_->Attach();
	  }
	catch (std::string e)
	  {
	    cout << "Can't create the FEC ShareMemory " << e <<endl;
	    return;
	  }
      }


    if (downloadCON_)
      {
	std::cout<<"Creating CON Share Memory with size"<<hex << CONShareMemorySize_ << dec <<std::endl;
	try 
	  {
	    dbconmem_ =  new TShare((char*) CONShareMemoryName_.c_str(),TShare_CREATE,CONShareMemorySize_);
	    //dbconstart_ = (char*) dbconmem_->Attach();
	  }
	catch (std::string e)
	  {
	    cout << "Can't create the CON ShareMemory " << e <<endl;
	    return;
	  }
      }


  
 

  // Partition Name
    if (partitionName_ == "NONE")
      {
	char* cpart=getenv("ENV_CMS_TK_PARTITION");
	if (cpart != NULL) 
	  {
	    string spart(cpart);
	    partitionName_ = spart;
	  }
      }
}

void DBCacheHandler::Attach()
{
  // FED access
  if (dbfedmem_!=NULL && dbfedstart_ == NULL)
    {
      try 
	{
	  dbfedstart_ = (char*) dbfedmem_->Attach();
	  std::cout<<"FED memory attahed at "<<dbfedstart_<<std::endl;
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::Attach: cannot access FED share memory "<<s<<std::endl;
	  dbfedstart_ = NULL;
	}      
    }

  // FEC access
  if (dbfecmem_!=NULL && dbfecstart_ == NULL)
    {
      try 
	{
	  dbfecstart_ = (char*) dbfecmem_->Attach();
	  std::cout<<"FEC memory attahed at "<<dbfecstart_<<std::endl;
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::Attach: cannot access FEC share memory "<<s<<std::endl;
	  dbfecstart_ = NULL;
	}      
    }


  // COnnection access
  if (dbconmem_!=NULL && dbconstart_ == NULL)
    {
      try
	{
	  dbconstart_ = (char*) dbconmem_->Attach();
	  std::cout<<"Connection memory attahed at "<<dbconstart_<<std::endl;
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::Attach: cannot access COnnection share memory "<<s<< std::endl;
	  dbconstart_ = NULL;
	}

    }

}


/**
 */
void DBCacheHandler::FillShareMemory(bool disableApvError)
{

  vector<std::string> vPart;
  DbClient::splitInVector(partitionName_,vPart);

  // Creating all db interface (one per partition)
  for (std::vector<std::string>::iterator it = vPart.begin() ; it != vPart.end() ; it ++) {
    if (mapDbInterface_.find(*it) == mapDbInterface_.end()) {
      std::cout << "Create the DB interface for partition " << *it << std::endl ;
      mapDbInterface_[*it] = new DbInterface() ;
      o2oStatus_= o2oStatus_ && (mapDbInterface_[*it]->getO2OXchecked(*it) == 1);
    }
  }

#ifdef OLDWAY
    dbfecstart_=0;dbfedstart_=0;dbconstart_=0;
#endif
  curall_=0; //Reinitialise the default pointer
  // Load FED part
#ifdef OLDWAY
  if (downloadFED_)
    {
      try 
	{
	  dbfedstart_ = (char*) dbfedmem_->Attach();
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::FillShareMemory: cannot access FED share memory "<<s<<std::endl;
	  dbfedstart_ = 0;
	}
    }
#endif
  if (downloadFED_ && dbfedstart_ !=NULL)
    {
      char* curfed=(char*)dbfedstart_;
      fedChanged_ =false;

      // Loop on Partitions
      for (unsigned int ipart=0;ipart<vPart.size();ipart++)
	{
	  bool newfed=true;
	  std::vector<Fed9U::Fed9UDescription*> vfed = mapDbInterface_[vPart[ipart]]->downloadFEDFromDatabase (vPart[ipart],newfed);
	  fedChanged_ = (fedChanged_ || newfed);
	  for (unsigned int ifed=0;ifed<vfed.size();ifed++)
	    {
	      Fed9U::Fed9UDescription* t =vfed[ifed];
	      if (t!=NULL)
		{
		  
		  cout << sizeof(Fed9U::Fed9UDescription) << "Fed found!!!! " <<ifed <<endl;
		  unsigned int* ibuf=(unsigned int*) curfed;
		  
		  ibuf[0] = 0XDB<< 24 | DBC_FED9U_TYPE << 20;
                  ibuf[0] |= sizeof(Fed9U::Fed9UDescription);
		  cout << hex<< ibuf << curfed <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
		  memcpy(&ibuf[1],t,sizeof(Fed9U::Fed9UDescription));
		  curfed+=sizeof(Fed9U::Fed9UDescription)+sizeof(int);
		}
	    }
	}
      if (commonMemory_) curall_ = curfed;
    }


  // Load the FEC part
#ifdef OLDWAY
  if (downloadFEC_)
    {
      try
	{
	  dbfecstart_ = (char*) dbfecmem_->Attach();
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::FillShareMemory: cannot access FEC share memory "<<s<<std::endl;
	  dbfecstart_ = 0;
	}


    }
#endif
  if (downloadFEC_ && (dbfecstart_ !=NULL || (commonMemory_ && curall_!=NULL)) )
    {
      char* current=(char*)dbfecstart_;
      fecChanged_ = false;
      if (commonMemory_ && curall_!=NULL) current= curall_;
      for (unsigned int ipart=0;ipart<vPart.size();ipart++)
	{


	  cout << "DB cache "  << ": 6. Downloading Fec devices " <<endl;
	  bool newfec=true;
	  mapDbInterface_[vPart[ipart]]->downloadFECFromDatabase(vPart[ipart],newfec);
	  fecChanged_ = (fecChanged_||newfec);
	  cout << "DB cache "  << ": 7. Download done with DBI " <<endl;
	  deviceVector vDev = mapDbInterface_[vPart[ipart]]->getCurrentDevices();
	  cout << "DB cache "  << ": 8. accessing FEC device vector for " << vDev.size() << " devices" <<endl;
	  
	  
	  int napv=0,naoh=0,ndoh=0,nmux=0,npll=0,ndcu=0;
	  for (unsigned int i=0;i<vDev.size();i++)
	    {
	      int objsize;
	      int* ibuf=(int*) current;
	      deviceDescription* dd= vDev[i];
	      //std::cout<< i << " " <<dd->getDeviceType() <<std::endl;
	      switch (dd->getDeviceType())
		{
		case APV25:
		  napv++;
		  objsize=sizeof(apvDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_APV_TYPE << 20 | objsize;
		  if (disableApvError)
		    {
		      apvDescription* apv = (apvDescription*) dd;
		      if (apv->getApvError()!=0)
			{
			  // Switch off Analog output
			  unsigned int mode = apv->getApvMode();
			  mode = mode &0xFE;
			  apv->setApvMode(mode);
			}
		    }


		  break;
		case LASERDRIVER:
		  naoh++;
		  objsize=sizeof(laserdriverDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_AOH_TYPE << 20 | objsize;

		  // Switch off the laser of Disabled APV
		  if (disableApvError)
		    {
		      unsigned int bias0off=0;
		      unsigned int bias1off=0;
		      unsigned int bias2off=0;
		      for (unsigned int j=0;j<vDev.size();j++)
			{
			  deviceDescription* da= vDev[j];
			  if (dd->getFecSlot()!=da->getFecSlot()) continue;
			  if (dd->getRingSlot()!=da->getRingSlot()) continue;
			  if (dd->getCcuAddress()!=da->getCcuAddress()) continue;
			  if (dd->getChannel()!=da->getChannel()) continue;

			  if (da->getDeviceType()!=APV25) continue;
			  apvDescription* apv = (apvDescription*) da;
			  if (apv->getApvError()==0) continue;
			  if (da->getAddress()== 32) bias0off++;  
			  if (da->getAddress()== 33) bias0off++;  
			  if (da->getAddress()== 34) bias1off++;  
			  if (da->getAddress()== 35) bias1off++; 
			  if (da->getAddress()== 36) bias2off++;  
			  if (da->getAddress()== 37) bias2off++;  
			}
		      laserdriverDescription* las = (laserdriverDescription*) dd;
		      if (bias0off == 2) las->setBias(0,0);
		      if (bias1off == 2) las->setBias(1,0);
		      if (bias2off == 2) las->setBias(2,0);

		    }

		  break;
		case DOH:
		  // LM patch Drop DOH
		  ndoh++;
		  //std::cout << "DOH found" <<std::endl;
		  objsize=sizeof(laserdriverDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_DOH_TYPE << 20 | objsize;
		  break;
		case APVMUX:
		  nmux++;
		  objsize=sizeof(muxDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_MUX_TYPE << 20 | objsize;
		  break;
		case PLL:
		  npll++;
		  objsize=sizeof(pllDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_PLL_TYPE << 20 | objsize;
		  break;
		case DCU:
		  ndcu++;

		  objsize=sizeof(dcuDescription);
		  //std::cout << "DCU found " << DCU <<" " <<objsize << std::endl;
		  ibuf[0] = 0xDB << 24 |  DBC_DCU_TYPE << 20 | objsize;
		  break;
		default:
		  objsize=0;
		  break;
		}

   //   cout << hex<< ibuf << current <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
	      if (objsize == 0) continue;
	      memcpy(&ibuf[1],dd,objsize);
	      current+=sizeof(int)+objsize;

	    }

	  std::cout<< "Found APV " << napv <<std::endl;
	  std::cout<< "Found MUX " << nmux <<std::endl;
	  std::cout<< "Found AOH " << naoh <<std::endl;
	  std::cout<< "Found PLL " << npll <<std::endl;
	  std::cout<< "Found DCU " << ndcu <<std::endl;
	  std::cout<< "Found DOH " << ndoh <<std::endl;


	  cout <<"Accessing PIA vector " <<endl;
	  piaResetVector vPia = mapDbInterface_[vPart[ipart]]->getCurrentPia();
	  cout << " accessing PIA reset vector for " << vPia.size() << " devices" <<endl;
	  for (unsigned int i=0;i<vPia.size();i++)
	    {
	      int objsize;
	      int* ibuf=(int*) current;
	      piaResetDescription* dd= vPia[i];
	      objsize=sizeof(piaResetDescription);	
	      ibuf[0] = 0xDB << 24 |  DBC_PIA_TYPE << 20 | objsize;
	      //    cout << hex<< ibuf << current <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
	      memcpy(&ibuf[1],dd,objsize);
	      current+=sizeof(int)+objsize;
	  
	    }

	  std::cout << "------------------- redundancy download" << std::endl ;

	  int error = mapDbInterface_[vPart[ipart]]->downloadFecRedundancyFromDatabase (vPart[ipart]) ;

	  if (!error) {
	    tkringVector ringDescription = mapDbInterface_[vPart[ipart]]->getFecRedundancy() ;
	    std::cout << "Found " << std::dec << ringDescription.size() << " rings in "  << std::endl ;
	    for (tkringVector::iterator it = ringDescription.begin() ; it != ringDescription.end() ; it ++) 
	      {
		std::cout << "Ring on FEC " << (*it)->getFecHardwareId() << " on ring " << (*it)->getRingSlot() << " with " << (*it)->getCcuVector()->size() << " CCUs" << std::endl ;
	      }

	    void *memory = NULL ;
	    try {
	      // serialise the buffer in memory and extract it
	      unsigned int sizeBuffer ;
	      memory = FecFactory::writeTo ( ringDescription, sizeBuffer ) ;
	      std::cout << "Serialise " << ringDescription.size() << " ring descriptions, buffer size = " << sizeBuffer << std::endl ;
	      int objsize=sizeBuffer;
	      int* ibuf=(int*) current;
	      ibuf[0] = 0xDB << 24 |  DBC_CCU_TYPE << 20 | objsize;
	      //    cout << hex<< ibuf << current <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
	      memcpy(&ibuf[1],memory,objsize);
	      current+=sizeof(int)+objsize;

	      //char *coucou = (char *)memory ;
	      //std::cout << coucou << std::endl ;
	      free(memory) ;
	    }
	    catch (FecExceptionHandler &e) {
	      std::cout << "Error during the serialisation or deserialisation: " << e.what() << std::endl ;
	      if (memory != NULL) free(memory) ;
	    }
	  }
	  

	   std::cout << "------------------- DET ID download" << std::endl ;

	   bool changed=true;
	   error = mapDbInterface_[vPart[ipart]]->downloadDetIdFromDatabase (vPart[ipart], changed) ;

	   if (!error) {
	     Sgi::hash_map<unsigned long, TkDcuInfo *> listTkDcuInfo = mapDbInterface_[vPart[ipart]]->getDetIdList() ;
	     std::cout << "Found " << std::dec << listTkDcuInfo.size() << " det ids  " << std::endl ;
	     for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator it = listTkDcuInfo.begin();it != listTkDcuInfo.end();it++)
	       {
		 TkDcuInfo *dd = it->second ;
		 int* ibuf=(int*) current;
		 int objsize=sizeof(TkDcuInfo);	
		 ibuf[0] = 0xDB << 24 |  DBC_DETID_TYPE << 20 | objsize;
		 //    cout << hex<< ibuf << current <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
		 memcpy(&ibuf[1],dd,objsize);
		 current+=sizeof(int)+objsize;
		 
	       }

	   }
	   else {
	     std::cout << mapDbInterface_[vPart[ipart]]->getErrorMessage() << std::endl ;
	   }


	  






	}
      if (commonMemory_) curall_ = current;

    }
#ifdef OLDWAY
  if (downloadCON_)
    {
      try
	{
	  dbconstart_ = (char*) dbconmem_->Attach();
	}
      catch(std::string s)
	{
	  std::cout<<"DBCacheHandler::FillShareMemory: cannot access COnnection share memory "<<s<< std::endl;
	  dbconstart_ = 0;
	}

    }
#endif

  if (downloadCON_ && (dbconstart_ !=NULL || (commonMemory_ && curall_!=NULL)) )
    {
      char* current=(char*)dbconstart_;

      if (commonMemory_ && curall_!=NULL) current= curall_;
      for (unsigned int ipart=0;ipart<vPart.size();ipart++)
	{
	  
	  
	  bool newconnection=true;
	  if (mapDbInterface_[vPart[ipart]]->downloadConnectionsFromDatabase (vPart[ipart],newconnection) ==0) 
	    {
	      ConnectionVector v = mapDbInterface_[vPart[ipart]]->getConnections();
	      for (unsigned int i=0;i<v.size();i++)
		{
		  int* ibuf=(int*) current;
		  int objsize= sizeof(ConnectionDescription);
		  ibuf[0] = 0xDB << 24 |  DBC_CONNECTION_TYPE << 20 | objsize;
		  //       cout << hex<< ibuf << current <<":"<<ibuf[0]<<dec <<" is the shm address " <<endl;
		  memcpy(&ibuf[1],v[i],objsize);
		  current+=sizeof(int)+objsize;
		  
		}
	    }
	}
    }

#ifdef OLDWAY
  this->Detach();
#endif

}


void DBCacheHandler::Detach()
{
  std::cout<<"DBCacheHandler::Detach() 1) FEC "<< (long) dbfecstart_<<"  2) FED "<< (long) dbfedstart_<<" 3) Connection "<<(long) dbconstart_<<std::endl;
  if (dbfedstart_!=NULL) {dbfedmem_->Detach(dbfedstart_);dbfedstart_=NULL;}
  if (dbfecstart_!=NULL) {dbfecmem_->Detach(dbfecstart_);dbfecstart_=NULL;}
  if (dbconstart_!=NULL) {dbconmem_->Detach(dbconstart_);dbconstart_=NULL;}
 
}
