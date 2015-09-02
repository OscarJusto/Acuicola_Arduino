#include <LiquidCrystal.h>
#include <MenuSystem.h>


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

void on_item1_selected(MenuItem* p_menu_item) {
  Serial.println("PH");
  lcd.setCursor(0,1);
  lcd.print("       PH       ");
  delay(2000);
}

void on_item2_selected(MenuItem* p_menu_item) {
  Serial.println("OD1_TEM1");
}

void on_item3_selected(MenuItem* p_menu_item) {
  Serial.println("OD2_TEM2");
}

void on_item4_selected(MenuItem* p_menu_item) {
  Serial.println("OD3_TEM3");
}

void on_item5_selected(MenuItem* p_menu_item) {
  Serial.println("OD4_TEM4");
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
  
  configurar_botones();    
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
  
  buttonHandler();
  serialHandler();

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
