#!/usr/local/bin/perl

#Variables filea and fileb have the respectives names of each data file, change there if necesary.

if (!$ARGV[0]) { $filea="AllProductionDcus_190406.txt";} 
else { $filea=$ARGV[0];}

if (!$ARGV[1]) { $fileb="AllReceptionDcus_190406.txt";} 
else { $fileb=$ARGV[1];}

if (!$ARGV[2]) { $filec="AllDcus_210406.txt";} 
else { $filec="AllDcus_$ARGV[2].txt";}

open (PRODFILEOUT, ">AllProductionDcus_190406.txt.cmn");
open (RECFILEOUT, ">AllReceptionDcus_190406.txt.cmn");
 
open(FILEA,"<$filea");
open(FILEB,"<$fileb");

$buffer="";
$thr_gain = 0.043 ;
$thr_dcu6raw = 20 ;
$thr_i20 = 0.0010625 ;
$thr_kdiv = 0.01 ;
$thr_offsettscal = 100 ;
$thr_gain_low = 2.05 ;
$thr_gain_high = 2.25 ;
$thr_i20_low = 0.019 ;
$thr_i20_high= 0.023 ;
$thr_kdiv_low = 0.50 ;
$thr_kdiv_high = 0.56 ;
$thr_offsettscal_low = 2450 ;
$thr_offsettscal_high = 2750 ;


open (FILEOUT, ">$filec");
print FILEOUT "<?xml version = \"1.0\"?> \n" ;
print FILEOUT "<ROWSET xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation='http://cmsdoc.cern.ch/cms/cmt/System_aspects/FecControl/binaries/misc/conversionSchema.xsd'> \n" ;
print  FILEOUT "        <DCUCONVERTION dcuHardId = \"0\" subDetector = \"none\" dcuType = \"FEH\" structureId = \"none\" containerId = \"none\" adcGain0 = \"2.144\" adcOffset0 = \"0\" adcInl0 = \"0\" adcInl0OW = \"T\" adcCal0= \"F\" i20 = \"0.02122\" i10 = \"0.01061\" iCal = \"F\" kDiv = \"0.56\" kCal = \"F\" tsGain = \"8.9\" tsOffset =\"2432\" tsCal = \"F\" /> \n";

while ($lineb = <FILEB>) {
	
	@sep1=split ' ', $lineb;
	$hybid1=@sep1[0];
	$dcuid1=@sep1[1];
	$rdcuid1=@sep1[2];
	$gain1=@sep1[3];
	$offset1=@sep1[4];
	$inl1=@sep1[5];
	$inlow1=@sep1[6];
	$adc0cal1=@sep1[7];
	$i201=@sep1[8];
	$i101=@sep1[9];
	$kdiv1=@sep1[10];
	$thybcal1=@sep1[11];
	$offsettscal1=@sep1[12];
	$tdcucal1=@sep1[13];
	$thybtpa1=@sep1[14];
	$offsettstpa1=@sep1[15];
	$tdcutpa1=@sep1[16];
	$hybtemp1=@sep1[17];
	$dcuch41=@sep1[18];
	$dcuch71=@sep1[19];
	$dcu7raw1=@sep1[20];
	$dcu6raw1=@sep1[21];
	$vref1=@sep1[22];
	$v2501=@sep1[23];
	$frtestid1=@sep1[24];
	$frtoolid1=@sep1[25];
	$frinputid1=@sep1[26];
	$patestid1=@sep1[27];
	$patoolid1=@sep1[28];
	$painputid1=@sep1[29];
	$fbv1=@sep1[30];
	$ffv1=@sep1[31];
	$moduleid1=@sep1[32];
	$structureid1=@sep1[33];
	$detectorid1=@sep1[34];

	@sep2=split ' ', $buffer;
	$hybid2=@sep2[0];
	$dcuid2=@sep2[1];
	$rdcuid2=@sep2[2];
	$gain2=@sep2[3];
	$offset2=@sep2[4];
	$inl2=@sep2[5];
	$inlow2=@sep2[6];
	$adc0cal2=@sep2[7];
	$i202=@sep2[8];
	$i102=@sep2[9];
	$kdiv2=@sep2[10];
	$thybcal2=@sep2[11];
	$offsettscal2=@sep2[12];
	$tdcucal2=@sep2[13];
	$thybtpa2=@sep2[14];
	$offsettstpa2=@sep2[15];
	$tdcutpa2=@sep2[16];
	$hybtemp2=@sep2[17];
	$dcuch42=@sep2[18];
	$dcuch72=@sep2[19];
	$dcu7raw2=@sep2[20];
	$dcu6raw2=@sep2[21];
	$vref2=@sep2[22];
	$v2502=@sep2[23];
	$frtestid2=@sep2[24];
	$frtoolid2=@sep2[25];
	$frinputid2=@sep2[26];
	$patestid2=@sep2[27];
	$patoolid2=@sep2[28];
	$painputid2=@sep2[29];
	$fbv2=@sep2[30];
	$ffv2=@sep2[31];
	$moduleid2=@sep2[32];
	$structureid2=@sep2[33];
	$detectorid2=@sep2[34];


	if( $hybid1 eq $hybid2 ) {

                print PRODFILEOUT "$lineb" ;
                print RECFILEOUT "$linea" ;

      		$chk = 1;       

     	     	if( abs($dcu6raw1 - $dcu6raw2) < $thr_dcu6raw ) {       
			$chk_dcu6raw=1;
    		     	$dcu6raw = int (($dcu6raw1 + $dcu6raw2) / 2) ;
     	     	}
     	     	else {
     		     	$chk_dcu6raw = 0;       
     		     	$chk = 0;       
     	     	}

		if( abs($gain1 - $gain2) < $thr_gain ) {	
			$chk_gain = 1;	
			$gain = ($gain1 + $gain2) / 2 ;
		}
		else {
			$chk_gain = 0;	
			$chk = 0;	
		}

		if( ($inlow1 == "NOINLOW") && ($inlow2 == "NOINLOW") ){	
			$inlow = "T";	
			$inl = ($inl1 + $inl2) / 2;
		}
		else {
			if( $inlow1 == "NOINLOW") {	
				$inlow = "F";	
				$inl = $inl2;
			}
			else {
				if( $inlow2 == "NOINLOW") {	
					$inlow = "F";	
					$inl = $inl1;
				}
				else {
					$inlow = "F";	
					if ( $inl1 > $inl2 ) {
						$inl = $inl1;
					}
					else {
						$inl = $inl2;
					}
				}
			}
		}	

		if( abs($i201 - $i202) < $thr_i20 ) {
			$chk_i20 = 1;
			$i20 = ($i201 + $i202) / 2 ;
			$i10 = ($i101 + $i102) / 2 ;
		}
		else {
			$chk_i20 = 0;
			$chk = 0;
		}
		if( abs($kdiv1 - $kdiv2) < $thr_kdiv ) {
			$chk_kdiv = 1;
			$kdiv = ($kdiv1 + $kdiv2) / 2 ;
		}
		else {
			$chk_kdiv = 0;
			$chk = 0;
		}

 		if( abs($offsettscal1 - $offsettscal2) < $thr_offsettscal ) {
 			$chk_offsettscal = 1;
 			$offsettscal = int (($offsettscal1 + $offsettscal2) / 2) ;
 		}
 		else {
 			$chk_offsettscal = 0;
 			$chk = 0;
 		}

     	     	if( ($gain > $thr_gain_low) && ($gain < $thr_gain_high) ) {       
			$chk2_gain = 1;
		}
		else {
			$chk2_gain = 0;
     		     	$chk = 0;       
     	     	}

     	     	if( ($i20 > $thr_i20_low) && ($i20 < $thr_i20_high) ) {       
			$chk2_i20 = 1;
		}
		else {
			$chk2_i20 = 0;
     		     	$chk = 0;       
     	     	}

     	     	if( ($kdiv > $thr_kdiv_low) && ($kdiv < $thr_kdiv_high) ) {       
			$chk2_kdiv = 1;
		}
		else {
			$chk2_kdiv = 0;
     		     	$chk = 0;       
     	     	}

     	     	if( ($offsettscal > $thr_offsettscal_low) && ($offsettscal < $thr_offsettscal_high) ) {       
			$chk2_offsettscal = 1;
		}
		else { 
			$chk2_offsettscal = 0;
     		     	$chk = 0;       
     	     	}

		if( ($chk_dcu6raw == 1) && ($chk_gain == 1) && ($chk2_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
			$adc0cal = "T";
		}
		else {
			$adc0cal = "F";
			$gain = 2.144 ;
		}

		if( ($chk_dcu6raw == 1) && ($chk_i20 == 1) && ($chk2_i20 == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
			$ical = "T";
		}
		else {
			$ical = "F";
			$i20 = 0.02122 ;
			$i10 = 0.01061 ;
		}

		if( ($chk_dcu6raw == 1) && ($chk_kdiv == 1) && ($chk2_kdiv == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
			$kcal = "T";
		}
		else {
			$kcal = "F";
			$kdiv = 0.56 ;
		}

		if( ($chk_dcu6raw == 1) && ($chk_offsettscal == 1) && ($chk2_offsettscal == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
			$tscal = "T";
		}
		else {
			$offsettscal = "F";
			$offsettscal = 2432 ;
		}

 
		if ( $chk == 0 ) {
		print  FILEOUT  "$hybid1 not ok $adc0cal1 $inlow1 $adc0cal2 $inlow2 $chk_dcu6raw $chk_gain $chk2_gain $chk_i20 $chk2_i20 $chk_kdiv $chk2_kdiv $chk_offsettscal $chk2_offsettscal \n" ;
		print  FILEOUT "        <DCUCONVERTION dcuHardId = \"$rdcuid1\" subDetector = \"$detectorid1\" dcuType = \"FEH\" structureId = \"$structureid1\" containerId = \"$moduleid1\" adcGain0 = \"$gain\" adcOffset0 = \"0\" adcInl0 = \"$inl\" adcInl0OW = \"$inlow\" adcCal0 = \"$adc0cal\" i20 = \"$i20\" i10 = \"$i10\" iCal = \"T\" kDiv = \"$kdiv\" kCal = \"T\" tsGain = \"8.9\" tsOffset =\"$offsettscal\" tsCal = \"T\" /> \n";
		}
		else {
		print  FILEOUT "        <DCUCONVERTION dcuHardId = \"$rdcuid1\" subDetector = \"$detectorid1\" dcuType = \"FEH\" structureId = \"$structureid1\" containerId = \"$moduleid1\" adcGain0 = \"$gain\" adcOffset0 = \"0\" adcInl0 = \"$inl\" adcInl0OW = \"$inlow\" adcCal0 = \"$adc0cal\" i20 = \"$i20\" i10 = \"$i10\" iCal = \"T\" kDiv = \"$kdiv\" kCal = \"T\" tsGain = \"8.9\" tsOffset =\"$offsettscal\" tsCal = \"T\" /> \n";
		}

	}
	
	else { 
		while(($hybid2<=$hybid1) && ($linea = <FILEA>)) {
			$buffer=$linea;
			@sep2=split ' ', $linea;
			$hybid2=@sep2[0];
			$dcuid2=@sep2[1];
			$rdcuid2=@sep2[2];
			$gain2=@sep2[3];
			$offset2=@sep2[4];
			$inl2=@sep2[5];
			$inlow2=@sep2[6];
			$adc0cal2=@sep2[7];
			$i202=@sep2[8];
			$i102=@sep2[9];
			$kdiv2=@sep2[10];
			$thybcal2=@sep2[11];
			$offsettscal2=@sep2[12];
			$tdcucal2=@sep2[13];
			$thybtpa2=@sep2[14];
			$offsettstpa2=@sep2[15];
			$tdcutpa2=@sep2[16];
			$hybtemp2=@sep2[17];
			$dcuch42=@sep2[18];
			$dcuch72=@sep2[19];
			$dcu7raw2=@sep2[20];
			$dcu6raw2=@sep2[21];
			$vref2=@sep2[22];
			$v2502=@sep2[23];
			$frtestid2=@sep2[24];
			$frtoolid2=@sep2[25];
			$frinputid2=@sep2[26];
			$patestid2=@sep2[27];
			$patoolid2=@sep2[28];
			$painputid2=@sep2[29];
			$fbv2=@sep2[30];
			$ffv2=@sep2[31];
			$moduleid2=@sep2[32];
			$structureid2=@sep2[33];
			$detectorid2=@sep2[34];
			if( $hybid1 eq $hybid2 ) {

                                print PRODFILEOUT "$lineb" ;
                                print RECFILEOUT "$linea" ;
				
				$chk=1;

     	     			if( abs($dcu6raw1 - $dcu6raw2) < $thr_dcu6raw ) {       
     		     			$chk_dcu6raw = 1;       
     		     			$dcu6raw = int (($dcu6raw1 + $dcu6raw2) / 2) ;
     	     			}
     	     			else {
     		     			$chk_dcu6raw = 0;       
     		     			$chk = 0;       
     	     			}

				if( abs($gain1 - $gain2) < $thr_gain ) { 	
					$chk_gain = 1;	
					$gain = ($gain1 + $gain2) / 2 ;
					
				}
				else {
					$chk_gain = 0;	
					$chk = 0;	
				}

				if( ($inlow1 == "NOINLOW") && ($inlow2 == "NOINLOW") ){	
					$inlow = "T";	
					$inl = ($inl1 + $inl2) / 2;
				}
				else {
					if( $inlow1 == "NOINLOW") {	
						$inlow = "F";	
						$inl = $inl2;
					}
					else {
						if( $inlow2 == "NOINLOW") {	
							$inlow = "F";	
							$inl = $inl1;
						}
						else {
							$inlow = "F";	
							if ( $inl1 > $inl2 ) {
								$inl = $inl1;
							}
							else {
								$inl = $inl2;
							}
						}
					}
				}	

				if( abs($i201 - $i202) < $thr_i20 ) {
					$chk_i20 = 1;
					$i20 = ($i201 + $i202) / 2 ;
					$i10 = ($i101 + $i102) / 2 ;
				}
				else {
					$chk_i20 = 0;
					$chk = 0;
				}

				if( abs($kdiv1 - $kdiv2) < $thr_kdiv ) {
					$chk_kdiv = 1;
					$kdiv = ($kdiv1 + $kdiv2) / 2 ;
				}
				else {
					$chk_kdiv = 0;
					$chk = 0;
				}

 				if( abs($offsettscal1 - $offsettscal2) < $thr_offsettscal ) {
  					$chk_offsettscal = 1;
					$offsettscal = int (($offsettscal1 + $offsettscal2) / 2) ;
 				}
 				else {
 					$chk_offsettscal = 0;
 					$chk = 0;
 				}

     	     			if( ($gain > $thr_gain_low) && ($gain < $thr_gain_high) ) {       
					$chk2_gain = 1;
     	     			}
				else {
					$chk2_gain = 0;
     		     			$chk = 0;       
				}

     	     			if( ($i20 > $thr_i20_low) && ($i20 < $thr_i20_high) ) {       
					$chk2_i20 = 1;
     	     			}
				else {
					$chk2_i20 = 0;
     		     			$chk = 0;       

				}

     	     			if( ($kdiv > $thr_kdiv_low) && ($kdiv < $thr_kdiv_high) ) {       
					$chk2_kdiv = 1;
				}
				else {
					$chk2_kdiv = 0;
     		     			$chk = 0;       
     	     			}

     	     			if( ($offsettscal > $thr_offsettscal_low) && ($offsettscal < $thr_offsettscal_high) ) {       
				        $chk2_offsettscal = 1;
				}
				else {
					$chk2_offsettscal = 0;
     		     			$chk = 0;       
     	     			}

				if( ($chk_dcu6raw == 1) && ($chk_gain == 1) && ($chk2_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
					$adc0cal = "T";
				}
				else {
					$adc0cal = "F";
					$gain = 2.144 ;
				}

				if( ($chk_dcu6raw == 1) && ($chk_i20 == 1) && ($chk2_i20 == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
					$ical = "T";
				}
				else {
					$ical = "F";
					$i20 = 0.02122 ;
					$i10 = 0.01061 ;
				}

				if( ($chk_dcu6raw == 1) && ($chk_kdiv == 1) && ($chk2_kdiv == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
					$kcal = "T";
				}
				else {
					$kcal = "F";
					$kdiv = 0.56 ;
				}

				if( ($chk_dcu6raw == 1) && ($chk_offsettscal == 1) && ($chk2_offsettscal == 1) && ($chk_gain == 1) && ($adc0cal1 == "CALOK") && ($inlow1 == "NOINLOW") && ($adc0cal2 == "CALOK") && ($inlow2 == "NOINLOW")) {
					$tscal = "T";
				}
				else {
					$offsettscal = "F";
					$offsettscal = 2432 ;
				}


				if ( $chk == 0 ) {
				print  FILEOUT  "$hybid1 not ok $adc0cal1 $inlow1 $adc0cal2 $inlow2 $chk_dcu6raw $chk_gain $chk2_gain $chk_i20 $chk2_i20 $chk_kdiv $chk2_kdiv $chk_offsettscal $chk2_offsettscal\n" ;
				print  FILEOUT "        <DCUCONVERTION dcuHardId = \"$rdcuid1\" subDetector = \"$detectorid1\" dcuType = \"FEH\" structureId = \"$structureid1\" containerId = \"$moduleid1\" adcGain0 = \"$gain\" adcOffset0 = \"0\" adcInl0 = \"$inl\" adcInl0OW = \"$inlow\" adcCal0= \"$adc0cal\" i20 = \"$i20\" i10 = \"$i10\" iCal = \"T\" kDiv = \"$kdiv\" kCal = \"T\" tsGain = \"8.9\" tsOffset =\"$offsettscal\" tsCal = \"T\" /> \n";
				}
				else {
				print  FILEOUT "        <DCUCONVERTION dcuHardId = \"$rdcuid1\" subDetector = \"$detectorid1\" dcuType = \"FEH\" structureId = \"$structureid1\" containerId = \"$moduleid1\" adcGain0 = \"$gain\" adcOffset0 = \"0\" adcInl0 = \"$inl\" adcInl0OW = \"$inlow\" adcCal0= \"$adc0cal\" i20 = \"$i20\" i10 = \"$i10\" iCal = \"T\" kDiv = \"$kdiv\" kCal = \"T\" tsGain = \"8.9\" tsOffset =\"$offsettscal\" tsCal = \"T\" /> \n";
				}
			}	
		}	
	}
}

print FILEOUT "</ROWSET> \n" ;
close (FILEOUT);
close(FILEA);
close(FILEB);
close (RECFILEOUT);
close (PRODFILEOUT);
exit 0;
