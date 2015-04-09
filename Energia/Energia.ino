#include <avr/io.h>
#include <avr/interrupt.h>
#include <PString.h>
#include <XBee.h>
#include <EmonLib.h>

// create the XBee object
XBee xbee = XBee();

EnergyMonitor ct1, ct2, ct3, ct4;

int XB0 = 10;  int XB1 = 11;  int XB2 = 12;  int XB3 = 13;    //Entradas de comandos remotos xbee para activar motores
int MT0 = 9;   int MT1 = 8;   int MT2 = 7;   int MT3 = 6;     //Salidas de control de motores

int RD0 = 0;   int RD1 = 0;   int RD2 = 0;   int RD3 = 0;     //Indicador actual de estados de comandos remotos xbee
int LD0 = 0;   int LD1 = 0;   int LD2 = 0;   int LD3 = 0;     //Indicador anterior de estados de comandos remotos xbee

void sendInfoPayload(String info) {
  char payload[1+info.length()];
  PString infoString(payload, sizeof(payload));
  infoString.print(info);

  // SH + SL Address of receiving XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
  ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)payload, sizeof(payload));

  xbee.send(zbTx);
}

ISR(TIMER1_COMPA_vect) {

  RD0 = digitalRead(XB0);  RD1 = digitalRead(XB1);
  RD2 = digitalRead(XB2);  RD3 = digitalRead(XB3);

  if ((RD0 != LD0) || (RD1 != LD1) || (RD2 != LD2) || (RD3 != LD3)) {

    if (RD0 == HIGH) {
      digitalWrite(MT0, HIGH);
    } else {
      digitalWrite(MT0, LOW);
    }
  
    if (RD1 == HIGH) {
      digitalWrite(MT1, HIGH);
    } else {
      digitalWrite(MT1, LOW);
    }
  
    if (RD2 == HIGH) {
      digitalWrite(MT2, HIGH);
    } else {
      digitalWrite(MT2, LOW);
    }
  
    if (RD3 == HIGH) {
      digitalWrite(MT3, HIGH);
    } else {
      digitalWrite(MT3, LOW);
    }
  }
  
  LD0 = RD0;  LD1 = RD1;  LD2 = RD2;  LD3 = RD3;
  
}

String pzb;
char val[20];

void setup() {
  
  Serial.begin(9600);
  
  pinMode(XB0, INPUT);  pinMode(XB1, INPUT);  pinMode(XB2, INPUT);  pinMode(XB3, INPUT);
  pinMode(MT0, OUTPUT); pinMode(MT1, OUTPUT); pinMode(MT2, OUTPUT); pinMode(MT3, OUTPUT);

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
  
   //inicializar Timer1
  cli();                 //interrupciones de parada global
  TCCR1A = 0;            //establce todo el registro TCCR1 a 0
  TCCR1B = 0;            //hace lo mismo
  //Establecer el registro de comparacion para la cuenta del timer deseada
  OCR1A =15624; // -> 1 Seg
  //OCR1A =7811.5; // = (16MHz)/(1024*2) -1 = 31249 (debe ser < 65536) -> 0.5 Seg
  //OCR1A =3905.25; // = (16MHz)/(1024*4) -1 = 31249 (debe ser < 65536) -> 0.25 Seg
  //Turna en modo CTC:
  TCCR1B |= (1 << WGM12);
  //Ajusta los bits de CS10 y CS12 a 1024 prescale
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  //Permite al timer compara la interrupcion
  TIMSK1 |= (1 << OCIE1A);
  //Permite al timer1 interrupcion por desbordamiento
  //TIMSK1 = (1 << TOIE1);
  //habilita las interrupciones globales
  sei();

  delay (10000);
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
  String StringLD0 = String(LD0);
  String StringLD1 = String(LD1);
  String StringLD2 = String(LD2);
  String StringLD3 = String(LD3);
  
  pzb = "";

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
  pzb += " ";
  pzb += StringLD0;
  pzb += StringLD1;
  pzb += StringLD2;
  pzb += StringLD3;
  
  sendInfoPayload(pzb);
  
  delay(5000);
}
