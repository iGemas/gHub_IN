#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Indicador.h>
#include <Adafruit_NeoPixel.h>

/*Definicion de los pines que serán usados por SoftwareSerial*/
#define rx 2
#define tx 3

/*Instanciación de la clases*/
SoftwareSerial mySerial(rx, tx);
Indicador id;


//Variables y constantes para el seteo de la direccion por puerto serial
#define memoria_set 0
#define posicion_de_la_direccion 1
String contrasegna = "turing1234";
bool habilitador = false, habilitador2 = false;
uint16_t direccion = 0;

void introducir_contrasena(){
  while(true){
        Serial.print("\nIntroduce la contraseña y presiona enter: \n>>>");

        while(true){
          if(Serial.available() > 0){
            String pass = Serial.readStringUntil('\n');

            if(pass != contrasegna){
              Serial.print("\nContraseña incorrecta");
              break;
            }
            else
              habilitador = true;
          }

          if(habilitador){
            Serial.print("\nIntroduce la dirección y presiona enter: \n>>>");

            while (true)
            {
              if (Serial.available() > 0 )
              {
                String dir = Serial.readStringUntil('\n');

                //se indica que ha sido seteada la direccion
                EEPROM.write(memoria_set, 100);

                //se guarda la direccion en el espacio de memeoria designado
                uint16_t guardar_dir = dir.toInt();
                EEPROM.put(posicion_de_la_direccion, guardar_dir);

                habilitador2 = true;
                break;
              }
              
            }

            break;
            
          }
        }

        if(habilitador2)
          break;
  }
}

void seteado_de_direccion(){

  bool direccion_seteada = EEPROM.read(memoria_set) == 100 ? true: false;

    if(!direccion_seteada){
      introducir_contrasena();
    }
}

void cambio_de_direccion(){
  Serial.println("¿Quieres cambiar la dirección? escribe \"s\" y presiona enter en caso afirmativo");
  Serial.print("Tienes 10 segundos para hacerlo\n>>>");

  uint32_t tiempo = millis();

  while (true)
  {
    if((millis() - tiempo) < 10000){
      if(Serial.available() > 0){
        String afirmativo = Serial.readStringUntil('\n');

        if(afirmativo == "s"){
          introducir_contrasena();
          break;
        }
        else{
          break;
        }
      }
    }
    else{
      Serial.println("\nEl tiempo para cambiar la dirección ha concluido\nInicio del dispositivo");
      break;
    }
  }
  


}

void manejo_de_direccion(){
  seteado_de_direccion();

  habilitador = false;
  habilitador2 = false;

  cambio_de_direccion();
}

#define PIN_DE_FLUJO 4
#define PIN 7 
#define NUMPIXELS 6 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  while(!Serial){}

  pinMode(PIN_DE_FLUJO, OUTPUT);
  digitalWrite(PIN_DE_FLUJO, LOW);

  Serial.println("Inicio del setup");

  //se hace el seteo o cambio de la direccion
  manejo_de_direccion();

  //se obtiene la direccion del dispositivo
  EEPROM.get(posicion_de_la_direccion, direccion);

  Serial.print("La dirección del dispositivo es: ");
  Serial.println(direccion);

  //Se inicializan los objetos
  id.begin(&mySerial, direccion);

  pixels.begin();
  pixels.setBrightness(25);

  Serial.println("Final del setup");

}

void loop() {
  id.recibirComando();

  if(id.getUpdateLeds()){

    digitalWrite(PIN_DE_FLUJO, id.getFlujo() ? HIGH : LOW);

    if(id.getLeds() == 0){

      pixels.clear();

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      delay(20);

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      pixels.show();

    }
    else if(id.getLeds() == 20){
      
      pixels.clear();

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      delay(20);

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      pixels.show();
    }

    else if(id.getLeds() == 40){

      pixels.clear();

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      delay(20);

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      pixels.show();
    }

    else if(id.getLeds() == 60){
      
      pixels.clear();

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(3, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      delay(20);

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(3, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      pixels.show();
    }

    else if(id.getLeds() == 80){

      pixels.clear();

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(3, pixels.Color(0, 0, 255));
      pixels.setPixelColor(4, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      delay(20);

      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.setPixelColor(1, pixels.Color(0, 0, 255));
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.setPixelColor(3, pixels.Color(0, 0, 255));
      pixels.setPixelColor(4, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));

      pixels.show();
    }

    id.setUpdateLeds(false);
  }
}