Las especificciones del sensor **BME280**, no confundirlo con el **BMP280** que no mide la humedad, son las siguientes:

1. Resolución humedad 0.008 %RH
  - Tolerancia precisión absoluta ± 3 %RH
  
1. Resolución presión 0.18 Pa 
  - Rango de medición 300 a 1100 hPa
  - Tolerancia precisión absoluta 
    - 300 ... 1100hPa y -20 ºC ...0 ºC ±1.7 hPa
    - 300 ... 1100hPa y 0 ºC ... 65 ºC ±1 hPa
    - 300 ... 1100hPa y 25 ºC... 40 ºC ±1.5 hPa
  
1. Resolución temperatura 0.01 ºC
  - Tolerancia precisión absoluta 
    - 0 ºC ... 65 ºC ± 1ºC
    - -20 ºC ... 0 ºC ±1.25 ºC
    - -40 ºC ... -20 ºC ±1.5 ºC

El resultado obtenido es en Pascales. 
- **Equivalencias**
  - 1 bares = 100000 Pascales
  - 1 hPa = 100 Pascales
  - 1 mmHg = 133,32 Pascales
  - 1 hPa = 0.75 mmHg

En el código la unidad está combertida a hPa de la siguiente manera:

> **_(bme.readPressure() / 100.0F);_**

Un pequeño problema que me surgió a la hora de conseguir comunicación con el módulo del sensor BME280 fue el no haber tenido en cuenta que venía sin el puente de unión para elegir la dirección I2C. No conseguí encontrar referencias sobre las direcciones hasta que visité la magnífica página de [patagoniatec.com](http://patagoniatec.com) donde detallaban este tema en concreto.

Si hacéis la soldadura, no habrá mayor problema. Como a día de hoy, 24/12/2020 sigo esperando más sensores que usarán el protocolo I2C, no he querido hacer el puente para después que coincidiera alguna dirección con otro sensor y tuviera que desoldar. 

La manera rápida y limpia de corregir esto es mediante programación.

Cuando iniciamos el sensor en void setup() con

>**_bme.begin();_**

Dentro del paréntesis debemos indicarle la dirección elegida, 0x76 o 0x77.

>**_bme.begin(0x76);_**

![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/master/Sensores/BME280/20201224_064219.jpg)

La conexión con el nodeMCU no tiene complicación. Unimos el pin D1 del nodeMCU con el pin SCL y el pin D2 con el pin SDA del módulo. Compartimos masas y alimentamos el sensor con 3,3V o 5V, dependiendo si hemos comprado un módulo con regulador de tensión o sin él, en mi caso, compré ambos y terminé montando el de 3,3V. La placa del nodeMCU la alimentamos desde el USB.

![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/master/Sensores/BME280/BME280-NodeMCU.png)

La batería del esquema es orientativa, para diferenciar la alimentación del módulo del sensor de la placa donde está alojado el 8266.
