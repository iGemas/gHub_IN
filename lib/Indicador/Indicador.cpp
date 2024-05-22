#include "Arduino.h"
#include "SoftwareSerial.h"
#include "EEPROM.h"
#include "Indicador.h"



/***************************************************/
/******************Constructor**********************/
/***************************************************/

Indicador::Indicador(){}

void Indicador::begin(SoftwareSerial* puerto, uint16_t direccion){
    /*Propiedades que definen direcciones y tipo de dispositivos*/
    this-> _direccion = direccion;
    this-> _direccionGateway = 1;
    this-> _tipoIndicador = 55;
    this-> _tipoGateway = 50; 
    this-> _tipoIndicadorBroadcast = 0;

    /*Definicion del puerto Serial que utilizará el modulo RF*/
    this->_puerto = puerto;
    (*puerto).begin(9600);

    this->_leds = false;
    this->_updateLeds = 0;

    uint8_t nivel = EEPROM.read(NIVEL);
    uint8_t flujo = EEPROM.read(FLUJO);

    if(nivel != 255){ //Si la EEPROM devuelve 255 quiere decir que ese valor aun no ha sido escrito
        this->_actualizarLeds(nivel, flujo);
    }
}

/***************************************************/
/*****Metodos para actualizar el indicador**********/
/***************************************************/

void Indicador::_actualizarLeds(uint8_t nivel, uint8_t flujo){

    if (nivel == 0)
    {
        this->setLeds(0);
    }
    else if (nivel == 20)
    {
        this->setLeds(20);
    }
    else if (nivel == 40)
    {
        this->setLeds(40);
    }
    else if (nivel == 60)
    {
        this->setLeds(60);
    }
    else if (nivel == 80)
    {
        this->setLeds(80);
    }
    else{
        this->_error(70);
        return;
    }

    this->setUpdateLeds(true);

    EEPROM.write(NIVEL, nivel); //guardando el valor en la memoria EEPROM en caso de corte de luz
    EEPROM.write(FLUJO, flujo);


    this->_ok(70);
}

void Indicador::setLeds(uint8_t leds){ this->_updateLeds = leds; }

uint8_t Indicador::getLeds(){ return this->_updateLeds; }

void Indicador::setUpdateLeds(bool state){ this->_leds = state; }

bool Indicador::getUpdateLeds(){ return this->_leds; }

bool Indicador::getFlujo(){
    return (EEPROM.read(FLUJO) == 1 ? true : false);
}



/***************************************************/
/***************Metodos generales*******************/
/***************************************************/
void Indicador::recibirComando(){
    uint8_t len = 0;

    while((*_puerto).available() > len){
        len = (*_puerto).available();
        delayMicroseconds(1562); //delay entre bits    
    }   

    if(len == 0) return; //en caso de que no haya data devuelve al loop

    uint8_t* buffer = (uint8_t*) malloc(len); //reserva un espacio en memoria para guardar la info del buffer

    for(uint8_t i=0; i< len; i++){buffer[i] = (*_puerto).read();} //lee byte a byte el buffer y lo almacena en buffer

    
    if(buffer[0] != this->_tipoIndicador){ free(buffer); return; } //comprueba si la trama enviada es para dispositivos tipo Bomba1

    uint16_t crcTrama = ((buffer[9+buffer[8]] & 0x00ff)<<8) | (buffer[9+buffer[8]+1] & 0x00ff);
    uint16_t crc = this->_calcularCRC(buffer, 9+buffer[8]);

    if(!(crcTrama == crc)){ free(buffer); return; } //comprueba si el crc de la trama y el calculado son iguales

    uint8_t gDH = (this->_direccionGateway & 0xff00)>>8;
    uint8_t gDL = (this->_direccionGateway & 0x00ff);

    uint8_t dDH= (this->_direccion & 0xff00)>>8;
    uint8_t dDL= (this->_direccion & 0x00ff);

    if(!(buffer[1] == dDH || buffer[1]== this->_tipoIndicadorBroadcast)){ free(buffer); return; } //comprueba si la direccion del receptor enviada es la del dispositivo o la de tipoBomba1Broadcast
    if(!(buffer[2] == dDL || buffer[2]== this->_tipoIndicadorBroadcast)){ free(buffer); return; }

    if(!(buffer[3] == gDH)){ free(buffer); return; } //comprueba si la trama ha sido enviada por el gateway
    if(!(buffer[4] == gDL)){ free(buffer); return; }


    /*******************DEBUG*********************/
    Serial.println("\nData de entrada: ");
    for (uint8_t i = 0; i < len; i++)
    {
        Serial.print(buffer[i]);
        Serial.print("-");
    }

    switch(buffer[7]){
        
        case 70://comando para actualizar el indicador de nivel LVS
            this->_actualizarLeds(buffer[9], buffer[10]);
        break;

        /*case 71:{//comando para actualizar indicador de nivel LVH
            uint8_t parte_entera = buffer[9];
            uint8_t parte_decimal = buffer[10];
            float altura = (float)parte_entera + ((float) parte_decimal) / 100.0;

            uint8_t porcentaje = this->calcularPorcentaje(altura);
            this->_actualizarLeds(porcentaje, false);

            this->_ok(71);

            break;
        }

        case 72:{//comando para guardar la altura del tanque

            uint16_t altura = ((buffer[9] & 0x00FF) << 8) | buffer[10];

            this->guardarAltura(altura);

            this->_ok(72);

            break;
        }*/

        case 80://comando para enviar el estado de funcionamiento del dispositivo
            this->_ok(80);
        break;

        default:
            this->_error(0);
        break;
    }

    free(buffer);
}

void Indicador::_enviarMensaje(uint16_t memoria, int8_t funcion, uint8_t len, uint8_t* data){

    //delay para evitar el entrelazamiento de tramas
   delay(200);

    uint8_t* trama = (uint8_t*) malloc(9+len); //espacio en memoria reservado para contener la trama
    trama[0]=this->_tipoGateway;
    trama[1]=(this->_direccionGateway & 0xff00)>>8;
    trama[2]=(this->_direccionGateway & 0x00ff);
    trama[3]=(this->_direccion & 0xff00)>>8;
    trama[4]=(this->_direccion & 0x00ff);
    trama[5]=((memoria & 0xff00)>>8);
    trama[6]=(memoria & 0x00ff);
    trama[7]=funcion;
    trama[8]=len;
    for (uint8_t i = 9; i <(9 + len); i++)
    {
        trama[i] = data[i-9];
    }

    uint16_t crc = this->_calcularCRC(trama, 9+len); //calcula el crc de la trama 
    uint8_t crcHi = ((crc & 0xff00) >>8); //parte alta del crc
    uint8_t crcLow = (crc & 0x00ff); //parte baja del crc

    uint8_t gDH = (this->_direccionGateway & 0xff00)>>8;
    uint8_t gDL = (this->_direccionGateway & 0x00ff);

    uint8_t dDH= (this->_direccion & 0xff00)>>8;
    uint8_t dDL= (this->_direccion & 0x00ff);

    uint8_t msb = ((memoria & 0xff00)>> 8); //parte alta dela memoria
    uint8_t lsb = (memoria & 0x00ff); //parte baja de la memoria

    Serial.println("\nData de salida");
    for (uint8_t i = 0; i < 9; i++)
    {
        Serial.print(trama[i]);
        Serial.print("-");
    }
    for (uint8_t i = 0; i < len; i++)
    {
        Serial.print(data[i]);
        Serial.print("-");
    }
    Serial.print(crcHi);
    Serial.print("-");
    Serial.print(crcLow);
    Serial.print("&");
    
    (*_puerto).write(this->_tipoGateway);
    (*_puerto).write(gDH);
    (*_puerto).write(gDL);
    (*_puerto).write(dDH);
    (*_puerto).write(dDL);
    (*_puerto).write(msb);
    (*_puerto).write(lsb);
    (*_puerto).write(funcion);
    (*_puerto).write(len);
    for (uint8_t i = 0; i < len; i++)
    {
        (*_puerto).write(data[i]);
    }
    (*_puerto).write(crcHi);
    (*_puerto).write(crcLow);

    free(trama); //libera la memoria reservada para el array trama
    
}

uint16_t Indicador::_calcularCRC(uint8_t* trama, uint8_t len){

    uint16_t temp;
    uint8_t flag, temp2;

    temp = 0xFFFF;
    for (uint8_t i = 0; i < len; i++)
    {
        temp = temp ^ trama[i];
        for (uint8_t j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>=1;

            if (flag) temp ^= 0xA001;
        }
    }
    // Orden de byte inverso.
    temp2 = ((temp & 0xff00) >> 8);
    temp = ((temp & 0x00ff ) << 8) | temp2;
    temp &= 0xFFFF;
    // el valor devuelto ya está intercambiado
    // El byte crcLo es el primero y el byte crcHi es el último
    return temp;
}

void Indicador::_error(uint8_t funcion){
    uint8_t* aux = (uint8_t*) malloc(1);
    aux[0] = funcion;

    this->_enviarMensaje(0x0000, 254, 0x01, aux);

    free(aux);
}

void Indicador::_ok(uint8_t funcion){
    uint8_t* aux = (uint8_t*) malloc(1);
    aux[0] = funcion;

    this->_enviarMensaje(0x0000, 255, 0x01, aux);

    free(aux);
} 

/***************************************************/
/*Metodos gepara trabajar con sensores hidroestatios*/
/***************************************************/

uint8_t Indicador::calcularPorcentaje(float altura){

    uint8_t porcentaje = 0;

    uint16_t altura_del_tanque = 1;
    EEPROM.get(ALTURA, altura_del_tanque);

    uint16_t altura_medida = altura * 100.0;

    porcentaje = ((float)altura_medida / (float)altura_del_tanque) * 100.0;

    Serial.print("Altura guardada: ");
    Serial.println(altura_del_tanque);
    Serial.print("Altura enviada por el hub: ");
    Serial.println(altura_medida);
    Serial.print("Porcentaje: ");
    Serial.println(porcentaje);


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

bool Indicador::guardarAltura(uint16_t altura){

    EEPROM.put(ALTURA, altura);

    return true;
}