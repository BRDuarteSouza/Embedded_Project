// SM.c
// Máquina estados de teste para o projeto de embarcados
// Author: Bruno Duarte <brduart95@gmail.com>


#include<stdio.h>
#include<time.h>

typedef enum {
  Gira_R =0,
  Gira_L
}rodar;


void spinning(int timer){
  time_t now;
  time(&now);
  rodar rd = Gira_R;
  long ls_report,init_time;

  ls_report = now;
  init_time = now;
  while(1){
    time(&now);
    switch(rd){
      case(Gira_R):
        // printf("Rodando Direita");
        if(now - ls_report >= 1){
          printf("Girando pra esqueda\n");
          ls_report = now;
          rd = Gira_L;
          }
        if(now - init_time > timer){
          printf("saindo\n");
          return;
        }
        break;
      case(Gira_L):
        if(now - ls_report >= 1){
          printf("Girando pra Direita\n");
          ls_report = now;
          rd = Gira_R;
          }
        if(now - init_time > timer){
          printf("saindo\n");
          return;
          }
        break;
        }
      long casa = now - init_time;
      printf("Entrei: %d - %ld\n", rd, casa);
     }
  }

int main(void){
  spinning(15);
  return 0;
}
