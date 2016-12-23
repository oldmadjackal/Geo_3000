/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ЗАДАЧА ОБСЛУЖИВАНИЯ"    		    */
/*								    */
/********************************************************************/

#pragma warning( disable : 4996 )
#define  stricmp  _stricmp

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <malloc.h>
#include <io.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys\stat.h>

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Service.h"


#define  _SECTION_FULL_NAME   "SERVICE"
#define  _SECTION_SHRT_NAME   "SERV"

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  CREATE_DIALOG  CreateDialogIndirectParam


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}


/********************************************************************/
/*								    */
/*		    	Программный модуль                          */

     static   RSS_Module_Service  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_SERVICE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_SERVICE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	       ОПИСАНИЕ КЛАССА ЗАДАЧИ "ВЫХОД В ТОЧКУ"              **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Service_instr  RSS_Module_Service_InstrList[]={

 { "help",       "?",  "#HELP (?) - список доступных команд", 
                        NULL,
                       &RSS_Module_Service::cHelp   },
 { "list",       "l",  "#LIST (L) - редактирование описания сервисной задачи", 
                        NULL,
                       &RSS_Module_Service::cList   },
 { "export",     "e",  "#EXPORT (E) - выгрузка описания сервисной задачи в файл", 
                       " EXPORT <Имя файла>\n",
                       &RSS_Module_Service::cExport },
 { "import",     "i",  "#IMPORT (E) - загрузка описания сервисной задачи из файла", 
                       " IMPORT <Имя файла>\n",
                       &RSS_Module_Service::cImport },
 { "run",        "r",  "#RUN (R) - выполнение сервисной задачи", 
                       " RUN             - выполнение текущей сервисной задачи\n"
                       " RUN <Имя файла> - выполнение сервисной задачи из файла\n",
                       &RSS_Module_Service::cRun },
 { "vars",       "v",  "#VARS (V) - отображение окна переменных", 
                        NULL,
                       &RSS_Module_Service::cVariables },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Service_instr *RSS_Module_Service::mInstrList       =NULL ;

                              FRAME *RSS_Module_Service::mScenario        =NULL ;
                                int  RSS_Module_Service::mScenario_cnt    =  0 ;

                                VAR *RSS_Module_Service::mVariables       =NULL ;
                                int  RSS_Module_Service::mVariables_cnt   =  0 ;
                               HWND  RSS_Module_Service::mVariables_Window=NULL ;

                          RSS_IFace  RSS_Module_Service::mExecIFace ;
                             double  RSS_Module_Service::mExecValue ;
                                int  RSS_Module_Service::mExecError ;
                                int  RSS_Module_Service::mExecFail ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Service::RSS_Module_Service(void)

{
                keyword="Geo" ;
         identification="Service_task" ;

             mInstrList=RSS_Module_Service_InstrList ;

//             mLineAcc=  0. ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Service::~RSS_Module_Service(void)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Service::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Service::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

/*--------------------------------------------- Идентификация секции */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;
/*- - - - - - - - - - - - - - - - - - - -  Обработка адресных команд */
        if(command[0]=='&') {
                                end=command+1 ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  Обработка прямых команд */
   else if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                            }
/*- - - - - - - - - - - - - - - - - - - -  Обработка опросных команд */
   else                     {
                                end=command ;
                            }
/*----------------------- Включение/выключение режима прямой команды */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Включение */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Service:") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)identification) ;
                         }
                                    return(0) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Выключение */
   if(end!=NULL && !strcmp(end, "..")) {

                            direct_command=0 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)"") ;

                                           return(0) ;
                                       }
/*--------------------------------------------- Выделение инструкции */

       instr=end ;                                                  /* Выделяем слово с названием команды */

     for(end=instr ; *end!= 0  &&
                     *end!=' ' &&
                     *end!='>' &&
                     *end!='.' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* Ищем команду в списке */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* Если такой команды нет... */

          status=this->kernel->vExecuteCmd(cmd, iface) ;            /*  Пытаемся передать модулю ядра... */
       if(status)  SEND_ERROR("Секция SERVICE: Неизвестная команда") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end, iface) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Service::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
                       char *work ;
                       char *end ;
                        int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE SERVICE\n", 
                      strlen("#BEGIN MODULE SERVICE\n")) )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Service::vReadSave", 0, 0) ;

       strcpy(buff, data->c_str()) ;

/*-------------------------------------------------- Данные сценария */

                work=buff+strlen("#BEGIN MODULE SERVICE\n") ;

    for(mScenario_cnt=0 ; ; work=end+1) {                           /* CIRCLE.0 */

          end=strchr(work, '\n') ;
       if(end==NULL)  break ;
         *end=0 ;

        mScenario=(FRAME *)
                   gMemRealloc(mScenario, 
                               (mScenario_cnt+1)*sizeof(FRAME), 
                                "RSS_Module_Service::mScenario", 0, 0) ;
     if(mScenario==NULL) {
              SEND_ERROR("Секция SERVICE: Переполнение памяти") ;
  			       return ;
                         }

             memset(&mScenario[mScenario_cnt], 0, sizeof(FRAME)) ;
             memcpy( mScenario[mScenario_cnt].mark,    work  ,   2) ;
             memcpy( mScenario[mScenario_cnt].action,  work+ 2, 16) ;
            strncpy( mScenario[mScenario_cnt].command, work+18, sizeof(mScenario->command)-1) ;
                               mScenario_cnt++ ; 
                                        }                           /* CONTINUE.0 */
/*------------------------- Дополнительная обработка данных сценария */

    for(i=0 ; i<mScenario_cnt ; i++) {
                 end=strchr(mScenario[i].action, ' ') ;
              if(end!=NULL)  *end=0 ;
                                     }
/*-------------------------------------------- Освобождение ресурсов */

                        gMemFree(buff) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Service::vWriteSave(std::string *text)

{
  char  value[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE SERVICE\n" ;

/*-------------------------------------------------- Данные описания */

   for(i=0 ; i<this->mScenario_cnt ; i++)
     if(        this->mScenario[i].action[0]!=0   &&
        stricmp(this->mScenario[i].action, "NULL")  ) {
            
          sprintf(value, "%2.2s%-16.16s%s\n", 
                                  this->mScenario[i].mark,
                                  this->mScenario[i].action,
                                  this->mScenario[i].command) ;
           *text+=value ;
                                                      }
/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Service::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - Описание задачи обслуживания", "SERVICE") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*             Выполнить действие в контексте потока                */

    void  RSS_Module_Service::vChangeContext(void)

{
/*------------------------------------------ Вывод списка переменных */

   if(!stricmp(mContextAction, "VARS")) {

    this->mVariables_Window=
                     CREATE_DIALOG(GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_VARIABLES", RT_DIALOG),
           	                      NULL, Task_Service_Vars_dialog, 
                                       (LPARAM)this ) ;
                        ShowWindow(this->mVariables_Window, SW_SHOW) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Service::cHelp(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Service_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции LIST                    */

  int  RSS_Module_Service::cList(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			    (LPCDLGTEMPLATE)Resource("IDD_EDIT", RT_DIALOG),
			       GetActiveWindow(), Task_Service_Edit_dialog, (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции EXPORT                  */

  int  RSS_Module_Service::cExport(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

       char *pars[_PARS_MAX] ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char *end ;
       char  tmp[1024] ;
        int  i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

/*--------------------------------------------- Контроль имени файла */

    if(pars[0]==NULL) {                                             /* Если имя файла не задано... */
                    SEND_ERROR("Не задано имя файла.\n"
                              "Например: EXPORT <Имя_файла> ...") ;
                                     return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* Если имя не содержит расширения - */
         if(       end       ==NULL ||                              /*   - добавляем типовое             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".service") ;

/*---------------------------------------- Контроль наличия сценария */

     if(         this->mScenario_cnt==0 ||
        (        this->mScenario_cnt==1            &&
         stricmp(this->mScenario[0].action, "NULL")  )) {
               SEND_ERROR("Вы пытаетесь сохранить пустой сценарий") ;
                          return(-1) ;
                                                        }
/*----------------------------------------------- Формирование файла */

        file=fopen(path, "wt") ;                                    /* Открываем файла */
     if(file==NULL) {
                         sprintf(tmp, "Ошибка открытия файла: %s", path) ;
                      SEND_ERROR(tmp) ;
                          return(-1) ;
                    }

   for(i=0 ; i<this->mScenario_cnt ; i++)                           /* Пишем данные сценария */
     if(        this->mScenario[i].action[0]!=0   &&
        stricmp(this->mScenario[i].action, "NULL")  ) {
            
          sprintf(tmp, "%2.2s%-16.16s%s\n", 
                                  this->mScenario[i].mark,
                                  this->mScenario[i].action,
                                  this->mScenario[i].command) ;
          fwrite(tmp, 1, strlen(tmp), file) ;
                                                      }

             fclose(file) ;                                         /* Закрываем файла */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции IMPORT                  */

  int  RSS_Module_Service::cImport(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

  RSS_IFace  iface_ ;
       char *pars[_PARS_MAX] ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char *end ;
        int  cnt ;
        int  space ;
       char  mark[16] ;
       char  tmp[1024] ;
        int  i ;
        int  j ;

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

/*--------------------------------------------- Контроль имени файла */

    if(pars[0]==NULL) {                                             /* Если имя файла не задано... */
                           sprintf(tmp, "[Service.cImport] Не задано имя файла. Например: IMPORT <Имя_файла>") ;
                        SEND_ERROR(tmp) ;
                    iface->vSignal("FATAL", tmp) ;
                            return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* Если имя не содержит расширения - */
         if(       end       ==NULL ||                              /*   - добавляем типовое             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".service") ;

/*------------------------------------------------- Считывание файла */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - Открытие файла */
        file=fopen(path, "rt") ;                                    /* Открываем файла */
     if(file==NULL) {
                           sprintf(tmp, "[Service.cImport] Ошибка открытия файла: %s", path) ;
                        SEND_ERROR(tmp) ;
                    iface->vSignal("FATAL", tmp) ;
                          return(-1) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - -  Определение числа строк */
          for(cnt=0 ; ; cnt++) {
                  end=fgets(tmp, sizeof(tmp)-1, file) ;             /* Считываем очередную строку */
               if(end==NULL)  break ;                               /* Если все считано... */
                               }
/*- - - - - - - - - - - - - - - - - - - -  Подготовка массива данных */
        this->mScenario=(FRAME *)
                          this->gMemRealloc(this->mScenario, cnt*sizeof(FRAME), 
                                             "RSS_Module_Service::mScenario", 0, 0) ;
     if(this->mScenario==NULL) {
                   sprintf(tmp, "[Service.cImport] Переполнение памяти") ;
                SEND_ERROR(tmp) ;
            iface->vSignal("FATAL", tmp) ;
                 return(-1) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - -  Считывание сценария */
                     rewind(file) ;                                 /* Перематываем файл на начало */

       for(this->mScenario_cnt=0  ; ; ) {                           /* CIRCLE.1 */

                     memset(tmp, 0, sizeof(tmp)) ;
                  end=fgets(tmp, sizeof(tmp)-1, file) ;             /* Считываем очередную строку */
               if(end==NULL)  break ;                               /* Если все считано... */

                  end=strchr(tmp, '\n') ;                           /* Убираем спец-символы */
               if(end!=NULL)  *end=0 ;

            for(space=1, i=0, j=0 ; tmp[i] ; i++ ) {                /* Уживаем начальные, конечные  */
              if(tmp[i]==' ') space++ ;                             /*  и парные пробелы            */
              else            space=0 ;
              if(space<2) {  tmp[j]=tmp[i] ; j++ ;  }
                                                   }
                               tmp[j]=0 ;

              if(tmp[0]=='/' && 
                 tmp[1]=='/'   )  {  strcpy(mark, "//") ; 
                                     strcpy(tmp, tmp+2) ;  }
              else                   strcpy(mark, "  ") ; 

              if(tmp[0]==0)  continue ;

                 end=strchr(tmp, ' ') ;
              if(end!=NULL) {  *end=0 ;  end++ ;  }
              else              end="" ;

              if(stricmp(tmp, "break"   ) &&                        /* Проверяем соответствие ключевого слова */
                 stricmp(tmp, "declare" ) &&
                 stricmp(tmp, "execute" ) &&
                 stricmp(tmp, "result"  ) &&
                 stricmp(tmp, "for"     ) &&
                 stricmp(tmp, "formula" ) &&
                 stricmp(tmp, "if_done" ) &&
                 stricmp(tmp, "if_error") &&
                 stricmp(tmp, "next"    ) &&
                 stricmp(tmp, "rem"     ) &&
                 stricmp(tmp, "return"  )   )  strcpy(tmp, "?unknown") ;

            strcpy(this->mScenario[this->mScenario_cnt].mark,    mark) ;
            strcpy(this->mScenario[this->mScenario_cnt].action,  tmp) ;
            strcpy(this->mScenario[this->mScenario_cnt].command, end) ;
                                   this->mScenario_cnt++ ;
                                        }                           /* CONTINUE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Закрытие файла */
             fclose(file) ;                                         /* Закрываем файла */

/*-------------------------------------------------------------------*/

                    iface->vSignal("DONE", "") ;                    /* Выдаем результат по интерфейсу межмодульной связи */

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции RUN                     */

  int  RSS_Module_Service::cRun(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

  RSS_IFace  iface_ ;
       char *pars[_PARS_MAX] ;
      FRAME  next ;
       char *end ;
       char  result[1024] ;
        int  status ;
        int  n ;
        int  i ;

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

    for(i=0 ; i<_PARS_MAX ; i++)  
      if( pars[i]    !=NULL && 
         (pars[i])[0]==  0    )  pars[i]=NULL ;

/*----------------------------------------- Загрузка задачи из файла */

    if(pars[0]!=NULL) {                                             /* Если задано имя файла... */

           status=cImport(pars[0], iface) ;                         /* Загружаем сервисную задачу из файла */
        if(status)  return(-1) ;                                    /* Если ошибка загрузки */
                      }
/*------------------------------------------- Подготовка вычислителя */

                 this->mVariables_cnt=0 ;
                 this->mExecFail     =1 ;

/*----------------------------------------------- Отработка сценария */

                                     iface->vClear() ;              /* Сброс данных межмодульного интерфейса */

   for(n=0 ; n<mScenario_cnt ; ) {                                  /* CIRCLE.1 */

            status=iFrameExecute(&mScenario[n], &next, result) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - Завершение скрипта */
         if(status==   _EXIT_FRAME   ) {

                              iface->vPass(&this->mExecIFace) ;
                                                 break ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - Ошибка в скрипте */
         if(status==  _ERROR_FRAME   ) {

                              iface->vSignal("FATAL", result) ;
                                                  break ;
                                       }
/*- - - - - - - - - - - - - - - - - - - Переход к следующему "кадру" */
         if(status==   _NEXT_FRAME   ) {
                                         n++ ;  continue ;
                                       }
/*- - - - - - - - - - - - - - - - - Переход к целевому "кадру" вверх */
         if(status==_GOTO_UP_FRAME   ) {

            for(n-- ; n>=0 ; n--) 
              if(         mScenario[n].mark[0]!='/'          &&
                 !stricmp(mScenario[n].action,  next.action) &&
                 !memicmp(mScenario[n].command, next.command, 
                                         strlen(next.command)) )  break ;

                if(n<0) {
                          sprintf(result, "[Service.cRun] Не найден целевой оператор") ;
                       SEND_ERROR(result) ;
                   iface->vSignal("FATAL", result) ;
                            break ;
                        }
                                            continue ;
                                       }
/*- - - - - - - - - - - - - - - - -  Переход к целевому "кадру" вниз */
         if(status==_GOTO_DN_FRAME ||
            status==_JUMP_DN_FRAME   ) {

            for(n++ ; n<mScenario_cnt ; n++)
              if(         mScenario[n].mark[0]!='/'          &&
                 !stricmp(mScenario[n].action,  next.action) &&
                 !memicmp(mScenario[n].command, next.command, 
                                         strlen(next.command)) )  break ;

                if(n>=mScenario_cnt) {
                          sprintf(result, "[Service.cRun] Не найден целевой оператор") ;
                       SEND_ERROR(result) ;
                   iface->vSignal("FATAL", result) ;
                            break ;
                                     }

                if(status==_JUMP_DN_FRAME)  n++ ;

                                            continue ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }                                  /* CONTINUE.1 */

/*------------------------- Отработка результата исполнения сценария */


/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции VARIABLES               */

  int  RSS_Module_Service::cVariables(char *cmd, RSS_IFace *iface)

{ 
  if(mVariables_Window!=NULL)  EndDialog(mVariables_Window, 0) ;

       strcpy(mContextAction, "VARS") ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;

   return(0) ;
}



/********************************************************************/
/*								    */
/*              Реализация инструкций сервисной задачи              */
/*                                                                  */
/*  rem          -  Комментарий                                     */
/*  return       -  Завершение сервисной задачи                     */
/*  declare      -  Обьявление переменной                           */
/*  formula      -  Вычисление выражения                            */
/*  execute      -  Выполнение команды                              */
/*  if_done      -  Обработка "хорошего" завершения предыд.команды  */
/*  if_error     -  Обработка "плохого" завершения предыд.команды   */
/*  for          -  Цикл перебора                                   */
/*  next         -  Продолжение перебора                            */
/*  break        -  Прерывание перебора                             */

  int  RSS_Module_Service::iFrameExecute(FRAME *cmd, FRAME *next, char *result)

{ 
                FRAME  cmd_rec ;
           RSS_Kernel *Calc ;
                 void *calc_context ;
            RSS_IFace  iface ;
 struct RSS_Parameter *vars_list ;
                 char *result_name ;
               double  result_value ;
           RSS_Object  *object ;
                 char *formula ;
                  int  status ;
                 char  error[1024] ;
                 char  command[1024] ;
                 char  mask[1024] ;
                 char  name[1024] ;
                 char  tmp[1024] ;
                 char *word2 ;
                 char *word3 ;
                 char *entry ;
                 char *end ;
                  int  search_deep ;
                 char *search_value ;
                  int  n ;
                  int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- Инициализация */

          sprintf(tmp, "Scenario> %s %s", cmd->action, cmd->command) ;
   this->iLogFile(tmp) ;

/*------------------------------------------------------- Подготовка */

          Calc=iGetCalculator() ;                                   /* Определение вычислителя */
       if(Calc==NULL) {
                         sprintf(result, "[Service.cRun] Отсутсвует доступный вычислитель выражений") ;
                      SEND_ERROR(result) ;
                        return(_ERROR_FRAME) ;
                      }
/*------------------------------------------- Предварительный анализ */

       if(!stricmp(cmd->mark, "//"))  return(_NEXT_FRAME) ;         /* Если строка закомментарена... */

/*----------------------------------------------------- Операция REM */

       if(!stricmp(cmd->action, "rem"    )) {

                                                return(_NEXT_FRAME) ;
                                            }
/*-------------------------------------------------- Операция RETURN */

  else if(!stricmp(cmd->action, "return" )) {
                                               return(_EXIT_FRAME) ;
                                            }
/*------------------------------------------------- Операция DECLARE */

  else if(!stricmp(cmd->action, "declare")) {
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор инструкции */
                 memset(tmp, 0, sizeof(tmp)) ;
                strncpy(tmp, cmd->command, sizeof(tmp)-1) ;         /* Извлекаем данные из команды */

             end=strtok(tmp, " \t/.;:") ;                           /* Рассекаем по списку разделителей */
//               strupr(tmp) ;                                      /* Переводим в верхний регистр */

         if(*tmp==0) {                                              /* Проверяем наличие имени переменной */
                 sprintf(result, "[Service.cRun] Отсутсвует имя переменной: %s%s %s",
                                    cmd->mark, cmd->action, cmd->command ) ;
              SEND_ERROR(result) ;
                    return(_ERROR_FRAME) ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - -  Проверка уникальности */
         for(i=0 ; i<mVariables_cnt ; i++)                          /* Проверяем на повторное имя */
           if(!stricmp(mVariables[i].name, tmp))  break ;

         if(i<mVariables_cnt) {                                     /* Если такое имя уже использовано... */
                 sprintf(result, "[Service.cRun] Переменная уже существует: %s%s %s",
                                    cmd->mark, cmd->action, cmd->command ) ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - Регистрация переменной */
        mVariables=(VAR *)
                    gMemRealloc(mVariables, 
                                (mVariables_cnt+1)*sizeof(VAR), 
                                 "RSS_Module_Service::mVariables", 0, 0) ;
     if(mVariables==NULL) {
                 sprintf(result, "[Service.cRun] Переполнение памяти") ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                          }

             memset(&mVariables[mVariables_cnt], 0, sizeof(mVariables[0])) ;
            strncpy( mVariables[mVariables_cnt].name, tmp, sizeof(mVariables[0].name)-1) ;
                                mVariables_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - Обновление окна переменных */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            return(_NEXT_FRAME) ;
                                            }
/*------------------------------------------------- Операция FORMULA */

  else if(!stricmp(cmd->action, "formula")) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Разбор формулы */
                 memset(tmp, 0, sizeof(tmp)) ;
                strncpy(tmp, cmd->command, sizeof(tmp)-1) ;         /* Извлекаем данные из команды */ 

             end=strchr(tmp, '=') ;                                 /* Ищем операцию присвоения */
          if(end==NULL) {
                            result_name=NULL ;
                                formula=tmp ;
                        }       
          else          {
                            result_name=tmp  ;
                                formula=end+1 ;
                                   *end= 0   ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - -  Подготовка переменных */
          if(mVariables_cnt) {

                vars_list=(struct RSS_Parameter *)
                             calloc(mVariables_cnt+2, sizeof(*vars_list)) ;

             for(i=0 ; i<mVariables_cnt ; i++) {
                   strncpy(vars_list[i].name, mVariables[i].name, sizeof(vars_list[i].name)-1) ;
                           vars_list[i].value=strtod(mVariables[i].value, &end) ;
                                               }

            if(!mExecFail) {                                        /* Если есть возврат по интерфейсу внутреннух связей... */
                   strncpy(vars_list[i].name, "VALUE", sizeof(vars_list[i].name)-1) ;
                           vars_list[i].value=mExecValue ;
                           }
                             } 
          else               {                                      /* Если переменных нет */
                                   vars_list=NULL ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - Вычисление формулы */
                          calc_context=NULL ;

            status=Calc->vCalculate("STD_EXPRESSION",               /* Вычисление по формуле */
                                      formula, vars_list, NULL,
                                        &result_value,
                                        &calc_context, error) ;
         if(status==-1) {
                  sprintf(result, "[Service.cRun] Ошибка вычислителя\n\n"
                                  " %s\n %s",
                                   cmd->command, error) ;
               SEND_ERROR(result) ;
                   return(_ERROR_FRAME) ;
                        }

          Calc->vCalculate("CLEAR", NULL, NULL, NULL, NULL,         /* Очистка контекста вычислителя */
                                       &calc_context, error) ;

        if(mVariables_cnt) free(vars_list) ;                        /* Освобождение массива переменных */
/*- - - - - - - - - - - - - - - - - - - - - -  Сохранение результата */
       if(result_name!=NULL) {

         for(i=0 ; i<mVariables_cnt ; i++)                          /* Ищем переменную результата */
           if(!stricmp(mVariables[i].name, result_name))  break ;

         if(i==mVariables_cnt) {                                    /* Проверяем наличие имени переменной */
                 sprintf(result, "[Service.cRun] Переменная результата не описана: %s\n%s",
                                   result_name, cmd->command ) ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                               }      

                   sprintf(mVariables[i].value, "%-40.20f", result_value) ;
                    strcpy(result, mVariables[i].value) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - Обновление окна переменных */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            return(_NEXT_FRAME) ;
                                            }
/*---------------------------------------- Операции EXECUTE и RESULT */

  else if(!stricmp(cmd->action, "execute" ) ||
          !stricmp(cmd->action, "result"  )   ) {
/*- - - - - - - - - - - - - - - - - - - - - - Подстановка переменных */
              strcpy(command, cmd->command) ;

        for(i=0 ; i<mVariables_cnt ; i++) {
                     sprintf(mask, "$%s$", mVariables[i].name) ;

          do {
                  entry=strstr(command, mask) ;
               if(entry==NULL)  break ;

                  memmove(entry+strlen(mVariables[i].value), 
                              entry+strlen(mask), strlen(entry+strlen(mask))+1) ;

                  memmove(entry, mVariables[i].value, strlen(mVariables[i].value)) ;
             } while(1) ; 
                                          }
/*- - - - - - - - - - - - - - - - - - Разыменовывание мастер-объекта */
     do {                                                           /* BLOCK.1 */
                 memset(name, 0, sizeof(name)) ;
                strncpy(name, command, sizeof(name)-1) ;
             end=strchr(name, ' ') ;                                /* Вырезаем первое слово команды */
          if(end!=NULL)  *end=0 ;

        for(i=0 ; i<OBJECTS_CNT ; i++)                              /* Ишем обьект по имени */
          if(!stricmp(OBJECTS[i]->Name, name))  break ;

          if(i==OBJECTS_CNT)  break ;                               /* Если такого нет... */

       do {                                                         /* Анализ вербальной структуры команды */
                word2=strchr(command, ' ') ;
             if(word2==NULL) {   word2="" ;
                                 word3="" ;
                                *name = 0 ;
                                  break ;   }
                word2++ ;
                word3=strchr(word2, ' ') ;
             if(word3==NULL)    word3="" ;
             else            { *word3= 0 ;
                                word3++ ;  }                            
          } while(0) ;

           sprintf(tmp, "%s %s %s %s", OBJECTS[i]->Type,           /* Переформировываем команду */
                                        word2,
                                        name,
                                        word3            ) ;
            strcpy(command, tmp) ;

        } while(0) ;                                                /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - Исполнение команды */
   do {
         status= 99 ;
                                                    iface.vClear() ;
         status=this->kernel->vExecuteCmd(command, &iface) ;
      if(status==-1)  break ;
      if(status== 0)  break ;

    for(i=0 ; i<this->kernel->modules_cnt ; i++) {
      
         status=this->kernel->modules[i].entry->vExecuteCmd(command, &iface) ;
      if(status==-1)  break ;
      if(status== 0)  break ;
                                                 }
      } while(0) ;

      if(status!=-1 && 
         status!= 0    ) {
                 sprintf(result, "[Service.cRun] Нет модуля для обработки команды") ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                         } 
/*- - - - - - - - - - - - - - - - - - - - - - - Обработка результата */
      if(!stricmp(cmd->action, "result")) {                         /* Если задано - сохраняем текущий результат */
                       this->mExecIFace.vPass(&iface) ;             /*   как выходной                            */
                                          }
  
                     mExecValue=0. ;
                     mExecError=0 ;
                     mExecFail =0 ;

           if(!iface.vCheck( NULL)     )  mExecFail=1 ;             /* Определяем наличие сигналов */
      else if( iface.vCheck("FATAL") ||                             /*  и их характер              */
               iface.vCheck("ERROR")   )  mExecError=1 ;

                         iface.vDecode(NULL, tmp) ;                 /* Разбираем деталировку сигнала */
                     mExecValue=strtod(tmp, &end) ;
/*- - - - - - - - - - - - - - - - - - - - Обновление окна переменных */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        return(_NEXT_FRAME) ;
                                                }
/*----------------------------------------------------- Операция FOR */

  else if(!stricmp(cmd->action, "for" )) {
/*- - - - - - - - - - - - - - - - - - - - - - -  Разбор спецификации */
              memset(name, 0, sizeof(name)) ;
              memset(tmp,  0, sizeof(tmp)) ;
             strncpy(name, cmd->command, sizeof(name)-1) ;
          end=strchr(name, '=') ;                                   /* Выделяем имя поисковой переменной */
       if(end==NULL) {
                       sprintf(result, "[Service.cRun] Ошибка синтаксиса оператора FOR") ;
                    SEND_ERROR(result) ;
                        return(_ERROR_FRAME) ;
                     }
                          *end=0 ;
             strncpy(tmp,  end+1, sizeof(tmp)-1) ;                  /* Выделяем поисковое выражение */        
/*- - - - - - - - - - - - - - - - - - Адресация поисковой переменной */
       for(search_value=NULL, i=0 ; i<mVariables_cnt ; i++)         /* Ищем переменную по списку */
         if(!stricmp(mVariables[i].name, name)) {
                                search_value=mVariables[i].value ;
                                                    break ;
                                                }
         if(search_value==NULL) {
                sprintf(result, "[Service.cRun] Поисковая переменная оператора FOR не объявлена") ;
             SEND_ERROR(result) ;
                return(_ERROR_FRAME) ;
                                }
/*- - - - - - - - - - - - - - - - - - -  Анализ поискового выражения */
       for(object=NULL, i=0 ; i<OBJECTS_CNT ; i++)                  /* Проверяем не задан ли в поисковом выражении */
         if(!stricmp(OBJECTS[i]->Name, tmp)) {                      /*  обьект привязки по неопределенности        */
                                                object=OBJECTS[i] ;
                                                        break ;      
                                             }
/*- - - - - - - -  Присвоение нового значения (для объекта привязки) */
       if(object!=NULL) {                                           /* При наличии объекта привязки */
                          search_deep=object->vGetAmbiguity() ;

         if(search_deep==0) {                                       /* Если объект определяется одназначно... */
              if(search_value[0]==0)  strcpy(search_value, "0") ;   /*  Делаем один заход по циклу */
              else                           search_deep=-1 ;
                            }
         else               {                                       /* Если имеется неоднозначность... */
              if(search_value[0]==0)  n=0 ;                         /*  Определяем число пройденных шагов */ 
              else                    n=strtoul(search_value, &end, 2)+1 ;

              if(n>=(1<<search_deep)) {                             /*  Если все шаги отработаны... */
                                        search_deep=-1 ;
                                      }
              else                    {                             /*  Определяем код для следующего шага... */
                       iGetBinary(search_value, search_deep, n) ;
                                      }
                            }
                        }
/*- - - - - - - - - - - - - - - - - - - - Обновление окна переменных */
       if(search_deep==-1)  search_value[0]=0 ;

     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - -  Обработка завершения поиска */
       if(search_deep==-1) {
                               strcpy(next->action, "NEXT") ;
                               strcpy(next->command, name) ;

                                 return(_JUMP_DN_FRAME) ;
                           } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 return(_NEXT_FRAME) ;
                                         }
/*---------------------------------------------------- Операция NEXT */

  else if(!stricmp(cmd->action, "next" )) {

                   strcpy(next->action,  "FOR") ;
                  sprintf(next->command, "%s=", cmd->command) ;

                      return(_GOTO_UP_FRAME) ;

                                          }
/*--------------------------------------------------- Операция BREAK */

  else if(!stricmp(cmd->action, "break" )) {

                       strcpy(next->action, "NEXT") ;
                       strcpy(next->command, cmd->command) ;

                                 return(_JUMP_DN_FRAME) ;

                                          }
/*------------------------------------ Операция IF_ERROR или IF_DONE */

  else if(!stricmp(cmd->action, "if_error") || 
          !stricmp(cmd->action, "if_done" )   ) {

         if( mExecFail) {
                  sprintf(result, "[Service.cRun] Анализируемая операция не"
                                  " поддерживает интерфейс межмодульных связей") ;
               SEND_ERROR(result) ;
                   return(_ERROR_FRAME) ;
                        }

         if((!stricmp(cmd->action, "if_error") && !mExecError) ||
            (!stricmp(cmd->action, "if_done" ) &&  mExecError)   )  return(_NEXT_FRAME) ;

                     memset(&cmd_rec, 0, sizeof(cmd_rec)) ;

                     strcpy(command, cmd->command) ;
                 end=strchr(command, ' ') ;
              if(end!=NULL)  { *end=0 ;  end++ ; }
              else              end="" ;

                    strncpy(cmd_rec.action,  command, sizeof(cmd_rec.action)) ;
                     strcpy(cmd_rec.command, end) ;

                  status=iFrameExecute(&cmd_rec, next, result) ;
       
                          return(status) ;
                                                }
/*--------------------------------------------- Неизвестная операция */

  else                                      {

                 sprintf(result, "[Service.cRun] Неизвестная операция: %s%s %s", 
                                    cmd->mark, cmd->action, cmd->command ) ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                                            }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(_NEXT_FRAME) ;
}


/********************************************************************/
/*								    */
/*            Определение нужного вычислителя                       */

  RSS_Kernel *RSS_Module_Service::iGetCalculator(void)

{
  int  status ;
  int  i ;

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("STD_EXPRESSION", NULL, NULL, NULL, 
                                                          NULL, NULL, NULL ) ;
         if(status==0)  return(CALC[i]) ;
                                     }

#undef   CALC_CNT
#undef   CALC

   return(NULL) ;
}


/********************************************************************/
/*								    */
/*                   Вывод числа в бинарном виде                    */

  void  RSS_Module_Service::iGetBinary(char *text, int  digits, int  value)

{
   int  i ;
     
       memset(text, 0, digits+1) ;

  for(i=0 ; i<digits ; i++) 
    if((1<<i) & value)  text[digits-i-1]='1' ;
    else                text[digits-i-1]='0' ;
}
