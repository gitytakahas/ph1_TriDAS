#include <qapplication.h>
#include "GUISupervisor.h"

int main( int argc, char ** argv )
{
  QApplication a( argc, argv );
  
  // force acknowledge used (-noFack or -fack (default is -fack))
  // i2cSpeed - i2cSpeed used (-i2cSpeed)
  // initFec - initialise or not the FEC (true by default)
  // fecSlot - initialise only the corresponding FEC if it is different from 0xFFFFFFFF (only for VME FEC) (-fec <>)
  // invert the clock polarity (-invrecvclk)
  FecAccess *fecAccess = FecAccess::createFecAccess ( argc, argv, &cpt ) ;
  
  if (!fecAccess) {
    std::cerr << "Creation of FecAccess failed. fecAccess pointer null." << std::endl ; 
    exit (EXIT_FAILURE) ; ; 
  }
  
  new GUISupervisor(fecAccess);
  
  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  return a.exec();   
}
