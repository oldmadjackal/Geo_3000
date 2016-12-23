/********************************************************************/
/*								    */
/*		������ ������ "������ �����������"    		    */
/*								    */
/*                   ���������� ������                              */
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

/*-------------------------------------- ��������� ��������� ������� */

#define   _COL_CNT     8
#define   _ROW_CNT     4
#define  _CEIL_CNT  (_COL_CNT*_ROW_CNT)

  static  UD_color_panel  Palette[_CEIL_CNT] ;    /* ������� ������ �������� */
  static        COLORREF  Color ;                 /* ���������� ���� */

  static            HWND  hColorDialog ;


/*********************************************************************/
/*								     */
/*                      ����� ����� �� �������                       */

    COLORREF Task_Scan_color_choice(COLORREF color)  
{
   RSS_Kernel *Module ;
          int  coef ;
          int  row ;
          int  i ;

/*--------------------------------------------- ������������ ������� */

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
/*--------------------------------------------- ������� ������������ */

   for(i=0 ; i<_CEIL_CNT ; i++)  
              Palette[i].choice_proc=Task_Scan_set_color ;
     
/*---------------------------------------- �������� ����������� ���� */

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
/*	   ���������� ��������� ����������� ���� VIEW	             */	

    BOOL CALLBACK  Task_Scan_color_dialog(  HWND  hDlg,     UINT  Msg, 
 		                          WPARAM  wParam, LPARAM  lParam) 
{
              HWND  hElem  ;
               int  elm ;            /* ������������� �������� ������� */
               int  status ;
               int  i ; 

/*------------------------------------------------- ������� �������� */

                        hColorDialog=hDlg ;

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - - -  ������������� ������� */
     for(i=0 ; i<_CEIL_CNT ; i++) {
                      hElem=GetDlgItem(hDlg, IDC_COLOR0+i) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)UD_ptr_incode((void *)&Palette[i])) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;

			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
			    EndDialog(hDlg, TRUE) ;

			  return(FALSE) ;
			     break ;
			}
/*----------------------------------------------------------- ������ */

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
/*                          ��������� �����                          */

 COLORREF  Task_Scan_set_color(COLORREF color)

{
       Color=color ;

     EndDialog(hColorDialog, 0) ;

  return(color) ;
}


