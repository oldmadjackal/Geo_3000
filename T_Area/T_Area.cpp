/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ПОВЕРХНОСТИ"    		    */
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

#include "d:\_Projects\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Area.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  CREATE_DIALOG  CreateDialogIndirectParam

#define  _PI  3.1415926


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

     static   RSS_Module_Area  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_AREA_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_AREA_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	       ОПИСАНИЕ КЛАССА ЗАДАЧИ "АНАЛИЗ ПОВЕРХНОСТИ"         **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Area_instr  RSS_Module_Area_InstrList[]={

 { "help",       "?",  "#HELP (?) - список доступных команд", 
                        NULL,
                       &RSS_Module_Area::cHelp   },
 { "ilist",      "il", "#ILIST (IL) - выдает список индикаторов анализа поверхностей", 
                        NULL,
                       &RSS_Module_Area::cIndicatorList },
 { "ikill",      "ik", "#IKILL (IK) - удаляет индикаторов анализа", 
                       " IKILL          - удалить последний индикатор\n"
                       " IKILL <Индекс> - удалить индикатор с заданным индексом\n",
                       &RSS_Module_Area::cIndicatorKill },
 { "create",     "cr", "#CREATE - создать поверхность анализа",
                       " CREATE <Имя> <Вид>\n"
                       "   Создает именованный индикатор заданного вида:\n" 
                       "     FLAT - плоский\n"
                       "     CYL  - цилиндрический",
                       &RSS_Module_Area::cCreate },
 { "copy",       "cp", "#COPY - создать копию поверхности анализа",
                       " COPY <Имя_образца> <Имя_копии>\n",
                       &RSS_Module_Area::cCopy },
 { "base",       "b",  "#BASE - задать базовую точку поверхности анализа",
                       " BASE <Имя> <x> <y> <z>\n"
                       "   Задает базовую точку поверхности анализа\n"
                       " BASE/x <Имя> <x>\n"
                       "   Задает координату X базовой точки поверхности анализа\n"
                       "       (аналогично для Y и Z)\n"
                       " BASE/+x <Имя> <x>\n"
                       "   Задает приращение координаты X базовой точки поверхности анализа\n"
                       "       (аналогично для Y и Z)\n"
                       " BASE> <Имя>\n"
                       "   Задает клавиатурное управление базовой точкой поверхности анализа\n",
                       &RSS_Module_Area::cBase },
 { "angle",      "a",  "#ANGLE - задать углы ориентации поверхности анализа",
                       "           A (AZIMUTH)   - азимут\n"
                       "           E (ELEVATION) - возвышение\n"
                       "           R (ROLL)      - крен\n"
                       " ANGLE <Имя> <a> <e> <r>\n"
                       "   Задает углы ориентации поверхности анализа\n"
                       " ANGLE/a <Имя> <a>\n"
                       "   Задает угол ориентации A поверхности анализа\n"
                       "       (аналогично для E и R)\n"
                       " ANGLE/+a <Имя> <a>\n"
                       "   Задает приращение угла ориентации A поверхности анализа\n"
                       "       (аналогично для E и R)\n"
                       " ANGLE> <Имя>\n"
                       "   Задает клавиатурное управление углами ориентации поверхности анализа\n",
                       &RSS_Module_Area::cAngle },
 { "size",       "s",  "#SIZE - изменить размер поля поверхности анализа",
                       " SIZE <Имя> <size1>[ <size2>...]\n"
                       "   Задает размеры поля поверхности анализа\n"
                       " SIZE/N <Имя> <sizeN>\n"
                       "   Задает размер N поверхности анализа\n"
                       " SIZE/+N <Имя> <sizeN>\n"
                       "   Задает приращение размера N поверхности анализа\n"
                       " SIZE> <Имя>\n"
                       "   Задает клавиатурное управление размерами поверхности анализа\n",
                       &RSS_Module_Area::cSize },
 { "info",       "i",  "#INFO (I) - информация о поверхности анализа", 
                        NULL,
                        NULL },
 { "table",      "t",  "#TABLE (T) - задание таблицы углов подхода",
                        NULL,
                       &RSS_Module_Area::cTable },
 { "execute",    "ex", "#EXECUTE (EX) - выполнить анализ поверхности",
                       " EXECUTE[/D] <Индикатор> <Имя>[:<Неопр.>] <Azim> <Elev> <Roll>\n"
                       "   Проанализировать при заданных углах азимута, крена и тангажа\n"
                       " EXECUTE[/D] <Индикатор> <Имя>[:<Неопр.>]\n"
                       "   Проанализировать по таблице углов подхода\n",
                       &RSS_Module_Area::cExecute },
  {  NULL }
                                                            } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Area_instr *RSS_Module_Area::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Area::RSS_Module_Area(void)

{

	   keyword="Geo" ;
    identification="Area_task" ;

        mInstrList=RSS_Module_Area_InstrList ;

     memset(mIndicators, 0, sizeof(mIndicators)) ;

            mIndicator_last=NULL ;

                  mBadColor=  0 ;
                 mScanColor=RGB(0, 128,   0) ;
                 mGoodColor=RGB(0, 128, 128) ;

                    mNoShow=0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Area::~RSS_Module_Area(void)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Area::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "AREA"
#define  _SECTION_SHRT_NAME   "AR"

/*--------------------------------------------- Идентификация секции */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;

   if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                       }
   else                {
                             end=command ;
                       }
/*----------------------- Включение/выключение режима прямой команды */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  Включение */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Area:") ;
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

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  Пытаемся передать модулю ядра... */
       if(status)  SEND_ERROR("Секция AREA: Неизвестная команда") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* Выполнение команды */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_Area::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Area::vReadSave(std::string *data)

{
               char *buff ;
                int  buff_size ;
    RSS_Object_Area *object ;
               char  name[32] ;
                int  type ;
               char  type_txt[32] ;
               char  command[128] ;
                int  status ;
               char *entry ;
               char *end ;
                int  i ;

#define    I  object->Indicator

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE AREA\n", 
                      strlen("#BEGIN MODULE AREA\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT AREA\n", 
                      strlen("#BEGIN OBJECT AREA\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT AREA\n", 
              strlen("#BEGIN OBJECT AREA\n"))) {                    /* IF.1 */
/*- - - - - - - - - - - - - - - - - - - - - -  Извлечение параметров */
              memset(name, 0, sizeof(name)) ;
                           entry=strstr(buff, "NAME=") ;            /* Извлекаем имя объекта */
             strncpy(name, entry+strlen("NAME="), sizeof(name)-1) ;
          end=strchr(name, '\n') ;
         *end= 0 ;

                       entry=strstr(buff, "TYPE=") ;                /* Извлекаем модель объекта */
           type=strtol(entry+strlen("TYPE="), &end, 10) ;
/*- - - - - - - - - - - - - - - Проверка повторного создания объекта */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
         if(type==_CYLINDER_IND)  strcpy(type_txt, "CYL" ) ;
         else                     strcpy(type_txt, "FLAT") ;
  
                       this->mNoShow=1 ;
                             sprintf(command, "%s %s", name, type_txt) ;
                status=this->cCreate(command) ;
                       this->mNoShow=0 ;

             if(status)  return ;

        object=(RSS_Object_Area *)this->kernel->kernel_objects[this->kernel->kernel_objects_cnt-1] ;
/*- - - - - - - - - - - - - - - - - - - -  Параметры области анализа */
       entry=strstr(buff, "SIZE_1=") ;  I->size1    =atof(entry+strlen("SIZE_1=")) ;
       entry=strstr(buff, "SIZE_2=") ;  I->size2    =atof(entry+strlen("SIZE_2=")) ;
       entry=strstr(buff, "CELL="  ) ;  I->cell     =atof(entry+strlen("CELL=")) ;
       entry=strstr(buff, "MOVE="  ) ;  I->move_flag=atoi(entry+strlen("MOVE=")) ;
/*- - - - - - - - - - - - Пропись базовой точки и ориентации объекта */
       entry=strstr(buff, "X_BASE=") ;  I->base.x   =atof(entry+strlen("X_BASE=")) ;
       entry=strstr(buff, "Y_BASE=") ;  I->base.y   =atof(entry+strlen("Y_BASE=")) ;
       entry=strstr(buff, "Z_BASE=") ;  I->base.z   =atof(entry+strlen("Z_BASE=")) ;
       entry=strstr(buff, "A_AZIM=") ;  I->base.azim=atof(entry+strlen("A_AZIM=")) ;
       entry=strstr(buff, "A_ELEV=") ;  I->base.elev=atof(entry+strlen("A_ELEV=")) ;
       entry=strstr(buff, "A_ROLL=") ;  I->base.roll=atof(entry+strlen("A_ROLL=")) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - Данные индикации */         
        status=iIndicatorAlloc(I) ;
     if(status) {
                    SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                     return ;
                }

       entry=strstr(buff, "DATA_CNT=") ;  I->data_cnt=atoi(entry+strlen("DATA_CNT=")) ;

     if(I->data_cnt>0) {

                 i= 0 ;
              entry=strstr(buff, "DATA=") ;
        while(entry!=NULL) {
                                        end=entry+strlen("DATA") ;
               do {
                     I->data[i]=strtoul(end+1, &end, 16) ;
                             i++ ;
                  } while(*end==',') ;

              entry=strstr(entry+1, "DATA=") ;
                           }  

                       }
/*- - - - - - - - - - - - - - - - - - - - - - - Отрисовка индикатора */
                          iIndicatorShow_(I) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }                    /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/

#undef    I
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Area::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE AREA\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*             Выполнить действие в контексте потока                */

    void  RSS_Module_Area::vChangeContext(void)

{
/*-------------------- Определение параметров графического контекста */

   if(!stricmp(mContextAction, "GET_RANGE")) {

                     RSS_Kernel::display.SetFirstContext("Show") ;
        this->mRange=RSS_Kernel::display.GetContextPar  ("Range") ;
                                             }
/*------------------------------------------- Отображение индикатора */

   if(!stricmp(mContextAction, "SHOW")) {

                      iIndicatorShow(mContextObject) ;
                                        }
/*----------------------------------------- Вывод списка индикаторов */

   if(!stricmp(mContextAction, "LIST")) {

    this->mIndListWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_INDICATORS_LIST", RT_DIALOG),
           	                      NULL, Task_Area_IList_dialog, 
                                       (LPARAM)this ) ;
                        ShowWindow(this->mIndListWnd, SW_SHOW) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Area::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Area_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      Реализация инструкции INDICATORS_LIST                 */

  int  RSS_Module_Area::cIndicatorList(char *cmd)

{
    strcpy(mContextAction, "LIST") ;
           mContextObject=NULL ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*             Реализация инструкции INDICATORS_KILL                */

  int  RSS_Module_Area::cIndicatorKill(char *cmd)

{
    int  idx ;
   char *end ;

/*----------------------------------- Удаление последнего индикатора */

   if(*cmd==0) {
                  if(mIndicator_last==NULL)  return(0) ;

                      this->iIndicatorKill(mIndicator_last) ;
                       this->kernel->vShow("REFRESH") ;

                         mIndicator_last=NULL ;                 
               }
/*----------------------------------- Удаление индикатора по индексу */

   else        {

                    idx=strtoul(cmd, &end, 10) ;

        if(*end!=  0      ||
            idx>=_IND_MAX   ) {
                        SEND_ERROR("Секция AREA: Неправильный индекс индикатора") ;
                                      return(-1) ;
                              }

        if(mIndicators[idx]==NULL) {
                        SEND_ERROR("Секция AREA: Такого индикатора не существует") ;
                                      return(-1) ;
                                   }

                      this->iIndicatorKill(mIndicators[idx]) ;
                       this->kernel->vShow("REFRESH") ;
               }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя> <Тип>                                          */

  int  RSS_Module_Area::cCreate(char *cmd)

{
  RSS_Module_Area_Indicator *Indicator ;
            RSS_Object_Area *object ;
                       char *name ;
                       char *type ;
                       char *end ;
                     double  range ;
                        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- Дешифровка командной строки */

                            type ="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя индикатора */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                  type=end+1 ;                                      /* Извлекаем вид индикатора */
                   end=strchr(type, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
      } while(0) ;                                                  /* BLOCK.1 */

/*--------------------------------------------------- Контроль имени */

   if(name[0]== 0  ||
      name[0]==' '   ) {

             SEND_ERROR("Секция AREA: Не задано имя индикатора") ;
                                return(-1) ;
                       }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
              SEND_ERROR("Секция AREA: Объект с таким именем уже существует") ;
                                return(-1) ;
                                              }
/*---------------------------------------------------- Контроль типа */

   if(type[0]== 0  ||
      type[0]==' '   ) {
             SEND_ERROR("Секция AREA: Не задан вид индикатора") ;
                                return(-1) ;
                       }

   if(stricmp(type, "FLAT") &&
      stricmp(type, "CYL" )   ) {
             SEND_ERROR("Секция AREA: Неизвестный вид индикатора") ;
                                            return(-1) ;
                                }
/*-------------- Получение параметров главного контекста отображения */

            strcpy(mContextAction, "GET_RANGE") ;
                   mContextObject=NULL ;

          SendMessage(RSS_Kernel::kernel_wnd, 
                        WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                                 (LPARAM) this                ) ;

                   range=this->mRange ;

/*---------------------------------------------- Создание индикатора */

     for(i=0 ; i<_IND_MAX ; i++)                                    /* Поиск свободного слота */
       if(mIndicators[i]==NULL)  break ;

       if(i>=_IND_MAX) {
             SEND_ERROR("Список анализируемых поверхностей слишком велик.\n"
                        "Очистите его"                                      ) ;
                                return(-1) ;
                       }

          Indicator=(RSS_Module_Area_Indicator *)
                       calloc(1, sizeof(RSS_Module_Area_Indicator)) ;
       if(Indicator==NULL) {
                    SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                return(-1) ;
                           }

                       Indicator->context    =this ;
                       Indicator->idx        = i ;
                       Indicator->move_flag  = 1 ;
                       Indicator->formed_flag= 0 ;

               strncpy(Indicator->name, name, sizeof(Indicator->name)-1) ;

        mIndicators[i]=Indicator ;
/*- - - - - - - - - - - - - - - - -  Поверхность анализа - Плоскость */
       if(!stricmp(type, "FLAT")) {

                      Indicator->type =_FLAT_IND ;
                      Indicator->cell = range/40. ;
                      Indicator->size1= range/2. ;
                      Indicator->size2= range/2. ;
                                  }
/*- - - - - - - - - - - - - - - - - -  Поверхность анализа - Цилиндр */
       if(!stricmp(type, "CYL" )) {

                      Indicator->type =_CYLINDER_IND ;
                      Indicator->cell = range/40. ;
                      Indicator->size1= range/2. ;
                      Indicator->size2= range/2. ;
                                  }
/*---------------------- Введение Индикатора в общий список объектов */

       object=new RSS_Object_Area ;
    if(object==NULL) {
              SEND_ERROR("Секция AREA: Недостаточно памяти для создания индикатора") ;
                        return(-1) ;
                     }

              object->Indicator=Indicator ;  
       strcpy(object->Name, name) ;

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция AREA: Переполнение памяти") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)name) ;

/*------------------------------------------------ Отображение сцены */

   if(!mNoShow) {
                           iIndicatorShow_(Indicator) ;

                       this->kernel->vShow(NULL) ;
//                     this->kernel->vShow("REFRESH") ;
                }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции COPY                    */
/*								    */
/*      COPY <Имя образца> <Имя копии>                              */

  int  RSS_Module_Area::cCopy(char *cmd)

{
  RSS_Module_Area_Indicator *Ind_src ;
  RSS_Module_Area_Indicator *Ind_dst ;
            RSS_Object_Area *object ;
                       char *name ;
                       char *copy ;
                       char *end ;
                        int  i ;

#define   OBJECTS       this->kernel->kernel_objects
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- Дешифровка командной строки */

                           copy="" ;

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя образца */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

                  copy=end+1 ;                                      /* Извлекаем имя копии */
                   end=strchr(copy, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
          
      } while(0) ;                                                  /* BLOCK.1 */

/*------------------------------------------- Контроль имени образца */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя образца.\n"
                                 "Например: COPY <Имя образца> ...") ;
                                     return(-1) ;
                   }

       Ind_src=FindIndicator(name) ;                                /* Ищем индикатор-образец по имени */
    if(Ind_src==NULL)  return(-1) ;

/*--------------------------------------------- Контроль имени копии */

   if(copy[0]== 0  ||
      copy[0]==' '   ) {
             SEND_ERROR("Секция AREA: Не заданo имя копии") ;
                                return(-1) ;
                       }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, copy)) {
              SEND_ERROR("Секция AREA: Объект с таким именем уже существует") ;
                                return(-1) ;
                                                    }
/*---------------------------------------------- Создание индикатора */

     for(i=0 ; i<_IND_MAX ; i++)                                    /* Поиск свободного слота */
       if(mIndicators[i]==NULL)  break ;

       if(i>=_IND_MAX) {
             SEND_ERROR("Список анализируемых поверхностей слишком велик.\n"
                        "Очистите его"                                      ) ;
                                return(-1) ;
                       }

          Ind_dst=(RSS_Module_Area_Indicator *)
                       calloc(1, sizeof(RSS_Module_Area_Indicator)) ;
       if(Ind_dst==NULL) {
                    SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                return(-1) ;
                         }

                       Ind_dst->context    =this ;
                       Ind_dst->idx        = i ;
                       Ind_dst->move_flag  = 1 ;
                       Ind_dst->formed_flag= 0 ;

               strncpy(Ind_dst->name, copy, sizeof(Ind_dst->name)-1) ;

                       Ind_dst->type =Ind_src->type ;
                       Ind_dst->cell =Ind_src->cell ;
                       Ind_dst->size1=Ind_src->size1 ;
                       Ind_dst->size2=Ind_src->size2 ;
                       Ind_dst->base =Ind_src->base ;

        mIndicators[i]=Ind_dst ;

/*---------------------- Введение Индикатора в общий список объектов */

       object=new RSS_Object_Area ;
    if(object==NULL) {
              SEND_ERROR("Секция AREA: Недостаточно памяти для создания индикатора") ;
                        return(-1) ;
                     }

              object->Indicator=Ind_dst ;  
       strcpy(object->Name, copy) ;

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция AREA: Переполнение памяти") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)copy) ;

/*------------------------------------------------ Отображение сцены */

                   iIndicatorShow_(Ind_dst) ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BASE                    */
/*								    */
/*        BASE    <Имя> <X> <Y> <Z>                                 */
/*        BASE/X  <Имя> <X>                                         */
/*        BASE/+X <Имя> <X>                                         */
/*        BASE    <Имя> <Объект>                                    */
/*        BASE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        BASE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Area::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

                       char  *pars[_PARS_MAX] ;
                       char  *name ;
                       char **xyz ;
                     double   coord[_COORD_MAX] ;
                        int   coord_cnt ;
                     double   inverse ;
  RSS_Module_Area_Indicator  *Indicator ;
                        int   xyz_flag ;          /* Флаг режима одной координаты */
                        int   delta_flag ;        /* Флаг режима приращений */
                        int   arrow_flag ;        /* Флаг стрелочного режима */
                       char  *arrows ;
                       char  *error ;
                       char  *end ;
                        int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'x')!=NULL ||
                   strchr(cmd, 'X')!=NULL   )    xyz_flag='X' ;
           else if(strchr(cmd, 'y')!=NULL ||
                   strchr(cmd, 'Y')!=NULL   )    xyz_flag='Y' ;
           else if(strchr(cmd, 'z')!=NULL ||
                   strchr(cmd, 'Z')!=NULL   )    xyz_flag='Z' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='X' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='X' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='Y' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='Z' ;  inverse=-1. ;  }
           
                          xyz=&pars[2] ;
                   }
/*---------------------------------------- Контроль имени индикатора */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя индикатора. \n"
                                 "Например: BASE <Имя_индикатора> ...") ;
                                     return(-1) ;
                   }

       Indicator=FindIndicator(name) ;                              /* Ищем объект по имени */
    if(Indicator==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='X')   Indicator->base.x+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   Indicator->base.y+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   Indicator->base.z+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='X')   Indicator->base.x=coord[0] ;
     else if(xyz_flag=='Y')   Indicator->base.y=coord[0] ;                 
     else if(xyz_flag=='Z')   Indicator->base.z=coord[0] ;
     else                   {
                              Indicator->base.x=coord[0] ;
                              Indicator->base.y=coord[1] ;
                              Indicator->base.z=coord[2] ;
                            }
                  }
/*------------------------------------------------ Отображение сцены */

                   iIndicatorShow_(Indicator) ;

               this->kernel->vShow(NULL) ;
//             this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции SIZE                    */
/*								    */
/*        SIZE    <Имя> <Size1> <Size2> ...                         */
/*        SIZE/N  <Имя> <SizeN>                                     */
/*        SIZE/+N <Имя> <SizeN>                                     */
/*        SIZE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        SIZE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Area::cSize(char *cmd)

{
#define  _COORD_MAX   16
#define   _PARS_MAX   17

                       char  *pars[_PARS_MAX] ;
                       char  *name ;
                       char **xyz ;
                     double   coord[_COORD_MAX] ;
                        int   coord_cnt ;
                     double   inverse ;
  RSS_Module_Area_Indicator  *Indicator ;
                        int   xyz_flag ;          /* Флаг режима одной координаты */
                        int   delta_flag ;        /* Флаг режима приращений */
                        int   arrow_flag ;        /* Флаг стрелочного режима */
                       char  *arrows ;
                       char  *error ;
                       char  *end ;
                        int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL)  delta_flag= 1 ;

                if(strchr(cmd, '1')!=NULL)    xyz_flag='1' ;
           else if(strchr(cmd, '2')!=NULL)    xyz_flag='2' ;
           else if(strchr(cmd, '3')!=NULL)    xyz_flag='3' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='1' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='1' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='2' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='2' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='3' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='3' ;  inverse=-1. ;  }
           
                          xyz=&pars[2] ;
                   }
/*---------------------------------------- Контроль имени индикатора */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя индикатора. \n"
                                 "Например: SIZE <Имя_индикатора> ...") ;
                                     return(-1) ;
                   }

       Indicator=FindIndicator(name) ;                              /* Ищем объект по имени */
    if(Indicator==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <1)  error="Должны быть указаны размеры" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='1')   Indicator->size1+=inverse*coord[0] ;
     else if(xyz_flag=='2')   Indicator->size2+=inverse*coord[0] ;                 
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='1')   Indicator->size1=coord[0] ;
     else if(xyz_flag=='2')   Indicator->size2=coord[0] ;                 
     else                   {
                              Indicator->size1=coord[0] ;
                              Indicator->size2=coord[1] ;
                            }
                  }
/*------------------------------------------------ Отображение сцены */

                                  Indicator->move_flag  =1 ;
                                  Indicator->formed_flag=0 ;
                  iIndicatorShow_(Indicator) ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ANGLE                   */
/*								    */
/*       ANGLE    <Имя> <A> <E> <R>                                 */
/*       ANGLE/A  <Имя> <A>                                         */
/*       ANGLE/+A <Имя> <A>                                         */
/*       ANGLE>   <Имя> <Код стрелочки> <Шаг>                       */
/*       ANGLE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Area::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

                       char  *pars[_PARS_MAX] ;
                       char  *name ;
                       char **xyz ;
                     double   coord[_COORD_MAX] ;
                        int   coord_cnt ;
                     double   inverse ;
  RSS_Module_Area_Indicator  *Indicator ;
                        int   xyz_flag ;          /* Флаг режима одной координаты */
                        int   delta_flag ;        /* Флаг режима приращений */
                        int   arrow_flag ;        /* Флаг стрелочного режима */
                       char  *arrows ;
                       char  *error ;
                       char  *end ;
                        int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                        xyz_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL   )  delta_flag= 1 ;

                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }

  else if(*cmd=='>') {
                           delta_flag=1 ;
                           arrow_flag=1 ;

                          cmd=strchr(cmd, ' ') ;
                       if(cmd==NULL)  return(0) ;
                          cmd++ ;
                     } 
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   
                  inverse=   1. ; 

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[1] ;

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }

      if(strstr(arrows, "left" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL &&  
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }  
           
                          xyz=&pars[2] ;   
                   }
/*---------------------------------------- Контроль имени индикатора */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя индикатора. \n"
                                 "Например: BASE <Имя_индикатора> ...") ;
                                     return(-1) ;
                   }

       Indicator=FindIndicator(name) ;                              /* Ищем объект по имени */
    if(Indicator==NULL)  return(-1) ;

/*------------------------------------------------- Разбор координат */

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <3)  error="Должно быть указаны 3 координаты" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='A')   Indicator->base.azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   Indicator->base.elev+=inverse*coord[0] ;                 
     else if(xyz_flag=='R')   Indicator->base.roll+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='A')   Indicator->base.azim=coord[0] ;
     else if(xyz_flag=='E')   Indicator->base.elev=coord[0] ;                 
     else if(xyz_flag=='R')   Indicator->base.roll=coord[0] ;
     else                   {
                              Indicator->base.azim=coord[0] ;
                              Indicator->base.elev=coord[1] ;
                              Indicator->base.roll=coord[2] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(Indicator->base.azim> 180.)  Indicator->base.azim-=360. ;
     while(Indicator->base.azim<-180.)  Indicator->base.azim+=360. ;

     while(Indicator->base.elev> 180.)  Indicator->base.elev-=360. ;
     while(Indicator->base.elev<-180.)  Indicator->base.elev+=360. ;

     while(Indicator->base.roll> 180.)  Indicator->base.roll-=360. ;
     while(Indicator->base.roll<-180.)  Indicator->base.roll+=360. ;

/*------------------------------------------------ Отображение сцены */

                  iIndicatorShow_(Indicator) ;

              this->kernel->vShow("REFRESH") ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TABLE                   */
/*								    */
/*      TABLE <Имя>                                                 */

  int  RSS_Module_Area::cTable(char *cmd)

{
#define   _PARS_MAX   4
                       char *pars[_PARS_MAX] ;
                       char *name ;
  RSS_Module_Area_Indicator *Indicator ;
                       char *end ;
                        int  status ;
                        int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */
    for(i=0 ; i<_PARS_MAX ; i++)  pars[i]=NULL ;

    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {

                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя анализируемой поверхности.\n"
                                      "Например: TABLE <Имя> ...") ;
                                     return(-1) ;
                         }

       Indicator=FindIndicator(name) ;                              /* Ищем объект по имени */
    if(Indicator==NULL)  return(-1) ;

/*--------------------------------------- Считывание данных из файла */

  if(pars[1]!=NULL) {
                             status=iReadTableFile(Indicator, pars[1]) ;
                      return(status) ;
                    }
/*----------------------------------------------- Проведение диалога */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_ANGLES", RT_DIALOG),
			             GetActiveWindow(),
                                     Task_Area_Angles_dialog,
                                    (LPARAM)Indicator         ) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/*********************************************************************/
/*								     */
/*		      Реализация инструкции EXECUTE                  */
/*   EXECUTE[/D] <Индикатор> <Имя>[:<Неопр.>] <Azim> <Elev> <Roll>   */
/*   EXECUTE[/D] <Индикатор> <Имя>[:<Неопр.>]                        */
/*                                                                   */
/*  <Неопр.> - Критерий разрешения неопределенности:                 */
/*              - пустой, если используется предыдущий вариант       */
/*              - последовательность цифр 0 и 1, задающая вариант    */
/*         D - отрисовывать положения                                */

  int  RSS_Module_Area::cExecute(char *cmd)

{
#define  _COORD_MAX   6
#define   _PARS_MAX   7

                        char  *pars[_PARS_MAX] ;
                        char  *ind_name ;
                        char  *sub_name ;
                        char **xyz ;
                      double   coord[_COORD_MAX] ;
                         int   coord_cnt ;
   RSS_Module_Area_Indicator  *Indicator ;
                   RSS_Point   Target ;
                  RSS_Object  *object ;
                    COLORREF   PrevColor ;       /* Цвет доступности */ 
                         int   draw_flag ;       /* Флаг отрисовки манипулятора */
               static   char   undef_decl[128] ;
                        char   undef[128] ;
                         int   undef_cnt ;
                         int   idx ;
                         int   idx_prv ;
                         int   status ;
                         int   bad ;
                        char  *end ;
                        char  *error ;
                         int   i ;
                         int   j ;
                         int   k ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- Инициализация */

/*---------------------------------------- Разборка командной строки */

                       draw_flag=0 ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
       if(*cmd=='/') { 

                              cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'd')!=NULL ||
                   strchr(cmd, 'D')!=NULL   )      draw_flag= 1 ;

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

                 ind_name= pars[0] ;
                 sub_name= pars[1] ;
                      xyz=&pars[2] ;   

/*---------------------------------------- Контроль имени индикатора */

    if(ind_name==NULL) {                                            /* Если имя не задано... */
                      SEND_ERROR("Не задано имя анализируемой поверхности.\n") ;
                                     return(-1) ;
                       }

       Indicator=FindIndicator(ind_name) ;                          /* Ищем объект по имени */
    if(Indicator==NULL)  return(-1) ;

/*------------------------ Спецификация разрешения неопределенностей */

    do {                                                            /* BLOCK.1 */      
              end=strchr(sub_name, ':') ;
           if(end==NULL) {
                            undef_decl[0]=0 ; 
                                break ;
                         }
             *end=0 ;
              end++ ; 

           if(*end!=0)  strcpy(undef_decl, end) ;

       } while(0) ;                                                 /* BLOCK.1 */      

/*---------------------------- Контроль имени анализируемого объекта */

    if(sub_name==NULL) {                                            /* Если имя не задано... */
                      SEND_ERROR("Не задано имя анализируемого объекта.\n") ;
                                     return(-1) ;
                       }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, sub_name)) {  object=OBJECTS[i] ;
                                                                break ;  }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Анализируемого объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*------------------------------------------------- Разбор координат */

                      memset(coord, 0, sizeof(coord)) ;

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;

        if(coord_cnt >1 &&
           coord_cnt!=3   )  error="Должны быть указаны углы азимута, возвышения и крена" ;

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------- Формирование таблицы углов подхода */

     if(coord_cnt) {

               memset(Indicator->angles, 0, sizeof(Indicator->angles)) ;

                      Indicator->angles[0].use_flag            =  1 ; 
                      Indicator->angles[0].color               = this->mGoodColor; 
                      Indicator->angles[0].azim_range.type     =_INLIST_RANGE ;
                      Indicator->angles[0].azim_range.a_lst[0] =  coord[0] ;
                      Indicator->angles[0].azim_range.a_lst_cnt=   1 ;
               strcpy(Indicator->angles[0].azim_range.a_lst_text, xyz[0]) ;
                      Indicator->angles[0].elev_range.type     =_INLIST_RANGE ; 
                      Indicator->angles[0].elev_range.a_lst[0] =  coord[1] ; 
                      Indicator->angles[0].elev_range.a_lst_cnt=   1 ;
               strcpy(Indicator->angles[0].elev_range.a_lst_text, xyz[1]) ;
                      Indicator->angles[0].roll_range.type     =_INLIST_RANGE ; 
                      Indicator->angles[0].roll_range.a_lst[0] =  coord[2] ; 
                      Indicator->angles[0].roll_range.a_lst_cnt=   1 ;
               strcpy(Indicator->angles[0].roll_range.a_lst_text, xyz[2]) ;
                      Indicator->angle_step                    =  10. ;
                   }
     else          {
                   }
/*----------------------------------------------- Подготовка анализа */

                          undef_cnt=object->vGetAmbiguity() ;
    if(undef_decl[0]!=0)  undef_cnt=  0 ;

         status=iIndicatorAlloc(Indicator) ;
      if(status) {
                    SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                     return(-1) ;
                 }
/*----------------------------------------------------- Цикл анализа */

                            Indicator->move_flag= 0 ;
                                        idx_prv =-1 ;
                                            bad = 0 ;

       for(i=0 ; ; i++) {                                           /* CIRCLE.1 - Сканирование поверхности */
/*- - - - - - - - - - - - - - - - - - -  Задание анализируемой точки */
             status=iGetNextPoint(Indicator, i, &Target, &idx, &PrevColor, bad) ;

          if(idx_prv>= 0 &&                                         /* Запись результатов анализа */
             idx_prv!=idx  )  Indicator->data[idx_prv]=PrevColor ;  /*  предыдущей точки          */
                              Indicator->data_cnt     = idx ; 

              idx_prv=idx ;

          if(status<0)  break ;                                     /* Если все точки перебраны */
/*- - - - - - - - - - - - - - - - - - - Разрешение неопределенностей */
        for(j=0 ; j<(1<<undef_cnt) ; j++) {                         /* CIRCLE.2 - Перебор вариантов схемы */

                            memset(undef, 0, sizeof(undef)) ;

             if(undef_decl[0]!=0) {
                                      strcpy(undef, undef_decl) ;
                                  }
             else                 {

                   for(k=0 ; k<undef_cnt ; k++)
                     if(j & (1<<k))  undef[k]='1' ;
                     else            undef[k]='0' ;
                                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Разрешение ОКЗ */
                       object->vSetAmbiguity  (undef) ;             /* Уст.критерия разрешения неопределенности */
                       object->vSetTargetPoint(&Target, 1) ;        /* Уст.целевую точку */
                       object->vErrorMessage  (0) ;
                   bad=object->vSolveByTarget () ;                  /* Решаем обратную задачу */
         if(!bad)  bad=object->vCheckFeatures (NULL) ;              /* Проверяем непротиворечивость свойств */
                       object->vErrorMessage  (1) ;
/*- - - - - - - - - - - - - - - - - - - - -  Управление отображением */
         if(  j==0   ) {                 Indicator->formed_flag=0 ;
                         iIndicatorShow_(Indicator) ;               }

         if(draw_flag)  this->kernel->vShow(NULL) ;
         else           this->kernel->vShow("REFRESH") ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
         if(bad==0)  break ;                                        /* Если наиден "успешный" вариант..." */
                                          }                         /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }                                /* CONTINUE.1 */
/*----------------------- Окончательное отображение данных на экране */

                            Indicator->formed_flag=0 ;
            iIndicatorShow_(Indicator) ;

                  this->kernel->vShow(NULL) ;

/*----------------------------------------- Восстановление контекста */

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск индикатора по имени                        */

  RSS_Module_Area_Indicator *RSS_Module_Area::FindIndicator(char *name)

{
     char   text[1024] ;
      int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- Поиск объекта по имени */ 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Индикатора с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------------- Контроль типа объекта */

     if(strcmp(OBJECTS[i]->Type, "Area")) {

           SEND_ERROR("Объект не является Поверхностью Анализа") ;
                            return(NULL) ;
                                          }
/*-------------------------------------------------------------------*/ 

   return(((RSS_Object_Area *)OBJECTS[i])->Indicator) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/*********************************************************************/
/*								     */
/*                     Расшифровка кода ошибки                       */

  char *RSS_Module_Area::iErrorDecode(int  error)

{
   if(error==_RSS_OBJECT_NOTARGETED        )  return("Анализируемый объект является НЕИЗМЕНЯЕМЫМ") ;
   if(error==_RSS_OBJECT_UNATTAINABLE_POINT)  return("Недостижимая точка") ;
   if(error==_RSS_OBJECT_DEGREES_LIMIT     )  return("Нарушение диапазона значений в степени подвижности") ;
   if(error==_RSS_OBJECT_LOW_ACTIVE        )  return("Число степеней подвижности меньше 6") ;
   if(error==_RSS_OBJECT_MATCH_ACTIVE      )  return("Число степеней подвижности больше 6") ;
   if(error==_RSS_OBJECT_BAD_SCHEME        )  return("Некорректная схема") ;
   if(error==_RSS_OBJECT_UNKNOWN_ERROR     )  return("Неизвестная ошибка") ;
                                              return("Код ошибки неопределен") ;
}


/********************************************************************/
/*								    */
/*             Отображение индикатора с передачей контекста         */

  void  RSS_Module_Area::iIndicatorShow_(RSS_Module_Area_Indicator *Indicator)

{
    strcpy(mContextAction, "SHOW") ;
           mContextObject=Indicator ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;
}


/*********************************************************************/
/*								     */
/*                     Отображение индикатора                        */

  void  RSS_Module_Area::iIndicatorShow(RSS_Module_Area_Indicator *Indicator)

{
  COLORREF  color ;
       int  status ;
       int  col_cnt ;
       int  s1_idx ;
       int  s2_idx ;
       int  i ;

#define    I  Indicator

/*-------------------------------- Резервирование дисплейного списка */

     if(I->dlist1_idx==0) {

           I->dlist1_idx=RSS_Kernel::display.GetList(2) ;
           I->dlist2_idx=RSS_Kernel::display.GetList(1) ;
                          }

     if(I->dlist1_idx==0 ||
        I->dlist2_idx==0   )  return ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- Настройка контекста отображения */

      if(!I->formed_flag) {                                         /* BLOCK.1 - Блок отрисовки узора */
                               I->formed_flag=1 ;

             glNewList(I->dlist2_idx, GL_COMPILE) ;                 /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
              glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
           glDepthMask(0) ;                                         /* Откл.запись Z-буфера */

/*-------------------------------------------- Индикатор - плоскость */

    if(I->type==_FLAT_IND) {

           double  w ;
           double  h ;

                           w=I->size1/2. ;
                           h=I->size2/2. ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Настроечный вид */
     if(I->move_flag) {

              glColor4d(GetRValue(this->mScanColor)/256., 
                        GetGValue(this->mScanColor)/256.,
                        GetBValue(this->mScanColor)/256., 0.3) ;

                glBegin(GL_POLYGON) ;
             glVertex3d( w, -h, 0.) ;
             glVertex3d( w,  h, 0.) ;
             glVertex3d(-w,  h, 0.) ;
             glVertex3d(-w, -h, 0.) ;
	          glEnd();

                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - Результирующий вид */
     else             {

#define  _MAX_1   ((int)(Indicator->size1/Indicator->cell+0.1))
#define  _MAX_2   ((int)(Indicator->size2/Indicator->cell+0.1))

                i=0 ;

       for(s2_idx=0 ; s2_idx<_MAX_2 ; s2_idx++) 
       for(s1_idx=0 ; s1_idx<_MAX_1 ; s1_idx++) {

              if(i>  I->data_cnt ) {  s2_idx=_MAX_2 ;  break ;  }
              if(i>=_MAX_1*_MAX_2) {  s2_idx=_MAX_2 ;  break ;  }

              if(i==I->data_cnt)  color=mScanColor ;
              else                color=I->data[i] ;

                 i++ ;
              
              if(color==0)  continue ;

                w=-I->size1/2.+s1_idx*I->cell+I->cell/2. ;          /* Рассчет координат точки на */
                h=-I->size2/2.+s2_idx*I->cell+I->cell/2. ;          /*   поверхности анализа      */

              glColor4d(GetRValue(color)/256., 
                        GetGValue(color)/256.,
                        GetBValue(color)/256., 0.3) ;

                glBegin(GL_POLYGON) ;
             glVertex3d(w+I->cell/2., h-I->cell/2., 0.) ;
             glVertex3d(w+I->cell/2., h+I->cell/2., 0.) ;
             glVertex3d(w-I->cell/2., h+I->cell/2., 0.) ;
             glVertex3d(w-I->cell/2., h-I->cell/2., 0.) ;
	          glEnd();

                                                }

#undef   _MAX_1
#undef   _MAX_2
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                           }
/*---------------------------------------------- Индикатор - цилиндр */

    if(I->type==_CYLINDER_IND) {

           double  r ;
           double  h ;
           double  step ;
           double  angle ;

                           r=I->size1/2. ;
                           h=I->size2/2. ;
                     col_cnt=2.*_PI*r/I->cell ;
                        step=2.*_PI/col_cnt ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Настроечный вид */
     if(I->move_flag) {

              glColor4d(GetRValue(this->mScanColor)/256., 
                        GetGValue(this->mScanColor)/256.,
                        GetBValue(this->mScanColor)/256., 0.3) ;

           for(i=0 ; i<col_cnt ; i++) {

                         angle=step*i ;

                glBegin(GL_POLYGON) ;
             glVertex3d(r*sin(angle+step/2.), -h, r*cos(angle+step/2.)) ;
             glVertex3d(r*sin(angle+step/2.),  h, r*cos(angle+step/2.)) ;
             glVertex3d(r*sin(angle-step/2.),  h, r*cos(angle-step/2.)) ;
             glVertex3d(r*sin(angle-step/2.), -h, r*cos(angle-step/2.)) ;
	          glEnd();
                                      }
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - Результирующий вид */
     else             {

#define  _MAX_1   ((int)(_PI*Indicator->size1/Indicator->cell+0.1))
#define  _MAX_2   ((int)(    Indicator->size2/Indicator->cell+0.1))

                i=0 ;

       for(s2_idx=0 ; s2_idx<_MAX_2 ; s2_idx++) 
       for(s1_idx=0 ; s1_idx<_MAX_1 ; s1_idx++) {

              if(i>  I->data_cnt ) {  s2_idx=_MAX_2 ;  break ;  }
              if(i>=_MAX_1*_MAX_2) {  s2_idx=_MAX_2 ;  break ;  }

              if(i==I->data_cnt)  color=mScanColor ;
              else                color=I->data[i] ;

                 i++ ;
              
              if(color==0)  continue ;

           angle=step*s1_idx ;

//             z= r*cos(angle+step/2.) ;                            /* Рассчет координат точки на */
//             x= r*sin(angle+step/2.) ;                            /*   поверхности анализа      */
               h=-I->size2/2.+s2_idx*I->cell+I->cell/2. ;

              glColor4d(GetRValue(color)/256., 
                        GetGValue(color)/256.,
                        GetBValue(color)/256., 0.3) ;

                glBegin(GL_POLYGON) ;
             glVertex3d(r*sin(angle+step/2.), h-I->cell/2., r*cos(angle+step/2.)) ;
             glVertex3d(r*sin(angle+step/2.), h+I->cell/2., r*cos(angle+step/2.)) ;
             glVertex3d(r*sin(angle-step/2.), h+I->cell/2., r*cos(angle-step/2.)) ;
             glVertex3d(r*sin(angle-step/2.), h-I->cell/2., r*cos(angle-step/2.)) ;
	          glEnd();
                                                }

#undef   _MAX_1
#undef   _MAX_2

                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                               }
/*----------------------------- Восстановление контекста отображения */

           glDepthMask(1) ;                                         /* Вкл.запись Z-буфера */
             glDisable(GL_BLEND) ;                                  /* Откл.смешивание цветов */
             glEndList() ;                                          /* Закрытие группы */

                       }                                            /* BLOCK.1 */ 

/*----------------------------- Формирование масштабирующей оболочки */

             glNewList(I->dlist1_idx, GL_COMPILE) ;                 /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;
//      glLoadIdentity() ;

          glTranslated( I->base.x, I->base.y, I->base.z) ;

             glRotated( I->base.azim, 0., 1., 0.) ;
             glRotated(-I->base.elev, 1., 0., 0.) ;
             glRotated(-I->base.roll, 0., 0., 1.) ;

            glCallList(I->dlist2_idx) ;                             /* Отрисовка плоскости анализа */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef    I
}


/*********************************************************************/
/*								     */
/*                        Удаление индикатора                        */

  void  RSS_Module_Area::iIndicatorKill(RSS_Module_Area_Indicator *Indicator)

{
  RSS_Module_Area_Indicator **data_list ;
                        int   i ; 


        if(Indicator->context->mIndListWnd!=NULL)
            SendMessage(Indicator->context->mIndListWnd, WM_USER, 
                                    (WPARAM)_TASK_AREA_KILL_INDICATOR,
                                     (LPARAM)Indicator) ;

        data_list   =(RSS_Module_Area_Indicator **)                  /* Убираем запись об индикаторе */
                          Indicator->context->mIndicators ;  
                  i =     Indicator->idx ;                           /*   из списка индикаторов      */
        data_list[i]= NULL ;           

                      iIndicatorHide(Indicator) ;

                                free(Indicator->data) ;              /* Освобождаем ресурсы индикатора */
                                free(Indicator) ;
}


/*********************************************************************/
/*								     */
/*                    Удаление индикатора из сцены                   */

  void  RSS_Module_Area::iIndicatorHide(RSS_Module_Area_Indicator *Indicator)

{
      if(Indicator->dlist1_idx)
           RSS_Kernel::display.ReleaseList(Indicator->dlist1_idx) ;

      if(Indicator->dlist2_idx)
           RSS_Kernel::display.ReleaseList(Indicator->dlist2_idx) ;

         Indicator->dlist1_idx=0 ;
         Indicator->dlist2_idx=0 ;
}


/*********************************************************************/
/*								     */
/*              Размещение ресурсов индикатора                       */

  int  RSS_Module_Area::iIndicatorAlloc(RSS_Module_Area_Indicator *Indicator)

{
  int  cnt ;

/*---------------------------- Определение числа анализируемых точек */

    if(Indicator->type==    _FLAT_IND) {

               cnt=((int)(Indicator->size1/Indicator->cell+0.1))*
                   ((int)(Indicator->size2/Indicator->cell+0.1)) ;

                                       }
    if(Indicator->type==_CYLINDER_IND) {

               cnt=((int)(_PI*Indicator->size1/Indicator->cell+0.1))*
                   ((int)(    Indicator->size2/Indicator->cell+0.1)) ;

                                       }
/*------------------------------------------------- Выделение памяти */

      if(Indicator->data)  free(Indicator->data) ;

         Indicator->data    =(COLORREF *)calloc(cnt, sizeof(*Indicator->data)) ;
      if(Indicator->data==NULL)  return(-1) ;

        Indicator->data_cnt=0 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*              Считывание файла описания таблицы углов              */

  int  RSS_Module_Area::iReadTableFile(
                           RSS_Module_Area_Indicator *Indicator,
                                                char *path)
{
                    FILE *file ;
                    char  text[4096] ;
                    char *words[10] ;
                    char *end ;
                     int  cR, cG, cB ;
   RSS_Module_Area_Range  azim_range ;
   RSS_Module_Area_Range  elev_range ;
   RSS_Module_Area_Range  roll_range ;
   RSS_Module_Area_Range *range ;
                     int  row ;
                     int  n ;
                     int  i ;

/*--------------------------------------------------- Открытие файла */

       file=fopen(path, "rt") ;
    if(file==NULL) {
                       SEND_ERROR("Ошибка открытия файла") ;
                                       return(-1) ;
                   }
/*------------------------------------------------ Считывание данных */
/* Ожидаемая структура строки:                                       */
/*    <RGB> <ANGLE_1> <ANGLE_2> <ANGLE_3>                            */
/* где                                                               */
/*  <RGB>     - RGB:n.n.n                                            */
/*  <ANGLE_n> - {A|E|R}:{And|Or|List}:{min/max|a1,...,aN}:{ABS|NORM} */

         memset(Indicator->angles, 0, sizeof(Indicator->angles)) ;

                      row=0 ;
                        n=0 ;

   while(1) {                                                       /* CIRCLE.1 - Построчно читаем файл */
               memset(&azim_range, 0, sizeof(azim_range)) ;
               memset(&elev_range, 0, sizeof(elev_range)) ;
               memset(&roll_range, 0, sizeof(roll_range)) ;
                       azim_range.type=-1 ;
                       elev_range.type=-1 ;
                       roll_range.type=-1 ;
/*- - - - - - - - - - - - - - - - - - -  Считывание очередной строки */
                      row++ ;

                      memset(text, 0, sizeof(text)) ;
                   end=fgets(text, sizeof(text)-1, file) ;          /* Считываем строку */
                if(end==NULL)  break ;

            if(text[0]==';')  continue ;                            /* Проходим комментарий */

                end=text+strlen(text)-1 ;                           /* Удаляем символ конца строки */
            if(*end=='\n')  *end=0 ;
/*- - - - - - - - - - - - - - - - Обработка спецификатора шага STEP= */
      if(!memicmp(text, "STEP=", 5)) {

                    Indicator->angle_step=strtod(text+5, &end) ;

                                         continue ;
                                     } 
/*- - - - - - - - - - - - - - - - - - - - - - Разбор строки на слова */
           memset(words, 0, sizeof(words)) ;

                   i = 0 ;
             words[i]=strtok(text, " \t") ;

       while(words[i]!=NULL && i<5) {
                   i++ ;
             words[i]=strtok(NULL, " \t") ;
                                    }

      if(words[3]==NULL ||
         words[4]!=NULL   ) {
                       sprintf(text, "Строка %d - строка должна содержать ровно 4 слова", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - Обработка RGB-секции */
      if(memicmp(words[0], "RGB:", 4)){
                       sprintf(text, "Строка %d - первым должен следовать RGB-спецификатор", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                                      }

           cR=strtoul(words[0]+4, &end, 10) ;
      if(*end!='.'){
                       sprintf(text, "Строка %d - формат RGB-спецификатора - RGB:<red>.<green>.<blue>", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                   }

           cG=strtoul(end+1, &end, 10) ;
      if(*end!='.'){
                       sprintf(text, "Строка %d - формат RGB-спецификатора - RGB:<red>.<green>.<blue>", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                   }

           cB=strtoul(end+1, &end, 10) ;
      if(*end!= 0 ){
                       sprintf(text, "Строка %d - формат RGB-спецификатора - RGB:<red>.<green>.<blue>", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - Обработка ANGLE-секций */
     for(i=1 ; i<4 ; i++) {
 
            if(!memicmp(words[i], "A:", 2)) {  range=&azim_range ;  words[i]+=2 ;  }
       else if(!memicmp(words[i], "E:", 2)) {  range=&elev_range ;  words[i]+=2 ;  }
       else if(!memicmp(words[i], "R:", 2)) {  range=&roll_range ;  words[i]+=2 ;  }
       else                                 {
                       sprintf(text, "Строка %d - ANGLE-спецификатор должен начинаться с символов A:, E: или R:", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                                            }

            if(!memicmp(words[i], "Or:",   3)) {  range->type=    _OR_RANGE ;  words[i]+=3 ;  }
       else if(!memicmp(words[i], "And:",  4)) {  range->type=   _AND_RANGE ;  words[i]+=4 ;  }
       else if(!memicmp(words[i], "List:", 5)) {  range->type=_INLIST_RANGE ;  words[i]+=5 ;  }
       else                                    {
                       sprintf(text, "Строка %d - ANGLE-спецификатор может иметь тип Or, And или List", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                                               }

               end=strchr(words[i], ':') ;
            if(end==NULL)     end="ABS" ;
            else          {  *end= 0 ;  end++ ;  }

            if(!stricmp(end, "ABS" ))  range->base=_ABSOLUTE_VALUE ;
       else if(!stricmp(end, "NORM"))  range->base=  _NORMAL_VALUE ;
       else                                    {
                       sprintf(text, "Строка %d - ANGLE-спецификатор может квалификатор ориентации ABS или NORM", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                                               }

            if(range->type==_INLIST_RANGE) {
                    strncpy(range->a_lst_text, words[i], sizeof(range->a_lst_text)-1) ;
                            range->a_lst[0]=strtod(range->a_lst_text, &end) ;
                            range->a_lst_cnt=1 ;
                                           }
            else                           {
                         end=strchr(words[i], '/') ;
                      if(end==NULL) {
                       sprintf(text, "Строка %d - ANGLE-спецификатор - не найден расделитель диапазона - /", row) ;
                    SEND_ERROR(text) ;
                         return(-1) ;
                                    }

                        *end=0 ;
                         end++ ;

                    strncpy(range->a_min_text, words[i], sizeof(range->a_min_text)-1) ;
                    strncpy(range->a_max_text,  end    , sizeof(range->a_max_text)-1) ;
                            range->a_min=strtod(range->a_min_text, &end) ;
                            range->a_max=strtod(range->a_max_text, &end) ;
                                           }

                          }
/*- - - - - - - - - - - - - - - - - - - -  Создание записи в таблице */
         Indicator->angles[n].use_flag  = 1 ;
         Indicator->angles[n].color     =RGB(cR, cG, cB) ;
         Indicator->angles[n].azim_range=azim_range ;
         Indicator->angles[n].elev_range=elev_range ;
         Indicator->angles[n].roll_range=roll_range ;
                           n++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            }                                                       /* CONTINUE.1 */
/*--------------------------------------------------- Закрытие файла */

                fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*              Определение следующей анализируемой точки            */

  int  RSS_Module_Area::iGetNextPoint(
                           RSS_Module_Area_Indicator *Indicator,
                                                 int  seq, 
                                           RSS_Point *point, 
                                                 int *idx, 
                                            COLORREF *color, 
                                                 int  prev_result)

{
  static  char *angles_status ;   /* Массив состояния углов подхода */
  static   int  s1_idx ;
  static   int  s2_idx ;
           int  next_point ;
      Matrix2d  Summary ;
      Matrix2d  Local ;
      Matrix2d  Normal ;
      Matrix2d  Base0 ;
        double  r ;
        double  step ;
        double  angle ;
           int  col_cnt ;
        double  x ;
        double  y ;
        double  z ;
        double  norm_azim ;
        double  norm_elev ;
           int  i_a ;
           int  i_e ;

#define   _ANGLE_MAX       36
#define  _ANGLES_SIZE   _ANGLE_MAX*_ANGLE_MAX*_ANGLE_MAX

#define        I         Indicator

/*------------------------------------------------- Входной контроль */

   if(Indicator->angle_step<10.) {                                  /* Контроль шага сканирования углов */

               SEND_ERROR("Angles scan steps to small."
                          " Value 10 degree will be used") ;
                         
                                     Indicator->angle_step=10. ;
                                 }
/*------------------------- Выделение буфера состояния углов подхода */

   if(angles_status==NULL) {

       angles_status=(char *)calloc(1, _ANGLES_SIZE) ;
    if(angles_status==NULL) {
                              SEND_ERROR("Memory over.") ;
                                         return(-1) ;
                            }
                           }
/*---------------------------------------------------- Инициализация */

   if(seq==0) {
                  iPrepareAngles(I, point) ;                        /* Подготовка таблицы угловых секторов */
                    iResetAngles(I) ;

                          memset(angles_status, 0, _ANGLES_SIZE) ;

                            *idx=0 ;
                          s1_idx=0 ;
                          s2_idx=0 ;
              }
/*---------------------------------------- Определение углов подхода */

                next_point=0 ;
   
    if(seq!=0)
          next_point=iAnalisePrevious(I, prev_result, color) ;      /* Анализ результатов теста предыдущей точки */

         if(next_point)  iResetAngles(I) ;
    
                        iGetNextAngle(I) ;                          /* Определение углов следующей точки */

/*-------------------------------------- Определение координат точки */

       if(next_point) {  (*idx)++ ;  s1_idx++ ;  }
/*- - - - - - - - - - - - - - - - - - - - - -  Индикатор - плоскость */
    if(I->type==_FLAT_IND) {

#define  _MAX_1   ((int)(Indicator->size1/Indicator->cell+0.1))
#define  _MAX_2   ((int)(Indicator->size2/Indicator->cell+0.1))

       if(s1_idx>=_MAX_1) {  s2_idx++ ;  s1_idx=0 ;  }              /* Если ряд пройден... */
       if(s2_idx>=_MAX_2)   return(-1) ;                            /* Если все ряды пройдены... */ 

               x=-I->size1/2.+s1_idx*I->cell+I->cell/2. ;           /* Рассчет координат точки на */
               y=-I->size2/2.+s2_idx*I->cell+I->cell/2. ;           /*   поверхности анализа      */
               z= 0. ;

             Normal.LoadZero(4, 1) ;
             Normal.SetCell (0, 0, 0 ) ;
             Normal.SetCell (1, 0, 0 ) ;
             Normal.SetCell (2, 0, 1.) ;
             Normal.SetCell (3, 0, 1.) ;


#undef   _MAX_1
#undef   _MAX_2
                           }
/*- - - - - - - - - - - - - - - - - - - - - - -  Индикатор - цилиндр */
    if(I->type==_CYLINDER_IND) {

#define  _MAX_1   ((int)(_PI*Indicator->size1/Indicator->cell+0.1))
#define  _MAX_2   ((int)(    Indicator->size2/Indicator->cell+0.1))

       if(s1_idx>=_MAX_1) {  s2_idx++ ;  s1_idx=0 ;  }              /* Если ряд пройден... */
       if(s2_idx>=_MAX_2)   return(-1) ;                            /* Если все ряды пройдены... */ 

                           r=I->size1/2. ;
                     col_cnt=2.*_PI*r/I->cell ;
                        step=2.*_PI/col_cnt ;
                       angle=step*s1_idx ;

               z= r*cos(angle+step/2.) ;                            /* Рассчет координат точки на */
               x= r*sin(angle+step/2.) ;                            /*   поверхности анализа      */
               y=-I->size2/2.+s2_idx*I->cell+I->cell/2. ;

             Normal.LoadZero(4, 1) ;
             Normal.SetCell (0, 0, x ) ;
             Normal.SetCell (1, 0, 0 ) ;
             Normal.SetCell (2, 0, z ) ;
             Normal.SetCell (3, 0, 1.) ;

#undef   _MAX_1
#undef   _MAX_2
                               }
/*- - - - - - - - - - - - - - - - -  Пересчет в рабочее пространство */
           Summary.LoadE(4, 4) ;                                    /* Строим матрицу пересчета в  */
             Local.Load4d_base(I->base.x, I->base.y, I->base.z) ;   /*   координаты рабочего пространства */
           Summary.LoadMul    (&Summary, &Local) ;
             Local.Load4d_azim(-I->base.azim) ;
           Summary.LoadMul    (&Summary, &Local) ;
             Local.Load4d_elev(-I->base.elev) ;
           Summary.LoadMul    (&Summary, &Local) ;
             Local.Load4d_roll(-I->base.roll) ;
           Summary.LoadMul    (&Summary, &Local) ;

             Local.LoadZero(4, 1) ;
             Local.SetCell (0, 0, x ) ;
             Local.SetCell (1, 0, y ) ;
             Local.SetCell (2, 0, z ) ;
             Local.SetCell (3, 0, 1.) ;

             Local.LoadMul (&Summary, &Local) ;                     /* Пересчит.в рабочее пространство */

                   point->x=Local.GetCell(0, 0) ;
                   point->y=Local.GetCell(1, 0) ;
                   point->z=Local.GetCell(2, 0) ;
/*- - - - - - - - - - - - - - - Определение углов ориентации нормали */
                    Base0.LoadZero(4, 1) ;
                    Base0.SetCell (3, 0, 1.) ;

                    Base0.LoadMul(&Summary, &Base0) ;
                   Normal.LoadMul(&Summary, &Normal) ;
                 x=Normal.GetCell(0, 0)-Base0.GetCell(0, 0) ;
                 y=Normal.GetCell(1, 0)-Base0.GetCell(1, 0) ; ;
                 z=Normal.GetCell(2, 0)-Base0.GetCell(2, 0) ; ;

         norm_azim=atan2(x,      z       )*_RAD_TO_GRD ;
         norm_elev=atan2(y, sqrt(x*x+z*z))*_RAD_TO_GRD ;

/*------------------------------------ Пересчет ориентации к нормали */

#define  A   I->angles[I->angles_idx]

      if(A.priority_type==_A_E_R_PRIO) {  i_a=0 ;  i_e=1 ;  }
      if(A.priority_type==_A_R_E_PRIO) {  i_a=0 ;  i_e=2 ;  }
      if(A.priority_type==_E_A_R_PRIO) {  i_a=1 ;  i_e=0 ;  }
      if(A.priority_type==_E_R_A_PRIO) {  i_a=2 ;  i_e=0 ;  }
      if(A.priority_type==_R_A_E_PRIO) {  i_a=1 ;  i_e=2 ;  }
      if(A.priority_type==_R_E_A_PRIO) {  i_a=2 ;  i_e=1 ;  }

      if(A.scan[i_a].range->base==_NORMAL_VALUE)  *A.scan[i_a].angle+=norm_azim ;
      if(A.scan[i_e].range->base==_NORMAL_VALUE)  *A.scan[i_e].angle-=norm_elev ;

#undef  A

/*-------------------------------------------------------------------*/

#undef   I

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                  Подготовка таблицы угловых секторов              */

  void  RSS_Module_Area::iPrepareAngles(
                            RSS_Module_Area_Indicator *Indicator, 
                                            RSS_Point *Point     )

{
   int  i ;
   int  j ;

#define   I   Indicator
#define   A   Indicator->angles[i] 
#define   S   Indicator->angles[i].scan[j]

/*------------------------------------ Перебор 3D-секторов в таблице */

         I->angles_idx= 0 ;

   for(i=0 ; i<_ANGLES_LIST_MAX ; i++) {                            /* CIRCLE.1 */

          if(A.use_flag==0)  continue ;

            memset(A.scan, 0, sizeof(A.scan)) ;

/*------------ Определение порядка сканирования сегментов в секторах */

     for(j=0 ; j<3 ; j++) {

       if((j==0 && (A.priority_type==_A_E_R_PRIO ||
                    A.priority_type==_A_R_E_PRIO   )) ||
          (j==1 && (A.priority_type==_E_A_R_PRIO ||
                    A.priority_type==_R_A_E_PRIO   )) ||
          (j==2 && (A.priority_type==_E_R_A_PRIO ||
                    A.priority_type==_R_E_A_PRIO   ))   ) {
                            A.scan[j].range=&A.azim_range ;
                            A.scan[j].angle=&Point->azim ;
                                                          }
       if((j==0 && (A.priority_type==_E_A_R_PRIO ||
                    A.priority_type==_E_R_A_PRIO   )) ||
          (j==1 && (A.priority_type==_A_E_R_PRIO ||
                    A.priority_type==_R_E_A_PRIO   )) ||
          (j==2 && (A.priority_type==_A_R_E_PRIO ||
                    A.priority_type==_R_A_E_PRIO   ))   ) {
                            A.scan[j].range=&A.elev_range ;
                            A.scan[j].angle=&Point->elev ;
                                                          }
       if((j==0 && (A.priority_type==_R_E_A_PRIO ||
                    A.priority_type==_R_A_E_PRIO   )) ||
          (j==1 && (A.priority_type==_A_R_E_PRIO ||
                    A.priority_type==_E_R_A_PRIO   )) ||
          (j==2 && (A.priority_type==_A_E_R_PRIO ||
                    A.priority_type==_E_A_R_PRIO   ))   ) {
                            A.scan[j].range=&A.roll_range ;
                            A.scan[j].angle=&Point->roll ;
                                                          }
                          }
/*------------------------------ Определение параметров сканирования */

     for(j=0 ; j<3 ; j++) {

         if(S.range->type==_INLIST_RANGE) {
                S.type   = _AND_RANGE ;
                S.idx_max=S.range->a_lst_cnt ;
                                          }
         if(S.range->type==    _OR_RANGE) {
                S.type   = _OR_RANGE ;
                S.idx_max=(S.range->a_max-S.range->a_min
                                         +I->angle_step/1.001)
                                                /I->angle_step+1 ;
                                          }
         if(S.range->type==   _AND_RANGE) {
                S.type   = _AND_RANGE ;
                S.idx_max=(S.range->a_max-S.range->a_min
                                         +I->angle_step/1.001)
                                                /I->angle_step+1 ;
                                          }
                          }
/*------------------------------------ Перебор 3D-секторов в таблице */
                                       }                            /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   I
#undef   A
#undef   S
}


/*********************************************************************/
/*								     */
/*  Установка анализа таблицы угловых секторов в исходное положение  */

  void  RSS_Module_Area::iResetAngles(
                            RSS_Module_Area_Indicator *Indicator)

{
  int  i ;

#define   I   Indicator


                         I->angles_idx=0 ;

    for(i=0 ; i<_ANGLES_LIST_MAX ; i++) {
                         I->angles[i].scan[0].idx=0 ;
                         I->angles[i].scan[1].idx=0 ;
                         I->angles[i].scan[2].idx=0 ;
                                        }

#undef    I   

}


/*********************************************************************/
/*								     */
/*              Анализ результатов теста предыдущей точки            */
/*		  и просчет "индекса" следующей точки                */

  int  RSS_Module_Area::iAnalisePrevious(
                           RSS_Module_Area_Indicator *Indicator,
                                                 int  prev_result,
                                            COLORREF *color       )
{
  int  next_point ;
  int  layer ;

#define   I   Indicator
#define   S   Indicator->angles[I->angles_idx]

/*------------------------------------------------------ Подготовка */

                         next_point=0 ;

/*------------------------------------ Отрицательный результат теста */

   if(prev_result) {                                                /* IF.1 */

     for(layer=2 ; layer>=0 ; layer--) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - OR-тип */
       if(S.scan[layer].type==_OR_RANGE) {  

              S.scan[layer].idx++ ;
           if(S.scan[layer].idx<S.scan[layer].idx_max)  break ;

              S.scan[layer].idx=0 ;
                     continue ;      
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  AND-тип */
       else                              {  

              S.scan[layer].idx=0 ;
                     continue ;      
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }

            if(S.scan[0].idx==0 &&                                  /* Если угловой сектор 'плохой'... */
               S.scan[1].idx==0 &&
               S.scan[2].idx==0   ) {
                                                I->angles_idx++ ;
       while(I->angles_idx<_ANGLES_LIST_MAX &&
                S.use_flag==  0               )  I->angles_idx++ ;

            if(I->angles_idx>=_ANGLES_LIST_MAX) {
                              *color=this->mBadColor ;
                          next_point= 1 ;
                       I->angles_idx= 0 ;
                                                }
                                    }
                   }                                                /* ELSE.1 */
/*------------------------------------ Положительный результат теста */

   else            {                                                /* ELSE.1 */
                     
     for(layer=2 ; layer>=0 ; layer--) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - OR-тип */
       if(S.scan[layer].type==_OR_RANGE) {  

              S.scan[layer].idx=0 ;
                     continue ;      
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  AND-тип */
       else                              {  

              S.scan[layer].idx++ ;
           if(S.scan[layer].idx<S.scan[layer].idx_max)  break ;

              S.scan[layer].idx=0 ;
                     continue ;      
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }

            if(S.scan[0].idx==0 &&                                  /* Если угловой сектор       */
               S.scan[1].idx==0 &&                                  /*   успешно отсканирован... */ 
               S.scan[2].idx==0   ) {
                                             *color=S.color ;
                                         next_point= 1 ;
                                      I->angles_idx= 0 ;
                                    }
                   }                                                /* END.1 */
/*-------------------------------------------------------------------*/

#undef   I
#undef   S

  return(next_point) ;
}


/*********************************************************************/
/*								     */
/*                 Задание следующего углового положения             */

  void  RSS_Module_Area::iGetNextAngle(
                           RSS_Module_Area_Indicator *Indicator)

{
   int  j ;

#define   I   Indicator
#define   S   Indicator->angles[Indicator->angles_idx].scan[j]


     for(j=0 ; j<3 ; j++)
       if(S.range->type==_INLIST_RANGE) {
               *S.angle=S.range->a_lst[S.idx] ;
                                        }
        else                            {
             *S.angle=S.range->a_min+I->angle_step*S.idx ;
          if(*S.angle>S.range->a_max) *S.angle=S.range->a_max ;
                                        }

#undef   I
#undef   S
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**	  ОПИСАНИЕ КЛАССА ОБЪЕКТА "ПОВЕРХНОСТЬ АНАЛИЗА"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_Area::RSS_Object_Area(void)

{
   strcpy(Type, "Area") ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

   Indicator     =NULL ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Area::~RSS_Object_Area(void)

{
}


/********************************************************************/
/*								    */
/*		       Освобождение ресурсов                        */

  void   RSS_Object_Area::vFree(void)

{
     this->Indicator->context->iIndicatorKill(this->Indicator) ;
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_Area::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;

#define  I  this->Indicator

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT AREA\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n",       this->Name  ) ;  *text+=field ;
    sprintf(field, "TYPE=%d\n",       I->type     ) ;  *text+=field ;
    sprintf(field, "SIZE_1=%.10lf\n", I->size1    ) ;  *text+=field ;
    sprintf(field, "SIZE_2=%.10lf\n", I->size2    ) ;  *text+=field ;
    sprintf(field, "CELL=%.10lf\n",   I->cell     ) ;  *text+=field ;
    sprintf(field, "MOVE=%d\n",       I->move_flag) ;  *text+=field ;
    sprintf(field, "X_BASE=%.10lf\n", I->base.x   ) ;  *text+=field ;
    sprintf(field, "Y_BASE=%.10lf\n", I->base.y   ) ;  *text+=field ;
    sprintf(field, "Z_BASE=%.10lf\n", I->base.z   ) ;  *text+=field ;
    sprintf(field, "A_AZIM=%.10lf\n", I->base.azim) ;  *text+=field ;
    sprintf(field, "A_ELEV=%.10lf\n", I->base.elev) ;  *text+=field ;
    sprintf(field, "A_ROLL=%.10lf\n", I->base.roll) ;  *text+=field ;
    sprintf(field, "DATA_CNT=%d\n",   I->data_cnt ) ;  *text+=field ;

                   field[0]=0 ;

   for(i=0 ; i<I->data_cnt ; i++) {
     if(!(i%10)) {
                   if(i) {  *text+=field ;
                            *text+="\n" ;  }

                    sprintf(field, "DATA=%x", I->data[i]) ;
                 }
     else        {
                    sprintf(field+strlen(field), ",%x", I->data[i]) ;
                 }
                                  }

                            *text+=field ;
                            *text+="\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  ВСПОМОГАТЕЛЬНЫЕ ПРОЦЕДУРЫ                        **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*           Получение ссылки на программный модуль                 */

   RSS_Kernel *GetTaskAreaEntry(void)

{
	return(&ProgramModule) ;
}

