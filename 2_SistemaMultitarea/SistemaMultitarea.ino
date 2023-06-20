#include "AsyncTaskLib.h"
#include "DHTStable.h"
#include <LiquidCrystal.h>

#define DHT11_PIN A1
#define PHOTOCELL_PIN A0

LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

DHTStable DHT;

int outputValue = 0;

void fnDHC(){
  lcd.setCursor(4,0);
  lcd.print("                ");
  lcd.setCursor(4,0);
  DHT.read11(DHT11_PIN);
  lcd.print(DHT.getHumidity());
  lcd.print("\t");
  lcd.println(DHT.getTemperature());
}

void photocell(){
  lcd.setCursor(4,1);
  lcd.print("                ");
  lcd.setCursor(4,1);
  outputValue = analogRead(PHOTOCELL_PIN);
  lcd.print(outputValue);
}

AsyncTask asyncTask1(3000, true, fnDHC);
AsyncTask asyncTask2(1000, true, photocell);

void setup()
{
  Serial.begin(115200);
  lcd.begin(16, 2);

  lcd.print("H/T:");
  lcd.setCursor(0,1);
  lcd.print("Luz:");

  asyncTask1.Start();
  asyncTask2.Start();
}
void loop()
{
  asyncTask1.Update();
  asyncTask2.Update();
}
