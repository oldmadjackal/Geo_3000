/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ВЫХОД В ТОЧКУ"    		    */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#pragma warning( disable : 4996 )
#define  stricmp  _stricmp

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "d:\_Projects\_Libraries\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Reach.h"

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

    BOOL CALLBACK  Task_Reach_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Reach  Module ;
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

    BOOL CALLBACK  Task_Reach_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                    WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Reach *Module ;
                     int  elm ;         /* Идентификатор элемента диалога */
                     int  status ;
                    char  text[512] ;
                    char *end ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Reach *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 SETx(IDC_START_CUR,   !Module->mResetStart) ;
                 SETx(IDC_START_MID,    Module->mResetStart) ;

                 SETx(IDC_DRAW_SCAN,    Module->mDraw) ;
                 SETx(IDC_CHECK_RANGES, Module->mCheckRanges) ;
                 SETi(IDC_SCAN_STEP,    Module->mScanStepCoef) ;
                 SETi(IDC_JAMP_RANGE,   Module->mJampRangeCoef) ;
                 SETi(IDC_FAIL_MAX,     Module->mFailMax) ;

              sprintf(text, "%lf", Module->mLineAcc) ;
                 SETs(IDC_LINE_ACC, text) ;
              sprintf(text, "%lf", Module->mAngleAcc) ;
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

             Module->mResetStart   =GETx(IDC_START_MID) ;
             Module->mDraw         =GETx(IDC_DRAW_SCAN) ;
             Module->mCheckRanges  =GETx(IDC_CHECK_RANGES) ;
             Module->mScanStepCoef =GETi(IDC_SCAN_STEP) ;
             Module->mJampRangeCoef=GETi(IDC_JAMP_RANGE) ;
             Module->mFailMax      =GETi(IDC_FAIL_MAX) ;

                                    GETs(IDC_LINE_ACC, text) ;
             Module->mLineAcc      =strtod(text, &end) ;
         if(*end!=0) {
                        SEND_ERROR("Некорректное значение точности координат") ;
  			       return(FALSE) ;
                     }

                                    GETs(IDC_ANGLE_ACC, text) ;
             Module->mAngleAcc     =strtod(text, &end) ;
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

