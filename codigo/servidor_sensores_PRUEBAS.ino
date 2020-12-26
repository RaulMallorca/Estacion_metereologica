/*
  Lectura de datos de temperatura, humedad y presión con sensor BME280 con la placa
  NodeMCU. Monitorización en la plataforma www.thinger.io.
  Envío de email según declaremos en IFTTT

  Código parcial de:
  Dani No www.esploradores.com

*/

#include <ESP8266WiFi.h>       //Librería de conexión WiFi del módulo ESP8266
#include <ThingerESP8266.h>    //Librería de la plataforma thinger.io
#include <Wire.h>              // Librería para el sensor BME280
#include <SPI.h>               // Librería para el sensor BME280
#include <Adafruit_Sensor.h>  // Librería para el sensor BME280
#include <Adafruit_BME280.h>  // Librería para el sensor BME280

//-------------------Si usamos sensores DHT, descomentar---------------------
//#include "DHT.h"               //Librería de los sensores DHT11, DHT22, etc.
//------------------------------FIN DESCOMENTAR------------------------------

#define presion_Nivel_Mar_HPA (1013.25)  //Presión a nivel del mar

// Parámetros de la conexión con Thinger.io
#define usuario "EstacionMeteo" //Nuestro usuario de Thinger.io
#define device_Id "Servidor_Meteo" //Poner el device_ID elegido en Thinger.io
#define device_credentials "A+aKTL!K2!Dv" //Poner nuestra device_credencials de Thinger.io

ThingerESP8266 thing(usuario, device_Id, device_credentials);

#define EMAIL_ENDPOINT "Prueba_IFTT"  //El endpoint se usa para integrar la plataforma thinger.io con servicios externos, en este caso, recibir notificación por email.

// Parámetros de conexión WiFi
const char WiFi_ssid[] = "RedWifi";             //Nombre de red
const char WiFi_password[] = "KirguisT@N2035";  //Clave de red

Adafruit_BME280 bme; // BME290 conectado por I2C

//---------Si usamos sensores DHT, descomentar--------
/*// Parámetros de los sensores DHT
  #define DHT22PIN D4       //Pin de conexión - GPIO02
  #define DHT22TYPE DHT22   //Modelo DHT22
  #define DHT11PIN D5       //Pin de conexión - GPIO14
  #define DHT11TYPE DHT11   //Modelo DHT11
*/
//-----------------FIN DESCOMENTAR---------------------

// Variables de tiempo
unsigned long currentMillis;
unsigned long previousMillis_Lecturas = 0;
unsigned long previousMillis_Aviso = 0;
unsigned long intervalo_entre_Lecturas = 0;
unsigned long intervalo_Aviso = 0;

// Para indicar si ya se realizo un aviso y no volver a hacerlo
bool Aviso = true;
byte ldr_Limite = 50;

//----Si usamos sensores DHT, descomentar----
//DHT sensorDHT22 (DHT22PIN, DHT22TYPE);
//DHT sensorDHT11 (DHT11PIN, DHT11TYPE);
//-----------------FIN DESCOMENTAR-----------

int pinLDR = A0; //pin analógico donde está conectada la LDR
int Luminosidad = analogRead (pinLDR); //Nos servirá para activar el endpoint y recibir un email al guardar el valor de "luminosidad"

void mostrar_info_Red () {
  // Mostrar mensaje de exito y dirección IP asignada
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
  
void leer_Sensores () {
  //Mostrar los datos de presión, temperatura, altitud y humedad por el monitor serie.
  Serial.print ("Temperatura: ");
  Serial.print (bme.readTemperature());
  Serial.println (" ºC");
  Serial.print ("Humedad: ");
  Serial.print (bme.readHumidity());
  Serial.println ("%");
  Serial.print ("Presión: ");
  Serial.print (bme.readPressure() / 100.0F);
  Serial.println (" hPa");
  Serial.print ("Altitud aproximada = ");
  Serial.print (bme.readAltitude (presion_Nivel_Mar_HPA));
  Serial.println (" m");

  /*---------Si usamos sensores DHT, descomentar-------
    //obtener y mostrar la temperatura y humedad DHT11
    Serial.print ("Temperatura: ");
    Serial.print (sensorDHT11.readTemperature());
    Serial.println (" ºC");
    Serial.print ("Humedad: ");
    Serial.print (sensorDHT11.readHumidity());
    Serial.println ("%");

    //obtener y mostrar la temperatura y humedad DHT22
    Serial.print ("Temperatura: ");
    Serial.print (sensorDHT22.readTemperature());
    Serial.println (" ºC");
    Serial.print ("Humedad: ");
    Serial.print (sensorDHT22.readHumidity());
    Serial.println ("%");
  */
  //-----------------FIN DESCOMENTAR---------------------

  //Obtener y mostrar la luminosidad del ldr
  Serial.print ("Luminosidad: ");
  Serial.println (Luminosidad);

  //Usamos la condición if para llamar el endpoint si la luminosidad supera al valor de 100
  if (Luminosidad < ldr_Limite) {
    Aviso = true;
    Serial.println("Avisos activados para LDR");
  }
  Serial.println (" ");
}

void setup() {
  Serial.begin (9600);
  while (!Serial);   // time to get serial running
  Serial.println(F("BME280 test"));
  unsigned status;

  // default settings
  status = bme.begin(0x76);
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
    Serial.println("¡¡No se encuentra el sensor BME280, comprueba conexiones, dirección y la ID del sensor!!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probablemente la dirección está mal o el sensor es un BMP 180 o BMP 085\n");
    Serial.print("   ID of 0x56-0x58 representa un BMP 280,\n");
    Serial.print("        ID of 0x60 representa un BME 280.\n");
    Serial.print("        ID of 0x61 representa un BME 680.\n");
    while (1) delay(10);
  }

  //---------Si usamos sensores DHT, descomentar------
  //sensorDHT22.begin();  // Inicialización del DHT22
  //sensorDHT11.begin();  // Inicialización del DHT11
  //-----------------FIN DESCOMENTAR------------------

  //Iniciando wifi para comunicarse con Thinger.io
  thing.add_wifi(WiFi_ssid, WiFi_password); // Inicialización de la WiFi para comunicarse con la API
  Serial.println(" ");
  Serial.println ("Comunicando con Thinger.io");
  Serial.println (" ");

  // Inicialización de la lectura de datos desde la API
  //El tipo de datos pson puede contener diferentes tipos de datos, se usa para recibir múltiples valroes al mismo tiempo
  thing["sensores"] >> [](pson & out) { //Nombre del recurso que elegiremos en el bucket de Thinger.io
    out["Temperatura"] = bme.readTemperature(); //Nombre del campo para trazar asociado a la temperatura
    out["Humedad"] = bme.readHumidity();  //Nombre del campo para trazar asociado a la humedad
    out["Presión"] = (bme.readPressure() / 100.0F); //Nombre del campo para trazar asociado a la presión
    out["Altitud aproximada"] = bme.readAltitude (presion_Nivel_Mar_HPA); //Nombre del campo para trazar asociado a la altitud aproximada
    out["Luminosidad"] = analogRead (pinLDR); //Nombre del campo para trazar asociado a la luminosidad de la ldr
    out["RSSI out"] = WiFi.RSSI(); //Nombre del campo para trazar asociado a la señal wifi de la estación exterior.
    
    /*--------------------------------------------------------DHT11 y DHT22-------------------------------------------------------
      thing["sensores"] >> [](pson & out) { //Nombre del recurso que elegiremos en el bucket de Thinger.io
      out["Temperatura_DHT22"] = sensorDHT22.readTemperature(); //Nombre del campo para trazar asociado a la temperatura del DHT22
      out["Humedad_DHT22"] = sensorDHT22.readHumidity();  //Nombre del campo para trazar asociado a la humedad del DHT22
      out["Temperatura_DHT11"] = sensorDHT11.readTemperature(); //Nombre del campo para trazar asociado a la temperatura del DHT11
      out["Humedad_DHT11"] = sensorDHT11.readTemperature(); //Nombre del campo para trazar asociado a la humedad del DHT11
      out["Luminosidad"] = analogRead (pinLDR); //Nombre del campo para trazar asociado a la luminosidad de la ldr
      -------------------------------------------------FIN DHT11 y DHT22------------------------------------------------------------*/
  };

  // Creamos los recursos DE ENTRADA para la API
  thing["ldr_Lim"] << inputValue(ldr_Limite, {  // se ejecuta cuando el valor cambia
    Serial.print("Desde Thinger.io ha cambiado el valor. El nuevo valor de ldr_Limite es: ");
    Serial.println(ldr_Limite);
  });

  // Temporizaciones para medir y para enviar avisos
  intervalo_entre_Lecturas = 60000; // 60 segundos
  intervalo_Aviso = 70000;  //70 segundos

  // Primer estado
  previousMillis_Lecturas = millis();
  previousMillis_Aviso = millis();
}

void loop() {
  thing.handle();
    
  currentMillis = millis();

  if (currentMillis - previousMillis_Lecturas > intervalo_entre_Lecturas) {  //Si la resta es > que intervalo_entre_Lecturas, vuelve a leer los sensores
    previousMillis_Lecturas = currentMillis;
    leer_Sensores ();
    thing.stream (thing["sensores"]); //Envía los datos a Thinger.io
    mostrar_info_Red();  //Mostramos información de la red Wifi por el monitor serie.
  }

  // Temporizacion para el aviso
  if (currentMillis - previousMillis_Aviso > intervalo_Aviso) { //Si la resta es > que el intervalo_Aviso, vuelve a enviar el e-mail de aviso.
    previousMillis_Aviso = currentMillis;

    // Si el valor del sensor ldr es mayor de ldr_Limite y no se ha enviado aviso, envío email
    // Dejo de envíar emails hasta que el valor de LDR baje de ldr_Limite
    if (Luminosidad > ldr_Limite && Aviso == true) {
      Serial.println("¡Valor LDR sobrepasado!");
      pson data;
      data["ldr_Limite"] = Luminosidad;
      thing.call_endpoint("High_Ldr_Email", data);

      Aviso = false;
      Serial.println("Avisos apagado");
      Serial.println(" ");
    }
  }
}
