#include <EmonLib.h>

EnergyMonitor ct1, ct2, ct3, ct4;

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
  
  delay(5000);
}
