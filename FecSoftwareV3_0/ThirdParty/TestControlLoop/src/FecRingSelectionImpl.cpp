#include "FecRingSelectionImpl.h"

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qvaluevector.h>

FecRingSelectionImpl::FecRingSelectionImpl(QWidget *parent, GUISupervisor *guiSupervisor) : FecRingSelection(parent, "Fec-Ring Selection", true) {

    supervisor = guiSupervisor;
    
    mainLayout = new QGridLayout( this, 1, 1, 11, 6, "FecRingSelectionLayout"); 

    okButtonLayout = new QHBoxLayout( 0, 0, 6, "OkButtonLayout"); 
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    okButtonLayout->addItem( spacer );

    okButton = new QPushButton( this, "okButton" );
    okButton->setText("Ok");
    okButtonLayout->addWidget( okButton );
    QSpacerItem* spacer_2 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    okButtonLayout->addItem( spacer_2 );

    mainLayout->addLayout( okButtonLayout, 1, 0 );

    // signals and slots connections
    connect( okButton, SIGNAL( released() ), this, SLOT( fecRingSelected() ) );
    
    chBLayout = new QGridLayout (0, 1, 1, 0, 6, "chBLayout");
    
    QValueVector<int> vFecSlot;
    QValueVector<int> vRingSlot;    
    QValueVector<int> fecRingSelected = supervisor->getFecRingSelected();
    
    int column = -1;
    int maxFecRingPerColumn = 8;
    int indice = 0;
    for (QValueVector<int>::iterator it = fecRingSelected.begin(); it < fecRingSelected.end() ; it = it+3) { // create a check box for each fec-ring
	if(indice%maxFecRingPerColumn == 0) column++;
	addNewCheckBox(*it, *(it+1), *(it+2), indice%maxFecRingPerColumn, column);
	indice++;
    }
    
    mainLayout->addLayout( chBLayout, 0, 0 );
    
    // ajusting the display
    int ySize = 56 + (maxFecRingPerColumn-1)*27;
    int xSize = 219 + column*112;
    resize( QSize(xSize, ySize).expandedTo(minimumSizeHint()) );
}

void FecRingSelectionImpl::addNewCheckBox(int fecSlot, int ringSlot, int selected, int row, int column) {
    QCheckBox *chB = new QCheckBox( this, "chB" );
    QString label;
    label.sprintf("Fec %d Ring %d", fecSlot, ringSlot);
    chB->setText(label);
    if(selected == 1) chB->setChecked(true);
    else chB->setChecked(false);
    existingCheckBox.push_back(chB);
    chBLayout->addWidget(chB, row, column);
}

void FecRingSelectionImpl::fecRingSelected() {
    /*
1/ recuperation du vecteur de GUISupervisor
2/ parcours pour chaque checkBox puis
si checkbox selectionnee, on fait rien, sinon on passe la valeur a false
3/ update du vecteur de GUISupervisor
*/
    
    QValueVector<int> fecRingSelected = supervisor->getFecRingSelected(); // 1/
    QValueVector<int>::iterator it = fecRingSelected.begin();
    
    for(QValueVector<QCheckBox *>::iterator chBIt = existingCheckBox.begin(); chBIt < existingCheckBox.end() ; chBIt++) { // 2/
	if((*chBIt)->isChecked()) *(it+2) = 1;
	else *(it+2) = 0;
	
	it = it+3;	
    }    
    supervisor->updateFecRingSelected(fecRingSelected); // 3/
   
    close();    
}
