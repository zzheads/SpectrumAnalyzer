/*****************************************************************************
*           : 
*           :     Copyright (c) 1993-94 by Alexey V. Papin. 
*           :   This information may not be changed or copyed 
*           :       without special permission of author. 
*           : 
* Name      : AS_KEYS     
* Describe  :  
* File      : C:\ALEX\WORK\GRAPH\AS_KEYS.H        
* Last edit : 03.01.93 02.26.10
* Remarks   : 
*****************************************************************************/

#ifndef KEYS 

#define KEYS   1

/*      KB_S_UP                                                              */
/*       ^ ^  ^                                                              */
/*       | |  |                                                              */
/*       | |  +-------- Название клавиши                                     */
/*       | |                                                                 */
/*       | +----------- S значит Shift, может быть N - нормальная,           */
/*       |              S - с Shift, C - с Ctrl, или A - с Alt.              */
/*       |                                                                   */
/*       +------------- KB значит KeyBoard.                                  */

#define KB_LAST            -1

#define KB_N_SPACE         32
#define KB_N_BS            8
#define KB_N_ENTER         13
#define KB_N_ESC           27
#define KB_N_TAB           9
#define KB_N_UP            72
#define KB_N_DOWN          80
#define KB_N_LEFT          75
#define KB_N_RIGHT         77
#define KB_N_HOME          71
#define KB_N_END           79
#define KB_N_PGUP          73
#define KB_N_PGDN          81
#define KB_N_INS           82
#define KB_N_DEL           83
#define KB_N_F1            59
#define KB_N_F2            60
#define KB_N_F3            61
#define KB_N_F4            62
#define KB_N_F5            63
#define KB_N_F6            64
#define KB_N_F7            65
#define KB_N_F8            66
#define KB_N_F9            67
#define KB_N_F10           68
#define KB_N_F11        // нет
#define KB_N_F12        // нет

#define KB_S_BS         // аналогично KB_N_..
#define KB_S_ENTER      //
#define KB_S_ESC        //
#define KB_S_TAB           15
#define KB_S_UP         // аналогично KB_N_..
#define KB_S_DOWN       //
#define KB_S_LEFT       //
#define KB_S_RIGHT      //
#define KB_S_HOME       //
#define KB_S_END        //
#define KB_S_PGUP       //
#define KB_S_PGDN       //
#define KB_S_INS        //
#define KB_S_DEL        //
#define KB_S_F1             84
#define KB_S_F2             85
#define KB_S_F3             86
#define KB_S_F4             87
#define KB_S_F5             88
#define KB_S_F6             89
#define KB_S_F7             90
#define KB_S_F8             91
#define KB_S_F9             92
#define KB_S_F10            93

#define KB_A_BS         // нет кода
#define KB_A_ENTER      //
#define KB_A_ESC        //
#define KB_A_TAB        //
#define KB_A_UP         //
#define KB_A_DOWN       //
#define KB_A_LEFT       //
#define KB_A_RIGHT      //
#define KB_A_HOME       //
#define KB_A_END        //
#define KB_A_PGUP       //
#define KB_A_PGDN       //
#define KB_A_INS        //
#define KB_A_DEL        //
#define KB_A_F1             104
#define KB_A_F2             105
#define KB_A_F3             106
#define KB_A_F4             107
#define KB_A_F5             108
#define KB_A_F6             109
#define KB_A_F7             110
#define KB_A_F8             111
#define KB_A_F9             112
#define KB_A_F10            113

#define KB_A_A              30
#define KB_A_B              48
#define KB_A_C              46
#define KB_A_D              32
#define KB_A_E              18
#define KB_A_F              33
#define KB_A_G              34
#define KB_A_H              35
#define KB_A_I              23
#define KB_A_J              36
#define KB_A_K              37
#define KB_A_L              38
#define KB_A_M              50
#define KB_A_N              49
#define KB_A_O              24
#define KB_A_P              25
#define KB_A_Q              16
#define KB_A_R              19
#define KB_A_S              31
#define KB_A_T              20
#define KB_A_U              22
#define KB_A_V              47
#define KB_A_W              17
#define KB_A_X              45
#define KB_A_Y              21
#define KB_A_Z              44

#define KB_C_BS             127
#define KB_C_ENTER          10
#define KB_C_ESC         // как KB_N_ESC
#define KB_C_TAB         // нет
#define KB_C_UP          //
#define KB_C_DOWN        //
#define KB_C_LEFT           115
#define KB_C_RIGHT          116
#define KB_C_HOME           119
#define KB_C_END            117
#define KB_C_PGUP           132
#define KB_C_PGDN           118
#define KB_C_INS         // нет
#define KB_C_DEL         //
#define KB_C_F1             94
#define KB_C_F2             95
#define KB_C_F3             96 
#define KB_C_F4             97 
#define KB_C_F5             98 
#define KB_C_F6             99 
#define KB_C_F7             100
#define KB_C_F8             101
#define KB_C_F9             102
#define KB_C_F10            103

#endif