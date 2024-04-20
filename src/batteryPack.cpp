// Sample Gauges main program
// QT4 - Experimental stage!!

#include <QtXml>
#include "batteryPack.h"

batteryPack::batteryPack(){
  clear();
}

void batteryPack::clear(){
  // Default settings for Ni-Cad...
  id="Unknown";
  capacity=1500;
  batt_chem=1;
  cells=1;
  endVoltage=1.0;
  current=50;
}

void batteryPack::readXml(QDomElement xml){
  QDomElement element;
  
  element=xml.firstChildElement();
  while (!element.isNull()) {
    if (element.tagName().toLower()=="id") id=element.text();
    if (element.tagName().toLower()=="capacity") capacity=element.text().toDouble();
    if (element.tagName().toLower()=="chemistry"){
      if (element.text().toLower()=="ni-cad"){
        batt_chem=1; nominalVoltage=1.2;
      }
      if (element.text().toLower()=="nimh"){
        batt_chem=2; nominalVoltage=1.2;
      }
      if (element.text().toLower()=="lipo"){
        batt_chem=3; nominalVoltage=3.7;
      }
      if (element.text().toLower()=="pb"){
        batt_chem=4; nominalVoltage=2;
      }
    }
    if (element.tagName().toLower()=="cells") cells=element.text().toInt();
    if (element.tagName().toLower()=="endvoltage") endVoltage=element.text().toDouble();
    if (element.tagName().toLower()=="current") current=element.text().toDouble();
    element = element.nextSiblingElement();
  }
}
    
QString batteryPack::chemistry(){
  if (batt_chem==1) return QString("Ni-Cad");
  if (batt_chem==2) return QString("NiMH");
  if (batt_chem==3) return QString("LiPo");
  if (batt_chem==4) return QString("Lead-Acid");
  return QString("Unknown");
}

bool batteryPack::isPack(QString packId){
  return id.toLower()==packId.toLower();
}

void batteryPack::printDetails(FILE *stream){
  int pThrs, pTmin;
  float ratedEnergy, predictedTime;
  
  ratedEnergy=cells*(nominalVoltage*capacity/1000.*3600.)/1000.;
  predictedTime=capacity/current * 60.;
  
  pThrs=(int)(predictedTime / 60.);
  pTmin=(int)(predictedTime) % 60;
  
  fprintf(stream, "Pack Details:\n");
  fprintf(stream, "           Cells: %i\n", cells);
  fprintf(stream, "     Nom Voltage: %0.2f V\n", nominalVoltage);
  fprintf(stream, "        Capacity: %0.0f mAh\n", capacity);
  fprintf(stream, "    Rated Energy: %0.3f kJ\n", ratedEnergy);
  fprintf(stream, "  Discharge Time: %i hrs %i mins\n", pThrs, pTmin );
  
}
