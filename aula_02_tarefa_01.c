#include <stdio.h>
#include "pico/stdlib.h"

// Pino responsável pelo eixo horizontal
const int VRX = 26;
// Pino responsável pelo eixo vertical
const int VRY = 27;
// Pino responsável pelo LED vermelho
const uint red_pin = 13;
// Pino responsável pelo LED verde
const uint green_pin = 11;
// Pino responsável pelo LED azul
const uint blue_pin = 12;
// Canal 0 do ADC
const int ADC_CHANNEL_0 = 0;
// Canal 1 do ADC
const int ADC_CHANNEL_1 = 1;

void init_setup()
{
    // Inicializa as portas de console serial
    stdio_init_all();
    // Inicializa o pino do LED vermelho e o configura como saída
    gpio_init(red_pin);
    gpio_set_dir(red_pin, GPIO_OUT);
    // Inicializa o pino do LED verde e o configura como saída
    gpio_init(green_pin);
    gpio_set_dir(green_pin, GPIO_OUT);
    // Inicializa o pino do LED azul e o configura como saída
    gpio_inti(blue_pin);
    gpio_set_dir(blue_pin, GPIO_OUT);
    // Inicializa o ADC
    adc_init();
    // Inicializa e configura o eixo horizontal no ADC
    adc_gpio_init(VRX);
    // Inicializa e configura o eixo vertical no ADC
    adc_gpio_init(VRY);
}

int main()
{
    init_setup();

    while (true)
    {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
