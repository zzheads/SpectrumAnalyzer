/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : M_OPTION
* Describe  :
* File      : C:\ALEX\WORK\M_OPTION.H
* Last edit : 30.06.93 01.06.12
* Remarks   :
*****************************************************************************/

enum asWindType   { REC, BAR, PAR, HAN, HAM, BLC, FTP };
enum asCurType	{HARM,SIDE,DELT};

void options (void);
void cursortype (void);
void windowtype (void);
void set_rec_type (void);	// Установка какой прибор подключен (т.е.формат)
void setrec (void);
void setbar (void);
void setpar (void);
void sethan (void);
void setham (void);
void setblc (void);
void set_gips (void);
void set_tables (void);
void set_graphmode (void);	// Установка графического режима
void set_graph (void);
void set_port (void);
void save_config (void);
void view_config (void);
