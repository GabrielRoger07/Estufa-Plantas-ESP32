# ESP 1 - Modo Energia
A placa identificada como ESP 1 utiliza 4 sensores para o controle da temperatura, umidade, indicação do estado do sistema de irrigação da estufa e alertas de temperatura e umidade. Assim, segue anexo 
duas imagens com a foto de como o a placa e os sensores foram montados, bem como uma imagem da pinagem da placa.

O primeiro LED RGB fica verde com a umidade entre 50% e 35% e com a temperatura entre 22°C e 35°C, caso algum desses parâmetros fique fora da sua respectiva, faixa o LED fica vermelho e o Buzzer toca, 
sendo representado também no Widget "Alarme Temperatura e Umidade", em que aparece o valor 1, caso o Buzzer toque e 0 o contrário. 

Já o segundo LED RGB é utilizado para representar o controle da intensidade do sistema de irrrigação (0 a 100%), 
por meio do Widget "Controle Intensidade Irrigação". Por fim, a temperatura e a umidade são atualizados no gráfico de linha "Temperatura e Umidade".

# Observação

Para uma montagem mais organizada e clean, o LED RGB de irrigação está apenas com o pino de - e o pino de G (cor verde) setados, portanto está sendo usado apenas 2 jumpers para conectá-lo na protoboard.

# Placa montada com os sensores 
![image1](/assets/esp1.png)

# Pinagem da placa ESP32
![image2](/assets/espPinos.jpg)
