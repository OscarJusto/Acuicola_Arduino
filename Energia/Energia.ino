#include <XBee.h>
#include <EmonLib.h>
#define MAX_PAYLOAD_SIZE 52

// create the XBee object
XBee xbee = XBee();
uint8_t payload[MAX_PAYLOAD_SIZE];

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

EnergyMonitor ct1, ct2, ct3, ct4;

String pzb;
char val[20];

void setup() {
  
  Serial.begin(9600);
  
  // Calibration = CT ratio / burden resistance = (100A / 0.05A) / 33 Ohms = 60.606
  ct1.current(1, 60.606);
  ct2.current(2, 60.606);
  ct3.current(3, 60.606);
  ct4.current(4, 60.606);
  
  // (ADC input, calibration, phase_shift)
  ct1.voltage(0, 122.54, 1.7);
  ct2.voltage(0, 122.54, 1.7);
  ct3.voltage(0, 122.54, 1.7);
  ct4.voltage(0, 122.54, 1.7);
  
}

void loop() {

  // Calculate all. No.of crossings, time-out
  ct1.calcVI(20,2000);
  ct2.calcVI(20,2000);
  ct3.calcVI(20,2000);
  ct4.calcVI(20,2000);
  
  float V1  = ct1.Vrms;
  float V2  = ct2.Vrms;
  float V3  = ct3.Vrms;
  float V4  = ct4.Vrms;
  float I1  = ct1.Irms;
  float I2  = ct2.Irms;
  float I3  = ct3.Irms;
  float I4  = ct4.Irms;
  float P1  = ct1.realPower;
  float P2  = ct2.realPower;
  float P3  = ct3.realPower;
  float P4  = ct4.realPower;
  
    
  String StringV1 = dtostrf(V1,3,1,val);
  String StringV2 = dtostrf(V2,3,1,val);
  String StringV3 = dtostrf(V3,3,1,val);
  String StringV4 = dtostrf(V4,3,1,val);
  String StringI1 = dtostrf(I1,2,1,val);
  String StringI2 = dtostrf(I2,2,1,val);
  String StringI3 = dtostrf(I3,2,1,val);
  String StringI4 = dtostrf(I4,2,1,val);
  String StringP1 = dtostrf(P1,3,1,val);
  String StringP2 = dtostrf(P2,3,1,val);
  String StringP3 = dtostrf(P3,3,1,val);
  String StringP4 = dtostrf(P4,3,1,val);
  
  pzb  = StringV1;
  pzb += " ";
  pzb += StringV2;
  pzb += " ";
  pzb += StringV3;
  pzb += " ";
  pzb += StringV4;
  pzb += " ";
  pzb += StringI1;
  pzb += " ";
  pzb += StringI2;
  pzb += " ";
  pzb += StringI3;
  pzb += " ";
  pzb += StringI4;
  pzb += " ";
  pzb += StringP1;
  pzb += " ";
  pzb += StringP2;
  pzb += " ";
  pzb += StringP3;
  pzb += " ";
  pzb += StringP4;
  
  for (int i=0; i<=pzb.length(); i++) {
    payload[i] = pzb[i];
  }
  
  delay(5000);
}
