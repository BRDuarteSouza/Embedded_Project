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

maquina_e lavar = PARADA; // Criar variável da máquina principal;

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


/* Contexto das máquinas internas */
typedef struct {
  int select_init;
} fsm_parada_s;

typedef struct {
  estados_molho_e ml;
  int t_giro;
} fsm_molho_s;

typedef struct {
  estados_lavagem_e lvg;
  time_t instante;
  long ul_espera;
  int turns;
  int t_espera;
  int t_giros;
} fsm_lavagem_s;

typedef struct {
  estados_enxague_e exg;
  int t_giro;
} fsm_enxague_s;

typedef struct {
  estados_centrifuga_e cen;
  time_t tempo;
  long ul_centri;
  int t_centri;
} fsm_centrifuga_s;

/* Função de inicialização das máquinas internas */
static void fsm_molho_init(fsm_molho_s *st)
{
  st->ml = ENCHER_ML;
  st->t_giro = 10;
}

static void fsm_lavagem_init(fsm_lavagem_s *st)
{
  st->lvg = RODAR_LVG;
  st->turns = 0;
  st->t_espera = 2;
  st->t_giros = 12;
  time(&st->instante);
  st->ul_espera = st->instante;
}

static void fsm_enxague_init(fsm_enxague_s *st)
{
  st->exg = ENCHER_EXG;
  st->t_giro = 15;
}

static void fsm_centrifuga_init(fsm_centrifuga_s *st)
{
  st->cen = GIRA_R_CENTRI;
  time(&st->tempo);
  st->ul_centri = st->tempo;
  st->t_centri = 20;
}

/* Variaveis das estruturas das máqunas internas */
fsm_parada_s fsm_prd;

fsm_molho_s fsm_ml;

fsm_lavagem_s fsm_lvg;

fsm_enxague_s fsm_exg;

fsm_centrifuga_s fsm_cen;


/* Função de configuração */

void setup(){


}



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
        if(lavar == EMERGENCIA){
          return;
        } // verifcar emergência
        // printf("Rodando Direita");
        if(agora - ls_report >= 1){
          printf("Girando pra Esqueda\n");
          ls_report = agora;
          rd = GIRA_L;
          }
        if(agora - init_time > timer){
          printf("Fim do Giro\n");
          return;
        }
        break;
      case(GIRA_L):
        if(lavar == EMERGENCIA){
          return;
        } // verifcar emergência
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

    case(PARADA):
      if(lavar == EMERGENCIA){
        break;
      } // verifcar emergência
      printf("Inicio da Parada\n");
      printf("Escolha o estado inicial:\n");
      scanf("%d", &fsm_prd.select_init);

      if(fsm_prd.select_init == 1){
        lavar = MOLHO;
        printf("Inicio MOLHO\n");
        fsm_molho_init(&fsm_ml);
      }
      if(fsm_prd.select_init == 2){
        lavar = LAVAGEM;
        printf("Inicio LAVAGEM\n");
        fsm_lavagem_init(&fsm_lvg);
      }
      if(fsm_prd.select_init == 3){
        lavar = ENXAGUE;
        printf("Inicio ENXAGUE\n");
        fsm_enxague_init(&fsm_exg);
      }
      if(fsm_prd.select_init == 4){
        lavar = CENTRIFUGA;
        fsm_centrifuga_init(&fsm_cen);
        printf("Inicio CENTRIFUGA\n");
      }

    break;


    case(MOLHO):
    // printf("Inicio Molho\n");

      switch(fsm_ml.ml){
        case(ENCHER_ML):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          printf("DEBUG: Super MOLHO - estado ENCHER_ML\n");
          printf("Abrir água\n");
          if(1){
            printf("Sensor Cheio\n");
            fsm_ml.ml = RODAR_ML;
            printf("Início Giros Molho\n");
          }
          break;

        case(RODAR_ML):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          printf("DEBUG: Super MOLHO - estado RODAR_ML\n");
          spinning(fsm_ml.t_giro);
          lavar = LAVAGEM;
          fsm_lavagem_init(&fsm_lvg);
          printf("Fim Molho\n");
          printf("Inicio LAVAGEM\n");
          break;
      }
      break;


    case(LAVAGEM):
      // printf("Inicio Lavagem\n");

      switch(fsm_lvg.lvg){
        case(RODAR_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          printf("DEBUG Super LAVAGEM - estado RODAR_LVG\n");
          printf("Inicio giros Lavagem\n");
          spinning(fsm_lvg.t_giros);
          fsm_lvg.ul_espera = time(&fsm_lvg.instante);
          fsm_lvg.lvg = ESPERA_LVG;
          printf("DEBUG: Super LAVAGEM - estado ESPERA\n");

          break;

        case(ESPERA_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          time(&fsm_lvg.instante);
          if(fsm_lvg.instante - fsm_lvg.ul_espera > fsm_lvg.t_espera){
            fsm_lvg.ul_espera = time(&fsm_lvg.instante);
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
          if(lavar == EMERGENCIA){
            break;
          } // verificar emergência
          printf("DEBUG: Super LAVAGEM - estado ESVAZIA_LVG\n");
          if(1){
            printf("Sensor Vazio: ON\n");
            printf("Fim LAVAGEM\n");
            printf("Inicio ENXAGUE\n");
              lavar = ENXAGUE;
              fsm_enxague_init(&fsm_exg);
            } // fim "if" sensor vazio
            break;
          } // fim switch LAVAGEM
          break;


    case(ENXAGUE):
      // printf("Inicio Enxague\n ");

      switch(fsm_exg.exg){
        case(ENCHER_EXG):
          if(lavar == EMERGENCIA){
            break;
          } // verificar emergência
          printf("DEBUG: Super ENXAGUE - estado ENCHER_EXG\n");
          if(1){
            printf("Sensor Cheio: ON\n");
            fsm_exg.exg = RODAR_EXG;

          } // fim "if" Sensor cheio
          break;

        case(RODAR_EXG):
          if(lavar == EMERGENCIA){
            break;
          } // verificar emergência
          printf("DEBUG: Super ENXAGUE - estado RODAR_EXG\n");
          spinning(fsm_exg.t_giro);
          fsm_exg.exg = ESVAZIA_EXG;

        break;

        case(ESVAZIA_EXG):
          if(lavar == EMERGENCIA){
            break;
          } //  verificar a emergência

          printf("DEBUG: Super ENXAGUE - estados ESVAZIA_EXG\n");
          if(1){
             printf("Sensor Vazio: ON\n");
             lavar = CENTRIFUGA;
             fsm_centrifuga_init(&fsm_cen);
             printf("Fim Enxágue\n");
             printf("Inicio CENTRIFUGA\n");
          }// fim "if" sensor vazio
        break;
      } // fim switch ENXAGUE
      break;


      case(CENTRIFUGA):
      // printf("Inicio Centrifuga\n");

        switch(fsm_cen.cen){
          case(GIRA_R_CENTRI):
            if(lavar == EMERGENCIA){
              break;
            }
            // printf("DEBUG: Super CENTRIFUGA - estado GIRA_R_CENTRI\n");
            time(&fsm_cen.tempo);
            // printf("Super:%d - estado:%d\n ",lavar, fsm_cen.cen);
            // printf("CENTRIFUGA: %ld - %ld\n", fsm_cen.instante, fsm_cen.ul_espera);
            if(fsm_cen.tempo - fsm_cen.ul_centri > fsm_cen.t_centri){
              lavar = PARADA;
              printf("FIM DA Máquina\n");
            }
          break; // break de GIRA_R_CENTRI
        } // fim switch CENTRIFUGA
    break; // break de centrífuga.


    case(EMERGENCIA):
      printf("PORTA ABERTA. ERRO !!\n");
      while(1);

    break; // break de emergência
  } // fim switch máquina principal
} // fim função da máquina principal.


int main(void){
  while(1){
    lavar_maquina();
  }
  //casa 
  return 0;
}
