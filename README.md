# VARIACIONES SOBRE EL AJEDREZ KRAMNIK

JUEGO AJEDREZ Y SUS VARIACIONES KRAMNIK

-La normativa especifica del trabajo explicita que se debe incluir la posibilidad de comer sus propias piezas (ajedrez KRAMNIK). Ademas cada grupo tiene asignado una u varias variantes de tablero y de normas especificas; nuestro grupo implementará las reglas del ajedres DEMI y del ajedrez SILVERBULLET 4X5 (así como el juego clasico de ajedrez). Desde el equipo hemos decidido incluir estas 3 variantes, con la posibilidad para cada una de implementar la regla KRAMNIK en cada una de ellas. 

-El ajedrez DEMI consiste en un tablero 4x8 con la mitad derecha del tablero clasico, 1es decir REY - ALFIL - CABALLO - TORRE y una fila de peones. No se puede enrocar, y la coronacion excluye la reina.

-El ajedrez SILVERBULLET 4X5 consiste en un tablero 4x5 con las piezas siguientes: TORRE - REINA - REY - TORRE  y una fila de peones. No se puede enrocar y la coronacion solo permite torre y reina.


# Objetivos del trabajo:

El principal objetivo de este proyecto es entender los principios de la programación en C++, a la vez que pasarlo bien creando un proyecto interesante sobre el juego de la ajedrez y que nos acerca un poco más a la realidad de lo que es la informática en cierta parte. Además, podremos empezar a desarrollar nuestros propios proyectos con mayor conocimiento y habilidad, ya sea con un fin didáctico o personal.


# Organización del proyecto:

El proyecto se llama "MyChessProject" y está ambientado en un ajedrez medieval de piedra, y tiene incorporado texturas y formas innovadoras, también la vista del ojo va girando según pasa cada turno para ofrecer al usuario esa sensación de primera persona y una experiencia más cercana.

Comenzaremos describiendo la arquitectura del sistema y las estructuras de datos, seguido de una descripción de los algoritmos utilizados para la validación de movimientos y el juego de la IA. Finalmente, presentaremos los resultados de las pruebas y discutiremos posibles vías para el desarrollo futuro.

Al compilar el código, lo primero que se nos abre es una pantalla conocida como menú principal, en la cual podremos seleccionar el tipo y la variante del ajedrez que el usuario quiera jugar.

Seleccionar modo de juego:
1.	Classic
2.	Classic Kramnik
3.	Demi
4.	Demi Kramnik
5.	Silverbullet
6.	Silverbullet Kramnik


Una vez se selecciona uno de los modos, distinguiendo entre las variantes Kramnik y normales, se pide al usuario que seleccione quien va a jugar la partida, entre las siguientes opciones, quedando el menú de la siguiente manera:

Tipo de Partida
•	Jugador VS Jugador
•	Jugador VS IA



Si se selecciona el modo de Jugador vs jugador, la partida esta lista para comenzar pues no hay más configuración del juego posible. Sin embargo, en el caso de que el usuario elija la opción “Jugador VS IA”, el sistema le preguntara al usuario con que color va a desear jugar el, con un menú con la siguiente estructura:

Elegir color de las piezas del Jugador
•	Blancas
•	Negras


Una vez el usuario elige, también se le da a elegir entre los diferentes grados de dificultad de la IA, que desarrollaremos más adelante, para afrontar más o menos desafío:

Seleccione dificultad
•	Fácil 
•	Medio
•	Difícil
•	Maestro


Una vez configuradas las opciones del menú principal comienza la partida con el modo, los jugadores, la dificultad y el color seleccionado.




# Integrantes del equipo:

Mario Fernández Liétor    mario.fernandez.lietor@alumnos.upm.es

Victor Bressy Sanz        victor.bressy@alumnos.upm.es

Remi Garcia Girard        remi.garcia@alumnos.upm.es

Marcos Rodríguez Pena     m.rpena@alumnos.upm.es

Daniel Lobera Rodriguez   daniel.lobera@alumnos.upm.es

