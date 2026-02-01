#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define POT_CHANNEL ADC_CHANNEL_7
#define LIGHT_CHANNEL ADC_CHANNEL_2

#define ADC_ATTEN ADC_ATTEN_DB_12
#define BITWIDTH ADC_BITWIDTH_12
#define NUM_SAMPLES 1000

#define D_SEAT GPIO_NUM_4
#define P_SEAT GPIO_NUM_5
#define D_SEATBELT GPIO_NUM_6
#define P_SEATBELT GPIO_NUM_7

#define IGNITION_LED GPIO_NUM_9
#define ENGINE_LED GPIO_NUM_10

#define BUZZER GPIO_NUM_11
#define IGNITION GPIO_NUM_12
#define LEFT_LAMP GPIO_NUM_13
#define RIGHT_LAMP GPIO_NUM_14

#define ON_LEVEL 3000
#define OFF_LEVEL 1000

#define DAYLIGHT 1250
#define DUSK 2930

#define POT_ON  2730
#define POT_OFF   1365


void app_main(void)
{
    int pot_bits;
    int light_bits;
  
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_new_unit(&init_config1, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };
    //GPIO8, 3
    adc_oneshot_config_channel
    (adc1_handle, POT_CHANNEL, &config);

    adc_oneshot_config_channel
    (adc1_handle, LIGHT_CHANNEL, &config);

    gpio_reset_pin(D_SEAT);
    gpio_set_direction(D_SEAT, GPIO_MODE_INPUT);
    gpio_pullup_en(D_SEAT);

    gpio_reset_pin(P_SEAT);
    gpio_set_direction(P_SEAT, GPIO_MODE_INPUT);
    gpio_pullup_en(P_SEAT);

    gpio_reset_pin(D_SEATBELT);
    gpio_set_direction(D_SEATBELT, GPIO_MODE_INPUT);
    gpio_pullup_en(D_SEATBELT);

    gpio_reset_pin(P_SEATBELT);
    gpio_set_direction(P_SEATBELT, GPIO_MODE_INPUT);
    gpio_pullup_en(P_SEATBELT);

    gpio_reset_pin(IGNITION);
    gpio_set_direction(IGNITION, GPIO_MODE_INPUT);
    gpio_pullup_en(IGNITION);

    gpio_reset_pin(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 0);

    gpio_reset_pin (IGNITION_LED);
    gpio_set_direction(IGNITION_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(IGNITION_LED,0);

    gpio_reset_pin(ENGINE_LED);
    gpio_set_direction(ENGINE_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(ENGINE_LED,0);
    
    gpio_reset_pin(LEFT_LAMP);
    gpio_set_direction(LEFT_LAMP,GPIO_MODE_OUTPUT);
    gpio_set_level(LEFT_LAMP,0);

    gpio_reset_pin(RIGHT_LAMP);
    gpio_set_direction(RIGHT_LAMP,GPIO_MODE_OUTPUT);
    gpio_set_level(RIGHT_LAMP,0);

    bool d_seat, p_seat; // Seat occupancy states
    bool d_belt, p_belt; // Seatbelt states

    bool ignit = false;                     //Ignition button state
    bool ignition_enabled= false;  //Indicates ignition readiness
    bool engine_running = false;   //Indicates engine running

    bool prev_state = false;
    bool last_ignit = false;
 
    bool welcome_not_shown = true;  //Ensures welcome message prints once

    while(1){

        //Read all inputs (active-low: pressed = 0)
        d_seat = (gpio_get_level(D_SEAT) == 0);
        p_seat = (gpio_get_level(P_SEAT) == 0);
        d_belt = (gpio_get_level(D_SEATBELT) == 0);
        p_belt = (gpio_get_level(P_SEATBELT) == 0);
        ignit = (gpio_get_level(IGNITION) == 0);

        adc_oneshot_read
        (adc1_handle, POT_CHANNEL, &pot_bits);
        
        adc_oneshot_read
        (adc1_handle, LIGHT_CHANNEL,&light_bits);

        // STAGE 1: ENGINE NOT RUNNING
        if (!engine_running){

            //Headlights must always be OFF
            gpio_set_level(LEFT_LAMP,0);
            gpio_set_level(RIGHT_LAMP,0);

            //Welcome message
            if (d_seat && welcome_not_shown) {
                printf("Welcome to enhanced alarm system model 218-W26\n");
                welcome_not_shown = false;
            }

            // Ignition enabled
            if (d_seat && p_seat && d_belt && p_belt){
                ignition_enabled = true;
                gpio_set_level(IGNITION_LED,1);
            }
            else {
                ignition_enabled = false;
                gpio_set_level(IGNITION_LED,0);
            }

            //Ignition pressed
            if (ignit && !last_ignit){
                // Case 1: All safety conditions met
                if (ignition_enabled){
                    engine_running = true;
                    gpio_set_level(ENGINE_LED,1);
                    gpio_set_level(IGNITION_LED,0);
                    printf("Engine started\n");
                }
                // Case 2: Safety conditions not met
                else {
                    gpio_set_level(BUZZER, 1);
                    printf("Ignition inhibited\n");

                if (!d_seat){
                    printf("Driver seat not occupied\n");
                }
                    
                if (!p_seat){
                    printf("Passenger seat not occupied\n");
                }
                if (!d_belt){
                    printf("Driver's seatbelt not fastened\n");
                }
                if (!p_belt){
                    printf("Passenger's seatbelt not fastened\n");
                }
                
                //delay_ms(500);
                //gpio_set_level(BUZZER,0);
            }
        }
    }
    //STAGE 2: ENGINE RUNNING
    else{   
            //STOP ENGINE
            if (ignit && !last_ignit){
                engine_running = false;
                gpio_set_level(ENGINE_LED, 0);
                printf("car stopped\n");
            }

            //HEADLIGHTS SUBSYSTEM
            if (pot_bits >= POT_ON) {
                gpio_set_level(LEFT_LAMP, 1);
                gpio_set_level(RIGHT_LAMP, 1);
            }
            if (pot_bits <= POT_OFF) {
                gpio_set_level(LEFT_LAMP, 0);
                gpio_set_level(RIGHT_LAMP, 0);
            }
            
            else {
                //AUTO MODE
                if (light_bits > DAYLIGHT){
                    vTaskDelay(2000/portTICK_PERIOD_MS);
                    gpio_set_level(LEFT_LAMP, 1);
                    gpio_set_level(RIGHT_LAMP, 1);
                    prev_state = false;
                }
                else if (light_bits < DUSK){
                    vTaskDelay(1000/portTICK_PERIOD_MS);
                    gpio_set_level (LEFT_LAMP,0);
                    gpio_set_level (RIGHT_LAMP,0);
                    prev_state = true;
                }
                else {
                    gpio_set_level (LEFT_LAMP, prev_state);
                    gpio_set_level (RIGHT_LAMP, prev_state);
                    }
                }
            }
            last_ignit=ignit;
            vTaskDelay(25/portTICK_PERIOD_MS);
        }
    }

