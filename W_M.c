// W_M.c
// Implement a state machine to execute a washing machine taks, with ChibiOS
// Author: Bruno Duarte <brduart95@gmail.com>

#include <stdint.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>


/* Define Outputs*/
#define PARADA_ID 7
#define MOLHO_ID 0
#define LAVAGEM_ID 1
#define ENXAGUE_ID 2
#define CENTRI_ID 3
#define ENCHE_ID 4
#define SECA_ID 5
#define MOTOR_DIR 0
#define MOTOR_ESQ 1


/*Define Inputs*/
#define BOT_SELECT 6
#define BOT_INIT 5
#define SEN_CHEIO 4
#define SEN_VAZIO 3
#define EMER 2

/* System variables*/
#define GIRO_TEMPO 1

/* All State machines of the project */

/* Main Machine*/
typedef enum{
  PARADA = 0,
  MOLHO,
  LAVAGEM,
  ENXAGUE,
  CENTRIFUGA,
  EMERGENCIA
} maquina_e;

maquina_e lavar = PARADA; // Create an object of Main Machine;


/* Spinning States*/
typedef enum {
  GIRA_R = 0,
  GIRA_L
} rodar_e;


/* Moisten states*/
typedef enum{
  ENCHER_ML = 6,
  RODAR_ML
} estados_molho_e;


/* Washing states*/
typedef enum{
  RODAR_LVG = 8,
  ESPERA_LVG,
  ESVAZIA_LVG
} estados_lavagem_e;


/* Rinse states*/
typedef enum{
  ENCHER_EXG = 11,
  RODAR_EXG,
  ESVAZIA_EXG
} estados_enxague_e;


/* Centrifugal states*/
typedef enum{
  GIRA_R_CENTRI = 14,
} estados_centrifuga_e;


/* Variables of intern Machines */
typedef struct {
  int select_init;
} fsm_parada_s;

typedef struct {
  estados_molho_e ml;
  int t_giro;
} fsm_molho_s;

typedef struct {
  estados_lavagem_e lvg;
  systime_t instante;
  systime_t ul_espera;
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
  systime_t tempo;
  systime_t ul_centri;
  int t_centri;
} fsm_centrifuga_s;


/* Start Condition functions*/
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
  st->instante = chVTGetSystemTime();
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
  st->tempo = chVTGetSystemTime();
  st->ul_centri = st->tempo;
  st->t_centri = 20;
}


/* Variables of the states */
fsm_parada_s fsm_prd;

fsm_molho_s fsm_ml;

fsm_lavagem_s fsm_lvg;

fsm_enxague_s fsm_exg;

fsm_centrifuga_s fsm_cen;


/* Functions*/
void starting(){
  // Output
  palSetPadMode(IOPORT2, SECA_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT2, ENCHE_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT2, CENTRI_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT2, ENXAGUE_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT2, LAVAGEM_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT2, MOLHO_ID, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT3, MOTOR_DIR, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT3, MOTOR_ESQ, PAL_MODE_OUTPUT_PUSHPULL);
  palSetPadMode(IOPORT4, PARADA_ID, PAL_MODE_OUTPUT_PUSHPULL);


  // Input
  palSetPadMode(IOPORT4, EMER, PAL_MODE_INPUT);
  palSetPadMode(IOPORT4, SEN_VAZIO, PAL_MODE_INPUT);
  palSetPadMode(IOPORT4, SEN_CHEIO, PAL_MODE_INPUT);
  palSetPadMode(IOPORT4, BOT_INIT, PAL_MODE_INPUT);
  palSetPadMode(IOPORT4, BOT_SELECT, PAL_MODE_INPUT);

  // Start
  palClearPad(IOPORT2, SECA_ID);
  palClearPad(IOPORT2, ENCHE_ID);
  palClearPad(IOPORT2, CENTRI_ID);
  palClearPad(IOPORT2, ENXAGUE_ID);
  palClearPad(IOPORT2, LAVAGEM_ID);
  palClearPad(IOPORT2, MOLHO_ID);
  palClearPad(IOPORT3, MOTOR_DIR);
  palClearPad(IOPORT3, MOTOR_ESQ);
  palClearPad(IOPORT4, PARADA_ID);

  chprintf((BaseSequentialStream *)&SD1, "pinos iniciados \n\r");

}


/* Spin Function */

void spinning(int timer) {
  systime_t agora = chVTGetSystemTime();
  rodar_e rd = GIRA_R;
  systime_t ls_report,init_time;

  ls_report = agora;
  init_time = agora;
  while(1){
    agora = chVTGetSystemTime();
    switch(rd){
      case(GIRA_R):
        if(lavar == EMERGENCIA){
          return;
        } // verifcar emergência
        palSetPad(IOPORT3, MOTOR_DIR);
        if(agora - ls_report >= TIME_S2I(GIRO_TEMPO)){
          // printf("Girando pra Esqueda\n");
          palClearPad(IOPORT3, MOTOR_DIR);
          ls_report = agora;
          rd = GIRA_L;
          }
        if(agora - init_time > TIME_S2I(timer)){
          // printf("Fim do Giro\n");
          return;
        }
        break;
      case(GIRA_L):
        if(lavar == EMERGENCIA){
          return;
        } // verifcar emergência
        palSetPad(IOPORT3, MOTOR_ESQ);
        if(agora - ls_report >= TIME_S2I(GIRO_TEMPO)){
          // printf("Girando pra Direita\n");
          palClearPad(IOPORT3, MOTOR_ESQ);
          ls_report = agora;
          rd = GIRA_R;
          }
        if(agora - init_time > TIME_S2I(timer)){
          // printf("saindo\n");
          return;
          }
        break;
        }
     }
  }

/* Main Machine */

void lavar_maquina(){
  // int t_cheio;
  // chprintf((BaseSequentialStream *)&SD1, "ENtrei na maquina");
  switch(lavar){

    // case(PARADA):
    //   if(lavar == EMERGENCIA){
    //     break;
    //   } // verifcar emergência
    //   printf("Inicio da Parada\n");
    //   printf("Escolha o estado inicial:\n");
    //   scanf("%d", &fsm_prd.select_init);
    //
    //   if(fsm_prd.select_init == 1){
    //     lavar = MOLHO;
    //     printf("Inicio MOLHO\n");
    //     fsm_molho_init(&fsm_ml);
    //   }
    //   if(fsm_prd.select_init == 2){
    //     lavar = LAVAGEM;
    //     printf("Inicio LAVAGEM\n");
    //     fsm_lavagem_init(&fsm_lvg);
    //   }
    //   if(fsm_prd.select_init == 3){
    //     lavar = ENXAGUE;
    //     printf("Inicio ENXAGUE\n");
    //     fsm_enxague_init(&fsm_exg);
    //   }
    //   if(fsm_prd.select_init == 4){
    //     lavar = CENTRIFUGA;
    //     fsm_centrifuga_init(&fsm_cen);
    //     printf("Inicio CENTRIFUGA\n");
    //   }
    //
    // break;


    case(MOLHO):
    // printf("Inicio Molho\n");
    chprintf((BaseSequentialStream *)&SD1, "Inicio MOLHO \n\r");

      switch(fsm_ml.ml){
        case(ENCHER_ML):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          // printf("DEBUG: Super MOLHO - estado ENCHER_ML\n");
          // printf("Abrir água\n");
          palSetPad(IOPORT2, ENCHE_ID);
          chprintf((BaseSequentialStream *)&SD1, "Enchendo... \n\r");
          if(palReadPad(IOPORT4, SEN_CHEIO) == PAL_HIGH){
            // printf("Sensor Cheio\n");
            chprintf((BaseSequentialStream *)&SD1, "Sensor cheio\n\r");
            palClearPad(IOPORT2, ENCHE_ID);
            fsm_ml.ml = RODAR_ML;
            // printf("Início Giros Molho\n");
          }
          break;

        case(RODAR_ML):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          // printf("DEBUG: Super MOLHO - estado RODAR_ML\n");
          spinning(fsm_ml.t_giro);
          lavar = LAVAGEM;
          fsm_lavagem_init(&fsm_lvg);
          // printf("Fim Molho\n");
          // printf("Inicio LAVAGEM\n");
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
          // printf("DEBUG Super LAVAGEM - estado RODAR_LVG\n");
          // printf("Inicio giros Lavagem\n");
          spinning(fsm_lvg.t_giros);
          fsm_lvg.ul_espera = chVTGetSystemTime();
          fsm_lvg.lvg = ESPERA_LVG;
          // printf("DEBUG: Super LAVAGEM - estado ESPERA\n");

          break;

        case(ESPERA_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // verifcar emergência
          fsm_lvg.instante = chVTGetSystemTime();
          if(fsm_lvg.instante - fsm_lvg.ul_espera > TIME_S2I(fsm_lvg.t_espera)){
            fsm_lvg.ul_espera = chVTGetSystemTime();
            if(fsm_lvg.turns >= 3){
              fsm_lvg.turns = 0;
              fsm_lvg.lvg = ESVAZIA_LVG;
              // printf("Fim giros Lavagem\n");

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
          palSetPad(IOPORT2, SECA_ID);
          // printf("DEBUG: Super LAVAGEM - estado ESVAZIA_LVG\n");
          if(palReadPad(IOPORT4, SEN_VAZIO) == PAL_HIGH){
            // printf("Sensor Vazio: ON\n");
            palClearPad(IOPORT2, SECA_ID);
            // printf("Fim LAVAGEM\n");
            // printf("Inicio ENXAGUE\n");
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
          // printf("DEBUG: Super ENXAGUE - estado ENCHER_EXG\n");
          palSetPad(IOPORT2, ENCHE_ID);
          if(palReadPad(IOPORT4, SEN_CHEIO) == PAL_HIGH){
            // printf("Sensor Cheio: ON\n");
            palClearPad(IOPORT2, ENCHE_ID);
            fsm_exg.exg = RODAR_EXG;

          } // fim "if" Sensor cheio
          break;

        case(RODAR_EXG):
          if(lavar == EMERGENCIA){
            break;
          } // verificar emergência
          // printf("DEBUG: Super ENXAGUE - estado RODAR_EXG\n");
          spinning(fsm_exg.t_giro);
          fsm_exg.exg = ESVAZIA_EXG;

        break;

        case(ESVAZIA_EXG):
          if(lavar == EMERGENCIA){
            break;
          } //  verificar a emergência

          // printf("DEBUG: Super ENXAGUE - estados ESVAZIA_EXG\n");
          palSetPad(IOPORT2, SECA_ID);
          if(palReadPad(IOPORT4, SEN_VAZIO) == PAL_HIGH){
             // printf("Sensor Vazio: ON\n");
             palClearPad(IOPORT2, SECA_ID);
             lavar = CENTRIFUGA;
             fsm_centrifuga_init(&fsm_cen);
             // printf("Fim Enxágue\n");
             // printf("Inicio CENTRIFUGA\n");
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
            palSetPad(IOPORT3, MOTOR_DIR);
            // printf("DEBUG: Super CENTRIFUGA - estado GIRA_R_CENTRI\n");
            fsm_cen.tempo = chVTGetSystemTime();
            // printf("Super:%d - estado:%d\n ",lavar, fsm_cen.cen);
            // printf("CENTRIFUGA: %ld - %ld\n", fsm_cen.instante, fsm_cen.ul_espera);
            if(fsm_cen.tempo - fsm_cen.ul_centri > TIME_S2I(fsm_cen.t_centri)){
              palClearPad(IOPORT3, MOTOR_DIR);
              lavar = PARADA;
              // printf("FIM DA Máquina\n");
            }
          break; // break de GIRA_R_CENTRI
        } // fim switch CENTRIFUGA
    break; // break de centrífuga.


    case(EMERGENCIA):
      // printf("PORTA ABERTA. ERRO !!\n");
      while(1);

    break; // break de emergência
  } // fim switch máquina principal
} // fim função da máquina principal.


int main(void) {
  halInit();
  chSysInit();
  sdStart(&SD1, 0);
  chprintf((BaseSequentialStream *)&SD1, "INICIO MÁQUINA\n\r");
  starting();
  lavar = MOLHO;
  fsm_molho_init(&fsm_ml);
  chprintf((BaseSequentialStream *)&SD1, "%d \n\r", lavar);

  while(1){
    lavar_maquina();
  }
  //casa
  return 0;
}
