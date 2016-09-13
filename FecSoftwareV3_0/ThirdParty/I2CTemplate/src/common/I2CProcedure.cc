/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, James Lamb - CERN, Geneva, Switzerland
*/
#include <iostream>

#include "FecVmeRingDevice.h"

#include "i2cAccess.h"
#include "apvAccess.h"
#include "muxAccess.h"
#include "pllAccess.h"
#include "laserdriverAccess.h"
#include "DohAccess.h"
#include "dcuAccess.h"
#include "FecAccess.h"
#include "keyType.h"
#include "PiaResetAccess.h"
#include "piaResetDescription.h"
  //#include "APIAccess.h"
			   

//sub-methods to run the tests for specific types of i2c errors - just for readability
bool testErrorType1(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans);
bool testErrorType2(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans);
bool testErrorType3(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int *nApvs, int nCcuChans);
bool testErrorType4(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int *nApvs, int nCcuChans);
bool testErrorType5(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans);
bool testErrorType6(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans);
void helpMe(); //print help

//copied from APIAccess.cc:
void allCCUsPiaReset ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress );

/* ----------------------------------------------------------------------- */
/*                                                                         */
/*                       main function                                     */
/*                                                                         */
/* ----------------------------------------------------------------------- */

/* runs digital tests to detect the known types of i2c errors, according to tests provided by 
   Guido Magazzu. */
/* I left some flexibility to choose i2c channels and number of apvs per module, in case this needs to be 
   made to work for petals at some point */
/* note: assumes the fec is in a good state, so if used in standalone mode should run a fec reset first with ProgramTest.exe */
/* exits with code 0 if all tests passed, negative exit code if error */
/* does pia resets before and after the test suite */
/* james.lamb@cern.ch 11/05 from Frederic Drouhin's example */

//required arguments:
//-pci 
//-ccu <ccuAddress> !address in hex!
//-rtype <rtype> type of rod, either ss4, ss6, or ds
//example: I2CProcedure -pci -ccu 0x16 -rtype ss4

//dependencies:
//FecSoftwareV3_0 and naturellement source xdaq_profile

//revision history:

//testing history:

//9/11/05 tested on Guido's test setup (ss4 rod ccu address 0x16).  No i2c errors expected, none received

//17/11/05 tested on Guido's test setup with icc at position 1 replaced by an icc on which discrete capicitors can be put on 
//the hybrid or laserdriver SCL line.
//putting 600pF on the hybrid SCL shows error type 1 and 3.  Putting 150pF (&220pF &120pF) on hybrid SCL shows error type 3 only.  This makes sense.
//we were not able to introduce capacitances to get the full range of errors _with_the_signatures_specified_, but we did get some indication of an error
//every time a capacitor was introduced on a line.  That is to say, as far as we know, if all tests are passed, then the rod is free of i2c errors


int main ( int argc, char *argv[] ) {

  // Create the hardware access
  //I don't know why fred declares this inside main
  FecAccess *createFecAccess ( int argc, char **argv, int *cnt ) ;

  unsigned int ccuAddress=0;
  //first six are common to SS and DS, second six DS only:
  unsigned int ccuChans[12]={0x11, 0x13, 0x14, 0x17, 0x19, 0x1a, 0x10, 0x16, 0x12, 0x18, 0x15, 0x1b};

  int nCcuChans=0;
  int *nApvs=0;
  int nApvs4[12]={4,4,4,4,4,4,4,4,4,4,4,4};
  int nApvs6[6]={6,6,6,6,6,6};
  char rtype[10];//rod type (ss4, ss6, ds)

  // Hardware access
  FecAccess *fecAccess = NULL ;

  //if the first parameter is -help or --help, show help and quit
  if (argc == 2 && (!(strcmp(argv[1],"-help")) || !(strcmp(argv[1],"--help")))) {
    helpMe();
    exit(0);
  }


  // Check the parameter
  if (argc < 2 || strcmp(argv[1],"-pci")) {

    std::cerr << argv[0] << ": error in usage" << std::endl ;
    exit(-1);
  }

  // Create the FEC Access
  int cnt ;
  try {
    fecAccess = createFecAccess ( argc, argv, &cnt ) ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cout << "------------ Exception ----------" << std::endl ;
    std::cout << e.what()  << std::endl ;
    std::cout << "---------------------------------" << std::endl ;
    exit (-1) ;
  }
  //I believe it is possible (although unlikely) that the createFecAccess could fail
  //without an exception
  if (!fecAccess) {
    std::cerr<<"I2CProcedure: fecAccess not created"<<std::endl;
    exit(-1);
  }


  //take care of command-line arguments:
  for (int i = cnt ; i < argc ; i ++) {
    if (strcasecmp (argv[i],"-ccu") == 0) { // If a new ccuAddress has been set
      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%x",&ccuAddress) || (argv[i+1][0] == '-')) { // Check and init the ccuAddress value 
	std::cout <<"I2CProcedure: incorrect setting of -ccu" << std::endl;
      }
      else {
	i++; //increment because parsed the argument to -ccu
      }
    } else if (strcasecmp (argv[i],"-rtype") == 0) {
      if ((argc <= (i+1)) || !sscanf(argv[i+1],"%s",rtype) || (argv[i+1][0] == '-')) { // Check and init the rtype value
	std::cout <<"I2CProcedure: incorrect setting of -rtype"<<std::endl;
      } else {
	i++; //increment because parsed the argument to -rtype
	//convert to lowercase
	int len=strlen(rtype);
	for (int il=0;il<len;il++) rtype[il]=tolower(rtype[il]);
	if (!strcmp(rtype,"ss4")) {
	  nApvs=nApvs4;
	  nCcuChans=6;
	  //nCcuChans=5;
	  
	} else if (!strcmp(rtype,"ss6")) {
	  nApvs=nApvs6;
	  nCcuChans=6;
	} else if (!strncmp(rtype,"ds",2)) {
	  nApvs=nApvs4;
	  nCcuChans=12;
	} else {
	  std::cerr <<"I2CProcedure: unknown rtype"<<std::endl;
	}
      }
      
    }  
  }
  if (ccuAddress==0 || nCcuChans==0) {
    std::cout <<"I2CProcedure: failed to set arguments"<<std::endl;
    try {
      delete fecAccess ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "------------ Exception ----------" << std::endl ;
      std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
      std::cerr << "---------------------------------" << std::endl ;
    }
    exit(-1);
  }

  //perform resets:
  allCCUsPiaReset (fecAccess, (tscType8) 0, (tscType8) 0) ;
  
  std::cout <<"testing type 1"<<std::endl;
  bool error1=testErrorType1(fecAccess,ccuAddress,ccuChans,nCcuChans);
  
  std::cout <<"testing type 2"<<std::endl;
  bool error2=testErrorType2(fecAccess,ccuAddress,ccuChans,nCcuChans);

  std::cout <<"testing type 3"<<std::endl;

  bool error3=testErrorType3(fecAccess,ccuAddress,ccuChans,nApvs,nCcuChans);

  std::cout <<"testing type 4"<<std::endl;
  bool error4=testErrorType4(fecAccess,ccuAddress,ccuChans,nApvs,nCcuChans);

  std::cout <<"testing type 5"<<std::endl;
  bool error5=testErrorType5(fecAccess,ccuAddress,ccuChans,nCcuChans);
  
  std::cout <<"testing type 6"<<std::endl;
  bool error6=testErrorType6(fecAccess,ccuAddress,ccuChans,nCcuChans);

  bool errorcode=1*error1+2*error2+4*error3+8*error4+16*error5+32*error6;
  errorcode*=-1;

  //debugging:
  std::cout <<"error1: "<<error1<<std::endl;
  std::cout <<"error2: "<<error2<<std::endl;
  std::cout <<"error3: "<<error3<<std::endl;
  std::cout <<"error4: "<<error4<<std::endl;
  std::cout <<"error5: "<<error5<<std::endl;
  std::cout <<"error6: "<<error6<<std::endl;

  if (!errorcode) {
    std::cout <<"I2CProcedure: All Tests Passed"<<std::endl;
  } else {
    std::cout <<"I2CProcedure: Device Failure"<<std::endl;
  }
  
  
  //perform resets:
  allCCUsPiaReset (fecAccess, (tscType8) 0, (tscType8) 0) ;

  
  try {
    delete fecAccess ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "------------ Exception ----------" << std::endl ;
    std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
    std::cerr << "---------------------------------" << std::endl ;
  }
  
  
  exit(errorcode);
}

/* testErrorType1: tests errors type 1 according to Guido document.  Repeatedly reads/writes odd values to DCU TREG and checks
   that the low bit is not dropped. */
/* runs in sequence on all the ccu i2c channels specified for the ccu */

//requires:
//modifies: nothing

//ccuAddress: obvious
//ccuChans: array of ccu i2c channels to run the tests on.
//nCcuChans: length of ccuChans

bool testErrorType1(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans) {

  bool globalErr=0;//did it pass the i2c tests?
  int failureFlag=0;//flag to exit gracefully upon unknown failure
  
  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexDcu=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    char msgDcu[80] ; decodeKey(msgDcu,indexDcu) ; // use for display
    dcuAccess *dcu = NULL ;
    try {
      dcu = new dcuAccess ( fecAccess, indexDcu ) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Cannot create an access to the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    if (dcu==NULL) {
      failureFlag=1;
      goto exit_failure;
    }
    try {
      //do all the register accesses here:
      signed int dcuSetsToWrite[14]={0xff,0xfd,0x83,0x81,0x87,0x85,0x8b,0x89,0x8f,0x8d,0x93,0x91,0x97,0x95};
      //signed int dcuSetsToWrite[14]={0xfd,0xfd,0x83,0x81,0x87,0x85,0x8b,0x89,0x8f,0x8d,0x93,0x91,0x97,0x95};
      

      for (int is=0;is<14;is++) {
	//resets done with cheap hack for now:
// 	char cmd[200];
// 	sprintf(cmd,"../APIConsoleDebugger/bin/linux/x86/ProgramTest.exe -pci -reset");
// 	system(cmd);
// 	sprintf(cmd,"../APIConsoleDebugger/bin/linux/x86/ProgramTest.exe -pci -piaReset -ccu 0x%0x",ccuAddress);
// 	system(cmd);
	
	dcu->setDcuTestRegister ( dcuSetsToWrite[is] ) ;
	tscType8 readVal=dcu->getDcuTestRegister();
	//debugging:
	//std::cout <<"i read the value: "<<(int) readVal<<std::endl;
	if (readVal==(dcuSetsToWrite[is] & 0xfe)) {//if the last bit is set to zero
	  globalErr=1;
	  std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
          std::cout << "*** CCU 0x" << std::hex << ccuAddress << " CHAN 0x" << ccuChans[ic] << " FAILED TEST ERROR TYPE 1" << std::endl ;
	  std::cout <<"*****************************************************" <<std::endl;
          std::cout << "I failed when writing: 0x" << std::hex << dcuSetsToWrite[is] << std::endl ;
	  //the above is what indicates an i2c error type 1.  But if we don't get back what we
	  //set it's still a problem
	} else if (readVal!=dcuSetsToWrite[is]) {
	  globalErr=1;
	  std::cout<<"*** I2CProcedure, testErrorType1: didn't read back the set value ***"<<std::endl;
	}
      }
	
    }
    catch (FecExceptionHandler &e) {
	
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }  
    try {
      delete dcu ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }

    
    /* something bad happened.  Delete everything and try to exit gracefully */
  exit_failure: 
    if (failureFlag) {
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType1...."<<std::endl<<std::endl;
      if (dcu) {
	try {
	  delete dcu ;
	}
	catch (FecExceptionHandler &e) {
	  
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	}
      }
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
    /* end failure handling section */
  }
  
  return globalErr;
}
  
/* testErrorType2: tests errors type 2 according to Guido document.  Writes 0xbf to DCU TREG and checks for i2c error
 on subsequent read of the TREG*/
/* runs in sequence on all the ccu i2c channels specified for the ccu */

//requires:
//modifies: nothing

//ccuAddress: obvious
//ccuChans: array of ccu i2c channels to run the tests on.
//nCcuChans: length of ccuChans

bool testErrorType2(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans) {

  bool globalErr=0;
  int failureFlag=0;//flag to exit gracefully upon unknown failure

  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexDcu=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    char msgDcu[80] ; decodeKey(msgDcu,indexDcu) ; // use for display
    dcuAccess *dcu = NULL ;
    try {
      dcu = new dcuAccess ( fecAccess, indexDcu ) ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Cannot create an access to the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    if (dcu == NULL) {
      failureFlag=1;
      goto exit_failure;
    }
    try {
      dcu->setDcuTestRegister ( 0xbf  ) ;
    }		
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Error during the setting of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    //i do the read as a second try/catch block because to easily note the difference between a
    //system problem and the device failure signature
    try {
      tscType8 readVal=dcu->getDcuTestRegister();
      //the i2c error type 2 should produce an exception.  But its still a problem
      //if we don't read what we set
      if (readVal!=0xbf) {
	std::cout<<"I2CProcedure, testErrorType2: didn't read back the correct value from DCU"<<std::endl;
      }
    }		
    catch (FecExceptionHandler &e) {
      globalErr=1;
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
	
      std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
      std::cout << "**** " << msgDcu << " FAILED TEST ERROR TYPE 2" << std::endl ;
      std::cout <<"*****************************************************" <<std::endl;
    }
      
    try {
      delete dcu ;
    }
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    /* something bad happened.  Delete everything and try to exit gracefully */
  exit_failure: 
    if (failureFlag) {
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType2...."<<std::endl<<std::endl;
      if (dcu) {
	try {
	  delete dcu ;
	}
	catch (FecExceptionHandler &e) {
	  
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	}
      }
     try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
    /* end failure handling section */
  }
  return globalErr;
}

/* testErrorType3: tests errors type 3 according to Guido document ("problem 230").  Writes values to the laserdriver and 
   checks that this doesn't kill the i2c communication with the APVs. */
/* first sets lld settings, then tries to read the apvs.  Repeats this for four cases: setting even bias values, setting 
   odd bias values, setting even gain values, setting odd bias values */
/* runs in sequence on all the ccu i2c channels specified for the ccu */

//requires:
//modifies: nothing

//ccuAddress: obvious
//ccuChans: array of ccu i2c channels to run the tests on.
//nApvs: array of number of apvs expected per ccu chan
//nCcuChans: length of ccuChans array

bool testErrorType3(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int *nApvs, int nCcuChans) {
  
  bool globalErr=0;
  int failureFlag=0;
  
  for (int ic=0;ic<nCcuChans;ic++) {

    //create the access to lld and apvs for this ccu i2c channel
    keyType indexLld=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0x60);
    char msgLld[80] ; decodeKey(msgLld,indexLld) ; // use for display
    laserdriverAccess *lld = NULL ;
    apvAccess *apv[6]={6*0};
    keyType indexApv[6];
    apvDescription *apvD=NULL;
    for (int i=0;i<6;i++) {
      indexApv[i]=buildCompleteKey(0,0,(ccuAddress),ccuChans[ic],(0x20+i));
    }
    
    try {
      lld = new laserdriverAccess ( fecAccess, indexLld ) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Cannot create an access to the LLD: "<< msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    try {
      apv[0]=new apvAccess (fecAccess, indexApv[0]);
      apv[1]=new apvAccess (fecAccess, indexApv[1]);
      apv[4]=new apvAccess (fecAccess, indexApv[4]);
      apv[5]=new apvAccess (fecAccess, indexApv[5]);
      if (nApvs[ic]==6) {
	apv[2]=new apvAccess (fecAccess, indexApv[2]);
	apv[3]=new apvAccess (fecAccess, indexApv[3]);
      }	
    } catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Cannot create an access to the APVs: " << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    if (lld==NULL || apv[0]==NULL || apv[1]==NULL || apv[4]==NULL || apv[5]==NULL || ((nApvs[ic]==6 && apv[3]==NULL) || apv[4]==NULL)) {
      failureFlag=1;
      goto exit_failure;
    }
    
    /* set the even bias values and check the apv read*/
    try {
      tscType8 bias[3]={0x24,0x24,0x24};
      lld->setBias (bias) ;
      lld->getBias (bias);
    }		
    catch (FecExceptionHandler &e) {
      
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Error during the setting of the LLD " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
  
    //get the apvs and if an error it's a device failure
    try {
      apvD=apv[0]->getValues();
      apvD=apv[1]->getValues();
      apvD=apv[4]->getValues();
      apvD=apv[5]->getValues();
      if (nApvs[ic]==6) {
	apvD=apv[2]->getValues();
	apvD=apv[3]->getValues();
      }
    }
    catch (FecExceptionHandler &e) {
      char failstr[100];
      sprintf(failstr,"I2CProcedure::testErrorType3: CCU 0x%0x CHAN 0x%0x\n",ccuAddress,ccuChans[ic]);
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
      std::cout <<"*** "<<failstr<<" FAILED TEST ERROR TYPE 3"<<std::endl;
      std::cout <<"*****************************************************" <<std::endl;
    }
    
    /* set the odd bias values and check the apv read*/
    try {
      tscType8 bias[3]={0x25,0x25,0x25};
      lld->setBias (bias);
      lld->getBias (bias);
    }		
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Error during the setting of the DCU " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    //get the apvs and if an error it's a device failure
    try {
      apvD=apv[0]->getValues();
      apvD=apv[1]->getValues();
      apvD=apv[4]->getValues();
      apvD=apv[5]->getValues();
      if (nApvs[ic]==6) {
	apvD=apv[2]->getValues();
	apvD=apv[3]->getValues();
      }
    }
    catch (FecExceptionHandler &e) {
      char failstr[100];
      sprintf(failstr,"I2CProcedure::testErrorType3: CCU 0x%0x CHAN 0x%0x\n",ccuAddress,ccuChans[ic]);
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
      std::cout <<"*** "<<failstr<<" FAILED TEST ERROR TYPE 3"<<std::endl;
      std::cout <<"*****************************************************" <<std::endl;
    }
    
    /* set the even gain values and check the apv read*/
    try {
      tscType8 gain=0x2;
      gain=gain<<2;
      gain|=0x2;
      gain=gain<<2;
      gain|=0x2;
      gain=gain<<2;
      gain|=0x2;
      lld->setGain (gain) ;
      lld->getGain (); 
    }		
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Error during the setting of the DCU " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    //get the apvs and if an error it's a device failure
    try {
      apvD=apv[0]->getValues();
      apvD=apv[1]->getValues();
      apvD=apv[4]->getValues();
      apvD=apv[5]->getValues();
      if (nApvs[ic]==6) {
	apvD=apv[2]->getValues();
	apvD=apv[3]->getValues();
      }
    }
    catch (FecExceptionHandler &e) {
      char failstr[100];
      sprintf(failstr,"I2CProcedure::testErrorType3: CCU 0x%0x CHAN 0x%0x\n",ccuAddress,ccuChans[ic]);
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
      std::cout <<"*** "<<failstr<<" FAILED TEST ERROR TYPE 3"<<std::endl;
      std::cout <<"*****************************************************" <<std::endl;
    }
    
    /* set the odd gain values and check the apv read*/
    try {
      tscType8 gain=0x3;
      gain=gain<<2;
      gain|=0x3;
      gain=gain<<2;
      gain|=0x3;
      gain=gain<<2;
      gain|=0x3;
      lld->setGain (gain) ;
      lld->getGain (); 
    }		
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Error during the setting of the LLD " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    //get the apvs and if an error it's a device failure
    try {
      apvD=apv[0]->getValues();
      apvD=apv[1]->getValues();
      apvD=apv[4]->getValues();
      apvD=apv[5]->getValues();
      if (nApvs[ic]==6) {
	  apvD=apv[2]->getValues();
	  apvD=apv[3]->getValues();
      }
    }
    catch (FecExceptionHandler &e) {
      char failstr[100];
      sprintf(failstr,"I2CProcedure::testErrorType3: CCU 0x%0x CHAN 0x%0x\n",ccuAddress,ccuChans[ic]);
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
      std::cout <<"*** "<<failstr<<" FAILED TEST ERROR TYPE 3"<<std::endl;
      std::cout <<"*****************************************************" <<std::endl;
    }

    //cleanup
    try {
      delete lld ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType3: Error deleting the laserdriverAccess " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    //already checked that apvs that should be non-null are non-null
    for (int iapv=0;iapv<6;iapv++) {
      if (apv[iapv]!=NULL) {
	try {
	  delete apv[iapv];
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "I2CProcedure::testErrorType3: Error deleting the apvAccess " << msgLld << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	  failureFlag=1;
	  goto exit_failure;
	}
      }
    }
    /* something bad happened.  Delete everything and try to exit gracefully */
  exit_failure: 
    if (failureFlag) {
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType3...."<<std::endl<<std::endl;
      if (lld) {
	try {
	  delete lld ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "Error during the deleting of the LLD " << msgLld << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	}
      }
      for (int iapv=0;iapv<6;iapv++) {
	if (apv[iapv]!=NULL) {
	  try {
	    delete apv[iapv];
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "*************** ERROR *********************" << std::endl ;
	    std::cerr << e.what() << std::endl ;
	    std::cerr << "I2CProcedure::testErrorType3: Error deleting the apvAccess " << msgLld << std::endl ;
	    std::cerr << "*******************************************" << std::endl ;
	  }
	}
      }
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
    /* end failure handling section */
  }   
  return globalErr;
 
}

bool testErrorType4(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int *nApvs, int nCcuChans) {
  
  //these are the values to write/read to TREG that might cause problems
  int dcuSetsToWrite[4]={0xc1,0xc3,0xc5,0xc7};
  int nDcuSets=4;

  bool globalErr=0;
  int failureFlag=0;
  
  for (int ic=0;ic<nCcuChans;ic++) {
    //create the access to the DCU and LLD
    keyType indexDcu=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    char msgDcu[80] ; decodeKey(msgDcu,indexDcu) ; // use for display
    dcuAccess *dcu = NULL;
    keyType indexLld=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0x60);
    char msgLld[80] ; decodeKey(msgLld,indexLld) ; // use for display
    laserdriverAccess *lld = NULL ;
    try {
      dcu = new dcuAccess ( fecAccess, indexDcu ) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType4: Cannot create an access to the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    try {
      lld = new laserdriverAccess ( fecAccess, indexLld ) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType4: Cannot create an access to the LLD: "<< msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    for (int is=0;is<nDcuSets;is++) {
      try {
	dcu->setDcuTestRegister ( dcuSetsToWrite[is] ) ;
	dcu->getDcuTestRegister();
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "*************** ERROR *********************" << std::endl ;
	std::cerr << e.what() << std::endl ;
	std::cerr << "I2CProcedure::testErrorType4: Error during the settings of the DCU " << msgDcu << std::endl ;
	std::cerr << "*******************************************" << std::endl ;
	failureFlag=1;
	goto exit_failure;
      }
      try {
	tscType8 gain=0x3;
	lld->setGain (gain) ;
	lld->getGain (); 
      }		
      catch (FecExceptionHandler &e) {
	//i2c error here means we have a device failure
	globalErr=1;
	std::cerr << "*************** ERROR *********************" << std::endl ;
	std::cerr << e.what() << std::endl ;
	std::cerr << "Error during the setting of the LLD " << msgLld << std::endl ;
	std::cerr << "*******************************************" << std::endl ;
	std::cout <<"***************** DEVICE FAILURE ********************" <<std::endl;
	std::cout <<"*** FAILED TEST ERROR TYPE 4"<<std::endl;
	std::cout <<"*****************************************************" <<std::endl;
      }
    }
    //cleanup
    try {
      delete lld ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType4: Error during the deletion of the LLD " << msgLld << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    try {
      delete dcu ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "*************** ERROR *********************" << std::endl ;
      std::cerr << e.what() << std::endl ;
      std::cerr << "I2CProcedure::testErrorType4: Error during the deletion of the DCU " << msgDcu << std::endl ;
      std::cerr << "*******************************************" << std::endl ;
      failureFlag=1;
      goto exit_failure;
    }
    
    /* something bad happened.  Delete everything and try to exit gracefully */
  exit_failure: 
    if (failureFlag) {
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType4...."<<std::endl<<std::endl;
      if (lld) {
	try {
	  delete lld ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "Error during the deleting of the LLD " << msgLld << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	}
      }
      if (dcu) {
	try {
	  delete dcu ;
	}
	catch (FecExceptionHandler &e) {
	  std::cerr << "*************** ERROR *********************" << std::endl ;
	  std::cerr << e.what() << std::endl ;
	  std::cerr << "Error during the settings of the DCU " << msgDcu << std::endl ;
	  std::cerr << "*******************************************" << std::endl ;
	}
      }
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
      /* end failure handling section */
    }   
  }
  return globalErr;
}

bool testErrorType5(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans) {
  
  //set the i2c speed to 1MHz for all the channels
  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexF=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    tscType8 channelCRA = 0 ;
    try {
      channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
      channelCRA &= 0xFC ;
      channelCRA |= 0x2;
      fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "I2CProcedure::testErrorType5: Cannot get/set the CRA of i2c channel: " << e.what() << std::endl ;
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType5...."<<std::endl<<std::endl;
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
  }
  
  //error type 5 is just error type 1 at 1MHz
  bool globalErr = testErrorType1(fecAccess, ccuAddress, ccuChans, nCcuChans);
  
  //set the speed back to 100kHz for all the channels
  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexF=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    tscType8 channelCRA = 0 ;
    try {
      channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
      channelCRA &= 0xFC ;
      channelCRA |= 0x0;
      fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "I2CProcedure::testErrorType5: Cannot get/set the CRA of i2c channel: " << e.what() << std::endl ;
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType5...."<<std::endl<<std::endl;
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
  }
  
  return globalErr;
  
}

bool testErrorType6(FecAccess *fecAccess, unsigned int ccuAddress, unsigned int *ccuChans, int nCcuChans) {

  //set the i2c speed to 1MHz for all channels
  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexF=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    tscType8 channelCRA = 0 ;
    try {
      channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
      channelCRA &= 0xFC ;
      channelCRA |= 0x2;
      fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "I2CProcedure::testErrorType6: Cannot get/set the CRA of i2c channel: " << e.what() << std::endl ;
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType6...."<<std::endl<<std::endl;
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
  }

  //error type 6 is just error type 2 at 1MHz
  bool globalErr = testErrorType2(fecAccess, ccuAddress, ccuChans, nCcuChans);
  
  //set the speed back to 100kHz for all channels
  for (int ic=0;ic<nCcuChans;ic++) {
    keyType indexF=buildCompleteKey(0,0,ccuAddress,ccuChans[ic],0);
    tscType8 channelCRA = 0 ;
    try {
      channelCRA = fecAccess->geti2cChannelCRA (indexF) ;
      channelCRA &= 0xFC ;
      channelCRA |= 0x0;
      fecAccess->seti2cChannelCRA (indexF,channelCRA) ;
    }
    catch (FecExceptionHandler &e) {
      std::cerr << "I2CProcedure::testErrorType6: Cannot get/set the CRA of i2c channel: " << e.what() << std::endl ;
      std::cerr<<std::endl<<"Exiting at I2CProcedure::testErrorType6...."<<std::endl<<std::endl;
      try {
	delete fecAccess ;
      }
      catch (FecExceptionHandler &e) {
	
	std::cerr << "------------ Exception ----------" << std::endl ;
	std::cerr << e.what() << " when I delete the FecAccess" << std::endl ;
	std::cerr << "---------------------------------" << std::endl ;
      }
      exit(-1);
    }
  }
  
  return globalErr;
  
}


/** Create the FEC Access class depending on the version of the FecSoftware and the FEC type
 */
FecAccess *createFecAccess ( int argc, char **argv, int *cnt ) {

  FecAccess *fecAccess = NULL ;

  int cpt = 1 ;

  // FEC type
  enumFecBusType fecBusType = FECVME ;
  enumBusAdapter busAdapter = SBS    ;

  // VME configuration
#define MAXCARS 1000
  char vmeFileName[MAXCARS] = "" ;

  if (argc >= 2) {
    if (strcasecmp (argv[cpt],"-pci") == 0) { // If a new fecAddress has been set
      
      fecBusType = FECPCI ;
      cpt ++ ;
    } 
    else if ( (strcasecmp (argv[cpt],"-vmesbs") == 0) || (strcasecmp (argv[cpt],"-vme") == 0)) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = SBS    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenpci") == 0) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME ;
      busAdapter = CAENPCI    ;
    }
    else if (strcasecmp (argv[cpt],"-vmecaenusb") == 0) { // If a new fecAddress has been set

      cpt ++ ;
      fecBusType = FECVME  ;
      busAdapter = CAENUSB ;
    }
    else if (strcasecmp (argv[cpt],"-usb") == 0) { // If a new fecAddress has been set
      
      fecBusType = FECUSB ;
      cpt ++ ;
    } 
  }

  if (fecBusType == FECVME) {
    // Check if a filename is given
    if ( (cpt < argc) && (argv[cpt][0] != '-') ) {
      strncpy (vmeFileName, argv[cpt], MAXCARS) ;
      cpt++ ;
      
      std::ifstream fichier (vmeFileName) ;
      if (fichier) {
	std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "The file " << vmeFileName << " does not exists" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
    else {

      // $ENV_TRACKER_DAQ or $ENV_CMS_TK_FEC_ROOT or $FECSOFTWARE_ROOT /config/FecAddressTable.dat
      char *basic=getenv ("ENV_TRACKER_DAQ") ;
      if (basic == NULL) basic=getenv ("ENV_CMS_TK_FEC_ROOT") ;
      if (basic == NULL) basic=getenv ("FECSOFTWARE_ROOT") ;
      if (basic != NULL) {
	strncpy (vmeFileName, basic, MAXCARS) ;
	strncat (vmeFileName, "/config/FecAddressTable.dat", MAXCARS) ;
	std::ifstream fichier (vmeFileName) ;
	if (! fichier) basic = NULL ;
      }

      if (basic) {
	std::cout << "VME FEC will be used with the file " << vmeFileName << std::endl ;
      }
      else {
	std::cerr << "No file found for VME" << std::endl ;
	exit (EXIT_FAILURE) ;
      }
    }
  } 
  
  // Create the FEC Access
  switch ( fecBusType ) {
  case FECPCI:
    // bool forceAck, bool initFec, bool scan
    fecAccess = new FecAccess (true, false, true, false, 100) ;
    break ;
  case FECVME:
    // ulong adapterSlot, std::string configurationFile, 
    // bool forceAck, bool init, bool scan
    fecAccess = new FecAccess (0, vmeFileName, FecVmeRingDevice::VMEFECBASEADDRESSES, true, false, true, false, 100, FecVmeRingDevice::STR_BUSADAPTER_NAME[busAdapter]) ;
    break ;
  case FECUSB:
    // FEC usb
    fecAccess = new FecAccess (0,true, false, true, false, 100) ;
    break ;
  }

  if (!fecAccess) {
     std::cout << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
     exit (EXIT_FAILURE) ; ; 
  }

  std::list<keyType> *fecList = fecAccess->getFecList() ;

  if ((fecList == NULL) || (fecList->empty())) {
    std::cerr << "Warning: No FEC rings board found" << std::endl ;
  }

  delete fecList;

  *cnt = cpt ;

  return (fecAccess) ;
}

void helpMe() {
  
  std::cout<<  "//required arguments:" <<std::endl;
  std::cout<<      "//-pci " <<std::endl;
  std::cout<<  "//-ccu <ccuAddress> !address in hex!" <<std::endl;
  std::cout<<  "//-rtype <rtype> type of rod, either ss4, ss6, or ds" <<std::endl;
  std::cout<<  "//example: I2CProcedure -pci -ccu 0x16 -rtype ss4" <<std::endl;

}

//jlamb: this function taken from APIAccess.cc.  I couldn't make to include that library (don't know why...)
//it is more or less intact, I commented out the block referring to noGetchar_

void allCCUsPiaReset ( FecAccess *fecAccess, tscType8 fecAddress, tscType8 ringAddress ) {

  std::cout << "Making a PIA reset for the FEC " << std::dec << (int)fecAddress << " ring " << (int)ringAddress << " on all CCUs on all PIA channels" << std::endl ;

  keyType indexFEC = buildFecRingKey(fecAddress,ringAddress) ;
      
  try {
    // Scan the ring for each FECs and display the CCUs
    std::list<keyType> *ccuList = fecAccess->getCcuList ( indexFEC ) ;
    
    if (ccuList != NULL) {
      
      for (std::list<keyType>::iterator p=ccuList->begin() ; p!=ccuList->end() ; p++) {
	
	keyType index = *p ;
	
	// For each channel
	for (tscType8 channel = 0x30 ; channel <= 0x33 ; channel ++) {
	  
	  try {
	    keyType indexPIA = buildCompleteKey(getFecKey(index),getRingKey(index),getCcuKey(index),channel,0) ;
		
	    char msg[80] ;
	    decodeKey(msg, indexPIA) ;
	    
	    PiaResetAccess piaResetAccess(fecAccess, indexPIA, 0xFF) ;
	    piaResetDescription piaD ( index, (unsigned long)1000, (unsigned long)100, (tscType8)0xFF ) ;
	    piaResetAccess.setValues (piaD) ;

	    //piaResetDescription *piaD1 = piaResetAccess.getValues ( ) ;
	    
	    //if (piaD == *piaD1)
	    //std::cout << "Pia reset comparison ok" << std::endl ;
	    //else
	    //std::cout << "Pia reset comparison error" << std::endl ;
	  }
	  catch (FecExceptionHandler &e) {

	    std::cout << "------------ Exception ----------" << std::endl ;
	    std::cout << e.what()  << std::endl ;
	    std::cout << "---------------------------------" << std::endl ;

	    // display all registers
	    if (e.getFecRingRegisters() != NULL ) FecRingRegisters::displayAllRegisters ( *(e.getFecRingRegisters()) ) ;
	    // Original frame
	    if (e.getFrame() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFrame()) << std::endl ;	    
	    if (e.getDAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getDAck()) << std::endl ;
	    if (e.getFAck() != NULL) std::cerr << FecRingRegisters::decodeFrame (e.getFAck()) << std::endl ;
	  }
	}
      }
    }
    else {
      std::cerr << "Did not found CCU on the FEC " << getFecKey(indexFEC) << " ring " << getRingKey(indexFEC) << std::endl ;
    }
    
    delete ccuList ;
  }
  catch (FecExceptionHandler &e) {
    
    std::cerr << "*********** ERROR ********************************" << std::endl ; 
    std::cerr << "An error occurs during hardware access" << std::endl ;
    std::cerr << e.what()  << std::endl ;
    std::cerr << "**************************************************" << std::endl ;
  }
}
