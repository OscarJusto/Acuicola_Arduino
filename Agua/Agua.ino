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

void on_item1_selected(MenuItem* p_menu_item) {
  Serial.println("PH");
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



void setup() {
  
  Serial.begin(9600); 
       
  configurar_menu();
    
}

void loop() {
  // put your main code here, to run repeatedly:

}
