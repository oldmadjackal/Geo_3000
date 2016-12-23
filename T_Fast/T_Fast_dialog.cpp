/********************************************************************/
/*								    */
/*      МОДУЛЬ ЗАДАЧИ "ОПТИМИЗАЦИЯ ТРАЕКТОРИИ ПО БЫСТРОДЕЙСТВИЮ"    */
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

#include "T_Fast.h"

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

/*-------------------------------------------------------- Мнемоники */

#define   _SEQUENCE_METHOD_TEXT  "Последовательно с накомплением"
#define     _RANDOM_METHOD_TEXT  "Случайно с накоплением"
#define       _FULL_METHOD_TEXT  "Полный перебор до лучшего" 

/*---------------------------------------------------- Прототипы п/п */


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Task_Fast_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Fast  Module ;
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

    BOOL CALLBACK  Task_Fast_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                   WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Fast *Module ;
                    int  elm ;         /* Идентификатор элемента диалога */
                    int  status ;
                   char  value[1024] ;
                   char *end ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Fast *)lParam ;
/*- - - - - - - - - - - - - - - - - -  Инициализация списков методов */
             CB_CLEAR(IDC_SIMPL_METHOD) ;
          CB_ADD_LIST(IDC_SIMPL_METHOD, _SEQUENCE_METHOD_TEXT) ;
          CB_ADD_LIST(IDC_SIMPL_METHOD, _RANDOM_METHOD_TEXT) ;
          CB_ADD_LIST(IDC_SIMPL_METHOD, _FULL_METHOD_TEXT) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                 SETs(IDC_TIME_CALC,  Module->mTimeCalc_cmd) ;
                 SETs(IDC_LINE_CHECK, Module->mLineCheck_cmd) ;
                 SETs(IDC_LINE_DRAW,  Module->mLineDraw_cmd) ;

                 SETx(IDC_SIMPL_OPT,  Module->mSimplification_opt) ;
          if(Module->mSimplification_method==_SEQUENCE_METHOD)  strcpy(value, _SEQUENCE_METHOD_TEXT) ;
     else if(Module->mSimplification_method==  _RANDOM_METHOD)  strcpy(value,   _RANDOM_METHOD_TEXT) ;
     else if(Module->mSimplification_method==    _FULL_METHOD)  strcpy(value,     _FULL_METHOD_TEXT) ;
     else                                                       strcpy(value,        "** ОШИБКА **") ;
                 SETc(IDC_SIMPL_METHOD,  value) ;

                 SETx(IDC_SMOOTH_OPT, Module->mSmoothing_opt) ;

              sprintf(value, "%.3lf", Module->mCutSegment_MinTime) ;
                 SETx(IDC_CUT_OPT,  Module->mCutSegment_opt) ;
                 SETs(IDC_CUT_TIME, value) ;
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
                                    GETs(IDC_TIME_CALC,  Module->mTimeCalc_cmd) ;
                                    GETs(IDC_LINE_CHECK, Module->mLineCheck_cmd) ;
                                    GETs(IDC_LINE_DRAW,  Module->mLineDraw_cmd) ;
        Module->mSimplification_opt=GETx(IDC_SIMPL_OPT) ;
        Module->mSmoothing_opt     =GETx(IDC_SMOOTH_OPT) ;
        Module->mCutSegment_opt    =GETx(IDC_CUT_OPT) ;

                                    GETc(IDC_SIMPL_METHOD, value) ;
          if(!stricmp(value, _SEQUENCE_METHOD_TEXT))  Module->mSimplification_method=_SEQUENCE_METHOD ;
     else if(!stricmp(value,   _RANDOM_METHOD_TEXT))  Module->mSimplification_method=  _RANDOM_METHOD ;
     else if(!stricmp(value,     _FULL_METHOD_TEXT))  Module->mSimplification_method=    _FULL_METHOD ;

                                    GETs(IDC_CUT_TIME, value) ;
      Module->mCutSegment_MinTime=strtod(value, &end) ;

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




