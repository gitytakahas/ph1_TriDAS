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
#ifndef DOHACCESS_H
#define DOHACCESS_H

#include "FecAccess.h"

#include "laserdriverAccess.h"
#include "laserdriverDescription.h"

// Francois Vasey <Francois.Vasey@cern.ch>
// Wed, 17 Sep 2003 14:50:48 +0100
// 1. Les laser drivers sur les AOH (FED) et DOH (FEC) sont identiques a deux
// exceptions pres:
//   A)Adresse AOH: Hex 60
//             DOH: Hex 70
// B)Courrant laser apres power up ou reset(DRES) AOH : Dec 00
//                                                DOH : Dec 48
// Ceci signifie que le laser de readout est eteint apres un power up ou
// reset, alors que le laser de controle a un courrant par defaut d'environ 
// 22mA.  
// Ceci guaranti que l'anneau de controle est immediatement operationel sans 
// devoir telecharger des parametres nominaux.
//
// 2. Bien que l'anneau fonctionne parfaitement des le startup, on pourrait 
// facilement choisir un courrant nominal plus bas pour epargner le laser et 
// minimiser la dissipation de puissance. Le gain peut rester inchange.  
// Nous n'avons pas vraiment etudie ce point en detail, mais une valeur de 
// courant de l'ordre de Dec 24 devrait etre optimale.
// Dans le passe, Christian a observe des interruptions de l'anneau de control 
// lorsque la puissance optique est variee brusquement. Ceci est du au systeme 
// d'ajustement automatique du gain sur le TRx.  Je ne pense pas que ce 
// probleme soit visible pour un passage de 48 a 24, mais je te propose pour 
// commencer d'approcher la valeur de 24 en 4 etapes sans pauses 
// intermediaires: 48, 40, 32, 24.

// STEP by STEP
#define DOHSTEP 1
#define MINBIASVALUE 10

// ADDRESS of the DOH
#define DOHI2CADDRESS 0x70

/**
 * \class DohAccess
 * This class defined a doh and make possible the downloading or
 * uploading of the values related to a specific register.
 * It also implements all the method specific to the DOH device.
 * \author Frederic Drouhin
 * \date April 2002
 * \warning all the I2C accesses are done in normal mode but the address is used
 * with an offset and a shift on it for the old doh. The new one does
 * not use anymore this specification.
 * \brief This class define all the hardware access for the doh througth the FecAccess class
 */
class DohAccess: public laserdriverAccess {

 public:

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  DohAccess ( FecAccess *fec, 
	      int Partition_id, int Module_id,
	      int Version_major_id,
	      int Version_minor_id,
	      int DohId,
	      tscType16 Fec_slot,
	      tscType16 Ring_slot,
	      tscType16 Ccu_slot,
	      tscType16 channelId,
	      tscType16 I2C_address ) ;
  
  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  DohAccess (FecAccess *fec, 
	     keyType key ) ;

  /** \brief Call the deviceAccess constructor, in order to build
   * an access to an I2C channel
   */
  DohAccess (FecAccess *fec, 
	     tscType16 Fec_slot,
	     tscType16 Ring_slot,
	     tscType16 Ccu_slot,
	     tscType16 channelId,
	     tscType16 I2C_address ) ;

  /** \brief Remove the access
   */
  ~DohAccess () ;

  /** \brief Get all values for a doh
   */
  laserdriverDescription *getValues ( ) ;

  /** \brief Set all values for a doh
   */
  void setValues ( laserdriverDescription dohValues, bool realValues = false ); 

  /** \brief Set the value in the specified register
   */
  void setBias ( tscType8 *bias ) ;

  /** \brief Find the optimal value for the DOH bias for a given gain
   */
  laserdriverDescription *calibrateDOH ( bool displayDebugMessage = false, unsigned char gain = 1, unsigned char minBiasValue = MINBIASVALUE ) throw (FecExceptionHandler) ;
};

#endif
