// W_M.c
// Implement a state machine to execute a washing machine taks, with ChibiOS
// Author: Bruno Duarte <brduart95@gmail.com>

#include <ch.h>
#include <hal.h>

#define RODAR_ESQUERDA 7
#define RODAR_DIREITA 6
#define TEMPO_GIRO 1


typedef enum {
  Gira_R = 0,
  Gira_L
} Giro;

void spining(int time){
  Giro rodar = Gira_L;
  long ls_pass;
  if

  switch(rodar){
    case(Gira_L){
      palSetPad(IOPORT4,RODAR_ESQUERDA );
      if
    }
  }

}


int main(void){

  return 0;
}
