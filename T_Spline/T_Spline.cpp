/********************************************************************/
/*								    */
/*          МОДУЛЬ ЗАДАЧИ "ПОСТОРЕНИЕ СГЛАЖЕННОЙ ТРАЕКТОРИИ"        */
/*								    */
/********************************************************************/


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

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "..\T_Path\T_Path.h"
#include "T_Spline.h"

#pragma warning(disable : 4996)


#define  _TEMP_NAME     "Spline_work_traj"

#define  _SECTION_FULL_NAME   "SPLINE"
#define  _SECTION_SHRT_NAME   "SPL"


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  SEND_INFO(text)     SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_SHOW_INFO,          \
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

     static   RSS_Module_Spline  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_SPLINE_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_SPLINE_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**           ОПИСАНИЕ КЛАССА ЗАДАЧИ "АНАЛИЗ СОПРЯЖЕНИЙ"           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Spline_instr  
         RSS_Module_Spline_InstrList[]={

 { "help",   "?",  "#HELP (?) - список доступных команд", 
                    NULL,
                   &RSS_Module_Spline::cHelp   },
 { "config", "c",  "#CONFIG (C) - задание параметров сглаживания", 
                    NULL,
                   &RSS_Module_Spline::cConfig   },
 { "drive",  "d",  "#DRIVE (D) - задание параметров приводов в степенях подвижности", 
                   " DRIVE <Имя обьекта>\n"
                   " DRIVE <Имя обьекта> <Номер степени> <Vmax> <Amax> <Формула расчета ошибки>\n",
                   &RSS_Module_Spline::cDrive   },
 { "glide",  "g",  "#GLIDE (G)- рассчитать прохождение сглаженной траектории",
                   " GLIDE[/AESIQ] <Имя траектории>\n"
                   "   A - Подобрать закон движения по трубке ошибок\n"
                   "   E - Показать трубку ошибок\n"
                   "   S - Сохранять сглаженную траекторию\n"
                   "   I - Показывать информацию о процессе\n"
                   "   Q - Отработка без информации\n",
                   &RSS_Module_Spline::cGlide },
 {  NULL }
                                       } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Spline_instr *RSS_Module_Spline::mInstrList  =NULL ;
                              long  RSS_Module_Spline::mDebug_stamp=  0 ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Spline::RSS_Module_Spline(void)

{
                keyword="Geo" ;
         identification="Spline_task" ;

             mInstrList=RSS_Module_Spline_InstrList ;

         mTurnZone_coef=5 ;

        mErrorPipeColor=RGB(200, 0, 0) ;

}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Spline::~RSS_Module_Spline(void)

{
}


/********************************************************************/
/*								    */
/*		      Инициализация связей      		    */

  void  RSS_Module_Spline::vInit(void)

{
  int  i ;

/*---------------------------------------------- Поиск Мастер-модуля */

#define   MODULES       this->kernel->modules 
#define   MODULES_CNT   this->kernel->modules_cnt 

   for(i=0 ; i<MODULES_CNT ; i++)
     if(!stricmp(MODULES[i].entry->identification, "Path_task")) {
          mModulePath=(class RSS_Module_Path *)MODULES[i].entry ;
                                        break ;
                                                                 }

     if(mModulePath==NULL) {
           MessageBox(NULL, "Модуль SPLINE требует наличия модуля PATH", "Geo", 0) ;
                                return ;
                           }
#undef   MODULES
#undef   MODULES_CNT

/*------------------------ Подготовка перехвата команд Мастер-модуля */

#define   REDIR       mModulePath->command_redirect 
#define   REDIR_CNT   mModulePath->command_redirect_cnt 

    if(REDIR==NULL)
            REDIR=(RSS_Redirect *)gMemCalloc (  1,   sizeof(*REDIR)*(REDIR_CNT+1),
                                                       "mModulePath->command_redirect", 0, 0) ;
    else    REDIR=(RSS_Redirect *)gMemRealloc(REDIR, sizeof(*REDIR)*(REDIR_CNT+1),
                                                        "mModulePath->command_redirect", 0, 0) ;

             memset(&REDIR[REDIR_CNT], 0, sizeof(*REDIR)*1) ;

                     REDIR[REDIR_CNT].master =_SECTION_FULL_NAME ;
                     REDIR[REDIR_CNT].command="glide" ;
                     REDIR[REDIR_CNT].module = this ;
                           REDIR_CNT++ ;

#undef    REDIR
#undef    REDIR_CNT

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Spline::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Spline::vExecuteCmd(const char *cmd, RSS_IFace *iface)

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Spline:") ;
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
       if(status)  SEND_ERROR("Секция SPLINE: Неизвестная команда") ;
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
/*                Отобразить связанные данные                       */

    void  RSS_Module_Spline::vShow(char *layer)

{
}


/*********************************************************************/
/*								     */
/*             Выполнить действие в контексте потока                 */

    void  RSS_Module_Spline::vChangeContext(void)

{
/*------------------------------------ Отображение трассы траектории */

   if(!stricmp(mContextAction, "SHOW")) {

                      iErrorPipeShow(mContextObject) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            Считать данные контекста модуля из строки             */

   int  RSS_Module_Spline::vReadData(RSS_Context ***contexts,
                                     std::string   *text     )
{
                       char *buff ;
                        int  buff_size ;
                RSS_Context *context ;
  RSS_Module_Spline_Context *data ;
                       char *entry ;

#define  C   (*contexts)

/*------------------------------------------ Идентификация контекста */

   if(memicmp(text->c_str(), this->identification, 
                      strlen(this->identification)))  return(0) ;

/*------------------------------------- Подготовка списка контекстов */

          context=vAddData(contexts) ;
       if(context==NULL) {                                          /* Если не удалось разместить... */
            SEND_ERROR("Spline:Недостаточно памяти для списка контекстов") ;
                               return(-1) ;
                         }
/*-------------------------------------- Извлечение данных контекста */
/*- - - - - - - - - - - - - - - - - - - - - Подготовка буфера данных */
              buff_size=text->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Spline::vReadData.buff", 0, 0) ;

       strcpy(buff, text->c_str()) ;
/*- - - - - - - - - - - - - - - - - - - -  Создание контекста модуля */
        data=(RSS_Module_Spline_Context *)context->data ;
     if(data==NULL)
        data=(RSS_Module_Spline_Context *)                          /* Размещение данных контекста */
               gMemCalloc(1, sizeof(RSS_Module_Spline_Context), 
                              "RSS_Module_Spline::vReadData.context->data", 0, 0) ;
     if(data==NULL) {                                               /* Если не удалось разместить... */
            SEND_ERROR("Spline:Недостаточно памяти для контекста модуля") ;
                           return(-1) ;
                    }

             context->data=data ;                                   /* Пропись данных контекста */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Разборка полей */
     entry=strstr(buff, "V_MAX=") ;         data->v_max=atof(entry+strlen("V_MAX=")) ;
     entry=strstr(buff, "A_MAX=") ;         data->a_max=atof(entry+strlen("A_MAX=")) ;

     entry=strstr(buff, "F_ERR=") ;  
            memset(data->f_err, 0, sizeof(data->f_err)) ;
           strncpy(data->f_err, entry+strlen("F_ERR="), sizeof(data->f_err)-1) ;

                   iErrorCalc(data, data->f_err) ;                  /* Компилируем формулу */

/*-------------------------------------------- Освобождение ресурсов */

                   gMemFree(buff) ;

/*-------------------------------------------------------------------*/

#undef  C

   return(1) ;
}


/********************************************************************/
/*								    */
/*            Записать данные контекста модуля в строку             */

   void  RSS_Module_Spline::vWriteData(RSS_Context *context,
                                       std::string *text    )
{
  RSS_Module_Spline_Context *data ;
                       char  field[1024] ;


        data=(RSS_Module_Spline_Context *)(context->data) ;

                                                     *text = "" ;
     sprintf(field, "%s ", this->identification ) ;  *text+=field ;
     sprintf(field, "V_MAX=%.10lf ", data->v_max) ;  *text+=field ;
     sprintf(field, "A_MAX=%.10lf ", data->a_max) ;  *text+=field ;
     sprintf(field, "F_ERR=%s",      data->f_err) ;  *text+=field ;
}


/********************************************************************/
/*								    */
/*            Освободить ресурсы данных контекстове модуля          */

    void  RSS_Module_Spline::vReleaseData(RSS_Context *context)

{
  RSS_Module_Spline_Context *data ;

/*------------------------------------------------- Входной контроль */

     if(context==NULL)  return ;

/*------------------------------------------- Описание данных модуля */

       data=(RSS_Module_Spline_Context *)context->data ;

     if(data->ErrorPipe !=NULL)  gMemFree(data->ErrorPipe) ;       

     if(data->dlist1_idx!=  0 ) RSS_Kernel::display.ReleaseList(data->dlist1_idx) ;

/*---------------------------------------- Освобождение общих данных */

        free(context->data) ;
        free(context) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Spline::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - Формирование гладкой траектории, "
                                   "оптимизированной по времени", "SPLINE") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Spline::cHelp(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Spline_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CONFIG                  */

  int  RSS_Module_Spline::cConfig(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			   (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
			     GetActiveWindow(), Task_Spline_Config_dialog, 
                              (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции DRIVE                   */
/*								    */
/*  DRIVE <Имя обьекта>                                             */
/*  DRIVE <Имя обьекта> <Номер степени> ...                         */
/*                   ...<Vmax> <Amax> <Формула расчета ошибки>      */

  int  RSS_Module_Spline::cDrive(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX   6

                       char *pars[_PARS_MAX] ;
                       char *name ;
                 RSS_Object *object ;
                RSS_Context *context ;
  RSS_Module_Spline_Context *data ;
                       char *end ;
                       char  desc[1024] ;
                        int  i ;

#define   OBJECTS       this->kernel->kernel_objects
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
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

                         name=pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                     SEND_ERROR("Не задано имя траектории.\n"
                                "Например: DRIVE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                                object=OBJECTS[i] ;
                                                      break ;  
                                              }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*------------------------- Извлечение описаний степеней подвижности */

        mDrives_cnt=object->vGetJoints(mDrives) ;
     if(mDrives_cnt<=0) {
             SEND_ERROR("Объект не имеет степеней подвижности") ;
                                return(-1) ;
                        }
/*-------------------------------------- Подготовка контекста модуля */

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* CIRCLE.1 - Перебираем степени подвижности */

     if(mDrives[i].fixed)  continue ;                               /* Если фиксированная степень... */
/*- - - - - - - - - - - - - - - - - - - Подготовка списка контекстов */
        context=vAddData(&mDrives[i].contexts) ;                    /* Подготовка списка контекстов */
     if(context==NULL) {                                            /* Если не удалось разместить... */
            SEND_ERROR("Недостаточно памяти для списка контекстов") ;
                               return(-1) ;
                       }
/*- - - - - - - - - - - - - - - - - - - -  Создание контекста модуля */
            sprintf(desc, "RSS_Module_Spline_Context: Object[%s]->drive[%s]", name, mDrives[i].name) ;

        data=(RSS_Module_Spline_Context *)context->data ;
     if(data==NULL)
        data=(RSS_Module_Spline_Context *)                          /* Размещение данных контекста */
               gMemCalloc(1, sizeof(RSS_Module_Spline_Context), desc, i, 0) ;
     if(data==NULL) {                                               /* Если не удалось разместить... */
            SEND_ERROR("Недостаточно памяти для контекста модуля") ;
                           return(-1) ;
                    }

         strcpy(data->name, mDrives[i].name) ;                      /* Пропись названия привода */

             context->data=data ;                                   /* Пропись данных контекста */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }                                 /* CONTINUE.1 */
/*--------------------------------------------------- Прямое задание */

   if(pars[1]!=NULL) {

     do {                                                           /* BLOCK */
/*- - - - - - - - - - - - - -  Определяем индекс степени подвижности */
               i=strtoul(pars[1], &end, 10) ;                       /* Извлекаем номер степени */
         if(*end!=0)  break ;                                       /* Если некорректный формат... */

            i-- ;
         if(i<        0   ||
            i>=mDrives_cnt  ) {
                      SEND_ERROR("SPLINE::DRIVE - Несуществующая степень подвижности") ;
                                  return(-1) ;
                              }

         if(mDrives[i].fixed)  break ;                              /* Если степень неподвижна... */
/*- - - - - - - - - - - - - - - - - - -  Извлечение контекста модуля */
             context= vAddData(&mDrives[i].contexts) ;
                data=(RSS_Module_Spline_Context *)context->data ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Пропись данных */
         if(pars[2]!=NULL)  data->v_max=strtod(pars[2], &end) ;     /* Максимальная скорость */
         if(*end!=0)  break ;                                       /* Если некорректный формат... */

         if(pars[3]!=NULL)  data->a_max=strtod(pars[3], &end) ;     /* Максимальное ускорение */
         if(*end!=0)  break ;                                       /* Если некорректный формат... */

         if(pars[4]!=NULL)  strcpy(data->f_err, pars[4]) ;          /* Формула расчета ошибки */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(0) ;                                                /* BLOCK */

           if(*end!=0) {                                            /* Если некорректный формат... */
                      SEND_ERROR("SPLINE::DRIVE - Некорректный формат данных") ;
                                  return(-1) ;
                       }
                     }
/*----------------------------------------------- Диалоговое задание */
   else              {

     DialogBoxIndirectParam(GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Resource("IDD_DRIVES", RT_DIALOG),
			      GetActiveWindow(), Task_Spline_Drives_dialog, 
                               (LPARAM)this) ;
                     }
/*------------------------- Сохранение описаний степеней подвижности */

         object->vSetJoints(mDrives, mDrives_cnt) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции GLIDE                   */
/*								    */
/*         GLIDE <Имя траектории>                                   */

  int  RSS_Module_Spline::cGlide(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX   2

                  RSS_IFace   iface_ ;
                       char  *pars[_PARS_MAX] ;
                       char  *traj_name ;
                        int   quiet_flag ;
                 RSS_Object  *master ;
            RSS_Object_Path  *path_object ;
            RSS_Object_Path  *work_object ;
                RSS_Context  *context ;
  RSS_Module_Spline_Context  *data ;
  RSS_Module_Spline_Context  *drives[50] ;
                    SEGMENT   drive_forces[50] ;
                        int   trace_regime ;      /* Режим трассировки участка */
                     double  *segment_time ;      /* Список времен по сегментам */
                     double   full_time ;
                     double  *v ;                 /* Массив переходных скоростей по сегментам */
                     double  *a_available ;       /* Массив коэф.ускорений по сегментам */
                        int   status ;
                     double   range ;
                     double   range_max ;
                       char  *end ;
                       char   text[1024] ;
                       char   desc[1024] ;
                       char   tmp[1024] ;
                        int   n ;
                        int   i ;
                        int   j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define    V1(n)    (&v[(n  )*W->Path_frame])
#define    V2(n)    (&v[(n+1)*W->Path_frame])

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------- Контроль управляющих параметров */

     if(mTurnZone_coef<2) {
         SEND_ERROR("Spline.Glide:Заданный коэффициент длины переходного участка некорректен."
                    "Установлен коэффициент 5.") ;
                               mTurnZone_coef=5 ;
                          }
/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                                mAdjustErrorPipe=0 ;
                                  mShowErrorPipe=0 ;
                                     mSaveGlided=0 ;
                                       mIndicate=0 ;
                                      quiet_flag=0 ;

     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Spline.Glide:Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )  mAdjustErrorPipe=1 ;
                if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    mShowErrorPipe=1 ;
                if(strchr(cmd, 's')!=NULL ||
                   strchr(cmd, 'S')!=NULL   )       mSaveGlided=1 ;
                if(strchr(cmd, 'i')!=NULL ||
                   strchr(cmd, 'I')!=NULL   )         mIndicate=1 ;
                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )        quiet_flag=1 ;

                           cmd=end+1 ;
                   }
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

                    traj_name=pars[0] ;

/*---------------------------------------- Контроль имени траектории */

    if(traj_name==NULL) {                                           /* Если имя не задано... */
                   SEND_ERROR("Не задано имя траектории.\n"
                              "Например: INDICATE <Имя_траектории> ...") ;
                                     return(-1) ;
                        }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, traj_name)) {
                        path_object=(RSS_Object_Path *)OBJECTS[i] ;
                                                      break ;  
                                                   }

         if(i==OBJECTS_CNT) {                                       /* Если траектория не найдена... */
             SEND_ERROR("Траектории с таким именем НЕ существует") ;
                                return(-1) ;
                            }

         if(stricmp(path_object->Type, "Path")) {                   /* Контролируем тип объекта-траектории */
             SEND_ERROR("Указанный объект не является траекторией") ;
                                return(-1) ;                            
                                                }

               master=path_object->Trajectory->Path_object ;        /* Извлекаем мастер-объект траектории */

/*------------------------- Извлечение описаний степеней подвижности */

        mDrives_cnt=master->vGetJoints(mDrives) ;
     if(mDrives_cnt<=0) {
             SEND_ERROR("Объект не имеет степеней подвижности") ;
                                return(-1) ;
                        }
/*------------------------------------ Контрольный прогон траектории */

        status=iCheckPath(path_object) ;                            /* Контрольная трассировка траектории */
     if(status) {
       if(iface==NULL)  SEND_ERROR("Базовая траектория непроходима") ;
                    iface->vSignal("ERROR",
                                   "Базовая траектория непроходима") ;
                                return(-1) ;
                }
/*------------------------------------------ Проверка полноты данных */

#define  C     mDrives[i].contexts

    for(i=0 ; i<mDrives_cnt ; i++) {                                /* CIRCLE.1 - Перебираем степени подвижности */

       if(mDrives[i].fixed)  continue ;                             /* Если фиксированная степень... */

       if(mDrives[i].contexts==NULL) {                              /* Если пустой список контекстов */
             SEND_ERROR("У объекта не заданы параметры приводов.\n"
                        "Используйте команду SPLINE DRIVE <ИМЯ ОБЪЕКТА>.") ;
                                         return(-1) ;
                                     }

      for(j=0 ; C[j]!=NULL ; j++)                                   /* Ищем контекст этого модуля */
        if(!strcmp(C[j]->name, this->identification))  break ;

        if(C[j]==NULL) {
             SEND_ERROR("У объекта не заданы параметры приводов.\n"
                        "Используйте команду SPLINE DRIVE <ИМЯ ОБЪЕКТА>.") ;
                                         return(-1) ;
                       }

               data=(RSS_Module_Spline_Context *)C[j]->data ;

        if(data->v_max<=0. ||
           data->a_max<=0.   ) {
             SEND_ERROR("У объекта не заданы максимальные скорости/ускорения приводов.\n"
                        "Используйте команду SPLINE DRIVE <ИМЯ ОБЪЕКТА>.") ;
                                         return(-1) ;
                               }

                                        drives[i]=data ;
                                   }
#undef  C

/*-------------------------------------- Создание рабочей траектории */

          work_object=iCopyPath(path_object, _TEMP_NAME) ;          /* Создаем копию исходной траектории */

/*---------------------------- Определение трассировочных параметров */

#define  P   path_object->Trajectory

                this->mModulePath->mPath_object=P->Path_object ;
         memcpy(this->mModulePath->mPath_degrees, P->Path_degrees,           
                               P->Path_frame*sizeof(P->Path_degrees[0])) ;

               this->mModulePath->iCulculateSteps() ;

#undef  P

/*---------------------------- Определение допустимых переходных зон */

     if(mDebug_stamp==7708) {
//                              SEND_ERROR("T_Spline: iTurnZone_check")  ;
                            }

          status=iTurnZone_check(work_object, path_object) ;
       if(status==-1)  return(0) ;

//                iDrawPath(work_object, master) ;

#define  W   work_object->Trajectory

/*-------------------------------------- Размещение рабочих разделов */

       v           =(double *)gMemCalloc((W->Path_vectors+1)*W->Path_frame, sizeof(double), 
                                                   "RSS_Module_Spline::cGlide.v", 0, 0) ;
       a_available =(double *)gMemCalloc( W->Path_vectors,                  sizeof(double),
                                                   "RSS_Module_Spline::cGlide.a_available", 0, 0) ;
       segment_time=(double *)gMemCalloc( W->Path_vectors,                  sizeof(double), 
                                                   "RSS_Module_Spline::cGlide.segment_time", 0, 0) ;

         for(j=0 ; j<W->Path_vectors ; j++)  a_available[j]=1. ;

/*------------------------------- Опр.контрольной точности рассчетов */

   for(i=0 ; i<mDrives_cnt ; i++) {

    for(range_max=0, j=0 ; j<W->Path_vectors ; j++) {

          range=fabs( W->Path[(j+1)*W->Path_frame+i]-
                      W->Path[(j  )*W->Path_frame+i] ) ;
       if(range>range_max)  range_max=range ;
                                                    }

                 mAccuracy[i]=range_max/1000. ;
                                  }
/*------------------------------------------------ Обсчет траектории */

                                   mErrorPipe    =NULL ;            /* Иниц. данных трубки ошибок */
                                   mErrorPipe_cnt=  0 ;
                                   mErrorPipe_max=  0 ;

                                     trace_regime=_TIME_MINIMIZE ;

                                         mAttempt=  0 ;

   for(n=0 ; n<W->Path_vectors ; n++) {                             /* CIRCLE.1 - Перебор участков траектории */

         mAttempt++ ;
//    if(mAttempt==6)  SEND_ERROR("Critical attempt reached!") ;

                sprintf(text, "Участок %d", n) ;
                 iDebug(text, NULL) ;

           memset(&drive_forces[n], 0, sizeof(drive_forces[n])) ;
/*- - - - - - - - - - - - - - - - - - - Индикация состояния процесса */
       if(mIndicate) {
                          sprintf(text, "Vectors:%3d\r\n"
                                        "Vector :%3d\r\n"
                                        "Coef   :%lf\r\n"
                                        "Attempt:%5d\r\n",
                                          W->Path_vectors, n, a_available[n], mAttempt) ; 
                        SEND_INFO(text) ;
                     } 
/*- - - - - - - - - - - - - - - -  Опр.закона движения по траектории */
        if(n==0) {                                                  /* Стартовый участок */
                           iDebug("iLineSegment.start", NULL) ;
              status=iLineSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }
        else
        if(n==W->Path_vectors-1) {                                  /* Финишный участок */

                           iDebug("iLineSegment.finish", NULL) ;
              status=iLineSegment(_V_CORRECTION, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                                 }
        else
        if(n%2)  {                                                  /* Спряжения прямых участков */

              if(trace_regime!=_V_CORRECTION)                       /* Если не коррекция по выходной скорости - */
                  for(j=0 ; j<W->Path_frame ; j++)                  /*  - строим вектор "выходной скорости"     */ 
                       V2(n)[j]=(W->Vectors+(n+1)*W->Path_frame)[j] ;

                           iDebug("iTurnSegment", NULL) ;
              status=iTurnSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }
        else     {                                                  /* Прямые участки */
                           iDebug("iLineSegment", NULL) ;
              status=iLineSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }

                           trace_regime =_TIME_MINIMIZE ;
/*- - - - - - - - - - - - - - - - - - - Обработка обратных коррекций */
         if(status) {
                           iDebug("BackCorrection", NULL) ;

           if(status==_V_CORRECTION) {
                  sprintf(text, "Обратная коррекция скорости на участке %d", n) ;
                   iDebug(text, NULL) ;
                                        trace_regime =_V_CORRECTION ;
                                       a_available[n]= 1. ;
                                                   n-= 2 ;
                                     }
           else                      {
                  sprintf(text, "Неизвестный тип коррекции %d на участке %d (попытка %d)", status, n, mAttempt) ;
               SEND_ERROR(text) ;
                                     }
 
                        continue ;
                    }

            sprintf(text, "Время прохождения участка %lf", segment_time[n]) ;
//       SEND_ERROR(text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Проход траектории */
                    iCutErrorPipe(n) ;                              /* Отсекаем трубку последующих сегментов */

             status=iTraceSegment(n, segment_time[n], 
                                   W->Path+n*W->Path_frame,
                                     V1(n), &drive_forces[n], 
                                               drives, master) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Анализ ошибок */
          if(status==_FATAL_ERROR) {                                /* Если критическая ошибка... */
                sprintf(text, "Критическая ошибка на участке %d ", n) ;
                 iDebug(text, NULL) ;

              iface->vSignal("ERROR", "Трассировка") ;
                                        break ;               
                                   }

          if(status==_PIPE_FAIL) {                                  /* Если коллизии трубки... */
                sprintf(text, "Коррекция по ускорению %lf на участке %d ", a_available[n], n) ;
                 iDebug(text, NULL) ;

                                    a_available[n]*=0.95 ;
                                                  n-- ; 
                                               continue ;
                                 }
 
          if(status==_PATH_FAIL) {                                  /* Если коллизии траектории... */

                if(n==0) {
                              sprintf(text, "Коллизия траектории на начальном учатске (попытка %d)", mAttempt) ;
                           SEND_ERROR(text) ;

                             iface->vSignal("ERROR", "Трассировка") ;
                                        break ;               
                         }

                sprintf(text, "Коррекция по скорости на участке %d", n) ;
                 iDebug(text, NULL) ;

                  for(j=0 ; j<W->Path_frame ; j++)  V1(n)[j]*=0.95 ;

                                        trace_regime =_V_CORRECTION ;
                                       a_available[n]= 1. ;
                                                   n-= 2 ;

                                               continue ;
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      }                             /* CONTINUE.1 */
/*------------------------------------- Регистрация контекста модуля */

/*- - - - - - - - - - - - - - - Создание/извлечение контекста модуля */
          context=vAddData(&path_object->Trajectory->contexts) ;
       if(context==NULL) {                                          /* Если не удалось разместить... */
            SEND_ERROR("Недостаточно памяти для списка контекстов") ;
                               return(-1) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - -  Очистка старых данных */
     if(context->data!=NULL) {

          data=(RSS_Module_Spline_Context *)context->data ;

                                gMemFree(data->ErrorPipe) ;       
                             }
/*- - - - - - - - - - - - - - - - -  Регистрация нового блока данных */
     else                    {

            sprintf(desc, "RSS_Module_Spline_Context: Object[%s]", work_object->Name) ;

          data=(RSS_Module_Spline_Context *)context->data ;
       if(data==NULL)
          data=(RSS_Module_Spline_Context *)                        /* Размещение данных контекста */
                 gMemCalloc(1, sizeof(RSS_Module_Spline_Context),
                             desc, 0, 0) ;
       if(data==NULL) {                                             /* Если не удалось разместить... */
            SEND_ERROR("Недостаточно памяти для контекста модуля") ;
                           return(-1) ;
                      }
                             }
/*- - - - - - - - - - - - - - - - - - Сохранение данных на контексте */
             context->data=data ;                                   /* Пропись данных трубки ошибок в контекст */
                           data->ErrorPipe     =mErrorPipe ; 
                           data->ErrorPipe_cnt =mErrorPipe_cnt ; 
                           data->ErrorPipeColor=mErrorPipeColor ; 

/*------------------------------------ Отoбражение данных таймеринга */

       for(full_time=0., i=0 ; i<W->Path_vectors ; i++)             /* Определяем общее время прохождения */
              full_time+=segment_time[i] ;

            sprintf(text, "Общее время: %lf\r\n", full_time) ;

       for(i=0 ; i<W->Path_vectors-1 ; i+=2) {                      /* Формируем данные таймеринга */
            sprintf(tmp, "%2d  L:%.2lf  T:%.2lf\r\n", i+1, segment_time[i], segment_time[i+1]) ;
             strcat(text, tmp) ;
                                             }

            sprintf(tmp, "%2d  L:%.2lf", W->Path_vectors, segment_time[W->Path_vectors-1]) ;
             strcat(text, tmp) ;

     if(!quiet_flag)                                                /* Если не режим "тешины" */
        SendMessage(this->kernel_wnd, WM_USER,                      /*  Выдаем данные на экран */
                    (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*----------------------- Выдача результата в межмодульный интерфейс */

                     sprintf(text, "%-20.10lf:", full_time) ;

    if(W->Path_vectors==1) {
                     sprintf(tmp, "%.4lf", segment_time[0]) ;
                      strcat(text, tmp) ;
                           }
    else                   {
       for(i=0 ; i<W->Path_vectors-1 ; i+=2) {                      /* Формируем данные таймеринга */
           if(i==0)  sprintf(tmp, "%.4lf,", segment_time[i]+segment_time[i+1]/2.) ;
           else      sprintf(tmp, "%.4lf,", segment_time[i]+segment_time[i-1]/2.+segment_time[i+1]/2.) ;
                      strcat(text, tmp) ;
                                             }

                                                        i=W->Path_vectors-1 ;
                     sprintf(tmp, "%.4lf", segment_time[i]+segment_time[i-1]/2.) ;
                      strcat(text, tmp) ;
                           }
 
                iface->vSignal("DONE", text) ;                      /* Выдаем результат по интерфейсу межмодульной связи */

#undef   W

/*-------------------------------------- Отрисовка трубки траектории */

     if(!quiet_flag) {

            iErrorPipeShow_(path_object->Trajectory) ;

                           this->kernel->vShow(NULL) ;
                     }
/*-------------------------------------- Удаление рабочей траектории */

                             sprintf(text, "kill %s", _TEMP_NAME) ;
           this->kernel->vExecuteCmd(text) ;

/*-------------------------------------------- Освобождение ресурсов */

                            gMemFree(v) ;            
                            gMemFree(a_available) ;
                            gMemFree(segment_time) ;

/*-------------------------------------------------------------------*/

                   iDebug("Complete", NULL) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Отрисовка траектории                          */

  int  RSS_Module_Spline::iDrawPath(RSS_Object_Path *path_object,
                                         RSS_Object *master      )

{
   char  cmd[1024] ;

                           sprintf(cmd, "Path color %s green",      /* Назначение цвета */
                                            path_object->Name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

                           sprintf(cmd, "Path indicate %s %s",      /* Отрисовка пути */
                                            path_object->Name, 
                                                 master->Name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		 Контрольная трассировка траектории                 */

  int  RSS_Module_Spline::iCheckPath(RSS_Object_Path *path_object)

{
       char  cmd[1024] ;
  RSS_IFace  iface ;
       char  tmp[1024] ;

/*------------------------------------------------------- Подготовка */

                  memset(tmp, 0, sizeof(tmp)) ;
 
/*------------------------------------------- Трассировка траектории */

                           sprintf(cmd, "Path trace %s", path_object->Name) ;
    this->mModulePath->vExecuteCmd(cmd, &iface) ;

/*---------------------------------------------------- Анализ ответа */

      if(!iface.vCheck("DONE"))  return(-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Копирование траектории                        */

  RSS_Object_Path *
       RSS_Module_Spline::iCopyPath(RSS_Object_Path *path_object,
                                               char *new_name    )

{
   char  cmd[1024] ;
    int  i ;

/*------------------------------------------- Копирование траектории */

                           sprintf(cmd, "Path copy/o %s %s", 
                                           path_object->Name, 
                                                    new_name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

/*------------------------------------------------------ Поиск копии */

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

       for(i=0 ; i<OBJECTS_CNT ; i++)              
         if(!stricmp(OBJECTS[i]->Name, new_name))
                        return((RSS_Object_Path *)OBJECTS[i]) ;

#undef   OBJECTS
#undef   OBJECTS_CNT

/*-------------------------------------------------------------------*/

  return(NULL) ;
}


/********************************************************************/
/*								    */
/*                 Определение допустимых переходных зон            */

  int  RSS_Module_Spline::iTurnZone_check(RSS_Object_Path *work_path,
                                          RSS_Object_Path *base_path )

{
  double  zone ;
  double  dummy[50] ;
     int  status ;
    char  desc[1024] ;
     int  i ;
     int  j ;


#define  W   work_path->Trajectory
#define  B   base_path->Trajectory

#define  WP(I, J)  W->Path   [(I)*W->Path_frame+(J)]
#define  BP(I, J)  B->Path   [(I)*B->Path_frame+(J)]
#define  WV(I, J)  W->Vectors[(I)*W->Path_frame+(J)]

/*----------------------------------------------- Довыделение памяти */

      W->Path_vectors=2*(B->Path_vectors-1)+1 ;

                      sprintf(desc, "[%s].Path", work_path->Name) ;
      W->Path   =(double *)
                  gMemRealloc(W->Path,
                             (W->Path_vectors+1)* 
                              W->Path_frame*sizeof(double), desc, 0, 0) ;

                      sprintf(desc, "[%s].Vectors", work_path->Name) ;
      W->Vectors=(double *)
                   gMemRealloc(W->Vectors,
                               W->Path_vectors*
                               W->Path_frame*sizeof(double), desc, 0, 0) ;
   if(W->Path   ==NULL &&
      W->Vectors==NULL   ) {
           SEND_ERROR("Недостаточно памяти для сглаженной траектории") ;
                                            return(-1) ;
                           }
/*----------------------------------- Случай траектории с 1 участком */

    if(W->Path_vectors==1) {
                               SEND_ERROR("Траектория с одним участком!") ;
                                            return(-1) ;
                           }
/*---------------------- Построение траектории со срезанными стыками */

   for(i=1 ; i<B->Path_vectors ; i++) {                             /* CIRCLE.1 - По числу "поворотных точек" */

     for(zone=this->mTurnZone_coef ; ; zone*=2.) {                  /* CIRCLE.2 - Перебираем длину переходного участка */
                                                                    /*               начиная от максимальной           */
/*- - - - - - - - - - - - - - - - - - -  Отсекаем переходной участок */
       for(j=0 ; j<B->Path_frame ; j++) {                           /* Определяем точки начала и конца перехода */
           WP(2*i-1,j)=BP(i,j)+(BP(i-1,j)-BP(i,j))/zone ;
           WP(2*i,  j)=BP(i,j)+(BP(i+1,j)-BP(i,j))/zone ;
                                        }

        this->mModulePath->iCalculateVector(&WP(2*i-1,0),           /* Строим вектор перехода */
                                            &WP(2*i  ,0), 
                                            &WV(2*i-1,0) ) ;
/*- - - - - - - - - - - - - - - - - - - - -  Проверяем линию отсечки */
                this->mModulePath->mPath_object=B->Path_object ;
                this->mModulePath->mPath_frame =B->Path_frame ;
         memcpy(this->mModulePath->mPath_degrees, B->Path_degrees,
                               B->Path_frame*sizeof(B->Path_degrees[0])) ;

                this->mModulePath->mDraw    =0 ;                    /* Не рисуем объект */
                this->mModulePath->mTraceTop=0 ;                    /* Не рисуем целевую точку */
                this->mModulePath->mCheck   =1 ;                    /* Проверяем коллизии */

         status=this->mModulePath->iTraceVector(&WP(2*i-1, 0),      /* Трассируем вектор по линии отсечки */
                                                &WP(2*i,   0),
                                                 dummy,
                                                &WV(2*i-1, 0) ) ;
      if(status!=-1)  break ;                                       /* Если вектор пройден - зона найдена */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                 }                  /* CONTINUE.2 */

                                      }                             /* CONTINUE.1 */
/*------------------------------------ Дополнение необходимых данных */

     for(j=0 ; j<W->Path_frame   ; j++)                             /* Перенос конечной точки */
           WP(W->Path_vectors,j)=BP(B->Path_vectors,j) ;

     for(i=0 ; i<W->Path_vectors ; i++)                             /* Рассчет векторов участков */
        this->mModulePath->iCalculateVector(&WP(i,  0), 
                                            &WP(i+1,0), &WV(i, 0)) ;

/*-------------------------------------------------------------------*/

#undef  W
#undef  B
#undef  P

  return(NULL) ;
}


/********************************************************************/
/*                                                                  */
/*               Построение движения на линейном участке            */
/*                                                                  */
/*   Движение строится по одной из следующих схем:                  */
/*     - движение с постоянным ускорением                           */
/*     - движение с постоянным ускорением до максимальной скорости  */
/*           одной из степеней, далее - с постоянной скоростью      */
/*                                                                  */
/*   Режим _TIME_MINIMIZE - исходит из минимального времени         */
/*                            прохождения участка                   */
/*            входные параметры - start, finish, v1                 */
/*                                                                  */
/*   Режим _V_CORRECTION - исходит из минимального времени          */
/*                            прохождения участка при обязательном  */
/*                              соблюдении выходной скорости        */
/*            входные параметры - start, finish, v1, v2             */
/*                                                                  */
/*   Return: время прохождения участка                              */

     int  RSS_Module_Spline::iLineSegment(   int   regime,
                                          double  *time_all,
                                          double  *start,
                                          double  *finish,
                                          double   a_available,
                                          double  *v1,
                                         SEGMENT  *forces,
                                          double  *v2,
                       RSS_Module_Spline_Context **drives   )

{
  double  v_available ;      /* Коэффициент "использования" предельной скорости */
  double  L[50] ;
  double  Amax[50] ;         /* Предельное ускорение */
  double  Vmax[50] ;         /* Предельная скорость */
  double  Lcr ;
  double  K ;
  double  K_r ;
  double  K_t ;
  double  t ;
  double  t_r ;
  double  t_t ;
  double  t_cr ;
  double  Vo ;
  double  Vo_correction ;    /* Коэф.коррекции начальной скорости */
  double  v_coef ;
  double  v_coef_prv ;
  double  a ;
  double  c_l ;
  double  c_v ;
  double  sgn ;
  double  temp ;
     int  i_base ;
    char  tmp[1024] ;
     int  n ;
     int  i ;
     int  j ;

#define  V_MAX(i)  (v_available*drives[i]->v_max)
#define  A_MAX(i)  (a_available*drives[i]->a_max)


/*----------------------- Определение предельно-допустимых скоростей */

          v_available=a_available ;

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* Предельно-допустимые скорости    */
                                                                    /*   не могут быть меньше начальных */
       if(drives[i]==NULL)  continue ;

       if(V_MAX(i)<fabs(v1[i])) {
              v_available=1.001*fabs(v1[i])/drives[i]->v_max ;
                        i= -1 ;
                                }
                                  }
/*-------------------------------------------- Определение дистанции */

   for(i=0 ; i<mDrives_cnt ; i++)
           if(drives[i]!=NULL)  L[i]=fabs(finish[i]-start[i]) ;

/*--------------------------------- Определение предельных ускорений */

            i_base=-1 ;

   for(i=0 ; i<mDrives_cnt ; i++) {

           if(drives[i]==NULL)  continue ;                          /* Пассивные степени игнорируем */
           if(     L[i]==  0.)  continue ;                          /* Неподвижные степени игнорируем */

           if(i_base==-1)  i_base=i ;

              K=A_MAX(i_base)*L[i]/(A_MAX(i)*L[i_base]) ;           /* Опр. коэф.превышения ускорения */
           if(K>1.)  i_base=i ;                                     /* В качастве опоной выбираем степень      */
                                                                    /*  с наибольшим коэф.превышения ускорения */
                                  }

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* Опр.максимальных располачаемых ускорений */
          if(     L[i]==  0.)  Amax[i]= 0. ;
     else if(drives[i]!=NULL)  Amax[i]=A_MAX(i_base)*L[i]/L[i_base] ;
                                  }
/*---------------------------------- Определение предельной скорости */

            i_base=0 ;

   for(i_base=0, i=0 ; i<mDrives_cnt ; i++) {                       /* Определение опорной степени */

           if(drives[i]==NULL)  continue ;                          /* Пассивные степени игнорируем */
           if(     L[i]==  0.)  continue ;                          /* Неподвижные степени игнорируем */

           if(i_base==-1)  i_base=i ;
      
              K=V_MAX(i_base)*L[i]/(V_MAX(i)*L[i_base]) ;           /* Опр. коэф.превышения скорости */
           if(K>1.)  i_base=i ;                                     /* В качастве опоной выбираем степень     */
                                                                    /*  с наибольшим коэф.превышения скорости */
                                            }

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* Определение предельной скорости */
          if(     L[i]==  0.)  Vmax[i]= 0. ;
     else if(drives[i]!=NULL)  Vmax[i]=V_MAX(i_base)*L[i]/L[i_base] ;
                                  }
/*-------------------------------- Проверка вектора входной скорости */

   for(i=0 ; i<mDrives_cnt ; i++) {

       if(drives[i]==NULL)  continue ;                          /* Пассивные степени игнорируем */

       if(v1[i]==0. && v1[i_base]==0.)  continue ;

            c_v= v1[i]/v1[i_base] ;
            c_l=(finish[i]-start[i])/(finish[i_base]-start[i_base]) ;
       if( fabs(c_v-c_l)>0.001*(fabs(c_v)+fabs(c_l)) ) {
                  SEND_ERROR("Несовпадение вектора скорости на линейном участке") ;
                                        break ;
                                                       }
                                  }
/*------------------------- Рассчет по критерию МИНИМАЛЬНОГО ВРЕМЕНИ */

   if(regime==_TIME_MINIMIZE) {
/*- - - - - - - - - - - Рассчет дистанции достижения максим.скорости */
                i=i_base ;
              Lcr=(Vmax[i]*Vmax[i]-v1[i]*v1[i])/(2.*Amax[i]) ;      /* Опр. дистанцию разгона */
                K=Lcr/L[i] ;                                        /* Опр. коэф.полноты разгона */
/*- - - - - - - - - - - - - - - - - - - - - -  - Опр.режима движения */
                         i =i_base ;

       if(K<1.) {                                                   /* Рассчет закритического режима */
                      Vo=fabs(v1[i]) ;
                      t_cr=(Vmax[i]-Vo)/Amax[i] ;
         if(t_cr<0.)  t_cr=0. ;
                      t   =t_cr+L[i]*(1.-K)/Vmax[i] ;
                }
       else     {                                                   /* Рассчет докритического режима */
                      Vo  =fabs(v1[i]) ;
                      t   =(-Vo+sqrt(Vo*Vo+2.*Amax[i]*L[i]))/Amax[i] ;
                      t_cr=  t ;
                }
/*- - - - - - - - - - - - - - - - - -  Рассчет скоростей и ускорений */
     for(i=0 ; i<mDrives_cnt ; i++) {

       if(drives[i]==NULL)  continue ;

       if(     L[i]==  0.)  {
                               forces[i].a1=0. ;
                               forces[i].t1=0. ;
                               forces[i].a2=0. ;
                               forces[i].t2=0. ;
                               forces[i].a3=0. ;
                            }
       else                 {
                                sgn  =((finish[i]-start[i])>0)?1.:-1. ;
                                a    =Amax[i]*sgn ;
                                v2[i]=v1[i]+a*t_cr ;

                               forces[i].a1= a ;
                               forces[i].t1=t_cr ;
                               forces[i].a2= 0. ;
                               forces[i].t2=t_cr ;
                               forces[i].a3= 0.;
                            }
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              }
/*---------------------------- Рассчет по критерию КОНЕЧНОЙ СКОРОСТИ */

   if(regime==_V_CORRECTION) {
/*- - - - - - - - - - - - - - Контроль предельной начальной скорости */
     do {
             i =i_base ;

          if(fabs(v1[i])<=fabs(v2[i]))  break ;                     /* Если мы идем с разгоном, то не рассматриваем,      */
                                                                    /*  т.к. режим расчета "по конечной скорости" идет    */
                                                                    /*   с уменьшением v2, рассчитанного "по min времени" */                    
             Vo=sqrt(2.*Amax[i]*L[i]+v2[i]*v2[i]) ; 
          if(Vo<fabs(v1[i])) {                                      /* Если начальная скорость слишком велика - */
                                 Vo_correction=fabs(Vo/v1[i]) ;     /*   - уменьшаем ее до допустимой           */ 
                                                                    /*       и идем на предыдущий участок       */
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) v1[j]*=Vo_correction*0.95 ;

                                  return(_V_CORRECTION) ;
                             }
        } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - Опр.промежуточной скорости */
                                      i     = i_base ;
                                  v_coef    = 1. ;
                                  v_coef_prv=fabs(v1[i]/Vmax[i]);   /* Опр.коэф.начальной скорости */

     for(n=0 ; n<20 ; n++) {                                        /* CIRCLE.1 */
 
          Vo =Vmax[i]*v_coef ;                                      /* Опр.промежуточную скорость */
          Lcr=fabs(Vo*Vo-v1[i]*v1[i])/(2.*Amax[i]) ;                /* Опр.дистанцию "разгона" */
          K_r=Lcr/L[i] ;                                            /* Опр.коэф.полноты "разгона" */
          Lcr=fabs(Vo*Vo-v2[i]*v2[i])/(2.*Amax[i]) ;                /* Опр.дистанцию "торможения" */
          K_t=Lcr/L[i] ;                                            /* Опр.коэф.полноты "торможения" */

        if(K_r+K_t<=1. && v_coef==1.)  break ;                      /* Если обеспечена предельная скорость... */ 

                          temp =v_coef ;
        if(K_r+K_t>1.)  v_coef-=fabs(v_coef-v_coef_prv)/2. ;        /* В зависимости от реализуемости -    */
        else            v_coef+=fabs(v_coef-v_coef_prv)/2. ;        /*  увеличиваем или умнеьшаем скорость */

                        v_coef_prv=temp ;

                           }                                        /* CONTINUE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Рассчет времен */
                       i= i_base ;
                     Lcr=fabs(Vo*Vo-v1[i]*v1[i])/(2.*Amax[i]) ;     /* Опр.дистанцию "разгона" */
                     K_r=Lcr/L[i] ;                                 /* Опр.коэф.полноты "разгона" */
                     Lcr=fabs(Vo*Vo-v2[i]*v2[i])/(2.*Amax[i]) ;     /* Опр.дистанцию "торможения" */
                     K_t=Lcr/L[i] ;                                 /* Опр.коэф.полноты "торможения" */

                     t_r=fabs(fabs(Vo)-fabs(v1[i]))/Amax[i] ;
         if(t_r<0.)  t_r=0. ;

                     t_t=fabs(fabs(Vo)-fabs(v2[i]))/Amax[i] ;

                     t  =t_r+t_t+L[i]*(1.-K_r-K_t)/fabs(Vo);
/*- - - - - - - - - - - - - - - - - -  Рассчет скоростей и ускорений */
     for(i=0 ; i<mDrives_cnt ; i++) {

       if(drives[i]==NULL)  continue ;

       if(     L[i]==  0.)  {
                               forces[i].a1=0. ;
                               forces[i].t1=0. ;
                               forces[i].a2=0. ;
                               forces[i].t2=0. ;
                               forces[i].a3=0. ;
                            }
       else                 { 

                                  sgn=((finish[i]-start[i])>0)?1.:-1. ;
//        if(fabs(v1[i_base])>Vo) sgn=-sgn ;                         

                                                  a =Amax[i]*sgn ;
                                        forces[i].a1= a ;
                                        forces[i].t1=t_r ;
                                        forces[i].a2= 0. ;
                                        forces[i].t2=t-t_t ;
                                        forces[i].a3=-a ;

          if(forces[i].t2<forces[i].t1) forces[i].t2=forces[i].t1 ;
                            }
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                             }
/*--------------------------------------------- Специальный контроль */

     for(i=0 ; i<mDrives_cnt ; i++) {

        if(drives[i]==NULL)  continue ;

        if(fabs(forces[i].a1)>A_MAX(i)*1.001) {
                                     sprintf(tmp, "SPLINE.iLineSegment@"
                                                  "Превышение максимального "
                                                  "ускорения (%d:%d)", mAttempt, i) ;
                                  SEND_ERROR(tmp) ;
                                                    break ;
                                              }
//      if(fabs(v2[i])>V_MAX(i)*1.001) {
//                      SEND_ERROR("Превышение максимальной скорости") ;
//                                                   break ;
//                                     }
                                    }
/*-------------------------------------------------------------------*/

    *time_all=t ;

  return(0) ;

#undef  V_MAX
#undef  A_MAX

}


/********************************************************************/
/*                                                                  */
/*          Построение движения на спряжении прямых участков        */
/*                                                                  */
/*   Режим _TIME_MINIMIZE - исходит из минимального времени         */
/*                            прохождения участка                   */
/*            входные параметры - start, finish, v1, вектор v2      */
/*                                                                  */
/*   Режим _V_CORRECTION - исходит из минимального времени          */
/*                            прохождения участка при обязательном  */
/*                              соблюдении выходной скорости        */
/*            входные параметры - start, finish, v1, v2             */
/*                                                                  */
/*   Return: время прохождения участка                              */

     int  RSS_Module_Spline::iTurnSegment(   int   regime,
                                          double  *time_all,
                                          double  *start,
                                          double  *finish,
                                          double   a_available,
                                          double  *v1,
                                         SEGMENT  *forces,
                                          double  *v2,
                       RSS_Module_Spline_Context **drives   )

{
  double  v_available ;      /* Коэффициент "использования" предельной скорости */
  double  Vend[50] ;         /* Предполагаемая конечная скорость */
  double  Vk_max ;           /* Предельная конечная скорость степени */
  double  Vo_max ;           /* Предельная начальная скорость степени */
  double  Vk_correction ;    /* Коэф.коррекции конечной скорости */
  double  Vo_correction ;    /* Коэф.коррекции начальной скорости */
  double  L ;                /* Дистанция */
  double  Lt;                /* Дистанция торможения */
  double  Lr;                /* Дистанция разгона */
  double  Vk ;               /* Конечная скорость */
  double  Vo ;               /* Начальная скорость */
  double  Tmin[50] ;         /* Минимальное возможное время прохода участка степенью */
  double  t ;
  double  t_2 ;
  double  t_3 ;
  double  T ;                /* Общее время прохождения участка */
  double  Tr;                /* Время разгона */
  double  Tt;                /* Время торможения */
  double  K ;
  double  a ;                /* Ускорение */
  double  sgn ;              /* Знак скорости */
  double  A ;
  double  B ;
  double  C ;
     int  i_base ;
     int  i ;
     int  j ;
    char  tmp[1024] ;

#define   V_MAX(i)  (v_available*drives[i]->v_max)
#define   A_MAX(i)  (a_available*drives[i]->a_max)

#define  _UNLIMITED  9999.

/*--------------------------------------- Работа с отладочной меткой */

                                                   mDebug_stamp++ ;
         sprintf(tmp, "Debug stamp %ld (heap %d)", mDebug_stamp, _heapchk()) ;
          iDebug(tmp, NULL) ;

//     if(mDebug_stamp==12499)  SEND_ERROR("T_Spline: Debug stamp point")  ;

/*----------------------- Определение предельно-допустимых скоростей */

          v_available=a_available ;

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* Предельно-допустимые скорости    */
                                                                    /*   не могут быть меньше начальных */
       if(drives[i]==NULL)  continue ;

       if(V_MAX(i)<fabs(v1[i])) {
              v_available=1.001*fabs(v1[i])/drives[i]->v_max ;
                        i= -1 ;
                                }
                                  }
/*------------------------- Определение предельной выходной скорости */


   for(i_base=0, i=0 ; i<mDrives_cnt ; i++) {                       /* Определение опорной степени */

       if(drives[i]==NULL)  continue ;                              /* Пассивные степени игнорируем */
      
               K=V_MAX(i_base)*v2[i]/(V_MAX(i)*v2[i_base]) ;        /* Опр. коэф.превышения скорости */
       if(fabs(K)>1.)  i_base=i ;                                   /* В качастве опорной выбираем степень    */
                                                                    /*  с наибольшим коэф.превышения скорости */
                                            }
/*- - - - - - - - - - - - - Определение предельной конечной скорости */
  if(regime==_V_CORRECTION) {                                       /* При коррекции по конечной скорости в   */
                                                                    /*  качестве предельной выходной скорости */
         for(i=0 ; i<mDrives_cnt ; i++)                             /*   берем заданную выходную скорость     */
            if(drives[i]!=NULL)  Vend[i]=v2[i] ;
                            }
  else                      {

         for(i=0 ; i<mDrives_cnt ; i++)
           if(drives[i]!=NULL)
             Vend[i]=V_MAX(i_base)*v2[i]/fabs(v2[i_base]) ;
                            }
/*------------------------------------------ Главный адаптивный цикл */

     do {                                                           /* CIRCLE.0 */

/*---------------------- Определение предельных временных интервалов */

                    Vk_correction=0. ;
                    Vo_correction=0. ;
 
       for(i=0 ; i<mDrives_cnt ; i++) {                             /* CIRCLE.1 */ 

                 if(drives[i]==NULL)  continue ;                    /* Пассивные степени игнорируем */

                L =finish[i]-start[i] ;                             /* Определяем дистанцию */
                Vo=v1[i] ;
                Vk=Vend[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Неподвижные степени */
         if(Vo==0. && Vk==0.) {
                                           forces[i].a1= 0. ;
                                           forces[i].a2= 0. ;
                                           forces[i].a3= 0. ;
                                           forces[i].t1= 0. ;
                                           forces[i].t2= 0. ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - Стартующие степени */
         else 
         if(Vo==0.) {

               sgn   =Vk>0.?1.:-1. ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vk_max=sqrt(2.*A_MAX(i)*L) ;                         /* Опр.предельную достижимую скорость */
            if(Vk_max<Vk) {                                         /* Если конечная скорость слишком велика - */
                                 Vk_correction=0.99*Vk_max/Vk ;     /*   - уменьшаем ее до допустимой          */ 
                                                                    /*       и идем на повторный обсчет        */
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) Vend[j]*=Vk_correction ;
                                       break ;
                          }

                      t =Vk/A_MAX(i) ;
                 Tmin[i]=0.5*t+L/Vk ;

                              forces[i].a1=sgn*A_MAX(i) ;
                              forces[i].a2=sgn*A_MAX(i) ;
                              forces[i].a3= 0. ;
                              forces[i].t1= 0. ;
                              forces[i].t2= t ;
                    }
/*- - - - - - - - - - - - - - - - - - - -  Останавливающиеся степени */
         else 
         if(Vk==0.) {

               sgn   =Vo>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               L     =fabs(L) ;
               Vo_max=sqrt(2.*A_MAX(i)*L) ;                         /* Опр.предельную останавливаемую скорость */
            if(Vo_max<Vo) {                                         /* Если начальная скорость слишком велика - */
                                 Vo_correction=fabs(Vo_max/Vo) ;    /*   - уменьшаем ее до допустимой           */ 
                                                                    
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) v1[j]*=Vo_correction*0.95 ;
                                          continue ;
                          }

                      t =Vo/A_MAX(i) ;
                 Tmin[i]=0.5*t+L/Vo ;

                              forces[i].a1= 0. ;
                              forces[i].a2= 0. ;
                              forces[i].a3=-sgn*A_MAX(i) ;
                              forces[i].t1= 0. ;
                              forces[i].t2= Tmin[i]-t ;
                    } 
/*- - - - - - - - - - - - - - - - - - Степени с сохранением скорости */
         else 
         if(Vk==Vo) {
                                 Tmin[i]=L/Vo ;

                            forces[i].a1= 0. ;
                            forces[i].a2= 0. ;
                            forces[i].a3= 0. ;
                            forces[i].t1= 0. ;
                            forces[i].t2= 0. ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - Степени с разгоном */
         else 
         if(Vk*Vo>0. && fabs(Vk)>fabs(Vo) && L*Vo>0.) {

               sgn   =Vk>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vk_max=sqrt(2.*A_MAX(i)*L+Vo*Vo) ;                   /* Опр.предельную достижимую скорость */
            if(Vk_max<Vk) {                                         /* Если конечная скорость слишком велика - */
                                 Vk_correction=0.99*Vk_max/Vk ;     /*   - уменьшаем ее до допустимой          */ 
                                                                    /*       и идем на повторный обсчет        */
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) Vend[j]*=Vk_correction ;
                                     break ;
                          }

                      t =(Vk-Vo)/A_MAX(i) ;
                 Tmin[i]=t+L/Vk-Vo*t/Vk-0.5*A_MAX(i)*t*t/Vk ;

                              forces[i].a1=sgn*A_MAX(i) ;
                              forces[i].a2=sgn*A_MAX(i) ;
                              forces[i].a3= 0. ;
                              forces[i].t1= 0. ;
                              forces[i].t2= t ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - -  Степени с торможением */
         else 
         if(Vk*Vo>0. && fabs(Vk)<fabs(Vo) && L*Vo>0.) {

               sgn   =Vo>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vo_max=sqrt(2.*A_MAX(i)*L+Vk*Vk) ;                   /* Опр.предельную начальную скорость */
            if(Vo_max<Vo) {                                         /* Если начальная скорость слишком велика - */
                                 Vo_correction=fabs(Vo_max/Vo) ;    /*   - уменьшаем ее до допустимой           */ 
                                                                    
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) v1[j]*=Vo_correction*0.95 ;
                                          continue ;
                          }

                      t =(Vo-Vk)/A_MAX(i) ;
                 Tmin[i]=t+L/Vo-Vk*t/Vo-0.5*A_MAX(i)*t*t/Vo ;

                       forces[i].a1= 0. ;
                       forces[i].a2= 0. ;
                       forces[i].a3=-sgn*A_MAX(i) ;
                       forces[i].t1= 0. ;
                       forces[i].t2= Tmin[i]-t ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - Степени с реверсом */
         else 
         if(Vk*Vo<0.) {

                                sgn=Vo>0.?1.:-1. ;
                                 L =-L*Vo/fabs(Vo) ;                /* Канонизируем знак дистанции */
                                 Lt=Vo*Vo/(2.*A_MAX(i)) ;           /* Опр.дистанции торможения */
                                 Lr=Vk*Vk/(2.*A_MAX(i)) ;           /* Опр.дистанции разгона */
                                 Vo=fabs(Vo) ;
                                 Vk=fabs(Vk) ;
 
                if(Lr-Lt<=L) {                                      /* Если ЕСТЬ дистанция после разгона */
                                  Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)   /*   до максимальной скорости        */
                                         +(L-(Lr-Lt))/Vk ;
                                        t=(Vk+Vo)/A_MAX(i) ;

                                 forces[i].a1=-sgn*A_MAX(i) ;
                                 forces[i].a2=-sgn*A_MAX(i) ;
                                 forces[i].a3= 0. ;
                                 forces[i].t1= 0. ;
                                 forces[i].t2= t ;
                             }
                else         {                                      /* Если НЕТ дистанции для разгона    */ 
                                  Tr=Vk/A_MAX(i) ;                  /*   до максимальной скорости        */
                                  Lt=fabs(Lr-L) ;                   /*  - увеличиваем ее за счет         */
                                   A=A_MAX(i) ;                     /*   введения начального участка     */
                                   B=2.*Vo ;                        /*    необходимой длины, проходимого */
                                   C=Vo*Vo/(2.*A_MAX(i))-Lt ;       /*     с увеличением скорости        */

                                   t=(-B+sqrt(B*B-4.*A*C))/(2.*A) ; /* Время предельного разгона */

                    if((Vo+A_MAX(i)*t)<=V_MAX(i)) {                 /* Если мы не превышаем предельную скорость... */

                           Tmin[i]=Vk/A_MAX(i)+t+(t+Vo/A_MAX(i)) ;
                                 forces[i].a1= sgn*A_MAX(i) ;
                                 forces[i].a2= sgn*A_MAX(i) ;
                                 forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  0. ;
                                 forces[i].t2=  t ;
                                                  }
                    else                          {                 /* Если предельную скорость превышается... */

                                  t  =(V_MAX(i)-Vo)/A_MAX(i) ;      /* Время разгона до предельной скорости */
                                  t_2= V_MAX(i)/A_MAX(i) ;          /* Время останова от предельной скорости */
                                  t_3=(Lt-Vo*t-0.5*A_MAX(i)*t*t     /* Время движения с максимальной скоростью */
                                     - 0.5*A_MAX(i)*t_2*t_2)/V_MAX(i) ;

                                      Tmin[i]=Tr+t+t_2+t_3 ;

                                 forces[i].a1= sgn*A_MAX(i) ;
                                 forces[i].a2=  0 ;
                                 forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  t+t_3 ;

                                                  }
                             }
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - Степени со "змейкой" */
         else 
         if(Vk*Vo>0. && L*Vo<0.) {

                 sgn   =Vo>0.?1.:-1. ;
                 L     =fabs(L) ;
                 Lt    =Vo*Vo/(2.*A_MAX(i)) ;                       /* Опр.дистанции торможения */
                 Lr    =Vk*Vk/(2.*A_MAX(i)) ;                       /* Опр.дистанции разгона */
                 Vo    =fabs(Vo) ;
                 Vk    =fabs(Vk) ;
                 Vk_max=sqrt(A_MAX(i)*(L+Lt+Lr)) ;                  /* Опр.предельную достижимую скорость */

              if(Vk_max>V_MAX(i)) {                                 /* Движение с "полочкой" */
                                    t=V_MAX(i)/A_MAX(i) ;           /* Время разгона до предельной скорости */
                              Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)+2*t
                                     +(L+Lt+Lr-A_MAX(i)*t*t)/V_MAX(i) ;
                                  }
              else                {                                 /* Движение без "полочки" */
                                   t =Vk_max/A_MAX(i) ; 
                              Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)+2*t ;
                                  }

                           forces[i].a1=-sgn*A_MAX(i) ;
                           forces[i].a2=  0 ;
                           forces[i].a3= sgn*A_MAX(i) ;
                           forces[i].t1= Vo/A_MAX(i)+t ;
                           forces[i].t2= Tmin[i]-Vk/A_MAX(i)-t ;

                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Непонятный случай */
         else              {
 
             SEND_ERROR("SPLINE.iTurnSegment@"
                        "Необрабатываемый случай соотношения параметров движения") ;
                                     return(-1) ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      }                             /* CONTINUE.1 */

             if(Vk_correction!=0.)  continue ;                      /* Если коррекция конечной скорости */
                                                                    /*   - идем на повторный обсчет     */
             if(Vo_correction!=0.) {                                /* Если коррекция начальной скорости      */
                                      return(_V_CORRECTION) ;       /*   - задаем коррекцию конечной скорости */
                                   }                                /*       предыдущего учатка               */
/*------------------------------ Выбор опорного временного интервала */

       for(i=0 ; i<mDrives_cnt ; i++) {

                 if(drives[i]==NULL)  continue ;                    /* Пассивные степени игнорируем */

          if(i==0 || T<Tmin[i])  T=Tmin[i] ;
                                      }
/*------------------------------------- Определение законов движения */

                    Vk_correction=0. ;

       for(i=0 ; i<mDrives_cnt ; i++) {                             /* CIRCLE.2 */ 

                 if(drives[i]==NULL)  continue ;                    /* Пассивные степени игнорируем */

                 if(T==Tmin[i])  continue ;                         /* Если время берется по минимальному... */

                L =finish[i]-start[i] ;                             /* Определяем дистанцию */
                Vo=v1[i] ;
                Vk=Vend[i] ;
          
/*- - - - - - - - - - - - - - - - - - - - - - -  Неподвижные степени */
         if(Vo==0. && Vk==0.) {
                                      
                      memset(&forces[i], 0, sizeof(*forces)) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - Стартующие степени */
         else 
         if(Vo==0.) {
                                     L =fabs(L) ;
                                   sgn =Vk>0.?1.:-1. ;

                                     a =0.5*Vk*Vk/L ;               /* Просчитываем равноускоренное движение */ 
                                     t =fabs(Vk/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2= 0. ;
                           forces[i].a3= a*sgn ;
                           forces[i].t1= 0. ;
                           forces[i].t2=T-t ;

                   if(T>t)  continue ;

                                     a =A_MAX(i) ;                  /* Просчитываем равноускоренное с полочкой */
                                     t =fabs(Vk/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2= a*sgn ;
                           forces[i].a3= 0. ;
                           forces[i].t2=T-(L-0.5*Vk*Vk/a)/fabs(Vk) ;
                           forces[i].t1=forces[i].t2-t ;                        
                    }
/*- - - - - - - - - - - - - - - - - - - -  Останавливающиеся степени */
         else 
         if(Vk==0.) {
                                     L =fabs(L) ;
                                   sgn =Vo>0.?1.:-1. ;

                                     a =0.5*Vo*Vo/L ;               /* Просчитываем равноускоренное движение */ 
                                     t =fabs(Vo/a) ;
                           forces[i].a1=-a*sgn ;
                           forces[i].a2= 0. ;
                           forces[i].a3= 0. ;
                           forces[i].t1= t ;
                           forces[i].t2= t ;

                   if(T>t)  continue ;

                                     a =A_MAX(i) ;                  /* Просчитываем равноускоренное с полочкой */
                                     t =fabs(Vo/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2=-a*sgn ;
                           forces[i].a3= 0. ;
                           forces[i].t1=(L-0.5*Vo*Vo/a)/fabs(Vo) ;
                           forces[i].t2=forces[i].t1+t ;
                    } 
/*- - - - - - - -  Степени с сонаправленной скоростью и без "змейки" */
         else 
         if(Vk*Vo>0. && L*Vo>0.) {
                                     L =fabs(L) ;
                                   sgn =Vk>0.?1.:-1. ;
                                     Vo=fabs(Vo) ;
                                     Vk=fabs(Vk) ;

                                      t=2.*L/(Vo+Vk) ;             /* Опр. время равномерного ускорения */

                if(Vo!=Vk &&                                       /* Если движение с ускорением на одном участке... */
                    T< t    ) {
                 if(Vo<Vk) {  
                                t=2.*(Vk*T-L)/(Vk-Vo) ;
                                a=(Vk-Vo)/t ;
                              forces[i].a1= 0. ;
                              forces[i].a2= a*sgn ;
                              forces[i].a3= 0. ;
                              forces[i].t1= 0. ;
                              forces[i].t2= t ;

                           }
                 else      {
                                t=2.*(Vo*T-L)/(Vo-Vk) ;
                                a=(Vo-Vk)/t ;
                              forces[i].a1= 0. ;
                              forces[i].a2= 0. ;
                              forces[i].a3=-a*sgn ;
                              forces[i].t1= 0. ;
                              forces[i].t2= T-t ;
                           }
                                    continue ;
                              }

                                      a=(Vo*Vo+Vk*Vk)/(2.*L) ;     /* Опр.ускорение для страт-стопа */
                                      t=(Vo+Vk)/a ;                /* Опр. время границы старт-стопа */

                if(T<t) {                                          /* Если до старт-стопа мы не дошли... */
                            A=T*T ;
                            B=2.*(2*L-T*(Vo+Vk)) ;
                            C=-(Vk-Vo)*(Vk-Vo) ;
                            a=(-B+sqrt(B*B-4.*A*C))/(2.*A) ;

                   if(a>A_MAX(i)) {                                /* Если ускорение слишком велико  */
                                        Vk_correction=0.95 ;       /*  - уменьшаем конечную скорость */
                                                                   /*   и идем на повторный обсчет   */  
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) Vend[j]*=Vk_correction ;
                                      break ;
                                  }
             
                            t=0.5*(T-(Vk-Vo)/a) ;
                              forces[i].a1= 0. ;
                              forces[i].a2=-a*sgn ;
                              forces[i].a3= a*sgn ;
                              forces[i].t1= 0. ;
                              forces[i].t2= t ;
                                    continue ;
                        }

                 if(a>A_MAX(i)) {                                  /* Если ускорение старт-стопа слишком велико  */
                                        Vk_correction=0.95 ;       /*  - уменьшаем конечную скорость             */
                                                                   /*   и идем на повторный обсчет               */  
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) Vend[j]*=Vk_correction ;
                                    break ;
                                }

                              forces[i].a1=-a*sgn ;                /* Старт-стоп */
                              forces[i].a2= 0. ;
                              forces[i].a3= a*sgn ;
                              forces[i].t1= Vo/a ;
                              forces[i].t2= T-Vk/a ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - Степени со "змейкой" */
         else 
         if(Vk*Vo>0. && L*Vo<0.) {

                    sgn=Vo>0.?1.:-1. ;
                    L  =fabs(L) ;
                    Lt =Vo*Vo/(2.*A_MAX(i)) ;                       /* Опр.дистанции торможения */
                    Lr =Vk*Vk/(2.*A_MAX(i)) ;                       /* Опр.дистанции разгона */
                    Vo =fabs(Vo) ;
                    Vk =fabs(Vk) ;

                    Tt =Vo/A_MAX(i) ;
                    Tr =Vk/A_MAX(i) ;
                    t  =T-Tr-Tt ;
                    t_2=0.5*t-sqrt(0.25*t*t-(L+Lt+Lr)/A_MAX(i)) ;   /* Время разгона/торможения до полки */

                           forces[i].a1=-sgn*A_MAX(i) ;
                           forces[i].a2=  0 ;
                           forces[i].a3= sgn*A_MAX(i) ;
                           forces[i].t1= Tt+t_2 ;
                           forces[i].t2= T-Tr-t_2 ;

                                 }
/*- - - - - - - - - - - - -  Степени с противонаправленной скоростью */
         else              {
                                sgn=Vo>0.?1.:-1. ;
                                 L =-L*Vo/fabs(Vo) ;                /* Канонизируем знак дистанции */
                                 Vo=fabs(Vo) ;
                                 Vk=fabs(Vk) ;

              if(forces[i].a3!=0) {                                 /* Если случай анализа Lt+L<=0. */
                                      Lr=Vk*Vk/(2.*A_MAX(i)) ;      /* Опр.дистанции разгона */
                                      Lt=fabs(Lr-L) ;               /* Опр.дистанции торможения */
                                      Tr=Vk/A_MAX(i) ;
                                      Tt=T-Tr ; 

                  t=2.*Lt/Vo ;                                      /* Опр.время равноуск.торможения */

               if(t<=Tt) {                                          /* Если случай с торможения с паузой после остановки */
                                 forces[i].a1=-sgn*Vo/t ;
                                 forces[i].a2=  0 ;
//                               forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  Tt ;
                                        continue ;
                        } 

                  t=Vo/A_MAX(i)+(Lt-Vo*Vo/(2.*A_MAX(i)))/Vo ;       /* Опр.время торможения с "полки" на максимум */
               if(t<=Tt) {                                          /* Если мы в пределах этого случая... */
                                           t =2.*Lt/Vo-Tt ;
                                 forces[i].a1=  0 ;
                                 forces[i].a2=-sgn*Vo/(Tt-t) ;
//                               forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  Tt ;
                                        continue ;
                         }            
                                                                    /* Опр.режим с разгоном до полки и торможением по максимуму */

#define        TAU_1  (Vo/A_MAX(i))
#define        TAU_2  (Tt-Vo/A_MAX(i))

                             A  =A_MAX(i) ;
                             B  =Vo-A_MAX(i)*TAU_1-A_MAX(i)*TAU_2 ;
                             C  =Lt-Vo*TAU_2-0.5*A_MAX(i)*TAU_1*TAU_1 ;
                             t  =(-B-sqrt(B*B-4.*A*C))/(2.*A) ;
//                           t  =(-B+sqrt(B*B-4.*A*C))/(2.*A) ;
                             t_2=TAU_2-2.*t ;
                          if(t_2<0.) {
                                     sprintf(tmp, "SPLINE.iTurnSegment@"
                                                  "Развал расчета закона движения "
                                                  "противоход (%d:%d)", mAttempt, i) ;
                                  SEND_ERROR(tmp) ;
                                      return(-1) ;
                                     }
#undef         TAU_1
#undef         TAU_2

                                 forces[i].a1= sgn*A_MAX(i) ;
                                 forces[i].a2=  0 ;
//                               forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  t+t_2 ;
                                        continue ;
                                  }

                             Lt=Vo*Vo/(2.*A_MAX(i)) ;               /* Опр.дистанции торможения */
                             Lr=L+Lt ;                              /* Опр.дистанции разгона */
                              t=Vo/A_MAX(i)+2.*fabs(Lr)/Vk ;        /* Определяем время при равно-ускоренном разгоне */

                 if(t<=T) {                                         /* Если равноускоренный разгон с остановкой */
                                   t=2.*fabs(Lr)/Vk ;

//                               forces[i].a1=-sgn*A_MAX(i) ;     
                                 forces[i].a2=  0. ;
                                 forces[i].a3=-sgn*Vk/t  ;
                                 forces[i].t1= Vo/A_MAX(i) ;
                                 forces[i].t2=  T-t ;
                                   continue ;
                          }

                                      t=2.*(T-Vo/A_MAX(i)-Lr/Vk) ;
                                      a=Vk/t ;

//                               forces[i].a1=-sgn*A_MAX(i) ;      /* Если разгон с полочкой */
                                 forces[i].a2=-sgn*a ;
                                 forces[i].a3=  0. ;
                                 forces[i].t1= Vo/A_MAX(i)  ;
                                 forces[i].t2= forces[i].t1+t ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/             
                                      }                             /* CONTINUE.1 */

             if(Vk_correction!=0.)  continue ;                      /* Если коррекция конечной скорости */
                                                                    /*   - идем на повторный обсчет     */
/*------------------------------------------ Главный адаптивный цикл */
                      
                      break ;

        } while(1) ;                                                /* CONTINUE.0 */

/*--------------------------------------------------------- Проверки */

/*----------------------------------------- Выдача результата наружу */

   for(i=0 ; i<mDrives_cnt ; i++)                                   /* Определение предельной скорости */
     if(drives[i]!=NULL)  v2[i]=Vend[i] ;

                    *time_all=T ;

/*-------------------------------------------------------------------*/

  return(0) ;

#undef  V_MAX
#undef  A_MAX

}


/********************************************************************/
/*                                                                  */
/*               Движение по участку траектории                     */

     int  RSS_Module_Spline::iTraceSegment(   int   n_segm,
                                           double   time_all,
                                           double  *start,
                                           double  *v0,
                                          SEGMENT  *forces,
                        RSS_Module_Spline_Context **drives,
                                       RSS_Object  *object )

{
          double  t ;
          double  dt ;
          double  v[50] ;
          double  p[50] ;
          double  p_prv[50] ;
          double  v_acc[50] ;         /* Оценка точности по скорости */
          double  p_acc[50] ;         /* Оценка точности по положению */
          double  v1 ;
          double  v2 ;
          double  a ;
  static  double *pipe ;              /* Список точек среза трубки ошибок */
             int  pipe_cnt ;
       RSS_Point  Top ;               /* Текущее положение целевой точки объекта */
       RSS_Point  Top_pipe ;          /* Положение целевой точки объекта на срезе трубки ошибок */
             int  status ;
            char  tmp[1024] ;
             int  i ;
             int  j ;

#define  V_MAX(i)               drives[i]->v_max
#define  A_MAX(i)  (a_available*drives[i]->a_max)

/*---------------------------------------------------- Инициализация */

           memset(v_acc, 0, sizeof(v_acc)) ;
           memset(p_acc, 0, sizeof(p_acc)) ;

           memcpy(v, v0,    mDrives_cnt*sizeof(v[0])) ;
           memcpy(p, start, mDrives_cnt*sizeof(p[0])) ;

    if(pipe==NULL)  pipe=(double *)
                           gMemCalloc(64, 50*sizeof(*pipe), 
                                       "RSS_Module_Spline::iTraceSegment.pipe", 0, 0) ;

/*---------------------------------------- Проверка разбивки времени */

   for(i=0 ; i<mDrives_cnt ; i++) {                                 
                                                                    
       if(drives[i]==NULL)  continue ;                    

       if(forces[i].t1<    0.      ||
          forces[i].t2<forces[i].t1  ) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "Некорректное разбиение времени для "
                                "участка траектории при трассировке (%d:%d:%d)", mAttempt, n_segm, i) ;
                SEND_ERROR(tmp) ;
                     return(_FATAL_ERROR) ;
                                       }  

                                  }

/*---------------------------------------- Обработка начальной точки */

      for(i=0 ; i<mDrives_cnt ; i++)              
         if(drives[i]!=NULL)  mDrives[i].value=p[i] ;

              object->vSetJoints     (mDrives, mDrives_cnt) ;
              object->vSolveByJoints () ;                           /* Решаем прямую задачу */
              object->vGetTargetPoint(&Top) ;                       /* Получаем положение целевой точки */  

                       iSaveErrorPipe(n_segm, &Top, &Top) ;         /* Сохраняем точку траектории */

/*------------------------------------------------ Цикл сканирования */

   for(t=0. ; t<time_all ; t+=dt) {                                 /* CIRCLE.0 */

           memcpy(p_prv, p,  mDrives_cnt*sizeof(p[0])) ;            /* Перенос предыдущей точки */

/*------------------------------ Подбор оптимального шага по времени */         

                          dt=time_all/10. ;                         /* Устанавливаем начальный шаг */
        do {
/*- - - - - - - - - - - - - - - - - -  Отсечка по критичным временам */
              for(i=0 ; i<mDrives_cnt ; i++) {                      /* Если пересекаем точку переключения - */
                                                                    /*   обрезаем время по ней              */
                 if(drives[i]==NULL)  continue ;                    

                 if( t    < forces[i].t1 &&
                    (t+dt)>=forces[i].t1   )  dt=(forces[i].t1-t)*1.00001 ;
            else if( t    < forces[i].t2 &&
                    (t+dt)>=forces[i].t2   )  dt=(forces[i].t2-t)*1.00001 ;
                                             }
 
                if((t+dt)>time_all)  dt=(time_all-t)*1.00001 ;      /* Если пересекаем границу участка */
/*- - - - - - - - - - - - - -  Рассчет значений степеней подвижности */
              for(i=0 ; i<mDrives_cnt ; i++) {                      /* Рассчет нового положения */

                         if(drives[i]==NULL)  continue ;                    

                         if(t<forces[i].t1)  a=forces[i].a1 ;
                    else if(t<forces[i].t2)  a=forces[i].a2 ;
                    else                     a=forces[i].a3 ;

                           p_acc[i]+=fabs(v[i]*dt+0.5*a*dt*dt) ;
                           v_acc[i]+=fabs(a*dt) ;

#define  T     (t+dt)
#define  A1  forces[i].a1
#define  A2  forces[i].a2
#define  A3  forces[i].a3

                         if(T<forces[i].t1) {   p[i] =0.5*A1*T*T+v0[i]*T+start[i] ;
                                                v[i] =v0[i]+A1*T ;
                                            }  
                    else if(T<forces[i].t2) {   v1  =A1*forces[i].t1+v0[i] ;
                                                p[i] =0.5*A2*(T-forces[i].t1)*(T-forces[i].t1)
                                                     +0.5*A1*forces[i].t1*forces[i].t1
                                                     +v1*(T-forces[i].t1)
                                                     +v0[i]*forces[i].t1
                                                     +start[i] ;
                                                v[i] =A2*(T-forces[i].t1)+v1 ;
                                            }
                    else                    {   v1  =A1*forces[i].t1+v0[i] ;
                                                v2  =A2*(forces[i].t2-forces[i].t1)+v1 ;
                                                p[i] =0.5*A3*(T-forces[i].t2)*(T-forces[i].t2)
                                                     +0.5*A2*(forces[i].t2-forces[i].t1)*(forces[i].t2-forces[i].t1)
                                                     +0.5*A1*forces[i].t1*forces[i].t1
                                                     +v2*(T-forces[i].t2)
                                                     +v1*(forces[i].t2-forces[i].t1)
                                                     +v0[i]*forces[i].t1
                                                     +start[i] ;
                                                v[i] =A3*(T-forces[i].t2)+v2 ;
                                            }

#ifdef  _INCREMENT

                           p[i]=p_prv[i]+v[i]*dt+0.5*a*dt*dt ;
                           v[i]=    v[i]+a*dt ;
#endif

#undef   T
#undef   A1
#undef   A2
#undef   A3


                     mDrives[i].value=p[i] ;

                      drives[i]->v=fabs(v[i]) ;
                      drives[i]->a=fabs(a) ;
                                             }
/*- - - - - - - - - - - - - - - - - - Контроль скоростей и ускорений */
/*- - - - - - - - - - - - - - - - - - - -  Рассчет положения объекта */
                    object->vSetJoints    (mDrives, mDrives_cnt) ;
             status=object->vSolveByJoints() ;                      /* Решаем прямую задачу */
          if(status) {                                              /* Если точка недостижима... */
                            iDebug("Нарушение внутренних ограничений на траектории", NULL) ;
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "Нарушение внутренних ограничений на траектории") ;
                             return(_PATH_FAIL) ;
                     }

                    object->vGetTargetPoint(&Top) ;                 /* Получаем положение целевой точки */  
                               break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
           } while(1) ;

/*--------------------------------- Обработка "идеального" положения */

                     iSaveErrorPipe(n_segm, &Top, &Top) ;           /* Сохраняем точку траектории */

       if(mAdjustErrorPipe) {
             status=object->vCheckFeatures(NULL) ;                  /* Проверяем непротиворечивость свойств */
          if(status) {                                              /* Если нарущение непротиворечивости свойств... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "Коллизия траектории") ;
                             return(_PATH_FAIL) ;
                     } 
                            }
/*--------------------------------- Формирование среза трубки ошибок */

   if(mAdjustErrorPipe ||
        mShowErrorPipe   ) {

           memcpy(pipe, p,  mDrives_cnt*sizeof(p[0])) ;             /* Занесение центральной точки */
                  pipe_cnt=1 ;                   

     for(i=0 ; i<mDrives_cnt ; i++) {                               /* Отработка ошибок */

            if(drives[i]==NULL)  continue ;

                  iErrorCalc(drives[i], NULL) ;                     /* Рассчет ошибки */

            if(drives[i]->d_err==0.)  continue ;

           memmove(&pipe[mDrives_cnt*pipe_cnt], pipe,               /* Раздваиваем точки среза */
                            mDrives_cnt*pipe_cnt*sizeof(*pipe)) ;

       for(j=0 ; j<pipe_cnt ; j++) {                                /* Вводим плюс/минус ошибку */
         pipe[mDrives_cnt* j          +i]=p[i]-drives[i]->d_err ;
         pipe[mDrives_cnt*(j+pipe_cnt)+i]=p[i]+drives[i]->d_err ;
                                   }
                                         pipe_cnt*=2 ;
                                    }

                           }
/*--------------------------------- Анализ точек среза трубки ошибок */

   if(mAdjustErrorPipe ||
        mShowErrorPipe   ) {

     for(i=0 ; i<pipe_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - - Рассчет положения объекта  */
       for(j=0 ; j<mDrives_cnt ; j++)
                     mDrives[j].value=pipe[i*mDrives_cnt+j] ;

                    object->vSetJoints    (mDrives, mDrives_cnt) ;
             status=object->vSolveByJoints() ;                      /* Решаем прямую задачу */
          if(status && mAdjustErrorPipe) {                          /* Если точка недостижима... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "Нарушение внутренних ограничений для трубки ошибок") ;
                                            return(_PIPE_FAIL) ;
                                         } 
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Обработка точки */
                  object->vGetTargetPoint(&Top_pipe) ;              /* Получаем положение целевой точки */

       if(mShowErrorPipe)  iSaveErrorPipe(n_segm, &Top, &Top_pipe); /* Сохраняем точку трубки ошибок */

       if(mAdjustErrorPipe) {
            status=object->vCheckFeatures(NULL) ;                   /* Проверяем непротиворечивость свойств */
         if(status) {                                               /* Если нарущение непротиворечивости свойств... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "Коллизия трубки ошибок") ;
                             return(_PIPE_FAIL) ;
                    } 
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }

                           }
/*------------------------------------------------ Цикл сканирования */
                                  }                                 /* CONTINUE.0 */
/*---------------------------- Проверка сходимости с конечной точкой */

   for(i=0 ; i<mDrives_cnt ; i++) {
                           p_acc[i]*=0.001 ;
                           v_acc[i]*=0.001 ;
                                  }

   for(i=0 ; i<mDrives_cnt ; i++) {
                                                                    
                 if(drives[i]==NULL)  continue ;                    

       if(fabs(p[i]-start[i+mDrives_cnt])>p_acc[i]) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "Непопадание в переходную точку "
                                "участков траектории при трассировке (%d:%d:%d)", mAttempt, n_segm, i) ;
                SEND_ERROR(tmp) ;
                     return(_FATAL_ERROR) ;
                                                    }  

       if(fabs( v[i]-v0[i+mDrives_cnt])>
          fabs(v0[i]-v0[i+mDrives_cnt])/500.) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "Непопадание в переходную скорость "
                                "участков траектории при трассировке (%d:%d:%d)", mAttempt, n_segm, i) ;
//              SEND_ERROR(tmp) ;
//                   return(_FATAL_ERROR) ;
                                               }
                                  }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   Сохранение точки трубки ошибок                  */

  int  RSS_Module_Spline::iSaveErrorPipe(      int  n_segm,
                                         RSS_Point *p0, 
                                         RSS_Point *p1 )
{
/*----------------------------------------------- Довыделение буфера */

   if(mErrorPipe_cnt==mErrorPipe_max) {

          mErrorPipe_max+=100 ;
          mErrorPipe     =(RSS_Module_Spline_Pipe *)
                            gMemRealloc(mErrorPipe,
                                        mErrorPipe_max*sizeof(RSS_Module_Spline_Pipe), 
                                          "RSS_Module_Spline::mErrorPipe", 0, 0) ;

       if(mErrorPipe==NULL) {
                   SEND_ERROR("SPLINE.iSaveErrorPipe@"
                              "Memory over for trajectory errors pipe") ;
                                  return(-1) ;
                            }
                                      }
/*------------------------------------------------- Сохранение точки */
                  
                  mErrorPipe[mErrorPipe_cnt].p0  =(*p0) ;
                  mErrorPipe[mErrorPipe_cnt].p1  =(*p1) ;
                  mErrorPipe[mErrorPipe_cnt].rad =(p0!=p1) ;
                  mErrorPipe[mErrorPipe_cnt].mark= n_segm ;
                             mErrorPipe_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              Обрезание трубки ошибок до данного сегмента          */

  int  RSS_Module_Spline::iCutErrorPipe(int  n_segm)

{
   for( ; mErrorPipe_cnt>0 ; mErrorPipe_cnt--)
     if(mErrorPipe[mErrorPipe_cnt-1].mark<n_segm)  break ;
    
   return(0) ;
}


/********************************************************************/
/*								    */
/*          Отображение трубки ошибок с передачей контекста         */

  void  RSS_Module_Spline::iErrorPipeShow_(TRAJECTORY *Trajectory)

{
    strcpy(mContextAction, "SHOW") ;
           mContextObject=Trajectory ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;
}


/*********************************************************************/
/*								     */
/*                     Отображение трубки ошибок                     */

  void  RSS_Module_Spline::iErrorPipeShow(TRAJECTORY *Trajectory)

{
                RSS_Context *context ;
  RSS_Module_Spline_Context *data ;
                        int  status ;
                        int  i ;

#define    T  Trajectory
#define    D  data

/*--------------------------------------------- Извлечение контекста */

          context=vAddData(&Trajectory->contexts) ;
       if(context      ==NULL ||
          context->data==NULL   ) {                                 /* Если не удалось разместить... */
                             SEND_ERROR("SPLINE.iErrorPipeShow@"
                                        "Трубка ошибок этой траектории не рассчитывалась") ;
                                             return ;
                                  }

          data=(RSS_Module_Spline_Context *)context->data ;

/*---------------------------------------- Отображение данных в файл */

//#define  __DEBUG__
#ifdef  __DEBUG__

  do {
        char  tmp[1024] ;

       for(i=0 ; i<D->ErrorPipe_cnt ; i++) 
         if(D->ErrorPipe[i].rad==0) {
            sprintf(tmp, "%d %12lf %12lf %12lf", D->ErrorPipe[i].mark, 
                                                 D->ErrorPipe[i].p0.x, 
                                                 D->ErrorPipe[i].p0.y, 
                                                 D->ErrorPipe[i].p0.z ) ;
             iDebug(tmp, NULL) ;
                                    }
     } while(0) ;

#endif

/*-------------------------------- Резервирование дисплейного списка */

     if(D->dlist1_idx==0) {

           D->dlist1_idx=RSS_Kernel::display.GetList(2) ;
                          }

     if(D->dlist1_idx==0)  return ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- Настройка контекста отображения */

             glNewList(D->dlist1_idx, GL_COMPILE) ;                 /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;

/*--------------------------------------------- Отрисовка траектории */

              glColor4d(GetRValue(D->ErrorPipeColor)/256., 
                        GetGValue(D->ErrorPipeColor)/256.,
                        GetBValue(D->ErrorPipeColor)/256., 1.) ;

                glBegin(GL_LINE_STRIP) ;

       for(i=0 ; i<D->ErrorPipe_cnt ; i++)
         if(D->ErrorPipe[i].rad==0)
             glVertex3d(D->ErrorPipe[i].p0.x, 
                        D->ErrorPipe[i].p0.y, 
                        D->ErrorPipe[i].p0.z ) ;

                  glEnd();

     if(mShowErrorPipe) {

                glBegin(GL_LINE_STRIP) ;

       for(i=0 ; i<D->ErrorPipe_cnt ; i++)
         if(D->ErrorPipe[i].rad==1) {
             glVertex3d(D->ErrorPipe[i].p0.x, D->ErrorPipe[i].p0.y, D->ErrorPipe[i].p0.z) ;
             glVertex3d(D->ErrorPipe[i].p1.x, D->ErrorPipe[i].p1.y, D->ErrorPipe[i].p1.z) ;
                                    }   
                  glEnd();

                        }
/*----------------------------- Восстановление контекста отображения */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef    T
}


/********************************************************************/
/*								    */
/*            Работа с формулой ошибки привода                      */

   int  RSS_Module_Spline::iErrorCalc(RSS_Module_Spline_Context *drive,
                                                           char *formula)
{
    static  RSS_Kernel *Calc ;
  struct RSS_Parameter  pars[10] ;
                   int  status ;
                  char  text[1024] ;
                  char  error[1024] ;
                   int  i ;

/*------------------------------------------ Определение Вычислителя */

   if(Calc==NULL) {

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("STD_EXPRESSION", NULL, NULL, NULL, 
                                                          NULL, NULL, NULL ) ;
         if(status==0)  break ;
                                     }

         if(i>=CALC_CNT) {
               SEND_ERROR("SPLINE@Не загружен вычислитель стандартных выражений") ;
                             return(-1) ;
                         }

                                Calc=CALC[i] ;

#undef   CALC_CNT
#undef   CALC
                  }
/*----------------------------------------------- Компиляция формулы */

   if(formula!=NULL) {
/*- - - - - - - - - - - - - - - - - - - - - - - - Пропись переменных */
                        memset(pars, 0, sizeof(pars)) ;

                        strcpy(pars[0].name, "V") ;
                               pars[0].ptr=&drive->v ;
                        strcpy(pars[1].name, "A") ;
                               pars[1].ptr=&drive->a ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Поджатие формулы */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Компиляция */
                          drive->calculate=NULL ;  // ???
 
          if(*formula==0)  return(0) ;
              
             status=Calc->vCalculate("STD_EXPRESSION",              /* Компилируем формулу */
                                      formula, pars, NULL,
                                     &drive->d_err,
                                     &drive->calculate, error) ;
          if(status==-1) {                                          /* Если ошибка... */
                              sprintf(text, "Ошибка вычислителя\n\n%s", error) ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     }
/*----------------------------------------------- Вычисление формулы */

   if(drive->calculate!=NULL) {

    if(!stricmp(formula,"0.01*V+0.01*A+0")) {
                                i=1 ;
                             }

                    Calc->vCalculate("STD_EXPRESSION", 
                                       NULL, NULL, NULL,
                                     &drive->d_err,
                                     &drive->calculate, NULL ) ;
                              }
   else                       {
                                        drive->d_err=0. ;
                              }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Module_Spline::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Module_Spline::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         

//return ;

    if(path==NULL) {
                             path="t_spline.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}
