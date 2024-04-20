#ifndef PIDISCHARGER_H
  #define PIDISCHARGER_H

#include <QSerialPort>
#include "../lib/ABElectronics_CPP_Libraries/ADCPi/ABE_ADCPi.h"
#include "../lib/ABElectronics_CPP_Libraries/IOPi/ABE_IoPi.h"
#include "loadHardware.h"

using namespace ABElectronics_CPP_Libraries;

class piDischargerChannel
{
  public:
    piDischargerChannel();
    ~piDischargerChannel();
    
    void registerHardware(ADCPi *adc, IoPi *digIo);
    void configure(QDomElement cfgElement);
    void enable(bool enable);
    bool voltage(double *volts);
    bool current(double *amps);
    bool power(double *watts);
    
  private:
    int m_adcChannel;
    int m_dioChannel;
    
    double m_slope;
    double m_offset;
    double m_resistance;
    
    double m_lastVolts;
    
    ADCPi *m_adc;
    IoPi  *m_digIoPort;
};

/*!
 * Base class for programmable load hardware. Don't instantiate this class directly,
 * subclass it and add your own functionality as required.
 *
 * The call sequence should be:
 *   * readSettings
 *   * initialise()
 *   * operations
 */
class PiDischarger : public loadHardware
{
  public:
    PiDischarger();

    bool initialise(int channel);
    bool shutdown();
    void enable(bool enable);

    bool voltage(double *volts);
    bool current(double *amps);
    bool power(double *watts);

  protected:
    void processSettings(QDomElement element);

  private:
    piDischargerChannel m_disChan[8];
    piDischargerChannel *m_activeChannel;
    
    int m_adcBits;     ///< User selectable bit-width for ADC.

    ADCPi *m_adc;
    IoPi  *m_digIoPort;
};

#endif
