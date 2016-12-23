/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ЗАДАЧА ОБСЛУЖИВАНИЯ"    		    */
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

#include "T_Service.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#define  CREATE_DIALOG  CreateDialogIndirectParam

#include "resource.h"

/*--------------------------------------------------- Форматы данных */

#define   _FRAME_FORMAT  "%-2.2s%-8.8s %s"

/*------------------------------------ Обработчики элементов диалога */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Scenario_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Service_Scenario_WndProc(HWND, UINT, WPARAM, LPARAM) ;

/*---------------------------------------------------- Прототипы п/п */


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Task_Service_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Service  Module ;
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
/* 	     Обработчик сообщений диалогового окна EDIT	             */

    BOOL CALLBACK  Task_Service_Edit_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
  static RSS_Module_Service *Module ;
                      FRAME  frame ;
                      FRAME  frame_next ;
              static  HFONT  font ;        /* Шрифт */
                        int  elm ;         /* Идентификатор элемента диалога */
                        int  status ;
                        int  index ;
                        int  cnt ;
                       char  text[512] ;
                       char  mask[512] ;
                       char  mark[16] ;
                       char  action[16] ;
                       char  command[256] ;
                       char  result[256] ;
                       char *end ;
                        int  i ;

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
                             Module=(RSS_Module_Service *)lParam ;
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
         SendMessage(ITEM(IDC_SCENARIO), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_ACTION),   WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_COMMAND),  WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - - Подготовка списка видов действий */
                       CB_CLEAR(IDC_ACTION) ;
                    CB_ADD_LIST(IDC_ACTION, "break") ;
                    CB_ADD_LIST(IDC_ACTION, "declare") ;
                    CB_ADD_LIST(IDC_ACTION, "execute") ;
                    CB_ADD_LIST(IDC_ACTION, "for") ;
                    CB_ADD_LIST(IDC_ACTION, "formula") ;
                    CB_ADD_LIST(IDC_ACTION, "if_done") ;
                    CB_ADD_LIST(IDC_ACTION, "if_error") ;
                    CB_ADD_LIST(IDC_ACTION, "next") ;
                    CB_ADD_LIST(IDC_ACTION, "return") ;
                    CB_ADD_LIST(IDC_ACTION, "rem") ;
//                  CB_ADD_LIST(IDC_ACTION, "null") ;
/*- - - - - - - - - - - - - - - - - - - - - Выдача "кадров" сценария */
                       LB_CLEAR(IDC_SCENARIO) ;

        for(i=0 ; i<Module->mScenario_cnt ; i++) {

          if(Module->mScenario[i].action[0]==0) {  i++ ;  break ;  }

                  sprintf(text, _FRAME_FORMAT, Module->mScenario[i].mark,
                                               Module->mScenario[i].action,
                                               Module->mScenario[i].command ) ;
               LB_ADD_ROW(IDC_SCENARIO, text) ;
                                                 }

         if(i==0 || stricmp(Module->mScenario[i-1].action, "NULL")) {

                  sprintf(text, _FRAME_FORMAT, "", "null", "") ;
               LB_ADD_ROW(IDC_SCENARIO, text) ;
                                                                    }  
/*- - - - - - - - - - - - - - - - -  Перехват обработчиков сообщений */
           Scenario_WndProc.par =GetWindowLong(ITEM(IDC_SCENARIO), GWL_WNDPROC) ;
                Tmp_WndProc.call=Task_Service_Scenario_WndProc ;
          SetWindowLong(ITEM(IDC_SCENARIO), GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Раскрытие строки */
       if(   elm==IDC_SCENARIO &&
          status==LBN_DBLCLK     ) {

                     index=LB_GET_ROW (elm) ;
                           LB_GET_TEXT(elm, index, text) ;

                      memset(action,  0, sizeof(action)) ;
                      memset(command, 0, sizeof(command)) ;
                      memcpy(action,  text+ 2, 9) ;
                      strcpy(command, text+11) ;
                     
                  end=strchr(action, ' ') ;
                 *end=0 ;    

                        SETc(IDC_ACTION,  action) ;
                        SETs(IDC_COMMAND, command) ;

			              return(FALSE) ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Пропись строки */
       if(elm==IDC_SET) {

                       cnt=LB_GET_COUNT(IDC_SCENARIO) ;
                     index=LB_GET_ROW  (IDC_SCENARIO) ;
                           LB_GET_TEXT (IDC_SCENARIO, index, text) ;

                        memset(mark,  0, sizeof(mark)) ;
                        memcpy(mark,  text, 2) ;

                          GETc(IDC_ACTION,  action) ;
                          GETs(IDC_COMMAND, command) ;

                   sprintf(text, _FRAME_FORMAT, mark, action, command) ;
                LB_DEL_ROW(IDC_SCENARIO, index) ;
                LB_INS_ROW(IDC_SCENARIO, index, text) ;

           if(index==cnt-1) {                                       /* Если мы прописали последнюю строку -  */
                   sprintf(text, _FRAME_FORMAT, "", "null", "") ;   /*   - добавляем еще одну пустую в конец */
          index=LB_GET_ROW(IDC_SCENARIO) ;
                LB_ADD_ROW(IDC_SCENARIO, text) ;
                            }
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Добавление строки */
       if(elm==IDC_INSERT_ROW) {

                   sprintf(text, _FRAME_FORMAT, "", "null", "") ;
          index=LB_GET_ROW(IDC_SCENARIO) ;
                LB_INS_ROW(IDC_SCENARIO, index, text) ;

			              return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Удаление строки */
       if(elm==IDC_DELETE_ROW) {

          index=LB_GET_ROW(IDC_SCENARIO) ;
                LB_DEL_ROW(IDC_SCENARIO, index) ;

			              return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - Комментирование строки */
       if(elm==IDC_SET_COMMENT) {

                     index=LB_GET_ROW  (IDC_SCENARIO) ;
                           LB_GET_TEXT (IDC_SCENARIO, index, text) ;

                           memset(action,  0, sizeof(action)) ;
                           memset(command, 0, sizeof(command)) ;
                           memcpy(action,  text+ 2, 9) ;
                           strcpy(command, text+11) ;

                           memset(mark,  0, sizeof(mark)) ;
                           memcpy(mark,  text, 2) ;
         if(mark[0]=='/')  strcpy(mark,  "  ") ;
         else              strcpy(mark,  "//") ;

                   sprintf(text, _FRAME_FORMAT, mark, action, command) ;
                LB_DEL_ROW(IDC_SCENARIO, index) ;
                LB_INS_ROW(IDC_SCENARIO, index, text) ;

			              return(FALSE) ;
                                }
/*- - - - - - - - - - - - - - - - - - - - -  Открыть окно переменных */
       if(elm==IDC_VARS) {

                            Module->cVariables(NULL, NULL) ;

			              return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - DEBUG: "Отработать" строку */
       if(elm==IDC_STEP) {

            index=LB_GET_ROW(IDC_SCENARIO) ;                        /* Опр.индекс "кадра" сценария */
         if(index==LB_ERR              ) {
              SEND_ERROR("Секция SERVICE: Выбирете исполняемый кадр") ;
                                            return(FALSE) ;
                                         }

                           memset(&frame, 0, sizeof(frame)) ;
                      LB_GET_TEXT(IDC_SCENARIO, index, text) ;

         if(text[0]==0) {
              SEND_ERROR("Секция SERVICE: Сценарий полностью исполнен") ;
                                            return(FALSE) ;
                        }

                           memcpy(frame.mark,    text, 2) ;
                           memcpy(frame.action,  text+ 2, 9) ;
                           strcpy(frame.command, text+11) ;

                       end=strchr(frame.action, ' ') ;
                      *end=0 ;    

            status=Module->iFrameExecute(&frame,                    /* Исполняем "кадр" */
                                         &frame_next, result) ;

               sprintf( mask, _FRAME_FORMAT, "", frame_next.action,
                                                 frame_next.command ) ;
                 
         if(status==   _NEXT_FRAME   ) {                            /* Если переход к следующему "кадру" */
                                        index++ ; 
                                       }
         if(status==_GOTO_UP_FRAME   ) {                            /* Если переход к целевому "кадру" вверх */
               for(index-- ; index>=0 ; index--) {
                      LB_GET_TEXT(IDC_SCENARIO, index, text) ;
                  if(!memicmp(text, mask, strlen(mask)))  break ;
                                                 }

                if(index<0) {
                     SEND_ERROR("Секция SERVICE: Не найден целевой оператор") ;
                                            return(FALSE) ;
                            }
                                       }
         if(status==_GOTO_DN_FRAME ||                               /* Если переход к целевому "кадру" вниз */
            status==_JUMP_DN_FRAME   ) {
                             cnt=LB_GET_COUNT(IDC_SCENARIO) ;
               for(index++ ; index<cnt ; index++) {
                      LB_GET_TEXT(IDC_SCENARIO, index, text) ;
                  if(!memicmp(text, mask, strlen(mask)))  break ;
                                                  }

                if(index>=cnt) {
                     SEND_ERROR("Секция SERVICE: Не найден целевой оператор") ;
                                            return(FALSE) ;
                               }

                if(status==_JUMP_DN_FRAME)  index++ ;
                                       }

                          LB_SET_SEL(IDC_SCENARIO, index) ;         /* Позиционируем нужный "кадр" */

//       if(status==1)  ...                                         /* Если сценарий успешно завершен */

			              return(FALSE) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                             cnt=LB_GET_COUNT(IDC_SCENARIO) ;
/*- - - - - - - - - - - - - - - - - - - -  Подготовка массива данных */
        Module->mScenario=(FRAME *)
                          Module->gMemRealloc(Module->mScenario, cnt*sizeof(FRAME), 
                                              "RSS_Module_Service::mScenario", 0, 0) ;
     if(Module->mScenario==NULL) {
              SEND_ERROR("Секция SERVICE: Переполнение памяти") ;
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
                                 }

        Module->mScenario_cnt=cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Извлечение данных */
            for(i=0 ; i<Module->mScenario_cnt ; i++) {

                           memset(mark,    0, sizeof(mark)) ;
                           memset(action,  0, sizeof(action)) ;
                           memset(command, 0, sizeof(command)) ;
                      LB_GET_TEXT(IDC_SCENARIO, i, text) ;

                           memcpy(mark,  text, 2) ;
                           memcpy(action,  text+ 2, 9) ;
                           strcpy(command, text+11) ;

                       end=strchr(action, ' ') ;
                      *end=0 ;    

                  strcpy(Module->mScenario[i].mark,    mark) ;
                  strcpy(Module->mScenario[i].action,  action) ;
                  strcpy(Module->mScenario[i].command, command) ;
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

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*        Обработчик сообщений диалогового окна VARIABLES            */

    BOOL CALLBACK  Task_Service_Vars_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Service  Module ;
       static  HFONT  font ;        /* Шрифт */
                 int  elm ;         /* Идентификатор элемента диалога */
                 int  status ;
                char  record[1024] ;
                char  iface_signal[1024] ;
                char  iface_data[1024] ;
                char  tmp[1024] ;
                 int  i ;

#define  VARS        Module.mVariables
#define  VARS_CNT    Module.mVariables_cnt

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {
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
         SendMessage(ITEM(IDC_VARS_LIST), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_EXEC_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  Инициализация списка переменных */
                      LB_CLEAR(IDC_VARS_LIST) ;

         for(i=0 ; i<VARS_CNT ; i++) {
                       sprintf(tmp, "%10.10s = %s", VARS[i].name, 
                                                    VARS[i].value) ;
                    LB_ADD_ROW(IDC_VARS_LIST, tmp) ;
                                     }
/*- - - - - - - - - - - - - - - - - Инициализация списка результатов */
            Module.mExecIFace.vDecode(iface_signal, iface_data) ;

          LB_CLEAR(IDC_EXEC_LIST) ;
           sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %.10lf", "@Value", Module.mExecValue) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %d",     "@Error", Module.mExecError) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
           sprintf(tmp, "%10.10s = %d",     "@Fail",  Module.mExecFail) ;
        LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------ Отработка внутренних сообений */

    case WM_USER:  {
/*- - - - - - - - - - - - - - - - - - Обновление данных о переменных */
        if(wParam==_USER_SHOW) {

         for(i=0 ; i<VARS_CNT ; i++) {                              /* Обновление списка переменных */
                       sprintf(tmp, "%10.10s = %s", VARS[i].name, 
                                                    VARS[i].value) ;

                        memset(record, 0, sizeof(record)) ;
                   LB_GET_TEXT(IDC_VARS_LIST, i, record) ;        

               if(       record[0]==0) { 
                                          LB_ADD_ROW(IDC_VARS_LIST, tmp) ;
                                       } 
          else if(strcmp(record, tmp)) {
                                          LB_DEL_ROW(IDC_VARS_LIST, i) ;
                                          LB_INS_ROW(IDC_VARS_LIST, i, tmp) ;
                                       } 
                                     }

            Module.mExecIFace.vDecode(iface_signal, iface_data) ;   /* Обновление данных интерфейса внутренних связей */

              sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
               memset(record, 0, sizeof(record)) ;
          LB_GET_TEXT(IDC_EXEC_LIST, 0, record) ;        

        if(!strcmp(record, tmp))   return(FALSE) ;                  /* Если данные не изменились... */

              LB_CLEAR(IDC_EXEC_LIST) ;
               sprintf(tmp, "%10.10s = %s:%s",  "@Buff",  iface_signal, iface_data) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %.10lf", "@Value", Module.mExecValue) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %d",     "@Error", Module.mExecError) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;
               sprintf(tmp, "%10.10s = %d",     "@Fail",  Module.mExecFail) ;
            LB_ADD_ROW(IDC_EXEC_LIST, tmp) ;

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
/*- - - - - - - - - - - - - - - - - - - - Очистить список переменных */
       if(elm==IDC_VARS_CLEAR) {
                                               VARS_CNT=0 ;

                                  LB_CLEAR(IDC_VARS_LIST) ;

			              return(FALSE) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- Закрытие */

    case WM_CLOSE:      {
                            Module.mVariables_Window=NULL ;
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

#undef  VAR
#undef  VAR_CNT

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*	          Новыe обработчики элементов ListBox                */    

  LRESULT CALLBACK  Task_Service_Scenario_WndProc(  HWND  hWnd,     UINT  Msg,
	                                          WPARAM  wParam, LPARAM  lParam) 
{
    HMENU  hMenu ;
  LRESULT  result ;
      int  menu_flag ;        /* Флаг отображения контекстного меню */
     RECT  Rect ;
      int  x ;
      int  y ;

/*------------------------------------------------------- Подготовка */

/*----------------------------------- Подмена правой кнопки на левую */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- Вызов штатной процедуры обработки */

      result=CallWindowProc(Scenario_WndProc.call,
                                        hWnd, Msg, wParam, lParam) ;
   if(!menu_flag)  return(result) ;

/*------------------------------------- Обработка меню правой кнопки */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ;
                         y=HIWORD(lParam) ;

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_INSERT_ROW, "Добавить строку") ;
          AppendMenu(hMenu, MF_STRING, IDC_DELETE_ROW, "Удалить строку") ;
          AppendMenu(hMenu, MF_STRING, IDC_SET_COMMENT, "Закомментировать/раскомментировать") ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                      0, GetParent(hWnd), NULL) ;

                     DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}
