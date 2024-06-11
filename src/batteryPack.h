#ifndef batteryPack_H
  #define batteryPack_H

#include <QtXml>
#include "batteryChemistry.h"

class batteryPack
{

  public:
    batteryPack(QList<batteryChemistry> *chem);
    void clear();
    void readXml(QDomElement xml);
    batteryChemistry chemistry();
    bool isPack(QString packId);
    void printDetails(FILE *stream);
      
    QString id;
    double capacity; // mAh
    int cells;
    double endVoltage();
    double ratedEnergy();
    double current; // Discharge Current (mA)
    
  private:
    bool   m_customEndVoltage;
    double m_endVolts;

    int activeChem; // list index.
    QList<batteryChemistry> *m_chem;
};

#endif

