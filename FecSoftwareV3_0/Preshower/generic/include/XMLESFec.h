/*
This file is part of Fec Software project.
It is used to test the performance of DB--FecSoftware

author: HUNG Chen-Chien(Dominique), NCU, Jhong-Li, Taiwan
based on XMLFec by
Frderic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/


#ifndef XMLESFEC_H
#define XMLESFEC_H

#ifdef DATABASE
#include "ESDbAccess.h"
#endif

// use in XMLESFec to declare ofstream
#include <fstream>
#include <sstream>

#include "deviceType.h"
#include "XMLCommonFec.h"

/** \brief This class represents an interface between the FEC supervisor software and the parameter value storage ( database or file ).<BR>
 * This class provides some features like :
 *  - downloading the parameter values from an XML file or from the database
 *  - uploading the parameter values into an XML file or into the database
 *
 * SAX parser(<I>parser_</I>) and Sax handler(<I>handler_</I>) are XMLFec attributes.<BR>
 * Otherwise, <I>handler_</I> will be instanciated in subclasses, depending what you need (Device, PiaReset,...).<BR> 
 * Features for <I>parser_</I> (error handler, document handler) will also be set in subclasses, once the <I>handler_</I> is created.<BR>
 */
class XMLESFec: public XMLCommonFec {
	protected:

  	/** Counter of elements
   		*/
  	unsigned int countPLL, countKchip, countDCUCCU, countDCUFEH, countDelta, countESMB, countPace;

 	public:

  	/** \brief Default constructor
   	 */
  	XMLESFec ();

  	/** \brief Constructor with xml buffer
   	 */
  	XMLESFec ( const XMLByte* buffer ) throw (FecExceptionHandler);

	#ifdef DATABASE
  	/** \brief Constructor whith database access
   	 */
  	XMLESFec ( ESDbAccess *dbAccess ) throw (FecExceptionHandler);
	#endif

  /** \brief Constructor with file access
   */
  XMLESFec ( std::string xmlFileName ) throw (FecExceptionHandler);

  /** \brief Deletes the device vector private attribute
   */
  virtual ~XMLESFec ();

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountPLL() { return countPLL ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountKchip() { return countKchip ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountDCUCCU() { return countDCUCCU ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountDCUFEH() { return countDCUFEH ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountDelta() { return countDelta ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountESMB() { return countESMB ; }

  	/** \brief return the number of devices
   	 * \param number of devices
   	 */
  	inline unsigned int getCountPace() { return countPace ; }


  /** \brief clear the vector
   */
  virtual void clearVector() = 0 ;

	#ifdef DATABASE
  	/** \brief This method sends a request to the database and parse the input source retrieved from database
   	 */
  	void parseDatabaseResponse ( std::string partitionName ) throw (FecExceptionHandler);

  	/** \brief This method sends a request to the database and parse the input source retrieved from database
   	 */
  	void parseDatabaseResponse ( std::string partitionName, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) ;

  	/** \brief This method sends a request to the database and parse the input source retrieved from database
   	 */
  	void parseDatabaseResponse ( std::string partitionName,  std::string fecHardId ) throw (FecExceptionHandler);

  	/** \brief This method sends a request to the database and parse the input source retrieved from database
   	 */
  	void parseDatabaseResponse ( std::string partitionName,  std::string fecHardId, unsigned int versionMajor, unsigned int versionMinor ) throw (FecExceptionHandler) ;
	#endif

  /** \brief prototype for the parser
   	*/
  virtual unsigned int parseAttributes ( XERCES_CPP_NAMESPACE::DOMNode *n ) = 0 ;

	/*   /\** \brief Parse the elements for a FEC buffer */
	/*    *\/ */
	/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, deviceVector &vDevices ) ; */

	/*   /\** \brief Parse the elements for a FEC buffer */
	/*    *\/ */
	/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, piaResetVector &vPiaReset ) ; */

	/*   /\** \brief Parse the elements for a FEC buffer */
	/*    *\/ */
	/*   unsigned int parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n, deviceVector &vDevices, piaResetVector &vPiaReset) ; */
} ;

#endif
