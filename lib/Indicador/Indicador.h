#ifndef Indicador_h
#define Indicador_h


#define LED_0 0
#define LED_1 1
#define LED_2 2
#define LED_3 3
#define LED_4 4
#define LED_FUNC 5
#define RED 255
#define BLUE 255
#define GREEN 255
#define OFF 0
#define NIVEL 20
#define FLUJO 22
#define BRILLO 25

#define ALTURA 30


#include "Arduino.h"
#include "Altura.h"

class Indicador
{
    private:
        /*Propiedades de direccion y tipo de dispositivo*/
        uint16_t _direccion;
        uint16_t _direccionGateway;
        uint8_t _tipoIndicador;
        uint8_t _tipoGateway; 
        uint8_t _tipoIndicadorBroadcast;
        Stream* _puerto;

        /*Metodos privados generales*/
        uint16_t _calcularCRC(uint8_t* trama, uint8_t len);
        void _enviarMensaje(uint16_t memoria, int8_t funcion, uint8_t len, uint8_t* data);
        void _error(uint8_t funcion);
        void _ok(uint8_t funcion);

        bool _leds;
        uint8_t _updateLeds;

        /*Metodo para actualizar el indicador de leds*/
        void _actualizarLeds(uint8_t nivel, uint8_t flujo);
        

    public:
        Indicador();
        void begin(SoftwareSerial* puerto, uint16_t direccion);
        void recibirComando();

        void setLeds(uint8_t leds);
        uint8_t getLeds();
        void setUpdateLeds(bool state);
        bool getUpdateLeds();
        bool getFlujo();

        uint8_t calcularPorcentaje(float altura);
        bool guardarAltura(uint16_t altura);

};

#endif