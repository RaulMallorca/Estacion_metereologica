/*
  Lectura de datos de temperatura, humedad, altura aproximada y presión con sensor BME280. Luminosidad con BH1750, Anemómetro, veleta y pluviómetro, índice UV con ML8511 con la placa
  NodeMCU. Monitorización en la plataforma www.thinger.io.
  Envío de email según declaremos en IFTTT

  Código parcial de:
  Dani No www.esploradores.com

*/

#include <ESP8266WiFi.h>       //Librería de conexión WiFi del módulo ESP8266
#include <ThingerESP8266.h>    //Librería de la plataforma thinger.io
#include <Wire.h>              // Librería para el sensor BME280, BH1750 y para el ADS1115
#include <Adafruit_ADS1015.h>  //Librería para el ADS1115
#include <SPI.h>               // Librería para el sensor BME280
#include <Adafruit_Sensor.h>  // Librería para el sensor BME280
#include <Adafruit_BME280.h>  // Librería para el sensor BME280
#include <ArduinoOTA.h>       //Librería necesaria para el uso de OTA
#include "ESP8266_Utils_OTA.hpp"  //Configuración para uso de OTA
#include <BH1750.h>           //Librería para el sensor BH1750.

#define presion_Nivel_Mar_HPA (1013.25)  //Presión a nivel del mar

//============================== Parámetros de la conexión con Thinger.io
#define usuario "EstacionMeteo" //Nuestro usuario de Thinger.io
#define device_Id "Servidor_Meteo" //Poner el device_ID elegido en Thinger.io
#define device_credentials "A+aKTL!K2!Dv" //Poner nuestra device_credencials de Thinger.io

ThingerESP8266 thing(usuario, device_Id, device_credentials);

#define EMAIL_ENDPOINT "Prueba_IFTT"  //El endpoint se usa para integrar la plataforma thinger.io con servicios externos, en este caso, recibir notificación por email.

//============================== Parámetros de conexión WiFi
const char WiFi_ssid[] = "RedWifi";             //Nombre de red
const char WiFi_password[] = "KirguisT@N2035";  //Clave de red

Adafruit_BME280 bme; // BME280 conectado por I2C
Adafruit_ADS1115 ads; //Creamos un objeto de la clase
BH1750 lightMeter(0x23);  //Dirección I2C

//============================== Variables de tiempo
unsigned long currentMillis;
unsigned long previousMillis_Lecturas = 0;
unsigned long previousMillis_Aviso = 0;
unsigned long intervalo_entre_Lecturas = 0;
unsigned long intervalo_Aviso = 0;

//============================== Variables de los sensores
float T_out;
float H_out;
float P_out;
float Altitud;
float Lumi;
float indiceUV;

//============================== Variables punto_Rocio
double raiz1;
double raiz2;
double Pun_rocio;

//============================== Variables para indicar si ya se realizo un aviso a la plataforma Thinger.io y no volver a hacerlo
bool Aviso = true;
byte lux_Limite = 200;

int Luminosidad; //Para activar el endpoint y recibir un email al guardar el valor de "luminosidad"

/*--------------FUNCIÓN PARA MOSTRAR POR EL MONITOR SERIE INFO WIFI--------------*/
void mostrar_info_Red ()
{
  //============================== Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  Serial.print ("RSSI =\t");
  Serial.println(WiFi.RSSI());
  Serial.print("hostname:\t");
  Serial.println(WiFi.hostname());
  Serial.print("subnetMask:\t");
  Serial.println(WiFi.subnetMask());
  Serial.print("Puerta de enlace:\t");
  Serial.println(WiFi.gatewayIP());
  Serial.print("IP DNS:\t");
  Serial.println(WiFi.dnsIP());
  Serial.print("macAddress:\t");
  Serial.println(WiFi.macAddress());

}

/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*------------------FUNCIÓN PARA MOSTRAR POR EL MONITOR SERIE LAS LECTURAS Y ACTIVAR EL AVISO DE LUMINOSIDAD-------------*/
void leer_Sensores ()
{
  //==============================Mostrar los datos de presión, temperatura, altitud, luminosidad, RSSI, punto rocío, índice UV y humedad por el monitor serie.
  Serial.print ("Temperatura: ");
  Serial.print (T_out);
  Serial.println (" ºC");
  Serial.print ("Humedad: ");
  Serial.print (H_out);
  Serial.println ("%");
  Serial.print ("Presión: ");
  Serial.print (P_out);
  Serial.println (" hPa");
  Serial.print ("Altitud aproximada = ");
  Serial.print (Altitud);
  Serial.println (" m");
  Serial.print ("RSSI servidor: ");
  Serial.print (WiFi.RSSI());
  Serial.println("dBm");
  Serial.print ("Luminosidad: ");
  Serial.print (Lumi);
  Serial.println (" lux");
  Serial.print ("El punto de rocío es: ");
  Serial.print (Pun_rocio);
  Serial.println (" ºC");
  Serial.print ("El índice UV es: ");
  Serial.print (indiceUV);
  Serial.println ("mW/cm^2");
}

/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*----------------------- FUNCIÓN PARA MAPEAR UNA FLOAT-----------------------*/
                          /*Para hallar el índice UV*/
                /*////////////// Gracias al siguiente post \\\\\\\\\\\\\\\*/
             /* From: http://forum.arduino.cc/index.php?topic=3922.0 */
float mapfloat (float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*-------------------------------FUNCIÓN PARA AVISOS THINGER.IO-------------------------------*/
void avisos_Thinger ()
{
  //Usamos la condición if para llamar el endpoint si la luminosidad supera al valor definido
  if (Luminosidad < lux_Limite) {
    Aviso = true;
    Serial.println("Avisos activados para luminosidad");
  }
  Serial.println (" ");
}
/*------------------------------------------FIN FUNCIÓN----------------------------------------*/

void setup()
{
  Serial.begin (9600);
  delay(200);
  Wire.begin(D2, D1); //Iniciamos el sensor BH1750
  ads.begin();  //Iniciamos el sensor ML8511

  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE); //Modo alta resolución "ONE TIME"
  while (!Serial);   // time to get serial running

  //============================== default settings
  unsigned status;
  status = bme.begin(0x76);
  //============================== You can also pass in a Wire library object like &Wire2
  //============================== status = bme.begin(0x76, &Wire2)
  if (!status) {
    Serial.println("¡¡No se encuentra el sensor BME280, comprueba conexiones, dirección y la ID del sensor!!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probablemente la dirección está mal o el sensor es un BMP 180 o BMP 085\n");
    Serial.print("   ID of 0x56-0x58 representa un BMP 280,\n");
    Serial.print("        ID of 0x60 representa un BME 280.\n");
    Serial.print("        ID of 0x61 representa un BME 680.\n");
    while (1) delay(10);
  }

  //============================== Iniciando wifi para comunicarse con Thinger.io
  thing.add_wifi(WiFi_ssid, WiFi_password); // Inicialización de la WiFi para comunicarse con la API
  Serial.println(" ");
  Serial.println ("Comunicando con Thinger.io");
  Serial.println (" ");

  //============================== Leemos los datos de los sensores
  T_out = bme.readTemperature ();
  H_out = bme.readHumidity ();
  P_out = (bme.readPressure() / 100.0F);
  Altitud = bme.readAltitude (presion_Nivel_Mar_HPA);
  Lumi = lightMeter.readLightLevel();

  //============================== Inicialización de la lectura de datos desde la API
  //============================== El tipo de datos pson puede contener diferentes tipos de datos, se usa para recibir múltiples valores al mismo tiempo.
  thing["sensores"] >> [](pson & out) { //Nombre del recurso que elegiremos en el bucket de Thinger.io
    out["Temperatura"] = T_out; //Nombre del campo para trazar asociado a la temperatura.
    out["Humedad"] = H_out;  //Nombre del campo para trazar asociado a la humedad.
    out["Punto de rocio"] = Pun_rocio;  //Nombre del campo para trazar asociado al punto de rocío.
    out["Presión"] = P_out; //Nombre del campo para trazar asociado a la presión.
    out["Altitud aproximada"] = Altitud; //Nombre del campo para trazar asociado a la altitud aproximada.
    out["Luminosidad"] = Lumi; //Nombre del campo para trazar asociado a la luminosidad de la ldr.
    out["Indice UV"] = indiceUV;  //Nombre del campo para trazar asociado al índice ultravioleta.
    out["RSSI out"] = WiFi.RSSI(); //Nombre del campo para trazar asociado a la señal wifi de la estación exterior.
  };

  //============================== Creamos los recursos DE ENTRADA para la API
  thing["ldr_Lim"] << inputValue(lux_Limite, {  // se ejecuta cuando el valor cambia
    Serial.print("Desde Thinger.io ha cambiado el valor. El nuevo valor de lux_Limite es: ");
    Serial.println(lux_Limite);
  });

  //============================== Temporizaciones para medir y para enviar avisos
  intervalo_entre_Lecturas = 60000; // 60 segundos
  intervalo_Aviso = 70000;  //70 segundos

  //============================== Primer estado
  previousMillis_Lecturas = millis();
  previousMillis_Aviso = millis();

  InitOTA();  //Ajustes como puerto, password, etc (ESP8266_Utils_OTA.hpp)
}

void loop()
{
  thing.handle();

  currentMillis = millis();

  if (currentMillis - previousMillis_Lecturas > intervalo_entre_Lecturas) //Si la resta es > que intervalo_entre_Lecturas, vuelve a leer los sensores
  {
    previousMillis_Lecturas = currentMillis;

    //============================== Leemos los datos de los sensores
    T_out = bme.readTemperature ();
    H_out = bme.readHumidity ();
    P_out = (bme.readPressure() / 100.0F);
    Altitud = bme.readAltitude (presion_Nivel_Mar_HPA);
    Lumi = lightMeter.readLightLevel();

    //============================== Calculamos el punto de rocío
    raiz1 = pow (H_out, 1.0 / 8);
    raiz2 = pow (100, 1.0 / 8);
    double div_raiz = raiz1 / raiz2;
    Pun_rocio = div_raiz * (110 + T_out) - 110;

    //============================== Calculamos el índiceUV
    int refLevel = ads.readADC_SingleEnded(0);  //Lectura del pin A0 del ADS1115 que está conectado al pin 3.3V y EN del ML8511
    int uvLevel = ads.readADC_SingleEnded(1);   //Lectura del pin A1 del ADS1115 que está conectado al pin OUT del ML8511

    float voltajeSalida = 3.3 / refLevel * uvLevel; //Dividimos el voltaje de entrada por la lectura del pin OUT y multiplicamos
    //el resultado por la lectura del pin 3.3V y EN
    indiceUV = mapfloat (voltajeSalida, 0.99, 2.8, 0.0, 15.0);  //Mapeamos para hallar el índice UV

    //============================== Mostramos el valor de los sensores por el monitor serie. Se puede comentar una vez funcione.
    leer_Sensores ();

    //============================== Envía los datos a Thinger.io
    thing.stream (thing["sensores"]);

  }

  //============================== Temporizacion para el aviso
  if (currentMillis - previousMillis_Aviso > intervalo_Aviso)   //Si la resta es > que el intervalo_Aviso, vuelve a enviar el e-mail de aviso.
  { 
    previousMillis_Aviso = currentMillis;

    //============================== Si el valor del sensor ldr es mayor de lux_Limite y no se ha enviado aviso, envío email
    //============================== Dejo de envíar emails hasta que el valor de LDR baje de lux_Limite
    if (Luminosidad > lux_Limite && Aviso == true) 
    {
      Serial.println("¡Valor LDR sobrepasado!");
      pson data;
      data["lux_Limite"] = Luminosidad;
      thing.call_endpoint("High_Ldr_Email", data);

      Aviso = false;
      Serial.println("Avisos apagado");
      Serial.println(" ");
    }
  }
  ArduinoOTA.handle();
}
