# INDICADOR DE NIVEL

Dispositivo capaz de mostrar el porcentaje en que se encuentra un tanque asociado bien sea a un dispositivo medidor simple o con sensor hidroestático.

> [!IMPORTANT]
> La alimentación del dispositivo debe ser de 12V DC y al menos 1A

## COMANDOS PARA INTERACTUAR CON UN DISPOSITIVO INDICADOR DE NIVEL DESDE EL SERVIDOR

> [!CAUTION]
> El formato de los comando debe ser tal cual como se indica a continuación. De no ser así, el dispositivo puede brindar respuesta incoherentes o mal funcionar.

### IN_ACTUALIZAR_NIVEL
> 📖 DESCRIPCIóN

Permite cambiar el porcentaje que muestra el indicador de nivel

> ⚠️ FORMATO DEL COMANDO

IN_ACTUALIZAR_NIVEL=30 NIVEL=20;

> ✅ RESPUESTA EN CASO DE EXITO

```json
{
    "h": "mac address del hub",
    "f": "timestamp",
    "c": "IN_ACTUALIZAR_NIVEL",
    "r": true,
    "d": 30,
    "i": [{
        "nivel" : 20
    }]
}
```

> ❌ RESPUESTA EN CASO DE FALLO

```json
{
    "h": "mac address del hub",
    "f": "timestamp",
    "c": "IN_ACTUALIZAR_NIVEL",
    "r": false,
    "d": 30,
    "i": [{
        "nivel" : 20
    }]
}
```