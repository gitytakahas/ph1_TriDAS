#ifndef _DbClient_h
#define _DbClient_h

#include "DeviceFactory.h"
#include "DbInterfaceDef.h"

#include <string>
#include "TShare.h"


class DbClient 
{
 public:
/**
  -Create a DbClient with the Share Memory name
*/
  DbClient(std::string name) throw (std::string);
    /**
      Detach the Sahre memory
     */   
  ~DbClient();
  /**
     purge Devices
   */
  void purge();
  /**
     Parse the Sahre memory and create and fill local vectors
  */
  void parse() throw (std::string);
    /**
    Return a ptr to the local vector of FEC devices
    */
  deviceVector* getDevices();
  /** Fill the vector */
  void getDevices(std::string fecid,deviceVector* t);
  void getDevices(std::string fecid,int ringslot, deviceVector* t);
  piaResetVector* getPiaReset();

void getPiaReset(std::string fecid,piaResetVector* t);
 std::vector<Fed9U::Fed9UDescription*>* getFed9UDescriptions();


 Fed9U::Fed9UDescription* getFed9UDescription(unsigned int id);
 ConnectionVector* getConnections();


 Sgi::hash_map<unsigned long, TkDcuInfo *> *getInfos(); 
 TkRingDescription* getTkRingDescription(std::string fecid,unsigned int ring);
 static void splitInVector(std::string ENV,vector<string> &v);



 private:
  
  TShare* dbmem_;
  char* start_;
  deviceVector* vDevices_;
  piaResetVector* vPiaReset_;
  std::vector<Fed9U::Fed9UDescription*>* vFed9Us_;
  ConnectionVector* vConn_;
  Sgi::hash_map<unsigned long, TkDcuInfo *> *mDetid_ ;
  tkringVector vTkRing_;
};
#endif
