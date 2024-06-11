#ifndef batteryChemistry_H
  #define batteryChemistry_H

#include <QtXml>
#include <QString>

class batteryChemistry
{

  public:
    batteryChemistry();
    batteryChemistry(QDomElement xml);

    int id();
    QString ref();
    QString name();
      
    bool verify(double endVoltage, double capacity, double current);

    double endVoltage();
    double nominalVoltage();

  private:
    int m_id;

    QString m_name, m_ref;
    double m_minVolts; // Minimum safe voltage
    double m_maxVolts; // Maximum safe voltage
    double m_nomVolts; // Nominal Cell voltage
    double m_endVolts; // Discharge End Voltage
    double m_maxCharge; // Maximum safe charge rate
    double m_maxDischarge; // Maximum safe discharge rate

    void clear();
};

#endif

