#ifndef Tenma7213210Load_H
  #define Tenma7213210Load_H

#include <QSerialPort>
#include "loadHardware.h"

/*!
 * Base class for programmable load hardware. Don't instantiate this class directly,
 * subclass it and add your own functionality as required.
 *
 * The call sequence should be:
 *   * readSettings
 *   * initialise()
 *   * operations
 */
class Tenma7213210Load : public loadHardware
{
  public:
    Tenma7213210Load();
    ~Tenma7213210Load();
    bool initialise(int channel);
    bool shutdown();
    void setCurrent(double preset);
    void enable(bool enable);

    bool voltage(double *volts);
    bool current(double *amps);
    bool power(double *watts);

  protected:
    void processSettings(QDomElement element);

  private:
    QString commPort;
    QSerialPort *ioStream;

    QByteArray readReply();
};

#endif
