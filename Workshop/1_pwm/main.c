/*
========================================================
 PWM CONTROL
--------------------------------------------------------
 pwm0_set_percent(0);    // Stop motor (0% duty)
 pwm0_set_percent(50);   // Half speed (50% duty)
 pwm0_set_percent(100);  // Full speed (100% duty)
========================================================
*/

#include <stdio.h>
#include <util/delay.h>
#include "usart.h"
#define F_CPU 16000000UL
#include <avr/io.h>

/*
--------------------------------------------------------
 Initialize Timer0 for Fast PWM on PD6 (OC0A)
--------------------------------------------------------
 PD6 corresponds to OC0A according to the Port D alternate function table
 (Datasheet p.69–71, Table 13-9 "Alternate Port Functions").

 Fast PWM uses WGM01:0 = 1,1 and WGM02 = 0 for TOP = 0xFF
 (Datasheet p.84, Table 14-8 "Waveform Generation Mode Bit Description").

 Non-inverting output on OC0A requires COM0A1:0 = 1,0
 (Datasheet p.84, Table 14-3 "Compare Output Mode, Fast PWM").

 Prescaler 64 corresponds to CS02:0 = 0,1,1
 (Datasheet p.86–87, Table 14-9 "Clock Select Bit Description").

 PWM frequency formula: f_PWM = f_clk / (N × 256)
 (Datasheet p.80, "Timer/Counter Clock Sources").
*/

static void pwm0_init_pd6(void)
{
    DDRD  |= (1 << DDD6);                // Set PD6 as output (OC0A pin) — Datasheet p.69–71
    TCCR0A = (1 << WGM01) | (1 << WGM00) // Fast PWM mode (WGM01:0 = 1,1) — p.84 Table 14-8
           | (1 << COM0A1);              // Non-inverting on OC0A (COM0A1:0 = 1,0) — p.84 Table 14-3
    TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64 (CS02:0 = 0,1,1) — p.86–87 Table 14-9
    OCR0A   = 51;                        // ~20% duty (0.20 × 255 ≈ 51) — OCR0A sets compare value p.86
}

/*
--------------------------------------------------------
 Set PWM duty cycle in percent (0–100)
--------------------------------------------------------
 OCR0A is compared with TCNT0 each cycle (Datasheet p.83–84 "Compare Match Output Unit").
 Updating OCR0A changes duty instantly on the next cycle (double-buffered register).
*/

static void pwm0_set_percent(uint8_t p)
{
    if (p > 100) p = 100;                // Limit to 100% — avoids overflow
    OCR0A = (uint8_t)((p * 255UL) / 100UL); // Scale 0–100% → 0–255 — Datasheet p.86 (OCR0A description)
}

/*
--------------------------------------------------------
 MAIN LOOP
--------------------------------------------------------
 Calls pwm0_init_pd6() once to start PWM hardware.
 PD6 outputs a ~976 Hz PWM signal continuously.
 Change motor speed by calling pwm0_set_percent(x).
*/

int main(void)
{
    pwm0_init_pd6();                     // Start PWM on PD6 (OC0A)
    while (1)
    {
    pwm0_set_percent(60);          // 60% duty cycle (test value)
    }
}
