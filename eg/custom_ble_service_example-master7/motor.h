#include "nordic_common.h"
#include "nrf.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf51_bitfields.h"
#include "nrf_delay.h"
#include "app_timer.h"
#include "app_pwm.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"

#define FORWARD "FastForward"
#define REWIND "Rewind"
#define STOP "Stop"
#define PAUSE "Pause"
#define PLAY "Play"
#define START "Start"
#define END "End"
#define RECORD "Rec"
#define SHUFFLE "Shuffle"

// current motors speeds
int motor_speeds[] = {80, 80};
// current direction
int curr_dir = true;
// currently stopped
bool stopped = true;

// events
typedef enum _BBEventType {
    eBBEvent_Start,
    eBBEvent_Stop,
    eBBEvent_Reverse,
    eBBEvent_Left,
    eBBEvent_Right,
} BBEventType;


// structure handle pending events
typedef struct _BBEvent
{
    bool pending;
    BBEventType event;
    int data;
} BBEvent;

BBEvent bbEvent;


// A flag indicating PWM status.
static volatile bool pwmReady = false;            

// enable motor A - PWM for speed control
uint32_t pinENA = 1;
// direction control for motor A
uint32_t pinIN1 = 2;
uint32_t pinIN2 = 3;
// enable motor B - PWM for speed control
uint32_t pinENB = 4;
// direction control for motor B
uint32_t pinIN3 = 5;
uint32_t pinIN4 = 6;

// PWM callback function
void pwm_ready_callback(uint32_t pwm_id);    

/* stop: sudden stop of both motors */
void stop();

/* set_speed: set speed for motor 0/1 */
void set_speed(int motor, uint8_t speed);
/* turn: turn in given direction for x milliseconds */
void turn(bool left, int tms);
/* direction: change motor direction */
void set_dir(bool forward);

// make a move autonomously
void auto_move();