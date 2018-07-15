# BallSeeker
El presente trabajo consiste en el desarrollo de un robot móvil que sea capaz de buscar y recolectar pelotas de un color determinado, y llevarlas a un espacio delimitado (meta). La recolección de pelotas lo hará el robot por sí sólo, él será capaz de identificar el color de la pelota, dirigirse hacia ella y guiarla hacia el objetivo. 

Para lograr lo antes propuesto, abordo del robot está un microcontrolador DemoQE128 que envia señales a los motores para avanzar, mientras que una cámara colocada en lo alto del lugar trabaja con un programa que identifica los objetivos, los obstáculos y al robot, envía a un servidor las coordenadas de cada elemento en la cancha y un programa en la PC (pathplanner) toma estos datos y realiza la planificación de la trayectoria que el carro debe seguir para alcanzar los objetivos y guiarlos a la meta evitando obstáculos. Establecida la trayectoria, el pathplanner le envía al carro vía Bluetooth el porcentaje de duty cycle que debe tener cada rueda.

Para ver una muestra de su funcionamiento:

[![Funcionamiento del carro](https://img.youtube.com/vi/nvuCGQraHNY/0.jpg)](https://www.youtube.com/watch?v=nvuCGQraHNY)

Este proyecto tuvo un par de etapas antes de su culminación
## Avanzar en línea recta
  Se debe determinar cual es el porcentaje de duty cycle que debe aplicarsele a cada motor para que en conjunto avancen y retrocedan en línea recta.
### Acercarse y alejarse de objetivo
   Con la utilización de un sensor de proximidad fue posible lograr que el carro se acerque a un objeto hasta hallarse a una distancia de apoximadamente 6cm y mantener esa distancia de separación, retrocede o avanza según sea necesario. Para ver el código utilizado ver en la carpeta de Adelante-Atras.
### Seguir color "perrito"
   El robot con una cámara abordo es capaz de seguir una pelota de un color determinado, se alínea en la dirección en la que la encuentra, se acerca ella y mantiene una distancia de separación constante.
