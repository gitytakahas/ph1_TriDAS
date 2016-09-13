/*
Authors:
Juha Petäjäjärvi
Andras Ster

Software is based on:
- FEC Graphical User Interface Debugger made by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
- Various Crepe test programs made by Wojciech BIALAS - CERN, Geneva-Switzerland
*/

#include "string.h"
#include <qapplication.h>
#include "vfat_controller.h"

void help ( char **argv ) {
  std::cerr << std::endl <<"COMMAND" << std::endl ;
  std::cerr << "\t" << argv[0] << " [INTERFACE] [OPTIONS]" << std::endl  ;
  std::cerr << "INTERFACE" << std::endl ;
  std::cerr <<" \t[-pci | -vmesbs | -vmecaenpci | -vmecaenusb | -usb]" << std::endl ;
  std::cerr << "OPTIONS" << std::endl ;
  std::cerr << " \t[-fec  < slot >]" << std::endl ;
  std::cerr << " \t[-fed  < slot >]" << std::endl ;
  std::cerr << " \t[-ttcci  < slot >]" << std::endl ;
  std::cerr << " \t[-i2cspeed  100 | 200 | 400 | 1000]" << std::endl ;
  std::cerr << " \t[-devicefile  < XML file >]" << std::endl ;
  std::cerr << " \t[-detectortype  RP | GEM]" << std::endl << std::endl ;
}

int main( int argc, char **argv ) {

  if ((argc <= 1) || (!strncmp(argv[1],"-help",5)) || (!strncmp(argv[1],"--help",6))) {
    help (argv) ;
    return -1 ;
  }

  // 1st parameter should be the interface type
  if(argc>0){

    std::string fecInterface = argv[1] ;

    if((fecInterface!="-pci") && (fecInterface!="-vmesbs") && (fecInterface!="-vmecaenpci") && (fecInterface!="-vmecaenusb") && (fecInterface!="-usb")){
      std::cerr << SET_FORECOLOR_RED << "FEC interface not specified correctly." << SET_DEFAULT_COLORS << std::endl ;
      help (argv) ;
      return -1 ;
    }
  }

  QApplication a( argc, argv ) ;

  vfat_controller *control = new vfat_controller() ;
  a.setMainWidget( control ) ;

  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  //a.connect( quitButton, SIGNAL(clicked()), qApp, SLOT(quit()) );

  return a.exec() ; 
}
