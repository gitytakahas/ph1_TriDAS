/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#ifndef VFATDEFINITIONS_H
#define VFATDEFINITIONS_H

//#define DEBUG_VFAT_CONTROLLER

#define SET_FORECOLOR_RED   "\033[31m"
#define SET_FORECOLOR_GREEN "\033[32m"
#define SET_FORECOLOR_BLUE  "\033[34m"
#define SET_FORECOLOR_CYAN  "\033[36m"
#define SET_DEFAULT_COLORS  "\033[0m"

#define WINDOW_W 962
#define WINDOW_H 852
#define THREAD_WAIT_TIME 1000
#define VFAT_I2C_RETRIES 5

#define VFAT_CH_NUM_MAX       128
#define VFAT_DAC_NUM_MAX      8
#define VFAT_CALPHASE_NUM_MAX 7
#define VFAT_REG_VALUE_MAX    255
#define CTS_CONVERTION_FACTOR 4.63  // -> uA
#define BE_CONVERTION_FACTOR  46.23 // -> nA

#define VFAT_PATH_MONITOR_CONFIG "work/results/MonitorCfg.xml"
#define VFAT_PATH_SUMMARY "work/results/results.txt"
#define VFAT_PATH_CONFIG "work/results/runConfigurationInfo.xml"
#define VFAT_PATH_I2CREGS "i2cRegisters.txt"
#define CHIPID_NONSENCE "XxNoChipId"
#define NOVFAT "NoChipId"

#define VFAT_RESET_OPTORX_COMMAND "resetOptoRx"
#define VFAT_TRIGGER_COMMAND "sudo $ENV_CMS_TK_FEC_ROOT/ThirdParty/Totem/vfat_controller/commands/writePrinter"
#define VFAT_CAEN_DRIVER_RELOAD "sudo /sbin/service caenvme restart"
#define FEC_SSID "TOTEM"

//Control And Test Panels
#define VFAT_TRIMDAC_MAX 32

//testVfatI2C
#define VFAT_I2CTEST_VALUE 0x55

//powerOnVfat
#define VFAT_POWERON_MASK 0x01

//positionMappingVfat
#define VFAT_PATH_POS_MAPPING "work/results/treasureHunt.xml"
#define VFAT_POWERON_INVERTED_MASK 0xfe

//testVfatDAC
#define VFAT_PATH_DAC_PREFIX "work/results/vfat_"
#define VFAT_PATH_DAC_POSIX "_DACs.txt"
#define VFAT_PATH_DAC_RP_HYBRID "work/results/RP_hybrid_current_info.txt"

#define VFAT_CALMODE_NORMAL_MASK 0x00
#define VFAT_CALMODE_VHI_MASK    0x40
#define VFAT_CALMODE_VLOW_MASK   0x80

#define VFAT_CALMODE_INVERTED_MASK 0x3f

// T1 and T2
#define DCU_I_CH   0x02
#define DCU_V_CH   0x03
#define DCU_T_CH   0x07

// RP
#define DCU_RP_BE_CH  0x02
#define DCU_RP_CTS_CH 0x03
#define DCU_RP_I_CH   0x04
#define DCU_RP_V_CH   0x05

//pulseOneChannelVfat
#define VFAT_PATH_PULSE_CH "work/results/pulseScan.xml"
#define VFAT_PATH_PULSE_CH_DATA "work/results/pulseScan.dat"

#define VFAT_CALPULSE_MASK 0x40

#define VFAT_TRIMDAC_MASK 0x1f

#define VFAT_CALPULSE_MASKCH_MASK 0x60

#define VFAT_MASKCH_MASK 0x20
#define VFAT_INVERTED_MASKCH_MASK 0xbf

#define VFAT_PULSE_FLAG 1

//checkDeadChannelsVfat
#define VFAT_PATH_DEAD_CH "work/results/deadCheck.xml"
#define VFAT_VCAL_TEST_VALUE 0x80

//thresholdMeasurementVfat
#define VFAT_PATH_THRESHOLD "work/results/thresholdScan.xml"

//latencyScanVfat
#define VFAT_PATH_LATENCY "work/results/latencyScan.xml"

#define VFAT_DEFAULT_CH 64

#define VFAT_CALPHASE_0   0x00
#define VFAT_CALPHASE_45  0x01
#define VFAT_CALPHASE_90  0x03
#define VFAT_CALPHASE_135 0x07
#define VFAT_CALPHASE_180 0x0f
#define VFAT_CALPHASE_225 0x1f
#define VFAT_CALPHASE_270 0x3f
#define VFAT_CALPHASE_315 0x7f

// DCU ADDRESSES
#define VFAT_DCU_RP_HYBRID 0x50
#define VFAT_DCU_TBB_1     0x00
#define VFAT_DCU_TBB_2     0xff

// TTCci Settings
#define MY_TTCCI_ADAPTER "CAENPCI"
#define MY_TTCCI_SLOT    4
#define MY_TTCCI_RSYNC   "RSync"

// TOTFED Settings
#define TOTFED_ADDR_TABLE "config/TOTFEDAddressTable.dat"
#define TOTFED_SLOT 9 //  0x48000000

#define TOTFED_POLL_REF 0
#define TOTFED_POLL_TIMEOUT_MS 1500
#define TOTFED_POLL_TIMEOUT_EXT_MS 600000 // 10 min
#define TOTFED_EVENT_SIZE_WORDS 1164
#define TOTFED_READOUT_BUFFER_SIZE 6200 // 1 EVENT = 4656 bytes, FIFOs' size = 6144 bytes (256*6*4)
#define TOTFED_EMPTY_FIFO 0xA0010000

#define TOTFED_READOUT_ERROR 1000
#define TOTFED_HEADER 0xB0000000

// Paths for normal scanning with VME
#define VME_DATA_POS_MAPPING "work/results/treasureHunt.vme"
#define VME_DATA_PULSESCAN "work/results/pulseScan.vme"
#define VME_DATA_DEADCHECK "work/results/deadCheck.vme"
#define VME_DATA_THRESHOLD "work/results/thresholdScan.vme"
#define VME_DATA_LATENCY "work/results/latencyScan.vme"

// Data with external trigger
#define VME_DATA "work/results/vme_outfile.vme"


// RP PVSS NAMING

#define SET_PVSS_DATAPOINT ". $XDAQ_ROOT/bin/dpSet.sh "
#define PSX_SERVER " http://$HOSTNAME:1972/pvss"

#define CCU_RP_1  0x6f
#define CCU_RP_2  0x7f
#define CCU_RP_3  0x7e
#define CCU_RP_4  0x7d
#define CCU_RP_5  0x7c
#define CCU_RP_6  0x7b
#define CCU_RP_7  0x7a
#define CCU_RP_8  0x6e
#define CCU_RP_9  0x6d
#define CCU_RP_10 0x6c
#define CCU_RP_11 0x6b
#define CCU_RP_12 0x6a

#define PVSS_NODE "totRpSv:"
#define PVSS_DP_PREFIX "XDAQ/DCU/"

#define PVSS_DP_START_RP2 "tot_Rp_56_220_fr_hr_"
#define PVSS_DP_START_RP3 "tot_Rp_56_220_nr_bt_"
#define PVSS_DP_START_RP4 "tot_Rp_56_220_nr_tp_"

#define PVSS_DP_START_RP5 "tot_Rp_56_220_nr_tp_"
#define PVSS_DP_START_RP6 "tot_Rp_56_220_nr_bt_"
#define PVSS_DP_START_RP7 "tot_Rp_56_220_nr_hr_"

#define PVSS_DP_START_RP1 "tot_Rp_45_220_fr_hr_"
//#define PVSS_DP_START_RP8 "tot_Rp_45_220_fr_bt_"
//#define PVSS_DP_START_RP9 "tot_Rp_45_220_fr_tp_"

//#define PVSS_DP_START_RP10 "tot_Rp_45_220_nr_hr_"
//#define PVSS_DP_START_RP11 "tot_Rp_45_220_nr_bt_"
//#define PVSS_DP_START_RP12 "tot_Rp_45_220_nr_tp_"

#define PVSS_DP_TEMP_VALUE "_Temp.rawvalue:_original.._value "
#define PVSS_DP_BE_VALUE "_CurrBe.rawvalue:_original.._value "
#define PVSS_DP_CTS_VALUE "_CurrCts.rawvalue:_original.._value "

#define PVSS_DP_DUMMY_VALUE "\"psx_15:_mp_SETPOINT.value:_original.._value\" "


#endif
