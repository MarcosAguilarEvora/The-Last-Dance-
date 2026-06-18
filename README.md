# The-Last-Dance-
⚽ Introducción al Proyecto
Este proyecto consiste en el desarrollo de un videojuego arcade completo en 2D con un estilo divertido, paródico y una fuerte temática futbolística. El diseño del juego se inspira en los clásicos shoot 'em up horizontales (como Space Invaders), donde el usuario se enfrenta a mecánicas de esquivar y disparar en un ritmo rápido y constante.

El objetivo final para que el jugador complete la aventura es derrotar de forma consecutiva a los 5 jefes del juego, acumulando un total de 100 puntos de victoria.

🕹️ Mecánicas Principales y Controles
La acción se desarrolla en una sola pantalla donde el jugador controla a un futbolista posicionado siempre en la banda inferior. En la parte superior de la pantalla vuelan los enemigos, desplazándose constantemente de izquierda a derecha mientras bombardean la zona baja con distintos proyectiles.

Controles del Teclado
El mapeo de controles está diseñado para ser directo y responsivo:

Flechas Izquierda (←) y Derecha (→): Permiten el desplazamiento horizontal del personaje por el suelo.

Flecha Arriba (↑): Activa la mecánica de salto para esquivar obstáculos a ras de piso.

Barra Espaciadora (Space): Genera el disparo de balones de fútbol hacia arriba.

Atributos del Disparo
Para mantener una jugabilidad dinámica y frenética, los balones que lanza el jugador son ilimitados y no cuentan con un tiempo de recarga o cooldown. Esto significa que se puede disparar continuamente en ráfagas mientras el personaje se mueve o salta al mismo tiempo. Cada balón que logre impactar en el cuerpo del enemigo flotante le restará automáticamente 1 punto de vida.

Sistema de Vidas y Derrota
El jugador inicia la partida con un contenedor de 3 vidas. Cada vez que un proyectil enemigo lo alcanza, pierde 1 vida; si el contador llega a cero, el juego termina inmediatamente en una pantalla de Game Over.

Sin embargo, existe una regla especial: si el jugador llega a tocar un objeto de "Tarjeta Roja", sufrirá una expulsión y derrota instantánea, finalizando la partida sin importar cuántas vidas le quedaran en ese momento.

👥 Personajes Jugables
Antes de saltar a la cancha, el juego ofrece una pantalla de selección con 6 personajes basados en nombres e íconos del fútbol. Es importante destacar que el cambio entre ellos es estrictamente estético (skins visuales); ninguno cuenta con estadísticas alteradas o habilidades especiales, garantizando que el balance del juego se mantenga idéntico.

Los futbolistas disponibles son:

Chicharrón

Cuau

Funesmorri

Giño

Lugo Sánchez

Chaqueta Giménez

👹 Estructura de Niveles y Enemigos
El juego se divide en 5 niveles ascendentes. Cada nivel cuenta con su propio escenario visual y está resguardado por un jefe flotante único que posee animaciones personalizadas y un patrón de ataque temático. Todos los jefes tienen 20 puntos de vida (HP) y otorgan 20 puntos como recompensa al ser eliminados.

Nivel 1 - Katie Itzel: Es el encuentro inicial. Se mueve por el cielo y ataca arrojando silbatos de árbitro hacia abajo.

Nivel 2 - Gata Ortencia: Mantiene una dinámica similar al primer nivel, atacando al jugador también mediante el lanzamiento de silbatos.

Nivel 3 - Telecomerciales: La dificultad empieza a escalar. Este jefe bombardea la pantalla lanzando fajos de dinero desde el cielo.

Nivel 4 - Funko Arreola: Introduce un ataque temático lanzando flechas verdes y rojas de ascenso y descenso que el jugador debe aprender a esquivar.

Nivel 5 - Mafia Mayor: El jefe final del juego. Ataca lanzando grandes cantidades de dinero continuamente. Su velocidad de movimiento es la más alta de la partida.

💎 Sistema de Objetos Especiales
Durante el juego, aparecerán objetos del cielo de forma completamente aleatoria o como recompensa inmediata al derrotar a un jefe de nivel. Estos objetos caen y se activan únicamente cuando el sprite del jugador colisiona con ellos.

Objetos Buenos (Power-ups)
Taquetes de fútbol: Incrementan la velocidad de movimiento horizontal del futbolista durante 5 segundos.

Espinilleras: Otorgan un estado de invencibilidad total contra cualquier proyectil por 5 segundos.

Guantes de portero: Brindan el mismo estado de invencibilidad total, pero con una duración extendida de 10 segundos.

Banda de capitán: El objeto más poderoso del juego. Durante 15 segundos otorga velocidad aumentada, invencibilidad y activa el Disparo Triple. Con este disparo, el personaje lanza un balón recto hacia arriba y dos balones en ángulos diagonales, multiplicando el daño potencial al jefe.

Objetos Malos (Debuffs)
Tarjeta amarilla: Penaliza al jugador congelándolo por completo en su posición durante 5 segundos, impidiéndole moverse o esquivar.

Tarjeta roja: Provoca la derrota automática y el fin de la partida por expulsión.

🖥️ Interfaz de Usuario y Pantallas del Juego
El flujo de la aplicación se gestiona mediante un sistema de estados que divide el juego en varias pantallas interactivas:

Pantalla de Inicio: El menú principal que muestra el logotipo del juego y los accesos para Jugar, Seleccionar Personaje, Ver Enemigos (una galería tipo bestiario con los datos de los jefes) y Salir de la aplicación.

HUD (Interfaz en juego): Una barra de información que permanece visible durante el nivel. Muestra los puntos acumulados, el número de nivel actual, las vidas del jugador, los puntos de vida restantes del jefe, el power-up que se encuentra activo y un seguidor de tiempo con los segundos restantes de dicho poder.

Pantallas de Transición y Cierre: Incluye la pantalla de "Nivel Completado" (con el resumen de puntos, vidas y un botón para avanzar), la "Pantalla de Victoria" al alcanzar los 100 puntos (que registra el tiempo total que tardaste en ganar) y la "Pantalla de Derrota" que muestra el puntaje alcanzado y ofrece un botón para reiniciar la partida o volver al menú.