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

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Reach.h"

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

               mLineAcc=  0. ;
              mAngleAcc=  0. ;
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
              int   status ;
              int   fail_cnt ;
              int   fail_all ;
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

#define  _DIR_ATTEMPTS   10

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
/*- - - - - - - - - - - - - - - Контроль завершения по числу попыток */
         if(fail_all>mFailMax ) {
              status=MessageBox(NULL, "Достигнуто критическое число "
                                      "неудачных попыток.\nПродолжать?",
                                      "Выход в точку", 
                                       MB_ICONQUESTION | MB_YESNO) ;
           if(status==IDNO)  break ;
                                }
/*- - - - - - - - - - - - - - - - - - - Отскок по случайному вектору */
         if(fail_cnt>_DIR_ATTEMPTS) {
                                                fail_cnt=0 ;

                iGenerateVector(vector, move_cnt) ;                 /* Генерируем вектор отскока */

              for(j=0, i=0 ; i<cnt ; i++)                           /* Делаем шаг по вектору */
                if(degrees[i].fixed==0) {
                 if(degrees[i].type==_L_TYPE) 
                         degrees[i].value+=vector[j]*v_size*l_step*mJampRangeCoef  ;
                 else    degrees[i].value+=vector[j]*v_size*a_step*mJampRangeCoef ;
                                                  j++ ;
                                        }

                     object->vSetJoints     (degrees, cnt) ;        /* Задаем вектор степеней подвижности */
              status=object->vSolveByJoints () ;                    /* Решаем прямую задачу в исходной точке */
                     object->vGetTargetPoint(&Top) ;                /* Получаем положение целевой точки */  

             range_prv=iCalcRange(&mTarget, &Top, l_step, a_step) ; /* Рассчитываем 'целевую дистанцию' */
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(stop_flag==0) ;                                     /* CONTINUE.1 */

/*------------------------------------------------ Отображение сцены */

                       this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

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

