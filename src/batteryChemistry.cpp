#include <QtXml>
#include "batteryChemistry.h"

batteryChemistry::batteryChemistry(){
  clear();
}

batteryChemistry::batteryChemistry(QDomElement xml){
  clear();
  QDomElement element;

  element=xml.firstChildElement();
  while (!element.isNull()) {
    if (element.tagName().toLower()=="id")   m_id=element.text().toInt();
    if (element.tagName().toLower()=="ref") m_ref=element.text();
    if (element.tagName().toLower()=="name") m_name=element.text();

    if (element.tagName().toLower()=="minvoltage")           m_minVolts=element.text().toDouble();
    if (element.tagName().toLower()=="maxvoltage")           m_maxVolts=element.text().toDouble();
    if (element.tagName().toLower()=="nomvoltage")           m_nomVolts=element.text().toDouble();
    if (element.tagName().toLower()=="endvoltage")           m_endVolts=element.text().toDouble();
    if (element.tagName().toLower()=="maxchargerate")       m_maxCharge=element.text().toDouble();
    if (element.tagName().toLower()=="maxdischargerate") m_maxDischarge=element.text().toDouble();

    element = element.nextSiblingElement();
  }
}

void batteryChemistry::clear(){
  m_ref=QString();
  m_name=QString("Unknown Chemistry");

  // Set conditions which should ALWAYS return false when verified
  m_minVolts=100;
  m_maxVolts=0;
  m_nomVolts=1;
  m_endVolts=100;
  m_maxCharge=0;
  m_maxDischarge=0;
}

QString batteryChemistry::ref(){
  return m_ref;
}

QString batteryChemistry::name(){
  return m_name;
}

int batteryChemistry::id(){
  return m_id;
}

double batteryChemistry::endVoltage(){
  return m_endVolts;
}

double batteryChemistry::nominalVoltage(){
  return m_nomVolts;
}
