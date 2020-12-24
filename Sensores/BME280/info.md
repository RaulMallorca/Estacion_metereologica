Un pequeño problema que me surgió a la hora de conseguir comunicación con el módulo del sensor BME280 fue el no haber tenido en cuenta que venía sin el puente de unión para elegir la dirección I2C. No conseguí encontrar referencias sobre las direcciones hasta que visité la magnífica página de patagoniatec.com donde detallaban este tema en concreto.

Si hacéis la soldadura, no habrá mayor problema. Como a día de hoy, 24/12/2020 sigo esperando más sensores que usarán el protocolo I2C, no quería hacer el puente para después que me coincidiera alguna dirección con otro sensor y tuviera que desoldar. 

La manera rápida y limpia de corregir esto es mediante programación.

Cuando iniciamos el sensor en void setup() con

**_bme.begin();_**

Dentro del paréntesis debemos indicarle la dirección elegida, 0x76 o 0x77.

**_bme.begin(0x76);_**

Sensores/BME280/20201224_064219.jpg
