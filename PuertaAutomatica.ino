/*
   Proyecto Electronica Digital
      PUERTA AUTOMATICA
*/

#include <Servo.h>                        //Lib Servomotor
#include <LiquidCrystal.h>                // Lib panel LCD
#include <Adafruit_NeoPixel.h>            // Lib Tira LED

//Conexiones PWM
#define echo 2                            //Echo Sensor Ultrasonico
#define trig 3                            //Trigger Sensor Ultrasonico
#define servoM 4                          //Servomotor
#define tiraLED 5                         //Tira LED
#define ledR 6                            //Led Rojo
#define ledV 7                            //Led Verde
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);  //Conexiones (Rs,E,D4,D5,D6,D7) panel LCD

//SERVO
int angulo = 0;
Servo servo;

//Tira LED
const int numLED = 4;
const int brillo = 100;
const int tiempo = 500;
Adafruit_NeoPixel tira = Adafruit_NeoPixel(numLED, tiraLED, NEO_GRB + NEO_KHZ800);
int x = 0;

void setup() {

  Serial.begin(9600); //Puerto de monitoreo

  //Entradas y salidas digitales
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledV, OUTPUT);

  //Inicio del servo
  servo.attach(servoM);
  servo.write(0);
  digitalWrite(trig, LOW); //Se inicializa Trigger en 0.

  //Inicio panel LCD
  lcd.begin(16, 2); //Se inicializa el panel LCD
  lcd.print("Iniciando Sistema");
  delay(100);
  lcd.home();

  //Inicio Tira LED
  tira.begin();
  tira.setBrightness(brillo);

}

void loop() {
  tira.setPixelColor(1, 255, 0, 0);
  tira.show();

  if (Sensor_Utrasonico() <= 150)
  {
    Puerta_Abrir();
    Puerta_Abierta();
  }
  else
  {
    Puerta_Cerrar();
    Puerta_Cerrada();
  }

}

void Puerta_Cerrada()
{
  if (servo.read() == 0)
  {
    do  {
      //Serial.println("**PUERTA CERRADA**");
      //Serial.println("LED rojo");
      //Serial.println("LED verde off");
      digitalWrite(ledR, HIGH);
      digitalWrite(ledV, LOW);
      tira.setPixelColor(1, 255, 0, 0);
      tira.show();
      lcd.clear();
      lcd.print("Cerrado");
      delay(500);
    } while (Sensor_Utrasonico() > 150);
  }
}

void Puerta_Cerrar()
{
  Serial.println("**CERRANDO PUERTA**");
  //Serial.println("LED rojo parpadea");
  //Serial.println("LED verde off");
  //Serial.print("Distancia Sensor_Utrasonico: ");
  //Serial.println(Sensor_Utrasonico());
  digitalWrite(ledR, HIGH);
  digitalWrite(ledV, LOW);
  lcd.clear();
  lcd.print("Cerrando");
  //delay(2000); //-----------------------------------------
  for (angulo  = servo.read() ; angulo >= 0; angulo--) {
    servo.write(angulo);
    //Serial.print("Angulo: ");
    //Serial.println(angulo);
    delayMicroseconds(10);
    int pos = angulo;
    if (Sensor_Utrasonico() <= 150)
    {
      Serial.println("---SENSOR DECTO UN OBJETO---");
      for (angulo  = pos; angulo <= 180; angulo++)
      {
        servo.write(angulo);
        //Serial.print("Angulo: ");
        //Serial.println(angulo);
        delayMicroseconds(10);
        if (angulo == 180)
        {
          Serial.println("**PUERTA ABIERTA**");
          //Serial.println("LED verde");
          //Serial.println("LED rojo off");
          digitalWrite(ledV, HIGH);
          digitalWrite(ledR, LOW);
          lcd.clear();
          lcd.print("Abierto");
          delay(5000);
          return;
        }
      }
    }
  }
}

void Puerta_Abierta()
{
  if (servo.read() == 180)
  {
    do  {
      Serial.println("**PUERTA ABIERTA**");
      //Serial.println("LED verde");
      //Serial.println("LED rojo off");
      digitalWrite(ledV, HIGH);
      digitalWrite(ledR, LOW);
      lcd.clear();
      lcd.print("Abierto");
      delay(5000);
    } while (Sensor_Utrasonico() <= 150);
  }
}

void Puerta_Abrir()
{
  Serial.println("**ABRIENDO PUERTA**");
  //Serial.println("LED verde parpadea");
  //Serial.println("LED rojo off");
  //Serial.print("Distancia Sensor_Utrasonico: ");
  //Serial.println(Sensor_Utrasonico());
  digitalWrite(ledV, HIGH);
  digitalWrite(ledR, LOW);
  lcd.clear();
  lcd.print("Abriendo");
  //delay(2000); //-----------------------------------------
  for (angulo  = servo.read(); angulo <= 180; angulo++) {
    servo.write(angulo);
    //Serial.print("Angulo: ");
    //Serial.println(angulo);
    delayMicroseconds(10);
  }
}

long Sensor_Utrasonico()
{
  long t;   //Tiempo de demora del ECO en llegar.
  long d;    //distancia en centimetros.

  //Señal del sensor ultrasonico con 1ms del tiempo Tx Rx.
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  t = pulseIn(echo, HIGH);
  d = t / 58.2; // Calculo de distancia en cm.

  return d;
}
