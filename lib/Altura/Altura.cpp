#include "Arduino.h"
#include "EEPROM.h"
#include "Altura.h"

Altura::Altura(){}

uint8_t Altura::calcularPorcentaje(float altura){

    uint8_t porcentaje = 0;

    uint16_t altura_del_tanque = 0;
    EEPROM.get(ALTURA, altura_del_tanque);

    uint16_t altura_medida = altura * 100.0;

    porcentaje = (altura_medida / altura_del_tanque) * 100;

    if(porcentaje <= 20){
        return 0;
    }
    else if(porcentaje > 20 && porcentaje <= 40){
        return 20;
    }
    else if(porcentaje > 40 && porcentaje <= 60){
        return 40;
    }
    else if(porcentaje > 60 && porcentaje <= 80){
        return 60;
    }
    else if(porcentaje > 80){
        return 80;
    }
}

bool Altura::guardarAltura(uint16_t altura){

    EEPROM.put(ALTURA, altura);

    return true;
}