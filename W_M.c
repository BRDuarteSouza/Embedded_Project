// W_M.c
// Implement a state machine to execute a washing machine taks, with ChibiOS
// Author: Bruno Duarte <brduart95@gmail.com>

#include <ch.h>
#include <hal.h>

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
  systime_t tempo;
  long ul_centri;
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


/* Variables of the states */
fsm_parada_s fsm_prd;

fsm_molho_s fsm_ml;

fsm_lavagem_s fsm_lvg;

fsm_enxague_s fsm_exg;

fsm_centrifuga_s fsm_cen;


/* Functions*/
void setup(){
  
}



int main(void){
  while(1){
    lavar_maquina();
  }
  //casa
  return 0;
}
