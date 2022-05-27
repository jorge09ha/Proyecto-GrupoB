/*
     Proyecto Electrónica Digital
        PUERTA AUTOMATICA
            Grupo B
*/

#include <Servo.h> //Lib Servomotor.
#include <LiquidCrystal.h> // Lib panel LCD.
#include <Adafruit_NeoPixel.h> // Lib Tira LED.

//Conexiones Digitales.
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Conexiones (Rs,E,D4,D5,D6,D7) panel LCD.
#define ledR 8 //Led Rojo pin 8.
#define ledV 9 //Led Verde pin 9.
#define tiraLED 10 //Tira LED pin 10.
#define trig 11 //Trigger Sensor Ultrasónico pin 11.
#define echo 12 //Echo Sensor Ultrasónico pin 12.
#define servoM 13 //Servomotor pin 13.

//Servomotor.
int angulo = 0; //Se inicia el ángulo en 0.
Servo servo; //Se declara servo.

//Tira LED
#define numLED 4 //Cantidad de leds en la tira.
Adafruit_NeoPixel pixels(numLED, tiraLED, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(9600); //Puerto de monitoreo.

  //Entradas y salidas digitales.
  pinMode(trig, OUTPUT); //El trigger como salida.
  pinMode(echo, INPUT); //El echo como salida.
  pinMode(ledR, OUTPUT); //El LED rojo como salida.
  pinMode(ledV, OUTPUT); //El LED verde como salida.
  digitalWrite(trig, LOW); //Se inicializa Trigger en OFF.

  servo.attach(servoM); //Conecta la variable Servo a un pin.
  servo.write(0); // Pone el servo en 0.

  lcd.begin(16, 2); //Inicializa la interfaz a la pantalla LCD y especifica las dimensiones (ancho y alto) de la pantalla.
  lcd.home(); //Coloca el cursor en la parte superior izquierda de la pantalla LCD.
  lcd.print("Grupo B"); //Print de control.
  delay (1000);
  pixels.begin(); //Inicio de la Tira LED.
}

void loop()
{
  if (Sensor_Utrasonico() <= 150) //Si el sensor ultrasónico detecta algo a 1.5 metros.
  {
    Puerta_Abrir(); //Proceso de puerta abriendo.
    Puerta_Abierta(); //Proceso de puerta abierta.
  }
  else
  {
    Puerta_Cerrar(); //Proceso de puerta cerrando.
    Puerta_Cerrada(); //Proceso de puerta cerrada.
  }
}
//--------------------Proceso de puerta cerrada--------------------//
void Puerta_Cerrada()
{
  if (servo.read() == 0) //Si el servo está en 0 la puerta esta apagada.
  {
    do
    {
      Serial.println("**PUERTA CERRADA**"); //Print de control
      digitalWrite(ledR, HIGH); //LED rojo ON.
      digitalWrite(ledV, LOW); //LED verde OFF.
      lcd.clear(); //Borra la pantalla LCD y coloca el cursor en la esquina superior izquierda.
      lcd.print("Cerrado"); //Imprime texto en la pantalla LCD.
      colorRojoF(); //Proceso de luz roja fija en la tira led.
      delay(5); //Espera 5ms.
    }
    while (Sensor_Utrasonico() > 150); //Si el sensor ultrasónico detecta algo a 1.5 metros.
  }
}
//--------------------Proceso de puerta cerrando--------------------//
void Puerta_Cerrar()
{
  Serial.println("===CERRANDO PUERTA==="); //Print de control.
  digitalWrite(ledR, HIGH); //LED rojo ON.
  digitalWrite(ledV, LOW); //LED verde OFF.
  lcd.clear(); //Borra la pantalla LCD y coloca el cursor en la esquina superior izquierda.
  lcd.print("Cerrando"); //Imprime texto en la pantalla LCD.
  //Este en un for es de control si el sistema detecta un objeto antes de cerrar la puerta completamente.
  for (angulo  = servo.read() ; angulo >= 0; angulo--) //Decrementa el angulo del servo.
  {
    servo.write(angulo); //escribe el valor del angulo en el servo.
    int pos = angulo; //Se almacena la posición del ángulo.
    colorNaranjaP(); //Proceso de luz rojo fijo en tira led.
    if (Sensor_Utrasonico() <= 150) //Si el sensor ultrasónico detecta algo a 1.5 metros.
    {
      Serial.println("---SENSOR DECTO UN OBJETO---"); //Print de control.
      /*
         Si el sensor ultrasónico detecta algo utiliza la variable pos
         como referencia del ángulo y se abre la puerta nuevamente.
      */
      for (angulo  = pos; angulo <= 180; angulo++) //Incrementa el angulo del servo.
      {
        colorNaranjaP(); //Proceso de luz roja parpadeando en la tira led.
        servo.write(angulo); //Cambia la posición del servo.
        Puerta_Abierta(); //Si el angulo es 180, se llama al proceso de puerta abierta.
        return;
      }
    }
  }
}
//--------------------Proceso de puerta abierta--------------------//
void Puerta_Abierta()
{
  if (servo.read() == 180) //Si el ángulo es igual a 180 la pueta está abierta.
  {
    do  
    {
      Serial.println("***PUERTA ABIERTA***"); //Print de control.
      digitalWrite(ledV, HIGH); //LED verde ON.
      digitalWrite(ledR, LOW); //LED rojo OFF.
      lcd.clear(); //Borra la pantalla LCD y coloca el cursor en la esquina superior izquierda.
      lcd.print("Abierto"); //Imprime texto en la pantalla LCD.
      colorVerdeF(); //Proceso de luz vede fija en la tira led.
      delay(5000);  //Tiempo de espera para que pase la persona.
    }
    while (Sensor_Utrasonico() <= 150); //Si el sensor ultrasónico detecta algo a 1.5 metros.
  }
}
//--------------------Proceso de puerta abriendo--------------------//
void Puerta_Abrir()
{
  Serial.println("...ABRIENDO PUERTA...");
  digitalWrite(ledV, HIGH);//LED verde ON.
  digitalWrite(ledR, LOW); //LED rojo OFF.
  lcd.clear(); //Borra la pantalla LCD y coloca el cursor en la esquina superior izquierda.
  lcd.print("Abriendo"); //Imprime texto en la pantalla LCD.
  for (angulo  = servo.read(); angulo <= 180; angulo++) 
  {
    servo.write(angulo); //Cambia la posición del servo.
    delay(10); //Tiempo para incrementar el ángulo.
    colorAmarilloP(); //Proceso de luz vere parpadeando en la tira led.
  }
}
//--------------------Proceso de cálculo de distancia--------------------//
long Sensor_Utrasonico()
{
  long t;   //Tiempo de demora del ECO en llegar.
  long d;    //distancia en centímetros.

  //Señal del sensor ultrasónico con 1ms del tiempo Tx Rx.
  digitalWrite(trig, HIGH);
  delayMicroseconds(1);
  digitalWrite(trig, LOW);

  t = pulseIn(echo, HIGH); //Tiempo del echo cuando detecta UN Objeto.
  d = t / 58.2; // Calculo de distancia en cm aproximado.

  return d; //Retorna el valor de distancia en cm.
}
//--------------------Proceso de luz roja fija--------------------//
void colorRojoF()
{
  pixels.clear();
  pixels.setBrightness(255); //Brillo del LED
  pixels.fill(pixels.Color(255, 0, 0)); //rojo
  pixels.show();
}
//--------------------Proceso de luz Naranja parpadea--------------------//
void colorNaranjaP()
{
  pixels.clear();
  pixels.setBrightness(255); //Brillo del LED.
  pixels.fill(pixels.Color(255, 140, 100), 0, 4); //Naranja.
  pixels.show();
  delay(10);
  pixels.fill(pixels.Color(0, 0, 0), 0, 4); //OFF
  pixels.show();
}
//--------------------Proceso de luz verde fija--------------------//
void colorVerdeF()
{
  pixels.clear();
  pixels.setBrightness(255); //Brillo del LED.
  pixels.fill(pixels.Color(0, 255, 0), 0, 4); //verde
  pixels.show();
}
//--------------------Proceso de luz amarillo parpadea--------------------//
void colorAmarilloP()
{
  pixels.clear();
  pixels.setBrightness(255); //Brillo del LED.
  pixels.fill(pixels.Color(173, 255, 47), 0, 4); //amarillo
  pixels.show();
  delay(10);
  pixels.fill(pixels.Color(0, 0, 0), 0, 4); //OFF
  pixels.show();
}
