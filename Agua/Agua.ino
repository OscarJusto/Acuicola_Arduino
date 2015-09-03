#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <PString.h>
#include <XBee.h>
#include <MenuSystem.h>

#define ONE_WIRE_BUS  45
#define TWO_WIRE_BUS  39
#define THREE_WIRE_BUS 48
#define FOUR_WIRE_BUS 47
#define FIVE_WIRE_BUS 46

OneWire oneWire(ONE_WIRE_BUS);
OneWire twoWire(TWO_WIRE_BUS);
OneWire threeWire(THREE_WIRE_BUS);
OneWire fourWire(FOUR_WIRE_BUS);
OneWire fiveWire(FIVE_WIRE_BUS);

DallasTemperature sensors_one(&oneWire);
DallasTemperature sensors_two(&twoWire);
DallasTemperature sensors_three(&threeWire);
DallasTemperature sensors_four(&fourWire);
DallasTemperature sensors_five(&fiveWire);

DeviceAddress sensores_temp[5];
int numberOfDevices;

int SENSOR_T0 = 0;
int SENSOR_T1 = 1;
int SENSOR_T2 = 2;
int SENSOR_T3 = 3;
int SENSOR_T4 = 4;

MenuSystem menu_principal;
Menu menu_info                ("Info estanques >");
MenuItem mi_estanque_1        ("   Estanque 1  >");
MenuItem mi_estanque_2        ("<  Estanque 2  >");
MenuItem mi_estanque_3        ("<  Estanque 3  >");
MenuItem mi_estanque_4        ("<  Estanque 4  >");
Menu menu_info_pH             ("< Sensor de pH >");
MenuItem mi_pH_1              ("<< Estanque 1 >>");
MenuItem mi_pH_2              ("<< Estanque 2 >>");
MenuItem mi_pH_3              ("<< Estanque 3 >>");
MenuItem mi_pH_4              ("<< Estanque 4   ");
Menu menu_calibrar            ("<  Calibrar     ");
MenuItem mc_sensor_1          ("<<     pH     >>");
MenuItem mc_sensor_2          ("<<     OD 1   >>");
MenuItem mc_sensor_3          ("<<     OD 2   >>");
MenuItem mc_sensor_4          ("<<     OD 3   >>");
MenuItem mc_sensor_5          ("<<     OD 4     ");

LiquidCrystal lcd(32,33,34,35,36,37);

volatile boolean BOTON_OPRIMIDO = false;
volatile char BOTON_NOMBRE;

XBee xbee = XBee();

String pzb;
char val [20];

//Funcion para ajustar payload xbee y enviar datos
void sendInfoPayload(String info) {
  char payload[1+info.length()];
  PString infoString(payload, sizeof(payload));
  infoString.print(info);
  
  // SH + SL Adrress of receiving XBee
  XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000);
  ZBTxRequest zbTx = ZBTxRequest(addr64, (uint8_t *)payload, sizeof(payload));
  
  xbee.send(zbTx);
}

void printAddress(DeviceAddress deviceAddress) {
  
  for (uint8_t i = 0; i < 8; i++) {
    
    if (deviceAddress[i] <16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void pedir_temperaturas() {
  
  sensors_one.requestTemperatures();
  sensors_two.requestTemperatures();
  sensors_three.requestTemperatures();
  sensors_four.requestTemperatures();
  sensors_five.requestTemperatures();
  
}  

void on_item1_selected(MenuItem* p_menu_item) {
  Serial.println("PH");
  lcd.setCursor(0,1);
  lcd.print("       PH       ");
  delay(2000);
}

void on_item2_selected(MenuItem* p_menu_item) {
  print_LCD_TEMP(SENSOR_T1);  
}

void on_item3_selected(MenuItem* p_menu_item) {
  print_LCD_TEMP(SENSOR_T2);
}

void on_item4_selected(MenuItem* p_menu_item) {
  print_LCD_TEMP(SENSOR_T3);
}

void on_item5_selected(MenuItem* p_menu_item) {
  print_LCD_TEMP(SENSOR_T4);
}

void on_cal1_selected(MenuItem* p_menu_item) {
  Serial.println("CAL_PH"); 
}

void on_cal2_selected(MenuItem* p_menu_item) {
  Serial.println("CAL_OD1");
}

void on_cal3_selected(MenuItem* p_menu_item) {
  Serial.println("CAL_OD2");
}

void on_cal4_selected(MenuItem* p_menu_item) {
  Serial.println("CAL_OD3");
}
void on_cal5_selected(MenuItem* p_menu_item) {
  Serial.println("CAL_OD4");
}

void configurar_menu () {
    
  Serial.println("Construyendo menu.");
  
  menu_info_pH.add_item(&mi_pH_1, &on_item1_selected);
  menu_info_pH.add_item(&mi_pH_2, &on_item1_selected);
  menu_info_pH.add_item(&mi_pH_3, &on_item1_selected);  
  menu_info_pH.add_item(&mi_pH_4, &on_item1_selected);  
  
  menu_info.add_menu(&menu_info_pH);
  menu_info.add_item(&mi_estanque_1, &on_item2_selected);
  menu_info.add_item(&mi_estanque_2, &on_item3_selected);
  menu_info.add_item(&mi_estanque_3, &on_item4_selected);
  menu_info.add_item(&mi_estanque_4, &on_item5_selected);
  
  menu_info.add_menu(&menu_calibrar);
  menu_calibrar.add_item(&mc_sensor_1, &on_cal1_selected);
  menu_calibrar.add_item(&mc_sensor_2, &on_cal2_selected);
  menu_calibrar.add_item(&mc_sensor_3, &on_cal3_selected);
  menu_calibrar.add_item(&mc_sensor_4, &on_cal4_selected);
  menu_calibrar.add_item(&mc_sensor_5, &on_cal5_selected);
  
  menu_calibrar.add_menu(&menu_info_pH);
  
  menu_principal.set_root_menu(&menu_info);
  
  Serial.println("Menu construido.");
  
}

void configurar_botones() {
  attachInterrupt(0, ISR_W, RISING);
  attachInterrupt(1, ISR_S, RISING);
  attachInterrupt(4, ISR_D, RISING);
  attachInterrupt(5, ISR_A, RISING);
}

void ISR_S() {
  BOTON_OPRIMIDO = true;
  BOTON_NOMBRE = 's';
}

void ISR_W() {
  BOTON_OPRIMIDO = true;
  BOTON_NOMBRE = 'w';
}

void ISR_D() {
  BOTON_OPRIMIDO = true;
  BOTON_NOMBRE = 'd';
}

void ISR_A() {
  BOTON_OPRIMIDO = true;
  BOTON_NOMBRE = 'a';
}

void setup() {
  
  Serial.begin(9600); 
  lcd.begin(16,2);
  sensors_one.begin();
  sensors_two.begin();
  sensors_three.begin();
  sensors_four.begin();
  sensors_five.begin();
  
  configurar_botones();    
  
  imprimir_info();
  configurar_menu();
    
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Eje Acuicola    ");
  lcd.setCursor(0,1);
  lcd.print("Version 0.1     ");
  delay(2500);  
  displayMenu();  
}

void loop() {
  
  pedir_temperaturas();  
  buttonHandler();
  serialHandler();
  
  print_TEMP(0);
  print_TEMP(1);
  print_TEMP(2);
  print_TEMP(3);
  print_TEMP(4);
  Serial.println();

  //todas_temperaturas();
  
}

void displayMenu() {
  
  lcd.clear();
  lcd.setCursor(0,0);  
  // Display the menu
  Menu const* cp_menu = menu_principal.get_current_menu();    
  lcd.print(cp_menu->get_name());
  
  lcd.setCursor(0,1);
  lcd.print(cp_menu->get_selected()->get_name());
}

void serialHandler() {
  char inChar;
  if((inChar = Serial.read())>0) {
    switch (inChar) {
    case 'w': // Previus item
      menu_principal.prev();
      displayMenu();
      break;
    case 's': // Next item
      menu_principal.next();
      displayMenu();
      break;
    case 'a': // Back presed
      menu_principal.back();
      displayMenu();
      break;
    case 'd': // Select presed
      menu_principal.select();
      displayMenu();
      break;
    case '?':
    case 'h': // Display help
      serialPrintHelp();
      break;
    default:
      break;
    }
  }
}

void buttonHandler() {
  if (BOTON_OPRIMIDO) {
    BOTON_OPRIMIDO = false;
    switch (BOTON_NOMBRE) {
      case 'w': // Previos item
          menu_principal.prev();
          displayMenu();
          break;
      case 's': // Next item
          menu_principal.next();
          displayMenu();
          break;
      case 'a': // Back presed
          menu_principal.back();
          displayMenu();
          break;
      case 'd': // Select presed
          menu_principal.select();
          displayMenu();
          break;
    }
  }
}
       

void serialPrintHelp() {
  Serial.println("***************");
  Serial.println("w: go to previus item (up)");
  Serial.println("s: go to next item (down)");
  Serial.println("a: go back (right)");
  Serial.println("d: select \"selected\" item");
  Serial.println("?: print this help");
  Serial.println("h: print this help");
  Serial.println("***************");

}

void imprimir_info() {
  
  numberOfDevices = sensors_one.getDeviceCount() + sensors_two.getDeviceCount() + sensors_three.getDeviceCount() + sensors_four.getDeviceCount() + sensors_five.getDeviceCount();
  
  Serial.print(numberOfDevices, DEC);
  Serial.println(" DS18B20");
      
  if (!sensors_one.getAddress(sensores_temp[0], 0));
  if (!sensors_two.getAddress(sensores_temp[1], 0));
  if (!sensors_three.getAddress(sensores_temp[2], 0));
  if (!sensors_four.getAddress(sensores_temp[3], 0));
  if (!sensors_five.getAddress(sensores_temp[4], 0)); 
    
  for (int i=0; i<5; i++) {
    Serial.print("T");
    Serial.print(i, DEC);
    Serial.print(": ");        
    printAddress(sensores_temp[i]);
    Serial.println();
  }
}

void todas_temperaturas() {
  
  for (int i=0; i<5; i++) {
    Serial.print("T");
    Serial.print(i, DEC);
    Serial.print(": ");
    leer_temperatura(i);
    Serial.println();
  }
  
}

void print_TEMP (int idx) {
  
   String pref = "T";
   int numidx = idx;
   float _T = leer_muestra(idx, "T");
   Serial.print(pref + String(numidx) + ": ");
   Serial.println(_T);
   
 }
 
void print_LCD_TEMP (int idx) {

  String pref = "T";
  int numidx = idx;
  float _T = leer_muestra(idx, "T");
  char tempod1[6];
  String _Ts = dtostrf(_T, 1, 2, tempod1);
  String msg = pref + String(numidx) + ": " + _Ts + " oC    ";
  lcd.setCursor(0,1);
  lcd.print(msg);
  delay(5000);

}
 
float leer_muestra (int num_sensor, String tipo_sensor) {
   
   if (tipo_sensor.equals("T")) {
     return leer_temperatura(num_sensor);
   }
   return 0.0;
 }
 
float leer_temperatura(int sensor) {  
  
  if (sensor==0) {
    return sensors_one.getTempC(sensores_temp[0]);    
  } else if (sensor == 1) {
    return sensors_two.getTempC(sensores_temp[1]);    
  } else if (sensor == 2) {
    return sensors_three.getTempC(sensores_temp[2]);    
  } else if (sensor == 3) {
    return sensors_four.getTempC(sensores_temp[3]);    
  } else if (sensor == 4) {
    return sensors_five.getTempC(sensores_temp[4]);    
  }
  
}
