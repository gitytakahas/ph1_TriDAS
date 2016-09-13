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

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3

*/

#ifndef ESDBMBRESETACCESS_H
#define ESDBMBRESETACCESS_H

#include "ESDbAccess.h"
#include "stringConv.h"
#include "esMbResetDescription.h"

/** \brief This class is implemented to handle the communication between the FEC supervisor software
 *  and the database.
 *
 * This class provides some features like :
 *	- OCCI environnement and database connection opening and closing
 *	- download and upload execution
 *	- database version request
 *
 */
class ESDbMbResetAccess : public ESDbAccess {
	private:
		//
		// private attributes
		//
		
	public:
		//
		// public functions
		//
		/** \brief Default constructor
		 */
		ESDbMbResetAccess(bool threaded = false) throw (oracle::occi::SQLException);
		
		/** \brief Constructor with connection parameters
		 */
		ESDbMbResetAccess (std::string user, std::string passwd, std::string dbPath, bool threaded = false) throw (oracle::occi::SQLException);
		
		
		/** \brief Destructor
		 */
		~ESDbMbResetAccess() throw (oracle::occi::SQLException);
		
		/** \brief Download a Clob from the database
		 */
		oracle::occi::Clob *getXMLClob (std::string partitionName) throw (oracle::occi::SQLException);
		
		/** \brief Download a Clob from the database
		 */
		oracle::occi::Clob *getXMLClob (std::string partitionName, std::string fecHardId) throw (oracle::occi::SQLException);
		
		/** \brief Upload a Clob from the database
		 */
		void setXMLClob (std::string buffer, std::string partitionName) throw (oracle::occi::SQLException, FecExceptionHandler);
	
};

#endif
 
