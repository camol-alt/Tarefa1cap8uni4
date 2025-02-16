Tarefa conversores A/D:

Objetivo: 
Esse projeto tem como principal intenção a realização e compreensão dos conteúdos abordados sobre  conversores, aplicando assim os conhecimentos adquiridos sobre conversores analógicos digitais (ADC), melhorando assim a fixação de todos os conteúdos abordados. Utilizando o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick. Abordando a posição do joystick no display SSD1306 por meio de um quadrado móvel, e aplicar o protocolo de comunicação I2C na integração com o display.

Foi utilizado para realizar uma simulação:
• Placa Raspberry Pi Pico W;
• LED RGB, com os pinos conectados às GPIOs (11, 12 e 13).
• Botão do Joystick conectado à GPIO 22.
• Joystick conectado aos GPIOs 26 e 27.
• Botão A conectado à GPIO 5.
• Display SSD1306 conectado via I2C (GPIO 14 e GPIO15).


EXEMPLO DE EXECUÇÃO:

O botão do joystick deverá no eixo y acionar o LED azul e no eixo x acionar o botão vermelho. Enquanto isso, esses movimentos deverão ser apresentados no display. E quando o botão a for pressionado deverá ativar ou desativar os LEDs PWM a cada acionamento. 
E quando o botão do joystick for pressionado alterar a o estado do LED verde a cada acionamento modificando a borda do display para indicar quando for pressionado.

Explicação: 
Deste modo é constituído este programa realizando alguns processos que  contribuem para o aprimoramento dos códigos  explorando   as funcionalidades da placa de desenvolvimento BitDogLab, consolidando os conhecimentos em programação de  microcontroladores.

LINK DO VÍDEO EXPLICATIVO:
Desta forma para simplificar a compreensão produzido um vídeo de  2 minutos, explicando  brevemente as funcionalidades implementadas. E mostrando o  execução do projeto na placa bitDogLad 
......
PROJETO DESENVOLVIDO POR:
Camile Vitória Lino dos Santos.
