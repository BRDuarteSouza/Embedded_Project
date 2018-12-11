  #ifndef TRANSITIONS_H
#define TRANSITIONS_H

#include "event.h"
#include "sm.h"


cb_status s_cb(event_t ev);

cb_status s1_main_machine_cb(event_t ev);

cb_status s11_parada_cb(event_t ev);

cb_status s12_molho_cb(event_t ev);
cb_status s121_enche_cb(event_t ev);
cb_status s123_gira_r_cb(event_t ev);
cb_status s124_gira_l_cb(event_t ev);

cb_status s13_lavagem_cb(event_t ev);
cb_status s131_gira_r_cb(event_t ev);
cb_status s132_gira_l_cb(event_t ev);
cb_status s133_espera_cb(event_t ev);
cb_status s134_esvazia_cb(event_t ev);

cb_status s14_enxague_cb(event_t ev);
cb_status s141_enche_cb(event_t ev);
cb_status s142_gira_r_cb(event_t ev);
cb_status s143_gira_l_cb(event_t ev);
cb_status s144_esvazia_cb(event_t ev);

cb_status s15_centrifuga_cb(event_t ev);
cb_status s151_gira_r_cb(event_t ev);

cb_status s2_emergency_cb(event_t ev);

cb_status s11_cb(event_t ev);
cb_status s2_cb(event_t ev);
cb_status s21_cb(event_t ev);
cb_status s211_cb(event_t ev);


#define Topo_init_tran() do {                   \
                push_state(s_cb);               \
                dispatch(ENTRY_EVENT);          \
                push_state(s1_main_machine_cb); \
                dispatch(ENTRY_EVENT);          \
} while (0); \

#define start_ser1_tran() do {                  \
                replace_state(s12_molho_cb);    \
                dispatch(EXIT_EVENT);           \
                dispatch(ENTRY_EVENT);          \
      } while (0); \


#define start_ser2_tran() do {                  \
    replace_state(s13_lavagem_cb);              \
    dispatch(EXIT_EVENT);                       \
    dispatch(ENTRY_EVENT);                      \
} while (0);                        \


#define start_ser3_tran() do {                  \
  replace_state(s14_enxague_cb);                \
  dispatch(EXIT_EVENT);  \
  dispatch(ENTRY_EVENT);                         \
} while (0); \


#define start_ser4_tran() do {\
  replace_state(s15_centrifuga_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define molho_dir_tran() do {\
  replace_state(s123_gira_r_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define molho_esq_tran() do {\
  replace_state(s124_gira_l_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define molho_fim_tran() do {                  \
  exit_inner_states(); \
  dispatch(EXIT_EVENT); \
  replace_state(s13_lavagem_cb); \
  dispatch(ENTRY_EVENT); \
  dispatch(INIT_EVENT); \
} while (0); \


#define lavagem_dir_tran() do {                \
  replace_state(s131_gira_r_cb);               \
  dispatch(EXIT_EVENT);                        \
  dispatch(ENTRY_EVENT);                       \
} while (0)                                    \


#define lavagem_esq_tran() do {                \
  replace_state(s132_gira_l_cb);               \
  dispatch(EXIT_EVENT);                        \
  dispatch(ENTRY_EVENT);                       \
} while (0) \


#define lavagem_espera_trans() do {            \
  replace_state(s133_espera_cb);                        \
  dispatch(EXIT_EVENT);                         \
  dispatch(ENTRY_EVENT); \
} while (0)   \


#define lavagem_secar_tran() do { \
  replace_state(s134_esvazia_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0) \


#define lavagem_fim_tran() do { \
  exit_inner_states(); \
  dispatch(EXIT_EVENT);   \
  replace_state(s14_enxague_cb); \
  dispatch(ENTRY_EVENT); \
  dispatch(INIT_EVENT); \
} while(0) \


#define enxague_dir_tran() do{  \
  replace_state(s142_gira_r_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define enxague_esq_tran() do{ \
  dispatch(EXIT_EVENT);  \
  replace_state(s143_gira_l_cb); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define enxague_secar_tran() do{ \
  replace_state(s144_esvazia_cb); \
  dispatch(EXIT_EVENT); \
  dispatch(ENTRY_EVENT); \
} while (0); \


#define enxague_fim_tran() do { \
  exit_inner_states(); \
  dispatch(EXIT_EVENT); \
  replace_state(s15_centrifuga_cb); \
  dispatch(ENTRY_EVENT); \
  dispatch(INIT_EVENT); \
} while (0); \


#define centri_fim_tran() do { \
  exit_inner_states(); \
  dispatch(EXIT_EVENT); \
  replace_state(s11_parada_cb); \
  dispatch(ENTRY_EVENT); \
  dispatch(INIT_EVENT); \
} while(0); \


#endif
// #define s_init_tran() do {                      \
//                 push_state(s1_cb);              \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s_s11_local_tran() do {                 \
//                 exit_inner_states();            \
//                 push_state(s1_cb);              \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s1_init_tran() do {                     \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s1_s_local_tran() do {                  \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(INIT_EVENT);           \
//         } while (0)
//
// #define s1_s1_tran() do {                       \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 dispatch(ENTRY_EVENT);          \
//                 dispatch(INIT_EVENT);           \
//         } while (0)
//
// #define s1_s11_local_tran() do {                \
//                 exit_inner_states();            \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s1_s2_tran() do {                       \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s2_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 dispatch(INIT_EVENT);           \
//         } while (0)
//
// #define s1_s211_tran() do {                     \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s2_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s21_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s211_cb);            \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s11_s_local_tran() do {                 \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #define s11_s1_local_tran() do {                \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #define s11_s211_tran() do {                    \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s2_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s21_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s211_cb);            \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s2_init_tran() do {                     \
//                 push_state(s21_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s211_cb);            \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s2_s1_tran() do {                       \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s1_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #define s2_s11_tran() do {                      \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s1_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while(0)
//
// #define s21_init_tran() do {                    \
//                 push_state(s211_cb);            \
//                 dispatch(ENTRY_EVENT);          \
//         } while (0)
//
// #define s21_s21_tran() do {                     \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 dispatch(ENTRY_EVENT);          \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #define s21_s211_local_tran() do {              \
//                 exit_inner_states();            \
//                 push_state(s211_cb);            \
//                 dispatch(ENTRY_EVENT);          \
//         } while(0)
//
// #define s21_s11_tran() do {                     \
//                 exit_inner_states();            \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(EXIT_EVENT);           \
//                 replace_state(s1_cb);           \
//                 dispatch(ENTRY_EVENT);          \
//                 push_state(s11_cb);             \
//                 dispatch(ENTRY_EVENT);          \
//         } while(0)
//
// #define s211_s_local_tran() do {                \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #define s211_s21_local_tran() do {              \
//                 dispatch(EXIT_EVENT);           \
//                 pop_state();                    \
//                 dispatch(INIT_EVENT);           \
//         } while(0)
//
// #endif /* TRANSITIONS_H */
