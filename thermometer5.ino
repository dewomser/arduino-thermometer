#include <OneWire.h>
#include <LiquidCrystal.h>
// Dieses Programm zeigt auf einem Display 2x16 Zeichen 
// 3 Temperaturen und den Durchschnittswert an.
// Zusammengebastelt hat das Stefan Höhn http://www.untergang.de
// 

//Hello World aus Crystaldisplay eingebunden.

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(10);  // on pin 10
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int  zaehler = 0; 
// Ich nehm an 10 Sensoren sind genug
float temperatur[10];
float tempschnitt = 0;

void setup(void) {
  Serial.begin(9600);
    // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("THERMOMETER");
 }

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    zaehler = 0;
    lcd.setCursor(0, 0); // top left
    lcd.print("TEMP1-3 = ");
    lcd.print(tempschnitt);
    
    
    
    delay(250);
    return;
  }
  else
  {
    zaehler=zaehler ++;
    
         }
  
  
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  lcd.setCursor(0, 1); // bottom left
  
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  
  lcd.print("TEMP");
  lcd.print(zaehler);
  lcd.print(" = ");
  lcd.print(celsius);
  lcd.print(" C");
  
  // Verdammt, ich kann die Länge des arrays nicht auslesen.
  temperatur[zaehler-1] = celsius;
  // tempschnitt:  Formel muss deshalb an die Anzahl der Sensoren angepasst werden.
  tempschnitt = (temperatur[0]+temperatur[1]+temperatur[2])/3;
  
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
  Serial.print("Durchschnittstemperatur = ");
  Serial.println(tempschnitt);
  
  
}
