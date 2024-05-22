#ifndef Altura_h
#define Altura_h

#define ALTURA 30

#include "Arduino.h"

class Altura{

    private:
        /* data */
    public:
        Altura(/* args */);
        uint8_t calcularPorcentaje(float altura);
        bool guardarAltura(uint16_t altura);

};

#endif