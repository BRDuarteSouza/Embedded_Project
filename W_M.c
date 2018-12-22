// W_M.c
// Implement a state machine to execute a washing machine task, with ChibiOS.
// Author: Bruno Duarte <brduart95@gmail.com>


/* Libraries*/
#include <stdint.h>
#include <string.h>
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
#define GIRO_TEMPO 500

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
static void fsm_parada_init(fsm_parada_s *st){
  st->select_init = 0;
}

static void fsm_molho_init(fsm_molho_s *st)
{
  st->ml = ENCHER_ML;
  st->t_giro = 2000;
}

static void fsm_lavagem_init(fsm_lavagem_s *st)
{
  st->lvg = RODAR_LVG;
  st->turns = 0;
  st->t_espera = 1000;
  st->t_giros = 2400;
  st->instante = chVTGetSystemTime();
  st->ul_espera = st->instante;
}

static void fsm_enxague_init(fsm_enxague_s *st)
{
  st->exg = ENCHER_EXG;
  st->t_giro = 3000;
}

static void fsm_centrifuga_init(fsm_centrifuga_s *st)
{
  st->cen = GIRA_R_CENTRI;
  st->tempo = chVTGetSystemTime();
  st->ul_centri = st->tempo;
  st->t_centri = 4000;
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
  palSetPad(IOPORT3, MOTOR_DIR);
  palSetPad(IOPORT3, MOTOR_ESQ);
  palClearPad(IOPORT4, PARADA_ID);

  chprintf((BaseSequentialStream *)&SD1, "pinos iniciados \n\r");

}

/* Interupt Callback*/

static void emergency_cb(){
  char msg[] = "PORTA ABERTA !!!\n\r";
  sdWriteI(&SD1, msg, strlen(msg));
  lavar = EMERGENCIA;
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
        } // Verify Emergency state
        palClearPad(IOPORT3, MOTOR_DIR);
        if(agora - ls_report >= TIME_MS2I(GIRO_TEMPO)){ // Spin to one side
          chprintf((BaseSequentialStream *)&SD1, "Gira Esqueda\n\r");
          palSetPad(IOPORT3, MOTOR_DIR);
          ls_report = agora;
          rd = GIRA_L;
          }
        if(agora - init_time > TIME_MS2I(timer)){ // Verify if the spinning time
          palSetPad(IOPORT3, MOTOR_DIR);          // has ended.
          palSetPad(IOPORT3, MOTOR_ESQ);
          chprintf((BaseSequentialStream *)&SD1, "saindo \n\r");
          return;
        }
        break;
      case(GIRA_L):
      palClearPad(IOPORT3, MOTOR_ESQ);
        if(lavar == EMERGENCIA){
          return;
        } // Verify emergency
        if(agora - ls_report >= TIME_MS2I(GIRO_TEMPO)){
          chprintf((BaseSequentialStream *)&SD1, "Gira Direita\n\r");
          palSetPad(IOPORT3, MOTOR_ESQ);
          ls_report = agora;
          rd = GIRA_R;
          }
        if(agora - init_time > TIME_MS2I(timer)){
          palSetPad(IOPORT3, MOTOR_DIR);
          palSetPad(IOPORT3, MOTOR_ESQ);
          chprintf((BaseSequentialStream *)&SD1, "Fim Giro\n\r");
          return;
          }
        break;
        }
     }
  }

/* Main Machine */

void lavar_maquina(){


  switch(lavar){

    case(PARADA):
      if(lavar == EMERGENCIA){
        break;
      } // Verify emergency
      chprintf((BaseSequentialStream *)&SD1, "Selecione o estado inicial\n\r");
      chprintf((BaseSequentialStream *)&SD1, "Modo inicial: Molho\n\r");
      while(1){
        if(fsm_prd.select_init == 0){
          palSetPad(IOPORT4, PARADA_ID);
          palSetPad(IOPORT2, MOLHO_ID);
          palSetPad(IOPORT2, LAVAGEM_ID);
          palSetPad(IOPORT2, ENXAGUE_ID);
          palSetPad(IOPORT2, CENTRI_ID);
          lavar = MOLHO;
          fsm_molho_init(&fsm_ml);
        }

        if(fsm_prd.select_init == 0 && palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH){
          chThdSleepMilliseconds(20); // Debouncing time
          while(palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH); //Debouncing trap
          lavar = LAVAGEM;
          fsm_lavagem_init(&fsm_lvg);
          fsm_prd.select_init = 1;
          chprintf((BaseSequentialStream *)&SD1, "Modo inicial: Lavagem \n\r");
          palClearPad(IOPORT2, MOLHO_ID);
        }

        if(fsm_prd.select_init == 1 && palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH){
          chThdSleepMilliseconds(20); // Debouncing time
          while(palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH); //Debouncing trap
          lavar = ENXAGUE;
          fsm_enxague_init(&fsm_exg);
          fsm_prd.select_init = 2;
          chprintf((BaseSequentialStream *)&SD1, "Modo inicial: Enxague\n\r");
          palClearPad(IOPORT2, LAVAGEM_ID);
        }

        if(fsm_prd.select_init == 2 && palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH){
          chThdSleepMilliseconds(20); // Debouncing time
          while(palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH); //Debouncing trap
          lavar = CENTRIFUGA;
          fsm_centrifuga_init(&fsm_cen);
          fsm_prd.select_init = 3;
          chprintf((BaseSequentialStream *)&SD1, "Modo inicial: Centrifuga\n\r");
          palClearPad(IOPORT2, ENXAGUE_ID);
        }

        if(fsm_prd.select_init == 3 && palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH){
          chThdSleepMilliseconds(20); // Debouncing time
          while(palReadPad(IOPORT4, BOT_SELECT) == PAL_HIGH);//Debouncing trap
          chprintf((BaseSequentialStream *)&SD1, "Modo inicial: Molho\n\r");
          fsm_prd.select_init = 0;
        }

        if(palReadPad(IOPORT4,BOT_INIT) == PAL_HIGH){
          chThdSleepMilliseconds(20); // Debouncing time
          chprintf((BaseSequentialStream *)&SD1, "Inicio do processo\n\r");
          if(fsm_prd.select_init == 0){ // Information on Debug.
            chprintf((BaseSequentialStream *)&SD1, "Enchendo para o Molho \n\r");
          }

          if(fsm_prd.select_init == 2){
            chprintf((BaseSequentialStream *)&SD1, "Enchendo para o enxague\n\r");
          }

          return;
        }
      }// end of PARADA loop

    break;


    case(MOLHO):

    palSetPad(IOPORT2, MOLHO_ID); // turn indicative LED on.
    palClearPad(IOPORT2, LAVAGEM_ID);
    palClearPad(IOPORT2, ENXAGUE_ID);
    palClearPad(IOPORT2, CENTRI_ID);
    palClearPad(IOPORT4, PARADA_ID);

      switch(fsm_ml.ml){
        case(ENCHER_ML):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          palSetPad(IOPORT2, ENCHE_ID);
          if(palReadPad(IOPORT4, SEN_CHEIO) == PAL_HIGH){
            chprintf((BaseSequentialStream *)&SD1, "Sensor cheio\n\r");
            palClearPad(IOPORT2, ENCHE_ID);
            fsm_ml.ml = RODAR_ML;
          }
          break;

        case(RODAR_ML):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          chprintf((BaseSequentialStream *)&SD1, "Giros Molho\n\r");
          spinning(fsm_ml.t_giro);
          lavar = LAVAGEM;
          fsm_lavagem_init(&fsm_lvg);
          chprintf((BaseSequentialStream *)&SD1, "FIM Molho\n\r");
          palClearPad(IOPORT2, MOLHO_ID);
          chprintf((BaseSequentialStream *)&SD1, ("Inicio LAVAGEM\n\r"));
          break;
      }
      break;


    case(LAVAGEM):
      palClearPad(IOPORT2, MOLHO_ID); // indicative LED
      palSetPad(IOPORT2, LAVAGEM_ID);
      palClearPad(IOPORT2, ENXAGUE_ID);
      palClearPad(IOPORT2, CENTRI_ID);
      palClearPad(IOPORT4, PARADA_ID);

      switch(fsm_lvg.lvg){
        case(RODAR_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          chprintf((BaseSequentialStream *)&SD1, "Giros Lavagem\n\r");
          spinning(fsm_lvg.t_giros);
          fsm_lvg.ul_espera = chVTGetSystemTime();
          fsm_lvg.lvg = ESPERA_LVG;
          chprintf((BaseSequentialStream *)&SD1, "Espera Lavagem\n\r");

          break;

        case(ESPERA_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          fsm_lvg.instante = chVTGetSystemTime();
          if(fsm_lvg.instante - fsm_lvg.ul_espera > TIME_MS2I(fsm_lvg.t_espera)){ // repeat tunrs of Washing
            fsm_lvg.ul_espera = chVTGetSystemTime();                              // x4.
            if(fsm_lvg.turns >= 3){
              fsm_lvg.turns = 0;
              fsm_lvg.lvg = ESVAZIA_LVG;
              chprintf((BaseSequentialStream *)&SD1, "Esvazia Lavagem \n\r");

            } else {
              fsm_lvg.turns ++;
              fsm_lvg.lvg = RODAR_LVG;
            } // end if voltas
          } // end if tempo
          break;


        case(ESVAZIA_LVG):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          palSetPad(IOPORT2, SECA_ID);
          if(palReadPad(IOPORT4, SEN_VAZIO) == PAL_HIGH){
            palClearPad(IOPORT2, SECA_ID);
            chprintf((BaseSequentialStream *)&SD1, "Fim Lavagem\n\r");
            palClearPad(IOPORT2, LAVAGEM_ID);

            chprintf((BaseSequentialStream *)&SD1, "Inicio Enxague\n\r");
              lavar = ENXAGUE;

              fsm_enxague_init(&fsm_exg);
              chprintf((BaseSequentialStream *)&SD1, "Enche Enxague\n\r");
            } // end "if" sensor vazio
            break;
          } // end switch LAVAGEM
          break;


    case(ENXAGUE):
    palClearPad(IOPORT2, MOLHO_ID);
    palClearPad(IOPORT2, LAVAGEM_ID);
    palSetPad(IOPORT2, ENXAGUE_ID);
    palClearPad(IOPORT2, CENTRI_ID);
    palClearPad(IOPORT4, PARADA_ID);

      switch(fsm_exg.exg){
        case(ENCHER_EXG):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          palSetPad(IOPORT2, ENCHE_ID);
          if(palReadPad(IOPORT4, SEN_CHEIO) == PAL_HIGH){
            palClearPad(IOPORT2, ENCHE_ID);
            fsm_exg.exg = RODAR_EXG;

          } // end "if" Sensor cheio
          break;

        case(RODAR_EXG):
          if(lavar == EMERGENCIA){
            break;
          } // Verify emergency
          chprintf((BaseSequentialStream *)&SD1, "Rodar Enxague\n\r");
          spinning(fsm_exg.t_giro);
          fsm_exg.exg = ESVAZIA_EXG;
          chprintf((BaseSequentialStream *)&SD1, "Esvazia Enxague\n\r");

        break;

        case(ESVAZIA_EXG):
          if(lavar == EMERGENCIA){
            break;
          } //  Verify a emergency

          palSetPad(IOPORT2, SECA_ID);
          if(palReadPad(IOPORT4, SEN_VAZIO) == PAL_HIGH){
             palClearPad(IOPORT2, SECA_ID);
             lavar = CENTRIFUGA;
             fsm_centrifuga_init(&fsm_cen);
             chprintf((BaseSequentialStream *)&SD1, "Fim Enxague\n\r");
             palClearPad(IOPORT2, ENXAGUE_ID);
             chprintf((BaseSequentialStream *)&SD1, "Inicio Centrifuga\n\r");
          }// fim "if" sensor vazio
        break;
      } // fim switch ENXAGUE
      break;


      case(CENTRIFUGA):
      palClearPad(IOPORT2, MOLHO_ID);
      palClearPad(IOPORT2, LAVAGEM_ID);
      palClearPad(IOPORT2, ENXAGUE_ID);
      palSetPad(IOPORT2, CENTRI_ID);
      palClearPad(IOPORT4, PARADA_ID);

        switch(fsm_cen.cen){
          case(GIRA_R_CENTRI):
            if(lavar == EMERGENCIA){
              break;
            }
            palClearPad(IOPORT3, MOTOR_DIR);
            fsm_cen.tempo = chVTGetSystemTime();
            if(fsm_cen.tempo - fsm_cen.ul_centri > TIME_MS2I(fsm_cen.t_centri)){
              palSetPad(IOPORT3, MOTOR_DIR);
              lavar = PARADA;
              fsm_parada_init(&fsm_prd);
              palClearPad(IOPORT2, CENTRI_ID);
              chprintf((BaseSequentialStream *)&SD1, "FIM DO PROCESSO\n\r");
            }
          break; // break de GIRA_R_CENTRI
        } // fim switch CENTRIFUGA
    break; // break de centrífuga.


    case(EMERGENCIA):
      if(palReadPad(IOPORT4, EMER) == PAL_LOW){
        lavar = PARADA;
        fsm_parada_init(&fsm_prd);
      }

    break; // break de emergency
  } // fim switch máquina principal
} // fim função da máquina principal.


int main(void) {

  /* Interruption Config */
  static const EXTConfig extcfg = {
    {
      {EXT_CH_MODE_RISING_EDGE , emergency_cb},      /* INT0 Config. */
      {EXT_CH_MODE_DISABLED , NULL},      /* INT1 Config. */
      {EXT_CH_MODE_DISABLED , NULL},      /* INT2 Config. */
      {EXT_CH_MODE_DISABLED , NULL},      /* INT3 Config. */
    }
};


  halInit();
  chSysInit();

/* ChibiOS Config*/
  sdStart(&SD1, 0);
  extStart(&EXTD1, &extcfg);
  extChannelEnable(&EXTD1, INT0); // PD2 (4)

  chprintf((BaseSequentialStream *)&SD1, "INICIO MÁQUINA\n\r");
  starting();
  lavar = PARADA;
  fsm_parada_init(&fsm_prd);

  while(1){
    lavar_maquina();
  }
  return 0;
}
