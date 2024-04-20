#ifndef eventLoop_H
  #define eventLoop_H

#include <QObject>
#include <QList>
#include <QSerialPort>
#include <QTimer>
#include "loadHardware.h"
#include "batteryPack.h"


class eventLoop : public QObject
{
   Q_OBJECT

  public:
    QString QALabelPrinter;
    
    eventLoop(QObject *parent=0);
    
    bool loadSetup();
    bool openPort();
    bool openLog();
    bool applySettings(QString packID);
    void listPackIDs();
    void startDischarge();
    void closeHardware();

  private slots:
    void operate();
    void stdOutValues();
    void runGnuplot();
    
    void readPhysical();
    
  private:
    loadHardware *m_progLoad;
    QDateTime startTime;
    QString fileNameStem;
    QSerialPort *ioStream;
    QTimer *opTimer, *stdOutTimer, *plotTimer;
    FILE *logFile;
    QList<batteryPack> packs;
    double voltage, current, power, endVoltage;
    double lastPower, energy, dischargeTimeSecs;
    int currentPack;
    
  public:  
    void produceQALabel();
    void listPrinters();
};

#endif

