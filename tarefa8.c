#include <stdio.h>             
#include "pico/stdlib.h"       
#include "hardware/adc.h"     
#include "hardware/i2c.h"
#include "hardware/pwm.h"  
#include "ssd1306.h"
#include "font.h"
#include "pico/bootrom.h"
#include "hardware/timer.h"

// definição de pinos coordenadas do oled
#define X 27   
#define Y 26 

//definindo pinos botões e leds
#define joys_botton 22   
#define BOTTON_A 5

#define LED_RED 13 
#define LED_BLUE 12
#define LED_GREEN 11

//Oled
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

//definindo funções
uint pwm_init_gpio(uint gpio, uint wrap);
void oled(uint16_t x_value, uint16_t y_value, ssd1306_t *ssd, bool cor);
void callba(uint gpio, uint32_t events);
void desenho();

volatile uint32_t interrup = 0;
ssd1306_t ssd; 

//variaveis globais para controle de botões
volatile bool statu_Desenho = false;
volatile bool status_Led = true;

int main() {
    stdio_init_all();

    gpio_init(joys_botton);
    gpio_set_dir(joys_botton, GPIO_IN);
    gpio_pull_up(joys_botton); 

    gpio_init(BOTTON_A);
    gpio_set_dir(BOTTON_A, GPIO_IN);
    gpio_pull_up(BOTTON_A);

    gpio_set_irq_enabled_with_callback(joys_botton, GPIO_IRQ_EDGE_FALL, true, &callba);
    gpio_set_irq_enabled_with_callback(BOTTON_A, GPIO_IRQ_EDGE_FALL, true, &callba);


    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    uint pwm_wrap = 4096;  
    uint pwm_slice = pwm_init_gpio(LED_RED, pwm_wrap);    
    uint pwm_slice2 = pwm_init_gpio(LED_BLUE, pwm_wrap);     

    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    
    adc_init();
    adc_gpio_init(X); 
    adc_gpio_init(Y); 

    bool cor = true;
    uint16_t x_value;
    uint16_t y_value;

    uint x = 64;
    uint y = 32;

    while (true) {
        adc_select_input(0); 
        y_value = adc_read(); 
        adc_select_input(1); 
        x_value = adc_read(); 
        bool bott_value = gpio_get(joys_botton) == 0; 

        uint16_t x_value_atual = abs(adc_read()-2048)*2;
        uint16_t y_value_atual = abs(adc_read()-2048)*2;
    

        if(status_Led)pwm_set_gpio_level(LED_RED, x_value_atual); 
        if(status_Led)pwm_set_gpio_level(LED_BLUE, y_value_atual);      

        //varia de 0 a 4095 o x e y
        if (x_value > 2100) 
        {
            if(status_Led)pwm_set_gpio_level(LED_RED, x_value_atual);
            x+=8;
            if(x > 118) 
            {
                x = 118;
            }
        } 
        else if (x_value < 1700) 
        {
            if(status_Led)pwm_set_gpio_level(LED_RED, x_value_atual);
            x-=5;
            if(x < 5) 
            {
                x = 5;
            }
        } 
        else 
        {
            pwm_set_gpio_level(LED_RED, 0);
            x = 64;
        }

        if (y_value > 2100) 
        {
            if(status_Led)pwm_set_gpio_level(LED_BLUE, y_value_atual);
            y-=5;
            if(y <= 5) 
            {
                y = 5;
            }
        } 
        else if (y_value < 1700) 
        {
            if(status_Led)pwm_set_gpio_level(LED_BLUE, y_value_atual);
            y+=5;
            if(y > 54) 
            {
                y = 54;
            }
        } 
        else 
        {
            pwm_set_gpio_level(LED_BLUE, 0);
            y = 32;
        }

        oled(x, y, &ssd, cor);//desenha no oled
        if(statu_Desenho)desenho();

        sleep_ms(100);  
    }
    return 0;
}

//brilho do led
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    
    pwm_set_enabled(slice_num, true);  
    return slice_num;  
}

//oled
void oled(uint16_t x, uint16_t y, ssd1306_t *ssd, bool cor) {
    ssd1306_fill(ssd, false); // Limpa a tela
    ssd1306_rect(ssd, 0, 0, 128, 64, cor, !cor); // Desenha um retângulo preenchido
    ssd1306_draw_string(ssd, ":", x, y); // Exibe o "R" na posição desejada
    ssd1306_send_data(ssd); // Atualiza o display
}

//callback para botões
void callba(uint gpio, uint32_t events)
{

    uint32_t time_atual = time_us_32();

    if(time_atual - interrup < 2000)return;

    interrup = time_atual;

    if(gpio == joys_botton)
    {   
        gpio_put(LED_GREEN, !gpio_get(LED_GREEN));

        statu_Desenho = !statu_Desenho;
    }

    else if(gpio == BOTTON_A)
    {
        status_Led = !status_Led;
    }
}

//desenha no oled quando o botão é pressionado
void desenho()
{
    ssd1306_draw_string(&ssd, "~", 20, 1);
    ssd1306_draw_string(&ssd, "~", 40, 1);
    ssd1306_draw_string(&ssd, "~", 60, 1);
    ssd1306_draw_string(&ssd, "~", 80, 1);
    ssd1306_draw_string(&ssd, "~", 100, 1);
    ssd1306_draw_string(&ssd, "/", 20, 55);
    ssd1306_draw_string(&ssd, "/", 40, 55);
    ssd1306_draw_string(&ssd, "/", 60, 55);
    ssd1306_draw_string(&ssd, "/", 80, 55);
    ssd1306_draw_string(&ssd, "/", 100, 55);
    ssd1306_send_data(&ssd);
}