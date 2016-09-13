/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

// Flags for debugging

//#define USB_GPIB_PS_NO_POWER_ON_OFF

#define CHIPTEST_VERSION 1.11

// Chip test related paths
#define CHIPTEST_VFATLIST "vfats_tested.txt"
#define CHIPTEST_SUMMARY "results.txt"
#define CHIPTEST_DATA "work/results/vfat_data/"
#define CHIPTEST_DEFAULT_SETTINGS "work/configurations/ChipTesting/chipTesting_defaults.xml"
#define CHIPTEST_SETTINGS_MASK "work/configurations/ChipTesting/chipTesting_defaults_mask.xml"

#define CHIPTEST_DATAPACKET_REF "work/results/vfat_data/DataPacketCheckRef.dat"
#define DATAPACKET_COMMAND "TTPDataPacketTest"
#define CHIPTEST_PARAMFILE "work/results/vfat_data/TTPTestConfig.txt"
#define CHIPTEST_MONITOR_CONFIG "MonitorCfg.xml"
#define MONITORCALL_MASK "commands/RunMaskCheck"
#define MONITORCALL_ANALYSIS "commands/RunAnalysis"
#define TESTMANAGERCALL "TestManager an -a $HOSTNAME -d "

#define ERRORDUMP_FIFOS "fifos.error.log"
#define ERRORDUMP_FRAMES "frames.error.log"

#define TTP_CONFIG_PULSESCAN "PulseScanLowRes.xml"
#define TTP_DATA_PULSESCAN "PulseScanLowRes.dat"

#define TTP_CONFIG_DATAPACKET "DataPacketCheck.xml"
#define TTP_DATA_DATAPACKET "DataPacketCheck.dat"

#define TTP_CONFIG_TREASURE "TreasureHunt.xml"
#define TTP_DATA_TREASURE "TreasureHunt.dat"

#define TTP_CONFIG_POSPOLARITY "BinaryCheckPos.xml"
#define TTP_DATA_POSPOLARITY "BinaryCheckPos.dat"

#define TTP_CONFIG_HIRESPULSESCAN "PulseScanHiRes.xml"
#define TTP_DATA_HIRESPULSESCAN "PulseScanHiRes.dat"

#define TTP_CONFIG_NEGPOLARITY "BinaryCheckNeg.xml"
#define TTP_DATA_NEGPOLARITY "BinaryCheckNeg.dat"

#define TTP_CONFIG_MASKCHECK "MaskCheck.xml"
#define TTP_DATA_MASKCHECK "MaskCheck.dat"

#define TTP_CONFIG_MSCHECK "MSCheck.xml"
#define TTP_DATA_MSCHECK "MSCheck.dat"

#define TTP_SBIT_INFO "sbit_info.txt"
#define TTP_DAC_CHECK "DACCheck.txt"
#define TTP_SUMMARY "Summary.txt"


// Paths for normal scanning
#define TTP_DATA_DEADCHECK "work/results/deadCheck.dat"
#define TTP_DATA_THRESHOLD "work/results/thresholdScan.dat"
#define TTP_DATA_LATENCY "work/results/latencyScan.dat"
#define TTP_DATA_FILE "work/results/ttp_outfile.dat"


// Error messages
#define CREPE_WRITE_ERROR "Problem while writing registers"
#define CREPE_READ_ERROR "Problem while reading registers"
#define I2C_ERROR "I2C PROBLEM"

#define DATAPACKET_WRONG_PARAM_NUM   1
#define DATAPACKET_WRONG_ID          2
#define DATAPACKET_SUMMARYFILE_ERROR 3
#define DATAPACKET_DATAFILE_ERROR    101
#define DATAPACKET_EVENT_ERROR       102
#define DATAPACKET_WRONG_EVENT_NUM   103
#define DATAPACKET_OK                100
#define DATAPACKET_FATAL             110

// Misc constants
#define CREPE_HEADER "TTP001"
#define MONITOR_VERBOSE 255

#define RESETPATTERNLENGTH 2

#define VFATPACKETLENGTH 12
#define MINIPACKETLENGTH 4

#define USBDEVCOUNT 1
#define CRCOUNT 9 // 10
#define WAITTIME_MSEC 1
#define TTP_CONSUMPTION 1.62 //1.59

#define DCU_RPHYBRID_CH 0x12
#define DCU_TTP_CH      0x10


// Crepe registers
#define CREPE_BASE_ADDR  0x0400
#define TRIGPATTERN_BASE 0x0000

#define CREPE_FIFO1   0x0420
#define CREPE_FIFO2   0x0430
#define CREPE_FIFO3   0x0440
#define CREPE_FIFO4   0x0450
#define CREPE_EVFIFO  0x0460
#define CREPE_BCFIFO  0x0470
#define CREPE_SFIFO   0x0480
#define CREPE_SBCFIFO 0x0490


// Crepe settings
#define CREPE_CONTROL1    0x00001000 // 1004
#define CREPE_CONTROL2    0x06030000 // 0x00000000
#define CREPE_CONTROL3    0x08000020 // 0x0800003E, 0x08000020
#define CREPE_FIRMWARE    0x00000000
#define CREPE_STATUS      0x00000000
#define CREPE_LAT         0x000000E6 // FF
#define CREPE_TRIGENABLE  0x00000FFF // FFF, F09 for sbits
#define CREPE_TRIGDISABLE 0x00000001 // 1, FF
#define CREPE_BC0         0x0000F000

// Settings for data packet check
#define CREPE_CONTROL1_OPT1   0x00001200
#define CREPE_CONTROL3_OPT1   0x08100020 // 0x0810003e
#define CREPE_LAT_OPT1        0x000003F0
#define CREPE_TRIGENABLE_OPT1 0x0000F000


// Trigger settings
#define TRIGPATTERNLENGTH_S  81   // 6
#define TRIGPATTERNLENGTH_L  201

#define SPACING_CAL   0x00000030
#define SPACING_NOCAL 0x00000005


// Other Crepe constants
#define CREPE_GEMDATA_MASK   0xf0ffffff
#define CREPE_RPDATA_MASK    0x08000000
#define CREPE_MINIFRAME_MASK 0x00100000


// Parameters for chip testing, used if parameter file missing
#define CHIPTEST_VCAL1_HI    10
#define CHIPTEST_VCAL2_HI    30
#define CHIPTEST_VCALSTEP_HI 1
#define CHIPTEST_VTHRESHOLD1 10
#define CHIPTEST_VTHRESHOLD2 0

#define CHIPTEST_VCAL1    40
#define CHIPTEST_VCAL2    80
#define CHIPTEST_VCALSTEP 1

#define CHIPTEST_SBITPASSLIMIT 100
#define CHIPTEST_SBITEXPECTEDHITS 255 // 85 per step, each sector gets hit 3 times
#define CHIPTEST_SBITMAXHITS 765 // 255 * 3, FIFO entries * valid steps (ones checked per sector)


#define CHIPTEST_MS_MAX  0x70
#define CHIPTEST_MS_STEP 0x10

#define CHIPTEST_PARALLEL_CH 16


// GBIP definitions
#ifdef USB_GPIB_PS

// GPIB LV power supply 
#define GPIB_BOARD_INDEX 0
#define GPIB_PRIMARY_ADDR 16  // tti address
#define GPIB_SECONDARY_ADDR 0
#define GPIB_TIMEOUT T10s
#define GPIB_LAST_BYTE_MODE 1
#define GPIB_END_STRING_MODE 0

#define GPIB_VOLTAGE 5.5
#define GPIB_CURRENT 3.0

// GPIB HV power supply
#define GPIB_HV_PRIMARY_ADDR 24  // tti address

#define GPIB_HV_VOLTAGE 150
#define GPIB_HV_CURRENT 1e-3
#define GPIB_HV_VOLTAGE_STEP 10
#define GPIB_HV_DELAY 1000

#endif


