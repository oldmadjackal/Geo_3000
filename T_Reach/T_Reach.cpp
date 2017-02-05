/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ВЫХОД В ТОЧКУ"    		    */
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

#include "d:\_Projects_2008\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Reach.h"

#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  SEND_CHECK(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_CHECK_MESSAGE,      \
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

     static   RSS_Module_Reach  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_REACH_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_REACH_API RSS_Kernel *GetEntry(void)

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

  struct RSS_Module_Reach_instr  RSS_Module_Reach_InstrList[]={

 { "help",       "?",  "#HELP (?) - список доступных команд", 
                        NULL,
                       &RSS_Module_Reach::cHelp   },
 { "config",     "c",  "#CONFIG (C) - настройка поисковых алгритмов", 
                        NULL,
                       &RSS_Module_Reach::cConfig   },
 { "target",     "t",  "#TARGET (T) - задает целевую точку объекта для решения\n"
                       "   обратной кинематической задачи и проверки условий допустимости", 
                       " TARGET  <Имя> <X> <Y> <Z> <Azim> <Elev> <Roll>\n"
                       "   Проанализировать точку заданную координатами\n"
                       " TARGET  <Имя> <Цель> [<Azim> <Elev> <Roll>]\n"
                       "   Проанализировать точку заданную целевым обьектом\n"
                       " TARGET/x <Имя> <x>\n"
                       "   Задает координату X анализируемой точки\n"
                       "       (аналогично для Y, Z, A, E, R)\n"
                       " TARGET/+x <Имя> <x>\n"
                       "   Задает приращение координаты X анализируемой точки\n"
                       "       (аналогично для Y, Z, A, E, R)\n"
                       " TARGET. <Имя>\n"
                       "   Проанализировать предыдущую точку без изменений\n"
                       " TARGET> <Имя>\n"
                       "   Задает клавиатурное управление анализируемой точкой\n",
                       &RSS_Module_Reach::cTarget },
 { "targetinfo", "ti", "#TARGETINFO (TI) - отобразить параметры целевой  точки", 
                        NULL,
                       &RSS_Module_Reach::cTargetInfo },
 { "rotate",     "r",  "#ROTATE (R) - Беотрывное вращение вокруг текущей целевой точки", 
                       " ROTATE  <Имя>\n",
                       &RSS_Module_Reach::cRotate   },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Reach_instr *RSS_Module_Reach::mInstrList=NULL ;
                        RSS_Point  RSS_Module_Reach::mTarget ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Reach::RSS_Module_Reach(void)

{
                keyword="Geo" ;
         identification="Reach_task" ;

             mInstrList=RSS_Module_Reach_InstrList ;

               mLineAcc=  0.01 ;
              mAngleAcc=  1.0 ;
            mResetStart=  0 ;
           mCheckRanges=  1 ;
                  mDraw=  1 ;
          mScanStepCoef=  5. ;
         mJampRangeCoef=  5. ;
               mFailMax=100 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Reach::~RSS_Module_Reach(void)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Reach::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "REACH"
#define  _SECTION_SHRT_NAME   "R"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Reach:") ;
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
       if(status)  SEND_ERROR("Секция REACH: Неизвестная команда") ;
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
/*		        Записать данные в строку		    */

    void  RSS_Module_Reach::vWriteSave(std::string *text)

{
  char  value[1024] ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE REACH\n" ;

    sprintf(value, "LINE_ACC=%lf\n", mLineAcc) ;
     *text+=value ;
    sprintf(value, "ANGLE_ACC=%lf\n", mAngleAcc) ;
     *text+=value ;
    sprintf(value, "RESET_START=%d\n", mResetStart) ;
     *text+=value ;
    sprintf(value, "CHECK_RANGES=%d\n", mCheckRanges) ;
     *text+=value ;
    sprintf(value, "DRAW=%d\n", mDraw) ;
     *text+=value ;
    sprintf(value, "SCAN_STEP_COEF=%d\n", mScanStepCoef) ;
     *text+=value ;
    sprintf(value, "JAMP_RANGE_COEF=%d\n", mJampRangeCoef) ;
     *text+=value ;
    sprintf(value, "FAIL_MAX=%d\n", mFailMax) ;
     *text+=value ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Reach::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Reach_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CONFIG                  */

  int  RSS_Module_Reach::cConfig(char *cmd)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			   (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
			     GetActiveWindow(), Task_Reach_Config_dialog, 
                              (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TARGET                  */
/*      TARGET    <Имя> <X> <Y> <Z> <Azim> <Elev> <Roll>            */
/*      TARGET    <Имя> <Цель> [<Azim> <Elev> <Roll>]               */
/*      TARGET/X  <Имя> <X>                                         */
/*      TARGET/+X <Имя> <X>                                         */
/*      TARGET.   <Имя>                                             */
/*      TARGET>   <Имя> <Код стрелочки> <Шаг>                       */
/*      TARGET>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Reach::cTarget(char *cmd)

{ 

#define  _COORD_MAX   6
#define   _PARS_MAX   7

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
             char   obj_name[256] ;
       RSS_Object  *object ;
              int   object_flag ;       /* Флаг работы с привязкой к объекту */
              int   prev_flag ;         /* Флаг работы с предудущей точкой */
              int   xyz_flag ;          /* Флаг режима одной координаты */
              int   delta_flag ;        /* Флаг режима приращений */
              int   arrow_flag ;        /* Флаг стрелочного режима */
             char  *arrows ;
             char  *error ;
             char  *end ;
        RSS_Point   Top ;               /* Текущее положение целевой точки объекта */
           double   range ;             /* 'Целевая' дистанция */
           double   range_prv ;
           double   v_size ;            /* Величина шага по вектору */
        RSS_Joint   degrees[50] ;       /* Вектор степеней подвижности объекта */
        RSS_Joint   degrees_prv[50] ;
              int   cnt ;               /* Число степеней подвижности */
              int   move_cnt ;          /* Число изменяемых степеней подвижности */
           double   vector[50] ;        /* Вектор приращения степеней подвижности */
           double   l_step ;            /* Шаг движения по линейным координат */
           double   a_step ;            /* Шаг движения по угловым координат */
              int   stop_flag ;         /* Флаг завершения поиска */
           double   dl_stop ;           /* Точность приближения линейных координат */
           double   da_stop ;           /* Точность приближения угловых координат */
           double   len_factor ;        /* Фактор линейных размеров */
           double   len ;
           double   coef ;
              int   status ;
              int   fail_cnt ;
              int   fail_all ;
              int   n ;
              int   i ;
              int   j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */

                     object_flag=0 ;
                       prev_flag=0 ;
                      delta_flag=0 ;
                      arrow_flag=0 ;
                        xyz_flag=0 ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
       if(*cmd=='.'   ) {
                             prev_flag=1 ;
                                cmd+=2 ;
                        }
  else if(*cmd=='/' ||
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
           else if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )    xyz_flag='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )    xyz_flag='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )    xyz_flag='R' ;

                           cmd=end+1 ;
                        }
  else if(*cmd=='>'   ) {
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
/*- - - - - - - - - - - - - - - Проверка на задание объекта привязки */
     if(pars[1]!=NULL) {
                          strtod(pars[1], &end) ;

                  if(*end=='*')  return(0) ;

                  if(*end!= 0 ) {  object_flag=  1 ;
                                           xyz=&pars[2] ;   }
                       }
/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {                        
                         arrows=pars[0] ;

     if(strstr(arrows, "shift")==NULL) {
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
                                       }
     else                              {
      if(strstr(arrows, "left" )!=NULL   ) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL   ) {  xyz_flag='A' ;  inverse= 1. ;  }  
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )==NULL   ) {  xyz_flag='E' ;  inverse=-1. ;  }
      if(strstr(arrows, "up"   )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL &&
         strstr(arrows, "ctrl" )!=NULL   ) {  xyz_flag='R' ;  inverse=-1. ;  }
                                       }

                          xyz=&pars[1] ;
                         name= pars[2] ;
                   }
/*------------------------------------------------- Разбор координат */

  if(!prev_flag) {
                      memset(coord, 0, sizeof(coord)) ;

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("Некорректное значение координаты") ;
                                       return(-1) ;
                    }

             if(xyz_flag) { i++ ;  break ;  }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
     if(!object_flag)
      if(xyz_flag) {
               if(coord_cnt==0)  error="Не указана координата или ее приращение" ;
                   }
      else         {
               if(coord_cnt <6)  error="Должно быть указано 6 координат" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

                 }
/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - Привязка к объекту */
        if(object_flag) {

                    memset(obj_name, 0, sizeof(obj_name)) ;         /* Извлекаем описание привязки */
                   strncpy(obj_name, pars[1], sizeof(obj_name)-1) ;

                end=strchr(obj_name, '.') ;                         /* Выделяем имя объекта привязки */
             if(end!=NULL) {  *end=0 ;  end++ ;  }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* Определяем объект привязки */
         if(!stricmp(OBJECTS[i]->Name, obj_name)) {  object=OBJECTS[i] ;
                                                            break ;      }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Объекта привязки с таким именем НЕ существует") ;
                                return(-1) ;
                            }

                 status=object->vGetTarget(end, &mTarget) ;
              if(status)  return(-1) ;

              if(coord_cnt>=1)  mTarget.azim=coord[0] ;
              if(coord_cnt>=2)  mTarget.elev=coord[1] ;
              if(coord_cnt>=3)  mTarget.roll=coord[2] ;
                        }
/*- - - - - - - - - - - - - - - - - - - - Работа с предыдущей точкой */
   else if(  prev_flag) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   else if( delta_flag) {

          if(xyz_flag=='X')  mTarget.x   +=inverse*coord[0] ;
     else if(xyz_flag=='Y')  mTarget.y   +=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')  mTarget.z   +=inverse*coord[0] ;
     else if(xyz_flag=='A')  mTarget.azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')  mTarget.elev+=inverse*coord[0] ;
     else if(xyz_flag=='R')  mTarget.roll+=inverse*coord[0] ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else                {

          if(xyz_flag=='X')   mTarget.x   =coord[0] ;
     else if(xyz_flag=='Y')   mTarget.y   =coord[0] ;                 
     else if(xyz_flag=='Z')   mTarget.z   =coord[0] ;
     else if(xyz_flag=='A')   mTarget.azim=coord[0] ;
     else if(xyz_flag=='E')   mTarget.elev=coord[0] ;
     else if(xyz_flag=='R')   mTarget.roll=coord[0] ;
     else                   {
                              mTarget.x   =coord[0] ;
                              mTarget.y   =coord[1] ;
                              mTarget.z   =coord[2] ;
                              mTarget.azim=coord[3] ;
                              mTarget.elev=coord[4] ;
                              mTarget.roll=coord[5] ;
                            }
                       }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: TARGET <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {  object=OBJECTS[i] ;
                                                            break ;  }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Анализируемого объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*------------------------------ Определение параметров сканирования */

           cnt=object->vGetJoints(degrees) ;                        /* Извлекаем начальный вектор степеней подвижности */

       for(len_factor=0., move_cnt=0, i=0 ; i<cnt ; i++) {          /* Опр.число изменяемых степеней подвижности */
         if(degrees[i].fixed==0)  move_cnt++ ;                      /*  и предельный фактор линейных размеров    */

         if(degrees[i].type==_L_TYPE) {
          if(degrees[i].fixed)  len=degrees[i].value ;
          else                  len=degrees[i].value_max ;

          if(fabs(len)>len_factor)  len_factor=fabs(len) ;
                                      }
                                                         }

                  a_step=5. ;
                  l_step=len_factor*sin(a_step*3.14/180.) ;

          if(this->mLineAcc !=0)  dl_stop=this->mLineAcc ;
          else                    dl_stop=l_step/50. ;

          if(this->mAngleAcc!=0)  da_stop=this->mAngleAcc ;
          else                    da_stop=a_step/50. ;

#define  _DIR_ATTEMPTS   50

/*------------------------------------------ Выход в стартовую точку */

    if(this->mResetStart) {

      for(i=0 ; i<cnt ; i++)
        if(degrees[i].fixed==0)
             degrees[i].value=(degrees[i].value_min+
                               degrees[i].value_max )/2. ;
                          }
/*---------------------------- Собственно поиск целевой конфигурации */

          status=object->vSolveByJoints() ;                         /* Решаем прямую задачу в исходной точке */
       if(status && mCheckRanges) {
              MessageBox(NULL, "Начальная точка поиска имеет "
                               "нарушение диапазонов степеней",
                               "Выход в точку", 0) ;
                                    return(0) ;
                                  }

                 object->vGetTargetPoint(&Top) ;                    /* Получаем положение целевой точки */  

             range_prv=iCalcRange(&mTarget, &Top, l_step, a_step) ; /* Рассчитываем 'целевую дистанцию' */
                v_size=  1. ;
             stop_flag=  0 ;
             fail_cnt =  0 ;
             fail_all =  0 ;

     do {                                                           /* CIRCLE.1 - Перебор векторов */
                iGenerateVector(vector, move_cnt) ;                 /* Генерируем вектор перемещения */

       do {                                                         /* CIRCLE.2 - Движение по вектору */
              memmove(degrees_prv, degrees, sizeof(degrees)) ;      /* Сохраняем текущий вектор степеней подвижности */

           if(this->kernel->stop)  break ;                          /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - - - - - - - - - -  Анализ завершения */
               object->vGetTargetPoint(&Top) ;                      /* Получаем текущее положение целевой точки */

           if(     fabs(Top.x   -mTarget.x   )<dl_stop &&
                   fabs(Top.y   -mTarget.y   )<dl_stop &&
                   fabs(Top.z   -mTarget.z   )<dl_stop &&
              iAngleDif(Top.azim-mTarget.azim)<da_stop &&
              iAngleDif(Top.elev-mTarget.elev)<da_stop &&
              iAngleDif(Top.roll-mTarget.roll)<da_stop   ) {
                                   stop_flag=1 ;
                                        break ;
                                                           }
/*- - - - - - - - - - - - - - - - - - - - - Редукция шага по вектору */
                 v_size=range_prv/this->mScanStepCoef  ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Шаг по вектору */
              for(j=0, i=0 ; i<cnt ; i++)                           /* Делаем шаг по вектору */
                if(degrees[i].fixed==0) {
                 if(degrees[i].type==_L_TYPE)
                         degrees[i].value+=vector[j]*v_size*l_step ;
                 else    degrees[i].value+=vector[j]*v_size*a_step ;
                                                  j++ ;
                                        }

                     object->vSetJoints     (degrees, cnt) ;        /* Задаем вектор степеней подвижности */
              status=object->vSolveByJoints () ;                    /* Решаем прямую задачу в исходной точке */
           if(status && mCheckRanges) {                             /* Если нарущение ограничений в степенях  - */
                memmove(degrees, degrees_prv, sizeof(degrees)) ;    /*  восстанавливаем предыдущую точку        */
                                         fail_cnt++ ;               /*   и генерируем другой вектор             */
                                             break ;     
                                      }

                     object->vGetTargetPoint(&Top) ;                /* Получаем положение целевой точки */  
/*- - - - - - - - - - - - - - - - - - - - - - - - - Анализ сближения */
              range=iCalcRange(&mTarget, &Top, l_step, a_step) ;    /* Рассчитываем 'целевую дистанцию' */

           if(range>range_prv) {                                    /* Если мы начинаем удаляться -      */
                memmove(degrees, degrees_prv, sizeof(degrees)) ;    /*  восстанавливаем предыдущую точку */
                                  fail_cnt++ ;                      /*   и генерируем другой вектор      */
                                      break ;     
                               }
          
           if(range<range_prv)                                      /* Если положение изменилось - */
            if(this->mDraw)  this->kernel->vShow(NULL) ;            /*   - отрисовываем сцену      */

                    range_prv=range ;
                     fail_cnt=  0 ;
                     fail_all=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          } while(1) ;                                              /* CONTINUE.2 */

         if(this->kernel->stop)  break ;                            /* Если внешнее прерывание поиска */
/*- - - - - - - - - - - - - - - Контроль завершения по числу попыток */
         if(fail_all>mFailMax) {
              status=MessageBox(NULL, "Достигнуто критическое число "
                                      "неудачных попыток.\nПродолжать?",
                                      "Выход в точку", 
                                       MB_ICONQUESTION | MB_YESNO) ;
           if(status==IDNO)  break ;
                               }
/*- - - - - - - - - - - - - - - - - - - Отскок по случайному вектору */
         if(fail_cnt>_DIR_ATTEMPTS) {

                iGenerateVector(vector, move_cnt) ;                 /* Генерируем вектор отскока */

                    coef=1.0 ;

           for(n=0 ; n<mJampRangeCoef ; n++) {                      /* CIRCLE - Движение по вектору отскока */

                memmove(degrees_prv, degrees, sizeof(degrees)) ;    /* Сохраняем текущий вектор степеней подвижности */

               if(this->kernel->stop)  break ;                      /* Если внешнее прерывание поиска */

              for(j=0, i=0 ; i<cnt ; i++)                           /* Делаем шаг по вектору */
                if(degrees[i].fixed==0) {
                 if(degrees[i].type==_L_TYPE) 
                         degrees[i].value+=vector[j]*v_size*l_step*coef  ;
                 else    degrees[i].value+=vector[j]*v_size*a_step*coef ;
                                                  j++ ;
                                        }

                     object->vSetJoints    (degrees, cnt) ;         /* Задаем вектор степеней подвижности */
              status=object->vSolveByJoints() ;                     /* Решаем прямую задачу в исходной точке */
           if(status && mCheckRanges) {                             /* Если нарущение ограничений в степенях  - */
                memmove(degrees, degrees_prv, sizeof(degrees)) ;    /*  восстанавливаем предыдущую точку        */
                                                                    /*   и либо сокращаем шаг, либо выходим     */
              if(n==0)  coef*=0.5 ;
              else       break ;     
                                      }
                                             }                      /* CONTINUE - Движение по вектору отскока */

                     object->vGetTargetPoint(&Top) ;                /* Получаем положение целевой точки */  

             range_prv=iCalcRange(&mTarget, &Top, l_step, a_step) ; /* Рассчитываем 'целевую дистанцию' */
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
               if(this->kernel->stop)  break ;                      /* Если внешнее прерывание поиска */

        } while(stop_flag==0) ;                                     /* CONTINUE.1 */

         if(this->kernel->stop)                                     /* Если внешнее прерывание поиска -                    */
                   object->vSetJoints(degrees_prv, cnt) ;           /*  - восстанавливаем последнюю "хорошую" конфигурацию */
/*---------------------------- Проверяем непротиворечивость свойств  */

   if(fail_all<=mFailMax) {

        status=object->vCheckFeatures(NULL) ;
     if(status)  SEND_CHECK(iErrorDecode(status)) ;
                          }
/*------------------------------------------------ Отображение сцены */

                       this->kernel->vShow(NULL) ;

/*------------------------- Приведение углов в канонический диапазон */

           cnt=object->vGetJoints(degrees) ;

    for(i=0 ; i<cnt ; i++)
      if(degrees[i].fixed==0)
       if(degrees[i].type==_A_TYPE) {
         while(degrees[i].value<-180.)  degrees[i].value+=360. ;
         while(degrees[i].value> 180.)  degrees[i].value-=360. ;                                              
                                    }

               object->vSetJoints(degrees, cnt) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ROTATE                  */
/*      ROTATE    <Имя>                                             */

  int  RSS_Module_Reach::cRotate(char *cmd)

{ 

#define  _COORD_MAX   6
#define   _PARS_MAX   7

             char  *pars[_PARS_MAX] ;
             char  *name ;
       RSS_Object  *object ;
             char  *end ;
        RSS_IFace  iface ;
        RSS_Point   Freeze ;            /* Начальное положение целевой точки объекта */
        RSS_Point   Top ;               /* Текущее положение целевой точки объекта */
        RSS_Point   Probe ;             /* Пробное положение целевой точки объекта */
        RSS_Point   Cr ;
              int   corr[10] ;
        RSS_Joint   degrees[50] ;       /* Вектор степеней подвижности объекта */
        RSS_Joint   degrees_dlt[50] ;
        RSS_Joint   degrees_prv[50] ;
        RSS_Point   delta[50] ;
              int   delta_vector[50] ;
              int   base_vector[50] ;
              int   assign_cnt ;
              int   cnt ;               /* Число степеней подвижности */
           double   vector[50] ;        /* Вектор приращения степеней подвижности */
           double   l_step ;            /* Шаг движения по линейным координат */
           double   a_step ;            /* Шаг движения по угловым координат */
           double   step_size ;
           double   coef ;
         Matrix2d   mtx ;
           double   det, det1, det2, det3 ;
              int   status ;
              int   new_vector ;
              int   i ;
              int   j ;
              int   k ;

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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ROTATE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {  object=OBJECTS[i] ;
                                                            break ;  }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Анализируемого объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*------------------------------------------ Проверка исходной точки */

          status=object->vSolveByJoints() ;                         /* Решаем прямую задачу в исходной точке */
       if(status) {
              MessageBox(NULL, "В начальной точке поиска имеется "
                               "нарушение диапазонов степеней",
                               "Проворот в точке", 0) ;
                                    return(0) ;
                  }

          status=object->vCheckFeatures(NULL) ;                     /* Проверяем непротиворечивость свойств */
       if(status) {
              MessageBox(NULL, "В начальной точке поиска имеется "
                               "пересечение тел сцены",
                               "Проворот в точке", 0) ;
                                    return(0) ;
                  }
/*------------------------------------------ Подготовка сканирования */

          if(this->mLineAcc !=0)  l_step=this->mLineAcc ;
          else                    l_step= 0.01 ;

          if(this->mAngleAcc!=0)  a_step=this->mAngleAcc ;
          else                    a_step= 1.00 ;

                 object->vGetTargetPoint(&Freeze) ;                 /* Получаем исходное положение целевой точки */

/*======================================== Сканирование конфигураций */

                     mtx.Create(3,3) ;

                      new_vector=1 ;

  do {
         if(this->kernel->stop) break ;

           cnt=object->vGetJoints(degrees_prv) ;                    /* Извлекаем текущую конфигурацию */

/*----------------------------------- Формирование вектора изменения */

   if(new_vector) {
/*- - - - - - - - - - - - - - - - - - Определение долевых приращений */

              memset(delta_vector, 0, sizeof(delta_vector)) ;
              memset(delta,        0, sizeof(delta       )) ;

     for(i=0 ; i<cnt ; i++)
       if(degrees_prv[i].fixed==0) {

        for(k=1 ; k<3 ; k++) {

               memcpy(degrees, degrees_prv, sizeof(degrees)) ;

           if(k==1)  coef=-1. ;
           else      coef= 1. ;

           if(degrees[i].type==_A_TYPE) coef*=a_step/2. ;
           else                         coef*=l_step/2. ;

                                        degrees[i].value+=coef ;

                 object->vSetJoints(degrees, cnt) ;
          status=object->vSolveByJoints() ;                         /* Решаем прямую задачу в пробной точке */
       if(status)  continue ;

                   delta_vector[i]+=k ;

                 object->vGetTargetPoint(&Top) ;                    /* Получаем перемещение пробной точки */

            delta[i].x=(Top.x-Freeze.x)/coef ;
            delta[i].y=(Top.y-Freeze.y)/coef ;
            delta[i].z=(Top.z-Freeze.z)/coef ;
                             }
                                 }
/*- - - - - - - - - - - - - Определение однозначно-задающих степеней */
              memset(base_vector, 0, sizeof(base_vector)) ;

     for(i=0 ; i<cnt ; i++)
       if(degrees_prv[i].fixed==0) {

         if(delta_vector[i]==1) {  base_vector[i]=-1 ;  continue ;  }
         if(delta_vector[i]==2) {  base_vector[i]= 1 ;  continue ;  }

         if(delta[i].x!=0.) {
              for(coef=0., j=0 ; j<cnt ; j++)  if(j!=i)  coef+=fabs(delta[i].x) ;
                if(coef==0.) {  base_vector[i]= 2 ;  continue ;  }
                            }

         if(delta[i].y!=0.) {
               for(coef=0., j=0 ; j<cnt ; j++)  if(j!=i)  coef+=fabs(delta[i].y) ;
                 if(coef==0.) {  base_vector[i]= 2 ;  continue ;  }
                            }

         if(delta[i].z!=0.) {
               for(coef=0., j=0 ; j<cnt ; j++)  if(j!=i)  coef+=fabs(delta[i].z) ;
                 if(coef==0.) {  base_vector[i]= 2 ;  continue ;  }
                            }
                                   }
       else                        {
                                      base_vector[i]=3 ;
                                   }
/*- - - - - - - - - - - - -  Определение остальных задающих степеней */
                                assign_cnt=0 ;

        for(i=0 ; i<cnt ; i++)  if(base_vector[i]!=0)  assign_cnt++ ;

           assign_cnt=cnt-3-assign_cnt ;

     while(assign_cnt>0) {

                        j=((long)cnt*rand())/RAND_MAX ;
         if(            j       >=cnt || 
            base_vector[j]      != 0    )  continue ; 

            base_vector[j]=rand()>RAND_MAX/2?-1:1 ;
                             assign_cnt-- ;
                         }
/*- - - - - - - - - - - - - - - - - -  Определение задающего вектора */
          memset(vector, 0, sizeof(vector)) ;

        for(coef=0., i=0 ; i<cnt ; i++)
          if(base_vector[i]== 1 ||
             base_vector[i]==-1   ) {  vector[i]=rand() ;  coef+=vector[i]*vector[i] ;  }

               coef=sqrt(coef) ;

        for(i=0 ; i<cnt ; i++)
          if(base_vector[i]== 1 ||
             base_vector[i]==-1   )  vector[i]=(vector[i]/coef)*base_vector[i] ;
/*- - - - - - - - - - - - - - -  Определение корректирующих степеней */
        for(i=0, j=0 ; i<cnt ; i++)
          if(base_vector[i]==0) {
                                    corr[j]=i ;
                                         j++ ;
                                }

          if(j!=3) {
                        object->vSetJoints(degrees_prv, cnt) ;
                            new_vector=1 ;
                               continue ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  }
/*------------------------------------ Движение по вектору изменения */

                      step_size=1. ;

   do {
          if(this->kernel->stop) break ;

                     new_vector=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - Шаг по вектору */
               memcpy(degrees, degrees_prv, sizeof(degrees)) ;

        for(i=0 ; i<cnt ; i++) {
           if(degrees[i].type==_A_TYPE) degrees[i].value+=vector[i]*a_step*step_size ;
           else                         degrees[i].value+=vector[i]*l_step*step_size ;
                               }

                   object->vSetJoints(degrees, cnt) ;
            status=object->vSolveByJoints() ;                       /* Решаем прямую задачу в пробной точке */
         if(status) {
                       object->vSetJoints(degrees_prv, cnt) ;
                        new_vector=1 ;
                           break ;
                    }

                 object->vGetTargetPoint(&Top) ;                    /* Получаем первичную точку */

                   Cr.x=Freeze.x-Top.x ;
                   Cr.y=Freeze.y-Top.y ;
                   Cr.z=Freeze.z-Top.z ;
/*- - - - - - - - - - - - - - - - - - Определение долевых приращений */
     for(j=0 ; j<3 ; j++) {
                               i=corr[j] ;

        for(k=1 ; k<3 ; k++) {

               memcpy(degrees_dlt, degrees, sizeof(degrees)) ;

           if(k==1)  coef=-1. ;
           else      coef= 1. ;

           if(degrees_dlt[i].type==_A_TYPE) coef*=a_step/2. ;
           else                             coef*=l_step/2. ;

                                      degrees_dlt[i].value+=coef ;

                 object->vSetJoints(degrees_dlt, cnt) ;
          status=object->vSolveByJoints() ;                         /* Решаем прямую задачу в пробной точке */
       if(status)  continue ;

                 object->vGetTargetPoint(&Probe) ;                  /* Получаем перемещение пробной точки */

            delta[i].x=(Probe.x-Top.x)/coef ;
            delta[i].y=(Probe.y-Top.y)/coef ;
            delta[i].z=(Probe.z-Top.z)/coef ;
                             }
                          }
/*- - - - - - - - - - - - - - - -  Отработка корректирующих степеней */
#define  X(n)  delta[corr[n]].x
#define  Y(n)  delta[corr[n]].y
#define  Z(n)  delta[corr[n]].z

                 mtx.SetCell(0,0,X(0)) ;  mtx.SetCell(0,1,X(1)) ;  mtx.SetCell(0,2,X(2)) ;
                 mtx.SetCell(1,0,Y(0)) ;  mtx.SetCell(1,1,Y(1)) ;  mtx.SetCell(1,2,Y(2)) ;
                 mtx.SetCell(2,0,Z(0)) ;  mtx.SetCell(2,1,Z(1)) ;  mtx.SetCell(2,2,Z(2)) ;
             det=mtx.Determinant() ;
          if(det==0.) {
                        object->vSetJoints(degrees_prv, cnt) ;
                            new_vector=1 ;
                               break ;
                      }

                  mtx.SetCell(0,0,Cr.x) ;  mtx.SetCell(0,1,X(1)) ;  mtx.SetCell(0,2,X(2)) ;
                  mtx.SetCell(1,0,Cr.y) ;  mtx.SetCell(1,1,Y(1)) ;  mtx.SetCell(1,2,Y(2)) ;
                  mtx.SetCell(2,0,Cr.z) ;  mtx.SetCell(2,1,Z(1)) ;  mtx.SetCell(2,2,Z(2)) ;
             det1=mtx.Determinant() ;

                  mtx.SetCell(0,0,X(0)) ;  mtx.SetCell(0,1,Cr.x) ;  mtx.SetCell(0,2,X(2)) ;
                  mtx.SetCell(1,0,Y(0)) ;  mtx.SetCell(1,1,Cr.y) ;  mtx.SetCell(1,2,Y(2)) ;
                  mtx.SetCell(2,0,Z(0)) ;  mtx.SetCell(2,1,Cr.z) ;  mtx.SetCell(2,2,Z(2)) ;
             det2=mtx.Determinant() ;

                  mtx.SetCell(0,0,X(0)) ;  mtx.SetCell(0,1,X(1)) ;  mtx.SetCell(0,2,Cr.x) ;
                  mtx.SetCell(1,0,Y(0)) ;  mtx.SetCell(1,1,Y(1)) ;  mtx.SetCell(1,2,Cr.y) ;
                  mtx.SetCell(2,0,Z(0)) ;  mtx.SetCell(2,1,Z(1)) ;  mtx.SetCell(2,2,Cr.z) ;
             det3=mtx.Determinant() ;
 
                   degrees[corr[0]].value+=det1/det ;
                   degrees[corr[1]].value+=det2/det ;
                   degrees[corr[2]].value+=det3/det ;

#undef   X
#undef   Y
#undef   Z

                             object->vSetJoints(degrees, cnt) ;
                      status=object->vSolveByJoints() ;             /* Решаем прямую задачу в пробной точке */
         if(!status)  status=object->vCheckFeatures (NULL) ;        /* Проверяем непротиворечивость свойств */
         if( status) {
                       object->vSetJoints(degrees_prv, cnt) ;
                        new_vector=1 ;
                           break ;
                     }
/*- - - - - - - - - - - - - - - - - - -  Проверка величины изменения */
       for(i=0 ; i<cnt ; i++) {
           if(degrees[i].type==_A_TYPE) coef=a_step ;
           else                         coef=l_step ;

        if(fabs(degrees[i].value-degrees_prv[i].value)>coef)  break ;
                              }
 
        if(cnt==i)  break ;

        if(step_size<0.01) {
                              object->vSetJoints(degrees_prv, cnt) ;
                                new_vector=1 ;
                                  break ;
                           }

            step_size/=2. ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(1) ;

         if(this->kernel->stop)  break ;

         if(new_vector)  continue ;

/*-------------------------------------------------------------------*/

                           this->kernel->vShow(NULL) ;
     } while(1) ;

         if(this->kernel->stop)                                     /* Если внешнее прерывание поиска -                    */
                    object->vSetJoints(degrees_prv, cnt) ;          /*  - восстанавливаем последнюю "хорошую" конфигурацию */

                     mtx.Free() ;

/*===================================================================*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*              Реализация инструкции TARGETINFO                    */

  int  RSS_Module_Reach::cTargetInfo(char *cmd)

{
             char   text[4096] ;

/*-------------------------------------------- Формирование описания */

      sprintf(text, "Target point\r\n"
                    "     X  %7.3lf\r\n" 
                    "     Y  %7.3lf\r\n" 
                    "     Z  %7.3lf\r\n"
                    "  Azim %8.3lf\r\n" 
                    "  Elev %8.3lf\r\n" 
                    "  Roll %8.3lf\r\n" 
                    "\r\n",
                      mTarget.x,    mTarget.y,    mTarget.z,
                      mTarget.azim, mTarget.elev, mTarget.roll) ;

/*-------------------------------------------------- Выдача описания */

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                     Расшифровка кода ошибки                       */

  char *RSS_Module_Reach::iErrorDecode(int  error)

{
   if(error==_RSS_OBJECT_NOTARGETED        )  return("Анализируемый объект является НЕИЗМЕНЯЕМЫМ") ;
   if(error==_RSS_OBJECT_UNATTAINABLE_POINT)  return("Недостижимая точка") ;
   if(error==_RSS_OBJECT_DEGREES_LIMIT     )  return("Нарушение диапазона значений в степени подвижности") ;
   if(error==_RSS_OBJECT_LOW_ACTIVE        )  return("Число степеней подвижности меньше 6") ;
   if(error==_RSS_OBJECT_MATCH_ACTIVE      )  return("Число степеней подвижности больше 6") ;
   if(error==_RSS_OBJECT_COLLISION_EXTERNAL)  return("Столкновение тел разных объектов сцены") ;
   if(error==_RSS_OBJECT_COLLISION_INTERNAL)  return("Столкновение тел одного из объектов сцены") ;
   if(error==_RSS_OBJECT_BAD_SCHEME        )  return("Некорректная схема") ;
   if(error==_RSS_OBJECT_UNKNOWN_ERROR     )  return("Неизвестная ошибка") ;
                                              return("Код ошибки неопределен") ;
}


/*********************************************************************/
/*								     */
/*                    Генерация случайного вектора                   */

  void  RSS_Module_Reach::iGenerateVector(double *vector, int  cnt)

{
  double  rad ;
     int  i ;


      for(rad=0., i=0 ; i<cnt ; i++) {                              /* Задаем случайный вектор */
                  vector[i]=rand()-RAND_MAX/2 ;                     /*  и определяем его норму */
                       rad+=vector[i]*vector[i] ;
                                     }

	               rad=sqrt(rad) ;

      for(i=0 ; i<cnt ; i++)  vector[i]/=rad ;                      /* Нормализуем вектор */
}


/*********************************************************************/
/*								     */
/*                    Вычисление 'целевой' дистанции                 */

  double  RSS_Module_Reach::iCalcRange(RSS_Point *p1, 
                                       RSS_Point *p2,
                                          double  l_step,
                                          double  a_step )
{
   double  rad ;
   double  dst ;


                 rad = 0 ;

                 dst =fabs(p1->x-p2->x)/l_step ;
                 rad+= dst*dst ;

                 dst =fabs(p1->y-p2->y)/l_step ;
                 rad+= dst*dst ;

                 dst =fabs(p1->z-p2->z)/l_step ;
                 rad+= dst*dst ;        

                 dst =iAngleDif(p1->azim-p2->azim)/a_step ;
                 rad+= dst*dst ;

                 dst =iAngleDif(p1->elev-p2->elev)/a_step ;
                 rad+= dst*dst ;

                 dst =iAngleDif(p1->roll-p2->roll)/a_step ;
                 rad+= dst*dst ;

                 rad = sqrt(rad) ;

  return(rad) ;
}


/*********************************************************************/
/*								     */
/*                    Вычисление разности углов                      */

  double  RSS_Module_Reach::iAngleDif(double  angle)
{
  double  dst ;

                 dst =fmod(fabs(angle), 360.) ;
   if(dst>180.)  dst =360.-dst ;

  return(dst) ;
}

