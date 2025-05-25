PAV - P5: síntesis musical polifónica
=====================================

Obtenga su copia del repositorio de la práctica accediendo a [Práctica 5](https://github.com/albino-pav/P5) 
y pulsando sobre el botón `Fork` situado en la esquina superior derecha. A continuación, siga las
instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para crear una rama con el apellido de
los integrantes del grupo de prácticas, dar de alta al resto de integrantes como colaboradores del proyecto
y crear la copias locales del repositorio.

Como entrega deberá realizar un *pull request* con el contenido de su copia del repositorio. Recuerde que
los ficheros entregados deberán estar en condiciones de ser ejecutados con sólo ejecutar:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.sh
  make release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A modo de memoria de la práctica, complete, en este mismo documento y usando el formato *markdown*, los
ejercicios indicados.

Ejercicios.
-----------

### Envolvente ADSR.

Tomando como modelo un instrumento sencillo (puede usar el InstrumentDumb), genere cuatro instrumentos que
permitan visualizar el funcionamiento de la curva ADSR.

* Un instrumento con una envolvente ADSR genérica, para el que se aprecie con claridad cada uno de sus
  parámetros: ataque (A), caída (D), mantenimiento (S) y liberación (R).

  <img width="699" alt="Captura de pantalla 2025-05-24 a las 13 32 34" src="https://github.com/user-attachments/assets/80f83e7f-d67b-4b27-ad69-7b806561a504" />

* Un instrumento *percusivo*, como una guitarra o un piano, en el que el sonido tenga un ataque rápido, no
  haya mantenimiemto y el sonido se apague lentamente.
  - Para un instrumento de este tipo, tenemos dos situaciones posibles:
    * El intérprete mantiene la nota *pulsada* hasta su completa extinción.

  <img width="683" alt="Captura de pantalla 2025-05-24 a las 13 33 35" src="https://github.com/user-attachments/assets/792dcad8-71e0-4c81-8d04-756a43a35f64" />

    * El intérprete da por finalizada la nota antes de su completa extinción, iniciándose una disminución abrupta del sonido hasta su finalización.

  <img width="691" alt="Captura de pantalla 2025-05-24 a las 13 34 20" src="https://github.com/user-attachments/assets/77b96d00-137e-4b8f-a2cc-a207b18ce83c" />

  - Debera representar en esta memoria **ambos** posibles finales de la nota.

* Un instrumento *plano*, como los de cuerdas frotadas (violines y semejantes) o algunos de viento. En
  ellos, el ataque es relativamente rápido hasta alcanzar el nivel de mantenimiento (sin sobrecarga), y la
  liberación también es bastante rápida.

  <img width="698" alt="Captura de pantalla 2025-05-24 a las 13 35 23" src="https://github.com/user-attachments/assets/b7688634-96e7-4c79-baa3-e404e4182efb" />


Para los cuatro casos, deberá incluir una gráfica en la que se visualice claramente la curva ADSR. Deberá
añadir la información necesaria para su correcta interpretación, aunque esa información puede reducirse a
colocar etiquetas y títulos adecuados en la propia gráfica (se valorará positivamente esta alternativa).

### Instrumentos Dumb y Seno.

Implemente el instrumento `Seno` tomando como modelo el `InstrumentDumb`. La señal **deberá** formarse
mediante búsqueda de los valores en una tabla.

- Incluya, a continuación, el código del fichero `seno.cpp` con los métodos de la clase Seno.

  ```
  #include "seno.h"
  #include "keyvalue.h"
  #include <cmath>
  #include <iostream>

  using namespace std;
  using namespace upc;

  Seno::Seno(const std::string &param) : adsr(SamplingRate, param) {
      bActive = false;
      x.resize(BSIZE);
      KeyValue kv(param);

      int N;
      if (!kv.to_int("N", N)) N = 1024;

      table.resize(N);
      for (int i = 0; i < N; ++i) {
          table[i] = sin(2 * M_PI * i / N);
      }

      index = 0;
      increment = 1.0;
      A = 1.0;
  }

  void Seno::command(long cmd, long note, long vel) {
      if (cmd == 9) { // Nota presionada
          bActive = true;
          adsr.start();
          A = vel / 127.0;
          float f = 440.0 * pow(2.0, (note - 69) / 12.0);
          increment = f * table.size() / SamplingRate;
          index = 0;
      }
      else if (cmd == 8) { // Nota soltada
          adsr.stop();
      }
      else if (cmd == 0) { // Fin abrupto
          adsr.end();
      }
  }

  const vector<float> &Seno::synthesize() {
      if (!adsr.active()) {
          x.assign(x.size(), 0.0f);
          bActive = false;
          return x;
      }
      else if (!bActive) {
          return x;
      }

      for (unsigned int i = 0; i < x.size(); ++i) {
          int idx = static_cast<int>(index) % table.size();
          x[i] = A * table[idx];
          index += increment;
      }

      adsr(x);  // Aplicar envolvente a la señal
      return x;
  }
  ```

- Explique qué método se ha seguido para asignar un valor a la señal a partir de los contenidos en la tabla,
  e incluya una gráfica en la que se vean claramente (use pelotitas en lugar de líneas) los valores de la
  tabla y los de la señal generada.

	Para generar la señal sinusoidal, utilizamos una tabla con un solo periodo de una onda seno dividida en N muestras. En lugar de calcular el seno en tiempo real, accedemos a la tabla con un índice fraccionario que se incrementa según la frecuencia deseada. Esto nos permite recorrer la tabla a distinta velocidad y generar señales de distintas frecuencias de forma eficiente. La gráfica que hicimos en Python muestra con pelotitas azules los valores de la tabla y con pelotitas naranjas los valores generados al recorrerla. Se puede ver cómo la señal final se construye a partir de la tabla original siguiendo el avance del índice.

  <img width="1000" alt="Captura de pantalla 2025-05-25 a las 11 57 55" src="https://github.com/user-attachments/assets/f2cc809c-09b2-443d-a22f-50e1338821f6" />

- Si ha implementado la síntesis por tabla almacenada en fichero externo, incluya a continuación el código
  del método `command()`.

    ```
  void Seno::command(long cmd, long note, long vel) {
        if (cmd == 9) { // Nota presionada
            bActive = true;
            adsr.start();
            A = vel / 127.0;
            float f = 440.0 * pow(2.0, (note - 69) / 12.0);
            increment = f * table.size() / SamplingRate;
            index = 0;
        }
        else if (cmd == 8) { // Nota soltada
            adsr.stop();
        }
        else if (cmd == 0) { // Fin abrupto
            adsr.end();
        }
    }
    ```

### Efectos sonoros.

- Incluya dos gráficas en las que se vean, claramente, el efecto del trémolo y el vibrato sobre una señal
  sinusoidal. Deberá explicar detalladamente cómo se manifiestan los parámetros del efecto (frecuencia e
  índice de modulación) en la señal generada (se valorará que la explicación esté contenida en las propias
  gráficas, sin necesidad de *literatura*).

  <img width="1189" alt="Captura de pantalla 2025-05-23 a las 14 02 55" src="https://github.com/user-attachments/assets/2444f8f1-dead-4d04-9879-0cb76c249a79" />

  <img width="1197" alt="Captura de pantalla 2025-05-23 a las 14 02 29" src="https://github.com/user-attachments/assets/cf441a03-64cd-48f2-bd38-b6225e211879" />

- Si ha generado algún efecto por su cuenta, explique en qué consiste, cómo lo ha implementado y qué
  resultado ha producido. Incluya, en el directorio `work/ejemplos`, los ficheros necesarios para apreciar
  el efecto, e indique, a continuación, la orden necesaria para generar los ficheros de audio usando el
  programa `synth`.

	Lo que hicimos fue generar una señal de tipo seno y la modificamos para que el sonido cambie entre el canal izquierdo y derecho a lo largo del tiempo. Para eso, utilizamos una onda de baja frecuencia que controla cuánto volumen tiene cada canal en cada momento. El canal izquierdo va subiendo mientras el derecho baja, y luego se invierten, repitiendo el patrón. Así se consigue una sensación de movimiento en el espacio estéreo, como si el sonido se estuviera desplazando de un lado al otro. El efecto se produce sin cambiar la frecuencia ni la forma de la onda, simplemente ajustando la salida en cada canal. El código esta en C++ y genera directamente un archivo .wav estéreo. Lo compilamos con synth y al ejecutarlo se crea el fichero de audio con el efecto aplicado.

### Síntesis FM.

Construya un instrumento de síntesis FM, según las explicaciones contenidas en el enunciado y el artículo
de [John M. Chowning](https://web.eecs.umich.edu/~fessler/course/100/misc/chowning-73-tso.pdf). El
instrumento usará como parámetros **básicos** los números `N1` y `N2`, y el índice de modulación `I`, que
deberá venir expresado en semitonos.

- Use el instrumento para generar un vibrato de *parámetros razonables* e incluya una gráfica en la que se
  vea, claramente, la correspondencia entre los valores `N1`, `N2` e `I` con la señal obtenida.

  En este apartado, se ha generado un instrumento llamado `instrument_fmvib`, que se encuentra en la carpeta `src/instruments`, para hacer el vibrato a partir del instrumento FM.
  En el `dum.orc`, se puede apreciar que se han usado los parámetros I = 1 y una fm = 2Hz en el InstrumentFMVib creado. En la carpeta `work` se encuentra el código MatLab `vib.m` con el que se ha graficado la señal `fmvib.wav`. 
  Seguidamente, se adjunta la gráfica resultante, con el periodo máximo y mínimo indicado:

  ![alt text](<Screenshot 2025-05-25 at 14.11.31.png>)

- Use el instrumento para generar un sonido tipo clarinete y otro tipo campana. Tome los parámetros del
  sonido (N1, N2 e I) y de la envolvente ADSR del citado artículo. Con estos sonidos, genere sendas escalas
  diatónicas (fichero `doremi.sco`) y ponga el resultado en los ficheros `work/doremi/clarinete.wav` y
  `work/doremi/campana.work`.

  Se ha generado `clarinete.wav`, correspondiente al instrumento 4 del `dumb.orc` y la `campana.wav`, que es el número 5. 
  En ambos casos, se ha ejecutado la partitura de `doremi.sco`, aunque para cada instrumento se a generado un `doremi.sco` concreto cambiando el número del instrumento; por ejemplo, para el caso de la campana se ha creado el `doremicampana.sco`.
  Dentro de `work/doremi` encontrará los resultados generados.
  * También puede colgar en el directorio work/doremi otras escalas usando sonidos *interesantes*. Por
    ejemplo, violines, pianos, percusiones, espadas láser de la
	[Guerra de las Galaxias](https://www.starwars.com/), etc.

    Se ha implementado, con el mismo proceso, el sonido `piano.wav` y la `tuba.wav`. 

### Orquestación usando el programa synth.

Use el programa `synth` para generar canciones a partir de su partitura MIDI. Como mínimo, deberá incluir la
*orquestación* de la canción *You've got a friend in me* (fichero `ToyStory_A_Friend_in_me.sco`) del genial
[Randy Newman](https://open.spotify.com/artist/3HQyFCFFfJO3KKBlUfZsyW/about).

- En este triste arreglo, la pista 1 corresponde al instrumento solista (puede ser un piano, flautas,
  violines, etc.), y la 2 al bajo (bajo eléctrico, contrabajo, tuba, etc.).

  En este caso, se ha usado el InstrumentFM ya que variando parámetros puedes obtener diversos instrumentos. Concretamente, aquí se ha escogido el piano para la pista 1 y la tuba para la pista 2.
- Coloque el resultado, junto con los ficheros necesarios para generarlo, en el directorio `work/music`.
- Indique, a continuación, la orden necesaria para generar la señal (suponiendo que todos los archivos
  necesarios están en directorio indicado).

  La orden necesaria para generar la señal, dentro de `work/music`, es: 
  `synth instr.orc ToyStory_A_Friend_in_me.sco ToyStory_A_Friend_in_me.wav`

También puede orquestar otros temas más complejos, como la banda sonora de *Hawaii5-0* o el villacinco de
John Lennon *Happy Xmas (War Is Over)* (fichero `The_Christmas_Song_Lennon.sco`), o cualquier otra canción
de su agrado o composición. Se valorará la riqueza instrumental, su modelado y el resultado final.
- Coloque los ficheros generados, junto a sus ficheros `score`, `instruments` y `efffects`, en el directorio
  `work/music`.
- Indique, a continuación, la orden necesaria para generar cada una de las señales usando los distintos
  ficheros.

> NOTA:
>
> No olvide escuchar el resultado generado y comprobar que no se producen ruidos extraños o distorsiones.
> Sobre todo, tenga en cuenta la salud auditiva de quien será encargado de corregir su trabajo.
