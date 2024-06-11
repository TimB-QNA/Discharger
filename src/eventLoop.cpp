// Sample Gauges main program
// QT4 - Experimental stage!!

#include <QtCore>
#include "eventLoop.h"
#include <QSerialPort>
#include <QByteArray>
#include <QProcess>
#include <unistd.h>
#include <QPrinter>
#include <QPrinterInfo>
#include <QPainter>

eventLoop::eventLoop(QObject *parent) : QObject(parent){
  m_progLoad=nullptr;

  opTimer = new QTimer();
  opTimer->setInterval(2000);
  connect(opTimer, SIGNAL(timeout()), this, SLOT(operate()));
  
  stdOutTimer = new QTimer();
  stdOutTimer->setInterval(10000); // Every 10 seconds
  connect(stdOutTimer, SIGNAL(timeout()), this, SLOT(stdOutValues()));
  
  plotTimer = new QTimer();
  plotTimer->setInterval(60000); // Every minute
  connect(plotTimer, SIGNAL(timeout()), this, SLOT(runGnuplot()));

  currentPack=0;
  m_activeChannel=0;
  dischargeTimeSecs=0;
  startTime=QDateTime::currentDateTime();
}

void eventLoop::setActiveChannel(int chan){
  m_activeChannel=chan;
}

bool eventLoop::loadSetup(){
  // load packs from setup file...
  loadHardware *tempHw;
  int errorLine, errorColumn;
  QString errorStr;
  QDomDocument domDocument;
  QDomNode root;
  QDomElement element;

  batteryPack tmpPack(&chem);
  
  QFile file("setup.xml");
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    printf("Could not open configuration file.\n");
    return false;
  }
  
  if (!domDocument.setContent(file.readAll(), true, &errorStr, &errorLine, &errorColumn)){
    printf("Parse error at line %i, column %i: %s", errorLine, errorColumn, errorStr.toLatin1().data());
    return false;
  }
  file.close();
  
  root = domDocument.documentElement();
  
  if (root.nodeName()!="batteries"){
    printf("This is not a battery discharge configuration file.\n");
    return false;
  }
  
  element=root.firstChildElement();
  while (!element.isNull()) {
    tempHw=loadHardware::factory(element); if (tempHw!=nullptr) m_progLoad=tempHw;
    if (element.tagName().toLower()=="chemistry"){
      chem.append(batteryChemistry(element));
//      packs.append(tmpPack);
    }

    if (element.tagName().toLower()=="pack"){
      tmpPack.clear();
      tmpPack.readXml(element);
      packs.append(tmpPack);
    }
    element = element.nextSiblingElement();
  }
  return true;
}

bool eventLoop::openPort(){
  if (m_progLoad==nullptr){
    printf("No load hardware defined\n");
    return false;
  }

  return m_progLoad->initialise(m_activeChannel);
}

bool eventLoop::applySettings(QString packID){
  int i;
  currentPack=nullptr;

  for (i=0;i<packs.count();i++){
    if (packs[i].isPack(packID)){
      currentPack=&(packs[i]);
    }
  }

  if (currentPack==nullptr) return false;

  currentPack->printDetails(stdout);
  printf("\n");
  printf("Setting discharge current to %lf A\n", packs[i].current/1000.);
  m_progLoad->setCurrent(currentPack->current/1000.);
      
  endVoltage=currentPack->endVoltage()*packs[i].cells;
  printf("Setting discharge end voltage to %lf V (%lf V/Cell)\n", endVoltage, currentPack->endVoltage());

  startTime=QDateTime::currentDateTime();
  fileNameStem=QString("logs/")+currentPack->id+QString("-")+startTime.toString("ddMMyyyy-hhmmss");

  return true;
}

void eventLoop::listPackIDs(){
  int i;
  
  for (i=0;i<packs.count();i++){
    printf("  %s\n", packs[i].id.toLatin1().data());
  }
}

void eventLoop::listHardware(){
  QStringList hw=loadHardware::typesAvailable();
  int i;
  
  for (i=0;i<hw.count();i++){
    printf("  %s\n", hw[i].toLatin1().data());
  }
}

bool eventLoop::openLog(){
  system("mkdir -p logs");
  logFile=fopen((fileNameStem+QString(".log")).toLatin1().data(), "w");
  return true;
}

void eventLoop::startDischarge(){
  energy=0;
  dischargeTimeSecs=0;

  startTime=QDateTime::currentDateTime();
  readPhysical();
  m_progLoad->enable(true);
  readPhysical();

  lastPower=power;

  stdOutValues();
  writeLogValues(startTime);

  opTimer->start();
  stdOutTimer->start();
  plotTimer->start();
}

void eventLoop::stdOutValues(){
  QDateTime thisSample=QDateTime::currentDateTime();
  QString time=thisSample.toString("hh:mm:ss");
  
  printf("%s   %6.3lf V   %6.3lf A   %7.3lf W   %0.3lf kJ\n", time.toLatin1().data(), voltage, current, power, energy/1000.);
}

void eventLoop::writeLogValues(QDateTime thisSample){
  QString time=thisSample.toString("dd/MM/yyyy-hh:mm:ss.zzz");

  fprintf(logFile, "%s %lf %lf %lf %lf %lf\n", time.toLatin1().data(), dischargeTimeSecs, voltage, current, power, energy);
  fflush(logFile);
}

void eventLoop::operate(){
  readPhysical();
  QDateTime thisSample=QDateTime::currentDateTime();
  QString time=thisSample.toString("dd/MM/yyyy-hh:mm:ss.zzz");
  
  dischargeTimeSecs = startTime.msecsTo(thisSample)/1000.;
  
  energy+=(power+lastPower)/2. * (double)opTimer->interval()/1000.;
  lastPower=power;
  
  writeLogValues(thisSample);

  if (voltage<endVoltage){
    m_progLoad->enable(false);
    fclose(logFile);
    runGnuplot();
    produceQALabel();
    closeHardware();
    exit(0);
  }
}

void eventLoop::readPhysical(){
  bool showMissed=false;
  bool v_ok, i_ok, p_ok;

  v_ok=m_progLoad->voltage(&voltage);
  i_ok=m_progLoad->current(&current);
  p_ok=m_progLoad->power(&power);

  if (showMissed){
    if (!v_ok) printf("Missed Voltage Reading\n");
    if (!i_ok) printf("Missed Current Reading\n");
    if (!p_ok) printf("Missed Power Reading\n");
  }
}


void eventLoop::runGnuplot(){
  QProcess gplt;
  QStringList arguments;
  char arg[40];
  
  arguments.clear();
  arguments.append("-c"); // Enable arguments
  arguments.append("batteryLog.gplt");
  arguments.append(currentPack->id);
  arguments.append(currentPack->chemistry().name());
  sprintf(arg, "%i",  currentPack->cells);                        arguments.append(arg);
  sprintf(arg, "%lf", currentPack->endVoltage());                 arguments.append(arg);
  sprintf(arg, "%lf", currentPack->chemistry().nominalVoltage()); arguments.append(arg);
  sprintf(arg, "%lf", currentPack->capacity);                     arguments.append(arg);
  arguments.append(fileNameStem);
  
  gplt.setStandardOutputFile("/dev/null");
  gplt.setStandardErrorFile("/dev/null");
  gplt.startDetached("gnuplot", arguments);
}

void eventLoop::produceQALabel(){
  int i, foundPrinter=-1, foundSize=-1, w, h;
  int y, gap;
  float relEnergy, predictedTime;
  char text[256], fail[10];
  QRect pageRect;
  QPainter painter;
  
  if (QALabelPrinter.isEmpty()) return;
  
  QList<QPrinterInfo> printers=QPrinterInfo::availablePrinters();
  for (i=0;i<printers.count();i++){
    if (printers[i].printerName().toLower() == QALabelPrinter.toLower()){
      foundPrinter=i;
      break;
    }
  }
  
  if (foundPrinter==-1){
    printf("Failed to find printer\n");
    return;
  }
  
  printf("Valid paper sizes...\n");
  QList<QPageSize> pageSizes=printers[foundPrinter].supportedPageSizes();
  for (i=0;i<pageSizes.count();i++){
    printf("  %s - %lf mm , %lf mm\n", pageSizes[i].name().toLatin1().data(), pageSizes[i].rect(QPageSize::Millimeter).width(), pageSizes[i].rect(QPageSize::Millimeter).height() );
    w=pageSizes[i].rect(QPageSize::Millimeter).width()*100;
    h=pageSizes[i].rect(QPageSize::Millimeter).height()*100;
    if (w==1693 && h==5398){
      foundSize=i;
      break;
    }
  }
  
  if (foundSize==-1){
    printf("Failed to find paper size\n");
    return;
  }
    
  QPrinter printer(printers[foundPrinter], QPrinter::HighResolution);
  
  printf("Assigned printer\n");
  
  if (printer.isValid()){
    printf("Printer is valid\n");
  }else{
    printf("Printer not valid\n");
  }
  
  printer.setPageSize(pageSizes[foundSize]);
  printer.setPageOrientation(QPageLayout::Landscape);
  
  if (!painter.begin(&printer)) { // failed to open file
    printf("Failed to initialise painter\n");
    return;
  }
  
  pageRect=printer.pageLayout().paintRectPixels(printer.resolution());
  printf("Resolution = %i\n", printer.resolution());
  printf("Page Size (w,h) = %i , %i\n", pageRect.width(), pageRect.height());
  
  relEnergy=energy/currentPack->ratedEnergy()/10.;
  
  predictedTime=currentPack->capacity/currentPack->current;
  
  printf("Relative Energy = %.2lf %%\n", relEnergy);
  if (relEnergy<85){
    sprintf(fail, "Fail");
  }else{
    sprintf(fail, "OK");
  }
  
  y=50; gap=165;
  sprintf(text, "Pack: %s", currentPack->id.toLatin1().data() );                         painter.drawText(0, y, text); y+=gap;
  sprintf(text, "Date: %s\t%s", startTime.toString("dd/MM/yy").toLatin1().data(), fail); painter.drawText(0, y, text); y+=gap;
  sprintf(text, "Cond: %0.1lfA %0.1lfV/cell\t%.0f%%",
                currentPack->current/1000.,
                currentPack->endVoltage(),
                relEnergy );                                                             painter.drawText(0, y, text); y+=gap;
  sprintf(text, "Type: %s %i cells %.0lf mAh",
                currentPack->chemistry().name().toLatin1().data(),
                currentPack->cells,
                currentPack->capacity );                                                 painter.drawText(0, y, text); y+=gap;
  
  
  painter.end();
}

void eventLoop::listPrinters(){
  int i;
  
  QList<QPrinterInfo> printers=QPrinterInfo::availablePrinters();
  
  for (i=0;i<printers.count();i++){
    printf("  %s\n", printers[i].printerName().toLatin1().data());
    printf("    - %s\n", printers[i].makeAndModel().toLatin1().data());
    printf("    - %s\n", printers[i].description().toLatin1().data());
  }
}

void eventLoop::closeHardware(){
  m_progLoad->enable(false);
  m_progLoad->shutdown();
}
