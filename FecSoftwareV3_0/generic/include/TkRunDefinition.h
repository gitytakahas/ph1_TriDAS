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

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#ifndef TKRUNDEFINITION_H
#define TKRUNDEFINITION_H
				
// List of name of parameter
			   enum{RUNNUMBER, RUNSTATENAME,RUNPARTITIONNAME,RUNMODE, RUNSTARTINGTIME, RUNENDINGTIME, RUNFECVERSIONMAJORID,RUNFECVERSIONMINORID,RUNFEDVERSIONMAJORID,RUNFEDVERSIONMINORID,RUNCONNECTIONVERSIONMAJORID,RUNCONNECTIONVERSIONMINORID,RUNDCUINFOVERSIONMAJORID,RUNDCUINFOVERSIONMINORID,RUNDCUPSUMAPVERSIONMAJORID,RUNDCUPSUMAPVERSIONMINORID,RUNO2O,RUNLOCAL,RUNCOMMENTS,RUNANALYSISVERSIONMAPID,RUNMASKVERSIONMAJORID,RUNMASKVERSIONMINORID};

static const char *TKRUNPARAMETERNAMES[] = {
  "runNumber",
  "stateName", 
  "partitionName", 
  "modeDescription",
  "startTime",
  "endTime",
  "fecVersionMajorId", 
  "fecVersionMinorId", 
  "fedVersionMajorId",
  "fedVersionMinorId",
  "connectionVersionMajorId",
  "connectionVersionMinorId",
  "dcuInfoVersionMajorId",
  "dcuInfoVersionMinorId",
  "dcuPsuMapVersionMajorId",
  "dcuPsuMapVersionMinorId",
  "o2o",
  "local",
  "comments",
  "analysisVersionMapPointerId",
  "maskVersionMajorId",
  "maskVersionMinorId"
} ;

#endif


