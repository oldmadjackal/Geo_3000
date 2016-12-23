/********************************************************************/
/*								    */
/*          МОДУЛЬ ЗАДАЧИ "ПОСТОРЕНИЕ СГЛАЖЕННОЙ ТРАЕКТОРИИ"        */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "d:\_Projects\_Libraries\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Spline.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"

/*------------------------------------ Обработчики элементов диалога */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Tmp_WndProc ;

/*---------------------------------------------------- Прототипы п/п */


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Task_Spline_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                   WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Spline  Module ;
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
/* 	     Обработчик сообщений диалогового окна CONFIG            */

    BOOL CALLBACK  Task_Spline_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                     WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Spline *Module ;
                      int  elm ;         /* Идентификатор элемента диалога */
                      int  status ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Spline *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 SETi(IDC_TURN_ZONE, Module->mTurnZone_coef) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {

             Module->mTurnZone_coef=GETi(IDC_TURN_ZONE) ;

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
/* 	     Обработчик сообщений диалогового окна DRIVES            */

    BOOL CALLBACK  Task_Spline_Drives_dialog(  HWND hDlg,     UINT Msg, 
 		  	                     WPARAM wParam, LPARAM lParam) 
{
 static         RSS_Module_Spline *Module ;
 static RSS_Module_Spline_Context *Drives[20] ;  /* Список данных приводов */
 static                       int  Drives_cnt ;
 static                       int  Drive_idx ;   /* Индекс выбранного привода */
                              int  elm ;         /* Идентификатор элемента диалога */
 static                     HFONT  font ;        /* Шрифт */
                              int  status ;
                             char  text[1024] ;
                              int  i ;
                              int  j ;

#define   _FORMAT_HEAD     "%-15.15s %8.10s %8.10s  %s"
#define   _FORMAT_DATA     "%-15.15s %8.3lf %8.3lf  %s"

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Spline *)lParam ;
/*- - - - - - - - - - - - - - - - - - - Извлечение данных контекстов */
#define  C     Module->mDrives[i].contexts

    for(Drives_cnt=0, i=0 ; i<Module->mDrives_cnt ; i++) {          /* CIRCLE.1 - Перебираем степени подвижности */

       if(Module->mDrives[i].fixed)  continue ;                     /* Если фиксированная степень... */

      for(j=0 ; C[j]!=NULL ; j++)                                   /* Ищем контекст этого модуля */
        if(!strcmp(C[j]->name, Module->identification))  break ;

         Drives[Drives_cnt]=(RSS_Module_Spline_Context *)C[j]->data ;
                Drives_cnt++ ;
                                                         }
#undef  C
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
             if(font==NULL)
                font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
         SendMessage(ITEM(IDC_DRIVES_TITLE), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_DRIVES_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 sprintf(text, _FORMAT_HEAD, "Привод",
                                             "Vmax",
                                             "Amax",
                                             "Функция ошибки" ) ;
                      SETs(IDC_DRIVES_TITLE, text) ;

                  LB_CLEAR(IDC_DRIVES_LIST) ;
       for(i=0 ; i<Drives_cnt ; i++) {
                 sprintf(text, _FORMAT_DATA, Drives[i]->name,
                                             Drives[i]->v_max,
                                             Drives[i]->a_max,
                                             Drives[i]->f_err ) ;
              LB_ADD_ROW(IDC_DRIVES_LIST, text) ;
                                     }    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Выбор привода */
       if(elm==IDC_DRIVES_LIST)
        if(status==LBN_SELCHANGE) {

                 Drive_idx=LB_GET_ROW(IDC_DRIVES_LIST) ;

            sprintf(text, "%.3lf", Drives[Drive_idx]->v_max) ;
               SETs(IDC_V_MAX, text) ;
            sprintf(text, "%.3lf", Drives[Drive_idx]->a_max) ;
               SETs(IDC_A_MAX, text) ;
               SETs(IDC_F_ERR, Drives[Drive_idx]->f_err) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - Пропись параметров привода */
       if(elm==IDC_V_MAX ||
          elm==IDC_A_MAX   )
        if(status==EN_KILLFOCUS) {
                                     GETs(IDC_V_MAX, text) ;        /* Извлекаем данные */
            Drives[Drive_idx]->v_max=atof(text) ;
                                     GETs(IDC_A_MAX, text) ;
            Drives[Drive_idx]->a_max=atof(text) ;

              sprintf(text, _FORMAT_DATA, Drives[Drive_idx]->name,  /* Показываем данные в списке */
                                          Drives[Drive_idx]->v_max,
                                          Drives[Drive_idx]->a_max,
                                          Drives[Drive_idx]->f_err ) ;
           LB_DEL_ROW(IDC_DRIVES_LIST, Drive_idx) ;
           LB_INS_ROW(IDC_DRIVES_LIST, Drive_idx, text) ;
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - Пропись функции ошибки */
       if(elm==IDC_F_ERR)
        if(status==EN_KILLFOCUS) {

              GETsl(IDC_F_ERR, text,                                /* Извлекаем формулу */
                      sizeof(Drives[Drive_idx]->f_err)-1) ;

             status=Module->iErrorCalc(Drives[Drive_idx], text) ;   /* Компилируем */
          if(status==-1)  return(TRUE) ;

               strcpy(Drives[Drive_idx]->f_err, text) ;             /* Заносим формулу */

              sprintf(text, _FORMAT_DATA, Drives[Drive_idx]->name,  /* Показываем данные в списке */
                                          Drives[Drive_idx]->v_max,
                                          Drives[Drive_idx]->a_max,
                                          Drives[Drive_idx]->f_err ) ;
           LB_DEL_ROW(IDC_DRIVES_LIST, Drive_idx) ;
           LB_INS_ROW(IDC_DRIVES_LIST, Drive_idx, text) ;
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


