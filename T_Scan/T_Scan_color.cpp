/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ПОВЕРХНОСТИ"    		    */
/*								    */
/*                   Управление цветом                              */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <math.h>

#include "d:\_Projects\_Libraries\UserDlg.h"
#include "d:\_Projects\_Libraries\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Scan.h"

#include "resource.h"

/*-------------------------------------- Обработка элементов диалога */

#define   _COL_CNT     8
#define   _ROW_CNT     4
#define  _CEIL_CNT  (_COL_CNT*_ROW_CNT)

  static  UD_color_panel  Palette[_CEIL_CNT] ;    /* Таблица цветов скаляров */
  static        COLORREF  Color ;                 /* Выбираемый цвет */

  static            HWND  hColorDialog ;


/*********************************************************************/
/*								     */
/*                      Выбор цвета по палитре                       */

    COLORREF Task_Scan_color_choice(COLORREF color)  
{
   RSS_Kernel *Module ;
          int  coef ;
          int  row ;
          int  i ;

/*--------------------------------------------- Формирование палитры */

   for(i=0, row=0 ; row<4 ; row++) {  
  
                         coef=64*row+63 ;

           Palette[i++].color=RGB(   0,    0,    0) ;
           Palette[i++].color=RGB(coef,    0,    0) ;
           Palette[i++].color=RGB(   0, coef,    0) ;
           Palette[i++].color=RGB(   0,    0, coef) ;
           Palette[i++].color=RGB(coef, coef,    0) ;
           Palette[i++].color=RGB(coef,    0, coef) ;
           Palette[i++].color=RGB(   0, coef, coef) ;
           Palette[i++].color=RGB(coef, coef, coef) ;
                                   }
/*--------------------------------------------- Пропись обработчиков */

   for(i=0 ; i<_CEIL_CNT ; i++)  
              Palette[i].choice_proc=Task_Scan_set_color ;
     
/*---------------------------------------- Создание диалогового окна */

                      Color=color ;
                      Module=GetTaskScanEntry() ;

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Module->Resource("IDD_COLOR_CHOICE", RT_DIALOG),
			   GetActiveWindow(), Task_Scan_color_dialog) ;

/*-------------------------------------------------------------------*/

  return(Color) ;
}


/*********************************************************************/
/*								     */
/*	   Обработчик сообщений диалогового окна VIEW	             */	

    BOOL CALLBACK  Task_Scan_color_dialog(  HWND  hDlg,     UINT  Msg, 
 		                          WPARAM  wParam, LPARAM  lParam) 
{
              HWND  hElem  ;
               int  elm ;            /* Идентификатор элемента диалога */
               int  status ;
               int  i ; 

/*------------------------------------------------- Большая разводка */

                        hColorDialog=hDlg ;

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - - -  Инициализация списков */
     for(i=0 ; i<_CEIL_CNT ; i++) {
                      hElem=GetDlgItem(hDlg, IDC_COLOR0+i) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)UD_ptr_incode((void *)&Palette[i])) ;
                                  }
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
			    EndDialog(hDlg, TRUE) ;

			  return(FALSE) ;
			     break ;
			}
/*----------------------------------------------------------- Прочее */

//  case WM_PAINT:    break ;

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
/*                          Установка цвета                          */

 COLORREF  Task_Scan_set_color(COLORREF color)

{
       Color=color ;

     EndDialog(hColorDialog, 0) ;

  return(color) ;
}


