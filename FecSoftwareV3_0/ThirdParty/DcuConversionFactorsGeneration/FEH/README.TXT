Documentation
--------------

FEH program:
  1. Retreive all hybrids from construction database
	select distinct OBJECT_ID from OBJECT_ASSEMBLY where object=\'HYB\' and OBJECT_ID is not null order by object_id

  2. For each hybrid and depending of the parameter given in the command line (RECEPTION or PRODUCTION)

        select ET_DCU_ID, DCU_ID, DCU_CAL_HIGHINPUT_MV, DCU_CAL_HIGH_VNOM, DCU_FHIT_MICROVPADC, DCU_0_RAW, DCU_1_RAW, DCU_2_RAW, DCU_3_RAW, DCU_4_RAW, DCU_7_RAW, FHIT_VREF_IN_MV, ET_V250_IN_MV, TOOL_ID, TEST_ID, INPUT_ID, FHIT_NUMBER, FHIT_FIRMWARE_VERSION, FHIT_BOARD_VERSION, DCU_6_RAW from " + tableName + " where OBJECT_ID=" + HybridId + " AND status='reference'</select>" ;


How to get the data
-------------------
1) Run the clientFEH both for reception and production
   type clientFEH -help

2) The execution of the following perl script produces the file AllDcus_

perl makexml2.pl PRODFILE RECFILE DATE

By default (defined inside makexml2.pl):

PRODFILE = AllDcuProduction_190406.txt 
RECFILE = AllDcuReception_190406.txt
FILEOUT = AllDcus_210406.txt

3) The execution of the following commands remove from the file the warnings

grep -v "not ok" AllDcus_210406.txt > AllDcus_210406.xml

4) Edit AllDcus_210406.xml and remove the fourth line 
