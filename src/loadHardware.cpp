#include "loadHardware.h"

// Factory code to create load hardware on demand.
#include "Tenma7213210Load.h"
#include "PiDischarger.h"

/*!
 * This function produces a specific hardware instance from the XML element provided.
 * If adding new hardware your hardware drive must be instantiated here.
 * To avoid a memory leak the temporary instance gets deleted in factorySetup. 
 */
loadHardware* loadHardware::factory(QDomElement element){
  loadHardware *testHw;
  QString xmlType;

  if (element.tagName().toLower()!="hardware") return nullptr;

  testHw = new Tenma7213210Load; if (factorySetup(element, testHw)) return testHw;
  testHw = new PiDischarger;     if (factorySetup(element, testHw)) return testHw;
  
  return nullptr;
}

/*!
 * This function provides a list of all the available hardware names.
 * If adding new hardware your hardware drive must be instantiated here.
 */
QStringList loadHardware::typesAvailable(){
  QStringList typeList;
  loadHardware *testHw;
  
  testHw = new Tenma7213210Load;   typeList.append(testHw->name());   delete testHw;
  testHw = new PiDischarger;       typeList.append(testHw->name());   delete testHw;
  
  return typeList;
}

/*!
 * This function handles type checking of the temporary hardware instance passed in.
 * If it is of the required type then the hardware is configured and the function returns true.
 * If it is not, the instance is deleted and the function returns false. 
 */
bool loadHardware::factorySetup(QDomElement element, loadHardware *temp){
  // Test that it's the correct type
  if (!temp->isOfType( element.attribute("type").toLower() )){
    delete temp;
    return false;
  }

  // Tests passed, configure hardware
  temp->readSettings(element);
  return true;
}

/*!
 * Constructor to allow subclasses to do a once-only assignment of the XML type for thier hardware.
 * This constructor *MUST* be called, otherwise the XML parser will not recognise the hardware and the
 * factory system will not instantiate your subclass.
 */
loadHardware::loadHardware(QString xmlType, QString hwName){
  m_hardwareTag = xmlType;
  m_hwname = hwName;
}

/*!
 * Destructor.
 */
loadHardware::~loadHardware(){
}

/*!
 * Return hardware name.
 */
QString loadHardware::name(){
  return m_hwname;
}

/*!
 * Return xml type attribute.
 */
QString loadHardware::xmlType(){
  return m_hardwareTag;
}
    
/*!
 * Default initialise routine. Do nothing and return failed. Use this to open connections to hardware etc.
 */
bool loadHardware::initialise(int channel){
  return false;
}

/*!
 * Default shutdown routine. Do nothing and return failed. Use this in your subclass to close
 * open connections to hardware etc.
 */
bool loadHardware::shutdown(){
  return false;
}

/*!
 * If the passed XML element is "hardware" loop over all sub-elements
 * and pass them to processSettings for handling in subclass.
 */
void loadHardware::readSettings(QDomElement element){
  QDomElement subElement;

  if (element.tagName().toLower()!="hardware") return;

  subElement=element.firstChildElement();
  while (!subElement.isNull()) {
    processSettings(subElement);
    subElement = subElement.nextSiblingElement();
  }
}

/*!
 * Set current function. Default implementation does nothing.
 */
void loadHardware::setCurrent(double preset){
}

/*!
 * Enable load function. Default implementation does nothing.
 * Subclasses should enable the load hardware when passed "true".
 */
void loadHardware::enable(bool enable){
}

/*!
 * Read voltage from hardware. Do not change the value passed in unless the reading was successful.
 * The default implementation returns false and leaves volts unchanged.
 */
bool loadHardware::voltage( double *volts ){
  return false;
}

/*!
 * Read current from hardware. Do not change the value passed in unless the reading was successful.
 * The default implementation returns false and leaves amps unchanged.
 */
bool loadHardware::current( double *amps ){
  return false;
}

/*!
 * Read power from hardware. Do not change the value passed in unless the reading was successful.
 * The default implementation returns false and leaves watts unchanged.
 */
bool loadHardware::power( double *watts ){
  return false;
}

/*!
 * Allows processing of configuration options in sub-class. Default implementation does nothing.
 */
void loadHardware::processSettings(QDomElement element){
}

/*!
 * Used in the factory system to produce hardware interfaces without making the type publicly known.
 */
bool loadHardware::isOfType(QString xmlType){
  return m_hardwareTag == xmlType;
}
