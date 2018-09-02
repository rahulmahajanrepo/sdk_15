#include "motor.h"

void pwm_ready_callback(uint32_t pwm_id) 
{
    pwmReady = true;
}


/* stop: sudden stop of both motors */
void stop()
{
  // set direction A
  nrf_gpio_pin_set(pinIN1);
  nrf_gpio_pin_set(pinIN2);
    
  // set direction B
  nrf_gpio_pin_set(pinIN3);
  nrf_gpio_pin_set(pinIN4);

  stopped = true;
}

/* set_speed: set speed for motor 0/1 */
void set_speed(int motor, uint8_t speed)
{
  // error check
  if (motor < 0 || motor > 1)
    return;

  // set speed
  while (app_pwm_channel_duty_set(&PWM1, motor, speed) == NRF_ERROR_BUSY);
  motor_speeds[motor] = speed; 

  stopped = false;
}


/* turn: turn in given direction for x milliseconds */
void turn(bool left, int tms)
{
  if(left) {
    // stop motor 0
    int tmp = motor_speeds[0];
    set_speed(0, 0);
    set_speed(1, 50);
    // wait
    nrf_delay_ms(tms);
    // reset 
    set_speed(0, tmp);
    set_speed(1, tmp);
  }
  else {
    // stop motor 1
    int tmp = motor_speeds[1];
    set_speed(1, 0);
    set_speed(0, 50);
    // wait
    nrf_delay_ms(tms);
    // reset 
    set_speed(0, tmp);
    set_speed(1, tmp);
  }
}

/* direction: change motor direction */
void set_dir(bool forward)
{
  if(forward) {
    // set direction A
    nrf_gpio_pin_set(pinIN1);
    nrf_gpio_pin_clear(pinIN2);
    // set direction B
    nrf_gpio_pin_set(pinIN3);
    nrf_gpio_pin_clear(pinIN4);
  }
  else {
     // set direction A
    nrf_gpio_pin_clear(pinIN1);
    nrf_gpio_pin_set(pinIN2);
    // set direction B
    nrf_gpio_pin_clear(pinIN3);
    nrf_gpio_pin_set(pinIN4);
  }
  curr_dir = forward;
}


// make a move autonomously
void auto_move()
{
    // get HC-SR04 distance
    float dist = 1.0;
    if(getDistance(&dist)) {
#if 0         
        // send distance via NUS
        uint8_t str[4];
        sprintf((char*)str, "%f cm", dist);
        ble_nus_string_send(&m_nus, str, strlen((char*)str));
#endif

        // obstacle avoidance
        if (dist < 20) {
         
            // stop 
            stop();

            // reverse 
            set_dir(false);
            set_speed(0, 50);
            set_speed(1, 50);
            nrf_delay_ms(1000);

            // turn left 
            turn(true, 500);

            // go
            set_dir(true);
            set_speed(0, 80);
            set_speed(1, 80);
        }
    }
}