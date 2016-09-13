/* File auto-generated - Do not modify */
/*
	FileName :		DD_PERROR.H

	Content :		export of dd_perror.c functions headers

	Used in :		Client program

	Programmer :		Laurent GROSS

	Version :		Unified-6.0

	Support :		mail to : fec-support@ires.in2p3.fr

	Remarks :		This file should not be modified by users ;
				File auto - generated from dderrors.h descriptions.
*/


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

Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3
*/


/*!
Begin Block - avoid multiple inclusions of this file
*/
#ifndef DD_PERROR_H
#define DD_PERROR_H


#ifdef __cplusplus
extern "C" {
#endif


/*!
<b>FUNCTION : dd_get_error_msg</b>
- Job
	- Link an error code to the appropriate error message.
	This function is the public interface for users.
- I/O
	- Inputs
		- DD_TYPE_ERROR code : Error code to analyse
	- Outputs
		- char *msg : string pointer in which the error message will be stored.
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- Void
- Notes
	- the string pointer *msg given as THIRD argument should have a minimal
	length of DD_MAX_DECODED_ERROR_MSG_LENGTH characters ; an exact length would be better.
	- DD_TYPE_ERROR is currently defined as int
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- void
- Sub-functions calls :
	- snprintf	(system)
*/
DD_TYPE_ERROR dd_get_error_msg ( DD_TYPE_ERROR code, char *msg);


/*!
<b>FUNCTION : dd_print_error_msg</b>
- Job
	- Link an error code to the appropriate error message, and
	prints it to the standard output for errors (stderr).
- I/O
	- Inputs
		- DD_TYPE_ERROR code : Error code to analyse
	- Outputs
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- Void
- Notes
	- DD_TYPE_ERROR is currently defined as int
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- void
- Sub-functions calls :
	- fprintf	(system)
*/
DD_TYPE_ERROR dd_print_error_msg ( DD_TYPE_ERROR code);


/*!
<b>FUNCTION : dd_get_error_msg</b>
- Job
	- Link an error code to the appropriate error message.
	This function should be a private function, and should not be accessed
	directly, but is nevertheless exported via dd_perror.h file for now.
- I/O
	- Inputs
		- DD_TYPE_ERROR param_code : Error code to analyse
	- Outputs
	- char *param_errormessage : string pointer in which the error message will be stored.
		- Error code returned by the function
- Internals
	- Void
- Global variables accessed
	- Void
- Notes
	- the string pointer *msg given as THIRD argument should have a minimal
	length of DD_MAX_DECODED_ERROR_MSG_LENGTH characters ; an exact length would be better.
	- DD_TYPE_ERROR is currently defined as int
- Error Management & Values returned
	- Operation successful :
		- DD_RETURN_OK
	- Errors :
		- void
- Sub-functions calls :
	- snprintf	(system)
*/
DD_TYPE_ERROR dd_decode_error ( DD_TYPE_ERROR code, char *msg);


/*!
End Block - avoid multiple inclusions of this file
*/
#ifdef __cplusplus
}
#endif
#endif
