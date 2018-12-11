// SM.c
// Máquina estados de teste para o projeto de embarcados
// Author: Bruno Duarte <brduart95@gmail.com>


#include<stdio.h>
#include<time.h>


#define TEMPO_GIRO 1
#define TEMPO_ESPERA 2

typedef enum {
  GIRA_R =0,
  GIRA_L
}rodar;

void spinning(int timer){
  time_t agora;
  time(&agora);
  rodar rd = GIRA_R;
  long ls_report,init_time;

  ls_report = agora;
  init_time = agora;
  while(1){
    time(&agora);
    switch(rd){
      case(GIRA_R):
        // printf("Rodando Direita");
        if(agora - ls_report >= 1){
          printf("Girando pra esqueda\n");
          ls_report = agora;
          rd = GIRA_L;
          }
        if(agora - init_time > timer){
          printf("Fim do Giro\n");
          return;
        }
        break;
      case(GIRA_L):
        if(agora - ls_report >= 1){
          printf("Girando pra Direita\n");
          ls_report = agora;
          rd = GIRA_R;
          }
        if(agora - init_time > timer){
          printf("saindo\n");
          return;
          }
        break;
        }
      // long casa = agora - init_time;
      // printf("Entrei: %d - %ld\n", rd, casa);
     }
  }

typedef enum{
  PARADA=0,
  MOLHO,
  LAVAGEM,
  ENXAGUE,
  CENTRIFUGA,
  EMERGENCIA
}maquina;

maquina lavar = MOLHO;

typedef enum{
  ENCHER_ML = 6,
  RODAR_ML
}estados_molho;

typedef enum{
  RODAR_LVG=8,
  ESPERA_LVG,
  ESVAZIA_LVG
}estados_lavagem;

estados_molho ml = ENCHER_ML;
estados_lavagem lvg = RODAR_LVG;

void lavar_maquina(){
  // int t_cheio;
  switch(lavar){


    case(MOLHO):
    printf("Inicio Molho\n");

      switch(ml){
        case(ENCHER_ML):
          printf("DEBUG: Super MOLHO - estado ENCHER_ML\n");
          printf("Abrir água\n");
          if(1){
            ml = RODAR_ML;
            printf("Início Giros Molho\n");
          }
          break;

        case(RODAR_ML):
          printf("DEBUG: Super MOLHO - estado RODAR_ML\n");
          spinning(10);
          lavar = LAVAGEM;
          lvg = RODAR_LVG;
          printf("Fim Molho\n");
          break;
      }
      break;


    case(LAVAGEM):
      printf("Inicio Lavagem\n");
    //a vida é bela
      time_t instante;
      long ul_espera;
      time(&instante);
      int turns=0;
      ul_espera =instante;

      switch(lvg){
        case(RODAR_LVG):
          printf("DEBUG Super LAVAGEM - estado RODAR_LVG\n");
          printf("Inicio giros Lavagem\n");
          spinning(12);
          lvg = ESPERA_LVG;
          break;

        case(ESPERA_LVG):
        time(&instante);
        // printf("DEBUG %d: Super LAVAGEM - estado ESPERA_LVG\n");
        printf("Instante: %ld, ul_espera: %ld\n", instante, ul_espera);
        if(instante - ul_espera > TEMPO_ESPERA){
          if(turns > 4){
            turns = 0;
            lvg = ESVAZIA_LVG;
            printf("Fim giros Lavagem\n");
          }else{
            lvg = RODAR_LVG;
          }
        }
        break;
      }
    break;
  }
}



int main(void){
  while(1){
    lavar_maquina();
  }
  return 0;
}
