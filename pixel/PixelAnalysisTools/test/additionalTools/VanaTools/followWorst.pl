#!/usr/bin/perl
use strict;
use warnings;

open(my $f1, "<", "../0deg_iter1/0deg_iter1.log") or die "Can't open. $!";
open(my $f2, "<", "../0deg_iter2/0deg_iter2.log") or die "Can't open. $!";
open(my $f3, "<", "../0deg_iter3/0deg_iter3.log") or die "Can't open. $!";
open(my $f4, "<", "../0deg_iter4/0deg_iter4.log") or die "Can't open. $!";
open(my $f5, "<", "../0deg_iter5/0deg_iter5.log") or die "Can't open. $!";
#open(my $f6, "<", "../0deg_iter6/0deg_iter6.log") or die "Can't open. $!";
#open(my $f7, "<", "../0deg_iter7/0deg_iter7.log") or die "Can't open. $!";
#open(my $f8, "<", "../0deg_iter8/0deg_iter8.log") or die "Can't open. $!";
my $fmax = 6;

my @a1=<$f1>;
my @a2=<$f2>;
my @a3=<$f3>;
my @a4=<$f4>;
#my @a5=<$f5>;
#my @a6=<$f6>;
#my @a7=<$f7>;
#my @a8=<$f8>;
#my @aa = (@a1, @a2, @a3, @a4, @a5, @a6);
#my @aa;
#$aa[1]=[@a6];
#$aa[2]=[@a5];
#$aa[3]=[@a4];
#$aa[4]=[@a3];
#$aa[5]=[@a2];
#$aa[6]=[@a1];

my $roc = "";
while(<$f5>){
    
    if(/(\S)(Pix_)(\S*)/){
	$roc="$1$2$3";
    }

    my $look = 0;
    if(/(deltaVana=)(\s)(\S*)/){
	my $thisDelta = $3;
	if($thisDelta > 5 || $thisDelta<-5){
	    print "$roc \n";
	    print "deltaVana= $thisDelta\n";
	    $look=1;
	}
    }


    if($look == 1){
#        ###############################
#	my $looknow = 0;
#	foreach(@a6){
#	    my $fline = $_;
#	    my $thisroc = "";
#	    if($fline=~/(\S)(Pix_)(\S*)/){
#		$thisroc="$1$2$3";
#		if($thisroc eq $roc){
#		    $looknow = 1;
#		}
#		else{
#		    $looknow = 0;
#		}
#	    }
#	    elsif($looknow == 1){
#		if($fline=~/(deltaVana=)(\s)(\S*)/){
#		    print "deltaVana= $3\n";
#		    last;
#		}
#	    }
#	    
#	}
#        ###############################	

        ###############################
#    my $looknow = 0;
#	foreach(@a5){
#	    my $fline = $_;
#	    my $thisroc = "";
#	    if($fline=~/(\S)(Pix_)(\S*)/){
#		$thisroc="$1$2$3";
#		if($thisroc eq $roc){
#		    $looknow = 1;
#		}
#		else{
#		    $looknow = 0;
#		}
#	    }
#	    elsif($looknow == 1){
#		if($fline=~/(deltaVana=)(\s)(\S*)/){
#		    print "deltaVana= $3\n";
#		    last;
#		}
#	    }
#	    
#	}
        ###############################	

        ###############################
	my $looknow = 0;
	foreach(@a4){
	    my $fline = $_;
	    my $thisroc = "";
	    if($fline=~/(\S)(Pix_)(\S*)/){
		$thisroc="$1$2$3";
		if($thisroc eq $roc){
		    $looknow = 1;
		}
		else{
		    $looknow = 0;
		}
	    }
	    elsif($looknow == 1){
		if($fline=~/(deltaVana=)(\s)(\S*)/){
		    print "deltaVana= $3\n";
		    last;
		}
	    }
	    
	}
        ###############################	

        ###############################
	$looknow = 0;
	foreach(@a3){
	    my $fline = $_;
	    my $thisroc = "";
	    if($fline=~/(\S)(Pix_)(\S*)/){
		$thisroc="$1$2$3";
		if($thisroc eq $roc){
		    $looknow = 1;
		}
		else{
		    $looknow = 0;
		}
	    }
	    elsif($looknow == 1){
		if($fline=~/(deltaVana=)(\s)(\S*)/){
		    print "deltaVana= $3\n";
		    last;
		}
	    }
	    
	}
        ###############################	

        ###############################
	$looknow = 0;
	foreach(@a2){
	    my $fline = $_;
	    my $thisroc = "";
	    if($fline=~/(\S)(Pix_)(\S*)/){
		$thisroc="$1$2$3";
		if($thisroc eq $roc){
		    $looknow = 1;
		}
		else{
		    $looknow = 0;
		}
	    }
	    elsif($looknow == 1){
		if($fline=~/(deltaVana=)(\s)(\S*)/){
		    print "deltaVana= $3\n";
		    last;
		}
	    }
	    
	}
        ###############################	

        ###############################
	$looknow = 0;
	foreach(@a1){
	    my $fline = $_;
	    my $thisroc = "";
	    if($fline=~/(\S)(Pix_)(\S*)/){
		$thisroc="$1$2$3";
		if($thisroc eq $roc){
		    $looknow = 1;
		}
		else{
		    $looknow = 0;
		}
	    }
	    elsif($looknow == 1){
		if($fline=~/(deltaVana=)(\s)(\S*)/){
		    print "deltaVana= $3\n";
		    last;
		}
	    }
	    
	}
        ###############################	









}
}



    
#    if($look == 1){
#	foreach(@aa){
#	    my @thisarray = $_;
#	    my $looknow = 3;
#	    foreach(@thisarray){
#		my $fline = $_;
#		my $thisroc = "";
#		print "$looknow $fline";
#		if($fline=~/(\S)(Pix_)(\S*)/){
#		    $thisroc="$1$2$3";
#		    if($thisroc eq $roc){
#			print "FINDME $roc\n";
#			$looknow = 1;
#		    }
#		    else{
#			$looknow = 0;
#		    }
#		}
#		#elsif($looknow == 1){
#		else{
#		    if($fline=~/(deltaVana=)(\s)(\S*)/){
#			print "$looknow deltaVana= $3\n";
#		       #last;
#		    }
#		}
#		
#	    }
#	}
#   }
#
#}


