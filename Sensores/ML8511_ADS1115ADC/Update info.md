Las especificaciones del sensor **ML8511 UV** son las siguientes:

- Fotodiodo sensible a la radiación UV-A y UV-B.
- Salida analógica.

- Voltaje  
  - min 2,7V
  - max 3,6V

- Consumo
  - Standby 0,1µA
  - Bajo consumo 300µA
  
[Fuente1](https://learn.sparkfun.com/tutorials/ml8511-uv-sensor-hookup-guide/all)
  
Las especificiaciones del ADC ADS1115 son las siguientes:

- Interfaz I2C, seleccionable mediante pin. 4 direcciones, 0x48, 0x49, 0x4A y 0x4B. [Fuente2](https://programarfacil.com/blog/arduino-blog/ads1115-convertidor-analogico-digital-adc-arduino-esp8266/)
- Resolución de 16bits.
- 4 pines analógicos.
- Amplificador de ganancia programable, **PGA**, mayor precisión y fiabilidad a través del código.
- Voltaje
  - min 2V
  - max 5,5V
- Consumo
  - Modo single-shot auto apagado.
  - Continuo 150µA
  
- VDD a GND –0,3V to +5,5 V
- Corriente entrada analógica 100mA momentary 
- Corriente de entrada analógica 10mA continuous

- **Voltaje entrada analógica  a GND –0,3V a VDD + 0,3V**
- SDA, SCL, ADDR, ALERT/RDY voltaje a GND –0,5V a +5,5V

 ![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/master/Sensores/ML8511_ADS1115ADC/ADS1115_ADC.png)
 
 
