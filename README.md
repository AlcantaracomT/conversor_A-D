# Controle de LEDs e Desenho no OLED com Joystick e Botões

## Descrição

Este código implementa o controle de LEDs e a exibição de desenhos em um display OLED SSD1306 utilizando um microcontrolador Raspberry Pi Pico W. Ele lê os valores de um joystick analógico (utilizando entradas de ADC) e botões (usando interrupções GPIO) para controlar a posição de um caractere no display OLED. Além disso, a intensidade de dois LEDs (vermelho e azul) é ajustada com base nos valores lidos do joystick.

## Componentes Necessários

- **Raspberry Pi Pico** ou qualquer placa compatível com o chip RP2040.
- **OLED SSD1306** (128x64 pixels, comunicação I2C).
- **Joystick analógico** (2 eixos) para controle de movimento.
- **Botões físicos** para interação.
- **LEDs RGB** para indicar o estado do sistema.

---
### Video
link: https://www.youtube.com/watch?v=HV-YIElahpg
---
## Pinos Utilizados

### Joystick:
- **Eixo X**: GPIO 27
- **Eixo Y**: GPIO 26
- **Botão do Joystick**: GPIO 22

### Botões:
- **Botão A**: GPIO 5
- **Botão B**: GPIO 6

### LEDs:
- **LED Vermelho**: GPIO 13 
- **LED Azul**: GPIO 12 
- **LED Verde**: GPIO 11

### OLED SSD1306:
- **SDA**: GPIO 14
- **SCL**: GPIO 15

---

## Descrição das Funções

### `main()`
- Inicializa os pinos GPIO, configuração do display OLED via I2C e a leitura do joystick.
- Controle de LEDs com base na posição do joystick.
- Desenho do caractere no display OLED.
- Interrupções para a leitura de botões e controle de LEDs e desenho.

### `pwm_init_gpio()`
- Inicializa o controle PWM para os LEDs, permitindo ajustar a intensidade de cada LED.

### `oled()`
- Desenha um caractere (representado por dois pontos ":") na posição indicada no display OLED.

### `callba()`
- Função de callback para tratar as interrupções dos botões, alternando entre estados de desenho no display e controle de LEDs.

### `desenho()`
- Desenha uma série de símbolos no display OLED quando o botão do joystick é pressionado.
