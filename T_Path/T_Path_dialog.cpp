/********************************************************************/
/*								    */
/*             МОДУЛЬ ЗАДАЧИ "ПОИСК БАЗОВОЙ ТРАЕКТОРИИ" 	    */
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

#include "T_Path.h"

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

    BOOL CALLBACK  Task_Path_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
   RSS_Module_Path  Module ;
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

    BOOL CALLBACK  Task_Path_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                   WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Path *Module ;
                    int  elm ;         /* Идентификатор элемента диалога */
                    int  status ;
                   char  text[512] ;
                   char *end ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Path *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 SETi(IDC_RANDOM_COUNT, Module->mRandomPerDirect) ;
                 SETi(IDC_VECTORS_MAX,  Module->mVectorsMax) ;

              sprintf(text, "%lf", Module->mLineStep_user) ;
                 SETs(IDC_LINE_STEP, text) ;
              sprintf(text, "%lf", Module->mAngleStep_user) ;
                 SETs(IDC_ANGLE_STEP, text) ;

                 SETi(IDC_SCAN_STEP,    Module->mScanStepCoef) ;
                 SETi(IDC_FAIL_MAX,     Module->mFailDirect) ;

              sprintf(text, "%lf", Module->mLineAcc_user) ;
                 SETs(IDC_LINE_ACC, text) ;
              sprintf(text, "%lf", Module->mAngleAcc_user) ;
                 SETs(IDC_ANGLE_ACC, text) ;
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

             Module->mRandomPerDirect=GETi(IDC_RANDOM_COUNT) ;
             Module->mVectorsMax     =GETi(IDC_VECTORS_MAX) ;
             Module->mScanStepCoef   =GETi(IDC_SCAN_STEP) ;
             Module->mFailDirect     =GETi(IDC_FAIL_MAX) ;

                                      GETs(IDC_LINE_STEP, text) ;
             Module->mLineStep_user=strtod(text, &end) ;
         if(*end!=0) {
                        SEND_ERROR("Некорректное значение шага координат") ;
  			       return(FALSE) ;
                     }

                                       GETs(IDC_ANGLE_STEP, text) ;
             Module->mAngleStep_user=strtod(text, &end) ;
         if(*end!=0) {
                        SEND_ERROR("Некорректное значение шага углов") ;
  			       return(FALSE) ;
                     }

                                     GETs(IDC_LINE_ACC, text) ;
             Module->mLineAcc_user=strtod(text, &end) ;
         if(*end!=0) {
                        SEND_ERROR("Некорректное значение точности координат") ;
  			       return(FALSE) ;
                     }

                                      GETs(IDC_ANGLE_ACC, text) ;
             Module->mAngleAcc_user=strtod(text, &end) ;
         if(*end!=0) {
                        SEND_ERROR("Некорректное значение точности углов") ;
  			       return(FALSE) ;
                     }

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
/* 	     Обработчик сообщений диалогового окна SAVE              */

    BOOL CALLBACK  Task_Path_Save_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
 static TRAJECTORY *Trajectory ;
               int  elm ;         /* Идентификатор элемента диалога */
               int  status ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

               Trajectory=(TRAJECTORY *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 SETs(IDC_NAME,        Trajectory->name) ;
                 SETs(IDC_DESCRIPTION, Trajectory->desc) ;
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

                 GETs(IDC_NAME,        Trajectory->name) ;
                 GETs(IDC_DESCRIPTION, Trajectory->desc) ;

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

