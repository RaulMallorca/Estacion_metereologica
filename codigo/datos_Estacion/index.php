<?php

/*Creamos las variables donde se guardarán los datos de la lecturas de los sensores*/
$TemperaturaINT = $_GET['tempint'];		//tempint es el nombre de la variable en Arduino y que nos envía vía link
$HumedadINT = $_GET['humeint'];	//$HumedadINT es la variable con la que igualamos la variable de Arduino para la BD
$RssiINT = $_GET['rssiInt'];
$TemperaturaOUT = $_GET['tempout'];
$HumedadOUT = $_GET['humeout'];
$PuntoRocio = $_GET['p_Rocio'];
$RssiOUT = $_GET['rssiOUT'];
$Presion = $_GET['presion'];
$Altitud = $_GET['altitud'];
$Dviento = $_GET['dViento'];
$Fviento = $_GET['fViento'];
$Uv = $_GET['uV'];
$LluviaUnah = $_GET['lLuvia1h'];
$LluviaDia = $_GET['lLuvia24h'];
$Luminosidad = $_GET['lumi'];

echo "La temperatura interior es: ".$TemperaturaINT." <br> La humedad interior es: ".$HumedadINT." <br> La señal Wifi es: ".$RssiINT." <br> La temperatura exterior es: ".$TemperaturaOUT." <br> La humedad exterior es: ".$HumedadOUT." <br> El punto de rocío es: ".$PuntoRocio." <br> La señal Wifi exterior es: ".$RssiOUT." <br> La presión es: ".$Presion." <br> La altura aproximada es: ".$Altitud." <br> La dirección del viento es: ".$Dviento." <br> La fuerza del viento es: ".$Fviento." <br> El índice UV es: ".$Uv." <br> La acumulación de agua en 1 hora es: ".$LluviaUnah." <br> La acumulación de agua en 24 horas es: ".$LluviaDia." <br> La luminosidad es: ".$Luminosidad;	//Mostramos los valores por pantalla.
$usuario = "raul";	//usuario base de datos, MariaDB phpMyAdmin
$contrasena = "*R@uL001492*";	//Constraseña MariaDB (phpMyAdmin)
$servidor = "192.168.1.59:3307";	//Servidor Synology + puerto de MariaDB
$basededatos = "estacion_meteo";	//Nombre base de datos

$conexion = mysqli_connect( $servidor, $usuario, $contrasena ) or die ("No se ha podido conectar al servidor de base de datos, posible error de usuario, contraseña o servidor");

$db = mysqli_select_db( $conexion, $basededatos ) or die ( "No se ha podido seleccionar la base de datos, posible error nombre base de datos" );

$Fecha = date("Y-m-d H:i:s");	//Creamos la variable fecha y le damos el formato Año-mes-día Hora-minutos-segundos

echo "<br>La fecha es: " .$Fecha;
$consulta = "INSERT INTO datos (Fecha, TemperaturaINT, HumedadINT, RssiINT, TemperaturaOUT, HumedadOUT, PuntoRocio, RssiOUT, Presion, Altitud, Dviento, Fviento, Uv, LluviaUnah, LluviaDia,  Luminosidad) VALUES (CURRENT_TIMESTAMP, ".$TemperaturaINT.", ".$HumedadINT.", ".$RssiINT.", ".$TemperaturaOUT.", ".$HumedadOUT.", ".$PuntoRocio.", ".$RssiOUT.", ".$Presion.", ".$Altitud.", ".$Dviento.", ".$Fviento.", ".$Uv.", ".$LluviaUnah.", ".$LluviaDia.", ".$Luminosidad.")"; 
/*INSERT INTO datos - datos es el nombre de la tabla de la base de datos*/
/*Fecha, Temperatura_INT, Humedad_INT...etc, ha de coincidir con los nombres de nuestra base de datos.*/
/*CURRENT_TIMESTAMP nos da la fecha enformato correcto y con la hora. En la base de datos hay que poner en el campo Fecha Tipo DATETIME Longitud/Valores 6*/

$resultado = mysqli_query( $conexion, $consulta );

?>