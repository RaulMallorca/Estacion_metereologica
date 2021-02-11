/*
  Lectura de datos de temperatura, humedad, altura aproximada y presión con sensor BME280. Luminosidad con BH1750, Anemómetro, veleta y pluviómetro con Argent Data Systems,
  índice UV con ML8511 con la placa NodeMCU. Monitorización en la plataforma www.thinger.io.
  
  El pluviómetro muestra la precipitación caída durante 1h y durante 24h. Después se reinician los dos contadores.
 *Necesita una resistencia de 10kOhm pull-up (Pin V, pin D6 y pin+ del pluviómetro.

  El anemómetro y la veleta también usan una resistencia cada uno de 10kOhm pull-up (Pin V, pines correspondientes y pin+ de cada uno de los sensores).
  
  Envío de email según declaremos en IFTTT
  
  Código parcial de:
  Dani No www.esploradores.com (Thinger.IO)
  https://www.aeq-web.com/ (Anemómetro)
  Code wind vane Hans Huth and licensed under Creative Commons (Veleta)
  RimvydasP, https://www.instructables.com/NodeMCU-Wireless-Weather-Station/  (Pluviómetro)
  
*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////===== Librerías =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

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

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////////////===== Fin librerías =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

//============================== Pines usados
const byte Anemometro = D5;     //Definimos el pin de interrupción para el anemómetro. GPIO 14
const byte RainSensorPin = D6;  //Pin Pluviómetro GPIO 12
#define direccion_Viento A0     //Pin Veleta

//============================== Definimos la presión a nivel del mar.
#define presion_Nivel_Mar_HPA (1013.25)  

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////===== Parámetros de Thinger.io =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*========================= Conexión con Thinger.io =========================*/

#define usuario "EstacionMeteo" //Nuestro usuario de Thinger.io
#define device_Id "Servidor_Meteo" //Poner el device_ID elegido en Thinger.io
#define device_credentials "A+aKTL!K2!Dv" //Poner nuestra device_credencials de Thinger.io

ThingerESP8266 thing(usuario, device_Id, device_credentials);

#define EMAIL_ENDPOINT "Prueba_IFTT"  //El endpoint se usa para integrar la plataforma thinger.io con servicios externos, en este caso, recibir notificación por email.

/*=========================  Variables para indicar si ya se realizo un aviso a la plataforma Thinger.io y no volver a hacerlo =========================*/
bool Aviso = true;
byte lux_Limite = 200;

int Luminosidad; //Para activar el endpoint y recibir un email al guardar el valor de "luminosidad"

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

/*-------------------------------FUNCIÓN PARA TENER DOS DECIMALES EN THINGER.IO-------------------------------*/
//Para tener dos decimales en el bucket de Thinger, hay que convertir las variables con los resutados a String. El problema es con el cliente que no guarda los datos bien
//en la base de datos. (No usada) https://community.thinger.io/t/value-in-the-data-bucket-from-esp8266-is-too-much/3582/9

//Gestión de decimales de Thinger
    float round_to_dp(float in_value, int decimal_place)
    {
      float multiplier = powf(10.0f, decimal_place);
      in_value = roundf(in_value * multiplier) / multiplier;
      return in_value;
    }
/*------------------------------------------FIN FUNCIÓN----------------------------------------*/
  
/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////===== Fin parámetros de Thinger.io =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////===== Parámetros de conexión WiFi =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

const char WiFi_ssid[] = "RedWifi";             //Nombre de red
const char WiFi_password[] = "KirguisT@N2035";  //Clave de red

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

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////===== Fin parámetros de conexión WiFi =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////////===== Parámetros sensores =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

Adafruit_BME280 bme; // BME280 conectado por I2C
Adafruit_ADS1115 ads; //Creamos un objeto de la clase
BH1750 lightMeter(0x23);  //Dirección I2C

//============================== Variables de los sensores
float T_out;
float H_out;
float P_out;
float Altitud;
float Lumi;
float indiceUV;
float Velocidad_Viento;
float valor_Grados_Pin_Veleta = 0;
float angulo = 0;      

//============================== Variables punto_Rocio
double raiz1;
double raiz2;
double Pun_rocio;

/*----------------------- FUNCIÓN PARA MAPEAR UNA FLOAT-----------------------*/
                          /*Para hallar el índice UV*/
                /*////////////// Gracias al siguiente post \\\\\\\\\\\\\\\*/
             /* From: http://forum.arduino.cc/index.php?topic=3922.0 */
float mapfloat (float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////===== Fin parámetros sensores =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////===== Variables de tiempo programa =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

unsigned long currentMillis;
unsigned long previousMillis_Lecturas = 0;
unsigned long previousMillis_Aviso = 0;
unsigned long intervalo_entre_Lecturas = 0;
unsigned long intervalo_Aviso = 0;

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////===== Fin variables de tiempo programa =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////===== Anemómetro =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

const int RecordTime = 3; //Definimos la medición del tiempo para la velocidad del viento (segundos)

//============================== Variable ISR anemómetro
volatile int InterruptCounter;

/*--------------FUNCIÓN PARA MEDIR LA VELOCIDAD DEL VIENTO CON EL ANEMÓMETRO--------------*/
 void medicion_anemometro() 
 {
  InterruptCounter = 0; //Reiniciamos el contador de interrupción 
  attachInterrupt(digitalPinToInterrupt(Anemometro), countup, RISING);
  delay(1000 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(Anemometro));
  Velocidad_Viento = (float)InterruptCounter / (float)RecordTime * 2.4; //Pasamos a km/h. Cada vuelta del anemómetro equivale a 1,492 mph.
}
/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*-----------------------FUNCIÓN DE INTERRUPCIÓN ANEMÓMETRO---------------------*/
ICACHE_RAM_ATTR void countup() 
{
  InterruptCounter++; //sumamos uno al contador de interrupción del anemómetro
}
/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////////////===== Fin anemómetro =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////===== Pluviómetro =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

#define Bucket_Size_EU 0.279           // Tamaño del pluviómetro( 0.279mm)

int Contador1h ;   // Contador para la adquisición de la precipitación en 1h
int Contador24h ;  // Contador para la adquisición de la precipitación en 24h  
volatile unsigned long contactTime;

//--------------------------------VARIABLES DEL PLUVIÓMETRO---------------------------------------------

float lluvia1h = 0;          // Precipitación en milímetros en 1 hora
float lluvia24h = 0;         // Precipitación en milímetros en 24 hours

//-------------------------------VARIBALES DE TIEMPO PARA EL PLUVIÓMETRO---------------------------------

const long _1Hora = 3600000;
const long _24Horas = 86400000;
unsigned long current_millis_Lluvia;
unsigned long previous_millis_Lluvia1h;
unsigned long previous_millis_Lluvia24h;

/*-----------------------FUNCIÓN DE INTERRUPCIÓN PARA EL PLUVIÓMETRO-----------------------*/

ICACHE_RAM_ATTR void isr_rg() {

  if((millis() - contactTime) > 2000 ) { // Evitar el falso contacto por rebote en el pluviómetro. 
    Contador1h++;
    Contador24h++;
    contactTime = millis(); 
    Serial.print ("El valor dentro del isr de Contador1h es: ");  //Comentar una vez funcione
    Serial.println (Contador1h);                                  //Comentar una vez funcione
    Serial.print ("El valor dentro del isr de Contador24h es: "); //Comentar una vez funcione
    Serial.println (Contador24h);                                 //Comentar una vez funcione
  } 
} 

/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*-----------------------FUNCIÓN ADQUISICIÓN DATOS PLUVIÓMETRO-----------------------*/
void getRain(void)
   {   
      current_millis_Lluvia = millis();
      
      cli();         //Desactivamos las interrupciones.
      
      lluvia1h = Contador1h * Bucket_Size_EU;   // 0.279 mm tamaño vaso
      lluvia24h = Contador24h * Bucket_Size_EU;  // 0.279 mm tamaño vaso
            
    if (current_millis_Lluvia - previous_millis_Lluvia1h >= _1Hora) //Reset precipitación 1h
    {      
      Serial.println("Reset lluvia 1 hora");
      Contador1h = 0; //reset  contador precipitación 1h
      previous_millis_Lluvia1h = millis();
    } 
    if ((current_millis_Lluvia - previous_millis_Lluvia24h >= _1Hora) && (current_millis_Lluvia - previous_millis_Lluvia24h >= _24Horas))    //Reset precipitación 24h
    {
      previous_millis_Lluvia24h = millis();
      
      Serial.println("Reset lluvia 24 horas");                            //Comentar una vez funcione
      Contador1h = 0;   //reset  contador precipitación 1h
      Contador24h = 0;  //reset  contador precipitación 24h
      //Serial.print ("Segunda condición. El valor de Contador 1h es: ");   //Comentar una vez funcione
      //Serial.println (Contador1h);                                        //Comentar una vez funcione
      //Serial.print ("El valor de Contador 24h es: ");                     //Comentar una vez funcione
      //Serial.println (Contador24h);                                       //Comentar una vez funcione
    }
      
   sei();         //Activamos las interrupciones.
}

/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////////////////////////===== Fin pluviómetro =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////////////////////////////////////////////////////////////////////////////===== Veleta =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*------------------------FUNCIÓN PARA HAYAR GRADOS VELETA------------------------*/
void grados_viento (int valor_Grados_Pin_Veleta) 
{
  //============================== Arrays específicos para la veleta con una resistencia de 10kOhm conectada al Vout del NodeMCU.
  int sensorExp [] = {74, 92, 102, 134, 198, 259, 304, 427, 482, 620, 652, 725, 808, 848, 906, 963};
  float dirGrados[] = {112.5,67.5,90,157.5,135,202.5,180,22.5,45,247.5,225,337.5,0,292.5,315,270};

  int sensorMin[] = {73, 90, 100, 132, 197, 258, 303, 424, 480, 618, 649, 723, 804, 845, 903, 960};
  int sensorMax[] = {77, 95, 105, 137, 201, 262, 307, 430, 485, 624, 655, 728, 812, 852, 910, 966};

  valor_Grados_Pin_Veleta = analogRead(direccion_Viento);
    for(int i=0; i<=15; i++) {
      if(valor_Grados_Pin_Veleta >= sensorMin[i] && valor_Grados_Pin_Veleta <= sensorMax[i]) {
      angulo = dirGrados[i]; 
      break;
      } 
    }
}
/*----------------------------------FIN FUNCIÓN----------------------------------*/


/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//////////////////////////////////////////////////////////////////////////===== Fin veleta =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////===== Muestra datos monitor serie =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*------------------FUNCIÓN PARA MOSTRAR POR EL MONITOR SERIE LAS LECTURAS Y ACTIVAR EL AVISO DE LUMINOSIDAD-------------*/
void leer_Sensores ()
{
  //==============================Mostrar datos de presión, temperatura, altitud, luminosidad, RSSI, punto rocío, índice UV, velocidad y dirección viento, lluvia y humedad.
  Serial.print ("Temperatura: ");
  Serial.print (T_out, 2);
  Serial.println (" ºC");
  Serial.print ("Humedad: ");
  Serial.print (H_out, 2);
  Serial.println ("%");
  Serial.print ("Presión: ");
  Serial.print (P_out, 1);
  Serial.println (" hPa");
  Serial.print ("Altitud aproximada = ");
  Serial.print (Altitud,1);
  Serial.println (" m");
  Serial.print ("RSSI servidor: ");
  Serial.print (WiFi.RSSI());
  Serial.println("dBm");
  Serial.print ("Luminosidad: ");
  Serial.print (Lumi, 1);
  Serial.println ("lux");
  Serial.print ("El punto de rocío es: ");
  Serial.print (Pun_rocio, 2);
  Serial.println (" ºC");
  Serial.print ("El índice UV es: ");
  Serial.print (indiceUV, 2);
  Serial.println ("mW/cm^2");
  Serial.print ("La velocidad del viento es: ");
  Serial.print (Velocidad_Viento, 2);
  Serial.println ("km/h");
  Serial.print ("La dirección del viento es: ");  
  Serial.print (angulo);
  Serial.println ("º"); 
  Serial.print("La acumulación de lluvia en 1h es: ");    
  Serial.println(lluvia1h, 2); 
  Serial.print("La acumulación de lluvia en 24h es: "); 
  Serial.print(lluvia24h, 2); 
  Serial.println("mm");

}

/*----------------------------------FIN FUNCIÓN----------------------------------*/

/*/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////////////////===== Fin muestra datos monitor serie =====\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////////////////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*================================================================================================================================================================*/

void setup()
{
  Serial.begin (9600);
  delay(200);

  pinMode(RainSensorPin, INPUT);  //pin pluviómetro
  attachInterrupt(digitalPinToInterrupt(RainSensorPin), isr_rg, FALLING);  //ISR pluviómetro.
  
  Wire.begin(D2, D1); //Iniciamos el sensor BH1750
  ads.begin();  //Iniciamos el sensor ML8511

  lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE); //Modo alta resolución "ONE TIME"
  while (!Serial);   // time to get serial running

  //============================== default settings bme280
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

  //============================== Inicialización de la lectura de datos desde la API
  //============================== El tipo de datos pson puede contener diferentes tipos de datos, se usa para recibir múltiples valores al mismo tiempo.
  thing["sensores"] >> [](pson & out) { //Nombre del recurso que elegiremos en el bucket de Thinger.io
    out["Temperatura"] =  T_out; //Nombre del campo para trazar asociado a la temperatura.
    out["Humedad"] = H_out;  //Nombre del campo para trazar asociado a la humedad.
    out["Punto de rocio"] = Pun_rocio;  //Nombre del campo para trazar asociado al punto de rocío.
    out["Presión"] = P_out; //Nombre del campo para trazar asociado a la presión.
    out["Altitud aproximada"] = Altitud; //Nombre del campo para trazar asociado a la altitud aproximada.
    out["Luminosidad"] = Lumi; //Nombre del campo para trazar asociado a la luminosidad de la ldr.
    out["Indice UV"] = indiceUV;  //Nombre del campo para trazar asociado al índice ultravioleta.
    out["RSSI out"] = WiFi.RSSI(); //Nombre del campo para trazar asociado a la señal wifi de la estación exterior.
    out["Viento-(V)"] = Velocidad_Viento;
    out["Ángulo"] = angulo;
    out["Lluvia1h"] = lluvia1h;
    out["Lluvia24h"] = lluvia24h;
    
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
    medicion_anemometro ();
    grados_viento(valor_Grados_Pin_Veleta);
    getRain();
    
    //============================== Calculamos el punto de rocío
    raiz1 = pow (H_out, 1.0 / 8);
    raiz2 = pow (100, 1.0 / 8);
    double div_raiz = raiz1 / raiz2;
    Pun_rocio = div_raiz * (110 + T_out) - 110;
      
    //============================== Calculamos el índiceUV
    int refLevel = ads.readADC_SingleEnded(0);  //Lectura del pin A0 del ADS1115 que está conectado al pin 3.3V y EN del ML8511
    int uvLevel = ads.readADC_SingleEnded(1);   //Lectura del pin A1 del ADS1115 que está conectado al pin OUT del ML8511

    float voltajeSalida = 3.3 / refLevel * uvLevel; //Dividimos el voltaje de entrada por la lectura del pin OUT y multiplicamos el resultado por la lectura del pin 3.3V y EN                                                    
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
