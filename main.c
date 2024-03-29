/*!****************************************************************************
 * @brief
 * main.c
 * 
 * @brief
 * Main program entry point
 * 
 * @date  20.09.2023
 ******************************************************************************/

/*- Header files -------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "hw_init.h"
#include "hw_gpio.h"


/*!****************************************************************************
 * @brief
 * Main program entry point
 * 
 * @date  20.09.2023
 ******************************************************************************/
int main(void)
{
  /* Initialise hardware                                  */
  vHW_Init();

  /* Main program loop                                    */
  while (1)
  {
    HAL_Delay(500u);
    vHW_GpioToggleLed();
  }
}