/*
  Lectura de datos de temperatura y humedad con sensor DHT22, DHT11 y ldr con la placa
  NodeMCU. Monitorización en la plataforma www.thinger.io.
  Envío de email según declaremos en IFTTT

  Código parcial de:
  Dani No www.esploradores.com

*/

#include <ESP8266WiFi.h>       //Librería de conexión WiFi del módulo ESP8266
#include <ThingerESP8266.h>    //Librería de la plataforma thinger.io
#include "DHT.h"               //Librería de los sensores DHT11, DHT22, etc. 

// Parámetros del conexión con thinger.io
#define usuario "EstacionMeteo" //Nuestro usuario de Thinger.io
#define device_Id "NodeMCU_Light" //Poner el device_ID elegido en Thinger.io
#define device_credentials "qMw76EourWNP0G" //Poner nuestra device_credencials de Thinger.io

ThingerESP8266 thing(usuario, device_Id, device_credentials);

#define EMAIL_ENDPOINT "Prueba_IFTT"  //El endpoint se usa para integrar la plataforma thinger.io con servicios externos, en este caso, recibir notificación por email.

// Parámetros de conexión WiFi
const char WiFi_ssid[] = "RedWifi";             //Nombre de red
const char WiFi_password[] = "KirguisT@N2035";  //Clave de red

// Parámetros de los sensores DHT
#define DHT22PIN D4       //Pin de conexión - GPIO02
#define DHT22TYPE DHT22   //Modelo DHT22
#define DHT11PIN D5       //Pin de conexión - GPIO14
#define DHT11TYPE DHT11   //Modelo DHT11

// Variables de tiempo
unsigned long currentMillis;
unsigned long previousMillis_Lecturas = 0;
unsigned long previousMillis_Aviso = 0;
unsigned long intervalo_entre_Lecturas = 0;
unsigned long intervalo_Aviso = 0;

// Para indicar si ya se realizo un aviso y no volver a hacerlo
bool Aviso = true;
byte ldr_Limite = 50;

DHT sensorDHT22 (DHT22PIN, DHT22TYPE);
DHT sensorDHT11 (DHT11PIN, DHT11TYPE);
int pinLDR = A0; //pin analógico donde está conectada la LDR
int Luminosidad = analogRead (pinLDR); //Nos servirá para activar el endpoint y recibir un email al guardar el valor de "luminosidad"

void leer_Sensores () {

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

  sensorDHT22.begin();  // Inicialización del DHT22
  sensorDHT11.begin();  // Inicialización del DHT11

  //Iniciando wifi para comunicarse con Thinger.io
  thing.add_wifi(WiFi_ssid, WiFi_password); // Inicialización de la WiFi para comunicarse con la API
  Serial.println(" ");
  Serial.println ("Comunicando con Thinger.io");
  Serial.println (" ");

  // Inicialización de la lectura de datos desde la API
  //El tipo de datos pson puede contener diferentes tipos de datos, se usa para recibir múltiples valroes al mismo tiempo
  thing["sensores"] >> [](pson & out) { //Nombre del recurso que elegiremos en el bucket de Thinger.io
    out["Temperatura_DHT22"] = sensorDHT22.readTemperature(); //Nombre del campo para trazar asociado a la temperatura del DHT22
    out["Humedad_DHT22"] = sensorDHT22.readHumidity();  //Nombre del campo para trazar asociado a la humedad del DHT22
    out["Temperatura_DHT11"] = sensorDHT11.readTemperature(); //Nombre del campo para trazar asociado a la temperatura del DHT11
    out["Humedad_DHT11"] = sensorDHT11.readTemperature(); //Nombre del campo para trazar asociado a la humedad del DHT11
    out["Luminosidad"] = analogRead (pinLDR); //Nombre del campo para trazar asociado a la luminosidad de la ldr
  };

  // Creamos los recursos DE ENTRADA para la API
   thing["ldr_Lim"] << inputValue(ldr_Limite, {  // se ejecuta cuando el valor cambia 
    Serial.print("Desde Thinger.io ha cambiado el valor. El nuevo valor de ldr_Limite es: ");
    Serial.println(ldr_Limite);
  });

  // Temporizaciones para medir y para enviar avisos
  intervalo_entre_Lecturas = 60000; // 60 segundos
  intervalo_Aviso = 70000; // 70 segundos

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
  }
  
  // Temporizacion para el aviso
  if(currentMillis - previousMillis_Aviso > intervalo_Aviso) {  //Si la resta es > que el intervalo_Aviso, vuelve a enviar el e-mail de aviso.
    previousMillis_Aviso = currentMillis;
    
    // Si el valor del sensor ldr es mayor de ldr_Limite y no se ha enviado aviso, envío email    
    // Dejo de envíar emails hasta que el valor de LDR baje de ldr_Limite
    if (Luminosidad > ldr_Limite && Aviso == true){
      Serial.println("¡Valor LDR sobrepasado!");
      pson data;
      /*data["Ldr"] = Luminosidad;
      thing.call_endpoint (EMAIL_ENDPOINT, "sensores");
    Serial.println ("Luminosidad por encima de 100, enviando email"); */
    data["ldr_Limite"] = Luminosidad;
      thing.call_endpoint("High_Ldr_Email", data);
      
      Aviso = false;       
      Serial.println("Avisos apagado");
      Serial.println(" ");
    }     
  }    
}
