/*Cliente estación metereológica con Pantalla Nextion. Ver los datos de Thinger.io por el monitor serie
   FUNCIONA - Falta corregir la muestra de TEMPERATURA_DHT22 que salen los carácteres }}] al final y conseguir dejar solo 2 decimales. 17,23 o 17.23
   Conseguir que el token no caduque.
   Comprobar si sigue baneado. Me sale el error PAYLOAD -1
   https://api.thinger.io/v1/users/EstacionMeteo/buckets/Prueba_bucket/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfUHJ1ZWJhc18xIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.YPksb1QoNFXigEuqBZsHtXdT7aVZPd-tu2xJX9t9Rv8
   ese link no debería caducar
 * */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ThingerESP8266.h>

//Datos Thinger.io
#define USERNAME "EstacionMeteo"
#define DEVICE_ID "NodeMCU_Light"
#define DEVICE_CREDENTIAL "qMw76EourWNP0G"
#define _DEBUG_

// Temporizador
unsigned long ultimaConsulta = 0;
unsigned long tiempoConsulta = 10000;

/*---------IMPORTANTE no poner la "s" en "http" ------------------*/
String link = "http://api.thinger.io/v1/users/EstacionMeteo/buckets/Prueba_bucket/data?items=1&max_ts=0&sort=desc&authorization=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJEYXNoYm9hcmRfUHJ1ZWJhc18xIiwidXNyIjoiRXN0YWNpb25NZXRlbyJ9.YPksb1QoNFXigEuqBZsHtXdT7aVZPd-tu2xJX9t9Rv8";
const char* ssid = "RedWifi";
const char* ssid_PASSWORD = "KirguisT@N2035";

HTTPClient clienteHttp; //Cliente web

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void configWifi () {
#ifdef _DEBUG_
  //Conexión con la red wifi
  Serial.print ("Conectando con ");
  Serial.println (ssid);
#endif

  //Configuración en modo cliente
  WiFi.mode (WIFI_STA);
  //Iniciar conexión con la red wifi
  WiFi.begin (ssid, ssid_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef _DEBUG_
    Serial.print (".");
#endif
  }

#ifdef _DEBUG_
  Serial.println ("");
  Serial.print ("Conectado a la red ");
  Serial.println (ssid);
#endif
}

String peticionHTTP(String link) {
  String payload;
  if (WiFi.status() == WL_CONNECTED) { //Comprueba el estado de la conexión wifi
    clienteHttp.begin(link);  //Especifica el destino de la petición
    int codigoHttp = clienteHttp.GET();    //Envía la petición

#ifdef _DEBUG_
    Serial.print ("Código HTTP: ");
    Serial.println (codigoHttp);
#endif

    if (codigoHttp > 0) { //Commprobamos el código obtenido. Códigos Thinger.io = 200 - Correcto, 401 - No autorizado, 404 - No encontrado
      payload = clienteHttp.getString();   //Obtener la respuesta de solicitud en payload
      Serial.println(payload);                     //Mostrar el resultado de payload
    } else if (codigoHttp < 0) {  //Si la respuesta es menor que 0 es que hay un error. Entonces
      payload = clienteHttp.getString();
      Serial.println ("ERROR payload"); //Mostrar error por el monitor serie.
    }
    /*Busca en que n° de caracter (o indice) de la cadena comienza el “nombre” del sensor del que se busca obtener el valor. En este caso “Luminosidad”.
       Esto lo hace la función indexOf() con la cadena buscada como argumento.
       Tomando como base el índice obtenido en el punto anterior, le suma 13 (que es la longitud de la cadena “Luminosidad” incluidas las comillas, y
       desde esa posición busca el indice de la primera “,” (coma) que aparezca. Nuevamente la funcion indexOf() esta vez con dos argumentos, el primero
       es desde que índice empezar a buscar, y el segundo el caracter buscado.
       Con estos 2 datos ya sabemos la posición dentro de la cadena en la que se encuentra nuestra variable, y usamos la función substring(a+13,b) para
      recuperar el valor. a y b son los indices obtenidos en los pasos 1 y 2.*/

    /*FUENTE
      https://community.thinger.io/t/consulta-lectura-datos-desde-data-bucket-reading-data-from-data-bucket/1482/12/ */

    Serial.print (payload.substring(((payload.indexOf("Luminosidad", ((payload.indexOf("Luminosidad"))))) + 13), (payload.indexOf(",", ((payload.indexOf("Luminosidad"))))))); //Muestra el valor de Luminosidad.
    Serial.println ();
    Serial.print (payload.substring(((payload.indexOf("Temperatura_DHT22", ((payload.indexOf("Temperatura_DHT22"))))) + 19), (payload.indexOf(",", ((payload.indexOf("Temperatura_DHT22"))))))); //Muestra el valor de Temperatura_DHT22.
    Serial.println (" ºC");
    Serial.print (payload.substring(((payload.indexOf("Humedad_DHT22", ((payload.indexOf("Humedad_DHT22"))))) + 15), (payload.indexOf(",", ((payload.indexOf("Humedad_DHT22"))))))); //Muestra el valor de Humedad_DHT22.
    Serial.println ("%");
    clienteHttp.end();   //Cerramos la conexión
  }
  return payload;
}

/*float luminosidadExtraida (String Luminosidad, String payload) {
  int Start = payload.indexOf(Luminosidad) + Luminosidad.length() + 2;
  int End = payload.indexOf(",", Start);
  if (End < 0) // Last variable doesnt have ',' at the end, have a ']'
  {
   End = payload.indexOf("]");
  }
  String stringVar = payload.substring(Start, End);
  float var = stringVar.toFloat();
  Serial.print ("El valor de luminosidad es ");
  Serial.println (var);
  delay(5000);
  return var;
  }*/

void temporizador () {
  if (millis() < ultimaConsulta) {  //Comprobar si se ha dado la vuelta
    //Asignar un nuevo valor
    ultimaConsulta = millis();
  }
  if ((millis() - ultimaConsulta) > tiempoConsulta) {
    ultimaConsulta = millis();  //Marca de tiempo
    peticionHTTP (link);
  }
}

void setup() {
  Serial.begin (9600);
  configWifi();
  thing.add_wifi(ssid, ssid_PASSWORD);
}

void loop() {
  temporizador();
  //float luminosidadExtraida(String Luminosidad, String payload);
}
