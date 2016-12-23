/********************************************************************/
/*								    */
/*		������ ������ "������ �����"    		    */
/*								    */
/*                   ���������� ���������                           */
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

#include "T_Point.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"

/*------------------------------------ ����������� ��������� ������� */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Title_WndProc ;
  static union WndProc_par  List_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Point_Edit_prc(HWND, UINT, WPARAM, LPARAM) ;
   LRESULT CALLBACK  Task_Point_List_prc(HWND, UINT, WPARAM, LPARAM) ;

/*---------------------------------------------------- ��������� �/� */

   char *Task_ptr_incode(void *) ;
   void *Task_ptr_decode(char *) ;


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

    BOOL CALLBACK  Task_Point_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Point  Module ;
               int  elm ;         /* ������������� �������� ������� */
               int  status ;
               int  index ;
               int  insert_flag ;
              char *help ;
              char  text[512] ;
              char *end ;
               int  i ;

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
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
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

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
/* 	     ���������� ��������� ����������� ���� ROUND             */

 typedef struct {  RSS_Module_Point_Indicator *data ;
                                         HWND  hDlg ; }  Round_context ;


   BOOL CALLBACK  Task_Point_Round_dialog(  HWND  hDlg,     UINT  Msg, 
                                          WPARAM  wParam, LPARAM  lParam) 
{
        static Round_context   contexts[_IND_MAX] ;
  RSS_Module_Point_Indicator  *context ;
                        HWND   hElem  ;
                         int   elm ;           /* ������������� �������� ������� */
                         int   status ;
                        char   title[1024] ;
                         int   i ;

/*-------------------------------------------- ����������� ��������� */

                                    context=NULL ;
    for(i=0 ; i<_IND_MAX ; i++) 
      if(contexts[i].hDlg==hDlg) {  context=contexts[i].data ;
                                               break ;           }

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {

                 context=(RSS_Module_Point_Indicator *)lParam ;
/*- - - - - - - - - - - - - - - - - -  �������� ���������� ��������� */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].data==context) {
                            EndDialog(contexts[i].hDlg, 0) ;
                                      contexts[i].hDlg=NULL ;
                                           }
/*- - - - - - - - - - - - - - - - - - - - - -  ����������� ��������� */
           for(i=0 ; i<_IND_MAX ; i++) 
             if(contexts[i].hDlg==NULL)  break ;

                contexts[i].hDlg=  hDlg ;
                contexts[i].data=context ;
/*- - - - - - - - - - - - - - - - -  �������� ������������ ��������� */
       Tmp_WndProc.call=Task_Point_Edit_prc ;
     Title_WndProc.par =GetWindowLong(ITEM(IDC_TITLE), GWL_WNDPROC) ;
                        SetWindowLong(ITEM(IDC_TITLE), GWL_WNDPROC,
                                                   Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
              sprintf(title, "%d: %s", context->idx, context->name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)title) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
                     SETs(IDC_TITLE, context->name) ;
                     HIDE(IDC_TITLE) ;

                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_SETTEXT, NULL, 
                       (LPARAM)Task_ptr_incode((void *)context)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_PAINT:      {

                      hElem=GetDlgItem(hDlg, IDC_INDICATOR) ;
          SendMessage(hElem, WM_PAINT, NULL, 
                       (LPARAM)Task_ptr_incode((void *)context)) ;
                          
 			    return(FALSE) ;
			       break ;
			}
/*------------------------------------------------ ��������� ������� */

    case WM_USER:      {
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
                 SHOW(IDC_CHANGE_TITLE) ;
                 HIDE(IDC_TITLE) ;              

                 GETs(IDC_TITLE, context->name) ;

              sprintf(title, "%d: %s", context->idx, context->name) ;
          SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)title) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 			    return(FALSE) ;
			       break ;
		       }
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {
                            status=HIWORD(wParam) ;
                               elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - -  ����������� ��������� ��������� */
    if(elm==IDC_CHANGE_TITLE) {
                                         HIDE(IDC_CHANGE_TITLE) ;
                                         SHOW(IDC_TITLE) ;              
                          SetFocus(GetDlgItem(hDlg, IDC_TITLE)) ;
                                          return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {

         for(i=0 ; i<_IND_MAX ; i++)                                /* ������� ������ �� ���������� */
           if(contexts[i].hDlg==hDlg) {  contexts[i].hDlg=NULL ;    /*   �� ������ ����������       */
                                                 break ;         }

                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

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
/*       ���������� ��������� ����������� ���� INDICATORS LIST       */	

    BOOL CALLBACK  Task_Point_IList_dialog(  HWND  hDlg,     UINT  Msg, 
 			                   WPARAM  wParam, LPARAM  lParam) 
{
                static  HWND   hActive ;        /* ������ �� �������� ���� */
    static  RSS_Module_Point  *Module ;
                         int   elm ;            /* ������������� �������� ������� */
                         int   status ;
  RSS_Module_Point_Indicator  *ind_cur ;
                         int   index ;
                        char   text[1024] ;
                         int   i ; 

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {

                Module=(RSS_Module_Point *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - ����������� ��������� ���� */
         if(hActive!=NULL) {
                               EndDialog(hActive, 0) ;
                           }

                                hActive=hDlg ;
/*- - - - - - - - - - - - - - - - -  ������������� ������� ��������� */
       for(i=0 ; i<_IND_MAX ; i++) {

         if(Module->mIndicators[i]==NULL)  continue ;

                       sprintf(text, "%03d: %s", i, Module->mIndicators[i]->name) ;
             index=LB_ADD_ROW (IDC_LIST, text) ;
                   LB_SET_ITEM(IDC_LIST, index, Module->mIndicators[i]) ;
                                   }
/*- - - - - - - - - - - - - - - - -  �������� ������������ ��������� */
           List_WndProc.par=GetWindowLong(ITEM(IDC_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=Task_Point_List_prc ;
          SetWindowLong(ITEM(IDC_LIST),
                         GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ ��������� ���������� �������� */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - -  ����������� ������� ������� */
               ind_cur=(RSS_Module_Point_Indicator *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - �������� ��������� */
     if(wParam==_TASK_POINT_ADD_INDICATOR) {

       for(i=0 ; i<_IND_MAX ; i++) {

         if(Module->mIndicators[i]!=ind_cur)  continue ;

                       sprintf(text, "%03d: %s", i, Module->mIndicators[i]->name) ;
             index=LB_ADD_ROW (IDC_LIST, text) ;
                   LB_SET_ITEM(IDC_LIST, index, Module->mIndicators[i]) ;

                                          break ;
                                   }

			            return(FALSE) ;

                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
     if(wParam==_TASK_POINT_KILL_INDICATOR) {

         for(i=0 ; i<_IND_MAX ; i++)
           if(lParam==LB_ROW_ITEM(IDC_LIST, i)) {
                                         LB_DEL_ROW(IDC_LIST, i) ;
                                                    break ;
                                                } 

			            return(FALSE) ;

                                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
  			     break ;
  		   }
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

                status=HIWORD(wParam) ;
	           elm=LOWORD(wParam) ;

               ind_cur=(RSS_Module_Point_Indicator *)LB_GET_ITEM(IDC_LIST) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ������ ����������� */
       if(elm==IDC_LIST) {

        if(status==LBN_DBLCLK) {
                                  Module->iIndicatorShow(ind_cur) ;
                                   Module->kernel->vShow("REFRESH") ;
                               }

			            return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ��������� */
       if(elm==IDC_IND_DEL) {

             Module->iIndicatorKill(ind_cur) ;
              Module->kernel->vShow("REFRESH") ;
                         LB_DEL_ROW(IDC_LIST, LB_GET_ROW(IDC_LIST)) ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - -   �������� ��������� */
       if(elm==IDC_IND_ALL  ||
          elm==IDC_IND_SCENE  ) {
                                  Module->iIndicatorShow(ind_cur) ;
                                   Module->kernel->vShow("REFRESH") ;

            if(elm==IDC_IND_ALL)  Module->iIndicatorTop (ind_cur) ;

  			               return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ������ ��������� */
       if(elm==IDC_IND_HIDE) {

             Module->iIndicatorHide(ind_cur) ;
              Module->kernel->vShow("REFRESH") ;

			            return(FALSE) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                           EndDialog(hDlg, 0) ;
                    Module->mIndListWnd=NULL ;
                                hActive=NULL ;

			      return(FALSE) ;
			          break ;
			}
/*----------------------------------------------------------- ������ */

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
/*	          ����e ����������� ���������                        */    

  LRESULT CALLBACK  Task_Point_Edit_prc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
  LRESULT  result ;

/*---------------------------------------- ��������� ������ �� ����� */

  if(Msg==WM_KEYUP   ||
     Msg==WM_SYSKEYUP  ) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  <Enter> */
    if(wParam==VK_RETURN) {

        SendMessage(GetParent(hWnd), WM_USER,
                     (WPARAM)_USER_EXECUTE_COMMAND, (LPARAM)NULL) ;

                                        return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        }
/*-------------------------------- ����� ������� ��������� ��������� */

      result=CallWindowProc(Title_WndProc.call,
                               hWnd, Msg, wParam, lParam) ;

/*-------------------------------------------------------------------*/

  return(result) ;
}


  LRESULT CALLBACK  Task_Point_List_prc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) 
{
   HMENU  hMenu ;
 LRESULT  result ;
     int  menu_flag ;
    RECT  Rect ;
     int  x ;
     int  y ;

/*----------------------------------- ������� ������ ������ �� ����� */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- ����� ������� ��������� ��������� */

      result=CallWindowProc(List_WndProc.call,
                                hWnd, Msg, wParam, lParam) ;

/*------------------------------------- ��������� ���� ������ ������ */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_ALL,   "�������� � ���� � �� �����") ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_SCENE, "�������� ������ �� �����"  ) ;
          AppendMenu(hMenu, MF_SEPARATOR,          0,      NULL                     ) ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_HIDE,  "������ �� �����"           ) ;
          AppendMenu(hMenu, MF_SEPARATOR,          0,      NULL                     ) ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_DEL,   "�������"                   ) ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

         DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


/*********************************************************************/
/*                                                                   */
/*            ������� - ��������� �������� ��������                  */

 LRESULT CALLBACK  Task_Point_RoundIndicator_prc(
                                      HWND  hWnd,     UINT  Msg,
 			            WPARAM  wParam, LPARAM  lParam
                                                )
{
                         HDC  hDC_wnd ;
                         HDC  hDC ;
                     HBITMAP  hBM ;
                 PAINTSTRUCT  PaintCfg ;
                        HPEN  Pen ;
                        HPEN  Pen_prv ;
                      HBRUSH  Brush ;
                      HBRUSH  Brush_prv ;
                        RECT  Rect ;
  RSS_Module_Point_Indicator *data ;               /* �������� ���������� */
                        char  data_ptr[32] ;       /* ����� ��������, ������������ */
                         int  rad ;
                         int  x_c ;
                         int  y_c ;
                      double  step ;
                      double  start ;
                      double  dir ;
                      double  angle ;
                         int  i ;

#define  _PI  3.1415926

/*------------------------------------------------ ���������� ������ */

   if(Msg==WM_PAINT        ||
      Msg==WM_LBUTTONDBLCLK  ) {
  
        SendMessage(hWnd, WM_GETTEXT, (WPARAM)sizeof(data_ptr),
                                      (LPARAM)       data_ptr  ) ;

           data=(RSS_Module_Point_Indicator *)Task_ptr_decode(data_ptr) ;
                               }
/*--------------------------------------------------- ����� �������� */

  switch(Msg) {

/*----------------------------------------------- �������� ��������� */

    case WM_CREATE:   break ;

    case WM_COMMAND:  break ;

/*-------------------------------------------------------- ��������� */

    case WM_PAINT:   {
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
                InvalidateRect(hWnd, NULL, false) ;

        if(data==NULL) {
                          ValidateRect(hWnd,  NULL) ;
                              break ;
                       }
/*- - - - - - - - - - - - - - - - - -  �������� "��������" ��������� */
       hDC_wnd=BeginPaint(hWnd, &PaintCfg) ;

           hDC=CreateCompatibleDC    (hDC_wnd) ; 
           hBM=CreateCompatibleBitmap(hDC_wnd, GetDeviceCaps(hDC_wnd, HORZRES), 
                                               GetDeviceCaps(hDC_wnd, VERTRES) ) ; 
                         SelectObject(hDC, hBM) ;
/*- - - - - - - - - - - - - - - - - - - - - �������������� ��������� */
               GetClientRect(hWnd, &Rect) ;

                 x_c=(Rect.left+Rect.right)/2 ;
                 y_c=(Rect.bottom+Rect.top)/2 ;
                 rad=(Rect.bottom-Rect.top)/2-10 ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� ���� ���� */
             Brush    =    CreateSolidBrush(RGB(192, 192, 192)) ;
             Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

                                   FillRect(hDC, &Rect, Brush) ;

                               SelectObject(hDC, Brush_prv) ;
                               DeleteObject(Brush) ;
/*- - - - - - - - - - - - - - - - - - - ������������ ��������� ����� */
                                        step=2.*_PI/data->data_cnt ;

          if(data->angle_code=='A') {  start=+_PI/2. ;
                                         dir= -1.   ;  }
          if(data->angle_code=='E') {  start=  0 ;
                                         dir= -1.   ;  }
          if(data->angle_code=='R') {  start=-_PI/2 ;
                                         dir= -1.   ;  }

           for(i=0 ; i<data->data_cnt ; i++) {

                      if(data->data[i]==0)  continue ;

                                      angle=start+dir*step*i ;

                           Pen    =           CreatePen(PS_SOLID, 0, data->data[i]) ;
                           Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;
                         Brush    =    CreateSolidBrush(data->data[i]) ;
                         Brush_prv=(HBRUSH)SelectObject(hDC, Brush) ;

#pragma warning(disable : 4244)
                                  Pie(hDC, x_c-rad, y_c-rad, 
                                           x_c+rad, y_c+rad,
                                           x_c+rad*cos(angle-dir*step/2.),
                                           y_c+rad*sin(angle-dir*step/2.),
                                           x_c+rad*cos(angle+dir*step/2.),
                                           y_c+rad*sin(angle+dir*step/2.) ) ;
#pragma warning(default : 4244)

                                   SelectObject(hDC, Brush_prv) ;
                                   SelectObject(hDC, Pen_prv) ;
                                   DeleteObject(Brush) ;
                                   DeleteObject(Pen) ;
                                             }
/*- - - - - - - - - - - - - - - - - - - -  �������������� ���������� */
               Pen    =           CreatePen(PS_SOLID, 0, 0) ;
               Pen_prv=(HPEN)  SelectObject(hDC, Pen) ;

                         MoveToEx(hDC, x_c, y_c+rad+1, NULL) ;
                           LineTo(hDC, x_c, y_c+rad+6) ;
                         MoveToEx(hDC, x_c, y_c-rad-1, NULL) ;
                           LineTo(hDC, x_c, y_c-rad-6) ;

                         MoveToEx(hDC, x_c+rad+1, y_c, NULL) ;
                           LineTo(hDC, x_c+rad+6, y_c) ;
                         MoveToEx(hDC, x_c-rad-1, y_c, NULL) ;
                           LineTo(hDC, x_c-rad-6, y_c) ;

#pragma warning(disable : 4244)
               for(i=0 ; i<36 ; i++) {
                                                      angle=_PI*i/18. ;
                        SetPixelV(hDC, x_c+(rad+2)*cos(angle), 
                                       y_c+(rad+2)*sin(angle), 0) ;
                                     }
#pragma warning(default : 4244)
/*- - - - - - - - - - - �������������� ���������� �������� ��������� */
        if(data->scan_flag) {
#pragma warning(disable : 4244)
                              angle=start+dir*data->scan*_PI/180. ;
                         MoveToEx(hDC, x_c, y_c, NULL) ;
                           LineTo(hDC, x_c+(rad+2)*cos(angle), 
                                       y_c+(rad+2)*sin(angle) ) ;
#pragma warning(disable : 4244)
                            }

                               SelectObject(hDC, Pen_prv) ;
                               DeleteObject(Pen) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ����� ��������� */
            BitBlt(hDC_wnd, 
                   Rect.left, Rect.top, 
                   Rect.right-Rect.left, Rect.bottom-Rect.top, 
                   hDC,
                   Rect.left, Rect.top, 
                   SRCCOPY                           ) ;

                    ValidateRect(hWnd,  NULL) ;
                        EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - -  ������������ �������� */
            if(hBM!=NULL)  DeleteObject(hBM) ;
            if(hDC!=NULL)  DeleteDC    (hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			     break ;
		     }
/*------------------------------------------------- ������ ��������� */

    default :        {
		return( DefWindowProc(hWnd, Msg, wParam, lParam) ) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                �������������� ������ ������ � ������              */
/*                �������������� ������ � ����� ������               */

#define  _UD_PTR_PREFIX   "UD_Pointer:"

  union {
          unsigned  long  code ;
                    void *ptr ;  } Task_ptr_cvt ;


   char *Task_ptr_incode(void *ptr)

{
  static char  ptr_text[128] ;
         char  tmp[32] ;


              Task_ptr_cvt.ptr=ptr ;
        ultoa(Task_ptr_cvt.code, tmp, 16) ;

       strcpy(ptr_text, _UD_PTR_PREFIX) ;
       strcat(ptr_text,     tmp       ) ;

   return(ptr_text) ;
}


   void *Task_ptr_decode(char *text)

{
  char *end ;


        end=strstr(text, _UD_PTR_PREFIX) ;
     if(end==NULL)  return(NULL) ;

        end=text+strlen(_UD_PTR_PREFIX) ;

          Task_ptr_cvt.code=strtoul(end, &end, 16) ;

   return(Task_ptr_cvt.ptr) ;
}


