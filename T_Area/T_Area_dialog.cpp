/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ПОВЕРХНОСТИ"    		    */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "d:\_Projects\_Libraries\Controls.h"
#include "d:\_Projects\_Libraries\UserDlg.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Area.h"

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

  static union WndProc_par  List_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Area_List_prc(HWND, UINT, WPARAM, LPARAM) ;

/*---------------------------------------------------- Прототипы п/п */

   char *Task_ptr_incode(void *) ;
   void *Task_ptr_decode(char *) ;


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Task_Area_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
   RSS_Module_Area  Module ;
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
/*       Обработчик сообщений диалогового окна INDICATORS LIST       */	

    BOOL CALLBACK  Task_Area_IList_dialog(  HWND  hDlg,     UINT  Msg, 
 			                  WPARAM  wParam, LPARAM  lParam) 
{
                static  HWND   hActive ;        /* Ссылка на активное окно */
     static  RSS_Module_Area  *Module ;
                         int   elm ;            /* Идентификатор элемента диалога */
                         int   status ;
   RSS_Module_Area_Indicator  *ind_cur ;
                         int   index ;
                        char   text[1024] ;
                         int   i ; 

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

                Module=(RSS_Module_Area *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - Регистрация активного окна */
         if(hActive!=NULL) {
                               EndDialog(hActive, 0) ;
                           }

                                hActive=hDlg ;
/*- - - - - - - - - - - - - - - - -  Инициализация значеий элементов */
       for(i=0 ; i<_IND_MAX ; i++) {

         if(Module->mIndicators[i]==NULL)  continue ;

                       sprintf(text, "%03d: %s", i, Module->mIndicators[i]->name) ;
             index=LB_ADD_ROW (IDC_LIST, text) ;
                   LB_SET_ITEM(IDC_LIST, index, Module->mIndicators[i]) ;
                                   }
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
           List_WndProc.par=GetWindowLong(ITEM(IDC_LIST), GWL_WNDPROC) ;

                Tmp_WndProc.call=Task_Area_List_prc ;
          SetWindowLong(ITEM(IDC_LIST),
                         GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - -  Определение рабочих позиций */
               ind_cur=(RSS_Module_Area_Indicator *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Добавить индикатор */
     if(wParam==_TASK_AREA_ADD_INDICATOR) {

       for(i=0 ; i<_IND_MAX ; i++) {

         if(Module->mIndicators[i]!=ind_cur)  continue ;

                       sprintf(text, "%03d: %s", i, Module->mIndicators[i]->name) ;
             index=LB_ADD_ROW (IDC_LIST, text) ;
                   LB_SET_ITEM(IDC_LIST, index, Module->mIndicators[i]) ;

                                          break ;
                                   }

			            return(FALSE) ;

                                           }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Удалить индикатор */
     if(wParam==_TASK_AREA_KILL_INDICATOR) {

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
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                status=HIWORD(wParam) ;
	           elm=LOWORD(wParam) ;

               ind_cur=(RSS_Module_Area_Indicator *)LB_GET_ITEM(IDC_LIST) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Список индикаторов */
       if(elm==IDC_LIST) {

        if(status==LBN_DBLCLK) {
                                  Module->iIndicatorShow(ind_cur) ;
                                   Module->kernel->vShow("REFRESH") ;
                               }

			            return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Удалить индикатор */
       if(elm==IDC_IND_DEL) {

             Module->iIndicatorKill(ind_cur) ;
              Module->kernel->vShow("REFRESH") ;
                         LB_DEL_ROW(IDC_LIST, LB_GET_ROW(IDC_LIST)) ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - -   Показать индикатор */
       if(elm==IDC_IND_ALL  ||
          elm==IDC_IND_SCENE  ) {
                                  Module->iIndicatorShow(ind_cur) ;
                                   Module->kernel->vShow("REFRESH") ;

  			               return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Скрыть индикатор */
       if(elm==IDC_IND_HIDE) {

             Module->iIndicatorHide(ind_cur) ;
              Module->kernel->vShow("REFRESH") ;

			            return(FALSE) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                           EndDialog(hDlg, 0) ;
                    Module->mIndListWnd=NULL ;
                                hActive=NULL ;

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
/*         Обработчик сообщений диалогового окна ANGLES              */

    BOOL CALLBACK  Task_Area_Angles_dialog(  HWND hDlg,     UINT Msg,
 		  	                     WPARAM wParam, LPARAM lParam)
{
 static             UD_color_panel  colors[_ANGLES_LIST_MAX] ;  /* Таблица цветов */
 static  RSS_Module_Area_Indicator *Indicator ;
             RSS_Module_Area_Range *range ;
                               int  elm ;                       /* Идентификатор элемента диалога */
                               int  status ;
                               int  elems[32] ;
                              char  v1[1024] ;
                              char  v2[1024] ;
                               int  flag_1 ;
                               int  flag_2 ;
                              char  tmp[1024] ;
                              char *end ;
                               int  i ;
                               int  j ;
                               int  k ;
     
                      static  char *Set_Values[]={"List", "Or", "And", NULL} ;
                      static  char *Base_Values[]={"Absolute", "Normal", NULL} ;
                      static  char *Order_Values[]={"A->E->R", "A->R->E", "E->A->R",
                                                    "E->R->A", "R->A->E", "R->E->A", NULL} ;

#define  I   Indicator

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              Indicator=(RSS_Module_Area_Indicator *)lParam ;
/*- - - - - - - - - - - - - - - - - - -  Инициализация списка цветов */
      for(i=0 ; i<_ANGLES_LIST_MAX ; i++) {

                colors[i].color      = I->angles[i].color ;
                colors[i].choice_proc=Task_Area_color_choice ;
                                          }
/*- - - - - - - - - - - - - - - - - - - -  Загрузка списков значений */
      for(i=0 ; i<4 ; i++) {
                    CB_CLEAR(IDC_AZIM_SET_1+i*100) ;
                 CB_ADD_LIST(IDC_AZIM_SET_1+i*100, Set_Values[0]) ;
                 CB_ADD_LIST(IDC_AZIM_SET_1+i*100, Set_Values[1]) ;
                 CB_ADD_LIST(IDC_AZIM_SET_1+i*100, Set_Values[2]) ;

                    CB_CLEAR(IDC_ELEV_SET_1+i*100) ;
                 CB_ADD_LIST(IDC_ELEV_SET_1+i*100, Set_Values[0]) ;
                 CB_ADD_LIST(IDC_ELEV_SET_1+i*100, Set_Values[1]) ;
                 CB_ADD_LIST(IDC_ELEV_SET_1+i*100, Set_Values[2]) ;

                    CB_CLEAR(IDC_ROLL_SET_1+i*100) ;
                 CB_ADD_LIST(IDC_ROLL_SET_1+i*100, Set_Values[0]) ;
                 CB_ADD_LIST(IDC_ROLL_SET_1+i*100, Set_Values[1]) ;
                 CB_ADD_LIST(IDC_ROLL_SET_1+i*100, Set_Values[2]) ;

                    CB_CLEAR(IDC_ORDER_1+i*100) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[0]) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[1]) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[2]) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[3]) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[4]) ;
                 CB_ADD_LIST(IDC_ORDER_1+i*100, Order_Values[5]) ;

                    CB_CLEAR(IDC_AZIM_BASE_1+i*100) ;
                 CB_ADD_LIST(IDC_AZIM_BASE_1+i*100, Base_Values[0]) ;
                 CB_ADD_LIST(IDC_AZIM_BASE_1+i*100, Base_Values[1]) ;

                    CB_CLEAR(IDC_ELEV_BASE_1+i*100) ;
                 CB_ADD_LIST(IDC_ELEV_BASE_1+i*100, Base_Values[0]) ;
                 CB_ADD_LIST(IDC_ELEV_BASE_1+i*100, Base_Values[1]) ;
                           }
/*- - - - - - - - - - - - - - - Конфигурация видимости и доступности */
      for(i=0 ; i<4 ; i++) {

        if(!I->angles[i].use_flag)
         for(j=IDC_AZIM_SET_1 ; j<=IDC_PRIORITY_1 ; j++)  DISABLE(j+i*100) ;

        if(I->angles[i].azim_range.type==_INLIST_RANGE) {
                       SHOW(IDC_AZIM_LIST_1+i*100) ;
                       HIDE(IDC_AZIM_MIN_1+i*100) ;
                       HIDE(IDC_AZIM_MAX_1+i*100) ;
                                                        }
        else                                            {
                       HIDE(IDC_AZIM_LIST_1+i*100) ;
                       SHOW(IDC_AZIM_MIN_1+i*100) ;
                       SHOW(IDC_AZIM_MAX_1+i*100) ;
                                                        }

        if(I->angles[i].elev_range.type==_INLIST_RANGE) {
                       SHOW(IDC_ELEV_LIST_1+i*100) ;
                       HIDE(IDC_ELEV_MIN_1+i*100) ;
                       HIDE(IDC_ELEV_MAX_1+i*100) ;
                                                        }
        else                                            {
                       HIDE(IDC_ELEV_LIST_1+i*100) ;
                       SHOW(IDC_ELEV_MIN_1+i*100) ;
                       SHOW(IDC_ELEV_MAX_1+i*100) ;
                                                        }
        if(I->angles[i].roll_range.type==_INLIST_RANGE) {
                       SHOW(IDC_ROLL_LIST_1+i*100) ;
                       HIDE(IDC_ROLL_MIN_1+i*100) ;
                       HIDE(IDC_ROLL_MAX_1+i*100) ;
                                                        }
        else                                            {
                       HIDE(IDC_ROLL_LIST_1+i*100) ;
                       SHOW(IDC_ROLL_MIN_1+i*100) ;
                       SHOW(IDC_ROLL_MAX_1+i*100) ;
                                                        }
                           }
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
      for(i=0 ; i<4 ; i++) {

        SETx(IDC_USE_1+i*100, I->angles[i].use_flag) ;

        SendMessage(ITEM(IDC_COLOR_1+i*100), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[i])) ;

        SETc(IDC_AZIM_SET_1+i*100, Set_Values[I->angles[i].azim_range.type]) ;
        SETc(IDC_ELEV_SET_1+i*100, Set_Values[I->angles[i].elev_range.type]) ;
        SETc(IDC_ROLL_SET_1+i*100, Set_Values[I->angles[i].roll_range.type]) ;

        SETc(IDC_AZIM_BASE_1+i*100, Base_Values[I->angles[i].azim_range.base]) ;
        SETc(IDC_ELEV_BASE_1+i*100, Base_Values[I->angles[i].elev_range.base]) ;

        SETc(IDC_ORDER_1+i*100, Order_Values[I->angles[i].priority_type]) ;

        SETs(IDC_AZIM_LIST_1+i*100, I->angles[i].azim_range.a_lst_text) ;
        SETs(IDC_AZIM_MIN_1+i*100,  I->angles[i].azim_range.a_min_text) ;
        SETs(IDC_AZIM_MAX_1+i*100,  I->angles[i].azim_range.a_max_text) ;

        SETs(IDC_ELEV_LIST_1+i*100, I->angles[i].elev_range.a_lst_text) ;
        SETs(IDC_ELEV_MIN_1+i*100,  I->angles[i].elev_range.a_min_text) ;
        SETs(IDC_ELEV_MAX_1+i*100,  I->angles[i].elev_range.a_max_text) ;

        SETs(IDC_ROLL_LIST_1+i*100, I->angles[i].roll_range.a_lst_text) ;
        SETs(IDC_ROLL_MIN_1+i*100,  I->angles[i].roll_range.a_min_text) ;
        SETs(IDC_ROLL_MAX_1+i*100,  I->angles[i].roll_range.a_max_text) ;
                           }

         sprintf(                tmp, "%lf", I->angle_step) ;
            SETs(IDC_ANGLE_STEP, tmp) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

                         status=HIWORD(wParam) ;
	                    elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Активация слота */
     if(elm==IDC_USE_1 ||
        elm==IDC_USE_2 ||
        elm==IDC_USE_3 ||
        elm==IDC_USE_4   ) {

               status=GETx(elm) ;
                    i=(elm-IDC_USE_1)/100 ;

         for(j=IDC_AZIM_SET_1 ; j<=IDC_PRIORITY_1 ; j++)
           if(status)  ENABLE(j+i*100) ;
           else       DISABLE(j+i*100) ;

                           }
/*- - - - - - - - - - - - - - - - Изменение вида значения по азимуту */
     if(elm==IDC_AZIM_SET_1 ||
        elm==IDC_AZIM_SET_2 ||
        elm==IDC_AZIM_SET_3 ||
        elm==IDC_AZIM_SET_4   ) {
                                    GETc(elm, tmp) ;

           for(status=0 ; Set_Values[status] ; status++)
             if(!strcmp(tmp, Set_Values[status]))  break ;

                    i=(elm-IDC_AZIM_SET_1)/100 ;
              
             if(status==_INLIST_RANGE) {  SHOW(IDC_AZIM_LIST_1+i*100) ;
                                          HIDE(IDC_AZIM_MIN_1+i*100) ;
                                          HIDE(IDC_AZIM_MAX_1+i*100) ;   }
        else                           {  HIDE(IDC_AZIM_LIST_1+i*100) ;
                                          SHOW(IDC_AZIM_MIN_1+i*100) ;
                                          SHOW(IDC_AZIM_MAX_1+i*100) ;   }               
                                }   
/*- - - - - - - - - - - - Изменение вида значения по углу возвышения */
     if(elm==IDC_ELEV_SET_1 ||
        elm==IDC_ELEV_SET_2 ||
        elm==IDC_ELEV_SET_3 ||
        elm==IDC_ELEV_SET_4   ) {
                                    GETc(elm, tmp) ;

           for(status=0 ; Set_Values[status] ; status++)
             if(!strcmp(tmp, Set_Values[status]))  break ;

                    i=(elm-IDC_ELEV_SET_1)/100 ;
              
             if(status==_INLIST_RANGE) {  SHOW(IDC_ELEV_LIST_1+i*100) ;
                                          HIDE(IDC_ELEV_MIN_1+i*100) ;
                                          HIDE(IDC_ELEV_MAX_1+i*100) ;   }
        else                           {  HIDE(IDC_ELEV_LIST_1+i*100) ;
                                          SHOW(IDC_ELEV_MIN_1+i*100) ;
                                          SHOW(IDC_ELEV_MAX_1+i*100) ;   }               
                                }   
/*- - - - - - - - - - - - - - - - - Изменение вида значения по крену */
     if(elm==IDC_ROLL_SET_1 ||
        elm==IDC_ROLL_SET_2 ||
        elm==IDC_ROLL_SET_3 ||
        elm==IDC_ROLL_SET_4   ) {
                                    GETc(elm, tmp) ;

           for(status=0 ; Set_Values[status] ; status++)
             if(!strcmp(tmp, Set_Values[status]))  break ;

                    i=(elm-IDC_ROLL_SET_1)/100 ;
              
             if(status==_INLIST_RANGE) {  SHOW(IDC_ROLL_LIST_1+i*100) ;
                                          HIDE(IDC_ROLL_MIN_1+i*100) ;
                                          HIDE(IDC_ROLL_MAX_1+i*100) ;   }
        else                           {  HIDE(IDC_ROLL_LIST_1+i*100) ;
                                          SHOW(IDC_ROLL_MIN_1+i*100) ;
                                          SHOW(IDC_ROLL_MAX_1+i*100) ;   }               
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Перемещение слота */
     if(elm==IDC_PRIORITY_2 ||
        elm==IDC_PRIORITY_3 ||
        elm==IDC_PRIORITY_4   ) {

                    i =(elm-IDC_PRIORITY_1)/100 ;
                    j = i-1 ;
                    i*= 100 ;
                    j*= 100 ;

              elems[0]=IDC_AZIM_SET_1 ;
              elems[1]=IDC_ELEV_SET_1 ;
              elems[2]=IDC_ROLL_SET_1 ;
              elems[3]=IDC_AZIM_BASE_1 ;
              elems[4]=IDC_ELEV_BASE_1 ;
              elems[5]=IDC_ORDER_1 ;

         for(k=0 ; k<6 ; k++) {
               GETc(elems[k]+i, v1) ;   GETc(elems[k]+j, v2) ;
               SETc(elems[k]+i, v2) ;   SETc(elems[k]+j, v1) ;
                              }

              elems[0]=IDC_AZIM_LIST_1 ;
              elems[1]=IDC_AZIM_MIN_1 ;
              elems[2]=IDC_AZIM_MAX_1 ;
              elems[3]=IDC_ELEV_LIST_1 ;
              elems[4]=IDC_ELEV_MIN_1 ;
              elems[5]=IDC_ELEV_MAX_1 ;
              elems[6]=IDC_ROLL_LIST_1 ;
              elems[7]=IDC_ROLL_MIN_1 ;
              elems[8]=IDC_ROLL_MAX_1 ;

         for(k=0 ; k<9 ; k++) {
               GETs(elems[k]+i, v1) ;  GETs(elems[k]+j, v2) ;
               SETs(elems[k]+i, v2) ;  SETs(elems[k]+j, v1) ;

                 flag_1=VISIBLE(elems[k]+i) ;
                 flag_2=VISIBLE(elems[k]+j) ;
              if(flag_2)   SHOW(elems[k]+i) ;
              else         HIDE(elems[k]+i) ;
              if(flag_1)   SHOW(elems[k]+j) ;
              else         HIDE(elems[k]+j) ;
                              }

               flag_1=GETx(IDC_USE_1+i) ;
               flag_2=GETx(IDC_USE_1+j) ;
                      SETx(IDC_USE_1+i, flag_2) ;
                      SETx(IDC_USE_1+j, flag_1) ;
           
         for(k=IDC_AZIM_SET_1 ; k<=IDC_PRIORITY_1 ; k++)  {
                                         if(flag_2)  ENABLE(k+i) ;
                                         else       DISABLE(k+i) ;

                                         if(flag_1)  ENABLE(k+j) ;
                                         else       DISABLE(k+j) ;
                                                          }
                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
/*- - - - - - - - - - - - - - - - - - - - - - -  Извлечение значений */
      for(i=0 ; i<4 ; i++) {

           I->angles[i].use_flag=GETx(IDC_USE_1+i*100) ;
           I->angles[i].color   =colors[i].color ;

        GETc(IDC_AZIM_SET_1+i*100, tmp) ;
         for(status=0 ; Set_Values[status] ; status++)
           if(!strcmp(tmp, Set_Values[status]))  I->angles[i].azim_range.type=status ;

        GETc(IDC_ELEV_SET_1+i*100, tmp) ;
         for(status=0 ; Set_Values[status] ; status++)
           if(!strcmp(tmp, Set_Values[status]))  I->angles[i].elev_range.type=status ;

        GETc(IDC_ROLL_SET_1+i*100, tmp) ;
         for(status=0 ; Set_Values[status] ; status++)
           if(!strcmp(tmp, Set_Values[status]))  I->angles[i].roll_range.type=status ;

        GETc(IDC_AZIM_BASE_1+i*100, tmp) ;
         for(status=0 ; Base_Values[status] ; status++)
           if(!strcmp(tmp, Base_Values[status]))  I->angles[i].azim_range.base=status ;

        GETc(IDC_ELEV_BASE_1+i*100, tmp) ;
         for(status=0 ; Base_Values[status] ; status++)
           if(!strcmp(tmp, Base_Values[status]))  I->angles[i].elev_range.base=status ;

        GETc(IDC_ORDER_1+i*100, tmp) ;
         for(status=0 ; Order_Values[status] ; status++)
           if(!strcmp(tmp, Order_Values[status]))  I->angles[i].priority_type=status ;

        GETs(IDC_AZIM_LIST_1+i*100, I->angles[i].azim_range.a_lst_text) ;
        GETs(IDC_AZIM_MIN_1+i*100,  I->angles[i].azim_range.a_min_text) ;
        GETs(IDC_AZIM_MAX_1+i*100,  I->angles[i].azim_range.a_max_text) ;

        GETs(IDC_ELEV_LIST_1+i*100, I->angles[i].elev_range.a_lst_text) ;
        GETs(IDC_ELEV_MIN_1+i*100,  I->angles[i].elev_range.a_min_text) ;
        GETs(IDC_ELEV_MAX_1+i*100,  I->angles[i].elev_range.a_max_text) ;

        GETs(IDC_ROLL_LIST_1+i*100, I->angles[i].roll_range.a_lst_text) ;
        GETs(IDC_ROLL_MIN_1+i*100,  I->angles[i].roll_range.a_min_text) ;
        GETs(IDC_ROLL_MAX_1+i*100,  I->angles[i].roll_range.a_max_text) ;
                           }

        GETs(IDC_ANGLE_STEP, tmp) ;
              I->angle_step=strtod(tmp, &end) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разборка значений */
      for(i=0 ; i<4 ; i++)
      for(k=0 ; k<3 ; k++) {

             if(k==0)  range=&(I->angles[i].azim_range) ;
        else if(k==1)  range=&(I->angles[i].elev_range) ;
        else           range=&(I->angles[i].roll_range) ;
 
               range->a_min=strtod(range->a_min_text, &end) ;
               range->a_max=strtod(range->a_max_text, &end) ;

           for(end=range->a_lst_text, j=0 ; 
                 *end!=0 && j<_ANGLES_MAX ; j++) {
                         range->a_lst[j]=strtod(end, &end) ;         
                     while(*end==',' || *end==' ')  end++ ;
                                                 }

                               range->a_lst_cnt=j ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - Завершение диалога */
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
/*	          Новыe обработчики элементов                        */    

  LRESULT CALLBACK  Task_Area_List_prc(  HWND  hWnd,     UINT  Msg,
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

      result=CallWindowProc(List_WndProc.call,
                                hWnd, Msg, wParam, lParam) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_SCENE, "Показать на сцене") ;
          AppendMenu(hMenu, MF_SEPARATOR,          0,      NULL            ) ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_HIDE,  "Убрать со сцены"  ) ;
          AppendMenu(hMenu, MF_SEPARATOR,          0,      NULL            ) ;
          AppendMenu(hMenu, MF_STRING,   IDC_IND_DEL,   "Удалить"          ) ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

         DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


/*********************************************************************/
/*                                                                   */
/*                Преобразование адреса данных в строку              */
/*                Преобразование строки в адрес данных               */

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


