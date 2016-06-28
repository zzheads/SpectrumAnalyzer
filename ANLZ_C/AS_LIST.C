/*****************************************************************************
*           :
*           :     Copyright (c) 1993-94 by Alexey V. Papin.
*           :   This information may not be changed or copyed
*           :       without special permission of author.
*           :
* Name      : AS_LIST
* Describe  : Гpафическое меню (стp. 141)
* File      : D:\BC\WORK\AS_LIST.C
* Last edit : 12-10-93 08:19:45pm
* Remarks   :
*****************************************************************************/

#include <graphics.h>
#include <string.h>
#include <alloc.h>
#include <conio.h>
#include "as_ftime.h"
#include "as_wind.h"
#include "as_keys.h"
#include "as_list.h"
#include "as_check.h"

static void movelightbox (int box_newcol,int box_newrow);
static void drawitembox  (int drawbox_col,int drawbox_row,
                          int item_color,int box_color);
static void draw_about (int drawbox_col,int drawbox_row);
static void editname (int l_i);


static asList *listg;
static int  cols,
            rows,
            maxlen,
            char_xsize,
            char_ysize,
            col_step,
            row_step,
            box_xsize,
            box_ysize,
            x0_box,
            y0_box,
            x_box,
            y_box,
            light_item,
            light_box_col,
            light_box_row,
            page_beg_item,
            page_end_item,
            info_xsize,
						info_ysize,
            panel_color,
            frame_color,
            title_color,
            item_color,
            light_item_color,
						light_box_color;
extern asHotKey hotkeys[];
static char *item_ptr;
static char PgUp_prompt[]="< PgUp >",
	    PgDn_prompt[]="< PgDn >";

int grlist1 (char *menu_title,asList list[],int page_left,int page_top,
			int page_cols,int page_rows)
/***********                        Menu Anlz|Trend & |Record & |Cascade & Export & File|ChDir
* Describe :                        display filenames
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
						prev_light_box_col,prev_light_box_row;
 struct viewporttype prev_viewport;
 int key,i;
 int items,
     curmode,
     maxx,
     maxy,
		 page_xsize,
     page_ysize,
		 title_page_size,
     prompt_page_size,
     draw_title,
		 title_xsize,
     Pg_box_xsize,
		 PgUp_exist,
     PgDn_exist,
     col_box,
     row_box,
     return_item,
     page_volume,
		 full_rows,
     full_cols,
     items_on_page,
     num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 getviewsettings (&prev_viewport);		// запоминаем параметры пред.окна
 setviewport(0,0,getmaxx(),getmaxy(),1);	// уст. начало от полн.экрана

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
  case 0x06 :                                              /* монохpомная    */
	case 0x0f :                                              /* гpафика        */
  case 0x11 :                                              /*                */
  case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
              title_color = 0;                             /*                */
							item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
							frame_color = 0;                             /* палитpы        */
							title_color = 0;                             /*                */
							item_color  = CGA_LIGHTGRAY;                 /*                */
							light_item_color = CGA_MAGENTA;              /*                */
							light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
	case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
              frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
              item_color  = BLACK;                         /*                */
							light_item_color = LIGHTGREEN;               /*                */
							light_box_color = RED;                       /*                */
							break;                                       /*                */
																													 /*                */
	default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
	return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
	maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<=items)
 {
	light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
  light_box_col = prev_light_box_col;
  light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
  light_box_col = 0;
  light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
	return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

// settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize =	(int) textwidth ("W");				// 6;
 char_ysize =	(int) textheight ("Ry");				// 8;
 info_xsize = 40*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
  title_xsize = 0;
  draw_title = 0;
 }
 else
 {
  title_xsize = textwidth(menu_title)+char_xsize;
  draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
	x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
	x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
		 (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
  return_item = menuErr_ScrExceed;
  goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
													 page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
  return_item = menuErr_ImgSize;
  goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
  return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,0);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
	bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
  settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
	full_cols=items_on_page/rows;
  full_rows=(items_on_page-1) % rows + 1;

	if (page_beg_item>0)
	 PgUp_exist=1;
	else
   PgUp_exist=0;
  if (page_beg_item+page_volume<items)
   PgDn_exist=1;
	else
   PgDn_exist=0;
	setcolor(frame_color);
  if (PgUp_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
	 outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
	 line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
	}
  if (PgDn_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
	 settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
	 outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
	 line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

  setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

	for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
   for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
   {
    bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
		if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
	 }

  movelightbox(light_box_col,light_box_row);

  while(1)
  {
	 if ((key=getch())!=0)
   {
    switch (key)
    {
     case KB_N_ENTER : return_item=light_item+1;
                       goto MENU_EXIT;

		 case KB_N_ESC   : return_item=menuEscape;
											 goto MENU_EXIT;

     default : continue;
    }
	 }
   else
	 {
    switch(key=getch())
    {
     case 75 : if (cols>1)        /* left */
               {
                if (light_box_col>0)
								 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
               }
	       else
                movelightbox(0,0);
               continue;

		 case 77 : if (cols>1)        /* right */
               {
                if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
									movelightbox (light_box_col+1,light_box_row);
                 else
									movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
               }
               else
                movelightbox(cols-1,full_rows-1); /* нижний правый угол */
							 continue;

     case 72 : if (rows>1)   /* up */
               {
                if (light_box_row>0)
                 movelightbox(light_box_col,light_box_row-1);
                else
								 if (light_box_col>0)
									movelightbox(light_box_col-1,rows-1);
               }
               continue;

		 case 80 : if (rows>1)   /* down */
               {
								if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
									movelightbox (light_box_col+1,0);
	       }
							 continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
               continue;

     case 79 : row_box = full_rows-1;      /* end */
							 col_box = (full_rows==rows) ? full_cols-1 : full_cols;
							 if ((light_box_row!=row_box)||(light_box_col!=col_box))
                 movelightbox(col_box,row_box);
               continue;

		 case 73 : if (PgUp_exist)             /* PgUp */
               {
								page_end_item = page_beg_item;
                page_beg_item = page_end_item-page_volume+1;
               }
               else
								continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {                           /* No full page-move curs */
               if((items-page_end_item-1)<(light_box_col*rows+light_box_row))
                   movelightbox((items-page_end_item-1)/rows,(items-page_end_item-1)%rows);
                page_beg_item = page_end_item;
                page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
								light_item = page_beg_item;
							 }
               else
                continue;
							 break;
													/* изменяем директорию */
		 case KB_N_F7	 : changedir();
										 return_item=menuEscape;
										 goto MENU_EXIT;

		 default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
								if ((hotkeys[i].key==key)&&(hotkeys[i].able))
								{
		 setviewport(0,0,getmaxx(),getmaxy(),1);
                 hotkeys[i].action ();
								 setviewport(page_left,page_top,page_left+page_xsize,
														 page_top+page_ysize,0);
                }
               continue;
		}
	 }
	 break;
	}
 }
 MENU_EXIT :  putimage (0,0,image_saved,COPY_PUT);	// восстанавливаем картинку
							farfree (image_saved);		// и параметры пред. окна
							setviewport(prev_viewport.left,prev_viewport.top,
											prev_viewport.right,prev_viewport.bottom,prev_viewport.clip);

 MENU_ABORT :
							prev_light_item = light_item ;
							prev_page_beg_item = page_beg_item;
							prev_light_box_col = light_box_col;
							prev_light_box_row = light_box_row;

							return return_item;
}

int grlist2 (char *menu_title,asList list[],int page_left,int page_top,
						int page_cols,int page_rows)
/***********                        Meny Anlz|Record & |Cascade, Files|Receive
* Describe :                        display date files,          zap_names
* Params   : char *menu_title       - заголовок меню
*          : asList list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
						prev_light_box_col,prev_light_box_row;
 struct viewporttype prev_viewport;
 int key,i;
 int items,
		 curmode,
     maxx,
		 maxy,
     page_xsize,
		 page_ysize,
     title_page_size,
     prompt_page_size,
     draw_title,
     title_xsize,
     Pg_box_xsize,
		 PgUp_exist,
     PgDn_exist,
		 col_box,
     row_box,
     return_item,
     page_volume,
		 full_rows,
     full_cols,
     items_on_page,
     num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 getviewsettings (&prev_viewport);		// запоминаем параметры пред.окна
 setviewport(0,0,getmaxx(),getmaxy(),1);	// уст. начало от полн.экрана

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
	case 0x06 :                                              /* монохpомная    */
	case 0x0f :                                              /* гpафика        */
	case 0x11 :                                              /*                */
	case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
							title_color = 0;                             /*                */
              item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
							break;                                       /*                */
																													 /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
              frame_color = 0;                             /* палитpы        */
              title_color = 0;                             /*                */
							item_color  = CGA_LIGHTGRAY;                 /*                */
              light_item_color = CGA_MAGENTA;              /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
	case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
							frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
							item_color  = BLACK;                         /*                */
              light_item_color = LIGHTGREEN;               /*                */
              light_box_color = RED;                       /*                */
              break;                                       /*                */
                                                           /*                */
	default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
  return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
  maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<items)
 {
	light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
	light_box_col = prev_light_box_col;
	light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
	light_box_col = 0;
	light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
	return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

// settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize =	(int) textwidth ("W");				// 6;
 char_ysize =	(int) textheight ("Ry");				// 8;
 info_xsize = 50*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
  title_xsize = 0;
  draw_title = 0;
 }
 else
 {
	title_xsize = textwidth(menu_title)+char_xsize;
	draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
	x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
	x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
     (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
	return_item = menuErr_ScrExceed;
	goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
													 page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
  return_item = menuErr_ImgSize;
  goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
	return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,0);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
	bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
	settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
	full_cols=items_on_page/rows;
	full_rows=(items_on_page-1) % rows + 1;

  if (page_beg_item>0)
   PgUp_exist=1;
	else
   PgUp_exist=0;
  if (page_beg_item+page_volume<items)
	 PgDn_exist=1;
	else
   PgDn_exist=0;
	setcolor(frame_color);
  if (PgUp_exist)
	{
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
	{
	 setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
	}
  if (PgDn_exist)
	{
   setfillstyle(SOLID_FILL,panel_color);
	 settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
	 outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
	else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

	setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

	for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
	 for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
   {
    bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
    if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
	 }

	movelightbox(light_box_col,light_box_row);

	while(1)
	{
	 if ((key=getch())!=0)
	 {
		switch (key)
		{
		 case KB_N_ENTER : return_item=light_item+1;
											 goto MENU_EXIT;

		 case KB_N_ESC   : return_item=menuEscape;
											 goto MENU_EXIT;

		 case 43 : for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
								if (list[num_item].name[0]==' ')    /* Gray + */
								{
								 list[num_item].mark=1;
								 list[num_item].name[0]='*';
                }
								break;

     case 45 : for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
                if (list[num_item].name[0]=='*')   /* Gray - */
                {
                 list[num_item].mark=0;
                 list[num_item].name[0]=' ';
								}
								break;

		 case '*': check ((asList *) listg+page_beg_item,page_volume);
               break;

     case '/': for (num_item=page_beg_item;num_item<page_beg_item+page_volume;num_item++)
								if (!strcmp(list[light_item].info,list[num_item].info))
                {
                 list[num_item].mark=1;
                 list[num_item].name[0]='*';
								}
								break;

     case KB_N_TAB : editname (light_item);
                     movelightbox(light_box_col,light_box_row);
                     continue;

     default : continue;
    }
	 }
	 else
	 {
    switch(key=getch())
    {
		 case 75 : if (cols>1)        /* left */
               {
								if (light_box_col>0)
                 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
							 }
               else
								movelightbox(0,0);
               continue;

     case 77 : if (cols>1)        /* right */
               {
                if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
									movelightbox (light_box_col+1,light_box_row);
								 else
									movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
							 }
               else
								movelightbox(cols-1,full_rows-1); /* нижний правый угол */
               continue;

     case 72 : if (rows>1)   /* up */
							 {
                if (light_box_row>0)
								 movelightbox(light_box_col,light_box_row-1);
                else
                 if (light_box_col>0)
                  movelightbox(light_box_col-1,rows-1);
               }
               continue;

		 case 80 : if (rows>1)   /* down */
							 {
								if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
								 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
							 }
               continue;

     case 82 : num_item = page_beg_item+light_box_col*rows+light_box_row;
							 if ((list[num_item].name[0]==' ')||(list[num_item].name[0]=='*'))
               {
								if (!list[num_item].mark)
                {                                       /* Insert */
                 list[num_item].mark=1;
                 list[num_item].name[0]='*';
                }
                else
                {
								 list[num_item].mark=0;
								 list[num_item].name[0]=' ';
								}
               }
	       if (rows>1)   /* move down */
							 {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
								 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
								 else
                  movelightbox (light_box_col,light_box_row);
							 }   /* в любом случае перерисовываем */
               else
                movelightbox (light_box_col,light_box_row);
               continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
							 continue;

		 case 79 : row_box = full_rows-1;      /* end */
               col_box = (full_rows==rows) ? full_cols-1 : full_cols;
               if ((light_box_row!=row_box)||(light_box_col!=col_box))
								 movelightbox(col_box,row_box);
               continue;

     case 73 : if (PgUp_exist)             /* PgUp */
               {
                page_end_item = page_beg_item;
								page_beg_item = page_end_item-page_volume+1;
               }
							 else
                continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {                           /* No full page-move curs */
               if((items-page_end_item-1)<(light_box_col*rows+light_box_row))
									 movelightbox((items-page_end_item-1)/rows,(items-page_end_item-1)%rows);
								page_beg_item = page_end_item;
								page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
								light_item = page_beg_item;
							 }
							 else
								continue;
							 break;

		 case KB_N_F7	 :
										 prev_light_item = light_item ;
										 prev_page_beg_item = page_beg_item;
										 prev_light_box_col = light_box_col;
										 prev_light_box_row = light_box_row;
										 changedir();     /* изменение директории */
										 light_item = prev_light_item;
										 page_beg_item = prev_page_beg_item;
										 light_box_col = prev_light_box_col;
										 light_box_row = prev_light_box_row;
										 return_item=menuChDir;
										 goto MENU_EXIT;
																			/* удаление записи из файла */
		 case KB_N_DEL : return_item=-(light_item+1+100);
										 goto MENU_EXIT;  /* возвращает номер удаляемой записи */
																			/*увеличенный на 100 с обр. знаком */

																			/* запись помеч. записей в друг. файл */
		 case KB_A_C   : return_item=menuRecCopy;
										 goto MENU_EXIT;  /* возвращает номер удаляемой записи */
																			/*увеличенный на 100 с обр. знаком */

		 default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
								if ((hotkeys[i].key==key)&&(hotkeys[i].able))
								{
								 setviewport(0,0,getmaxx(),getmaxy(),1);
								 hotkeys[i].action ();
								 setviewport(page_left,page_top,page_left+page_xsize,
														 page_top+page_ysize,0);
								}
               continue;
    }
	 }
	 break;
  }
 }
 MENU_EXIT :  putimage (0,0,image_saved,COPY_PUT);	// восстанавливаем картинку
							farfree (image_saved);		// и параметры пред. окна
							setviewport(prev_viewport.left,prev_viewport.top,
											prev_viewport.right,prev_viewport.bottom,prev_viewport.clip);

 MENU_ABORT :
							prev_light_item = light_item ;
							prev_page_beg_item = page_beg_item;
							prev_light_box_col = light_box_col;
							prev_light_box_row = light_box_row;

							return return_item;
}

int grlist3 (char *menu_title,asList list[],int page_left,int page_top,
            int page_cols,int page_rows)
/***********                        Meny Anlz|Trend
* Describe :                        Display diagnose
* Params   : char *menu_title       - заголовок меню
*          : asList	 list[]          - список элементов
*          : int page_left          - лев. гpаница окна меню
*          : int page_top           - веpхняя гpаница окна меню
*          : int page_cols          - столбцов в окне меню
*          : int page_rows          - стpок в окне меню
* Return   : int                    - если > 0 то номеp выбpанного элемента
* Call     :                          иначе см. ниже коды ошибок
***********/
{
 static int prev_light_item,prev_page_beg_item,
						prev_light_box_col,prev_light_box_row;
 struct viewporttype prev_viewport;
 int key,i;
 int items,
		 curmode,
		 maxx,
		 maxy,
		 page_xsize,
		 page_ysize,
		 title_page_size,
		 prompt_page_size,
     draw_title,
     title_xsize,
		 Pg_box_xsize,
		 PgUp_exist,
     PgDn_exist,
     col_box,
     row_box,
     return_item,
     page_volume,
     full_rows,
		 full_cols,
		 items_on_page,
		 num_item;
 unsigned image_size;
 void far *image_saved;
 union REGS inr,outr;

 getviewsettings (&prev_viewport);		// запоминаем параметры пред.окна
 setviewport(0,0,getmaxx(),getmaxy(),1);	// уст. начало от полн.экрана

 inr.h.ah = 0x0f;
 int86 (0x10,&inr,&outr);
 curmode = outr.h.al;
 switch (curmode)
 {
  case 0x06 :                                              /* монохpомная    */
  case 0x0f :                                              /* гpафика        */
  case 0x11 :                                              /*                */
  case 0x30 :                                              /*                */
  case 0x40 : panel_color = 1;                             /*                */
              frame_color = 0;                             /*                */
              title_color = 0;                             /*                */
							item_color  = 0;                             /*                */
              light_item_color = 1;                        /*                */
              light_box_color = 0;                         /*                */
              break;                                       /*                */
                                                           /*                */
  case 0x04 : panel_color = CGA_CYAN;                      /* фиксиpованные  */
              frame_color = 0;                             /* палитpы        */
              title_color = 0;                             /*                */
							item_color  = CGA_LIGHTGRAY;                 /*                */
              light_item_color = CGA_MAGENTA;              /*                */
							light_box_color = 0;                         /*                */
							break;                                       /*                */
                                                           /*                */
  case 0x0e :                                              /* динамические   */
  case 0x10 :                                              /* палитpы        */
  case 0x12 : panel_color = CYAN;                          /*                */
              frame_color = BLUE;                          /*                */
              title_color = YELLOW;                        /*                */
              item_color  = BLACK;                         /*                */
              light_item_color = LIGHTGREEN;               /*                */
							light_box_color = RED;                       /*                */
              break;                                       /*                */
                                                           /*                */
  default   : return menuErr_InvGrMode;                    /* не поддеpж.    */
 }

 if ((page_rows<0)||(page_cols<=0))
  return menuErr_InvForm;

 for (items=0,maxlen=0;strcmp(list[items].name,"LAST");items++)
	maxlen = max((strlen(list[items].name)),(maxlen));

 if (prev_light_item<=items)
 {
  light_item = prev_light_item;
  page_beg_item = prev_page_beg_item;
  light_box_col = prev_light_box_col;
  light_box_row = prev_light_box_row;
 }
 else
 {
  light_item = 0;
  page_beg_item = 0;
  light_box_col = 0;
  light_box_row = 0;
 }

 listg = list;
 rows = page_rows;
 cols = page_cols;

 if (items==0)
  return menuErr_NoItems;

 maxx = getmaxx();
 maxy = getmaxy();

// settextstyle (SMALL_FONT,HORIZ_DIR,4);
 char_xsize =	(int) textwidth ("W");				// 6;
 char_ysize =	(int) textheight ("Ry");				// 8;
 info_xsize = 40*char_xsize;
 info_ysize = char_ysize*1.5;

 col_step = (maxlen+INTERCOL_FIELD)*char_xsize;
 row_step = (double) 3*char_ysize/2;
 page_xsize = max((cols*maxlen+2*SIDE_FIELD)*char_xsize+INTERCOL_FIELD*char_xsize*(cols-1),info_xsize+2*SIDE_FIELD*char_xsize);
 page_ysize = (rows*row_step+(TOP_FIELD+BOTTOM_FIELD*2)*char_ysize+info_ysize);

 if ((menu_title==NULL)||(*menu_title==0))
 {
	title_xsize = 0;
	draw_title = 0;
 }
 else
 {
  title_xsize = textwidth(menu_title)+char_xsize;
  draw_title = 1;
 }
 Pg_box_xsize = textwidth(PgUp_prompt);
 box_xsize = maxlen*char_xsize;
 box_ysize = char_ysize;
 x0_box = SIDE_FIELD*char_xsize;
 y0_box = TOP_FIELD*char_ysize;
 page_volume = rows*cols;
 page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;

 if ((draw_title)&&((title_page_size=title_xsize+2*char_xsize)>page_xsize))
 {
	x0_box+=(title_page_size-page_xsize)/2;
  page_xsize = title_page_size;
 }

 if ((prompt_page_size=5*Pg_box_xsize/2+2*char_xsize)>page_xsize)
 {
  x0_box+=(prompt_page_size-page_xsize)/2;
  page_xsize = prompt_page_size;
 }

 if ((page_left<0)||
     (page_top<0)||
		 (page_left+page_xsize>maxx)||
     (page_top+page_ysize>maxy))
 {
  return_item = menuErr_ScrExceed;
  goto MENU_ABORT;
 }

 if ((image_size=imagesize(page_left,page_top,
													 page_left+page_xsize,page_top+page_ysize))==0xFFFF)
 {
	return_item = menuErr_ImgSize;
	goto MENU_ABORT;
 }

 if ((image_saved=farmalloc(image_size))==NULL)
 {
  return_item = menuErr_NoImgMem;
  goto MENU_ABORT;
 }

 getimage (page_left,page_top,page_left+page_xsize,page_top+page_ysize,image_saved);

 setviewport(page_left,page_top,page_left+page_xsize,page_top+page_ysize,0);
 setlinestyle(SOLID_LINE,0,NORM_WIDTH);
 setfillstyle(SOLID_FILL,panel_color);
 bar(0,0,page_xsize,page_ysize);
 setcolor(frame_color);
 rectangle(char_xsize/2,char_ysize,page_xsize-char_xsize/2,page_ysize-char_ysize/2);
 rectangle(char_xsize/2+2,char_ysize+2,page_xsize-char_xsize/2-2,page_ysize-char_ysize/2-2);
 line(char_xsize/2+2,y0_box+rows*row_step+7,page_xsize-char_xsize/2-2,y0_box+rows*row_step+7);

 if (draw_title)
 {
  setfillstyle(SOLID_FILL,panel_color);
  bar((page_xsize-title_xsize)/2,char_ysize/2,(page_xsize+title_xsize)/2,3*char_ysize/2+char_ysize/2);
  setcolor(title_color);
  settextjustify(CENTER_TEXT,TOP_TEXT);
  outtextxy(page_xsize/2,char_ysize/2,menu_title);
 }

 while(1)
 {
  items_on_page = page_end_item-page_beg_item+1;
  full_cols=items_on_page/rows;
  full_rows=(items_on_page-1) % rows + 1;

  if (page_beg_item>0)
   PgUp_exist=1;
	else
   PgUp_exist=0;
	if (page_beg_item+page_volume<items)
	 PgDn_exist=1;
  else
   PgDn_exist=0;
  setcolor(frame_color);
  if (PgUp_exist)
  {
   setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
	 outtextxy(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize-2,PgUp_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2-Pg_box_xsize/4,page_ysize);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2-5*Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2-Pg_box_xsize/4,page_ysize-char_ysize/2-2);
	}
  if (PgDn_exist)
	{
	 setfillstyle(SOLID_FILL,panel_color);
   settextjustify(LEFT_TEXT,TOP_TEXT);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
   outtextxy(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize-2,PgDn_prompt);
  }
  else
  {
   setfillstyle(SOLID_FILL,panel_color);
   bar(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize,page_xsize/2+5*Pg_box_xsize/4,page_ysize);
	 line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2);
   line(page_xsize/2+Pg_box_xsize/4,page_ysize-char_ysize/2-2,page_xsize/2+5*Pg_box_xsize/4,page_ysize-char_ysize/2-2);
  }

  setfillstyle(SOLID_FILL,panel_color);
  setcolor(item_color);
  settextjustify(LEFT_TEXT,TOP_TEXT);

	for (col_box=0,x_box=x0_box,i=page_beg_item;col_box<cols;col_box++,x_box+=col_step)
   for (row_box=0,y_box=y0_box;row_box<rows;row_box++,y_box+=row_step,i++)
	 {
		bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
    if (strcmp(list[i].name,"LAST")&&(i<page_end_item+1))
     outtextxy(x_box+2,y_box,list[i].name);
   }

  movelightbox(light_box_col,light_box_row);

  while(1)
  {
	 if ((key=getch())!=0)
   {
    switch (key)
    {
     case KB_N_ENTER : return_item=light_item+1;
                       goto MENU_EXIT;

     case KB_N_ESC   : return_item=menuEscape;
											 goto MENU_EXIT;

		 default : continue;
		}
   }
   else
   {
    switch(key=getch())
    {
     case 75 : if (cols>1)        /* left */
               {
                if (light_box_col>0)
								 movelightbox(light_box_col-1,light_box_row);
                else
                 movelightbox(0,0);   /* верхний лев. угол */
               }
               else
                movelightbox(0,0);
               continue;

		 case 77 : if (cols>1)        /* right */
               {
								if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
								 if ((light_box_col<full_cols-1)||(light_box_row<full_rows))
                  movelightbox (light_box_col+1,light_box_row);
                 else
                  movelightbox (light_box_col+1,full_rows-1);
                else
                 movelightbox ((full_rows==rows)?full_cols-1:full_cols,full_rows-1);
               }
               else
                movelightbox(cols-1,full_rows-1); /* нижний правый угол */
							 continue;

     case 72 : if (rows>1)   /* up */
               {
                if (light_box_row>0)
                 movelightbox(light_box_col,light_box_row-1);
                else
                 if (light_box_col>0)
									movelightbox(light_box_col-1,rows-1);
               }
							 continue;

     case 80 : if (rows>1)   /* down */
               {
                if (light_box_row<((light_box_col<full_cols) ? rows-1 : full_rows-1))
                 movelightbox(light_box_col,light_box_row+1); /* в этой кол. */
                else
                 if (light_box_col<( (full_rows==rows) ? full_cols-1:full_cols))
                  movelightbox (light_box_col+1,0);
               }
							 continue;

     case 71 : if ((light_box_row!=0)||(light_box_col!=0)) /* home */
                 movelightbox(0,0);
               continue;

     case 79 : row_box = full_rows-1;      /* end */
               col_box = (full_rows==rows) ? full_cols-1 : full_cols;
							 if ((light_box_row!=row_box)||(light_box_col!=col_box))
                 movelightbox(col_box,row_box);
							 continue;

     case 73 : if (PgUp_exist)             /* PgUp */
               {
                page_end_item = page_beg_item;
                page_beg_item = page_end_item-page_volume+1;
               }
               else
                continue;
               break;

     case 81 : if (PgDn_exist)             /* PgDown */
               {                           /* No full page-move curs */
               if((items-page_end_item-1)<(light_box_col*rows+light_box_row))
                   movelightbox((items-page_end_item-1)/rows,(items-page_end_item-1)%rows);
                page_beg_item = page_end_item;
                page_end_item = page_beg_item+min(page_volume,items-page_beg_item)-1;
                light_item = page_beg_item;
							 }
               else
								continue;
							 break;

     default : for (i=0;hotkeys[i].key!=KB_LAST;i++)
                if ((hotkeys[i].key==key)&&(hotkeys[i].able))
                {
								 setviewport(0,0,getmaxx(),getmaxy(),1);
                 hotkeys[i].action ();
								 setviewport(page_left,page_top,page_left+page_xsize,
														 page_top+page_ysize,0);
								}
							 continue;
		}
	 }
	 break;
	}
 }
 MENU_EXIT :  putimage (0,0,image_saved,COPY_PUT);	// восстанавливаем картинку
							farfree (image_saved);		// и параметры пред. окна
							setviewport(prev_viewport.left,prev_viewport.top,
											prev_viewport.right,prev_viewport.bottom,prev_viewport.clip);

 MENU_ABORT :
							prev_light_item = light_item ;
							prev_page_beg_item = page_beg_item;
							prev_light_box_col = light_box_col;
							prev_light_box_row = light_box_row;

							return return_item;
}




static void movelightbox(int box_newcol,int box_newrow)
{
 drawitembox(light_box_col,light_box_row,item_color,panel_color);
 light_box_row = box_newrow;
 light_box_col = box_newcol;
 drawitembox(light_box_col,light_box_row,light_item_color,light_box_color);
 draw_about (light_box_col,light_box_row);
 light_item = page_beg_item+light_box_col*rows+light_box_row;
}

static void drawitembox(int drawbox_col,int drawbox_row,int text_color,int box_color)
{
 item_ptr = listg[page_beg_item+drawbox_col*rows+drawbox_row].name;
 x_box = x0_box+drawbox_col*col_step;
 y_box = y0_box+drawbox_row*row_step;
 setfillstyle(SOLID_FILL,box_color);
 bar(x_box,y_box,x_box+box_xsize+2,y_box+box_ysize+2);
 setcolor(text_color);
 outtextxy(x_box+2,y_box,item_ptr);
}

static void draw_about (int drawbox_col,int drawbox_row)
{
 item_ptr = listg[page_beg_item+drawbox_col*rows+drawbox_row].info;
 x_box = x0_box;
 y_box = y0_box+rows*row_step;
 setfillstyle(SOLID_FILL,1);
 bar(x_box,y_box+8,x_box+info_xsize,y_box+7+info_ysize);
 setcolor(0);
 outtextxy(x_box+2,y_box+9,item_ptr);
}

static void editname (int l_i)
/***********
* Describe : редактировать строку из списка
* Params   : int l_i - номер строки в списке
* Return   : void
* Call     : 
***********/
{
 setframe (col_step*1,row_step*2,
           col_step*2,row_step*6," Edit name: ");
 editstring(col_step*1.2,row_step*4,8,
            (char *)memchr(listg[l_i].name,'.',10)+2);
 killframe ();
}