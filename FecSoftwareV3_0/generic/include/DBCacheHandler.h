#ifndef _DBCacheHandler_h_
#define _DBCacheHandler_h_



#include "DbInterface.h"
#include "TShare.h"

//DIAGREQUESTED


/**
  \class DBCacheHandler
  \author  L.Mirabito 
  \date August 2004
  \version 1.0
  \date January 2005
  \version 1.1

   \brief Main control class of the tracker local data acquisition

   <h2> Description</h2>
   The <il>DBCacheHandler </il> organizes the local data acquisition loop interacting with:
   <ul>
   <li> The Trigger Controller (LTC,TSC or TCW)
   <li> The Fast Control (FEC) handle by the FECSupervisor's
   <li> The FED's (Fed9USupervisor, FedPmcSupervisor,FedEmulator)
   <li> The Fed Builders (dataSender,G3DataSender)
   <li> The Event Manager (EVM)
   </ul>

   Various acquisition loops are implemented in a thread (Enable state) according to the following scheme:
   <ol>
   <li> Trigger is stopped
   <li> Fed's are configured in the needed mode (SCOPE, VIRGIN RAW or PROCESSED)
   <li> Loop on settings, if any, starts
   <li> Fast Control and trigger settings are changed if needed
   <li> Trigger is re enable for N events
   <li> Fed Builder's are polled for N collected events
   <li> N triggers are sent to the EVM
   <li> Go back to step 4 until the end of the setting loop or a stop run request (Halt state)
   </ol>

   The acquisition types implemented are:
   <ul>
   <li> 1     = Normal physic run. Default FED mode is VIRGIN RAW. No interaction with  the control
   <li> 2     = Pedestal. Same as the physic run but the local trigger controller is set to internal trigger
   <li> 3/33  = Calibration in peak/ Deconvolution mode.
   <li> 4     = AOH gain and bias scan.
   <li> 5     = PLL tick scan
   <li> 12    = FED tick scan
   <li> 6     = Local trigger controller coarse delay ( 25 ns) scan
   <li> 7     = Fine delay scan to adjust trigger sampling with the PLL and Local Trigger Controller.
   <li> 8     = Same as 7 but using PLL and FED9U TTC or channel delay adjustement (recommended)
   <li> 11    = FED connection test 
   <li> 13    = FED bare connection test
   <li> 14    = VPSP scan run (base line adjustement)
   </ul>


  

  
*/

class DBCacheHandler 
{
	
 public:
  /** Constructor */
  DBCacheHandler(std::string partition,
		 bool downloadfed=true, 
		 bool downloadfec=true, 
		 bool downloadcon = true,
		 std::string fedname ="FEDSM00",
		 std::string fecname ="FECSM00",
		 std::string conname ="CONSM00",
		 unsigned int fedsize=0xea00000,
		 unsigned int fecsize=0x800000,
		 unsigned int consize=0x200000,
		 bool commonmem=true
		 ) ;

  /** Destructor */
  ~DBCacheHandler() ;

  /**
     Initialise state method
   */

  void InitialiseShareMemory(); 

  /**
     Configure state method
   */

  void FillShareMemory(bool disableApvError=false);



  inline char* getFedStart() {
    if (dbfedstart_ == 0)
      {
	dbfedstart_ = (char*) dbfedmem_->Attach();
      }
    return dbfedstart_;}
  inline char* getFecStart() {
    if (dbfecstart_ == 0)
      {
	dbfecstart_ = (char*) dbfecmem_->Attach();
      }

    return dbfecstart_;}
  inline char* getConStart() {
    if (dbconstart_ == 0)
      {
	dbconstart_ = (char*) dbconmem_->Attach();
      }

    return dbconstart_;}

  void Detach();
  void Attach();

  bool isO2OOk() { return  o2oStatus_;}
  bool isFecChanged() { return fecChanged_;}
  bool isFedChanged() { return fedChanged_;}
 private:
 
  TShare *dbfedmem_,*dbfecmem_,*dbconmem_;
  char *dbfedstart_,*dbfecstart_,*dbconstart_,*curall_;

  std::map<std::string, DbInterface *> mapDbInterface_ ;
  std::string FEDShareMemoryName_,FECShareMemoryName_,CONShareMemoryName_,partitionName_;
  unsigned int FEDShareMemorySize_,FECShareMemorySize_,CONShareMemorySize_;
  bool downloadFED_,downloadFEC_,downloadCON_,commonMemory_;
  bool fecChanged_,fedChanged_;
 bool o2oStatus_;

};
#endif
