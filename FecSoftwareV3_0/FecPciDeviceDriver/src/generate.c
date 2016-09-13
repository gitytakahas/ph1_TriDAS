/*
   FileName : 		GENERATE.C

   Content : 		Helper that auto-generates the files associated to error management
   			when the error list described in includes/dderrors.h is modified.
			No one except me should ever need this helper program, because no one
			except me should have to qualify new Fec Driver errors.

   Used in : 		DRIVER

   Programmer : 	Laurent GROSS

   Version : 		Unified-6.0
   
   Date of last modification : 03/05/2005

   Support : 		mail to : fec-support@ires.in2p3.fr
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


/*
error management files auto-generator.
You should not have to use this file ; 
I only put it on cvs for backup purposes.
*/
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
FILE *filein;
FILE *c_fileout;
FILE *h_fileout;


void put_sourcefile_headers(FILE *);
void put_sourcefile_body(FILE *, FILE *);
void put_sourcefile_end(FILE *);
void put_includefile_contents(FILE *h_fileout);



/* First, check if args are present */
if (argc != 4)
{
	printf("Usage is : <program> <input filename> <output source filename> <output header filename>\n");
	return 0;
}


/* then, try to open input file */
if ((filein=fopen(argv[1],"r")) == NULL)
{
	printf("Unable to open input file %s\n",argv[1]);
	printf("Aborting program.\n");
	return (1);
}


/* then, try to open output source file */
if ((c_fileout=fopen(argv[2],"w")) == NULL)
{
	printf("Unable to open output source file %s.c\n",argv[2]);
	printf("Aborting program.\n");
	fclose(filein);
	return (1);
}


/* then, try to open output header file */
if ((h_fileout=fopen(argv[3],"w")) == NULL)
{
	printf("Unable to open output source file %s.h\n",argv[3]);
	printf("Aborting program.\n");
	fclose(filein);
	fclose(c_fileout);
	return (1);
}

/* files are open, now auto-generate source file */
put_sourcefile_headers(c_fileout);
put_sourcefile_body(filein, c_fileout);
put_sourcefile_end(c_fileout);

/* auto-generate header file */
put_includefile_contents(h_fileout);

fclose(filein);
fclose(c_fileout);
fclose(h_fileout);
return 0;
}





void put_sourcefile_headers(FILE *c_fileout)
{
	/* PRINT PAGE header */
	fprintf(c_fileout,"/* File auto-generated - Do not modify */\n");
	fprintf(c_fileout,"/*\n");
	fprintf(c_fileout,"\tFileName :\t\tDD_PERROR.C\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tContent :\t\tTranscription of numeric error code towards text report\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tUsed in :\t\tClient program\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tProgrammer :\t\tLaurent GROSS\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tVersion :\t\tUnified-6.0\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tSupport :\t\tmail to : fec-support@ires.in2p3.fr\n");
	fprintf(c_fileout,"\n");
	fprintf(c_fileout,"\tRemarks :\t\tThis file should not be modified by users ;\n");
	fprintf(c_fileout,"\t\t\t\tFile auto - generated from dderrors.h descriptions.\n");
	fprintf(c_fileout,"*/\n");


	/* PRINT GNU-GPL header */
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*\n");
	fprintf(c_fileout,"This file is part of Fec Software project.\n\n");
	fprintf(c_fileout,"Fec Software is free software; you can redistribute it and/or modify\n");
	fprintf(c_fileout,"it under the terms of the GNU General Public License as published by\n");
	fprintf(c_fileout,"the Free Software Foundation; either version 2 of the License, or\n");
	fprintf(c_fileout,"(at your option) any later version.\n\n");
	fprintf(c_fileout,"Fec Software is distributed in the hope that it will be useful,\n");
	fprintf(c_fileout,"but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	fprintf(c_fileout,"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	fprintf(c_fileout,"GNU General Public License for more details.\n\n");
	fprintf(c_fileout,"You should have received a copy of the GNU General Public License\n");
	fprintf(c_fileout,"along with Fec Software; if not, write to the Free Software\n");
	fprintf(c_fileout,"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\n");
	fprintf(c_fileout,"Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3\n");
	fprintf(c_fileout,"*/\n");


	/* PRINT INCLUDES header */
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*\n");
	fprintf(c_fileout,"Standard includes\n");
	fprintf(c_fileout,"*/\n");
	fprintf(c_fileout,"#include <stdio.h>\n");
	fprintf(c_fileout,"#include <string.h>\n");
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*\n");
	fprintf(c_fileout,"device driver dependant includes\n");
	fprintf(c_fileout,"*/\n");
	fprintf(c_fileout,"#include \"../include/datatypes.h\"\n");
	fprintf(c_fileout,"#include \"../include/dderrors.h\"\n");
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*\n");
	fprintf(c_fileout,"specific includes\n");
	fprintf(c_fileout,"*/\n");
	fprintf(c_fileout,"#include \"../include/dd_perror.h\"\n");



	/* PRINT FIRST FUNCTION HEADER */
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*!\n");
	fprintf(c_fileout,"See the public file dd_perror.h for this API description.\n");
	fprintf(c_fileout,"*/\n");

	/* PRINT FIRST FUNCTION BODY */
	fprintf(c_fileout,"DD_TYPE_ERROR dd_get_error_msg ( DD_TYPE_ERROR code, char *msg )\n");
	fprintf(c_fileout,"{\n");
	fprintf(c_fileout,"\tstrcpy(msg,\"\");\n");
	fprintf(c_fileout,"\tdd_decode_error ( code, msg);\n\n");
	fprintf(c_fileout,"return DD_RETURN_OK;\n");
	fprintf(c_fileout,"}\n");



	/* PRINT SECOND FUNCTION HEADER */
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*!\n");
	fprintf(c_fileout,"See the public file dd_perror.h for this API description.\n");
	fprintf(c_fileout,"*/\n");

	/* PRINT SECOND FUNCTION BODY */
	fprintf(c_fileout,"DD_TYPE_ERROR dd_print_error_msg ( DD_TYPE_ERROR code)\n");
	fprintf(c_fileout,"{\n");
	fprintf(c_fileout,"char lcl_error_msg[DD_MAX_DECODED_ERROR_MSG_LENGTH];\n\n");
	fprintf(c_fileout,"\tdd_decode_error ( code, lcl_error_msg);\n");
	fprintf(c_fileout,"\tfprintf(stderr, \"Error \%%d : \%%s\\n\", code, lcl_error_msg);\n\n");

	fprintf(c_fileout,"return DD_RETURN_OK;\n");
	fprintf(c_fileout,"}\n");



	/* PRINT THIRD FUNCTION HEADER */
	fprintf(c_fileout,"\n\n");
	fprintf(c_fileout,"/*!\n");
	fprintf(c_fileout,"See the public file dd_perror.h for this API description.\n");
	fprintf(c_fileout,"*/\n");

	/* PRINT THIRD FUNCTION BEGINNING OF BODY */
	fprintf(c_fileout,"DD_TYPE_ERROR dd_decode_error ( DD_TYPE_ERROR param_code, char *param_errormessage )\n");
	fprintf(c_fileout,"{\n");
	fprintf(c_fileout,"\tswitch (param_code)\n");
	fprintf(c_fileout,"\t{\n");


}


void put_sourcefile_body(FILE *filein, FILE *c_fileout)
{
char onestring[512];
char error_msg[512];
char error_name[512];

/* analyse input ; */
/* generates output according to input */

	/* initialize string to NULL */
	strcpy(onestring,"");

	/* now, parse file until string read is "BEGIN_TAG */
	while (!(feof(filein)))
	{
		while ( (strcmp(onestring,"BEGIN_TAG\n") != 0) &&  (!(feof(filein))) ) fgets(onestring,511,filein);
		if (!(feof(filein)))
		{

		/* once tag is found, store error message */
		fgets(error_msg,511,filein);
		error_msg[strlen(error_msg)-1]='\0';

		/* unbuff END_TAG tag */
		fgets(onestring,511,filein);

		/*unbuff \*\/ tag */
		fgets(onestring,511,filein);

		/* unbuff #define */
		fscanf(filein, "%s", onestring);

		/* store error name */
		fscanf(filein, "%s", error_name);

		/* now, dump info in source file */
		fprintf(c_fileout,"\t\tcase %s:\n", error_name);
		fprintf(c_fileout,"\t\t\tsnprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),\"%s\");\n", error_msg);
		fprintf(c_fileout,"\t\tbreak;\n");
		}
	}
}





void put_sourcefile_end(FILE *c_fileout)
{
	fprintf(c_fileout,"\t\tdefault:\n");
	fprintf(c_fileout,"\t\t\tsnprintf (param_errormessage, (DD_MAX_DECODED_ERROR_MSG_LENGTH-1),\" The error code %%d is not defined in the function dd_perror\", param_code) ;\n");
	fprintf(c_fileout,"\t\tbreak;\n");
	fprintf(c_fileout,"\t}\n");
	fprintf(c_fileout,"\treturn DD_RETURN_OK;\n");
	fprintf(c_fileout,"}\n");
}



void put_includefile_contents(FILE *h_fileout)
{
	/* PRINT PAGE headers */
	fprintf(h_fileout,"/* File auto-generated - Do not modify */\n");
	fprintf(h_fileout,"/*\n");
	fprintf(h_fileout,"\tFileName :\t\tDD_PERROR.H\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tContent :\t\texport of dd_perror.c functions headers\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tUsed in :\t\tClient program\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tProgrammer :\t\tLaurent GROSS\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tVersion :\t\tUnified-6.0\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tSupport :\t\tmail to : fec-support@ires.in2p3.fr\n");
	fprintf(h_fileout,"\n");
	fprintf(h_fileout,"\tRemarks :\t\tThis file should not be modified by users ;\n");
	fprintf(h_fileout,"\t\t\t\tFile auto - generated from dderrors.h descriptions.\n");
	fprintf(h_fileout,"*/\n");

	/* PRINT GNU-GPL header */
	fprintf(h_fileout,"\n\n");
	fprintf(h_fileout,"/*\n");
	fprintf(h_fileout,"This file is part of Fec Software project.\n\n");
	fprintf(h_fileout,"Fec Software is free software; you can redistribute it and/or modify\n");
	fprintf(h_fileout,"it under the terms of the GNU General Public License as published by\n");
	fprintf(h_fileout,"the Free Software Foundation; either version 2 of the License, or\n");
	fprintf(h_fileout,"(at your option) any later version.\n\n");
	fprintf(h_fileout,"Fec Software is distributed in the hope that it will be useful,\n");
	fprintf(h_fileout,"but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	fprintf(h_fileout,"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	fprintf(h_fileout,"GNU General Public License for more details.\n\n");
	fprintf(h_fileout,"You should have received a copy of the GNU General Public License\n");
	fprintf(h_fileout,"along with Fec Software; if not, write to the Free Software\n");
	fprintf(h_fileout,"Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\n");
	fprintf(h_fileout,"Copyright 2002 - 2005, Laurent GROSS - IReS/IN2P3\n");
	fprintf(h_fileout,"*/\n");


	/* Print conditional headers */
	fprintf(h_fileout,"\n\n");
	fprintf(h_fileout,"/*!\n");
	fprintf(h_fileout,"Begin Block - avoid multiple inclusions of this file\n");
	fprintf(h_fileout,"*/\n");
	fprintf(h_fileout,"#ifndef DD_PERROR_H\n");
	fprintf(h_fileout,"#define DD_PERROR_H\n");

	fprintf(h_fileout,"\n\n");

	fprintf(h_fileout,"#ifdef __cplusplus\n");
	fprintf(h_fileout,"extern \"C\" {\n");
	fprintf(h_fileout,"#endif\n");

	/* PRINT FIRST FUNCTION HEADER */
	fprintf(h_fileout,"\n\n");
	fprintf(h_fileout,"/*!\n");
	fprintf(h_fileout,"<b>FUNCTION : dd_get_error_msg</b>\n");
	fprintf(h_fileout,"- Job\n");
	fprintf(h_fileout,"\t- Link an error code to the appropriate error message.\n");
	fprintf(h_fileout,"\tThis function is the public interface for users.\n");
	fprintf(h_fileout,"- I/O\n");
	fprintf(h_fileout,"\t- Inputs\n");
	fprintf(h_fileout,"\t\t- DD_TYPE_ERROR code : Error code to analyse\n");
	fprintf(h_fileout,"\t- Outputs\n");
	fprintf(h_fileout,"\t\t- char *msg : string pointer in which the error message will be stored.\n");
	fprintf(h_fileout,"\t\t- Error code returned by the function\n");
	fprintf(h_fileout,"- Internals\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Global variables accessed\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Notes\n");
	fprintf(h_fileout,"\t- the string pointer *msg given as THIRD argument should have a minimal\n");
	fprintf(h_fileout,"\tlength of DD_MAX_DECODED_ERROR_MSG_LENGTH characters ; an exact length would be better.\n");
	fprintf(h_fileout,"\t- DD_TYPE_ERROR is currently defined as int\n");
	fprintf(h_fileout,"- Error Management & Values returned\n");
	fprintf(h_fileout,"\t- Operation successful :\n");
	fprintf(h_fileout,"\t\t- DD_RETURN_OK\n");
	fprintf(h_fileout,"\t- Errors :\n");
	fprintf(h_fileout,"\t\t- void\n");
	fprintf(h_fileout,"- Sub-functions calls :\n");
	fprintf(h_fileout,"\t- snprintf	(system)\n");
	fprintf(h_fileout,"*/\n");
	/* Print function1 body */
	fprintf(h_fileout,"DD_TYPE_ERROR dd_get_error_msg ( DD_TYPE_ERROR code, char *msg);\n");




	/* PRINT SECOND FUNCTION HEADER */
	fprintf(h_fileout,"\n\n");
	fprintf(h_fileout,"/*!\n");
	fprintf(h_fileout,"<b>FUNCTION : dd_print_error_msg</b>\n");
	fprintf(h_fileout,"- Job\n");
	fprintf(h_fileout,"\t- Link an error code to the appropriate error message, and\n");
	fprintf(h_fileout,"\tprints it to the standard output for errors (stderr).\n");
	fprintf(h_fileout,"- I/O\n");
	fprintf(h_fileout,"\t- Inputs\n");
	fprintf(h_fileout,"\t\t- DD_TYPE_ERROR code : Error code to analyse\n");
	fprintf(h_fileout,"\t- Outputs\n");
	fprintf(h_fileout,"\t\t- Error code returned by the function\n");
	fprintf(h_fileout,"- Internals\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Global variables accessed\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Notes\n");
	fprintf(h_fileout,"\t- DD_TYPE_ERROR is currently defined as int\n");
	fprintf(h_fileout,"- Error Management & Values returned\n");
	fprintf(h_fileout,"\t- Operation successful :\n");
	fprintf(h_fileout,"\t\t- DD_RETURN_OK\n");
	fprintf(h_fileout,"\t- Errors :\n");
	fprintf(h_fileout,"\t\t- void\n");
	fprintf(h_fileout,"- Sub-functions calls :\n");
	fprintf(h_fileout,"\t- fprintf	(system)\n");
	fprintf(h_fileout,"*/\n");
	/* Print function1 body */
	fprintf(h_fileout,"DD_TYPE_ERROR dd_print_error_msg ( DD_TYPE_ERROR code);\n");



	/* PRINT THIRD FUNCTION HEADER */
	fprintf(h_fileout,"\n\n");
	fprintf(h_fileout,"/*!\n");
	fprintf(h_fileout,"<b>FUNCTION : dd_get_error_msg</b>\n");
	fprintf(h_fileout,"- Job\n");
	fprintf(h_fileout,"\t- Link an error code to the appropriate error message.\n");
	fprintf(h_fileout,"\tThis function should be a private function, and should not be accessed\n");
	fprintf(h_fileout,"\tdirectly, but is nevertheless exported via dd_perror.h file for now.\n");
	fprintf(h_fileout,"- I/O\n");
	fprintf(h_fileout,"\t- Inputs\n");
	fprintf(h_fileout,"\t\t- DD_TYPE_ERROR param_code : Error code to analyse\n");
	fprintf(h_fileout,"\t- Outputs\n");
	fprintf(h_fileout,"\t- char *param_errormessage : string pointer in which the error message will be stored.\n");
	fprintf(h_fileout,"\t\t- Error code returned by the function\n");
	fprintf(h_fileout,"- Internals\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Global variables accessed\n");
	fprintf(h_fileout,"\t- Void\n");
	fprintf(h_fileout,"- Notes\n");
	fprintf(h_fileout,"\t- the string pointer *msg given as THIRD argument should have a minimal\n");
	fprintf(h_fileout,"\tlength of DD_MAX_DECODED_ERROR_MSG_LENGTH characters ; an exact length would be better.\n");
	fprintf(h_fileout,"\t- DD_TYPE_ERROR is currently defined as int\n");
	fprintf(h_fileout,"- Error Management & Values returned\n");
	fprintf(h_fileout,"\t- Operation successful :\n");
	fprintf(h_fileout,"\t\t- DD_RETURN_OK\n");
	fprintf(h_fileout,"\t- Errors :\n");
	fprintf(h_fileout,"\t\t- void\n");
	fprintf(h_fileout,"- Sub-functions calls :\n");
	fprintf(h_fileout,"\t- snprintf	(system)\n");
	fprintf(h_fileout,"*/\n");
	/* Print function2 body */
	fprintf(h_fileout,"DD_TYPE_ERROR dd_decode_error ( DD_TYPE_ERROR code, char *msg);\n");
	fprintf(h_fileout,"\n\n");


	/* PRINT end of include file */
	fprintf(h_fileout,"/*!\n");
	fprintf(h_fileout,"End Block - avoid multiple inclusions of this file\n");
	fprintf(h_fileout,"*/\n");
	fprintf(h_fileout,"#ifdef __cplusplus\n");
	fprintf(h_fileout,"}\n");
	fprintf(h_fileout,"#endif\n");
	fprintf(h_fileout,"#endif\n");
}
