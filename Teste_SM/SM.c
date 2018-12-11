// SM.c
// Máquina estados de teste para o projeto de embarcados
// Author: Bruno Duarte <brduart95@gmail.com>

#include <stdio.h>
#include <time.h>

#define TEMPO_GIRO 1
#define TEMPO_ESPERA_LAVAGEM 3


/* estados da máquinas do projeto */
typedef enum {
  GIRA_R = 0,
  GIRA_L
} rodar_e;

typedef enum{
  PARADA = 0,
  MOLHO,
  LAVAGEM,
  ENXAGUE,
  CENTRIFUGA,
  EMERGENCIA
} maquina_e;

typedef enum{
  ENCHER_ML = 6,
  RODAR_ML
} estados_molho_e;

typedef enum{
  RODAR_LVG = 8,
  ESPERA_LVG,
  ESVAZIA_LVG
} estados_lavagem_e;

typedef enum{
  ENCHER_EXG = 11,
  RODAR_EXG,
  ESVAZIA_EXG
} estados_enxague_e;

typedef enum{
  GIRA_R_CENTRI = 14,
} estados_centrifuga_e;

maquina_e lavar = MOLHO;


/* Contexto das máquinas internas */
typedef struct {
  estados_molho_e ml;
} fsm_molho_s;

typedef struct {
  estados_lavagem_e lvg;
  time_t instante;
  long ul_espera;
  int turns;
} fsm_lavagem_s;

typedef struct {
  estados_enxague_e exg;
} fsm_enxague_s;

typedef struct{
  estados_centrifuga_e cen;
} fsm_centrifuga_s;

/* Função de inicialização das máquinas internas */
static void fsm_molho_init(fsm_molho_s *st)
{
  st->ml = ENCHER_ML;
}

static void fsm_lavagem_init(fsm_lavagem_s *st)
{
  st->lvg = RODAR_LVG;
  st->turns = 0;
  time(&st->instante);
  st->ul_espera = st->instante;
}

static void fsm_enxague_init(fsm_enxague_s *st)
{
  st->exg = ENCHER_EXG;
}

static void fsm_centrifuga_init(fsm_centrifuga_s *st)
{
  st->cen = GIRA_R_CENTRI;
}

/* Variaveis das estruturas das máqunas internas */
fsm_molho_s fsm_ml;

fsm_lavagem_s fsm_lvg;

fsm_enxague_s fsm_exg;

fsm_centrifuga_s fsm_cen;

/* Função de Giro */

void spinning(int timer) {
  time_t agora;
  time(&agora);
  rodar_e rd = GIRA_R;
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
      // printf("Entrei: %d - %ld\n", rd, casa);
      // long casa = agora - init_time;
     }
  }


/* Máquina Principal */

void lavar_maquina(){
  // int t_cheio;
  switch(lavar){


    case(MOLHO):
    printf("Inicio Molho\n");

      switch(fsm_ml.ml){
        case(ENCHER_ML):
          printf("DEBUG: Super MOLHO - estado ENCHER_ML\n");
          printf("Abrir água\n");
          if(1){
            printf("Sensor Cheio\n");
            fsm_ml.ml = RODAR_ML;
            printf("Início Giros Molho\n");
          }
          break;

        case(RODAR_ML):
          printf("DEBUG: Super MOLHO - estado RODAR_ML\n");
          spinning(2);
          lavar = LAVAGEM;
          // lvg = RODAR_LVG;
          fsm_lavagem_init(&fsm_lvg);
          printf("Fim Molho\n");
          break;
      }
      break;


    case(LAVAGEM):
      printf("Inicio Lavagem\n");

      switch(fsm_lvg.lvg){
        case(RODAR_LVG):
          printf("DEBUG Super LAVAGEM - estado RODAR_LVG\n");
          printf("Inicio giros Lavagem\n");
          spinning(2);
          fsm_lvg.lvg = ESPERA_LVG;
          break;

        case(ESPERA_LVG):
        // printf("DEBUG %d: Super LAVAGEM - estado ESPERA_LVG\n");
        time(&fsm_lvg.instante);
          printf("Instante: %ld, ul_espera: %ld\n", fsm_lvg.instante, fsm_lvg.ul_espera);
          if(fsm_lvg.instante - fsm_lvg.ul_espera > TEMPO_ESPERA_LAVAGEM){
            fsm_lvg.ul_espera = time(&fsm_lvg.instante);
            printf("DEBUG: Super LAVAGEM - estado ESPERA\n");
            printf("Entrar em tempo estado: %d\n ", fsm_lvg.lvg);
            if(fsm_lvg.turns >= 3){
              fsm_lvg.turns = 0;
              fsm_lvg.lvg = ESVAZIA_LVG;
              printf("Fim giros Lavagem\n");

            } else {
              fsm_lvg.turns ++;
              fsm_lvg.lvg = RODAR_LVG;
            } // fim if voltas
          } // fim if tempo
        break;

        case(ESVAZIA_LVG):
        printf("DEBUG: Super LAVAGEM - estado ESVAZIA_LVG\n");
        if(1){
          printf("Sensor Vazio: ON\n");
          printf("Fim LAVAGEM\n");
          lavar = ENXAGUE;
          fsm_enxague_init(&fsm_exg);
        } // fim "if" sensor vazio
        break;
      } // fim switch LAVAGEM


    case(ENXAGUE):
      printf("Inicio Enxague\n ");

      switch(fsm_exg.exg){
        case(ENCHER_EXG):
          printf("DEBUG: Super ENXAGUE - estado ENCHER_EXG\n");
          if(1){
            printf("Sensor Cheio: ON\n");
            fsm_exg.exg = RODAR_EXG;
          } // fim "if" Sensor cheio
          break;

        case(RODAR_EXG):
          printf("DEBUG: Super ENXAGUE - estado RODAR_EXG\n");
          spinning(15);
          fsm_exg.exg = ESVAZIA_EXG;
        break;

        case(ESVAZIA_EXG):
          printf("DEBUG: Super ENXAGUE - estados ESVAZIA_EXG\n");
          if(1){
             printf("Sensor Vazio: ON\n");
             lavar = CENTRIFUGA;
             fsm_centrifuga_init(&fsm_cen);
             printf("Fim Enxágue");
          }// fim "if" sensor vazio
        break;
      } // fim switch ENXAGUE


      case(CENTRIFUGA):
        switch(fsm_cen.cen){
          case(GIRA_R_CENTRI):
          printf("DEBUG: Super CENTRIFUGA - estado GIRA_R_CENTRI\n");


        } // fim switch CENTRIFUGA
    break;
  } // fim switch máquina principal
} // fim função da máquina principal.


int main(void){
  fsm_molho_init(&fsm_ml);
  while(1){
    lavar_maquina();
  }
  return 0;
}
