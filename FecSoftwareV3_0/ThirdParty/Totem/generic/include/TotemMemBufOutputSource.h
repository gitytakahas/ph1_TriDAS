
#ifndef TOTEMMEMBUFOUTPUTSOURCE_H
#define TOTEMMEMBUFOUTPUTSOURCE_H

#include "vfatDescription.h"
#include "totemCChipDescription.h"
#include "totemBBDescription.h"
#include "MemBufOutputSource.h"


/** \brief This class is implemented to generate a character buffer from a device vector.
 */
class TotemMemBufOutputSource : public MemBufOutputSource {

 protected:

  std::ostringstream memBufferVfat_, memBufferCChip_, memBufferTbb_ ;

 public:

  /** \brief Default constructor
   */
  TotemMemBufOutputSource();

  /** \brief Create a MemBufOuputSource object from a ccuVector
   */
  TotemMemBufOutputSource(ccuVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a TkRingDescription
   *  \param withCcus bool parameter default value is false
   */
  TotemMemBufOutputSource (TkRingDescription, bool forDb, bool withCcus ) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a vector of TkRingDescription
   *  \param withCcus bool parameter default value is false
   */
  TotemMemBufOutputSource (tkringVector rings, bool forDb, bool withCcus) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a deviceVector
   */
  TotemMemBufOutputSource(ConnectionVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a piaResetVector
   */
  TotemMemBufOutputSource(piaResetVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a dcuConversionVector
   */
  TotemMemBufOutputSource (dcuConversionVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a dcuConversionVector
   */
  TotemMemBufOutputSource (TkIdVsHostnameVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a vector of commissioning analysis
   */
  TotemMemBufOutputSource (std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType analysisType, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a tkDcuInfoVector
   */
  TotemMemBufOutputSource (tkDcuInfoVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a MemBufOuputSource object from a tkDcuPsuMapVector
   */
  TotemMemBufOutputSource (tkDcuPsuMapVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a TotemMemBufOutputSource object from a deviceVector
   */
  TotemMemBufOutputSource(deviceVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Create a TotemMemBufOutputSource object from a deviceVector
   */
  TotemMemBufOutputSource(deviceVector, piaResetVector, bool forDb=false) throw (FecExceptionHandler);

  /** \brief Destructor
   */
  ~TotemMemBufOutputSource();

  /**Write on <I>memBuffer_</I> attribute device information
   */
  void generateDeviceTag(deviceVector deviceParameters, bool forDb=false) throw (FecExceptionHandler);

  /**Generates a Vfat element
   */
  void generateXMLVfat(vfatDescription *, std::ostringstream &, bool forDb=false);

  /**Generates a CChip element
   */
  void generateXMLCChip(totemCChipDescription *, std::ostringstream &, bool forDb=false);

  /**Generates a Tbb element
   */
  void generateXMLTbb(totemBBDescription *, std::ostringstream &, bool forDb=false);

  /** \brief Return the generated Vfat XML buffer
   */
  std::ostringstream *getVfatOutputBuffer();

  /** \brief Return the generated CChip XML buffer
   */
  std::ostringstream *getCChipOutputBuffer();

  /** \brief Return the generated Tbb XML buffer
   */
  std::ostringstream *getTbbOutputBuffer();

} ;

#endif
