
#include "DbClient.h"

/**
  -Create a DbClient with the Share Memory name
*/
DbClient::DbClient(std::string name) throw (std::string)
    {
      dbmem_ = new TShare((char*)name.c_str(),TShare_OPEN,0x200000);
      start_ = (char*) dbmem_->Attach();
      vDevices_=NULL;
      vFed9Us_=NULL;
      vPiaReset_=NULL;
      vConn_=NULL;
      mDetid_ =NULL;
    }
    /**
      Detach the Sahre memory
     */   
  DbClient::~DbClient()
    {

      purge() ;
      dbmem_->Detach(start_);
      delete dbmem_;
    }

  /**
     purge Devices
   */
void DbClient::purge()
    {
      std::cout<< "vDvices " << hex <<vDevices_ <<dec << std::endl;

      // FEC devices
      if (vDevices_!=NULL)
	{
	  FecFactory::deleteVectorI(*vDevices_) ;
	  delete vDevices_;
	}

      vDevices_ = NULL;
      std::cout<< "vPia " << hex <<vPiaReset_ <<dec << std::endl;
      // Pia Resets
      if (vPiaReset_!=NULL)
	{
	  FecFactory::deleteVectorI(*vPiaReset_) ;
	  delete vPiaReset_;
	}
      vPiaReset_ = NULL;
      // FED9US
      std::cout<< "vFed " << hex <<vFed9Us_ <<dec << std::endl;
      if (vFed9Us_!=NULL)
	{
	  for (unsigned int i=0;i<vFed9Us_->size();i++) delete (*vFed9Us_)[i];
	  delete vFed9Us_;
	}
      vFed9Us_ = NULL;

      std::cout<< "vConn " << hex <<vConn_ <<dec << std::endl;
      // Connections
      if (vConn_!=NULL)
	{
	  std::cout << "Purging "<<vConn_->size()<<" connection " <<std::endl;
	  ConnectionFactory::deleteVectorI(*vConn_);


	  //	  for (unsigned int i=0;i<vConn_->size();i++) delete (*vConn_)[i];
	  std::cout << "Done Purging "<<vConn_->size()<<" connection " <<std::endl;
	  delete vConn_;
	  std::cout << "Vector destroyed " <<std::endl;
	}
      vConn_ = NULL;

      std::cout<< "End of the delete " << std::endl;


      // Detid
      if (mDetid_ !=NULL)
	{
	  for (Sgi::hash_map<unsigned long, TkDcuInfo *>::iterator it=mDetid_->begin();it!=mDetid_->end();it++)
	    {
	      delete it->second;
	    }
	  delete mDetid_;
	}


      FecFactory::deleteVectorI(vTkRing_) ;

      //      if(parser_!=NULL) parser_->clear();
      // parser_=NULL;
    }
  /**
     Parse the Sahre memory and create and fill local vectors
  */
  void DbClient::parse() throw (std::string)
    {
      purge();
    // FEC devices
    vDevices_ = new deviceVector();
   // Pia Resets
   vPiaReset_ = new piaResetVector();
   // FED9US
   vFed9Us_ = new std::vector<Fed9U::Fed9UDescription*>;
   
   // Connections
   vConn_=new ConnectionVector();
   // Detid
   mDetid_ = new Sgi::hash_map<unsigned long, TkDcuInfo *>;

   //   parser_ =  new TrackerParser();

   // Parsing
   char* current= (char*)start_;
   int* ibuf = (int*) current;
   std::cout << " IBUF " << hex << ibuf[0] <<std::dec <<std::endl;
      while (((ibuf[0]>>24)&0xFF) == 0xdb )
	{
	  int objsize= ibuf[0] & 0XFFFFF;
	  //	  std::cout << "Device " << hex << ibuf[0] <<" size" << objsize << std::endl;
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_FED9U_TYPE )
	    {
	      Fed9U::Fed9UDescription* t=NULL;
	      try
		{
		  t = new Fed9U::Fed9UDescription((*(Fed9U::Fed9UDescription*) &ibuf[1]));
		  //t = (Fed9U::Fed9UDescription*) &ibuf[1];
		}
	      catch (...)
		{
		  std::cout << "Cannot create Fed9U::Fed9UDescription " << std::endl;
		}
	      std::cout << "FED :" << t->getFedId() <<std::endl;	     
	      vFed9Us_->push_back(t);
	      //	      delete t;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_CONNECTION_TYPE )
	    {
	      ConnectionDescription* c=new ConnectionDescription(*(ConnectionDescription*) &ibuf[1]);
	      vConn_->push_back(c);
	      //	      c->toXML(std::cout);
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_APV_TYPE )
	    {
	      apvDescription* apv = new apvDescription(*((apvDescription*)&ibuf[1]));
	      vDevices_->push_back(apv);
	      //  std::cout << "APV25 found " <<objsize<<std::endl;
	    }
	if ( (  (ibuf[0]>>20)&0xF) ==  DBC_PIA_TYPE )
	    {
	      piaResetDescription* pia = new piaResetDescription(*((piaResetDescription*)&ibuf[1]));
	      vPiaReset_->push_back(pia);
	      //	      std::cout << "PIARESET found " <<objsize<<std::endl;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_MUX_TYPE )
	    {
	      muxDescription* mux = new muxDescription(*((muxDescription*)&ibuf[1]));
	      vDevices_->push_back(mux);
	      //	      std::cout << "MUX found " <<objsize<<std::endl;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_PLL_TYPE )
	    {
	      pllDescription* pll = new pllDescription(*((pllDescription*)&ibuf[1]));
	      vDevices_->push_back(pll);

	      //	      std::cout << "PLL found " <<objsize<<std::endl;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_AOH_TYPE )
	    {
	      laserdriverDescription* l = new laserdriverDescription(*((laserdriverDescription*) &ibuf[1]));

								     //*(laserdriverDescription*) &ibuf[1]));
	      vDevices_->push_back(l);

	      //std::cout << "AOH found " <<l->getFecHardwareId()<<std::endl;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_DOH_TYPE )
	    {
	      laserdriverDescription* l = new laserdriverDescription(*((laserdriverDescription*) &ibuf[1]));

								     //*(laserdriverDescription*) &ibuf[1]));
	      vDevices_->push_back(l);

	      //	      std::cout << "DOH found " <<objsize<<std::endl;
	    }

	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_DCU_TYPE )
	    {
	      dcuDescription* dcu = new dcuDescription(*((dcuDescription*)&ibuf[1]));
	      vDevices_->push_back(dcu);

	      //	      std::cout << "PLL found " <<objsize<<std::endl;
	    }

	  
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_DETID_TYPE )
	    {
	     TkDcuInfo* t = new TkDcuInfo(*((TkDcuInfo*) &ibuf[1]));

#ifdef DEBUGMSGERROR
	     std::cout<< " Inserting new dcu Hard Id " << t->getDcuHardId() <<std::endl;
#endif
	     (*mDetid_)[(unsigned long) t->getDcuHardId()] = t;

	      //	      std::cout << "DOH found " <<objsize<<std::endl;
	    }
	  if ( (  (ibuf[0]>>20)&0xF) ==  DBC_CCU_TYPE )
	    {
	      //tkringVector v = FecFactory::readFrom ( memory ) ;

	      vTkRing_ = FecFactory::readFrom ( &ibuf[1] ) ;

	      std::cout << "Found " << vTkRing_.size() << " ring descriptions after deserialisation buffer size" << std::endl ;
	      for (tkringVector::iterator it = vTkRing_.begin() ; it != vTkRing_.end() ; it ++) {
		std::cout << "Ring on FEC " << (*it)->getFecHardwareId() << " on ring " << (*it)->getRingSlot() << " with " << (*it)->getCcuVector()->size() << " CCUs" << std::endl ;
	      }
	      
	    }



	  current+=objsize+sizeof(int);
	  ibuf = (int*) current;
	}


      if (vDevices_->size() <=0 && vPiaReset_->size()<=0 && vFed9Us_->size() <=0 && vConn_->size()<=0)       throw std::string("DbClient: Empty Share memory ");


      return;
    }

    /**
    Return a ptr to the local vector of FEC devices
    */
deviceVector* DbClient::getDevices()
    {
#ifdef DEBUG
      std::cout<<" Vdevices size " << vDevices_->size() <<endl;
      for (deviceVector::iterator device = vDevices_->begin() ; device != vDevices_->end() ;
	   device ++) 
	{
	  
	  deviceDescription *deviced = *device ;
	  std::cout <<"in getDevices() " <<deviced->getFecHardwareId()<<std::endl;
	}
#endif
      return vDevices_;
    }


  /** Fill the vector */
void DbClient::getDevices(std::string fecid,deviceVector* t)
	{

 	for (deviceVector::iterator device = vDevices_->begin() ; device != vDevices_->end() ;
 	device ++) 
	{

       		deviceDescription *deviced = *device ;
		if (deviced->getFecHardwareId() == fecid)
        		t->push_back ( deviced) ;
      	}

    	return;
	}

  /** Fill the vector */
void DbClient::getDevices(std::string fecid,int ringslot, deviceVector* t)
	{

 	for (deviceVector::iterator device = vDevices_->begin() ; device != vDevices_->end() ;
 	device ++) 
	{

       		deviceDescription *deviced = *device ;
		if (deviced->getFecHardwareId() == fecid && deviced->getRingSlot()==ringslot)
        		t->push_back ( deviced) ;
      	}

    	return;
	}

piaResetVector* DbClient::getPiaReset(){return vPiaReset_;}

void DbClient::getPiaReset(std::string fecid,piaResetVector* t){
	 for (piaResetVector::iterator device = vPiaReset_->begin() ; device != vPiaReset_->end() ;
 device ++) {
 	piaResetDescription *deviced = *device ;
	if (deviced->getFecHardwareId() == fecid)
        	t->push_back ( deviced ) ;
      }
    return;}

std::vector<Fed9U::Fed9UDescription*>* DbClient::getFed9UDescriptions() { return vFed9Us_;} 

Fed9U::Fed9UDescription* DbClient::getFed9UDescription(unsigned int id)
{
      cerr << "loooking for id: " << id << endl;
      cerr << "vFed9Us_->size(): " << vFed9Us_->size() << endl;
      for (unsigned int i=0;i<vFed9Us_->size();i++)
	{
	  cerr << "\tid: " << (*vFed9Us_)[i]->getFedId() << endl;
	  if ((*vFed9Us_)[i]->getFedId() == id)  return (*vFed9Us_)[i];
	}
      return NULL;}

ConnectionVector* DbClient::getConnections(){return vConn_;}

Sgi::hash_map<unsigned long, TkDcuInfo *>* DbClient::getInfos ( ) {

    return mDetid_ ;
  }



  TkRingDescription* DbClient::getTkRingDescription(std::string fecid,unsigned int ring)
	{

 	for (tkringVector::iterator device = vTkRing_.begin() ; device != vTkRing_.end() ;
 	device ++) 
	{

       		TkRingDescription *deviced = *device ;
		if (deviced->getFecHardwareId() == fecid && deviced->getRingSlot() == ring)
        	       return deviced ;
      	}

    	return 0;
	}





void DbClient::splitInVector(std::string ENV,vector<string> &v)
    {
      int idx1=0,idx2;
      size_t pos;
      do 
	{
	  pos = ENV.find(":",idx1);
	  if (pos!=string::npos)
	    {
	      idx2= pos-1;
	    }
	  else
	    idx2= ENV.length();
	  //cout << "adding " << ENV.substr(idx1,idx2-idx1+1) <<endl;
	  v.push_back(ENV.substr(idx1,idx2-idx1+1));
	  idx1 =pos+1;
	} while (pos!=string::npos);
      
    }



