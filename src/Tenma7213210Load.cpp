#include "Tenma7213210Load.h"

Tenma7213210Load::Tenma7213210Load() : loadHardware("tenma7213210", "Tenma 72-13210") {
  commPort="";
  ioStream=nullptr;
}

Tenma7213210Load::~Tenma7213210Load(){
}
    
void Tenma7213210Load::processSettings(QDomElement element){
  if (element.tagName().toLower()=="port") commPort=element.text();
}

bool Tenma7213210Load::initialise(int channel){
  if (commPort.isEmpty()){
    printf("Serial port not specified. Cannot open port.\n");
    return false;
  }

  ioStream = new QSerialPort;
  ioStream->setPortName(commPort);
  ioStream->setBaudRate(115200);
  ioStream->setDataBits(QSerialPort::Data8);
  ioStream->setParity(QSerialPort::NoParity);
  ioStream->setStopBits(QSerialPort::OneStop);
  ioStream->setFlowControl(QSerialPort::NoFlowControl);

  printf("Opening serial port\n");
  if (!ioStream->open(QIODevice::ReadWrite)){
    printf("Failed to open port\n");
    return false;
  }
  printf("Port opened sucessfully\n");
  return true;
}

bool Tenma7213210Load::shutdown(){
  ioStream->close();
  return true;
}

void Tenma7213210Load::setCurrent(double preset){
  char cmd[16];
  if (preset>30) preset=30;
  sprintf(cmd,":CURR %.4lfA\r",preset);
  ioStream->write(cmd,strlen(cmd));
  ioStream->waitForBytesWritten(100);
}

void Tenma7213210Load::enable(bool enable){
  char cmd[9] = { 0x00, 0x00, 0x3a, 0x49, 0x4e, 0x50, 0x20, 0x31, 0x0d};
  if (!enable) cmd[7]=0x30;
  ioStream->write(cmd,9);
  ioStream->waitForBytesWritten(100);
}

bool Tenma7213210Load::voltage(double *volts){
  QByteArray reply;

  ioStream->flush();
  ioStream->write(":MEAS:VOLT?\r");
  reply=readReply();
  if (reply.isEmpty()) return false;

  reply.chop(1); // remove trailing V
//  printf("Voltage Reply: '%s'\n", reply.data());
  *volts=reply.toDouble();
  return true;
}

bool Tenma7213210Load::current(double *amps){
  QByteArray reply;

  ioStream->flush();
  ioStream->write(":MEAS:CURR?\r");
  reply=readReply();
  if (reply.isEmpty()) return false;

  reply.chop(1); // remove trailing A
//  printf("Current Reply: '%s'\n", reply.data());
  *amps=reply.toDouble();
  return true;
}

bool Tenma7213210Load::power(double *watts){
  QByteArray reply;

  ioStream->flush();
  ioStream->write(":MEAS:POW?\r");
  reply=readReply();
  if (reply.isEmpty()) return false;

  reply.chop(1); // remove trailing W
//  printf("Power Reply: '%s'\n", reply.data());
  *watts=reply.toDouble();
  return true;
}

QByteArray Tenma7213210Load::readReply(){
  int i;
  QByteArray buffer;
  
  for (i=0;i<30 && !buffer.contains("\r");i++){
    ioStream->waitForReadyRead(10);
    buffer.append(ioStream->readAll());
  }

  return buffer.trimmed();
}

