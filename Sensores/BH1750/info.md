Las características del sensor **BH1750** son las siguientes:
- Interfaz I2C con dos posibles direcciones. 0x5C y 0x23 (por defecto).
- Representación del espectro apróximadamente a la del ojo humano.
- Alta resolución, (1 - 65535 lx) 
```
lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
```
En este modo el sensor mide una vez y automaticamente entra en el modo apagado.
O:
```
lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
```
En este modo el sensor mide continuamente.

- Baja dependencia al tipo de luz. (Incandescente, fluorescente, halógena, LED, sol)
- Resutado de medición ajustable teniendo en cuenta el "cascarón" que cubre el sensor. (Es posible detectar un mínimo de 0,11 lx, un máximo de 100000 lx utilizando esta función.)
- ±20% de error
- Baja influencia de luz infrarroja.

Especificaciones:
- Voltaje
  - mín 2,4V
  - max 3,6V
- Resolución 4lux, 1lux, 0,5lux. Usando la resolución de 1lux permite distinguir iluminaciones por debajo de los 10lux (luz crepúscular) Para 1 lux y 4 lux se usan los 16bits de datos, llegando a los 65535 lux (día soleado sin luz directa) En el modo 0,5 lux usa 15 bits y puede representar un valor máximo de 32767 lux (exterior sin luz directa) ![Fuente](http://polaridad.es/bh1750-luz-sensor-iluminacion-ambiental-i2c-medida-luminosidad-medicion/)

Si queremos usar la dirección I2C 0x5C tenemos que conectar el pin ADDR a un voltaje mayor de 0,7V, por ejemplo al pin de 3,3V que alimenta al sensor. Si el voltaje del pin ADDR es menor a 0,7V, la dirección será 0x23.
