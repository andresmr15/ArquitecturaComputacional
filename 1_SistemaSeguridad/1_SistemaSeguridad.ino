//Librerias
#include <LiquidCrystal.h>
#include <Keypad.h>

//Definición de constantes para led RGB y buzzer
#define LED_RED A5
#define LED_BLUE A4
#define LED_GREEN A3
#define BUZZER A0

//Variables para el control de la contraseña
short int count = 0;
short int trycount = 0;
boolean estado;

//Keypad
char password[6] = {'3','2','1','3','2','1'};
char inPassword[6];

const uint8_t ROWS = 4;
const uint8_t COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t colPins[COLS] = { 26, 27, 28, 29 }; // Pins connected to C1, C2, C3, C4
uint8_t rowPins[ROWS] = { 22, 23, 24, 25 }; // Pins connected to R1, R2, R3, R4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//LiquidCrystal
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

void setup() {
pinMode(LED_RED, OUTPUT); //Sistema bloqueado
pinMode(LED_BLUE, OUTPUT); //Clave incorrecta
pinMode(LED_GREEN, OUTPUT); //Clave correcta
pinMode(BUZZER, OUTPUT);

  //Keypad
  Serial.begin(9600);
  //LiquidCrystal
  lcd.begin(16, 2);
  //Pedir clave al iniciar
    lcd.print("Clave:");
}

//Procedimiento para limpiar el contenido del LCD y pedir la clave nuevamente
void reiniciarLCD(){
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,0);
    lcd.print("Clave:");
}

void claveCorrecta(){
    trycount=0;//Reiniciar el contador de intentos

  
  digitalWrite(LED_GREEN, HIGH);//Encender led en color verde
  lcd.setCursor(0,1);//Cambiar posicion en la que se va a escribir la siguiente cadena
  lcd.print("Correcta :)");

  //Tono de clave correcta para un buzzer pasivo
  tone(BUZZER, 1800, 100);
  delay(100);
  tone(BUZZER, 2000, 100);
  delay(100);
  tone(BUZZER, 2200, 100);
  delay(100);
  tone(BUZZER, 2500, 350);
  delay(350);
  noTone(BUZZER);

  delay(1000);//Esperar para apagar led y reiniciar LCD
  digitalWrite(LED_GREEN, LOW);//Apagar led
  reiniciarLCD();
}

void claveIncorrecta(){
  digitalWrite(LED_BLUE, HIGH);//Encender led en color azul
  lcd.setCursor(0,1);//Cambiar posicion en la que se va a escribir la siguiente cadena
  lcd.print("Incorrecta >:(");

 //Tono de clave incorrecta para un buzzer pasivo
  tone(BUZZER, 1000, 200);
  delay(200);
  noTone(BUZZER);

  delay(1000);//Esperar para apagar led y reiniciar LCD
  digitalWrite(LED_BLUE, LOW);//Apagar led
  reiniciarLCD();
  trycount++;//Incrementar el número de intentos
}

void tiempoAgotado(){
  digitalWrite(LED_BLUE, HIGH);//Encender led en color azul
  lcd.setCursor(0,1);//Cambiar posicion en la que se va a escribir la siguiente cadena
  lcd.print("Tiempo agotado...");
  
  //Tono de tiempo de espera agotado para un buzzer pasivo
  tone(BUZZER, 200, 150);
  delay(200);
  tone(BUZZER, 200, 150);
  delay(150);
  noTone(BUZZER);

  delay(1000);//Esperar para apagar led y reiniciar LCD
  digitalWrite(LED_BLUE, LOW);
  reiniciarLCD();
  trycount++;//Incrementar el número de intentos
}

//Funcion para comparar la clave ingresada con la clave almacenada
boolean comprobarClave(){
    for(int i=0; i<6; i++){
      if(inPassword[i] != password[i]){
        return false;
      }
    }
    return true;
}

void updateDisplay(){
  //Al llegar a tres intentos consecutivos de clave incorrecta
  if(trycount==3){
    digitalWrite(LED_RED, HIGH); //Se activa el led en rojo
    lcd.setCursor(0,1);//Se cambia la posicion en la que se va a escribir la siguiente cadena
    lcd.print("Sis.bloqueado :o");
    if(trycount==3){
      //Tono de sistema bloqueado para un buzzer pasivo, el cual solo ocurre una vez despues 
      //de que el número de intentos incrementa a 4
      tone(BUZZER, 100, 150);
      delay(150);
      noTone(BUZZER);
      trycount++;
    }
  }
  else if(count==6){
    count = 0;
    estado = comprobarClave();//Comprobación de clave dependiendo del estado
    if(estado == true){
      claveCorrecta();
    } 
    else{
      claveIncorrecta();
    }
  }
}

void loop() {
  unsigned int startTime; //Valor del tiempo en el que inicia el conteo despues de presionar una tecla
  unsigned int actualTime; //Valor del tiempo actual
  
  //Mostrar cursor (_) cada cierto tiempo y solo cuando el contador de intetos sea menor que 3
  if(millis() /250 % 2  == 0 && trycount<3)
  {
    lcd.cursor();//Mostrar cursor
  }
  else{
    lcd.noCursor();//Ocultar cursor
  }

  updateDisplay();
  
  //Si se presiona una tecla, se muestra *
  char key = keypad.getKey();
  if (key) {
    startTime = millis();//Se guarda el valor del tiempo en el que se presiona una tecla nuevamente
    lcd.print("*");
    inPassword[count] = key;//Almacena el caracter de la tecla presionada en el arreglo inPassword
    count++;//Se incrementa el valor del contador de teclas presionadas
  }
   actualTime = millis();//Se actualiza el tiempo actual

  if(actualTime-startTime >= 4000 && count>0){//Esperar 4 segundos cuando se ha presionado al menos una tecla
    tiempoAgotado();//Mostrar el mensaje de tiempo de espera agotado
    startTime=0; //Se reinicia el tiempo actual
    count=0;//Reiniciar el valor del contador de teclas presionadas
  }
}
