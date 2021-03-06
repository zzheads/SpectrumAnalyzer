/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MSG
* Describe  : ��।������ ᮮ�饭��
* File      : C:\ALEX\WORK\AS_MSG.C
* Last edit : 30.06.93 00.54.11
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include "as_msg.h"

#ifdef ENGLISH				// �������筠� �����

const char *slMain    =  " F1=Help F2=Trend F3=Record F4=Cascade F5=Print F6=Capture F7=ChDir F8=ChTime AltX=Quit ";
const char *slControl =  " F1=Help F5=Print F6=Capture F8=ChTime                               AltX=Quit ";
const char *slRecord  =  " F1=Help F5=Print F6=Capture F8=ChTime                                        AltX=Quit ";
const char *slCascade =  " F1=Help F5=Print F6=Capture F8=ChTime                                        AltX=Quit ";
const char *slOptions =  " F1=Help                                                                      AltX=Quit ";
const char *slPrint   =  " F1=Help                                                                      AltX=Quit ";
const char *slGraph   =  " F5=Prn Sp,Bsp=Sp.curs AO,AP=Opt AF5/AF6=Bef/Aft AF7=Lg/Ln AF8=Scl AF9=Spe AF10/CF10=Itg/Dif";
const char *sl3DGraph =  " F5=Print F6=Capture AO,AP=Opt AF8=Scale AF9=Spe AF10/CF10=Integ/Diff Ent=Par        AX=Quit";
const char *sldbGraph =  " F5=Print F6=Capture AltF5/AltF6=Spe left/right AltO=Options AltP=Print opt   AltX=Quit ";
const char *slChFtime =  " Ins=Mark +/-=MarkAll /=AutoMark Enter=Load F7=ChDir Delete Esc=Cancel           AltX=Quit ";
const char *slChFtimes=  " Ins=Mark +/-=MarkAll /=AutoMark Enter=Load F7=ChDir AltC=Copy Esc=Cancel         AltX=Quit ";
const char *slChFile  =  " Enter=Load F7=ChDir F8=ChTime Esc=Cancel                                     AltX=Quit ";
const char *slChField =  " Enter=Load F8=ChTime Esc=Cancel                                              Altx=Quit ";
const char *slReceive =  " Tab=Edit Ins=Mark +/-=MarkAll *=AutoMark Enter=Save F7=ChDir F8=ChTime Esc=Break ";

char *msgMaxTime[] ={"You didn't choose time, so",
										 "will loaded last record.",
										 NULL};
char *msgEscape[] ={"Are You really want to",
										"delete all remaining records",
										"without saving to file?",
										NULL};
char *msgNoItems[] ={"Not found items, so",
										 "can't create menu.",
										 NULL};
char *msgImgSize[] ={"Size of menu is too large,",
										 "so can't create menu.",
										 NULL};
char *msgNoImgMem[] ={"No enough memory to save screen",
											"under menu, so can't create menu.",
											NULL};
char *msgChDir[] ={"Can't cross to specify directory.",
									 "This directory wasn't exist.",
									 NULL};
char *msgReceive[] ={"Make sure that PR90 ready and",
										 "there's notice 'Wait AD'.",
										 "Select <OK> for begin read,",
										 "<Cancel> for break operation.",
										 NULL};
char *msgSig[] =    {"Can't switch to logarithmic",
										 "scale, because current function",
										 "is signal. Only spectrum may be",
										 "switched to logarithmic scale.",
										 NULL};
char *msgLog[] =    {"Can't switch to logarithmic",
										 "scale, because current function",
										 "already is in logarithmic scale.",
										 NULL};
char *msgLin[] =    {"Can't switch to linear scale",
										 "because current function",
										 "already is in linear scale.",
										 NULL};
char *msgFFTSpe[] = {"Can't do FFT to spectrum",
										 "because current function",
										 "already is spectrum.",
										 NULL};
char *msgFFTSig[] = {"Can't do BackFFT to signal",
										 "because current function",
										 "already is signal.",
										 NULL};
char *msgNoDif[] =  {"Can't differentiate this function",
										 "because this operation allowed",
										 "only for displacement or velocity.",
										 NULL};
char *msgNoInt[]  = {"Can't integrating this function",
										 "because this operation allowed",
										 "only for velocity or acceleration.",
										 NULL};

const char *errNoMemory[] = {"No enough memory.",
														 "Select any button to abort operation.",
														 "Unload all resident programs and try again.",
														 NULL};
const char *errNoFiles[]  = {"Can't find files.",
														 "Select any button to abort operation.",
														 "Check, are there these files.",
														 NULL};
const char *errNoTime[]  =  {"Can't find time field in file .spe",
														 "Please, check format of file",
														 "and try again.",
														 NULL};
const char *errFormFile[] = {"Unknown or invalid format of file.",
														 "Select any button to abort operation.",
														 "Check, that format of file is right.",
														 NULL};
const char *errNoHelp[] = {"Help file not found.",
													 "File anlzr.hlp must be in",
													 "start program directory.",
														 NULL};
const char *longpathmes[] =  {"       Path is very long      ",
															NULL};


#else				// ���᪮��筠� �����

const char *slMain    =  " F1=������ F2=�७� F3=����� F4=��᪠�� F5=����� F7=������ F8=����६ AltX=��室 "; // F6=Capture
const char *slControl =  " F1=������ F5=����� F8=����६���                                            AltX=��室 ";
const char *slRecord  =  " F1=������ F5=����� F8=����६���                                            AltX=��室 ";
const char *slCascade =  " F1=������ F5=����� F8=����६���                                            AltX=��室 ";
const char *slOptions =  " F1=������                                                                    AltX=��室 ";
const char *slPrint   =  " F1=������                                                                    AltX=��室 ";
const char *slGraph   =  " F5=�� Sp,Bsp=��.��� AO,AP=��� AF5/AF6=��/���� AF7=��/�� AF8=��� AF9=��� AF10/CF10=��/���";
const char *sl3DGraph =  " F5=����� AO,AP=��� AF8=����⠡ AF9=������ AF10/CF10=��⥣��/���� Ent=��ࠬ���� AX=��室 ";
const char *sldbGraph =  " F5=����� AltF5/AltF6=�।/���� AltO=��樨 AltP=���.�ਭ��                  AltX=��室 ";
const char *slChFtime =  " Ins=���. +/-=���.��� /=��⮬�� Enter=����. F7=������ Delete=������ Esc=����� AltX=��� ";
const char *slChFtimes=  " Ins=���. +/-=���.��� /=��⮬�� Enter=����. F7=������ AltC=����஢ Esc=�����  AltX=��� ";
const char *slChFile  =  " Enter=����㧨�� F7=������ F8=����६ Esc=�����                                 AltX=��室 ";
const char *slChField =  " Enter=����㧨�� F8=����६ Esc=�����                                           Altx=��室 ";
const char *slReceive =  " Tab=������� Ins=��� +/-=���.��� *=��⮬�� Enter=���� F7=������ F8=����६ Esc=��室 ";

char *msgMaxTime[] ={"�� �� 㪠���� �६�, ���⮬�",
										 "����㦠���� ᫥����� ������.",
										 NULL};
char *msgEscape[] ={"�� ����⢨⥫쭮 ���",
										"������� �� ��⠢訥�� �����",
										"��� ��࠭���� �� � 䠩��?",
										NULL};
char *msgNoItems[] ={"���� ᮧ������",
										 "�� �� 祣�.",
										 NULL};
char *msgImgSize[] ={"�祭� ����讥 ����,",
										 "�� ���� ᮧ����.",
										 NULL};
char *msgNoImgMem[] ={"��� ����� ��� ��࠭����",
											"��࠭�, �� ᮧ��� ����.",
											NULL};
char *msgMkDir[] ={"����� ��४��� 㦥 ����.",
									 NULL};
char *msgReceive[] ={"            ���� �ਭ��� ����� � �ଠ�",
										 "",
										 "                                                              ",
										 "",
										 "   ᭠砫� ������� ����� �� �ਡ��, ��⥬ �� ��������",
										 "",
										 "                    � � � � � � � � !!!",
										 "�� ���ࠢ��쭮� ��⠭���� ⨯� �ਡ�� ����� ���� ������,",
										 "��⠭���� �ந�������� � ���� ���䨣���� | ��� �ਡ�� ",
										 NULL};
char *msgSig[] =    {"� �����䬨�᪮� 誠�� �����",
										 "��ᬠ�ਢ��� ⮫쪮 �४��.",
										 NULL};
char *msgLog[] =    {"�����䬨�᪠� 誠��",
										 "㦥 ��⠭������.",
										 NULL};
char *msgLin[] =    {"�������� 誠��",
										 "㦥 ��⠭������.",
										 NULL};
char *msgFFTSpe[] = {"������ 㦥 ����祭.",
										 NULL};
char *msgFFTSig[] = {"������ 㦥 ����祭",
										 NULL};
char *msgNoDif[] =  {"����७�஢��� �����",
										 "⮫쪮 ����᪮����,",
										 "��� ���ய�६�饭��.",
										 NULL};
char *msgNoInt[]  = {"��⥣�஢��� �����",
										 "⮫쪮 ����᪮����,",
										 "��� �����᪮७��.",
										 NULL};

const char *errNoMemory[] = {"�������筮 �����.",
														 "���஡�� ���㧨�� १������",
														 "�ணࠬ�� � ��१������� �ணࠬ��.",
														 NULL};
const char *errNoFiles[]  = {"� ������ ��४�ਨ",
														 "䠩�� ����������.",
														 NULL};
const char *errNoTime[]  =  {"�� ������� ���� Time � 䠩��",
														 " .spe �஢���� �ଠ� 䠩��.",
														 NULL};
const char *errFormFile[] = {"�� �ᯮ������� �ଠ� 䠩��.",
														 NULL};
const char *errNoHelp[] = {"�� ������ 䠩� �����.",
														 NULL};
const char *longpathmes[] =  {" ���誮� ������� ��� ��४�ਨ.",
															NULL};

#endif