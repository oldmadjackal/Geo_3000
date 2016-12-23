/********************************************************************/
/*								    */
/*      МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ДИАПАЗОНА ПАРАМЕТРОВ"                 */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "d:\_Projects\_Libraries\controls.h"
#include "d:\_Projects\_Libraries\UserDlg.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Scan.h"

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

  static union WndProc_par  Pars_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Scan_Pars_prc(HWND, UINT, WPARAM, LPARAM) ;

/*-------------------------------------------------------- Мнемоники */

/*---------------------------------------------------- Прототипы п/п */

  void  iTScan_Pars_format(ZONE_PAR *, char *) ;   /* Форматирование параметра в строку */


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Task_Scan_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Scan  Module ;
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

    BOOL CALLBACK  Task_Scan_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                   WPARAM wParam, LPARAM lParam) 
{
 static  UD_color_panel  colors[3] ;  /* Таблица цветов */
             RSS_Kernel *Module_ ;
 static RSS_Module_Scan *Module ;
               ZONE_PAR  parameter ;
                    int  elm ;         /* Идентификатор элемента диалога */
          static  HFONT  font ;          /* Шрифт */
                    int  status ;
                   char  text[1024] ;
                    int  n ;
                    int  i ;


/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Scan *)lParam ;
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
         SendMessage(ITEM(IDC_PARS_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
           Pars_WndProc.par=GetWindowLong(ITEM(IDC_PARS_LIST), GWL_WNDPROC) ;
           Tmp_WndProc.call=Task_Scan_Pars_prc ;
        SetWindowLong(ITEM(IDC_PARS_LIST), GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - -  Списки значений элементов */
             CB_CLEAR(IDC_GOOD_TYPE) ;
          CB_ADD_LIST(IDC_GOOD_TYPE, "Point") ;
          CB_ADD_LIST(IDC_GOOD_TYPE, "Spot") ;
             CB_CLEAR(IDC_BAD_TYPE) ;
          CB_ADD_LIST(IDC_BAD_TYPE, "Point") ;
          CB_ADD_LIST(IDC_BAD_TYPE, "Spot") ;
             CB_CLEAR(IDC_GOOD_PRIORITY) ;
          CB_ADD_LIST(IDC_GOOD_PRIORITY, "MIN") ;
          CB_ADD_LIST(IDC_GOOD_PRIORITY, "MAX") ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                colors[0].color      = Module->mBasics_marker[0].color_min ;
                colors[0].choice_proc=Task_Scan_color_choice ;
                colors[1].color      = Module->mBasics_marker[0].color_max ;
                colors[1].choice_proc=Task_Scan_color_choice ;
                colors[2].color      = Module->mBasics_marker[1].color_min ;
                colors[2].choice_proc=Task_Scan_color_choice ;

        SendMessage(ITEM(IDC_GOOD_COLOR_MIN), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[0])) ;
        SendMessage(ITEM(IDC_GOOD_COLOR_MAX), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[1])) ;
        SendMessage(ITEM(IDC_BAD_COLOR), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[2])) ;

                 SETs(IDC_SERVICE_TASK, Module->mCheck_cmd) ;
                 SETx(IDC_SHOW_BASE,    Module->mBasePoints_show) ;
                 SETx(IDC_REDRAW_BASE,  Module->mBasePoints_redraw) ;
                 SETc(IDC_GOOD_TYPE,    Module->mBasics_marker[0].model) ;
                 SETc(IDC_BAD_TYPE,     Module->mBasics_marker[1].model) ;
                 SETi(IDC_SIZE_FACTOR,  Module->mBasics_marker[0].size) ;

                         LB_CLEAR(IDC_PARS_LIST) ;
        for(i=0 ; i<Module->mScanPars_cnt ; i++) {                  /* Список параметров сканирования */
               iTScan_Pars_format(&Module->mScanPars[i], text) ;
                       LB_ADD_ROW( IDC_PARS_LIST, text) ;
                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;

/*- - - - - - - - - - - - - - - - - - - - - Редактирование параметра */
       if(   elm==IDC_PARS_LIST  &&
          status==LBN_DBLCLK       ) {

                 n=LB_GET_ROW(IDC_PARS_LIST) ;                      /* Определяем номер строки */

                     memset(&parameter, 0, sizeof(parameter)) ;     /* Готовим к параметр к редактированию */
                             parameter       =Module->mScanPars[n] ;
                             parameter.module=Module ;

                             Module_=GetTaskScanEntry() ;

           status=DialogBoxIndirectParam(GetModuleHandle(NULL),     /* Вызываем диалог редактирования */ 
                                         (LPCDLGTEMPLATE)Module_->Resource("IDD_PARAMETER", RT_DIALOG),
			                   GetActiveWindow(), Task_Scan_Param_dialog, 
                                           (LPARAM)&parameter) ;
         if(status!=0)  return(FALSE) ;                             /* Если редакция отменена */

                                   Module->mScanPars[n]=parameter ; /* Заносим новые данные */ 
               iTScan_Pars_format(&Module->mScanPars[n], text) ;    /* Формируем описание */ 
                       LB_DEL_ROW( IDC_PARS_LIST, n) ;              /* Заменяем описание в списке */
                       LB_INS_ROW( IDC_PARS_LIST, n, text) ;

                                            return(FALSE) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Добавить параметр */
       if(elm==IDC_PAR_ADD) {

         if(Module->mScanPars_cnt==_SCAN_PARS_MAX) {
                     SEND_ERROR("Слишком большое число параметров") ;
                                   return(FALSE) ;
                                                   }

                     memset(&parameter, 0, sizeof(parameter)) ;
                             parameter.module=Module ;

                             Module_=GetTaskScanEntry() ;

           status=DialogBoxIndirectParam(GetModuleHandle(NULL),
                                         (LPCDLGTEMPLATE)Module_->Resource("IDD_PARAMETER", RT_DIALOG),
			                   GetActiveWindow(), Task_Scan_Param_dialog, 
                                           (LPARAM)&parameter) ;
         if(status!=0)  return(FALSE) ;

                  Module->mScanPars[Module->mScanPars_cnt]=parameter ;
                                    Module->mScanPars_cnt++ ;

               iTScan_Pars_format(&parameter, text) ;
                       LB_ADD_ROW( IDC_PARS_LIST, text) ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Удалить параметр */
       if(elm==IDC_PAR_DEL) {

                 n=LB_GET_ROW(IDC_PARS_LIST) ;                      /* Определяем номер строки */
                   LB_DEL_ROW(IDC_PARS_LIST, n) ;                   /* Удаляем описание из экранного списка */

          for(i=n+1 ; i<Module->mScanPars_cnt ; i++)                /* Сдвигаем данные */
                  Module->mScanPars[i-1]=Module->mScanPars[i] ;

                  Module->mScanPars_cnt-- ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {

                                 GETs(IDC_SERVICE_TASK,  Module->mCheck_cmd) ;
      Module->mBasePoints_show  =GETx(IDC_SHOW_BASE) ;
      Module->mBasePoints_redraw=GETx(IDC_REDRAW_BASE) ;


                                 GETc(IDC_GOOD_TYPE, Module->mBasics_marker[0].model) ;
                                 GETc(IDC_BAD_TYPE,  Module->mBasics_marker[1].model) ;

       Module->mBasics_marker[0].color_min=colors[0].color ;
       Module->mBasics_marker[0].color_max=colors[1].color ;
       Module->mBasics_marker[1].color_min=colors[2].color ;
       Module->mBasics_marker[1].color_max=colors[2].color ;

         Module->mBasics_marker[0].size=GETi(IDC_SIZE_FACTOR) ;
      if(Module->mBasics_marker[0].size<=0)
         Module->mBasics_marker[0].size=50 ;

         Module->mBasics_marker[1].size=Module->mBasics_marker[0].size ;

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
/*          Обработчик сообщений диалогового окна PARAMETER          */

    BOOL CALLBACK  Task_Scan_Param_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Scan *Module ;
        static ZONE_PAR *parameter ;
  static RSS_ControlPar *ctrl_pars ;          /* Список управляемых параметров объектов */
  static            int  ctrl_pars_cnt ;
  static           char  ctrl_object[128] ;   /* Ссылка на управляемый объекта */
  static           char  ctrl_link[64] ;      /* Ссылка на управляемый параметр объекта */
                   char *list[1024] ;         /* Список значений */
                    int  list_cnt ;
                    int  elm ;                /* Идентификатор элемента диалога */
                    int  status ;
                   char  value[1024] ;
                   char  group[1024] ;
                   char *error ;
                    int  i ;
                    int  j ;

    static  char *category_list[2]={ "Переменная",
                                     "Параметр объекта"       } ;
    static  char *    kind_list[2]={ "Диапазонно-изменяемый",
                                     "Рассчетный"             } ;

#define  P parameter
#define  M Module

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

#define   RAISE_EVENT(elm, status)  SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(elm, status), 0)

/*------------------------------------------------------- Подготовка */

      if(ctrl_pars==NULL) {

         ctrl_pars=(RSS_ControlPar *)
                        calloc(1000, sizeof(RSS_ControlPar)) ;
                          }
/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                           parameter=(ZONE_PAR *)lParam ;
                              Module= parameter->module ;
/*- - - - - - - - - - - - - - - - - Инициализация  списков значенией */
                              CB_CLEAR(IDC_CATEGORY) ;
     for(i=0 ; i<2 ; i++)  CB_ADD_LIST(IDC_CATEGORY, category_list[i]) ;

                              CB_CLEAR(IDC_KIND) ;
     for(i=0 ; i<2 ; i++)  CB_ADD_LIST(IDC_KIND, kind_list[i]) ;

                              CB_CLEAR(IDC_OBJECT) ;
     for(i=0 ; i<OBJECTS_CNT ; i++)
       if(OBJECTS[i]->vListControlPars(NULL))
                           CB_ADD_LIST(IDC_OBJECT, OBJECTS[i]->Name) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
              SETs(IDC_NAME,                   P->name     ) ;
              SETc(IDC_CATEGORY, category_list[P->category]) ;
              SETc(IDC_KIND,         kind_list[P->kind    ]) ;

          if(P->category==_OBJECT_PAR) {                            /* Прописываем объект и управляемый параметр */
                        SETc(IDC_OBJECT, P->object) ;
                 RAISE_EVENT(IDC_OBJECT, CBN_SELCHANGE) ;
               
            for(i=0 ; i<ctrl_pars_cnt ; i++)                        
              if(!stricmp(P->par_link, ctrl_pars[i].link)) {
                        SETc(IDC_TYPE, ctrl_pars[i].section_name) ;
                 RAISE_EVENT(IDC_TYPE, CBN_SELCHANGE) ;
                        SETc(IDC_ELEMENT, ctrl_pars[i].parameter_name) ;
                 RAISE_EVENT(IDC_ELEMENT, CBN_SELCHANGE) ;
                                           break ;
                                                           }
                                       }

              SETs(IDC_VALUE,                  P->value.formula) ;
              SETs(IDC_VALUE_FROM,             P->value_min.formula) ;
              SETs(IDC_VALUE_TO,               P->value_max.formula) ;
              SETs(IDC_VALUE_STEP,             P->value_step.formula) ;
/*- - - - - - - - - - - - - - - - - - - - - - Расстановка блокировок */
          if(P->name[0]!=0)  DISABLE(IDC_NAME) ;                    /* Если имя не пустое - блокируем его */

          if(P->category== _VAR_PAR) {  DISABLE(IDC_OBJECT) ;       /* Для переменных - блокируем поля */ 
                                        DISABLE(IDC_TYPE) ;         /*  привязки объекта               */
                                        DISABLE(IDC_ELEMENT) ;  }

          if(P->kind    ==_SCAN_PAR) {  DISABLE(IDC_VALUE) ;      } /* Для диапазонных - блокируем поле    */
          else                       {  DISABLE(IDC_VALUE_FROM) ;   /*  формулы значения, для рассчетных - */
                                        DISABLE(IDC_VALUE_TO) ;     /*    "диапазонные" поля               */
                                        DISABLE(IDC_VALUE_STEP) ; }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Изменение категории */
     if(elm==IDC_CATEGORY && status==CBN_SELCHANGE) {
                                GETc(IDC_CATEGORY, value) ;

        if(!stricmp(category_list[0], value)) {                     /* Для переменных - блокируем поля */ 
                       DISABLE(IDC_OBJECT) ;                        /*  привязки объекта               */
                       DISABLE(IDC_TYPE) ;
                       DISABLE(IDC_ELEMENT) ;
                                              }
        else                                  { 
                        ENABLE(IDC_OBJECT) ;
                        ENABLE(IDC_TYPE) ;
                        ENABLE(IDC_ELEMENT) ;
                                              }

                               return(FALSE) ;
                                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Изменение вида */
     if(elm==IDC_KIND && status==CBN_SELCHANGE) {

                              GETc(IDC_KIND, value) ;

          if(!stricmp(kind_list[0], value)) {                       /* Для диапазонных - блокируем поле    */
                  DISABLE(IDC_VALUE) ;                              /*  формулы значения, для рассчетных - */
                   ENABLE(IDC_VALUE_FROM) ;                         /*    "диапазонные" поля               */
                   ENABLE(IDC_VALUE_TO) ;     
                   ENABLE(IDC_VALUE_STEP) ;  
                                            }
          else                              {
                   ENABLE(IDC_VALUE) ;
                  DISABLE(IDC_VALUE_FROM) ;
                  DISABLE(IDC_VALUE_TO) ;
                  DISABLE(IDC_VALUE_STEP) ;
                                            }

                            return(FALSE) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Выбор объекта */
     if(elm==IDC_OBJECT && status==CBN_SELCHANGE) {

                                  *ctrl_link=0 ;                    /* Сброс текущей ссылки */

                                GETc(IDC_OBJECT, value) ;

                            CB_CLEAR(IDC_TYPE) ;
                            CB_CLEAR(IDC_ELEMENT) ;

        for(i=0 ; i<OBJECTS_CNT ; i++)
          if(!stricmp(value, OBJECTS[i]->Name))  break ;

          if(i>=OBJECTS_CNT) {
                                SETc(IDC_OBJECT, "???") ;
                          SEND_ERROR("Несуществующий объект") ;
  			       return(FALSE) ;
                             }

             ctrl_pars_cnt=OBJECTS[i]->vListControlPars(ctrl_pars); /* Запрашиваем список изменяемых параметров */
          if(ctrl_pars_cnt<=0) {
                      SETc(IDC_OBJECT, "???") ;
                SEND_ERROR("Объект не имеет управляемых параметров") ;
  			            return(FALSE) ;
                               }

              for(list_cnt=0, i=0 ; i<ctrl_pars_cnt ; i++) {        /* Формируем список секций */
                for(j=0 ; j<list_cnt ; j++)
                  if(!stricmp(list[j], ctrl_pars[i].section_name)) break ;

                  if(j>=list_cnt) {  list[list_cnt]=ctrl_pars[i].section_name ;
                                          list_cnt++ ;                          }
                                                           } 

               for(j=0 ; j<list_cnt ; j++)                          /* Вводим список секций в список значений */
                             CB_ADD_LIST(IDC_TYPE, list[j]) ;

                              return(FALSE) ;
                                                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Выбор типа элемента */
     if(elm==IDC_TYPE && status==CBN_SELCHANGE) {

                                  *ctrl_link=0 ;                    /* Сброс текущей ссылки */

                                GETc(IDC_TYPE, value) ;
                            CB_CLEAR(IDC_ELEMENT) ;

            for(list_cnt=0, i=0 ; i<ctrl_pars_cnt ; i++)            /* Формируем список элементов */
              if(!stricmp(value, ctrl_pars[i].section_name)) {  
                for(j=0 ; j<list_cnt ; j++)
                  if(!stricmp(list[j], ctrl_pars[i].parameter_name)) break ;

                  if(j>=list_cnt) {  list[list_cnt]=ctrl_pars[i].parameter_name ;
                                          list_cnt++ ;                            }
                                                             } 

               for(j=0 ; j<list_cnt ; j++)                          /* Вводим список элементов в список значений */
                             CB_ADD_LIST(IDC_ELEMENT, list[j]) ;

                              return(FALSE) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Выбор элемента */
     if(elm==IDC_ELEMENT && status==CBN_SELCHANGE) {

                                GETc(IDC_OBJECT,  ctrl_object) ;
                                GETc(IDC_TYPE,    group) ;
                                GETc(IDC_ELEMENT, value) ;

            for(i=0 ; i<ctrl_pars_cnt ; i++)                        /* Идентифицируем элемент */
              if(!stricmp(group, ctrl_pars[i].section_name  ) &&  
                 !stricmp(value, ctrl_pars[i].parameter_name)   ) {
                        strcpy(ctrl_link, ctrl_pars[i].link)  ;
                                               break ;
                                                                  } 

                              return(FALSE) ;
                                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Пропись данных */
     if(elm==IDC_SET) {

     if(P->name[0]==0) {                                            /* Извлечение и контроль имени */
                            GETs(IDC_NAME, P->name) ;

         if(P->name[0]==0) {
               SEND_ERROR("Имя параметра не может быть пустым") ;
  			       return(FALSE) ;
                           }

       if(Module!=NULL) {                                           /* Проверяем повторность имени */

           for(i=0 ; i<M->mScanPars_cnt ; i++) 
             if(!stricmp(M->mScanPars[i].name, P->name))  break ;

         if(i<M->mScanPars_cnt) {
                                    P->name[0]=0 ;
               SEND_ERROR("Такое имя параметра уже используется") ;
  			          return(FALSE) ;
                                }
                        }
                       }

              GETc(IDC_CATEGORY, value) ;                           /* Извлечение категории */

             for(i=0 ; i<2 ; i++)
               if(!stricmp(category_list[i], value))  P->category=i ;

         if(P->category==_OBJECT_PAR) {                             /* Извлекаем связку на управляемой параметр объекта */
          if(ctrl_link[0]==0) {  SEND_ERROR("Не задан параметр объекта") ;
  			                         return(FALSE) ;           }

               memset(P->object,   0, sizeof(P->object)) ;
               memset(P->par_link, 0, sizeof(P->par_link)) ;
              strncpy(P->object,   ctrl_object, sizeof(P->object)-1) ;
              strncpy(P->par_link, ctrl_link,   sizeof(P->par_link)-1) ;
                                      }

              GETc(IDC_KIND, value) ;                               /* Извлечение вида */

             for(i=0 ; i<2 ; i++)
               if(!stricmp(kind_list[i], value))  P->kind=i ;

              GETs(IDC_VALUE,      P->value.formula) ;              /* Извлечение и контроль значений */
              GETs(IDC_VALUE_FROM, P->value_min.formula) ;
              GETs(IDC_VALUE_TO,   P->value_max.formula) ;
              GETs(IDC_VALUE_STEP, P->value_step.formula) ;

                                           error= NULL ;

         if(P->kind==_SCAN_PAR) {
          if(P->value_min.formula[0] ==0)  error="Не задана формула рассчета минимального значения" ;
          if(P->value_max.formula[0] ==0)  error="Не задана формула рассчета максимального значения" ;
          if(P->value_step.formula[0]==0)  error="Не задана формула рассчета шага значения" ;
                                }
         else                   {
          if(P->value.formula[0]==0)  error="Не задана формула рассчета значения" ;
                                }

          if(error!=NULL) {    SEND_ERROR(error) ;
  			          return(FALSE) ;   }


                            EndDialog(hDlg, 0) ;                    /* Завершение диалога */

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

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  P
#undef  M

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*	          Новыe обработчики элементов                        */    

  LRESULT CALLBACK  Task_Scan_Pars_prc(  HWND  hWnd,     UINT  Msg,
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

      result=CallWindowProc(Pars_WndProc.call,
                                hWnd, Msg, wParam, lParam) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_PAR_ADD, "Добавить параметр") ;
          AppendMenu(hMenu, MF_STRING, IDC_PAR_DEL, "Удалить параметр") ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

         DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


/*********************************************************************/
/*								     */
/*	          Форматирование параметра в строку                  */

  void  iTScan_Pars_format(ZONE_PAR *par, char *text)

{
            char  link[128] ;
            char  value[1024] ;
    static  char *category_list[2]={ "VAR", "OBJ" } ;
    static  char *    kind_list[2]={ "FRE", "SLV" } ;

/*------------------------------------------------ Отображение линка */

                 memset(link, 0, sizeof(link)) ;

   if(par->category==_VAR_PAR) {
                               }
/*---------------------------------------- Формирование общей строки */

                 memset(value, 0, sizeof(value)) ;

   if(par->kind==_SCAN_PAR) {
           sprintf(value, "%s...%s : %s", par->value_min.formula, 
                                          par->value_max.formula, 
                                          par->value_step.formula) ;
                            }
   else                     {
           sprintf(value, "=%s", par->value.formula) ;
                            }
/*---------------------------------------- Формирование общей строки */

  sprintf(text, "%10.10s %3.3s %3.3s  %s  %s", 
                                   par->name, 
                     category_list[par->category],
                         kind_list[par->kind    ],
                                   link,
                                   value           ) ;

/*-------------------------------------------------------------------*/
}
