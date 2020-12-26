/*Cliente estación metereológica con pantalla Nextion. Ver los datos de Thinger.io por el monitor serie
   
  http://api.thinger.io/v1/users/EstacionMeteo/buckets/DATOS_ESTACION/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfRXN0YWNpb25fTWV0ZXJvIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.OcmRskMMOiRn3ns4bEadEmNai-T60R_s2S6Jy4m2eBo
   ese link no debería caducar
 * */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ThingerESP8266.h>

//Datos Thinger.io
#define USERNAME "EstacionMeteo"
#define DEVICE_ID "Servidor_Meteo"
#define DEVICE_CREDENTIAL "A+aKTL!K2!Dv"
#define _DEBUG_

//Value httpRequest
String lastBucketValue;

//Data variable
float humidity_Out;
float temperature_Out;
float lumi;
//float hDHT11; //Descomentar si usamos el sensor DHT11
float altitud;
int RSSIout;
float presion;
 
// Timer
unsigned long ultimaConsulta = 0;
unsigned long tiempoConsulta = 15000;

/*---------IMPORTANTE no poner la "s" en "http" ------------------*/
String link = "http://api.thinger.io/v1/users/EstacionMeteo/buckets/DATOS_ESTACION/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfRXN0YWNpb25fTWV0ZXJvIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.OcmRskMMOiRn3ns4bEadEmNai-T60R_s2S6Jy4m2eBo";
const char* ssid = "RedWifi";
const char* ssid_PASSWORD = "KirguisT@N2035";

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void configWifi () {
  Serial.print ("Conectando con ");
  Serial.println (ssid);

  //Wifi conexion
  WiFi.mode (WIFI_STA); //Client mode configuration
  WiFi.begin (ssid, ssid_PASSWORD); //Starting wifi conexion

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print (".");
  }
  Serial.println ("");
  Serial.print ("Conectado a la red ");
  Serial.println (ssid);
}

String httpRequest (String link) {
  String payload;
  if (WiFi.status() == WL_CONNECTED) {  //Check Wifi connection status
    HTTPClient http;  //Declare an object of class HTTPClient
    http.begin (link);  //Specify request destinatino
    int httpCode = http.GET();
   
    Serial.print ("Código HTTP: ");
    Serial.println (httpCode);
    
    if (httpCode > 0 ) {  //Check the returning code. Thinger.io codes. 200 = ok, 401 = No authorized, 404 = No found.
      payload = http.getString(); //Get the request response payload
      Serial.println ("El valor de payload es "); //comentar una vez comprobado que interpreta bien los datos
      Serial.println (payload); //comentar una vez comprobado que interpreta bien los datos
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
  Serial.println("El valor de Start es: "); //comentar una vez comprobado que interpreta bien los datos
  Serial.println (Start);   //comentar una vez comprobado que interpreta bien los datos
  int End = payload.indexOf (",", Start); //Indicamos donde acaba nuestro  valor del sensor
  Serial.println("El valor de End es: "); //comentar una vez comprobado que interpreta bien los datos
  Serial.println (End); //comentar una vez comprobado que interpreta bien los datos
    if (End < 0) {  //Last variable doesn't have "," at the end, have a "]"
      End = payload.indexOf ("}");
    }
  String stringVar = payload.substring (Start, End);  //Obtiene el valor con todos los decimales, 16.7784938493202 p.e.
  Serial.println("El valor de stringVar es :"); //comentar una vez comprobado que interpreta bien los datos
  Serial.println(stringVar);  //comentar una vez comprobado que interpreta bien los datos
  float var = stringVar.toFloat();  //Nos quedamos con solo 2 decimales.
  Serial.println("El valor de var es: "); //comentar una vez comprobado que interpreta bien los datos
  Serial.println(var);  //comentar una vez comprobado que interpreta bien los datos
  return var; 
}

void timer () {
  if (millis() < ultimaConsulta) {  //Comprobar si se ha dado la vuelta
    ultimaConsulta = millis();  //Asignar un nuevo valor
  }
  if ((millis() - ultimaConsulta) > tiempoConsulta) {
    ultimaConsulta = millis();  //Marca de tiempo
    lastBucketValue = httpRequest (link); 
    temperature_Out = varExtract ("Temperatura", lastBucketValue);
    Serial.println (temperature_Out); //comentar una vez comprobado que interpreta bien los datos
    humidity_Out = varExtract ("Humedad", lastBucketValue);
    Serial.println (humidity_Out);  //comentar una vez comprobado que interpreta bien los datos
    altitud = varExtract ("Altitud aproximada", lastBucketValue);
    Serial.println (altitud); //comentar una vez comprobado que interpreta bien los datos
    RSSIout = varExtract ("RSSI out", lastBucketValue);
    Serial.println (RSSIout); //comentar una vez comprobado que interpreta bien los datos
    presion = varExtract ("Presión", lastBucketValue);
    Serial.println (presion); //comentar una vez comprobado que interpreta bien los datos
    lumi = varExtract ("Luminosidad", lastBucketValue);
    Serial.println (lumi);  //comentar una vez comprobado que interpreta bien los datos
    
   /* --------DESCOMENTAR SI USAMOS EL SENSOR DHT11 y DHT22-------------
      hDHT11 = varExtract ("Humedad_DHT11", lastBucketValue);
    Serial.println (hDHT11);
    temperature_Out = varExtract ("Temperatura_DHT22", lastBucketValue);
    Serial.println (temperature_Out);
    humidity_Out = varExtract ("Humedad_DHT22", lastBucketValue);
    Serial.println (humidity_Out); 
    ------------------------Fin descomentar----------------------------*/  
  }
}  
void setup() {
  Serial.begin (9600);  //Starting monitor serial
  configWifi(); //Wifi conexion
  thing.add_wifi (ssid, ssid_PASSWORD);  //Thinger.io credentials data
}

void loop() {
  timer();

}
