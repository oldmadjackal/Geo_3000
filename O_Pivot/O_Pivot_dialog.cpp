/********************************************************************/
/*								    */
/*                  МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ        		    */
/*      ПЛОСКИЙ МАНИПУЛЯТОР НА ВРАЩАЮЩИМСЯ ОСНОВАНИИ С              */
/*            2-Х СТЕПЕННОЙ ПРОСТРАНСТВЕННОЙ КИСТЬЮ                 */
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

#include "O_Pivot.h"

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

  LRESULT CALLBACK  OP_BodyList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) ;
  LRESULT CALLBACK  OP_MassList_WndProc(  HWND  hWnd,     UINT  Msg,
	                                WPARAM  wParam, LPARAM  lParam) ;

/*-------------------------------------------------------------------*/

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Object_Pivot_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                    WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Pivot  Module ;
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
/* 	     Обработчик сообщений диалогового окна CREATE            */

    BOOL CALLBACK  Object_Pivot_Create_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
                    RSS_Module_Pivot *Module ;
 static RSS_Module_Pivot_Create_data *data ;
                                 int  elm ;               /* Идентификатор элемента диалога */
                                 int  status ;

     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              data=(RSS_Module_Pivot_Create_data *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
            SETs(IDC_NAME,      data->name) ;
            SETu(IDC_STRUCTURE, data->scheme) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
     if(elm==IDC_CREATE) {

                  GETs(IDC_NAME,      data->name) ;
                  GETu(IDC_STRUCTURE, data->scheme) ;

                status=Module->CreateObject(data) ;
             if(status==0)  EndDialog(hDlg, 0) ;

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
/* 	     Обработчик сообщений диалогового окна DEGREE            */

    BOOL CALLBACK  Object_Pivot_Degree_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Pivot *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
         static    HFONT  font_a ;           /* Шрифт "активных" степеней */
         static    HFONT  font_p ;           /* Шрифт "пассивных" степеней */
                    char  dgt[512] ;
                     int  i ;

#define   L   object->links
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Подготовка шрифтов */
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
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                SETu(IDC_STRUCTURE, object->scheme) ;

               sprintf(dgt, "%.3lf", object->link_leg.joint) ;
               SETFONT(IDC_BASE_JOINT, font_a) ;
                  SETs(IDC_BASE_JOINT, dgt) ;
        
               sprintf(dgt, "%.3lf", object->link_leg.link) ;
               SETFONT(IDC_LEG_HEIGHT, font_p) ;
                  SETs(IDC_LEG_HEIGHT, dgt) ;

               sprintf(dgt, "%.3lf", object->link_wrist.joint) ;
               SETFONT(IDC_WRIST_SWIVEL, font_a) ;
                  SETs(IDC_WRIST_SWIVEL, dgt) ;

               sprintf(dgt, "%.3lf", object->link_wrist.link) ;
               SETFONT(IDC_WRIST_LENGTH, font_p) ;
                  SETs(IDC_WRIST_LENGTH, dgt) ;

               sprintf(dgt, "%.3lf", object->link_gripper.joint) ;
               SETFONT(IDC_GRIPPER_ROLL, font_a) ;
                  SETs(IDC_GRIPPER_ROLL, dgt) ;

               sprintf(dgt, "%.3lf", object->link_gripper.link) ;
               SETFONT(IDC_GRIPPER_BASE, font_p) ;
                  SETs(IDC_GRIPPER_BASE, dgt) ;

         for(i=0 ; i<_LINKS_MAX ; i++) {

                 if(L[i].joint_fixed) SETFONT(IDC_JOINT1+i, font_p) ;
                 else                 SETFONT(IDC_JOINT1+i, font_a) ;

                 if(L[i].link_fixed ) SETFONT(IDC_LINK1+i, font_p) ;
                 else                 SETFONT(IDC_LINK1+i, font_a) ;

                    sprintf(dgt, "%.3lf", L[i].joint) ;
                       SETs(IDC_JOINT1+i, dgt) ;

                    sprintf(dgt, "%.3lf", L[i].link) ;
                       SETs(IDC_LINK1+i, dgt) ;

                 if(L[i].use_flag==0) {  HIDE(IDC_JOINT1+i) ;
                                         HIDE(IDC_LINK1 +i) ;  } 
                 else                 {  SHOW(IDC_JOINT1+i) ;
                                         SHOW(IDC_LINK1 +i) ;  } 
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - - Сообщение об изменении схемы */
        if(wParam==_USER_SCHEME_CHANGE) {

            GETu(IDC_STRUCTURE, object->scheme) ;
                                object->DissectScheme() ;  

        for(i=0 ; i<_LINKS_MAX ; i++)
          if(L[i].action)
           if(L[i].use_flag==0) {
                                         HIDE(IDC_JOINT1+i) ;
                                         HIDE(IDC_LINK1 +i) ;
                                } 
           else                 {

                  if(L[i].joint_fixed) SETFONT(IDC_JOINT1+i, font_p) ;
                  else                 SETFONT(IDC_JOINT1+i, font_a) ;

                  if(L[i].link_fixed ) SETFONT(IDC_LINK1+i, font_p) ;
                  else                 SETFONT(IDC_LINK1+i, font_a) ;

                                         SHOW(IDC_JOINT1+i) ;
                                         SHOW(IDC_LINK1 +i) ;
                                         GETs(IDC_JOINT1+i, dgt) ;
                                         SETs(IDC_JOINT1+i, dgt) ;
                                         GETs(IDC_LINK1 +i, dgt) ;
                                         SETs(IDC_LINK1 +i, dgt) ;
                               }

                                           return(FALSE) ;
                                        }
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
                                   GETs(IDC_BASE_JOINT, dgt) ;
            object->link_leg.joint=atof(dgt) ;
                                   GETs(IDC_LEG_HEIGHT, dgt) ;
             object->link_leg.link=atof(dgt) ;
                                   GETs(IDC_WRIST_SWIVEL, dgt) ;
          object->link_wrist.joint=atof(dgt) ;
                                   GETs(IDC_WRIST_LENGTH, dgt) ;
           object->link_wrist.link=atof(dgt) ;
                                   GETs(IDC_GRIPPER_ROLL, dgt) ;
        object->link_gripper.joint=atof(dgt) ;
                                   GETs(IDC_GRIPPER_BASE, dgt) ;
         object->link_gripper.link=atof(dgt) ;

       for(i=0 ; i<_LINKS_MAX ; i++) {
                              GETs(IDC_JOINT1+i, dgt) ;
                   L[i].joint=atof(dgt) ;
                              GETs(IDC_LINK1+i, dgt) ;
                    L[i].link=atof(dgt) ;
                                     } 
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

#undef   L

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна RANGE             */

    BOOL CALLBACK  Object_Pivot_Range_dialog(  HWND hDlg,     UINT Msg, 
 		  	                     WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Pivot *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
         static    HFONT  font_a ;           /* Шрифт "активных" степеней */
         static    HFONT  font_p ;           /* Шрифт "пассивных" степеней */
                    char  dgt[512] ;
                     int  i ;

#define   L   object->links
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
               sprintf(dgt, "%.3lf", object->link_leg.joint_min) ;
                  SETs(IDC_BASE_JOINT_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_leg.joint_max) ;
                  SETs(IDC_BASE_JOINT_MAX, dgt) ;
                  SETx(IDC_BASE_JOINT_TEST, object->link_leg.joint_test) ;
        
               sprintf(dgt, "%.3lf", object->link_wrist.joint_min) ;
                  SETs(IDC_WRIST_SWIVEL_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_wrist.joint_max) ;
                  SETs(IDC_WRIST_SWIVEL_MAX, dgt) ;
                  SETx(IDC_WRIST_SWIVEL_TEST, object->link_wrist.joint_test) ;

               sprintf(dgt, "%.3lf", object->link_gripper.joint_min) ;
                  SETs(IDC_GRIPPER_ROLL_MIN, dgt) ;
               sprintf(dgt, "%.3lf", object->link_gripper.joint_max) ;
                  SETs(IDC_GRIPPER_ROLL_MAX, dgt) ;
                  SETx(IDC_GRIPPER_ROLL_TEST, object->link_gripper.joint_test) ;

         for(i=0 ; i<_LINKS_MAX ; i++) {

                    sprintf(dgt, "%.3lf", L[i].joint_min) ;
                       SETs(IDC_JOINT1_MIN+i, dgt) ;
                    sprintf(dgt, "%.3lf", L[i].joint_max) ;
                       SETs(IDC_JOINT1_MAX+i, dgt) ;
                       SETx(IDC_JOINT1_TEST+i, L[i].joint_test) ;

                    sprintf(dgt, "%.3lf", L[i].link_min) ;
                       SETs(IDC_LINK1_MIN+i, dgt) ;
                    sprintf(dgt, "%.3lf", L[i].link_max) ;
                       SETs(IDC_LINK1_MAX+i, dgt) ;
                       SETx(IDC_LINK1_TEST+i, L[i].link_test) ;

             if(L[i].joint_fixed) {  DISABLE(IDC_JOINT1_MIN +i) ;
                                     DISABLE(IDC_JOINT1_MAX +i) ;
                                     DISABLE(IDC_JOINT1_TEST+i) ;  }
             if(L[i].link_fixed ) {  DISABLE(IDC_LINK1_MIN  +i) ;
                                     DISABLE(IDC_LINK1_MAX  +i) ;
                                     DISABLE(IDC_LINK1_TEST +i) ;  }

             if(L[i].use_flag==0) {     HIDE(IDC_JOINT1_MIN +i) ;
                                        HIDE(IDC_JOINT1_MAX +i) ;
                                        HIDE(IDC_JOINT1_TEST+i) ;
                                        HIDE(IDC_LINK1_MIN  +i) ;
                                        HIDE(IDC_LINK1_MAX  +i) ;
                                        HIDE(IDC_LINK1_TEST +i) ;  } 
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
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Снятие значений */
                                      GETs(IDC_BASE_JOINT_MIN, dgt) ;
          object->link_leg.joint_min =atof(dgt) ;
                                      GETs(IDC_BASE_JOINT_MAX, dgt) ;
          object->link_leg.joint_max =atof(dgt) ;
          object->link_leg.joint_test=GETx(IDC_BASE_JOINT_TEST) ;

                                      GETs(IDC_WRIST_SWIVEL_MIN, dgt) ;
        object->link_wrist.joint_min =atof(dgt) ;
                                      GETs(IDC_WRIST_SWIVEL_MAX, dgt) ;
        object->link_wrist.joint_max =atof(dgt) ;
        object->link_wrist.joint_test=GETx(IDC_WRIST_SWIVEL_TEST) ;

                                      GETs(IDC_GRIPPER_ROLL_MIN, dgt) ;
      object->link_gripper.joint_min =atof(dgt) ;
                                      GETs(IDC_GRIPPER_ROLL_MAX, dgt) ;
      object->link_gripper.joint_max =atof(dgt) ;
      object->link_gripper.joint_test=GETx(IDC_GRIPPER_ROLL_TEST) ;

       for(i=0 ; i<_LINKS_MAX ; i++) {

                               GETs(IDC_JOINT1_MIN+i, dgt) ;
               L[i].joint_min =atof(dgt) ;
                               GETs(IDC_JOINT1_MAX+i, dgt) ;
               L[i].joint_max =atof(dgt) ;
               L[i].joint_test=GETx(IDC_JOINT1_TEST+i) ;

                               GETs(IDC_LINK1_MIN+i, dgt) ;
                L[i].link_min =atof(dgt) ;
                               GETs(IDC_LINK1_MAX+i, dgt) ;
                L[i].link_max =atof(dgt) ;
                L[i].link_test=GETx(IDC_LINK1_TEST+i) ;
                                     }  
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

    BOOL CALLBACK  Object_Pivot_Bodies_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
              RSS_Kernel *Module ;
 static RSS_Object_Pivot *object ;
   RSS_Object_Pivot_Link *link ;
                     int  elm ;               /* Идентификатор элемента диалога */
                     int  status ;
                    char  desc[1024] ;
                     int  i ;
                     int  j ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
     for(elm=IDC_BASE_PARS ; elm<IDC_LINK8_PARS ; elm++) {

            if(elm==IDC_BASE_PARS   )  link=&object->link_base ;
       else if(elm==IDC_LEG_PARS    )  link=&object->link_leg ;
       else if(elm==IDC_WRIST_PARS  )  link=&object->link_wrist ;
       else if(elm==IDC_GRIPPER_PARS)  link=&object->link_gripper ;
       else                            link=&object->links[elm-IDC_LINK1_PARS] ;

                                 *desc=0 ;
            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->bodies[j].use_flag) {
                          strncat(desc, link->bodies[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

                       SETs(elm, desc) ;
                                                         }
/*- - - - - - - - - - - - - - - - - -  Установка невидимых элементов */
     for(i=0 ; i<_LINKS_MAX ; i++) 
       if(!object->links[i].use_flag) {
                                         HIDE(IDC_LINK1_PARS  +i) ;
                                         HIDE(IDC_ASSIGN_LINK1+i) ;
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
           elm<=IDC_ASSIGN_LINK8  ) {
                      
              if(elm==IDC_ASSIGN_BASE   )  link=&object->link_base ;
         else if(elm==IDC_ASSIGN_LEG    )  link=&object->link_leg ;
         else if(elm==IDC_ASSIGN_WRIST  )  link=&object->link_wrist ;
         else if(elm==IDC_ASSIGN_GRIPPER)  link=&object->link_gripper ;
         else                              link=&object->links[elm-IDC_ASSIGN_LINK1] ;

                Module=GetObjectPivotEntry() ;

                    DialogBoxIndirectParam( GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Module->Resource("IDD_BODY_ASSIGN", RT_DIALOG),
	            	                    GetActiveWindow(),
                                            Object_Pivot_BodyAssign_dialog,
                                    (LPARAM)link                            ) ;

                           memset(desc, 0, sizeof(desc)) ;

            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->bodies[j].use_flag) {
                          strncat(desc, link->bodies[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

              if(elm==IDC_ASSIGN_BASE   )  SETs(IDC_BASE_PARS,      desc) ;
         else if(elm==IDC_ASSIGN_LEG    )  SETs(IDC_LEG_PARS,       desc) ;
         else if(elm==IDC_ASSIGN_WRIST  )  SETs(IDC_WRIST_PARS,     desc) ;
         else if(elm==IDC_ASSIGN_GRIPPER)  SETs(IDC_GRIPPER_PARS,   desc) ;
         else                              SETs(IDC_LINK1_PARS+elm-
                                                IDC_ASSIGN_LINK1,   desc) ;

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

    BOOL CALLBACK  Object_Pivot_BodyAssign_dialog(  HWND hDlg,     UINT Msg, 
 		  	                          WPARAM wParam, LPARAM lParam) 
{
 static                  RECT  PictureFrame ;
                      HBITMAP  hBitmap ;
 static RSS_Object_Pivot_Link *link ;
        RSS_Object_Pivot_Body *body ;
 static RSS_Object_Pivot_Body *body_crn ;
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

               link=(RSS_Object_Pivot_Link *)lParam ;

              GetWindowRect(ITEM(IDC_PICTURE), &PictureFrame) ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
      BodyList_WndProc.par=GetWindowLong(ITEM(IDC_BODY_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=OP_BodyList_WndProc ;
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

          body=(RSS_Object_Pivot_Body *)LB_GET_ITEM(IDC_BODY_LIST) ;
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

                                  strcpy(morpho.object, "") ;       /* Исправление морфологического элемента */   
                                  strcpy(morpho.link,   "") ;
                                  strcpy(morpho.body, body_crn->desc) ;
                                         morpho.ptr  =body_crn ;
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

                                  strcpy(morpho.link, link->name) ; /* Добавление морфологического элемента */   
                                  strcpy(morpho.body, "body") ;
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

    BOOL CALLBACK  Object_Pivot_Masses_dialog(  HWND hDlg,     UINT Msg, 
 		  	                      WPARAM wParam, LPARAM lParam) 
{
              RSS_Kernel *Module ;
 static RSS_Object_Pivot *object ;
   RSS_Object_Pivot_Link *link ;
                     int  elm ;               /* Идентификатор элемента диалога */
                     int  status ;
                    char  desc[1024] ;
                     int  i ;
                     int  j ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
     for(elm=IDC_BASE_PARS ; elm<IDC_LINK8_PARS ; elm++) {

            if(elm==IDC_BASE_PARS   )  link=&object->link_base ;
       else if(elm==IDC_LEG_PARS    )  link=&object->link_leg ;
       else if(elm==IDC_WRIST_PARS  )  link=&object->link_wrist ;
       else if(elm==IDC_GRIPPER_PARS)  link=&object->link_gripper ;
       else                            link=&object->links[elm-IDC_LINK1_PARS] ;

                                 *desc=0 ;
            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->masses[j].use_flag) {
                          strncat(desc, link->masses[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

                       SETs(elm, desc) ;
                                                         }
/*- - - - - - - - - - - - - - - - - -  Установка невидимых элементов */
     for(i=0 ; i<_LINKS_MAX ; i++) 
       if(!object->links[i].use_flag) {
                                         HIDE(IDC_LINK1_PARS  +i) ;
                                         HIDE(IDC_ASSIGN_LINK1+i) ;
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
           elm<=IDC_ASSIGN_LINK8  ) {
                      
              if(elm==IDC_ASSIGN_BASE   )  link=&object->link_base ;
         else if(elm==IDC_ASSIGN_LEG    )  link=&object->link_leg ;
         else if(elm==IDC_ASSIGN_WRIST  )  link=&object->link_wrist ;
         else if(elm==IDC_ASSIGN_GRIPPER)  link=&object->link_gripper ;
         else                              link=&object->links[elm-IDC_ASSIGN_LINK1] ;

                Module=GetObjectPivotEntry() ;

                    DialogBoxIndirectParam( GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Module->Resource("IDD_MASS_ASSIGN", RT_DIALOG),
	            	                    GetActiveWindow(),
                                            Object_Pivot_MassAssign_dialog,
                                    (LPARAM)link                            ) ;

                           memset(desc, 0, sizeof(desc)) ;

            for(j=0 ; j<_BODIES_MAX ; j++)
               if(link->masses[j].use_flag) {
                          strncat(desc, link->masses[j].desc, sizeof(desc)-1) ;
                          strncat(desc, "; ",                 sizeof(desc)-1) ;
                                            }

              if(elm==IDC_ASSIGN_BASE   )  SETs(IDC_BASE_PARS,      desc) ;
         else if(elm==IDC_ASSIGN_LEG    )  SETs(IDC_LEG_PARS,       desc) ;
         else if(elm==IDC_ASSIGN_WRIST  )  SETs(IDC_WRIST_PARS,     desc) ;
         else if(elm==IDC_ASSIGN_GRIPPER)  SETs(IDC_GRIPPER_PARS,   desc) ;
         else                              SETs(IDC_LINK1_PARS+elm-
                                                IDC_ASSIGN_LINK1,   desc) ;

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

    BOOL CALLBACK  Object_Pivot_MassAssign_dialog(  HWND hDlg,     UINT Msg, 
 		  	                          WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Pivot_Link *link ;
        RSS_Object_Pivot_Mass *mass ;
 static RSS_Object_Pivot_Mass *mass_crn ;
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

               link=(RSS_Object_Pivot_Link *)lParam ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
      MassList_WndProc.par=GetWindowLong(ITEM(IDC_MASS_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=OP_MassList_WndProc ;
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

          mass=(RSS_Object_Pivot_Mass *)LB_GET_ITEM(IDC_MASS_LIST) ;
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

    BOOL CALLBACK  Object_Pivot_Keys_dialog(  HWND hDlg,     UINT Msg, 
 		  	                    WPARAM wParam, LPARAM lParam) 
{
 static RSS_Object_Pivot *object ;
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

#define   L   object->links
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - -  Прогрузка списка значений */
                         elem_cnt=0 ;
               elem_list[elem_cnt]=IDC_BASE_JOINT ;    elem_cnt++ ;
               elem_list[elem_cnt]=IDC_LEG_HEIGHT ;    elem_cnt++ ;
               elem_list[elem_cnt]=IDC_WRIST_SWIVEL ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_WRIST_LENGTH ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_GRIPPER_ROLL ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_GRIPPER_BASE ;  elem_cnt++ ; 

      for(i=0 ; i<_LINKS_MAX ; i++) {
               elem_list[elem_cnt]=IDC_JOINT1+i ;  elem_cnt++ ;
               elem_list[elem_cnt]=IDC_LINK1 +i ;  elem_cnt++ ; 
                                    }

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
                  SETc(IDC_BASE_JOINT,   keys_text[object->link_leg.joint_keys]);
                  SETc(IDC_LEG_HEIGHT,   keys_text[object->link_leg.link_keys]) ;
        
                  SETc(IDC_WRIST_SWIVEL, keys_text[object->link_wrist.joint_keys]) ;
                  SETc(IDC_WRIST_LENGTH, keys_text[object->link_wrist.link_keys]) ;

                  SETc(IDC_GRIPPER_ROLL, keys_text[object->link_gripper.joint_keys]) ;
                  SETc(IDC_GRIPPER_BASE, keys_text[object->link_gripper.link_keys]) ;

         for(i=0 ; i<_LINKS_MAX ; i++)
           if(L[i].use_flag==0) {
                                   HIDE(IDC_JOINT1+i) ;
                                   HIDE(IDC_LINK1 +i) ;
                                } 
           else                 {
                                   SHOW(IDC_JOINT1+i) ;
                                   SHOW(IDC_LINK1 +i) ;

                                   SETc(IDC_JOINT1+i, keys_text[L[i].joint_keys]) ;
                                   SETc(IDC_LINK1+i,  keys_text[L[i].link_keys]) ;
                               } 
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

                  SETc(IDC_BASE_JOINT,   keys_text[1]);
                  SETc(IDC_LEG_HEIGHT,   keys_text[0]) ;
        
                  SETc(IDC_WRIST_SWIVEL, keys_text[2]) ;
                  SETc(IDC_WRIST_LENGTH, keys_text[0]) ;

                  SETc(IDC_GRIPPER_ROLL, keys_text[3]) ;
                  SETc(IDC_GRIPPER_BASE, keys_text[0]) ;

         for(j=5, i=0 ; i<_LINKS_MAX && j<9 ; i++)
           if(L[i].use_flag) {
              if(L[i].joint_fixed) SETc(IDC_JOINT1+i, keys_text[0]) ;
              else                 SETc(IDC_JOINT1+i, keys_text[j++]) ;
              if(L[i].link_fixed ) SETc(IDC_LINK1+i,  keys_text[0]) ;
              else                 SETc(IDC_LINK1+i,  keys_text[j++]) ;
                             } 

                              return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                              GETc(IDC_BASE_JOINT, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_leg.joint_keys=j ;

                              GETc(IDC_LEG_HEIGHT, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_leg.link_keys=j ;

                              GETc(IDC_WRIST_SWIVEL, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_wrist.joint_keys=j ;

                              GETc(IDC_WRIST_LENGTH, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_wrist.link_keys=j ;

                              GETc(IDC_GRIPPER_ROLL, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_gripper.joint_keys=j ;

                              GETc(IDC_GRIPPER_BASE, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  object->link_gripper.link_keys=j ;

       for(i=0 ; i<_LINKS_MAX ; i++) {
                              GETc(IDC_JOINT1+i, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  L[i].joint_keys=j ;

                              GETc(IDC_LINK1+i, dgt) ;
           for(j=0 ; keys_text[j] ; j++)
             if(!strcmp(dgt, keys_text[j]))  L[i].link_keys=j ;
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

#undef   L

    return(TRUE) ;
}


/*********************************************************************/
/*                                                                   */
/*                 Окно структуры манипулятора                       */

 LRESULT CALLBACK  Object_Pivot_Structure_prc(  HWND  hWnd,     UINT  Msg,
 			                      WPARAM  wParam, LPARAM  lParam)
{
               RSS_Kernel *Module ;
 static  BITMAPINFOHEADER *Base_bmp ;
 static  BITMAPINFOHEADER *LinkA_bmp ;
 static  BITMAPINFOHEADER *LinkP_bmp ;
 static  BITMAPINFOHEADER *JointA_bmp ;
 static  BITMAPINFOHEADER *JointP_bmp ;
 static  BITMAPINFOHEADER *JointA1_bmp ;
 static  BITMAPINFOHEADER *JointP1_bmp ;
 static  BITMAPINFOHEADER *Gripper_bmp ;
                  HBITMAP  hBase ;
                  HBITMAP  hLinkA ;
                  HBITMAP  hLinkP ;
                  HBITMAP  hJointA ;
                  HBITMAP  hJointP ;
                  HBITMAP  hJointA1 ;
                  HBITMAP  hJointP1 ;
                  HBITMAP  hGripper ;
                      HDC  hDC ;
                      HDC  hBmpDC; 
                   HBRUSH  hBackBrush ;
                      int  x ;
                      int  y ;
                      int  x_mouse ;
                      int  y_mouse ;
                     RECT  WndRect ;
              PAINTSTRUCT  PaintCfg ;
                      int  j_flag ;
                      int  n ;
                      int  i ;
                      int  j ;

    static    struct {  int  joint ;
                        int  link ;  }  scheme[32] ;       /* Структура схемы */
    static                         int  scheme_cnt ;
    static                        char  scheme_desc[64] ;  /* Описание схемы */

/*---------------------------------------------- Загрузка фрагментов */

  if(Base_bmp==NULL) {
     
          Module=           GetObjectPivotEntry() ;
        Base_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_BASE", RT_BITMAP) ;
       LinkA_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_LINK_A", RT_BITMAP) ;
       LinkP_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_LINK_P", RT_BITMAP) ;
      JointA_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_JOINT_A", RT_BITMAP) ;
      JointP_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_JOINT_P", RT_BITMAP) ;
     JointA1_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_JOINT_A1", RT_BITMAP) ;
     JointP1_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_JOINT_P1", RT_BITMAP) ;
     Gripper_bmp=(BITMAPINFOHEADER *)Module->Resource("IDB_GRIPPER", RT_BITMAP) ;
                     }
/*--------------------------------------------------- Общая разводка */

  switch(Msg) {

/*----------------------------------------------- Основные сообщения */

    case WM_CREATE:   break ;

    case WM_COMMAND:  break ;

/*------------------------------------------------- Получение данных */

    case WM_SETTEXT: {

           memset(scheme_desc, 0, sizeof(scheme_desc)) ;
          strncpy(scheme_desc, (char *)lParam, sizeof(scheme_desc)-1) ;
                        break ;
                     }
/*---------------------------------------------------- Выдача данных */

    case WM_GETTEXT: {
                                     scheme_desc[0]=0 ;

       for(i=0 ; i<scheme_cnt && 
                 i<_LINKS_MAX   ; i++) {
         if(scheme[i].joint)  strcat(scheme_desc, "a") ;
         else                 strcat(scheme_desc, "p") ;

         if(scheme[i].link )  strcat(scheme_desc, "a") ;
         else                 strcat(scheme_desc, "p") ;
                                       }

           strcpy((char *)lParam, scheme_desc) ;
                          break ;
                     }
/*-------------------------------------------------------- Отрисовка */

    case WM_PAINT:   {

              GetClientRect(hWnd, &WndRect) ;
             InvalidateRect(hWnd, NULL, false) ;

             hDC=BeginPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Заливка окна */
            hBackBrush=CreateSolidBrush(RGB(255, 255, 255)) ;

                               FillRect(hDC, &WndRect, hBackBrush) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Подготовка BITMAP */
             hBmpDC=CreateCompatibleDC(hDC); 

              hBase=CREATE_BITMAP(hBmpDC, Base_bmp) ;
             hLinkA=CREATE_BITMAP(hBmpDC, LinkA_bmp) ;
             hLinkP=CREATE_BITMAP(hBmpDC, LinkP_bmp) ;
            hJointA=CREATE_BITMAP(hBmpDC, JointA_bmp) ;
            hJointP=CREATE_BITMAP(hBmpDC, JointP_bmp) ;
           hJointA1=CREATE_BITMAP(hBmpDC, JointA1_bmp) ;
           hJointP1=CREATE_BITMAP(hBmpDC, JointP1_bmp) ;
           hGripper=CREATE_BITMAP(hBmpDC, Gripper_bmp) ;
/*- - - - - - - - - - - - - - - - - - - - - - - Расшифровка описания */
           for(i=0 ; scheme_desc[i] ; i++)                          /* Нормализация описания */
                  if(scheme_desc[i]=='j' ||
                     scheme_desc[i]=='J' || 
                     scheme_desc[i]=='o' ||
                     scheme_desc[i]=='O'   )  scheme_desc[i]='j' ;
             else if(scheme_desc[i]=='s' ||
                     scheme_desc[i]=='S'   )  scheme_desc[i]='s' ;
             else if(scheme_desc[i]=='a' ||
                     scheme_desc[i]=='A'   )  scheme_desc[i]='a' ;
             else if(scheme_desc[i]=='p' ||
                     scheme_desc[i]=='P'   )  scheme_desc[i]='p' ;
             else                             scheme_desc[i]='l' ;

                memset(scheme, 0, sizeof(scheme)) ;                 /* Очистка схемы */

           for(scheme_cnt=-1, i=0 ; scheme_desc[i] ; i++) {         /* Построение схемы */
              
             if(scheme_desc[i]=='a') {
                                       scheme_cnt=i/2 ;
                       if(i%2)  scheme[scheme_cnt].link =1 ;
                       else     scheme[scheme_cnt].joint=1 ;
                                     }
             if(scheme_desc[i]=='p') {
                                       scheme_cnt=i/2 ;
                       if(i%2)  scheme[scheme_cnt].link =0 ;
                       else     scheme[scheme_cnt].joint=0 ;
                                     }
             if(scheme_desc[i]=='s') {
                                        scheme_cnt++ ;
                                     }
             if(scheme_desc[i]=='j') {
                                        scheme_cnt++ ;
                                 scheme[scheme_cnt].joint=1 ;
                                     } 
             if(scheme_desc[i]=='l') {
                 if(i==0 || scheme_desc[i-1]!='j')  scheme_cnt++ ;
                        scheme[scheme_cnt].link=1 ;
                                     } 
                                                          }

                        scheme_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Отрисовка базы */
                      y=WndRect.bottom-Base_bmp->biHeight-1 ;

              SelectObject(hBmpDC, hBase) ; 
                StretchBlt(   hDC, 0, y, Base_bmp->biWidth, 
                                         Base_bmp->biHeight,
                           hBmpDC, 0, 0, Base_bmp->biWidth, 
                                         Base_bmp->biHeight, SRCCOPY) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка звеньев */
                  x=Base_bmp->biWidth/2-JointA_bmp->biWidth/2 ;
 
           for(i=0, j=1 ; i<scheme_cnt ; i++, j=-j) {

            if(i==0) {
             if(scheme[i].joint)  SelectObject(hBmpDC, hJointA1) ;
             else                 SelectObject(hBmpDC, hJointP1) ;
                     }
            else     {
             if(scheme[i].joint)  SelectObject(hBmpDC, hJointA) ;
             else                 SelectObject(hBmpDC, hJointP) ;
                     }

                                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
                          if(j>0)  y-=JointA_bmp->biHeight ;
                          if(j<0)  y-=JointA_bmp->biHeight+LinkA_bmp->biHeight ;

                StretchBlt(   hDC, x, y, JointA_bmp->biWidth, j*JointA_bmp->biHeight,
                           hBmpDC, 0, 0, JointA_bmp->biWidth,   JointA_bmp->biHeight, SRCCOPY) ;

             if(scheme[i].link)   SelectObject(hBmpDC, hLinkA) ;
             else                 SelectObject(hBmpDC, hLinkP) ;

                                   x+=JointA_bmp->biWidth ;
                                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
                          if(j>0)  y-=JointA_bmp->biHeight+LinkA_bmp->biHeight ;
                          if(j<0)  y-=JointA_bmp->biHeight ;
              
                StretchBlt(   hDC, x, y, LinkA_bmp->biWidth, j*LinkA_bmp->biHeight,
                           hBmpDC, 0, 0, LinkA_bmp->biWidth,   LinkA_bmp->biHeight, SRCCOPY) ;


                                  x+=LinkA_bmp->biWidth ;
                                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Отрисовка захвата */
              SelectObject(hBmpDC, hGripper) ;

                                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
                          if(j>0)  y-=JointA_bmp->biHeight ;
                          if(j<0)  y-=JointA_bmp->biHeight+LinkA_bmp->biHeight ;
                                   y-=Gripper_bmp->biHeight/2 ;
                StretchBlt(   hDC, x, y, Gripper_bmp->biWidth, 
                                         Gripper_bmp->biHeight,
                           hBmpDC, 0, 0, Gripper_bmp->biWidth, 
                                         Gripper_bmp->biHeight, SRCCOPY) ;

               ValidateRect(hWnd,  NULL) ;
                   EndPaint(hWnd, &PaintCfg) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			     break ;
		     }
/*------------------------------------------------ Модификация схемы */

    case WM_LBUTTONDOWN: {
                                x_mouse=LOWORD(lParam) ; 
                                y_mouse=HIWORD(lParam) ; 

              GetClientRect(hWnd, &WndRect) ;
/*- - - - - - - - - - - - - - - - -  Определение активируемого звена */
                   n=-1 ;

                   y=WndRect.bottom ;
                   x= 0 ;
       if(x_mouse>=x && x_mouse<=x+Base_bmp->biWidth -1 &&          /* Проверка базы */
          y_mouse<=y && y_mouse>=y-Base_bmp->biHeight+1   )  n=101 ;

                   x=Base_bmp->biWidth/2-JointA_bmp->biWidth/2 ;
 
      for(i=0, j=1 ; i<scheme_cnt ; i++, j=-j) {                    /* CIRCLE.1 - Перебор звеньев/шарниров */

                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
//        if(j>0)  y-=0;
          if(j<0)  y-=JointA_bmp->biHeight+LinkA_bmp->biHeight ;

       if(x_mouse>=x && x_mouse<=x+JointA_bmp->biWidth -1 &&        /* Проверка шарнира */
          y_mouse<=y && y_mouse>=y-JointA_bmp->biHeight+1   ) {
                                  n=i ;
                             j_flag=1 ;
                                                              }

                   x+=JointA_bmp->biWidth ;
                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
                   y-=JointA_bmp->biHeight ;
              
       if(x_mouse>=x && x_mouse<=x+LinkA_bmp->biWidth -1 &&        /* Проверка звена */
          y_mouse<=y && y_mouse>=y-LinkA_bmp->biHeight+1   ) {
                                  n=i ;
                             j_flag=0 ;
                                                             }

                   x+=LinkA_bmp->biWidth ;
                                               }                    /* CONTINUE.1 */

                   y =WndRect.bottom-Base_bmp->biHeight-1 ;
          if(j>0)  y-=JointA_bmp->biHeight ;
          if(j<0)  y-=JointA_bmp->biHeight+LinkA_bmp->biHeight ;
                   y+=Gripper_bmp->biHeight/2 ;

       if(x_mouse>=x && x_mouse<=x+Gripper_bmp->biWidth -1 &&       /* Проверка захвата */
          y_mouse<=y && y_mouse>=y-Gripper_bmp->biHeight+1   )  n=102 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Добавление звена */
   if(n==102) {
                scheme[scheme_cnt].joint=0 ;
                scheme[scheme_cnt].link =0 ;
                       scheme_cnt++ ;
              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Удаление звена */
   if(n==101) {
                   if(scheme_cnt>1)  scheme_cnt-- ;
              }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Модификация звена */
   if(n>=   0      && 
      n< scheme_cnt  ) {

                if(j_flag)  scheme[n].joint=!scheme[n].joint ;
                else        scheme[n].link =!scheme[n].link ;         
                       }
/*- - - - - - - - - - - - - - - - - - - -  Построение описания схемы */
                                     scheme_desc[0]=0 ;

       for(i=0 ; i<scheme_cnt && 
                 i<_LINKS_MAX   ; i++) {
         if(scheme[i].joint)  strcat(scheme_desc, "a") ;
         else                 strcat(scheme_desc, "p") ;

         if(scheme[i].link )  strcat(scheme_desc, "a") ;
         else                 strcat(scheme_desc, "p") ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Перерисовка схемы */
                 SendMessage(hWnd, WM_PAINT, NULL, NULL) ;      
/*- - - - - - - - - - - - - - - - - - - - - - Сигнал изменения схемы */
                SendMessage(GetParent(hWnd), WM_USER, _USER_SCHEME_CHANGE, 0) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			         break ;
                        }
/*------------------------------------------------- Прочие сообщения */

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
/*								     */
/*	          Новыe обработчики элементов ListBox                */    

  LRESULT CALLBACK  OP_BodyList_WndProc(  HWND  hWnd,     UINT  Msg,
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


  LRESULT CALLBACK  OP_MassList_WndProc(  HWND  hWnd,     UINT  Msg,
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


/*********************************************************************/
/*								     */
/*         Обработчик сообщений диалогового окна AMBIGUITY           */

    BOOL CALLBACK  Object_Pivot_Ambiguity_dialog(  HWND hDlg,     UINT Msg,
 		  	                         WPARAM wParam, LPARAM lParam)
{
 static RSS_Object_Pivot *object ;
                     int  elm ;              /* Идентификатор элемента диалога */
                     int  status ;
     
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object_Pivot *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                SETx(IDC_WRIST, object->wrist_ambiguity) ;
                SETx(IDC_ARM,   object->arm_ambiguity) ;

            if(object->ambiguity_cnt<2)  HIDE(IDC_ARM) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                         status=HIWORD(wParam) ;
	                    elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Выбор модели */
     if(elm==IDC_DONE) {

                object->wrist_ambiguity=GETx(IDC_WRIST) ;
                object->arm_ambiguity  =GETx(IDC_ARM) ;

                            EndDialog(hDlg, 0) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {

                object->wrist_ambiguity=GETx(IDC_WRIST) ;
                object->arm_ambiguity  =GETx(IDC_ARM) ;

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


