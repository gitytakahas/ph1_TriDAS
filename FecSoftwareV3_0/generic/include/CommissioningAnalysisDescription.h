
#ifndef COMMISSIONINGANALYSISDESCRIPTION_H
#define COMMISSIONINGANALYSISDESCRIPTION_H


#include "CommissioningAnalysis.h"
#include "ParameterDescription.h"


/**
   @class CommissioningAnalysisDescription
   @author Sebastien BEL
   @brief  Base class for all commissioning analysis classes
*/
class CommissioningAnalysisDescription {


private:

  // properties:

	/* @brief FEC crate [0,1-4,invalid]. */
	uint16_t   _crate;
	/* @brief FEC slot [0,2-21,invalid]. */
	uint16_t   _slot;
	/* @brief FEC ring [0,1-8,invalid]. */
	uint16_t   _ring;
	/* @brief CCU module [0,1-127,invalid]. */
	uint16_t   _ccuAdr;
	/* @brief FE module [0,16-31,invalid]. */
	uint16_t   _ccuChan;
	/* @brief LLD channel [0,1-3,invalid]. */
	uint16_t   _lldChan;
	/* @brief APV I2C address [0,32-37,invalid]. */
	uint16_t   _i2cAddr;
	/* @brief FED id [0,50-489,invalid]. */
	uint16_t   _fedId;
	/* @brief FE unit [0,1-8,invalid]. */
	uint16_t   _feUnit;
	/* @brief FE channel [0,1-12,invalid]. */
	uint16_t   _feChan;
	/* @brief APV [0,1-2,invalid] */
	uint16_t   _fedApv;
	/* @brief Partition name. */
	c_string _partition;
	/* @brief Run number (0 is invalid) */
	uint32_t   _runNumber;
	/* @brief Indicates whether description is valid or not. */
	bool       _valid;
	/* @brief Comments related to encountered problems during analysis */
	std::vector<std::string> _comments;
	/* @brief Geometric detector identifier */
	uint32_t   _detId;
	/* @brief Dcu Hardware identifier */
	uint32_t   _dcuHardId;



public:

	/** @brief Enum of parameter names */
	enum {CRATE,SLOT,RING,CCUADR,CCUCHAN,I2CADDR,PARTITION,RUNNUMBER,ANALYSISTYPE,VALID,COMMENTS,FEDID,FEUNIT,FECHAN,FEDAPV,DCUHARDID,DETID,ENDOFNAMEENUM};

	/** @brief Type of commissioning analysis */
	typedef enum { T_UNKNOWN,T_ANALYSIS_APVLATENCY,T_ANALYSIS_CALIBRATION,T_ANALYSIS_FASTFEDCABLING,
		T_ANALYSIS_FINEDELAY,T_ANALYSIS_OPTOSCAN,T_ANALYSIS_PEDESTALS,T_ANALYSIS_TIMING,T_ANALYSIS_VPSPSCAN} commissioningType;

	/** @brief map between parameter enum and user readable analysis type */
	static const char *COMMISSIONINGANALYSISTYPE[T_ANALYSIS_VPSPSCAN+1];

	/** @brief List of parameter names */
	static const char *COMMISSIONINGANALYSIS[ENDOFNAMEENUM];


  // (de)constructors:

	/** @brief default constructor */
	CommissioningAnalysisDescription();

	/** 
	 * @brief base class / upload contructor 
	 * @param crate - device FEC crate
	 * @param slot - device FEC slot
	 * @param ring - device FEC ring
	 * @param ccuAdr - device CCU Module
	 * @param ccuChan - device FE Module
	 * @param i2cAddr - device APV I2C Address
	 * @param partition - analysis partition
	 * @param runNumber - analysis runNumber
	 * @param valid - analysis description is valid (true) or not (false)
	 * @param comments - concatened comments about encountered errors or warnings
	 * @param fedId - FED id [0,50-489,invalid]
	 * @param feUnit - FE unit [0,1-8,invalid]
	 * @param feChan - FE channel [0,1-12,invalid]
	 * @param fedApv - APV [0,1-2,invalid]
	 */
	CommissioningAnalysisDescription(uint16_t crate,uint16_t slot,uint16_t ring,uint16_t ccuAdr,uint16_t ccuChan,uint16_t i2cAddr,std::string partition,uint32_t runNumber,bool valid, std::string comments, uint16_t fedId,uint16_t feUnit, uint16_t feChan, uint16_t fedApv);

	/**
	 * Build a description based on a XML parsing (from file or database)
	 * @brief download constructor
 	 * @param parameterNames - parameter names extracted from XML
	 */
	CommissioningAnalysisDescription(parameterDescriptionNameType parameterNames);
	/** 
	 * @brief destructor 
	 */
	virtual ~CommissioningAnalysisDescription() { };



  // methods:

	/** @brief clone method */
	virtual CommissioningAnalysisDescription *clone() = 0 ;
	/** @brief display object content in standard output */
	virtual void display() = 0 ;
	/** @brief get the commissioning analysis type (see commissioningType) */
	virtual CommissioningAnalysisDescription::commissioningType getType() const { return T_UNKNOWN; };
	/** @brief human readable object content */
	virtual std::string toString() { return ""; }
	/** @brief get the list of parameter name (XML clob interface) */
 	static parameterDescriptionNameType *getParameterNames();
 	/** @brief remove the parameter list */
 	static void deleteParameterNames(parameterDescriptionNameType *parameterNames);



  // getters / setters:

	/** @brief set: crate */
	void setCrate( const uint16_t value );
	/** @brief get: crate */
	uint16_t getCrate() const;
	/** @brief set: partition name */
	void setPartition( const std::string value );
	/** @brief get: partition name */
	std::string getPartition() const;
	/** @brief set: commissioning run number */
	void setRunNumber( const uint32_t value );
	/** @brief get: commissioning run number */
	uint32_t getRunNumber() const;
	/** @brief set: slot */
	void setSlot( const uint16_t value );
	/** @brief get: slot */
	uint16_t getSlot() const;
	/** @brief set: ring */
	void setRing( const uint16_t value );
	/** @brief get: ring */
	uint16_t getRing() const;
	/** @brief set: ccu address */
	void setCcuAdr( const uint16_t value );
	/** @brief get: ccu address */
	uint16_t getCcuAdr() const;
	/** @brief set: ccu channel */
	void setCcuChan( const uint16_t value );
	/** @brief get: ccu channel */
	uint16_t getCcuChan() const;
	/** @brief set: I2C address */
	void setI2cAddr( const uint16_t value );
	/** @brief get: I2C address */
	uint16_t getI2cAddr() const;
	/** @brief get: human readable analysis type */
	static std::string getAnalysisType( const CommissioningAnalysisDescription::commissioningType type );
	/** @brief get: translate a human readable analysis type to an internal code */
	static CommissioningAnalysisDescription::commissioningType getAnalysisType( const std::string type );
	/** @brief add: comment related to encountered problems during analysis */
	void addComments( const std::string value );
	/** @brief get: comments related to encountered problems during analysis */
	std::vector<std::string> getComments() const;
	/** @brief get: string of serialized comments related to encountered problems during analysis */
	std::string getSerializedComments() const;
	/** 
	 * @brief set: comments related to encountered problems during analysis. 
	 * Set comments related to encountered problems during analysis. Input string is 
	 * a concatenation of all registered comments with a character separator defined by 
	 * CONCATENED_STRING_CHARACTER_SEPARATOR constant
	 */
	void setComments(const std::string value);
	/** @brief set: indicates whether description is valid or not. */
	void isValid( const bool value ) { this->_valid = value; }
	/** @brief get: indicates whether description is valid or not. */
	bool isValid() const { return this->_valid; }

	/** @brief get: FED hard ID */
	uint16_t getFedId() const { return this->_fedId; }
	/** @brief get: FE unit */
	uint16_t getFeUnit() const { return this->_feUnit; }
	/** @brief get: FE channel */
	uint16_t getFeChan() const { return this->_feChan; }
	/** @brief get: APV */
	uint16_t getFedApv() const { return this->_fedApv; }
	/** @brief get: Dcu hardware identifier */
	uint32_t getDcuHardId() const { return _dcuHardId; }
	/** @brief get: Geometric detector identifier (DetID) */
	uint32_t getDetId() const { return _detId; }


	/** @brief set: FED hardware ID */
	void setFedId(const uint16_t value) { this->_fedId=value; }
	/** @brief set: FE unit */
	void setFeUnit(const uint16_t value) { this->_feUnit=value; }
	/** @brief set: FED channel */
	void setFeChan(const uint16_t value) { this->_feChan=value; }
	/** @brief set: APV */
	void setFedApv(const uint16_t value) { this->_fedApv=value; }


private:
	/** @brief set: Dcu hardware identifier */
	void setDcuHardId(const uint32_t value) { this->_dcuHardId=value; }
	/** @brief set: Geometric detector identifier (DetID) */
	void setDetId(const uint32_t value) { this->_detId=value; }


protected:
	/** @brief human readable object content */
	std::string _toString();


};

#endif
