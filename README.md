# # Hoja de ruta - Estación metereológica con NodeMcu.

_Modesto proyecto para afianzar conocimientos adquiridos en varios ámbitos._
_Este proyecto está basado en la colaboración de todas esas personas que comparten desisteresadamente sus trabajos. Está modificado para conseguir lo deseado._
_En algun lugar de este readme.me intentaré añadir todos los enlaces, gracias a los cuales, espero conseguir realizar la estación meteorológica._

Descripción del proyecto
_La estación medirá la temperatura, humedad, presión, tanto interior como exterior, rayos UV, velocidad del viento y lluvia. Los datos se visualizarán en un dashboard creado 
en la plataforma de gestión de datos Thinger.io además de en una pantalla Nextion situada dentro del hogar.


## Material necesario 🚀

_El material utilizado se puede modificar pero se tendrá que hacer lo propio con el código._

**NodeMCU**

![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/pictures/NodeMCUv3.jpg?raw=true)

![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/pictures/NodeMCU-pines.png?raw=true)

Poco que añadir a esta placa. http://nodemcu.com/index_en.html. Basada en el ESP8266.

Tiene un puerto micro USB

Conversor Serie-USB, el CH340G

Tiene pines, un LED y dos botones, uno para reinicio y otro para flasheo.

Se alimenta a 3V.

3 salidas de 3V

1 de 5 (Solo da 5V si en las otras no hay nada conectado)

La v3 que yo tengo, solo se puede usar a 9600 bps Al establecer conexión mediante el puerto serie, *Serial.begin (9600);*


**SENSORES**

**BME280** 	

![alt text](https://github.com/RaulMallorca/Estacion_metereologica/blob/pictures/BMP280.jpg?raw=true)

            Temperatura (rango de -40 a + 85 °C, precisión ±1 °C y resolución 0,01 °C)

          	Humedad (a 100%, con una precisión de ±3% Pa y una resolución de 0.008%)
            
          	Presión (300-1100 hPa, precisión de ±1 Pa, y resolución de 0,18 Pa)
            

	Current consumption 1.8 μA a 1Hz - Humedad y temperatura
                    2.8 μA a 1Hz - Presión y temperatura
                    3.6 μA a 1Hz - Humedad, presión y temperatura
                    0.1 μA in sleep mode
		        V 3,3V
            
**LLUVIA**

**VELOCIDAD DEL VIENTO**

**UV**

**Pantalla Nextion NX8048P070-011C-Y**

Pantalla táctil TFT capacitiva colores reales RGB 65K.

Memoria Flash de 128MB para código de aplicación de usuario y datos.

EEPROM: 1024 bytes

RAM: 512 KB

Interfaz: interfaz serie XH2.54 4 pin TTL

Voltaje de entrada: 4,7-7 V (estándar de 5 V)

Corriente de entrada: 750mA (brillo máximo), 170mA (modo de suspensión). Potencia recomendada: 5V mínimo 1A

Altavoz: 0,5 vatios de capacidad nominal, impedancia de 16 Ω

Tamaño de la pantalla: 7,0 pulgadas

Resolución de la pantalla: 800x480 píxeles

Brillo de la pantalla: 300 cd/m ^ 2,(0 ~ 300 nit, el intervalo de ajuste es 1%)

Ranura para tarjeta micro SD integrada, FAT32. Adecuado para una tarjeta SD de hasta 32GB

Fuente de alimentación Micro USB macho.

Visual: 154,08mm(L) x 85,92mm(W)

Dimensiones: 218,1x150x22,5mm.

**Cajas**

Una para guardar los sensores y el NodeMCU exterior.

Una para guardar los sensores y el NodeMCU interior.

Una para la pantalla Nextion

### Pre-requisitos 📋

_Software, drivers y extras a instalar_

Un IDE para crear el skecht. Yo he usado el IDE de Arduino, concretamente el 1.8.13. https://www.arduino.cc/en/pmwiki.php?n=main/software.

Drivers para el conversor Serie-USB CH340G. Lo puedes encontrar fácilmente por internet.

### Instalación 🔧

_Una serie de ejemplos paso a paso que te dice lo que debes ejecutar para tener un entorno de desarrollo ejecutandose_

_Dí cómo será ese paso_

```
Da un ejemplo
```

_Y repite_

```
hasta finalizar
```

_Finaliza con un ejemplo de cómo obtener datos del sistema o como usarlos para una pequeña demo_

## Ejecutando las pruebas ⚙️

_Explica como ejecutar las pruebas automatizadas para este sistema_

### Analice las pruebas end-to-end 🔩

_Explica que verifican estas pruebas y por qué_

```
Da un ejemplo
```

### Y las pruebas de estilo de codificación ⌨️

_Explica que verifican estas pruebas y por qué_
```
Da un ejemplo
```

## Despliegue 📦

_Agrega notas adicionales sobre como hacer deploy_

## Construido con 🛠️

_Menciona las herramientas que utilizaste para crear tu proyecto_

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - El framework web usado
* [Maven](https://maven.apache.org/) - Manejador de dependencias
* [ROME](https://rometools.github.io/rome/) - Usado para generar RSS

## Contribuyendo 🖇️

Por favor lee el [CONTRIBUTING.md](https://gist.github.com/villanuevand/xxxxxx) para detalles de nuestro código de conducta, y el proceso para enviarnos pull requests.

## Wiki 📖

Puedes encontrar mucho más de cómo utilizar este proyecto en nuestra [Wiki](https://github.com/tu/proyecto/wiki)

## Versionado 📌

Usamos [SemVer](http://semver.org/) para el versionado. Para todas las versiones disponibles, mira los [tags en este repositorio](https://github.com/tu/proyecto/tags).

## Autores ✒️

_Menciona a todos aquellos que ayudaron a levantar el proyecto desde sus inicios_

* **Andrés Villanueva** - *Trabajo Inicial* - [villanuevand](https://github.com/villanuevand)
* **Fulanito Detal** - *Documentación* - [fulanitodetal](#fulanito-de-tal)

También puedes mirar la lista de todos los [contribuyentes](https://github.com/your/project/contributors) quíenes han participado en este proyecto. 

## Licencia 📄

Este proyecto está bajo la Licencia (Tu Licencia) - mira el archivo [LICENSE.md](LICENSE.md) para detalles

## Expresiones de Gratitud 🎁

* Comenta a otros sobre este proyecto 📢
* Invita una cerveza 🍺 o un café ☕ a alguien del equipo. 
* Da las gracias públicamente 🤓.
* etc.



---
⌨️ con ❤️ por [Villanuevand](https://github.com/Villanuevand) 😊
