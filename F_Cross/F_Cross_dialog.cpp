/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ ОБНАРУЖЕНИЕМ ПЕРЕСЕЧЕНИЯ ОБЪЕКТОВ          */
/*								    */
/*                   Диалоговые процедуры                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>

#include "d:\_Projects\_Libraries\controls.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "F_Cross.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"


/*********************************************************************/
/*								     */
/* 	     Обработчик сообщений диалогового окна HELP	             */

    BOOL CALLBACK  Feature_Cross_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                     WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Cross  Module ;
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
/* 	     Обработчик сообщений диалогового окна ASSIGN            */

    BOOL CALLBACK  Feature_Cross_Assign_dialog(  HWND hDlg,     UINT Msg, 
 		  	                       WPARAM wParam, LPARAM lParam) 
{
       static RSS_Object *object ;
       static RSS_Object *object_e ;
  RSS_Feature_Cross_Mark *mark_list[100] ;
  RSS_Feature_Cross_Mark  outer ;
  RSS_Feature_Cross_Mark  inner ;
  RSS_Feature_Cross_Mark *mark_1 ;
  RSS_Feature_Cross_Mark *mark_2 ;
                   HFONT  font ;          /* Шрифт */
                     int  elm ;           /* Идентификатор элемента диалога */
                     int  body_elm ;      /* Идентификатор элемента диалога - списка тел */
                     int  status ;
                    char  name[1024] ;
                    char  text[1024] ;
                     int  cnt ;
                     int  i ;
                     int  j ;

#define  OBJECTS_CNT   RSS_Kernel::kernel_objects_cnt
#define  OBJECTS       RSS_Kernel::kernel_objects

#define  DESC_E_MARK(text, m1, m2)     sprintf(text, " %c %11.11s.%-11.11s "        \
                                                     "   %10.10s.%s.%s     ",       \
                                                             (m1).active?'x':' ',   \
                                                             (m1).link, (m1).body,  \
                                                (m2).object, (m2).link, (m2).body )

#define  DESC_I_MARK(text, m1, m2)     sprintf(text, " %c %11.11s.%-11.11s "        \
                                                     "    %10.10s.%s       ",       \
                                                             (m1).active?'x':' ',   \
                                                             (m1).link, (m1).body,  \
                                                             (m2).link, (m2).body )

/*------------------------------------------------- Большая разводка */

  switch(Msg) {

/*---------------------------------------------------- Инициализация */

    case WM_INITDIALOG: {

              object=(RSS_Object *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись шрифтов */
                font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
         SendMessage(ITEM(IDC_EXTERNAL_LIST), WM_SETFONT, (WPARAM)font, 0) ;
         SendMessage(ITEM(IDC_INTERNAL_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - -  Пропись связей с внешними обьектами */
          cnt=RSS_Feature_Cross::iGetMarkEx(object->Name, mark_list, 100) ;

                        LB_CLEAR(IDC_EXTERNAL_LIST) ;

        for(i=0 ; i<cnt ; i++) {
               mark_2=(RSS_Feature_Cross_Mark *)mark_list[i]->pair ;
                    DESC_E_MARK(text, *mark_list[i], *mark_2) ;
                     LB_ADD_ROW(IDC_EXTERNAL_LIST, text) ;
                               }
/*- - - - - - - - - - - - - - - - Пропись связей между своими телами */
          cnt=RSS_Feature_Cross::iGetMarkIn(object->Name, mark_list, 100) ;

                        LB_CLEAR(IDC_INTERNAL_LIST) ;

        for(i=0 ; i<cnt ; i++) {
               mark_2=(RSS_Feature_Cross_Mark *)mark_list[i]->pair ;
                    DESC_I_MARK(text, *mark_list[i], *mark_2) ;
                     LB_ADD_ROW(IDC_INTERNAL_LIST, text) ;
                               }
/*- - - - - - - - - - - - - - - - - -  Инициализация списка обьектов */
                      CB_CLEAR(IDC_OBJECT_E) ;

   for(i=0 ; i<OBJECTS_CNT ; i++) {

      if(!strcmp(OBJECTS[i]->Name, object->Name))  continue ;       

    for(j=0 ; j<OBJECTS[i]->Features_cnt ; j++)                 
      if(!strcmp(OBJECTS[i]->Features[j]->Type, "Cross"))  break ;

      if(j==OBJECTS[i]->Features_cnt)  continue ;

                   CB_ADD_LIST(IDC_OBJECT_E, OBJECTS[i]->Name) ;
                                  }

                   CB_ADD_LIST(IDC_OBJECT_E, "- Все объекты -") ;
                          SETc(IDC_OBJECT_E, "- Все объекты -") ;
/*- - - - - - - - - - - - - - - - Инициализация спискoв звеньев тела */
                      CB_CLEAR(IDC_LINK_S) ;
                      CB_CLEAR(IDC_LINK_1) ;
               
           for(i=0 ; i<object->Morphology_cnt ; i++)                /* Составляем список звеньев */
               if(object->Morphology[i].link[0])
                if(CB_FIND_LIST(IDC_LINK_S, object->Morphology[i].link)==CB_ERR) {
                      CB_ADD_LIST(IDC_LINK_S, object->Morphology[i].link) ;
                      CB_ADD_LIST(IDC_LINK_1, object->Morphology[i].link) ;
                                                                                 }
                      CB_ADD_LIST(IDC_LINK_S, "- Все звенья -") ;
                      CB_ADD_LIST(IDC_LINK_1, "- Все звенья -") ;
                             SETc(IDC_LINK_S, "- Все звенья -") ;

                         CB_CLEAR(IDC_BODY_S) ;
                      CB_ADD_LIST(IDC_BODY_S, "- Все тела -") ;
                             SETc(IDC_BODY_S, "- Все тела -") ;
/*- - - - - - - - - - - - - - - - - - - - -  Инициализация элементов */
                         CB_CLEAR(IDC_LINK_E) ;
                      CB_ADD_LIST(IDC_LINK_E, "- Все звенья -") ;
                             SETc(IDC_LINK_E, "- Все звенья -") ;
                         CB_CLEAR(IDC_BODY_E) ;
                      CB_ADD_LIST(IDC_BODY_E, "- Все тела -") ;
                             SETc(IDC_BODY_E, "- Все тела -") ;
                         CB_CLEAR(IDC_LINK_2) ;
                         CB_CLEAR(IDC_BODY_2) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ Отработка событий */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - Выбор внешнего объекта */
       if(elm==IDC_OBJECT_E)
        if(status==CBN_SELCHANGE) {

                          GETc(IDC_OBJECT_E, name) ;                /* Извлекаем имя объекта */

          if(!strcmp(name, "- Все объекты -")) {                    /* Если указаны все объекты... */
                         CB_CLEAR(IDC_LINK_E) ;
                      CB_ADD_LIST(IDC_LINK_E, "- Все звенья -") ;
                             SETc(IDC_LINK_E, "- Все звенья -") ;
                         CB_CLEAR(IDC_BODY_E) ;
                      CB_ADD_LIST(IDC_BODY_E, "- Все тела -") ;
                             SETc(IDC_BODY_E, "- Все тела -") ;
                               return(FALSE) ;
                                               }

           for(i=0 ; i<OBJECTS_CNT ; i++)                           /* Определяем ссылку на объект */ 
             if(!strcmp(OBJECTS[i]->Name, name))  break ;

                          object_e=OBJECTS[i] ;

                      CB_CLEAR(IDC_LINK_E) ;
           for(i=0 ; i<object_e->Morphology_cnt ; i++)              /* Составляем список звеньев */
               if(object_e->Morphology[i].link[0])
                if(CB_FIND_LIST(IDC_LINK_E, object_e->Morphology[i].link)==CB_ERR)
                      CB_ADD_LIST(IDC_LINK_E, object_e->Morphology[i].link) ;

                      CB_ADD_LIST(IDC_LINK_E, "- Все звенья -") ;
                             SETc(IDC_LINK_E, "- Все звенья -") ;

                         CB_CLEAR(IDC_BODY_E) ;                     /* Формируем список тел */
                      CB_ADD_LIST(IDC_BODY_E, "- Все тела -") ;
                             SETc(IDC_BODY_E, "- Все тела -") ;

                                    return(FALSE) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - Выбор звена внешнего объекта */
       if(elm==IDC_LINK_E)
        if(status==CBN_SELCHANGE) {

                             GETc(IDC_LINK_E, name) ;               /* Извлекаем имя звена */

                         CB_CLEAR(IDC_BODY_E) ;

           for(i=0 ; i<object_e->Morphology_cnt ; i++)              /* Составляем список тел */
               if(!strcmp(object_e->Morphology[i].link, name))
                      CB_ADD_LIST(IDC_BODY_E, object_e->Morphology[i].body) ;

                      CB_ADD_LIST(IDC_BODY_E, "- Все тела -") ;
                             SETc(IDC_BODY_E, "- Все тела -") ;

                                        return(FALSE) ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Выбор звена объекта */
       if(elm==IDC_LINK_S ||
          elm==IDC_LINK_1 ||
          elm==IDC_LINK_2   )
        if(status==CBN_SELCHANGE) {

            if(elm==IDC_LINK_S)  body_elm=IDC_BODY_S ;              /* Определяем элемент - список тел */
            if(elm==IDC_LINK_1)  body_elm=IDC_BODY_1 ;
            if(elm==IDC_LINK_2)  body_elm=IDC_BODY_2 ;

                             GETc(elm, name) ;                      /* Извлекаем имя звена */

                         CB_CLEAR(body_elm) ;

           for(i=0 ; i<object->Morphology_cnt ; i++)                /* Составляем список тел */
               if(!strcmp(object->Morphology[i].link, name))
                      CB_ADD_LIST(body_elm, object->Morphology[i].body) ;

                      CB_ADD_LIST(body_elm, "- Все тела -") ;
                             SETc(body_elm, "- Все тела -") ;

          if(elm==IDC_LINK_1) {
                                   CB_CLEAR(IDC_LINK_2) ;
               
           for(i=0 ; i<object->Morphology_cnt ; i++)                /* Составляем список звеньев */
              if(object->Morphology[i].link[0])
               if(strcmp(object->Morphology[i].link, name))
                if(CB_FIND_LIST(IDC_LINK_2, object->Morphology[i].link)==CB_ERR)
                      CB_ADD_LIST(IDC_LINK_2, object->Morphology[i].link) ;

                      CB_ADD_LIST(IDC_LINK_2, "- Все звенья -") ;
                             SETc(IDC_LINK_2, "- Все звенья -") ;

                         CB_CLEAR(IDC_BODY_2) ;                     /* Формируем список тел */
                      CB_ADD_LIST(IDC_BODY_2, "- Все тела -") ;
                             SETc(IDC_BODY_2, "- Все тела -") ;
                              }

                                        return(FALSE) ;
                                  }
/*- - - - - - - - - - - - -  Регистрация отношений с внешнеми телами */
       if(elm==IDC_IN_ELIST)
        if(status==BN_CLICKED) {

                           strcpy(inner.object, object->Name) ;     /* Прописываем имя ведущего объекта */

                             GETc(IDC_LINK_S,   inner.link) ;       /* Снимаем данные из диалога */
                             GETc(IDC_BODY_S,   inner.body) ;
                             GETc(IDC_OBJECT_E, outer.object) ;
                             GETc(IDC_LINK_E,   outer.link) ;
                             GETc(IDC_BODY_E,   outer.body) ;

          if(!strcmp(inner.link,   "- Все звенья -" ))  strcpy(inner.link,   "*") ;
          if(!strcmp(inner.body,   "- Все тела -"   ))  strcpy(inner.body,   "*") ;
          if(!strcmp(outer.object, "- Все объекты -"))  strcpy(outer.object, "*") ;
          if(!strcmp(outer.link,   "- Все звенья -" ))  strcpy(outer.link,   "*") ;
          if(!strcmp(outer.body,   "- Все тела -"   ))  strcpy(outer.body,   "*") ;

                                                 inner.execute=0 ;  /* Ставим отмену контроля пересечения */ 
                                                 outer.execute=0 ;

               mark_1=RSS_Feature_Cross::iSetMark(&outer, &inner) ; /* Регистрируем пару */
            if(mark_1==NULL) {
                 SEND_ERROR("Не удалось зарегестрировать связь") ;
                                        return(FALSE) ;
                             }

                    DESC_E_MARK(text, *mark_1, outer) ;
                     LB_ADD_ROW(IDC_EXTERNAL_LIST, text) ;

                                        return(FALSE) ;
                               }
/*- - - - - - - - - - - - - -  Регистрация между собственными телами */
       if(elm==IDC_IN_ILIST)
        if(status==BN_CLICKED) {

                           strcpy(inner.object, object->Name) ;     /* Прописываем имя ведущего объекта */
                           strcpy(outer.object, object->Name) ;

                             GETc(IDC_LINK_1,   inner.link) ;       /* Снимаем данные из диалога */
                             GETc(IDC_BODY_1,   inner.body) ;
                             GETc(IDC_LINK_2,   outer.link) ;
                             GETc(IDC_BODY_2,   outer.body) ;

          if(inner.link[0]==0)  return(FALSE) ;                     /* Контроль наличия спецификации */

          if(!strcmp(inner.link,   "- Все звенья -"  ))  strcpy(inner.link,   "*") ;
          if(!strcmp(inner.body,   "- Все тела -"    ))  strcpy(inner.body,   "*") ;
          if(!strcmp(outer.link,   "- Все звенья -"  ))  strcpy(outer.link,   "*") ;
          if(!strcmp(outer.body,   "- Все тела -"    ))  strcpy(outer.body,   "*") ;

                                                 inner.execute=0 ;  /* Ставим отмену контроля пересечения */ 
                                                 outer.execute=0 ;

               mark_1=RSS_Feature_Cross::iSetMark(&outer, &inner) ; /* Регистрируем пару */
            if(mark_1==NULL) {
                 SEND_ERROR("Не удалось зарегестрировать связь") ;
                                        return(FALSE) ;
                             }

                    DESC_I_MARK(text, *mark_1, outer) ;
                     LB_ADD_ROW(IDC_INTERNAL_LIST, text) ;

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

#undef   OBJECTS_CNT
#undef   OBJECTS

    return(TRUE) ;
}


