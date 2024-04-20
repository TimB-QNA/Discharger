#ifndef batteryPack_H
  #define batteryPack_H

#include <QtXml>

class batteryPack
{

  public:
    batteryPack();
    void clear();
    void readXml(QDomElement xml);
    QString chemistry();
    bool isPack(QString packId);
    void printDetails(FILE *stream);
      
    QString id;
    double capacity; // mAh
    int cells;
    double endVoltage; // Discharge End Voltage
    double current; // Discharge Current (mA)
    double nominalVoltage; // Nominal Cell voltage (depends on cell chemistry)
    
  private:
    int batt_chem=1; // 1 NiCad, 2 NiMh, 3 LiPo
};

#endif

