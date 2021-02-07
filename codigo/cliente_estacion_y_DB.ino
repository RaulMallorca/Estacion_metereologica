/*Cliente estación metereológica con pantalla Nextion. Ver los datos de Thinger.io por el monitor serie

  http://api.thinger.io/v1/users/EstacionMeteo/buckets/DATOS_ESTACION/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfRXN0YWNpb25fTWV0ZXJvIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.OcmRskMMOiRn3ns4bEadEmNai-T60R_s2S6Jy4m2eBo
   ese link no debería caducar
 * */

#include <ESP8266WiFi.h>  //Librería de conexión WiFi del módulo ESP8266
#include <ESP8266HTTPClient.h>  //Librería de conexión cliente del módulo ESP8266
#include <ThingerESP8266.h> //Librería de la plataforma thinger.io
#include <Wire.h>              // Librería para el sensor BME280
#include <SPI.h>               // Librería para el sensor BME280
#include <Adafruit_Sensor.h>  // Librería para el sensor BME280
#include <Adafruit_BME280.h>  // Librería para el sensor BME280
#include <ArduinoOTA.h> //Librería necesaria para el uso de OTA
#include "ESP8266_Utils_OTA.hpp"  //Configuración para uso de OTA

//Datos Thinger.io
#define USERNAME "EstacionMeteo"
#define DEVICE_ID "Servidor_Meteo"
#define DEVICE_CREDENTIAL "A+aKTL!K2!Dv"
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#define _DEBUG_

//Value httpRequest_THINGERio
String lastBucketValue;

/*-----------------VARIABLES DE LOS DATOS OBTENIDOS DESDE THINGER.IO--------------------*/
float humeout;
float tempout;
float lumi;
float altitud;
int RSSIout;
float presion;

Adafruit_BME280 bme; // BME280 conectado por I2C

// Timer
unsigned long ultimaConsulta = 0;
unsigned long tiempoConsulta = 120000;

/*---------IMPORTANTE no poner la "s" en "http" ------------------*/
String link = "http://api.thinger.io/v1/users/EstacionMeteo/buckets/DATOS_ESTACION/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfRXN0YWNpb25fTWV0ZXJvIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.OcmRskMMOiRn3ns4bEadEmNai-T60R_s2S6Jy4m2eBo";

/*-----------Parámetros Wifi-----------*/
const char* ssid = "RedWifi"; // Nuestro SSID (Nombre de la red)
const char* password = "KirguisT@N2035"; // Nuestra contraseña
const char* host = "192.168.1.59";  // IP servidor
const int   port = 80;            // Puerto servidor
//const int   tiempo_Consulta = 60000;        // Tiempo hasta conexión Wifi.

/*----------FUNCIÓN PARA LA CONEXIÓN A LA RED WIFI-----------*/
void configWifi () {
 // IPAddress staticIP (192, 168, 1, 100); //Indicamos una IP fija
 // IPAddress gateway (192, 168, 1, 1); //Puerta de enlace de nuestra conexión
 // IPAddress subnet (255, 255, 255, 0); //Máscara de subred.

  Serial.print ("Conectando con ");
  Serial.println (ssid);

  //Wifi conexion
  WiFi.mode (WIFI_STA); //Configuración en modo cliente
  WiFi.begin (ssid, password); //Iniciamos la conexión WiFi
  //WiFi.config (staticIP, gateway, subnet);  //Configuramos la conexión
  
  while (WiFi.status() != WL_CONNECTED) { //Mientras no estemos conectados
    delay(500); //Cada medio segundo
    Serial.print (".");   //Imprimimos un "." por el monitor serie.
  }
  Serial.println ("");
  Serial.print ("Conectado a la red ");
  Serial.println (ssid);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

String httpRequest_THINGERio (String link) {
  String payload;
  if (WiFi.status() == WL_CONNECTED) {  //Check Wifi connection status
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin (link);  //Specify request destinatino
    int httpCode = http.GET();

    Serial.print ("Código HTTP: ");
    Serial.println (httpCode);

    if (httpCode > 0 ) {  //Check the returning code. Thinger.io codes. 200 = ok, 401 = No authorized, 404 = No found.
      payload = http.getString(); //Get the request response payload
      //Serial.println ("El valor de payload es "); //comentar una vez comprobado que interpreta bien los datos
      //Serial.println (payload); //comentar una vez comprobado que interpreta bien los datos
    } else if (httpCode < 0) {  //If the returning code is less than 0, there is an error, them
      Serial.println ("ERROR payload");
    }
    http.end(); //Close connection
  }
  return payload;
}

float varExtract (String varToExtract, String payload) {
  int Start = payload.indexOf (varToExtract) + varToExtract.length() + 2; //Indicamos el inicio donde se encuentra el valor del sensor.
  /*----------https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/length/ ----------------*/
  //Serial.println("El valor de Start es: "); //comentar una vez comprobado que interpreta bien los datos
  //Serial.println (Start);   //comentar una vez comprobado que interpreta bien los datos
  int End = payload.indexOf (",", Start); //Indicamos donde acaba nuestro  valor del sensor
  //Serial.println("El valor de End es: "); //comentar una vez comprobado que interpreta bien los datos
  //Serial.println (End); //comentar una vez comprobado que interpreta bien los datos
  if (End < 0) {  //Last variable doesn't have "," at the end, have a "]"
    End = payload.indexOf ("}");
  }
  String stringVar = payload.substring (Start, End);  //Obtiene el valor con todos los decimales, 16.7784938493202 p.e.
  //Serial.println("El valor de stringVar es :"); //comentar una vez comprobado que interpreta bien los datos
  //Serial.println(stringVar);  //comentar una vez comprobado que interpreta bien los datos
  float var = stringVar.toFloat();  //Nos quedamos con solo 2 decimales.
  //Serial.println("El valor de var es: "); //comentar una vez comprobado que interpreta bien los datos
  //Serial.println(var);  //comentar una vez comprobado que interpreta bien los datos
  return var;
}

void timer () {
  if (millis() < ultimaConsulta) {  //Comprobar si se ha dado la vuelta
    ultimaConsulta = millis();  //Asignar un nuevo valor
  }
  if ((millis() - ultimaConsulta) > tiempoConsulta) {
    ultimaConsulta = millis();  //Marca de tiempo
    lastBucketValue = httpRequest_THINGERio (link);

    /*---------desComentar una vez comprobado que interpreta bien los datos---------*/
    tempout = varExtract ("Temperatura", lastBucketValue);
    Serial.print ("Temperatura exterior: ");
    Serial.print (tempout);
    Serial.println ("ºC");
    /*----------------------------FIN COMENTAR----------------------------*/

    /*---------desComentar una vez comprobado que interpreta bien los datos---------*/
    humeout = varExtract ("Humedad", lastBucketValue);
    Serial.print ("Humedad exterior: ");
    Serial.print (humeout);
    Serial.println ("%");
    /*----------------------------FIN COMENTAR----------------------------*/

    /*---------desComentar una vez comprobado que interpreta bien los datos---------*/
    altitud = varExtract ("Altitud aproximada", lastBucketValue);
    Serial.print ("Altitud aproximada: ");
    Serial.print (altitud);
    Serial.println ("m");
    /*----------------------------FIN COMENTAR----------------------------*/

    /*---------desComentar una vez comprobado que interpreta bien los datos---------*/
    RSSIout = varExtract ("RSSI out", lastBucketValue);
    Serial.print ("Señal RSSI out: ");
    Serial.print (RSSIout);
    Serial.println ("dBm");
    /*----------------------------FIN COMENTAR----------------------------*/

    /*---------desComentar una vez comprobado que interpreta bien los datos---------*/
    presion = varExtract ("Presión", lastBucketValue);
    Serial.print ("Presión: ");
    Serial.print (presion);
    Serial.println ("hPa");
    /*----------------------------FIN COMENTAR----------------------------*/

    /*------desComentar una vez comprobado que interpreta bien los datos------*/
    lumi = varExtract ("Luminosidad", lastBucketValue);
    Serial.print ("Luminosidad: ");
    Serial.print (lumi);
    Serial.println ("lux");
    /*----------------------------FIN COMENTAR----------------------------*/
    leer_Sensores_Local ();

    //Envío datos BD
     WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("Fallo al conectar");
      return;
    }
    
    /*-------DECLARACIÓN DE LAS VARIABLES------------*/
  float tempint = bme.readTemperature();
  float humeint = bme.readHumidity();
  int rssiInt = WiFi.RSSI();
  
    String url = "/servidor_estacion/index.php?"; //Construimos la URL datos_Estacion/index.php?temp=[La tª del sensor]&hume=[la humedad del sensor]
  url += "&tempint=";
  url += tempint; //mostrará la temperatura después de temp=
  url += "&humeint="; //añadimos lo que falta a la URl(ha de coincidir con las variables creadas en el archivo php)
  url += humeint; //mostrará la humedad después de &hum=
  url += "&rssiInt=";
  url += rssiInt;
  url += "&tempout=";
  url += tempout;
  url += "&humeout="; //añadimos lo que falta a la URl(ha de coincidir con las variables creadas en el archivo php)
  url += humeout;
  url += "&presion="; //mostrará la presión después de &presion (ha de coincidir con las variables creadas en el archivo php)
  url += presion; //mostrará la presión después
  url += "&lumi=";
  url += lumi;
    
    // Enviamos petición al servidor
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    // Leemos la respuesta del servidor
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
}

/*---FUNCIÓN PARA EL ENVÍO DE LAS LECTURAS DE LOS SENSORES A LA BASE DE DATOS---*/
void httprequest_DATA_BASE () {
  /*-------DECLARACIÓN DE LAS VARIABLES------------*/
  float tempint = bme.readTemperature();
  float humeint = bme.readHumidity();
  int rssiInt = WiFi.RSSI();

  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("Fallo al conectar");
    return;
  }
  /*---------------------------------Así se muestra en el navegador---------------------------------*/
  /* //http://192.168.1.59/servidor_estacion/index.php?tempint=7&humeint=9&tempout=1&humeout=70&presion=1025.15&lumi=160 */

  String url = "/servidor_estacion/index.php?"; //Construimos la URL datos_Estacion/index.php?temp=[La tª del sensor]&hume=[la humedad del sensor]
  url += "&tempint=";
  url += tempint; //mostrará la temperatura después de temp=
  url += "&humeint="; //añadimos lo que falta a la URl(ha de coincidir con las variables creadas en el archivo php)
  url += humeint; //mostrará la humedad después de &hum=
  url += "&rssiInt=";
  url += rssiInt;
  url += "&tempout=";
  url += tempout;
  url += "&humeout=";
  url += humeout;
  url += "&presion="; //mostrará la presión después de &presion (ha de coincidir con las variables creadas en el archivo php)
  url += presion; //mostrará la presión después
  url += "&lumi=";
  url += lumi;

  // Enviamos petición al servidor
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Leemos la respuesta del servidor
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}

void leer_Sensores_Local () {
  //Mostrar los datos de presión, temperatura, altitud y humedad por el monitor serie.
  Serial.print ("Temperatura local: ");
  Serial.print (bme.readTemperature());
  Serial.println (" ºC");
  Serial.print ("Humedad local: ");
  Serial.print (bme.readHumidity());
  Serial.println ("%");
  Serial.print ("Temp out: ");
  Serial.println (tempout);
  Serial.print ("Humedad out: ");
  Serial.println (humeout);
  Serial.print ("RSSI local: ");
  Serial.print (WiFi.RSSI());
  Serial.println ("dBm");
}

void setup() {
  Serial.begin (9600);  //Iniciamos el monitor serie
  configWifi(); //Coenctamos a la red Wifi.
  thing.add_wifi (ssid, password);  //Conectamos a Thinger.IO

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

  InitOTA();  //Ajustes como puerto, password, etc (ESP8266_Utils_OTA.hpp)
}

void loop() {

  timer();
  ArduinoOTA.handle();
}
