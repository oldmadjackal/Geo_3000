/********************************************************************/
/*								    */
/*             МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "НАБОР ТОЧЕК"             */
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
#include <sys\stat.h>

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "O_PSet.h"

#pragma warning(disable : 4996)

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

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

     static   RSS_Module_PSet  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_PSET_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_PSET_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**    ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ "НАБОР ТОЧЕК"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_PSet_instr  RSS_Module_PSet_InstrList[]={

 { "help",   "?",  "#HELP   - список доступных команд", 
                    NULL,
                   &RSS_Module_PSet::cHelp   },
 { "create", "cr", "#CREATE - создать набор точек",
                   " CREATE <Имя>\n"
                   "   Создает именованный набор точек",
                   &RSS_Module_PSet::cCreate },
 { "add",    "a",  "#ADD - добавляет точку в набор точек",
                   " ADD <Имя набора>[:idx] X Y Z A E R\n"
                   "   Включить точку с абсолютными координатами\n"
                   " ADD <Имя набора>[:idx] <Имя мастер-обьекта>\n"
                   "   Включить точку, задаваемую конфигурацией мастер-обьекта\n"
                   " ADD <Имя набора>[:idx] @<Имя обьекта>\n"
                   "   Включить целевую точку обьекта\n"
                   "   Если задается idx - вставка производится в заданную позицию,\n"
                   "  а в противном случае -  в конец списка точек\n",
                   &RSS_Module_PSet::cAdd },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_PSet_instr *RSS_Module_PSet::mInstrList=NULL ;

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_PSet::RSS_Module_PSet(void)

{
	   keyword="Geo" ;
    identification="Points_set_object" ;

        mInstrList=RSS_Module_PSet_InstrList ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_PSet::~RSS_Module_PSet(void)

{
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_PSet::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_PSet::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "PSET"
#define  _SECTION_SHRT_NAME   "PSET"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Points Set:") ;
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
       if(status)  SEND_ERROR("Секция POINTS SET: Неизвестная команда") ;
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
/*		        Считать данные из строки		    */

    void  RSS_Module_PSet::vReadSave(std::string *data)

{

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE PSET\n", 
                      strlen("#BEGIN MODULE PSET\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT PSET\n", 
                      strlen("#BEGIN OBJECT PSET\n"))   )  return ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_PSet::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE PSET\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_PSet::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_PSet_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя>                                                */

  int  RSS_Module_PSet::cCreate(char *cmd)

{
           char *name ;
           char *end ;
            int  status ;

/*-------------------------------------- Дешифровка командной строки */

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* Извлекаем имя объекта */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

      } while(0) ;                                                  /* BLOCK.1 */

/*------------------------------------------------- Создание маркера */

      status=CreateObject(name) ;
   if(status!=0)  return(status) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ADD                     */
/*								    */
/*         ADD <Имя набора>[:idx] X Y Z A E R                       */
/*         ADD <Имя набора>[:idx] <Имя мастер-обьекта>              */
/*         ADD <Имя набора>[:idx] @<Имя обьекта>                    */

  int  RSS_Module_PSet::cAdd(char *cmd)

{
#define  _COORD_MAX   6
#define   _PARS_MAX   7

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
       RSS_Object_PSet  *set ;
  RSS_Object_PSet_elem   set_elem ;
                   int   idx ;
                   int   master_flag ;
                  char   object_name[128] ;
            RSS_Object  *object ;
             RSS_Joint   degrees[50] ;       /* Вектор степеней подвижности объекта */
                   int   cnt ;
                  char  *sep ;
                  char  *error ;
                  char  *end ;
                  char   text[1024] ;
                   int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */

                 memset(pars,  0, sizeof(pars)) ;
                 memset(coord, 0, sizeof(coord)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   

/*-------------------------------------------- Контроль имени набора */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя набора. \n"
                                 "Например: ADD <Имя набора> ...") ;
                                     return(-1) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Выделение индекса */
       sep=strchr(name, ':') ;
    if(sep!=NULL) {
                      idx=strtoul(sep+1, &end, 10) ;
                     if(*end!=0) {
                          SEND_ERROR("Индекс точки должен быть числом.\n"
                                     "Например: ADD SET:2 ...") ;
                                           return(-1) ;
                                 }                        
                  }
    else          {
                      idx=-1 ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Поиск набора по имени */
       set=FindObject(name) ;                                       /* Ищем объект по имени */
    if(set==NULL)  return(-1) ;

/*------------------------------------------- Контроль задания точки */

   if(xyz[0]==NULL) {
               SEND_ERROR("Не определена добавляемая точка.\n") ;
                                     return(-1) ;
                    }
/*------------------------------------------------- Анализ на обьект */

   if(!isdigit(*xyz[0])) {
/*- - - - - - - - - - - - - - - - Анализ спецификатора целевой точки */
      if(*xyz[0]=='@') {
                          master_flag=0 ;
                               xyz[0]++ ;
                   strcpy(object_name, xyz[0]) ;
                
               sep=strchr(object_name, '.') ;                       /* Отсекаем спецификацию частей объекта */
            if(sep!=NULL)  *sep=0 ;

                       }
      else             {
                          master_flag=1 ;
                           strcpy(object_name, xyz[0]) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - Поиск объекта по имени */
     for(i=0 ; i<OBJECTS_CNT ; i++)
       if(!stricmp(OBJECTS[i]->Name, object_name)) { 
                                            object=OBJECTS[i] ;
                                                       break ;     
                                                   }

       if(i==OBJECTS_CNT) {                                         /* Если объект не найден... */
             SEND_ERROR("Объекта с таким именем НЕ существует") ;
                                return(-1) ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         }
/*------------------------------------------------- Разбор координат */

   if(object==NULL) {

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
         if(coord_cnt<6)  error="Должно быть указаны 6 координат" ;

         if(error!=NULL) {  SEND_ERROR(error) ;
                                  return(-1) ;   }
                    }
/*------------------------------------------------- Добавление точки */

                     memset(&set_elem, 0, sizeof(set_elem)) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
        if(object==NULL  ) {

               set_elem.type       =_RSS_ABSOLUTE_TARGET ;
               set_elem.target.x   =coord[0] ;
               set_elem.target.y   =coord[1] ;
               set_elem.target.z   =coord[2] ;
               set_elem.target.azim=coord[3] ;
               set_elem.target.elev=coord[4] ;
               set_elem.target.roll=coord[5] ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - -  Целевая точка объекта */
   else if(master_flag==0) {

               set_elem.type  =_RSS_OBJECT_TARGET ;
               set_elem.object=(char *)calloc(1, strlen(xyz[0])+1) ;
        strcpy(set_elem.object, xyz[0]) ;
                           }
/*- - - - - - - - - - - - - - - - - - -  Конфигурация мастер-объекта */
   else                    {

         if(set->mMasterObject!= NULL &&                            /* Контроль смены мастер-объекта */
            set->mMasterObject!=object  ) {
                 sprintf(text, "Для данного набора был уже "
                               "установлен другой мастер-объект - %s",
                                               set->mMasterObject->Name) ;
              SEND_ERROR(text) ;
                   return(-1) ;
                                          }

                      cnt=object->vGetJoints(degrees) ;             /* Извлекаем начальный вектор степеней подвижности */

               set_elem.type  =_RSS_JOINTS_TARGET ;
               set_elem.joints=(RSS_Joint *)calloc(cnt, sizeof(*degrees)) ;
        memcpy(set_elem.joints, degrees, cnt*sizeof(*degrees)) ;

               set->mMasterObject=object ;
               set->mDegreesCnt  =  cnt ;
                           }
/*------------------------------------------------ Обработка индекса */

          if(idx==      -1       )  idx=set->mPointsCnt ;
          if(idx >set->mPointsCnt)  idx=set->mPointsCnt ;

/*------------------------------------------ Пополнение списка точек */

        set->mPointsCnt++ ;
        set->mPointsList=(RSS_Object_PSet_elem *)
                           realloc(set->mPointsList, 
                                    set->mPointsCnt*sizeof(*set->mPointsList)) ;

     if(set->mPointsList==NULL) {
                                     set->mPointsCnt=0 ;
                      SEND_ERROR("Недостаточно памяти") ;
                                      return(-1) ;
                                }

     if(idx!=set->mPointsCnt-1)
          memmove(&set->mPointsList[idx+1], 
                  &set->mPointsList[idx], 
                   (set->mPointsCnt-idx-1)*sizeof(*set->mPointsList)) ;

           memcpy(&set->mPointsList[idx], &set_elem, sizeof(set_elem)) ;
                       
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   Поиск обьекта типа PSET по имени                 */

  RSS_Object_PSet *RSS_Module_PSet::FindObject(char *name)

{
     char   text[1024] ;
      int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- Поиск объекта по имени */ 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Ищем объект по имени */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* Если имя не найдено... */
                           sprintf(text, "Объекта с именем '%s' "
                                         "НЕ существует", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------------- Контроль типа объекта */ 

     if(stricmp(OBJECTS[i]->Type, "PSet")) {
           SEND_ERROR("Объект не является объектом типа POINTS SET") ;
                            return(NULL) ;
                                           }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_PSet *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  int  RSS_Module_PSet::CreateObject(char *name)

{
    RSS_Object_PSet *object ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
 
/*--------------------------------------------------- Проверка имени */

    if(name[0]==0) {                                                /* Если имя НЕ задано */
              SEND_ERROR("Секция POINTS SET: Не задано имя объекта") ;
                                return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
              SEND_ERROR("Секция POINTS SET: Объект с таким именем уже существует") ;
                                return(-1) ;
                                              }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_PSet ;
    if(object==NULL) {
              SEND_ERROR("Секция POINTS SET: Недостаточно памяти для создания объекта") ;
                        return(-1) ;
                     }
/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция POINTS SET: Переполнение памяти") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
       strcpy(OBJECTS[OBJECTS_CNT]->Name, name) ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  ОПИСАНИЕ КЛАССА ОБЪЕКТА "НАБОР ТОЧЕК"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Object_PSet::RSS_Object_PSet(void)

{
   strcpy(Type, "PSet") ;

        mMasterObject=NULL ;
        mDegreesCnt  =  0 ;
        mPointsList  =NULL ;
        mPointsCnt   =  0 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_PSet::~RSS_Object_PSet(void)

{
  int  i ;


   if(mPointsList!=NULL) {

     for(i=0 ; i<mPointsCnt ; i++)
            if(mPointsList[i].type==_RSS_JOINTS_TARGET)  
                                free(mPointsList[i].joints) ;
       else if(mPointsList[i].type==_RSS_OBJECT_TARGET)  
                                free(mPointsList[i].object) ;

                                free(mPointsList) ;
                         }
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_PSet::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT PSET\n" ;

/*----------------------------------------------------------- Данные */

    sprintf(field, "NAME=%s\n",       this->Name  ) ;  *text+=field ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        Специальные действия                      */

     int  RSS_Object_PSet::vSpecial(char *oper, void *data)
{
  int  idx ;

/*--------------------------------------------------- Мастер-объект */

   if(!stricmp(oper, "POINTS_SET:MASTER")) {

                *((RSS_Object **)data)=mMasterObject ;

                            return(0) ;
                                           }
/*----------------------------------------------------- Запрос точки */

   if(!memicmp(oper, "POINTS_SET:GET", 
              strlen("POINTS_SET:GET"))) {

         idx=atoi(oper+strlen("POINTS_SET:GET")) ;
      if(idx<0 || idx>=mPointsCnt)  return(-1) ;

      if(mPointsList[idx].type==_RSS_ABSOLUTE_TARGET)
            memcpy(data, &mPointsList[idx].target, 
                   sizeof(mPointsList[idx].target)) ;

      if(mPointsList[idx].type==  _RSS_JOINTS_TARGET)
            memcpy(data,  mPointsList[idx].joints, 
                   sizeof(mPointsList[idx].joints[0])*mDegreesCnt) ;

      if(mPointsList[idx].type==  _RSS_OBJECT_TARGET)
            memcpy(data,  mPointsList[idx].object, 
                   strlen(mPointsList[idx].object)+1) ;

                            return(mPointsList[idx].type) ;
                                         }
/*-------------------------------------------------------------------*/


  return(-1) ;
}


