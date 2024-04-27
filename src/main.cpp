// Sample Gauges main program
// QT4 - Experimental stage!!

#include <QGuiApplication>
#include "eventLoop.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

eventLoop *eLoop=nullptr;

void sigInt_handler(sig_atomic_t s){
  printf("Caught signal %d\n",s);
  if (eLoop!=nullptr){
    printf("Closing Hardware\n");
    eLoop->closeHardware();
  }
  exit(1);
}

int main(int argc, char * argv[]){
  int rVal, i;
  QString packId;
  QCoreApplication app(argc,argv);

  signal (SIGINT,sigInt_handler);

  eLoop = new eventLoop;

  printf("****************************\n");
  printf("** Battery discharge tool **\n");
  printf("****************************\n");
  printf("\n");
    
  if (!eLoop->loadSetup()){
    printf("Failed to load system setup. Check \"setup.xml\".\n");
    return 1;
  }
  
  if (argc==1){
    printf("Usage:\n");
    printf("  -c <channel> discharger channel to use.\n");
    printf("  -l <printer> to produce a QA label.\n");
    printf("  -p <packID>.\n");
    printf("\n");
    printf("Valid printers are:\n");
    eLoop->listPrinters();
    
    printf("\n");
    printf("Valid hardware is:\n");
    eLoop->listHardware();
    
    printf("\n");
    printf("Valid packs are:\n");
    eLoop->listPackIDs();
    return 0;
  }
  
  for (i=0;i<argc;i++){
    if (strcmp(argv[i],"-c")==0) eLoop->setActiveChannel(atoi(argv[i+1]));
    if (strcmp(argv[i],"-p")==0) packId=QString(argv[i+1]);
    if (strcmp(argv[i],"-l")==0) eLoop->QALabelPrinter=QString(argv[i+1]);
  }

  if (!eLoop->openPort()) return 2;
  printf("Applying battery pack settings\n");
  if (!eLoop->applySettings(packId)){
    eLoop->closeHardware();
    printf("Battery Pack '%s' not found. Use -p <packID>. Valid packs are:\n", packId.toLatin1().data());
    eLoop->listPackIDs();
    return 3;
  }
  
  printf("Opening log file\n");
  if (!eLoop->openLog()) return 4;

  printf("Starting battery discharge\n");
  eLoop->startDischarge();
  
  rVal=app.exec();
  return rVal;
}
