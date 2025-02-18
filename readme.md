# Conversor ADC com Raspberry Pi Pico W

Este projeto implementa um conversor ADC utilizando o microcontrolador Raspberry Pi Pico W. O projeto lê valores de um joystick analógico e exibe as coordenadas em um display OLED SSD1306 via I2C. Além disso, controla LEDs RGB utilizando PWM.

## Arquivos do Projeto

- `Conversor_ADC.c`: Código principal do projeto.
- `ssd1306_i2c.h`: Header file para controle do display SSD1306 via I2C.
- `CMakeLists.txt`: Arquivo de configuração do CMake para compilação do projeto.
## Video
https://youtu.be/xuIkKoTBonc
## Funcionalidades

- Leitura de valores analógicos do joystick.
- Cálculo da média móvel dos valores lidos.
- Controle de LEDs RGB via PWM.
- Exibição das coordenadas do joystick em um display OLED SSD1306.
- Alteração do estilo da borda do display ao pressionar o joystick.
- Habilitação/desabilitação do PWM ao pressionar o botão A.

## Configuração do Hardware

- **Pinos do Joystick:**
    - `Joy_X`: GPIO 26
    - `Joy_Y`: GPIO 27
    - `JOYSTIC`: GPIO 22 (Botão do joystick)
- **Pinos dos LEDs RGB:**
    - `GREEN_PIN`: GPIO 11
    - `BLUE_PIN`: GPIO 12
    - `RED_PIN`: GPIO 13
- **Pinos do Display OLED SSD1306:**
    - `I2C_SDA`: GPIO 14
    - `I2C_SCL`: GPIO 15
- **Botão A:**
    - `BTN_A`: GPIO 5

## Compilação e Execução

1. Certifique-se de ter o SDK do Raspberry Pi Pico instalado.
2. Clone o repositório do projeto.
3. Navegue até o diretório do projeto.
4. Execute os seguintes comandos para compilar e carregar o código no Raspberry Pi Pico W:

```sh
mkdir build
cd build
cmake ..
make
```

5. Carregue o arquivo `.uf2` gerado no Raspberry Pi Pico W.

## Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e pull requests.