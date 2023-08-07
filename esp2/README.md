# ESP 2 - Modo Bateria
A placa identificada como ESP 2 foi implementada no modo bateria, ou seja, entra no modo deep sleep a cada 1 minuto e é acordada ao presisonar o botão BOOT da esp32, definido no pino 0 da GPIO no arquivo 
[pinosGpio.h](https://github.com/FGA-FSE/trabalho-final-2023-1-gabrieljoao/blob/main/esp2/main/pinosGpio.h). Nesse sentido a placa foi implementada juntamente com 3 sensores, sendo eles o Encoder para controlar
a intensidade do LED RGB, o próprio LED que representa o nível de iluminação da estufa (via PWM) e outro Encoder para controlar a intensidade da ventilação da estufa.

Ao girar os dois Encoders, a intensidade do LED e da ventilação são printados no terminal e armazenados na memória flash da ESP, assim como a quantidade de vezes que a placa foi acordada. Isso tudo é representado no dashboard nos Widgets "Intensidade da Luz", "Intensidade da Ventilação", 
"Quantidade de Wakes Up", respectivamente. Vale ressaltar que a quantidade de Wakes Up não é perdida quando a ESP entra no modo sleep.

Todos os atributos do projeto são enviados ao servidor central são armazenados na memória flash da placa.

# Placa montada com os sensores 
![image1](/assets/esp2.png)

# Pinagem da placa ESP32
![image2](/assets/espPinos.jpg)
