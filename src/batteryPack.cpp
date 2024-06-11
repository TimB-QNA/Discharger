// Sample Gauges main program
// QT4 - Experimental stage!!

#include <QtXml>
#include "batteryPack.h"

batteryPack::batteryPack(QList<batteryChemistry> *chem){
  clear();
  m_chem=chem;
}

void batteryPack::clear(){
  // Default settings for Ni-Cad...
  id="Unknown";
  capacity=1500;
  cells=1;
  current=50;
  activeChem=-1;

  m_customEndVoltage=false;
  m_endVolts=1.0;
}

void batteryPack::readXml(QDomElement xml){
  int i;
  QDomElement element;
  
  element=xml.firstChildElement();
  while (!element.isNull()) {
    if (element.tagName().toLower()=="id") id=element.text();
    if (element.tagName().toLower()=="capacity")  capacity=element.text().toDouble();
    if (element.tagName().toLower()=="chemistry"){
      for (i=0;i<m_chem->count();i++){
        if (m_chem->value(i).ref().toLower()==element.text().toLower()) activeChem=i;
      }
    }
    if (element.tagName().toLower()=="cells") cells=element.text().toInt();
    if (element.tagName().toLower()=="endvoltage"){
      m_customEndVoltage=true;
      m_endVolts=element.text().toDouble();
    }
    if (element.tagName().toLower()=="current") current=element.text().toDouble();
    element = element.nextSiblingElement();
  }

  if (activeChem==-1){
    printf("No chemistry defined for pack %s", id.toLatin1().data());
  }
}

batteryChemistry batteryPack::chemistry(){
  return m_chem->value(activeChem);
}

bool batteryPack::isPack(QString packId){
  return id.toLower()==packId.toLower();
}

void batteryPack::printDetails(FILE *stream){
  int pThrs, pTmin;
  float predictedTime;
  
  predictedTime=capacity/current * 60.;
  
  pThrs=(int)(predictedTime / 60.);
  pTmin=(int)(predictedTime) % 60;
  
  fprintf(stream, "Pack Details:\n");
  fprintf(stream, "           Cells: %i\n", cells);
  fprintf(stream, "       Chemistry: %s\n", chemistry().name().toLatin1().data());
  fprintf(stream, "     Nom Voltage: %0.2f V\n", chemistry().nominalVoltage());
  fprintf(stream, "        Capacity: %0.0f mAh\n", capacity);
  fprintf(stream, "    Rated Energy: %0.3f kJ\n", ratedEnergy());
  fprintf(stream, "  Discharge Time: %i hrs %i mins\n", pThrs, pTmin );
  
}

double batteryPack::endVoltage(){
  if (!m_customEndVoltage) return chemistry().endVoltage();
  return m_endVolts;
}

double batteryPack::ratedEnergy(){
  return cells*(chemistry().nominalVoltage()*capacity/1000.*3600.)/1000.;
}
