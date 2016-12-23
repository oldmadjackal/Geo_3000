/********************************************************************/
/*								    */
/*                  МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ        		    */
/*                   "ЛЕТАЮЩАЯ" СТРУКТУРА ТЕЛ                       */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>

#include "d:\_Projects\_Libraries\controls.h"
#include "d:\_Projects\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_Flyer.h"

#pragma warning(disable : 4996)

/*---------------------------------------------- Специальные функции */

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#define  CREATE_BITMAP(hDC, bmp)   CreateDIBitmap(hDC,                                \
                                                  bmp,                                \
                                                  CBM_INIT,                           \
                                           (LPSTR)bmp+bmp->biSize+16*sizeof(RGBQUAD), \
                                    (BITMAPINFO *)bmp,                                \
                                                  DIB_RGB_COLORS)

/*---------------------------------------------- Специальные сигналы */

#define  _USER_SCHEME_CHANGE  1

/*-------------------------------------------- Обработчики элементов */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  BodyList_WndProc ;
  static union WndProc_par  MassList_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

  LRESULT CALLBACK  OF_BodyList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) ;
  LRESULT CALLBACK  OF_MassList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) ;

/*-------------------------------------------------------------------*/

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Object_Flyer_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                    WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Flyer  Module ;
               int  elm ;         /* Идентификатор элемента диалога */
               int  status ;
               int  index ;
               int  insert_flag ;
              char *help ;
              char  text[512] ;
              char *end ;
               int  i ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Раскрытие справки */
       if(elm==IDC_HELP_LIST) {

        if(status==LBN_DBLCLK) {

             index=        LB_GET_ROW (elm) ;
              help=(char *)LB_GET_ITEM(elm) ;
                           LB_GET_TEXT(elm, index, text) ;
           if(text[0]!='#' || 
              help   ==NULL  )  return(FALSE) ;

               status=LB_GET_TEXT(elm, index+1, text) ;
           if( status==LB_ERR ||
              text[0]==  '#'    )  insert_flag=1 ;
           else                    insert_flag=0 ;

         do {
                   memset(text, 0, sizeof(text)) ;
                  strncpy(text, help, sizeof(text)-1) ;
               end=strchr(text, '\n') ;
           if( end!=NULL)  *end=0 ;

           if(insert_flag) {  LB_INS_ROW(elm, index+1, text) ;
                                              index++ ;        }
              help=strchr(help, '\n') ;
           if(help==NULL)  break ;
              help++ ;
              
            } while(1) ;
                               }

			            return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна DEGREE            */

    BOOL CALLBACK  Object_Flyer_Degree_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Flyer *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
         static    HFONT  font_a ;           /* Шрифт "активных" степеней */
         static    HFONT  font_p ;           /* Шрифт "пассивных" степеней */
                    char  dgt[512] ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Flyer *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Подготовка шрифтов */
       if(font_p==NULL) {

              font_p=CreateFont(16, 8, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier") ;

              font_a=CreateFont(16, 8, 0, 0, FW_SEMIBOLD,
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier") ;
                        }
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
               sprintf(dgt, "%.3lf", object->link_x.value) ;
               SETFONT(IDC_X, font_a) ;
                  SETs(IDC_X, dgt) ;
        
               sprintf(dgt, "%.3lf", object->link_y.value) ;
               SETFONT(IDC_Y, font_a) ;
                  SETs(IDC_Y, dgt) ;

               sprintf(dgt, "%.3lf", object->link_z.value) ;
               SETFONT(IDC_Z, font_a) ;
                  SETs(IDC_Z, dgt) ;

               sprintf(dgt, "%.3lf", object->link_azim.value) ;
               SETFONT(IDC_AZIM, font_a) ;
                  SETs(IDC_AZIM, dgt) ;

               sprintf(dgt, "%.3lf", object->link_elev.value) ;
               SETFONT(IDC_ELEV, font_a) ;
                  SETs(IDC_ELEV, dgt) ;

               sprintf(dgt, "%.3lf", object->link_roll.value) ;
               SETFONT(IDC_ROLL, font_a) ;
                  SETs(IDC_ROLL, dgt) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                         status=HIWORD(wParam) ;
	                    elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Снятие значений */
    if(status==EN_KILLFOCUS) {
                                    GETs(IDC_X, dgt) ;
            object->link_x   .value=atof(dgt) ;
                                    GETs(IDC_Y, dgt) ;
            object->link_y   .value=atof(dgt) ;
                                    GETs(IDC_Z, dgt) ;
            object->link_z   .value=atof(dgt) ;
                                    GETs(IDC_AZIM, dgt) ;
            object->link_azim.value=atof(dgt) ;
                                    GETs(IDC_ELEV, dgt) ;
            object->link_elev.value=atof(dgt) ;
                                    GETs(IDC_ROLL, dgt) ;
            object->link_roll.value=atof(dgt) ;

                             }  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {

                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна RANGE             */

    BOOL CALLBACK  Object_Flyer_Range_dialog(  HWND hDlg,     UINT Msg, 
 		  	                     WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Flyer *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
                    char  dgt[512] ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Flyer *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
               sprintf(dgt, "%.3lf", object->link_x.value_min) ;
                  SETs(IDC_X_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_x.value_max) ;
                  SETs(IDC_X_MAX, dgt) ;
                  SETx(IDC_X_TEST, object->link_x.value_test) ;
        
               sprintf(dgt, "%.3lf", object->link_y.value_min) ;
                  SETs(IDC_Y_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_y.value_max) ;
                  SETs(IDC_Y_MAX, dgt) ;
                  SETx(IDC_Y_TEST, object->link_y.value_test) ;

               sprintf(dgt, "%.3lf", object->link_z.value_min) ;
                  SETs(IDC_Z_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_z.value_max) ;
                  SETs(IDC_Z_MAX, dgt) ;
                  SETx(IDC_Z_TEST, object->link_z.value_test) ;

               sprintf(dgt, "%.3lf", object->link_azim.value_min) ;
                  SETs(IDC_AZIM_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_azim.value_max) ;
                  SETs(IDC_AZIM_MAX, dgt) ;
                  SETx(IDC_AZIM_TEST, object->link_azim.value_test) ;

               sprintf(dgt, "%.3lf", object->link_elev.value_min) ;
                  SETs(IDC_ELEV_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_elev.value_max) ;
                  SETs(IDC_ELEV_MAX, dgt) ;
                  SETx(IDC_ELEV_TEST, object->link_elev.value_test) ;

               sprintf(dgt, "%.3lf", object->link_roll.value_min) ;
                  SETs(IDC_ROLL_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_roll.value_max) ;
                  SETs(IDC_ROLL_MAX, dgt) ;
                  SETx(IDC_ROLL_TEST, object->link_roll.value_test) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                         status=HIWORD(wParam) ;
	                    elm=LOWORD(wParam) ;

			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Снятие значений */
                                    GETs(IDC_X_MIN, dgt) ;
          object->link_x.value_min =atof(dgt) ;
                                    GETs(IDC_X_MAX, dgt) ;
          object->link_x.value_max =atof(dgt) ;
          object->link_x.value_test=GETx(IDC_X_TEST) ;

                                    GETs(IDC_Y_MIN, dgt) ;
          object->link_y.value_min =atof(dgt) ;
                                    GETs(IDC_Y_MAX, dgt) ;
          object->link_y.value_max =atof(dgt) ;
          object->link_y.value_test=GETx(IDC_Y_TEST) ;

                                    GETs(IDC_Z_MIN, dgt) ;
          object->link_z.value_min =atof(dgt) ;
                                    GETs(IDC_Z_MAX, dgt) ;
          object->link_z.value_max =atof(dgt) ;
          object->link_z.value_test=GETx(IDC_Z_TEST) ;

                                       GETs(IDC_AZIM_MIN, dgt) ;
          object->link_azim.value_min =atof(dgt) ;
                                       GETs(IDC_AZIM_MAX, dgt) ;
          object->link_azim.value_max =atof(dgt) ;
          object->link_azim.value_test=GETx(IDC_AZIM_TEST) ;

                                       GETs(IDC_ELEV_MIN, dgt) ;
          object->link_elev.value_min =atof(dgt) ;
                                       GETs(IDC_ELEV_MAX, dgt) ;
          object->link_elev.value_max =atof(dgt) ;
          object->link_elev.value_test=GETx(IDC_ELEV_TEST) ;

                                       GETs(IDC_ROLL_MIN, dgt) ;
          object->link_roll.value_min =atof(dgt) ;
                                       GETs(IDC_ROLL_MAX, dgt) ;
          object->link_roll.value_max =atof(dgt) ;
          object->link_roll.value_test=GETx(IDC_ROLL_TEST) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

#undef   L

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна BODIES            */

    BOOL CALLBACK  Object_Flyer_Bodies_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
              RSS_Kernel *Module ;
 static RSS_Object_Flyer *object ;
   RSS_Object_Flyer_Link *link ;
                     int  elm ;               /* Идентификатор элемента диалога */
                     int  status ;
                    char  desc[1024] ;
                     int  j ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Flyer *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
     for(elm=IDC_BASE_PARS ; elm<=IDC_HEAD_PARS ; elm++) {

            if(elm==IDC_BASE_PARS)  link=&object->link_base ;
       else if(elm==IDC_HEAD_PARS)  link=&object->link_head ;

                                 *desc=0 ;
            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->bodies[j].use_flag) {
                          strncat(desc, link->bodies[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

                       SETs(elm, desc) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - Вызов диалога привязки тел */
        if(elm>=IDC_ASSIGN_BASE &&
           elm<=IDC_ASSIGN_HEAD   ) {
                      
              if(elm==IDC_ASSIGN_BASE)  link=&object->link_base ;
         else if(elm==IDC_ASSIGN_HEAD)  link=&object->link_head ;

                Module=GetObjectFlyerEntry() ;

                    DialogBoxIndirectParam( GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Module->Resource("IDD_BODY_ASSIGN", RT_DIALOG),
	            	                    GetActiveWindow(),
                                            Object_Flyer_BodyAssign_dialog,
                                    (LPARAM)link                            ) ;

                           memset(desc, 0, sizeof(desc)) ;

            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->bodies[j].use_flag) {
                          strncat(desc, link->bodies[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

              if(elm==IDC_ASSIGN_BASE)  SETs(IDC_BASE_PARS, desc) ;
         else if(elm==IDC_ASSIGN_HEAD)  SETs(IDC_HEAD_PARS, desc) ;

                			  return(FALSE) ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*        Обработчик сообщений диалогового окна BODIES_ASSIGN        */

    BOOL CALLBACK  Object_Flyer_BodyAssign_dialog(  HWND hDlg,     UINT Msg, 
 		  	                          WPARAM wParam, LPARAM lParam) 
{
 static                  RECT  PictureFrame ;
                      HBITMAP  hBitmap ;
 static RSS_Object_Flyer_Link *link ;
        RSS_Object_Flyer_Body *body ;
 static RSS_Object_Flyer_Body *body_crn ;
               RSS_Morphology  morpho ;
 static                  char  library[1024] ;
 static                  char  library_prv[1024] ;
 static                  char  models_list[4096] ;
                          int  elm ;               /* Идентификатор элемента диалога */
                          int  status ;
                          int  n ;
                          int  index ;
                         char  text[1024] ;              
                         char  tmp[1024] ;
                         char *end ;
                          int  i ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

               link=(RSS_Object_Flyer_Link *)lParam ;

              GetWindowRect(ITEM(IDC_PICTURE), &PictureFrame) ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
      BodyList_WndProc.par=GetWindowLong(ITEM(IDC_BODY_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=OF_BodyList_WndProc ;
          SetWindowLong(ITEM(IDC_BODY_LIST),
                         GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Пропись заголовка */
              sprintf(text, "%s - привязка тел", link->name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)text) ;
/*- - - - - - - - - - - - - - - - - - - - - Инициализация списка тел */
       for(i=0 ; i<_BODIES_MAX ; i++)
         if(link->bodies[i].use_flag) {
             index=LB_ADD_ROW (IDC_BODY_LIST, link->bodies[i].desc) ;
                   LB_SET_ITEM(IDC_BODY_LIST, index, &link->bodies[i]) ;
                                      }            
/*- - - - - - - - - - - - -  Инициализация списка модулей библиотеки */
       sprintf(library, ".\\Links.lib") ;
        strcpy(library_prv, library) ;
          SETs(IDC_LIBRARY_PATH, library) ;

           RSS_Model_list(library, models_list,
                            sizeof(models_list)-1, "BODY,LINK" ) ;

                                          CB_CLEAR(IDC_MODEL) ;
     for(end=models_list ; *end ; ) {
                                      CB_ADD_LIST(IDC_MODEL, end) ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                    }
/*- - - - - - - - - - - - - - - - - -  Установка невидимых элементов */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         body_crn=NULL ;

  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;

          body=(RSS_Object_Flyer_Body *)LB_GET_ITEM(IDC_BODY_LIST) ;
/*- - - - - - - - - - - - - - - - - - - - -  Извлечение свойств тела */
      if(  status==EN_KILLFOCUS &&
         body_crn!=NULL           ) {
                                      GETs(IDC_DESCRIBE, body_crn->desc) ;
                                      GETs(IDC_X_BASE, tmp) ;
                     body_crn->x_base=atof(tmp) ;
                                      GETs(IDC_Y_BASE, tmp) ;
                     body_crn->y_base=atof(tmp) ;
                                      GETs(IDC_Z_BASE, tmp) ;
                     body_crn->z_base=atof(tmp) ;
                                      GETs(IDC_AZIM, tmp) ;
                     body_crn->a_azim=atof(tmp) ;
                                      GETs(IDC_ELEV, tmp) ;
                     body_crn->a_elev=atof(tmp) ;
                                      GETs(IDC_ROLL, tmp) ;
                     body_crn->a_roll=atof(tmp) ;
                                      GETs(IDC_BABY_LEN, tmp) ;                
                  body_crn->zero_size=atof(tmp) ;

#define  M    body_crn->model

               for(i=0 ; i<5 ; i++)  GETs(IDC_PAR_VALUE_1+i, M.pars[i].value) ;
#undef   M

                                    }
/*- - - - - - - - - - - - - - - - - - - - -  Изменение названия тела */
      if(   elm==IDC_DESCRIBE &&
         status==EN_KILLFOCUS   ) {

        if(body_crn!=NULL) {
           
          for(i=0 ; i<_BODIES_MAX ; i++)
            if(LB_ROW_ITEM(IDC_BODY_LIST, i)==(LPARAM)body_crn) 
                LB_DEL_ROW(IDC_BODY_LIST, i) ;
                                        
          index=LB_ADD_ROW (IDC_BODY_LIST, body_crn->desc) ;
                LB_SET_ITEM(IDC_BODY_LIST, index, body_crn) ;

                                         morpho.object= NULL ;      /* Исправление морфологического элемента */   
                                         morpho.link  = NULL ;
                                         morpho.body  =body_crn->desc ;
                                         morpho.ptr   =body_crn ;
           link->parent->vEditMorphology(&morpho) ;
                           }
                                        return(FALSE) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - Добавление нового тела */
     if(elm==IDC_BODY_ADD ||
        elm==IDC_BODY_COPY  ) {

          for(n=0 ; n<_BODIES_MAX ; n++)                            /* Ищем свободный слот */
            if(link->bodies[n].use_flag==0)  break ;

            if(n==_BODIES_MAX) {
                                 SEND_ERROR("Переполнение списка тел") ;
                                    return(FALSE) ;
                               }

       if(elm==IDC_BODY_ADD) {
                    memset(&link->bodies[n], 0, sizeof(link->bodies[0])) ;
                    strcpy( link->bodies[n].desc, "-- Новое тело --") ;
                             }
       else                  {
                                          if(body==NULL)  return(FALSE) ;
                    memcpy(&link->bodies[n], body, sizeof(link->bodies[0])) ;
                             }

                            link->bodies[n].use_flag=1 ;

                                         morpho.link= link->name ;  /* Добавление морфологического элемента */   
                                         morpho.body="body" ;
                                         morpho.ptr =&link->bodies[n] ;
           link->parent->vAddMorphology(&morpho) ;

            index=LB_ADD_ROW (IDC_BODY_LIST, link->bodies[n].desc) ;
                  LB_SET_ITEM(IDC_BODY_LIST, index, &link->bodies[n]) ;

                                    return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - Показать свойства тела */
     if(elm==IDC_BODY_LIST) {

      if(status==LBN_SELCHANGE) {

                        if(body==NULL)  return(FALSE) ;

                                           body_crn=body ;

                                           body_crn->change=1 ;

                        SETs(IDC_DESCRIBE, body_crn->desc) ;
                     sprintf(tmp, "%.3lf", body_crn->x_base) ;
                        SETs(IDC_X_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->y_base) ;
                        SETs(IDC_Y_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->z_base) ;
                        SETs(IDC_Z_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->a_azim) ;
                        SETs(IDC_AZIM, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->a_elev) ;
                        SETs(IDC_ELEV, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->a_roll) ;
                        SETs(IDC_ROLL, tmp) ;
                     sprintf(tmp, "%.3lf", body_crn->zero_size) ;
                        SETs(IDC_BABY_LEN, tmp) ;                

#define  M    body_crn->model

                          if(M.lib_path[0]==0)  strcpy(M.lib_path, ".\\Links.lib") ;

                                     SETs(IDC_LIBRARY_PATH, M.lib_path) ;
                                     SETc(IDC_MODEL,        M.model) ;

        for(i=0 ; i<5 ; i++) {                                      /* Выдаем список параметров модели */
                                     SETs(IDC_PAR_NAME_1 +i, M.pars[i].text) ;
                                     SETs(IDC_PAR_VALUE_1+i, M.pars[i].value) ;
         if(M.pars[i].text[0]!=0)  ENABLE(IDC_PAR_VALUE_1+i) ;
         else                     DISABLE(IDC_PAR_VALUE_1+i) ;
                             }

            status=RSS_Model_Picture(&M, &hBitmap, &PictureFrame) ;
//       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap) ;

#undef   M
                                }

			            return(FALSE) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Удаление тела */
     if(elm==IDC_BODY_KILL) {

                  if(body==NULL)  return(FALSE) ;

               LB_DEL_ROW(IDC_BODY_LIST, LB_GET_ROW(IDC_BODY_LIST)) ;

                                body->use_flag=0 ;

                                    return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Смена библиотеки */
     if(elm==IDC_LIBRARY_PATH) {

      if(status!=EN_UPDATE)  return(FALSE) ;

              GETs(IDC_LIBRARY_PATH, library) ;

       if(!strcmp(library_prv, library))  break ;                   /* Если библиотека не менялась */

           RSS_Model_list(library, models_list,
                            sizeof(models_list)-1, "BODY,LINK") ;

                                          CB_CLEAR(IDC_MODEL) ;
      for(end=models_list ; *end ; ) {
                                       CB_ADD_LIST(IDC_MODEL, end) ;
                                          end+=strlen(end)+1 ;
                                          end+=strlen(end)+1 ;
                                     }

                                 strcpy(library_prv, library) ;           
                                      return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Выбор модели */
     if(elm==IDC_MODEL) {

                              if(status!=CBN_SELCHANGE)  return(FALSE) ;

                              if(body_crn==NULL)  return(FALSE) ;

#define  M    body_crn->model

                         GETc(IDC_MODEL,        M.model) ;          /* Извлекаем название модели */
                         GETs(IDC_LIBRARY_PATH, M.lib_path) ;       /* Извлекаем раздел библиотеки */

        for(end=models_list ; *end ; ) {                            /* Ищем модель по списку */
                        if(!stricmp(M.model, end))  break ;
                                         end+=strlen(end)+1 ;
                                         end+=strlen(end)+1 ;
                                       }

                                    end+=strlen(end)+1 ;            /* Извлекаем имя файла */

                      sprintf( M.path, "%s\\%s", M.lib_path, end) ;
           RSS_Model_ReadPars(&M) ;                                 /* Считываем параметры модели */

        for(i=0 ; i<5 ; i++) {                                      /* Выдаем список параметров модели */
                                     SETs(IDC_PAR_NAME_1+i, M.pars[i].text) ;
         if(M.pars[i].text[0]!=0)  ENABLE(IDC_PAR_VALUE_1+i) ;
         else                     DISABLE(IDC_PAR_VALUE_1+i) ;
                             }

            status=RSS_Model_Picture(&M, &hBitmap, &PictureFrame) ;
//       if(status)  break ;

               SendMessage(ITEM(IDC_PICTURE), STM_SETIMAGE,
                               (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap) ;

#undef  M
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна MASSES            */

    BOOL CALLBACK  Object_Flyer_Masses_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
              RSS_Kernel *Module ;
 static RSS_Object_Flyer *object ;
   RSS_Object_Flyer_Link *link ;
                     int  elm ;               /* Идентификатор элемента диалога */
                     int  status ;
                    char  desc[1024] ;
                     int  j ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Flyer *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
     for(elm=IDC_BASE_PARS ; elm<=IDC_HEAD_PARS ; elm++) {

            if(elm==IDC_BASE_PARS)  link=&object->link_base ;
       else if(elm==IDC_HEAD_PARS)  link=&object->link_head ;

                                 *desc=0 ;
            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->masses[j].use_flag) {
                          strncat(desc, link->masses[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

                       SETs(elm, desc) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - Вызов диалога привязки тел */
        if(elm>=IDC_ASSIGN_BASE &&
           elm<=IDC_ASSIGN_HEAD   ) {
                      
              if(elm==IDC_ASSIGN_BASE)  link=&object->link_base ;
         else if(elm==IDC_ASSIGN_HEAD)  link=&object->link_head ;

                Module=GetObjectFlyerEntry() ;

                    DialogBoxIndirectParam( GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Module->Resource("IDD_MASS_ASSIGN", RT_DIALOG),
	            	                    GetActiveWindow(),
                                            Object_Flyer_MassAssign_dialog,
                                    (LPARAM)link                            ) ;

                           memset(desc, 0, sizeof(desc)) ;

            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->masses[j].use_flag) {
                          strncat(desc, link->masses[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

              if(elm==IDC_ASSIGN_BASE)  SETs(IDC_BASE_PARS, desc) ;
         else if(elm==IDC_ASSIGN_HEAD)  SETs(IDC_HEAD_PARS, desc) ;

                			  return(FALSE) ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*        Обработчик сообщений диалогового окна MASSES_ASSIGN        */

    BOOL CALLBACK  Object_Flyer_MassAssign_dialog(  HWND hDlg,     UINT Msg, 
 		  	                          WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Flyer_Link *link ;
        RSS_Object_Flyer_Mass *mass ;
 static RSS_Object_Flyer_Mass *mass_crn ;
                          int  elm ;               /* Идентификатор элемента диалога */
                          int  status ;
                          int  n ;
                          int  index ;
                         char  text[1024] ;              
                         char  tmp[1024] ;
                          int  i ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

               link=(RSS_Object_Flyer_Link *)lParam ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
      MassList_WndProc.par=GetWindowLong(ITEM(IDC_MASS_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=OF_MassList_WndProc ;
          SetWindowLong(ITEM(IDC_MASS_LIST),
                         GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Пропись заголовка */
              sprintf(text, "%s - привязка масс", link->name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)text) ;
/*- - - - - - - - - - - - - - - - - - - - - Инициализация списка тел */
       for(i=0 ; i<_BODIES_MAX ; i++)
         if(link->masses[i].use_flag) {
             index=LB_ADD_ROW (IDC_MASS_LIST, link->masses[i].desc) ;
                   LB_SET_ITEM(IDC_MASS_LIST, index, &link->masses[i]) ;
                                      }            
/*- - - - - - - - - - - - - - - - - -  Установка невидимых элементов */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         mass_crn=NULL ;

  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;

          mass=(RSS_Object_Flyer_Mass *)LB_GET_ITEM(IDC_MASS_LIST) ;
/*- - - - - - - - - - - - - - - - - - - - - Извлечение свойств массы */
      if(  status==EN_KILLFOCUS &&
         mass_crn!=NULL           ) {
                                      GETs(IDC_DESCRIBE, mass_crn->desc) ;

                                      GETs(IDC_MASS, tmp) ;
                       mass_crn->mass=atof(tmp) ;
                                      GETs(IDC_J_X, tmp) ;
                       mass_crn->Jx  =atof(tmp) ;
                                      GETs(IDC_J_Y, tmp) ;
                       mass_crn->Jy  =atof(tmp) ;
                                      GETs(IDC_J_Z, tmp) ;
                       mass_crn->Jz  =atof(tmp) ;

                                      GETs(IDC_X_BASE, tmp) ;
                     mass_crn->x_base=atof(tmp) ;
                                      GETs(IDC_Y_BASE, tmp) ;
                     mass_crn->y_base=atof(tmp) ;
                                      GETs(IDC_Z_BASE, tmp) ;
                     mass_crn->z_base=atof(tmp) ;
                                      GETs(IDC_AZIM, tmp) ;
                     mass_crn->a_azim=atof(tmp) ;
                                      GETs(IDC_ELEV, tmp) ;
                     mass_crn->a_elev=atof(tmp) ;
                                      GETs(IDC_ROLL, tmp) ;
                     mass_crn->a_roll=atof(tmp) ;
                                      GETs(IDC_MOVE_COEF, tmp) ;                
                  mass_crn->move_coef=atof(tmp) ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - Изменение названия массы */
      if(   elm==IDC_DESCRIBE &&
         status==EN_KILLFOCUS   ) {

        if(mass_crn!=NULL) {
           
          for(i=0 ; i<_BODIES_MAX ; i++)
            if(LB_ROW_ITEM(IDC_MASS_LIST, i)==(LPARAM)mass_crn) 
                LB_DEL_ROW(IDC_MASS_LIST, i) ;
                                        
          index=LB_ADD_ROW (IDC_MASS_LIST, mass_crn->desc) ;
                LB_SET_ITEM(IDC_MASS_LIST, index, mass_crn) ;
                           }
                                        return(FALSE) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - Добавление новой массы */
     if(elm==IDC_BODY_ADD ||
        elm==IDC_BODY_COPY  ) {

          for(n=0 ; n<_BODIES_MAX ; n++)                            /* Ищем свободный слот */
            if(link->masses[n].use_flag==0)  break ;

            if(n==_BODIES_MAX) {
                                 SEND_ERROR("Переполнение списка масс") ;
                                    return(FALSE) ;
                               }

       if(elm==IDC_BODY_ADD) {
                    memset(&link->masses[n], 0, sizeof(link->masses[0])) ;
                    strcpy( link->masses[n].desc, "-- Новая масса --") ;
                             }
       else                  {
                                          if(mass==NULL)  return(FALSE) ;
                    memcpy(&link->masses[n], mass, sizeof(link->masses[0])) ;
                             }

                            link->masses[n].use_flag=1 ;

            index=LB_ADD_ROW (IDC_MASS_LIST, link->masses[n].desc) ;
                  LB_SET_ITEM(IDC_MASS_LIST, index, &link->masses[n]) ;

                                    return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - -  Показать свойства массы */
     if(elm==IDC_MASS_LIST) {

      if(status==LBN_SELCHANGE) {

                        if(mass==NULL)  return(FALSE) ;

                                           mass_crn=mass ;

                                           mass_crn->change=1 ;

                        SETs(IDC_DESCRIBE, mass_crn->desc) ;

                     sprintf(tmp, "%.3lf", mass_crn->mass) ;
                        SETs(IDC_MASS, tmp) ;
                     sprintf(tmp, "%.6lf", mass_crn->Jx) ;
                        SETs(IDC_J_X, tmp) ;
                     sprintf(tmp, "%.6lf", mass_crn->Jy) ;
                        SETs(IDC_J_Y, tmp) ;
                     sprintf(tmp, "%.6lf", mass_crn->Jz) ;
                        SETs(IDC_J_Z, tmp) ;

                     sprintf(tmp, "%.3lf", mass_crn->x_base) ;
                        SETs(IDC_X_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->y_base) ;
                        SETs(IDC_Y_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->z_base) ;
                        SETs(IDC_Z_BASE, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->a_azim) ;
                        SETs(IDC_AZIM, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->a_elev) ;
                        SETs(IDC_ELEV, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->a_roll) ;
                        SETs(IDC_ROLL, tmp) ;
                     sprintf(tmp, "%.3lf", mass_crn->move_coef) ;
                        SETs(IDC_MOVE_COEF, tmp) ;                
                                }

			            return(FALSE) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Удаление тела */
     if(elm==IDC_BODY_KILL) {

                  if(mass==NULL)  return(FALSE) ;

               LB_DEL_ROW(IDC_MASS_LIST, LB_GET_ROW(IDC_MASS_LIST)) ;

                                mass->use_flag=0 ;

                                    return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна KEYS              */

    BOOL CALLBACK  Object_Flyer_Keys_dialog(  HWND hDlg,     UINT Msg, 
 		  	                    WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Flyer *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
                    char  dgt[512] ;
                     int  elem_list[32] ;
                     int  elem_cnt ;
                     int  i ;
                     int  j ;
            static  char *keys_text[]={ "-- Без клавиш --",
                                        "Left,Right",
                                        "Up,Down",
                                        "Ctrl+Left,Right",
                                        "Ctrl+Up,Down",
                                        "Sh+Left,Right",
                                        "Sh+Up,Down",
                                        "Sh+Ctrl+Left,Right",
                                        "Sh+Ctrl+Up,Down",
                                          NULL                 } ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Flyer *)lParam ;
/*- - - - - - - - - - - - - - - - - - - -  Прогрузка списка значений */
                         elem_cnt=0 ;
               elem_list[elem_cnt]=IDC_X ;     elem_cnt++ ;
               elem_list[elem_cnt]=IDC_Y ;     elem_cnt++ ;
               elem_list[elem_cnt]=IDC_Z ;     elem_cnt++ ;
               elem_list[elem_cnt]=IDC_AZIM ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_ELEV ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_ROLL ;  elem_cnt++ ; 

      for(i=0 ; i<elem_cnt ; i++) {
                    CB_CLEAR(elem_list[i]) ;
                 CB_ADD_LIST(elem_list[i], "-- Без клавиш --") ;
                 CB_ADD_LIST(elem_list[i], "Left,Right") ;
                 CB_ADD_LIST(elem_list[i], "Up,Down") ;
                 CB_ADD_LIST(elem_list[i], "Ctrl+Left,Right") ;
                 CB_ADD_LIST(elem_list[i], "Ctrl+Up,Down") ;
                 CB_ADD_LIST(elem_list[i], "Sh+Left,Right") ;
                 CB_ADD_LIST(elem_list[i], "Sh+Up,Down") ;
                 CB_ADD_LIST(elem_list[i], "Sh+Ctrl+Left,Right") ;
                 CB_ADD_LIST(elem_list[i], "Sh+Ctrl+Up,Down") ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                  SETc(IDC_X,    keys_text[object->link_x.value_keys]);
                  SETc(IDC_Y,    keys_text[object->link_y.value_keys]);
                  SETc(IDC_Z,    keys_text[object->link_z.value_keys]);

                  SETc(IDC_AZIM, keys_text[object->link_azim.value_keys]);
                  SETc(IDC_ELEV, keys_text[object->link_elev.value_keys]);
                  SETc(IDC_ROLL, keys_text[object->link_roll.value_keys]);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                         status=HIWORD(wParam) ;
	                    elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - Установка по умолчанию */
     if(elm==IDC_DEFAULT) {

                  SETc(IDC_X,    keys_text[1]) ;
                  SETc(IDC_Y,    keys_text[2]) ;
                  SETc(IDC_Y,    keys_text[4]) ;
        
                  SETc(IDC_AZIM, keys_text[5]) ;
                  SETc(IDC_ELEV, keys_text[6]) ;
                  SETc(IDC_ROLL, keys_text[8]) ;

                              return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                              GETc(IDC_X, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_x.value_keys=j ;

                              GETc(IDC_Y, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_y.value_keys=j ;

                              GETc(IDC_Z, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_z.value_keys=j ;

                              GETc(IDC_AZIM, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_azim.value_keys=j ;

                              GETc(IDC_ELEV, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_elev.value_keys=j ;

                              GETc(IDC_ROLL, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_roll.value_keys=j ;

                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- Прочее */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*	          Новыe обработчики элементов ListBox                */    

  LRESULT CALLBACK  OF_BodyList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
    HMENU  hMenu ;
  LRESULT  result ;
      int  menu_flag ;
     RECT  Rect ;
      int  x ;
      int  y ;

/*----------------------------------- Подмена правой кнопки на левую */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- Вызов штатной процедуры обработки */

      result=CallWindowProc(BodyList_WndProc.call,
                               hWnd, Msg, wParam, lParam) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_ADD,   "Добавить тело"  ) ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_COPY,  "Копировать тело") ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_KILL,  "Удалить тело"   ) ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

                    DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


  LRESULT CALLBACK  OF_MassList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
    HMENU  hMenu ;
  LRESULT  result ;
      int  menu_flag ;
     RECT  Rect ;
      int  x ;
      int  y ;

/*----------------------------------- Подмена правой кнопки на левую */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- Вызов штатной процедуры обработки */

      result=CallWindowProc(MassList_WndProc.call,
                               hWnd, Msg, wParam, lParam) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_ADD,   "Добавить массу"  ) ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_COPY,  "Копировать массу") ;
          AppendMenu(hMenu, MF_STRING, IDC_BODY_KILL,  "Удалить массу"   ) ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

                    DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


