#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

// 20251RSE.MTC0019

// Pino responsável pelo eixo vertical ( > 3500: alta; 1600 a 1800: moderada; < 1600: baixa)
const int VRY = 27;
// Pino responsável pelo LED vermelho
const uint red_pin = 13;
// Pino responsável pelo LED verde
const uint green_pin = 11;
// Pino responsável pelo LED azul
const uint blue_pin = 12;
// Canal 0 do ADC
const int ADC_CHANNEL_0 = 0;
// Pino responsável pelo Buzzer
const uint buzzer_pin = 21;
// Define a frequência inicial do Buzzer
#define BUZZER_FREQUENCY 120

// Variável que recebe a flag da tarefa a ser executada (1 - alta, 2 - moderada ou 3 - baixa)
volatile int flag;
volatile int task = 0;
// Variável que recebe o valor do joystick
uint16_t vry_value;

// Inicializa o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.0f); // Ajusta divisor de clock
    pwm_init(slice_num, &config, true);
    pwm_set_gpio_level(pin, 0); // Desliga o PWM inicialmente
}

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
    gpio_init(blue_pin);
    gpio_set_dir(blue_pin, GPIO_OUT);
    // Inicializa o ADC
    adc_init();
    // Inicializa e configura o eixo vertical no ADC
    adc_gpio_init(VRY);
    pwm_init_buzzer(buzzer_pin);
}

// Definição de uma função para emitir um beep com duração especificada
void beep(uint duration_ms) {
    uint frequency = 400;
    uint slice_num = pwm_gpio_to_slice_num(buzzer_pin);
    uint32_t clock_freq = clock_get_hz(clk_sys);
    uint32_t top = clock_freq / frequency - 1;

    pwm_set_wrap(slice_num, top);
    pwm_set_gpio_level(buzzer_pin, top / 2); // 50% de duty cycle

    sleep_ms(duration_ms);

    pwm_set_gpio_level(buzzer_pin, 0); // Desliga o som após a duração
    sleep_ms(50); // Pausa entre notas
}

int64_t task_scheduler_callback(alarm_id_t id, void *user_data)
{
    printf("Enviando tarefa %d para o core 1...\n", task);
    multicore_fifo_push_blocking(task);

    task++;
    // Pino responsável pelo eixo vertical ( > 3500: alta; 1600 a 1800: moderada; < 1600: baixa)
    if (vry_value > 2500) // Atividade alta
    {
        flag = 3;
    }
    else if (vry_value <= 1600) // Atividade moderada
    {
        flag = 1;
    }
    else // atividade baixa
    {
        flag = 2;
    }

    // Programa para a task ser chamada sempre a cada 2 segundos (2.000.000 microssegundos)
    return 2000 * 1000;
}

void core1_entry()
{
    int task;
    while (true)
    {
        // Recebe o valor da tarefa do Core 0
        task = multicore_fifo_pop_blocking();

        printf("Executando Tarefa %d no core 1...\n\n", task);

        // desliga todos os LEDS
        gpio_put(red_pin, 0);
        gpio_put(green_pin, 0);
        gpio_put(blue_pin, 0);

        sleep_ms(100);

        if (flag == 3) // Alta
        {
            printf("Atividade Alta\n");
            gpio_put(red_pin, 1);

            beep(500);
            
        }
        else if (flag == 2) // Moderada
        {
            printf("Atividade Moderada\n");
            gpio_put(blue_pin, 1);
        }
        else if (flag == 1) // Baixa
        {
            printf("Atividade Baixa\n");
            gpio_put(green_pin, 1);
        }

        sleep_ms(1000);
        printf("Tarefa %d executada no core 1...\n\n", task);
    }
}

int main()
{
    init_setup();

    add_alarm_in_ms(2000, task_scheduler_callback, NULL, true);

    multicore_launch_core1(core1_entry);

    while (true)
    {
        adc_select_input(ADC_CHANNEL_0);
        sleep_ms(2);
        vry_value = adc_read();

        sleep_ms(1000);
        
        tight_loop_contents();
    }
}
