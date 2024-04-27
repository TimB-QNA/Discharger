#include "PiDischarger.h"

#include <stdexcept>
using namespace std;

piDischargerChannel::piDischargerChannel(){
  m_adcChannel=-1;
  m_dioChannel=-1;
  m_slope=1;
  m_offset=0;
  m_lastVolts=0;
  m_adc=nullptr;
  m_digIoPort=nullptr;
}

piDischargerChannel::~piDischargerChannel(){
}

void piDischargerChannel::registerHardware(ADCPi *adc, IoPi *digIo){
 m_adc=adc; m_digIoPort=digIo;
    
 if (m_dioChannel<1 || m_dioChannel>16) return;
 
 try{
   m_digIoPort->set_pin_direction(m_dioChannel, 0); // Set configured pin as output.
 } catch (exception &e){
   printf("Exception setting digital IO direction: %s\n", e.what());
 }

 enable(false);    
}
    
void piDischargerChannel::configure(QDomElement cfgElement){
  QDomElement element;
  
  element=cfgElement.firstChildElement();
  
  while (!element.isNull()) {
    if (element.tagName().toLower()=="adcchan")    m_adcChannel=element.text().toInt();
    if (element.tagName().toLower()=="diochan")    m_dioChannel=element.text().toInt();
    if (element.tagName().toLower()=="resistance") m_resistance=element.text().toDouble();
    if (element.tagName().toLower()=="slope")           m_slope=element.text().toFloat();
    if (element.tagName().toLower()=="offset")         m_offset=element.text().toFloat();   
    element = element.nextSiblingElement();
  }
}

void piDischargerChannel::enable(bool en){
  int val=0;
  
  if (m_digIoPort==nullptr) return;
  if (m_dioChannel<1 || m_dioChannel>16) return;
  
  if (en) val=1;
  try {
    m_digIoPort->write_pin(m_dioChannel, val);
  } catch (exception &e){
    printf("Exception setting digital IO pin state: %s\n", e.what());
  }
}

bool piDischargerChannel::voltage(double *volts){
  if (m_adc==nullptr) return false;
  if (m_adcChannel<1 || m_adcChannel>8) return false;
  
  try {
    m_lastVolts=m_adc->read_voltage(m_adcChannel) * m_slope + m_offset;
  } catch (exception &e){
    printf("Exception reading ADC Channel: %s\n", e.what());
    return false;
  }
  *volts=m_lastVolts;
  return true;
}

bool piDischargerChannel::current(double *amps){
  *amps=m_lastVolts/m_resistance;
  return true;
}

bool piDischargerChannel::power(double *watts){
  double amps;
  
  current(&amps);
  *watts=m_lastVolts*amps;
  
  return true;
}
  
PiDischarger::PiDischarger() : loadHardware("pidischarger", "Raspberry Pi Discharger") {
  m_adcBits=16;
  m_activeChannel=nullptr;
}

void PiDischarger::processSettings(QDomElement element){
  if (element.tagName().toLower()=="adcbits") m_adcBits   =element.text().toInt();
  if (element.tagName().toLower()=="channel") m_disChan[element.attribute("id").toInt()].configure(element);
}

bool PiDischarger::initialise(int channel){
  int i;

  try{
    m_adc = new ADCPi(0x68, 0x69); //, m_adcBits);
    m_adc->set_pga(1);
    m_adc->set_conversion_mode(0);
  } catch (exception &e){
    printf("Exception configuring ADC: %s\n", e.what());
  }

  try{
    m_digIoPort = new IoPi(0x20);
  } catch (exception &e){
    printf("Exception configuring digital IO: %s\n", e.what());
  }

  for (i=0;i<9;i++){
    m_disChan[i].registerHardware(m_adc, m_digIoPort);
  }
  
  if (channel<1 || channel>8) return false;
  m_activeChannel=&(m_disChan[channel]);
  return true;
}

bool PiDischarger::shutdown(){
  enable(false);
  delete m_adc;
  delete m_digIoPort;
  return true;
}

void PiDischarger::enable(bool en){
  if (m_activeChannel==nullptr) return;
  m_activeChannel->enable(en);
}

bool PiDischarger::voltage(double *volts){
  if (m_activeChannel==nullptr) return false;
  
  return m_activeChannel->voltage(volts);
}

bool PiDischarger::current(double *amps){
  if (m_activeChannel==nullptr) return false;
  
  return m_activeChannel->current(amps);
}

bool PiDischarger::power(double *watts){
  if (m_activeChannel==nullptr) return false;
  
  return m_activeChannel->power(watts);
}

