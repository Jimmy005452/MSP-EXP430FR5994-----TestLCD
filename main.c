#include "grlib.h"
#include "LcdDriver/Sharp128x128.h"
#include "driverlib.h"
#include "stdint.h"

void clockInit(void);
void boardInit();
void Delay(void);

Graphics_Context g_sContext;

#if defined(__IAR_SYSTEMS_ICC__)
int16_t __low_level_init(void) {
    // Stop WDT (Watch Dog Timer)
    WDTCTL = WDTPW + WDTHOLD;
    return(1);
}

#endif


#pragma PERSISTENT(numbers)
char *numbers = "0123456789D";
#pragma PERSISTENT(timer1)
int timer1 = 0;
#pragma PERSISTENT(timer2)
int timer2 = 0;
#pragma PERSISTENT(pauseFlag)
short pauseFlag = 0;

void main(void)
{
    uint16_t gie;

    P1DIR |= 0x03;
    P5DIR |= 0x60;

    boardInit();
    clockInit();

    // Set up the LCD
    Sharp128x128_initDisplay();

    /*
     * Disable the GPIO power-on default high-impedance mode to activate
     * previously configured port settings
     */

    PMM_unlockLPM5();

    __enable_interrupt();
    Graphics_initContext(&g_sContext, &g_sharp128x128LCD);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
    // Store current GIE state
    gie = __get_SR_register() & GIE;
    __disable_interrupt();
    // Flush Buffer to LCD
    Graphics_flushBuffer(&g_sContext);
    // Restore original GIE state
    __bis_SR_register(gie);

    P1OUT &= ~0x02;

    if(pauseFlag == 0)
    {
        P1OUT &= ~0x01;
    }
    else
    {
        P1OUT |= 0x01;
    }

    while(1)
    {
        Graphics_drawStringCentered(&g_sContext,
                                    "Timer:",
                                    AUTO_STRING_LENGTH,
                                    48,
                                    50,
                                    TRANSPARENT_TEXT);
        Graphics_drawStringCentered(&g_sContext,
                                    numbers+timer2,
                                    1,
                                    75,
                                    50,
                                    TRANSPARENT_TEXT);
        Graphics_drawStringCentered(&g_sContext,
                                    numbers+timer1,
                                    1,
                                    82,
                                    50,
                                    TRANSPARENT_TEXT);

        // Store current GIE state
        gie = __get_SR_register() & GIE;
        __disable_interrupt();
        // Flush Buffer to LCD
        Graphics_flushBuffer(&g_sContext);
        // Restore original GIE state
        __bis_SR_register(gie);

        if(pauseFlag == 0)
        {
            delay();

            timer1++;
            if(timer1 == 10)
            {
                timer1 -= 10;
                timer2++;
                if(timer2 == 10)
                {
                    timer2 -= 10;
                }
            }

            Graphics_clearDisplay(&g_sContext);
        }

        if((P5IN & 0x40) == 0x00)
        {
            P1OUT ^= 0x01;
            pauseFlag ^= 1;
            delay();
        }
        if((P5IN & 0x20) == 0x00)
        {
            P1OUT |= 0x02;
            timer1 = 0;
            timer2 = 0;
            Graphics_clearDisplay(&g_sContext);
        }
        else
        {
            P1OUT &= ~0x02;
        }
    }
}

void delay(void)
{
    volatile long int i = 30000;
    while(i > 0)
    {
        i--;
    }
}

void clockInit(void)
{
    CS_setDCOFreq(CS_DCORSEL_1,CS_DCOFSEL_3);

    CS_initClockSignal(
        CS_ACLK,
        CS_VLOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );

    CS_initClockSignal(
        CS_SMCLK,
        CS_DCOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );

    CS_initClockSignal(
        CS_MCLK,
        CS_DCOCLK_SELECT,
        CS_CLOCK_DIVIDER_1
        );
}

void boardInit()
{
    GPIO_setAsOutputPin(GPIO_PORT_PA,
                        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
                        GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
                        GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
                        GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15);

    GPIO_setAsOutputPin(GPIO_PORT_PB,
                        GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
                        GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
                        GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
                        GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15);

    GPIO_setOutputLowOnPin(GPIO_PORT_PA,
                           GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
                           GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
                           GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
                           GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15);

    GPIO_setOutputLowOnPin(GPIO_PORT_PB,
                           GPIO_PIN0 + GPIO_PIN1 + GPIO_PIN2 + GPIO_PIN3 +
                           GPIO_PIN4 + GPIO_PIN5 + GPIO_PIN6 + GPIO_PIN7 +
                           GPIO_PIN8 + GPIO_PIN9 + GPIO_PIN10 + GPIO_PIN11 +
                           GPIO_PIN12 + GPIO_PIN13 + GPIO_PIN14 + GPIO_PIN15);
}
