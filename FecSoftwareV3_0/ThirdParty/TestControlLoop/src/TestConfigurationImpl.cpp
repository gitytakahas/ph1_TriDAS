#include "TestConfigurationImpl.h"

#include "PowerUpStepsImpl.h"
#include "CCUInfoImpl.h"
#include "ModuleInfoImpl.h"
#include "SavePreferencesImpl.h"
#include "LoadPreferencesImpl.h"
#include "LoadResultsImpl.h"
#include "FecRingSelectionImpl.h"
#include "DirChoiceImpl.h"

#include <qpushbutton.h>
#include <qlineedit.h>

TestConfigurationImpl::TestConfigurationImpl(GUISupervisor *guiSupervisor, bool testPossible) : TestConfiguration() { 
  supervisor = guiSupervisor; 

  if(!testPossible) startButton->setEnabled(false);

  i2cSpecialCommandTestCB->setEnabled(false); // test not yet implemented
}

void TestConfigurationImpl::launchPowerUpStep() {
  updateSupervisor();
    supervisor->endTestConfiguration();
    close();
}

void TestConfigurationImpl::launchCCUInfo() {
    (new CCUInfoImpl(this, supervisor))->show();
}

void TestConfigurationImpl::launchModuleInfo() {
    (new ModuleInfoImpl(this, supervisor))->show();
}

void TestConfigurationImpl::savePref() {
  updateSupervisor();
    (new SavePreferencesImpl(this, supervisor))->show();
}

void TestConfigurationImpl::loadPref() {
    (new LoadPreferencesImpl(this, supervisor))->show();
}

void TestConfigurationImpl::fecSelection() {
  (new FecRingSelectionImpl(this, supervisor))->show();
}

int TestConfigurationImpl::getSelectedTest() {
  int res = 0;
    if(fecTestCB->isChecked()) res += fecTestMask;
    if(ccuTestCB->isChecked()) res += ccuTestMask;
    if(redundancyTestCB->isChecked()) res += redundancyTestMask;
    if(modulesDevicesTestCB->isChecked()) res += modulesDevicesTestMask;
    if(dohCalibrationCB->isChecked()) res += dohCalibrationMask;
    if(configurationTestCB->isChecked()) res += configurationTestMask;
    if(psCurrentsRecordCB->isChecked()) res += psCurrentsRecordMask;
    if(i2cSpeedTestCB->isChecked()) res+= i2cSpeedTestMask;
	
    return res;
}

void TestConfigurationImpl::updateSelectedTestCheckBox() {
  nbLoopOnModuleLineEdit->setText(QString::number(supervisor->getNbLoopOnModules()));

    if(GUISupervisor::testSelected & fecTestMask) fecTestCB->setChecked(true);
    else fecTestCB->setChecked(false);
    if(GUISupervisor::testSelected & ccuTestMask) ccuTestCB->setChecked(true);
    else ccuTestCB->setChecked(false);
    if(GUISupervisor::testSelected & redundancyTestMask) redundancyTestCB->setChecked(true);
    else redundancyTestCB->setChecked(false);
    if(GUISupervisor::testSelected & modulesDevicesTestMask) modulesDevicesTestCB->setChecked(true);
    else modulesDevicesTestCB->setChecked(false);
    if(GUISupervisor::testSelected & dohCalibrationMask) dohCalibrationCB->setChecked(true);
    else dohCalibrationCB->setChecked(false);
    if(GUISupervisor::testSelected & configurationTestMask) {
      configurationTestCB->setChecked(true);
      nbLoopOnModuleLineEdit->setEnabled(true);
    }
    else {
      configurationTestCB->setChecked(false);
      nbLoopOnModuleLineEdit->setEnabled(false);
    }
    if(GUISupervisor::testSelected & psCurrentsRecordMask) psCurrentsRecordCB->setChecked(true);
    else psCurrentsRecordCB->setChecked(false);
    if(GUISupervisor::testSelected & i2cSpeedTestMask) i2cSpeedTestCB->setChecked(true);
    else i2cSpeedTestCB->setChecked(false);
    if(GUISupervisor::specialI2CTestSelected & i2cSpecialCommandTestMask) i2cSpecialCommandTestCB->setChecked(true);
    else i2cSpecialCommandTestCB->setChecked(false);
}

void TestConfigurationImpl::uploadResults() {
  (new LoadResultsImpl(this, supervisor))->show();
}

void TestConfigurationImpl::prefDirChoice() {
  (new DirChoiceImpl(this, supervisor, "Preferences Directory Choice"))->show();
}

void TestConfigurationImpl::resDirChoice() {
  (new DirChoiceImpl(this, supervisor, "Results Directory Choice"))->show();
}

void TestConfigurationImpl::configTestChanged() {
  if(configurationTestCB->isChecked()) nbLoopOnModuleLineEdit->setEnabled(true);
  else nbLoopOnModuleLineEdit->setEnabled(false);
}

void TestConfigurationImpl::updateSupervisor() {
    GUISupervisor::testSelected = getSelectedTest();

    if(i2cSpecialCommandTestCB->isChecked()) GUISupervisor::specialI2CTestSelected = 1;
    else GUISupervisor::specialI2CTestSelected = 0;

    supervisor->setNbLoopOnModules((nbLoopOnModuleLineEdit->text()).toInt());
}
