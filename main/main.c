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

#define GREEN GPIO_NUM_9
#define BLUE GPIO_NUM_10

#define BUZZER GPIO_NUM_11
#define IGNITION GPIO_NUM_12
#define LAMP GPIO_NUM_13

void delay_ms(int t);
#define LOOP_DELAY_MS 25
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

    gpio_reset_pin(GREEN);
    gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN,0);

    gpio_reset_pin(BLUE);
    gpio_set_direction(BLUE, GPIO_MODE_OUTPUT);
    gpio_set_level(BLUE,0);
    
    gpio_reset_pin(LAMP);
    gpio_set_direction(LAMP,GPIO_MODE_OUTPUT);
    gpio_set_level(LAMP,0);

    bool d_seat, p_seat; // Seat occupancy states
    bool d_belt, p_belt; // Seatbelt states
    bool ignit;          //Ignition button state
    bool green_enabled= false;  //Indicates ignition readiness
    bool blue = false;   //Indicates engine running

    bool headlight = false;   //
    bool daylight, dusk;

    bool welcome_not_shown = true;  //Ensures welcome message prints once
    bool ignition_used = false;     //Prevents multiple ignition attempts
    bool lamp = false;
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


        if (d_seat && welcome_not_shown) {
             printf("Welcome to enhanced alarm system model 218-W26\n");
             welcome_not_shown = false;
        }
        // Ignition enabled
        // Green LED is ON only if:
        // - both seats are occupied
        // - both seatbelts are fastened
        // - ignition has not been used yet
        if (d_seat && p_seat && d_belt && p_belt && !ignition_used ){
            gpio_set_level(GREEN,1);
            green_enabled = true;
        }
        else {
            gpio_set_level(GREEN,0);
            green_enabled = false;
        }

        //Ignition pressed
        if (ignit && !ignition_used){
            ignition_used = true;

            // Case 1: All safety conditions met
            if (green_enabled){
                gpio_set_level(GREEN,0);
                gpio_set_level(BLUE,1);
                blue = true;
                printf("Engine started\n");
              
                }


                if (pot_mV > threshold){
                    gpio_set_level(LAMP,1);
                }
                if (pot_bits < threshold){
                    gpio_set_level(LAMP,0);
                }
                
                else{
                    if (pot_bits > light_bits){
                        delay_ms (2);
                        gpio_set_level(LAMP, 0);
                    }
                    if (headlight < light_bits){
                        delay_ms (1);
                        gpio_set_level (LAMP,1);
                  
                    }
            

                }
                if (ignit){
                    blue = false;
                }
            }
            // Case 2: Safety conditions not met
            else {
                printf("Ignition inhibited\n");
                gpio_set_level(BUZZER, 1);

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
                ignit = gpio_set_level(IGNITION,1);
                }
            }
            if (blue && (!d_seat) + (!p_seat)+(!d_belt)+(!p_belt)){
                blue = true;
            }
        
        //Small delay to limit polling rate
        delay_ms(25);
    }
}

void delay_ms(int t) {
    vTaskDelay(t /portTICK_PERIOD_MS);
} 