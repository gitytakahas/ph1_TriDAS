// $Id: TkExcept.h,v 1.1 2008/06/09 13:32:15 lgross Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/

#ifndef _xdaq_exception_TkExcept_h_
#define _xdaq_exception_TkExcept_h_

#include "xdaq/exception/Exception.h"

namespace xdaq {
	namespace exception { 
		class TkExcept: public xdaq::exception::Exception 
		{
			public: 
			TkExcept( std::string name, std::string message, std::string module, int line, std::string function ): 
					xdaq::exception::Exception(name, message, module, line, function) 
			{} 
			TkExcept( std::string name, std::string message, std::string module, int line, std::string function, xcept::Exception & e ): 
					xdaq::exception::Exception(name, message, module, line, function, e) 
			{} 
		}; 
	} 
}

#endif
