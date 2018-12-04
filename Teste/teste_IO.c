// teste_IO.c
// testins Chibios libraries
// Author: Bruno Duarte <brduarte95@gmail.com>

#include "hal.h"
#include "ch.h"


int main(void){
  halInit();
  chSysInit();

  palSetPadMode(IOPORT4,4,PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT4,7,PAL_MODE_INPUT);

  while (true) {

    if (palReadPad(IOPORT4, 7) == 1){
      palSetPad(IOPORT4, 4);
    }else{
      palClearPad(IOPORT4, 4);
    }
    palSetPad(IOPORT2, PORTB_LED1);
    chThdSleepMilliseconds(500);
    palClearPad(IOPORT2, PORTB_LED1);
    chThdSleepMilliseconds(500);
  }
  return 0;
}
