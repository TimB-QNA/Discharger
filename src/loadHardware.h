#ifndef loadHardware_H
  #define loadHardware_H

#include <QDomElement>

/*!
 * Base class for programmable load hardware. Don't instantiate this class directly,
 * subclass it and add your own functionality as required.
 *
 * The call sequence should be:
 *   * readSettings
 *   * initialise()
 *   * operations
 */
class loadHardware
{
// Factory
  public:
    static loadHardware* factory(QDomElement element);
    static QStringList   typesAvailable();
    
  private:
    static bool          factorySetup(QDomElement element, loadHardware *temp);

// Class
  public:
    loadHardware(QString xmlType, QString hwName);
    virtual ~loadHardware();
    QString name();
    QString xmlType();
        
    void readSettings(QDomElement element);

    virtual bool initialise(int channel);
    virtual bool shutdown();
    virtual void setCurrent(double preset);
    virtual void enable(bool enable);

    virtual bool voltage( double *volts );
    virtual bool current( double *amps  );
    virtual bool power(   double *watts );

  protected:
    virtual void processSettings(QDomElement element);

  private:
    QString m_hardwareTag, m_hwname;
    bool isOfType(QString xmlType);

};

#endif

