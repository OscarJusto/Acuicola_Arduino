#include <avr/io.h>
#include <avr/interrupt.h>
#include <PString.h>
#include <XBee.h>
#include <EmonLib.h>

XBee xbee = XBee();                                           // create the XBee object

EnergyMonitor ct0, ct1, ct2, ct3;

float V0, V1, V2, V3;
float I0, I1, I2, I3;
float P0, P1, P2, P3;

int XB0 = 10;  int XB1 = 11;  int XB2 = 12;  int XB3 = 13;    //Entradas de pines digitales para comandos remotos xbee
int MT0 = 9;   int MT1 = 8;   int MT2 = 7;   int MT3 = 6;     //Salidas de pines digitales para control de motores
int SF0 = 5;                                                  //Salida de pin digital para semaforo

int RD0 = 0;   int RD1 = 0;   int RD2 = 0;   int RD3 = 0;     //Indicadores de pines digitales actuales de comandos remotos xbee
int LD0 = 0;   int LD1 = 0;   int LD2 = 0;   int LD3 = 0;     //Indicadores de pines digitales anteriores de comandos remotos xbee

int BT0 = 0;   int BT1 = 0;   int BT2 = 0;   int BT3 = 0;     //Indicador de estados de motores

int SMF = 0;                                                  //Indicador de semaforo

String pzb;
char val[20];

volatile int volt_low = 100;
volatile int volt_high = 140;

volatile int time_out_0 = 40;
volatile int count_on_0 = 0;
volatile int count_off_0 = 0;

volatile int time_out_1 = 40;
volatile int count_on_1 = 0;
volatile int count_off_1 = 0;

volatile int time_out_2 = 40;
volatile int count_on_2 = 0;
volatile int count_off_2 = 0;

volatile int time_out_3 = 40;
volatile int count_on_3 = 0;
volatile int count_off_3 = 0;

volatile int count_jump_0 = 0;
volatile int count_jump_1 = 0;
volatile int count_jump_2 = 0;
volatile int count_jump_3 = 0;

volatile int seconds = 0;
volatile int samples = 0;

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

  seconds ++;

//  if (seconds <= 10) {
//    Serial.print("Seconds "); Serial.println(seconds);
//    }

  if (seconds >= 10) {

    seconds = 10;
    samples ++;
    
    RD0 = digitalRead(XB0);  RD1 = digitalRead(XB1);
    RD2 = digitalRead(XB2);  RD3 = digitalRead(XB3);
    
    if ( ((V0 < volt_low) || (V0 > volt_high)) || ((V1 < volt_low) || (V1 > volt_high)) || ((V2 < volt_low) || (V2 > volt_high)) || ((V3 < volt_low) || (V3 > volt_high)) ) {
      digitalWrite(SF0, HIGH);
      SMF = 1;
    } else {
      SMF = 0;
      digitalWrite(SF0, LOW);
    }

    if ( (V0 < volt_low) || (V0 > volt_high) ) {
      count_off_0 ++;
      count_on_0 = 0;
    } else {
      count_on_0 ++;
      count_off_0 = 0;
    }
    
    if ( (V1 < volt_low) || (V1 > volt_high) ) {
      count_off_1 ++;
      count_on_1 = 0;
      if (count_off_1 >= 40 || BT1 == LOW) {
        count_jump_1 = 0;
      } else {
        count_jump_1 = 10;
      }
    } else {
      count_on_1 ++;
      count_jump_1 ++;
      count_off_1 = 0;
    }
    
    if ( (V2 < volt_low) || (V2 > volt_high) ) {
      count_off_2 ++;
      count_on_2 = 0;
      if (count_off_2 >= 40 || BT2 == LOW) {
        count_jump_2 = 0;
      } else {
        count_jump_2 = 20;
      }
    } else {
      count_on_2 ++;
      count_jump_2 ++;
      count_off_2 = 0;
    }
    
    if ( (V3 < volt_low) || (V3 > volt_high) ) {
      count_off_3 ++;
      count_on_3 = 0;
      if (count_off_3 >= 40 || BT3 == LOW) {
        count_jump_3 = 0;
      } else {
        count_jump_3 = 30;
      }
    } else {
      count_on_3 ++;
      count_jump_3 ++;
      count_off_3 = 0;
    }
    
    if (count_off_0 >= time_out_0) {
      digitalWrite(MT0, LOW);
      BT0 = 0;
      count_off_0 = time_out_0;
    } else if (count_on_0 >= time_out_0) {
      if (RD0 == HIGH) {
      digitalWrite(MT0, HIGH);
      BT0 = 1;
      } else {
        digitalWrite(MT0, LOW);
        BT0 = 0;
      }
      count_on_0 = time_out_0;
    } else if (RD0 == LOW) {
      digitalWrite(MT0, LOW);
      BT0 = 0;
    }
    
    if (count_off_1 >= time_out_1) {
      digitalWrite(MT1, LOW);
      BT1 = 0;
      count_off_1 = time_out_1;
    } else if (count_on_1 >= time_out_1) {
      if (RD1 == HIGH && count_jump_1 >= 50 ) {
        digitalWrite(MT1, HIGH);
        BT1 = 1;
        count_jump_1 = 50;
      } else {
        digitalWrite(MT1, LOW);
        BT1 = 0;
        if (count_jump_1 >= 50) {
          count_jump_1 = 50;
        }
      }
      count_on_1 = time_out_1;
    } else if (RD1 == LOW) {
      digitalWrite(MT1, LOW);
      BT1 = 0;
    }
    
    if (count_off_2 >= time_out_2) {
      digitalWrite(MT2, LOW);
      BT2 = 0;
      count_off_2 = time_out_2;
    } else if (count_on_2 >= time_out_2) {
      if (RD2 == HIGH && count_jump_2 >= 60) {
        digitalWrite(MT2, HIGH);
        BT2 = 1;
        count_jump_2 = 60;
      } else {
        digitalWrite(MT2, LOW);
        BT2 = 0;
        if (count_jump_2 >= 60) {
          count_jump_1 = 60;
        }
      }
      count_on_2 = time_out_2;
    } else if (RD2 == LOW) {
      digitalWrite(MT2, LOW);
      BT2 = 0;
    }
    
    if (count_off_3 >= time_out_3) {
      digitalWrite(MT3, LOW);
      BT3 = 0;
      count_off_3 = time_out_3;
    } else if (count_on_3 >= time_out_3) {
      if (RD3 == HIGH && count_jump_3 >= 70) {
        digitalWrite(MT3, HIGH);
        BT3 = 1;
        count_jump_3 = 70;
      } else {
        digitalWrite(MT3, LOW);
        BT3 = 0;
        if (count_jump_3 >= 70) {
          count_jump_1 = 70;
        }
      }
      count_on_3 = time_out_3;
    } else if (RD3 == LOW) {
      digitalWrite(MT3, LOW);
      BT3 = 0;
    }
    
    LD0 = RD0;  LD1 = RD1;  LD2 = RD2;  LD3 = RD3;
/*
    Serial.print("V0: "); Serial.print(V0); Serial.print(" ");
    Serial.print("V1: "); Serial.print(V1); Serial.print(" ");
    Serial.print("V2: "); Serial.print(V2); Serial.print(" ");
    Serial.print("V3: "); Serial.print(V3); Serial.print(" ");
    
    Serial.print("off_0: "); Serial.print(count_off_0); Serial.print(" ");
    Serial.print("off_1: "); Serial.print(count_off_1); Serial.print(" ");
    Serial.print("off_2: "); Serial.print(count_off_2); Serial.print(" ");
    Serial.print("off_3: "); Serial.print(count_off_3); Serial.print(" ");
    Serial.print("on_0: ");  Serial.print(count_on_0);  Serial.print(" ");
    Serial.print("on_1: ");  Serial.print(count_on_1);  Serial.print(" ");
    Serial.print("on_2: ");  Serial.print(count_on_2);  Serial.print(" ");
    Serial.print("on_3: ");  Serial.print(count_on_3);  Serial.print(" ");

    Serial.print("jump_1: ");  Serial.print(count_jump_1); Serial.print(" ");
    Serial.print("jump_2: ");  Serial.print(count_jump_2); Serial.print(" ");
    Serial.print("jump_3: ");  Serial.print(count_jump_3); Serial.print(" ");
    
    Serial.print("Cmdos: "); Serial.print(LD0); Serial.print(LD1); Serial.print(LD2); Serial.print(LD3); Serial.print(" ");
    Serial.print("Edos: "); Serial.print(BT0); Serial.print(BT1); Serial.print(BT2); Serial.println(BT3);
*/
  }
  
  if (samples >= 5) {
    samples = 0;
    sendInfoPayload(pzb);
  }
  
}

void setup() {
  
  Serial.begin(9600);
  
  pinMode(XB0, INPUT);  pinMode(XB1, INPUT);  pinMode(XB2, INPUT);  pinMode(XB3, INPUT);
  pinMode(MT0, OUTPUT); pinMode(MT1, OUTPUT); pinMode(MT2, OUTPUT); pinMode(MT3, OUTPUT);
  pinMode(SF0, OUTPUT);

  // Calibration = CT ratio / burden resistance = (100A / 0.05A) / 33 Ohms = 60.606
  ct0.current(1, 60.606);
  ct1.current(2, 60.606);
  ct2.current(3, 60.606);
  ct3.current(4, 60.606);
  
  // (ADC input, calibration, phase_shift)
  ct0.voltage(0, 122.54, 1.7);
  ct1.voltage(0, 122.54, 1.7);
  ct2.voltage(0, 122.54, 1.7);
  ct3.voltage(0, 122.54, 1.7);
  
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

  delay (1000);
}

void loop() {

  // Calculate all. No.of crossings, time-out
  ct0.calcVI(20,2000);
  ct1.calcVI(20,2000);
  ct2.calcVI(20,2000);
  ct3.calcVI(20,2000);
  
  V0  = ct0.Vrms;
  V1  = ct1.Vrms;
  V2  = ct2.Vrms;
  V3  = ct3.Vrms;
  I0  = ct0.Irms;
  I1  = ct1.Irms;
  I2  = ct2.Irms;
  I3  = ct3.Irms;
  P0  = ct0.realPower;
  P1  = ct1.realPower;
  P2  = ct2.realPower;
  P3  = ct3.realPower;

  String StringV0 = dtostrf(V0,3,1,val);
  String StringV1 = dtostrf(V1,3,1,val);
  String StringV2 = dtostrf(V2,3,1,val);
  String StringV3 = dtostrf(V3,3,1,val);
  String StringI0 = dtostrf(I0,2,1,val);
  String StringI1 = dtostrf(I1,2,1,val);
  String StringI2 = dtostrf(I2,2,1,val);
  String StringI3 = dtostrf(I3,2,1,val);
  String StringP0 = dtostrf(P0,3,1,val);
  String StringP1 = dtostrf(P1,3,1,val);
  String StringP2 = dtostrf(P2,3,1,val);
  String StringP3 = dtostrf(P3,3,1,val);
  String StringLD0 = String(LD0);
  String StringLD1 = String(LD1);
  String StringLD2 = String(LD2);
  String StringLD3 = String(LD3);
  String StringBT0 = String(BT0);
  String StringBT1 = String(BT1);
  String StringBT2 = String(BT2);
  String StringBT3 = String(BT3);
  String StringSMF = String(SMF);
  
  pzb = "";

  pzb  = StringV0;
  pzb += " ";
  pzb += StringV1;
  pzb += " ";
  pzb += StringV2;
  pzb += " ";
  pzb += StringV3;
  pzb += " ";
  pzb += StringI0;
  pzb += " ";
  pzb += StringI1;
  pzb += " ";
  pzb += StringI2;
  pzb += " ";
  pzb += StringI3;
  pzb += " ";
  pzb += StringP0;
  pzb += " ";
  pzb += StringP1;
  pzb += " ";
  pzb += StringP2;
  pzb += " ";
  pzb += StringP3;
  pzb += " ";
  pzb += StringLD0;
  pzb += StringLD1;
  pzb += StringLD2;
  pzb += StringLD3;
  pzb += " ";
  pzb += StringBT0;
  pzb += StringBT1;
  pzb += StringBT2;
  pzb += StringBT3;
  pzb += " ";
  pzb += StringSMF;
  
  //sendInfoPayload(pzb);
  
  delay(4000);
}
