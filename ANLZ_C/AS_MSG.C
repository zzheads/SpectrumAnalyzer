/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_MSG
* Describe  : Определения сообщений
* File      : C:\ALEX\WORK\AS_MSG.C
* Last edit : 30.06.93 00.54.11
* Remarks   :
*****************************************************************************/

#include <stdio.h>
#include "as_msg.h"

#ifdef ENGLISH				// Англоязычная версия

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


#else				// Русскоязычная версия

const char *slMain    =  " F1=Помощь F2=Тренд F3=Записи F4=Каскады F5=Печать F7=ИзмДир F8=ИзмВрем AltX=Выход "; // F6=Capture
const char *slControl =  " F1=Помощь F5=Печать F8=ИзмВремени                                            AltX=Выход ";
const char *slRecord  =  " F1=Помощь F5=Печать F8=ИзмВремени                                            AltX=Выход ";
const char *slCascade =  " F1=Помощь F5=Печать F8=ИзмВремени                                            AltX=Выход ";
const char *slOptions =  " F1=Помощь                                                                    AltX=Выход ";
const char *slPrint   =  " F1=Помощь                                                                    AltX=Выход ";
const char *slGraph   =  " F5=Пч Sp,Bsp=Сп.кур AO,AP=Опц AF5/AF6=Прд/След AF7=Лг/Лн AF8=Мшт AF9=Спе AF10/CF10=Ин/Диф";
const char *sl3DGraph =  " F5=Печать AO,AP=Опц AF8=Масштаб AF9=Спектр AF10/CF10=Интегрир/Дифф Ent=Параметры AX=Выход ";
const char *sldbGraph =  " F5=Печать AltF5/AltF6=Пред/След AltO=Опции AltP=Опц.принтера                  AltX=Выход ";
const char *slChFtime =  " Ins=Марк. +/-=Марк.всех /=Автомарк Enter=Загр. F7=ИзмДир Delete=Удален Esc=Назад AltX=Вых ";
const char *slChFtimes=  " Ins=Марк. +/-=Марк.всех /=Автомарк Enter=Загр. F7=ИзмДир AltC=Копиров Esc=Назад  AltX=Вых ";
const char *slChFile  =  " Enter=Загрузить F7=ИзмДир F8=ИзмВрем Esc=Назад                                 AltX=Выход ";
const char *slChField =  " Enter=Загрузить F8=ИзмВрем Esc=Назад                                           Altx=Выход ";
const char *slReceive =  " Tab=Редактир Ins=Марк +/-=Марк.всех *=Автомарк Enter=Сохр F7=ИзмДир F8=ИзмВрем Esc=Выход ";

char *msgMaxTime[] ={"Вы не указали время, поэтому",
										 "загружается следующая запись.",
										 NULL};
char *msgEscape[] ={"Вы действительно хотите",
										"потерять все оставшиеся записи",
										"без сохранения их в файле?",
										NULL};
char *msgNoItems[] ={"Меню создавать",
										 "не из чего.",
										 NULL};
char *msgImgSize[] ={"Очень большое меню,",
										 "не могу создать.",
										 NULL};
char *msgNoImgMem[] ={"Нет памяти для сохранения",
											"экрана, не создаю меню.",
											NULL};
char *msgMkDir[] ={"Такая директория уже есть.",
									 NULL};
char *msgReceive[] ={"            Будут приняты данные в формате",
										 "",
										 "                                                              ",
										 "",
										 "   сначала запустите обмен на приборе, затем на компьютере",
										 "",
										 "                    В Н И М А Н И Е !!!",
										 "при неправильной установке типа прибора данные будут потеряны,",
										 "установка производится в меню Конфигурация | Тип прибора ",
										 NULL};
char *msgSig[] =    {"В логарифмической шкале можно",
										 "просматривать только спректр.",
										 NULL};
char *msgLog[] =    {"Логарифмическая шкала",
										 "уже установлена.",
										 NULL};
char *msgLin[] =    {"Линейная шкала",
										 "уже установлена.",
										 NULL};
char *msgFFTSpe[] = {"Спектр уже получен.",
										 NULL};
char *msgFFTSig[] = {"Сигнал уже получен",
										 NULL};
char *msgNoDif[] =  {"Дифференцировать можно",
										 "только виброскорость,",
										 "или виброперемещение.",
										 NULL};
char *msgNoInt[]  = {"Интегрировать можно",
										 "только виброскорость,",
										 "или виброускорение.",
										 NULL};

const char *errNoMemory[] = {"Недостаточно памяти.",
														 "Попробуйте выгрузить резидентные",
														 "программы и перезапустить программу.",
														 NULL};
const char *errNoFiles[]  = {"В данной директории",
														 "файлы отсутствуют.",
														 NULL};
const char *errNoTime[]  =  {"Не найдено поле Time в файле",
														 " .spe проверьте формат файла.",
														 NULL};
const char *errFormFile[] = {"Не распознается формат файла.",
														 NULL};
const char *errNoHelp[] = {"Не найден файл помощи.",
														 NULL};
const char *longpathmes[] =  {" Слишком длинное имя директории.",
															NULL};

#endif