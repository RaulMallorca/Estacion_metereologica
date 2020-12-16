##Para tener acceso al archivo necesario donde se guardarán los valores de los sensores en el Synology hay que tener acceso root al mismo.
```

<?php
	file_put_contents("text1.txt", "Holita");
?>

```

Con la sintaxis anterior creamos un archivo llamado **"text1.txt"** y escribimos **"Holita"** 

Este archivo se crea en la siguiente ruta:

>/volume2/web/Ejemplo_Jadsa_tv

Desde el navegador accedemos con la siguiente dirección:

>http://192.168.1.59/Ejemplo_Jadsa_tv/prueba.php //**http://IP_SERVIDOR/NOMBRE_CARPETA/ARCHIVO_PHP[donde está la función del principio]

Con el siguiente error:

>Warning: file_put_contents(text1.txt): failed to open stream: Permission denied in /volume2/web/Ejemplo_Jadsa_tv/prueba.php on line 3

https://www.php.net/manual/es/ini.core.php#ini.upload-tmp-dir


>El directorio temporal usado para almacenar ficheros durante el proceso de subida. Es necesario tener permisos de escritura para el usuario que está ejecutando PHP. Si no está especificado, PHP usará el predeterminado del sistema.

>Si el directorio especificado no tiene permisos de escritura, PHP recurrirá al directorio temporal predeterminado del sistema. Si la directiva open_basedir está activada, al directorio predeterminado del sistema se le ha de permitir la subida de ficheros para que funcione.



/etc/php

file_uploads = On

upload_tmp_dir = "/var/services/tmp"

upload_max_filesize = 32M
