/**************************************************************************
* Файл, содержащий декларации функций, приведенных в драйвере LBIOSDRV.ASM*
*                                                                	  *
* 	      L-card SoftWare Lab limited, Moscow, 03.05.95               *
*       	tel (095) 257-17-10, fax (095) 257-10-90		  *
**************************************************************************/
#include <stdlib.h>

#define LM101 0x4
#define LM104 0x8
#define LM102 0x12
#define LM201 0x2
#define LM202 0xf
#define LM301 0x14
#define LM401 0x18
#define LM402 0x1c
#define LM501 0xa

#define NLM   9

#ifndef __cplusplus
#define PARM void
#else
#define PARM ...
#endif

#ifdef __cplusplus
extern "C" {
#endif
void  	_Cdecl SETBASEADDRESS(int address);
int 	_Cdecl PLATA_TEST(void);
int   	_Cdecl MEMORY_STATE(void);
int   	_Cdecl MEMORY_PM_STATE(void);

void  	_Cdecl SETCHANNEL(int Nch);
int   	_Cdecl SAMPLE(void);
int   	_Cdecl ADCHAN(int Channel);
void  	_Cdecl KADR(int far *Data, int far *RealChannel, int Nch);
void  	_Cdecl STREAM(int far *Data, int NPoint, int Channel, int Rate);
void  	_Cdecl SOFT(int far *Data, int NKadr, int far *RealChannel, int Nch, int Rate);
int  	_Cdecl DMAONE(int far *Data, int NPoint, int Channel, int Rate, int dmm);
int 	_Cdecl DMAALL(int far *Data, int NKadr, int far *RealChannel,int Nch, int Rate, int dmm);
void 	_Cdecl DMA_ALL_DA(int *AdData, int AdNkadr, int far *Channels, int Nch, int Rate, int AdDmm, int *DaData, int DaNkadr, int DaDmm, int DaNumber);
void 	_Cdecl SYNCHRO_MODE(int SMode, int TtlMask, int AdChannel, int AdValue);

void 	_Cdecl SOFT_MEMORY_START(unsigned long Nkadr, int far *Channels, int Nch, int Rate, int PageNumber, int OffsetNumber, int DelayCh, int RestartMode);
int	_Cdecl TEST_FOR_INPUT_END(void);
void 	_Cdecl GET_INPUT_DATA(int *Data, int Npoint);
void 	_Cdecl SOFT_MEMORY_RESTART(void);

void 	_Cdecl SET_DA_NUMBER(int Mode, int DaNumber);
void  	_Cdecl OUTDA(int code);
void 	_Cdecl STREAM_OUTDA(int Code);
void  	_Cdecl DASTREAM(int rate, int NPoint, int far *Data);
void    _Cdecl DADMASTREAM(int far *Data,int Npoint,int Rate,int Dmm);

int 	_Cdecl INPBYTE(void);
void  	_Cdecl OUTBYTE(int code);
int 	_Cdecl DSP_INPBYTE(void);
void  	_Cdecl DSP_OUTBYTE(int code);
int 	_Cdecl INPBYTE_305(void);
void  	_Cdecl OUTBYTE_305(int code);

void  	_Cdecl INTR_SETUP(int irq_number);
void  	_Cdecl INIT_SIMPLE_INTR(int rate, void interrupt int_hand(PARM));
void  	_Cdecl STREAM_INTR(int rate, void interrupt int_hand(PARM), int Channel);
void  	_Cdecl SOFT_INTR(int rate, void interrupt int_hand(PARM), int far *RealChannel, int Nch);
void 	_Cdecl STOP_INTR(void);
int	_Cdecl READ_DATA(void);
void 	_Cdecl RESET_IRQ(void);

void  	_Cdecl DMA_OFF(void);
int 	_Cdecl DMA_TEST(void);
int 	_Cdecl DMA_ADDRESS_TEST(int far *Data, int NPoint);
int 	_Cdecl DMA_COUNTER(void);

void    _Cdecl FLT_SOFT_REPEAT(int far *Data, int Nkadr, int Nch);
void    _Cdecl FLT_DMA_REPEAT(int far *Data, int Nkadr, int Nch, int dmm);
void    _Cdecl FILTER_SOFT1(int far *Data, int Nkadr, unsigned int far *CtrlTable);
void    _Cdecl FILTER_SOFT(int far *Data, int Nkadr, unsigned int far *CtrlTable);

void	_Cdecl INIT_COMPARATOR(int Chan, int Porog, int BitNPoint, int HandleMode, int DmaOn, int *Data, int DmaLength, int DmaMode);
int 	_Cdecl DATA_READY(void);

void 	_Cdecl SET_ADSP_SPEED(int type);
void 	_Cdecl SET_WAIT_STATE(int wait_state);
void  	_Cdecl SET_TIMER_SCALE(int value);
void	_Cdecl SET_INTER_DELAY(int scale);
void 	_Cdecl CONFIG_FIFO(int fifo_mode);
void 	_Cdecl READ_FIFO(int far *Data, int Npoint);
void 	_Cdecl CONFIG_2FIFO_TYPE(int fifo_type, int fifo_length);
void 	_Cdecl STREAM_FIFO2_BEGIN(int Rate, void interrupt int_hand(PARM), int far *Channels, int Nch);

void 	_Cdecl PROGRAM_AMPLIFIER(int KrateNumber, int ModuleNumber, int Usil1, int Usil2, int InputSource);
int 	_Cdecl PROGRAM_FILTER(int KrateNumber, int ModuleNumber, int CutFrequency, int AnalogFilterType);
void 	_Cdecl PROGRAM_BKS(int Ch, unsigned long value);

int	_Cdecl GET_SLOT_CODE(int Crait, int Slot);
int 	_Cdecl MAKE_CHANNEL(int Crait, int Slot, int chan, int kadr, int usil);
void	_Cdecl RESET_ALL(void);
void 	_Cdecl PROGRAM_LM_201(int Crait, int Slot, int chan, int usil, int source);
void	_Cdecl PROGRAM_LM_102(int Crait, int Slot, int Usil);
void	_Cdecl PROGRAM_LM_DAC(int Crait, int Slot, int mode, int code, int chan);
void 	_Cdecl PROGRAM_LM_TTLOUT(int Crait, int Slot, int code);
void    _Cdecl PROGRAM_LM_501(int Crait, int Slot, int M3, int M2, int M1, int EN, int EP, int Chan1, int Chan2);
int	_Cdecl GET_LM_401(int Crait, int Slot);
int     _Cdecl LM_404(int Code);

void 	_Cdecl CALIBRATION(void);
void 	_Cdecl SET_UVX(int key);
void  	_Cdecl SET_MULTI(int key);
void  	_Cdecl LOW_POWER(int key);

unsigned long _Cdecl AllCoreLeft(void);
void* _Cdecl  dmaalloc(unsigned long SizeNeed);

#ifdef __cplusplus
}
#endif