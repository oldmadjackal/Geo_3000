/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ТОЧКИ"    		    */
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

#include "T_Point.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  SEND_CHECK(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_CHECK_MESSAGE,        \
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

     static   RSS_Module_Point  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_POINT_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_POINT_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	       ОПИСАНИЕ КЛАССА ЗАДАЧИ "АНАЛИЗ ТОЧКИ"               **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Point_instr  RSS_Module_Point_InstrList[]={

 { "help",       "?",  "#HELP (?) - список доступных команд",
                        NULL,
                       &RSS_Module_Point::cHelp   },
 { "check",      "c",  "#CHECK (C) - проверка текущей конфигурации объекта", 
                       " CHECK <Имя объекта>\n",
                       &RSS_Module_Point::cCheck  },
 { "target",     "t",  "#TARGET (T) - задает целевую точку объекта для решения\n"
                       "   обратной кинематической задачи и проверки условий допустимости", 
                       " TARGET  <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Elev> <Roll>\n"
                       "   Проанализировать точку заданную координатами\n"
                       " TARGET  <Имя>[:<Неопр.>] <Цель> [<Azim> <Elev> <Roll>]\n"
                       "   Проанализировать точку заданную целевым обьектом\n"
                       " TARGET/x <Имя>[:<Неопр.>] <x>\n"
                       "   Задает координату X анализируемой точки\n"
                       "       (аналогично для Y, Z, A, E, R)\n"
                       " TARGET/+x <Имя>[:<Неопр.>] <x>\n"
                       "   Задает приращение координаты X анализируемой точки\n"
                       "       (аналогично для Y, Z, A, E, R)\n"
                       " TARGET. <Имя>[:<Неопр.>]\n"
                       "   Проанализировать предыдущую точку без изменений\n"
                       " TARGET> <Имя>\n"
                       "   Задает клавиатурное управление анализируемой точкой\n",
                       &RSS_Module_Point::cTarget },
 { "targetinfo", "ti", "#TARGETINFO (TI) - отобразить параметры целевой  точки", 
                        NULL,
                       &RSS_Module_Point::cTargetInfo },
 { "round",      "r",  "#ROUND (R) - просмотр кругового сектора для одного\n"
                       "   из углов ориентации в целевой точке", 
                       " ROUND/A[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Elev> <Roll>\n"
                       " ROUND/E[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Roll>\n"
                       " ROUND/R[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Elev>\n"
                       "   Проанализировать по азимуту (крену, тангажу) точку заданную координатами\n"
                       " ROUND/A[D][I] <Имя>[:<Неопр.>] <Цель> <Elev> <Roll>\n"
                       " ROUND/E[D][I] <Имя>[:<Неопр.>] <Цель> <Azim> <Roll>\n"
                       " ROUND/R[D][I] <Имя>[:<Неопр.>] <Цель> <Azim> <Elev>\n"
                       "   Проанализировать по азимуту (крену, тангажу) точку заданную целевым обьектом\n",
                       &RSS_Module_Point::cRound },
 { "ilist",      "il", "#ILIST (IL) - выдает список индикаторов анализа точек", 
                        NULL,
                       &RSS_Module_Point::cIndicatorList },
 { "ikill",      "ik", "#IKILL (IK) - удаляет индикаторов анализа", 
                       " IKILL          - удалить последний индикатор\n"
                       " IKILL <Индекс> - удалить индикатор с заданным индексом\n",
                       &RSS_Module_Point::cIndicatorKill },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Point_instr *RSS_Module_Point::mInstrList=NULL ;
                        RSS_Point  RSS_Module_Point::mTarget ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Point::RSS_Module_Point(void)

{
  static  WNDCLASS  RoundIndicator_wnd ;

/*-------------------------------------------- Инициализация Open GL */

	   keyword="Geo" ;
    identification="Point_task" ;

        mInstrList=RSS_Module_Point_InstrList ;

     memset(mIndicators, 0, sizeof(mIndicators)) ;

            mIndicator_last=NULL ;

        mRoundStep=  2. ;
          mIndSize=  5. ;
         mBadColor=  0 ;
       mGoodAColor=RGB(0, 128,   0) ;
       mGoodEColor=RGB(0, 128,  64) ;
       mGoodRColor=RGB(0, 128, 128) ;

/*------------------------ Регистрация класса элемент RoundIndicator */

	RoundIndicator_wnd.lpszClassName="Task_Point_RoundIndicator_class" ;
	RoundIndicator_wnd.hInstance    = GetModuleHandle(NULL) ;
	RoundIndicator_wnd.lpfnWndProc  = Task_Point_RoundIndicator_prc ;
	RoundIndicator_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	RoundIndicator_wnd.hIcon        =  NULL ;
	RoundIndicator_wnd.lpszMenuName =  NULL ;
	RoundIndicator_wnd.hbrBackground=  NULL ;
	RoundIndicator_wnd.style        =    0 ;
	RoundIndicator_wnd.hIcon        =  NULL ;

            RegisterClass(&RoundIndicator_wnd) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Point::~RSS_Module_Point(void)

{
}


/********************************************************************/
/*								    */
/*                Отобразить связанные данные                       */

    void  RSS_Module_Point::vShow(char *layer)

{
     int  status ;
  double  zoom ;
     int  i ;
  
/*----------------------------------------- Масштабирование картинки */

  if(!stricmp(layer, "ZOOM")) {

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------------------- Перебор индикаторов */

     for(i=0 ; i<_IND_MAX ; i++) {                                  /* CIRCLE.2 */

       if(mIndicators[i]==NULL)  continue ;

#define   I    mIndicators[i]

       if(I->dlist1_idx==0)  continue ;

             glNewList(I->dlist1_idx, GL_COMPILE) ;                 /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(I->point.x, I->point.y, I->point.z) ;

     if(I->angle_code=='E' ||
        I->angle_code=='R'   )
             glRotated(I->point.azim, 0., 1., 0.) ;

     if(I->angle_code=='R'   )
             glRotated(-I->point.elev, 1., 0., 0.) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/mIndSize ;

              glScaled(zoom, zoom, zoom) ;

            glCallList(I->dlist2_idx) ;                             /* Отрисовка розы направлений */

             glEndList() ;                                          /* Закрытие группы */

#undef   I

                               }                                    /* CONTINUE.2 */
/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/
                               }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Point::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Point::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "POINT"
#define  _SECTION_SHRT_NAME   "P"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Point:") ;
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
       if(status)  SEND_ERROR("Секция POINT: Неизвестная команда") ;
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
/*		        Записать данные в строку		    */

    void  RSS_Module_Point::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE POINT\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Point::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - Анализ сервисной точки", "POINT") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*             Выполнить действие в контексте потока                */

    void  RSS_Module_Point::vChangeContext(void)

{
/*------------------------------------------- Отображение индикатора */

   if(!stricmp(mContextAction, "SHOW")) {

                      iIndicatorShow(mContextObject) ;
                                        }
/*----------------------------------------- Вывод списка индикаторов */

   if(!stricmp(mContextAction, "LIST")) {

    this->mIndListWnd=CREATE_DIALOG(GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_INDICATORS_LIST", RT_DIALOG),
           	                      NULL/*GetActiveWindow()*/, Task_Point_IList_dialog, 
                                       (LPARAM)this ) ;
                         ShowWindow(this->mIndListWnd, SW_SHOW) ;
                                        }
/*-------------------------- Отображение индикатора в отдельном окне */

   if(!stricmp(mContextAction, "TOP" )) {

      mContextObject->hWnd=CREATE_DIALOG(GetModuleHandle(NULL),
 	                                 (LPCDLGTEMPLATE)Resource("IDD_ROUND", RT_DIALOG),
		  	                   NULL, Task_Point_Round_dialog, 
                                            (LPARAM)mContextObject) ;
                              ShowWindow(mContextObject->hWnd, SW_SHOW) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Point::cHelp(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Point_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      Реализация инструкции INDICATORS_LIST                 */

  int  RSS_Module_Point::cIndicatorList(char *cmd, RSS_IFace *iface)

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
/*              Реализация инструкции ROUND_SET                     */

  int  RSS_Module_Point::cIndicatorKill(char *cmd, RSS_IFace *iface)

{
    int  idx ;
   char *end ;

/*----------------------------------- Удаление последнего индикатора */

   if(*cmd==0) {
                  if(mIndicator_last==NULL)  return(0) ;

        if(this->mIndListWnd!=NULL)
            SendMessage(this->mIndListWnd, WM_USER, 
                                    (WPARAM)_TASK_POINT_KILL_INDICATOR,
                                     (LPARAM)mIndicator_last) ;

                      this->iIndicatorKill(mIndicator_last) ;
                       this->kernel->vShow("REFRESH") ;

                         mIndicator_last=NULL ;                 
               }
/*----------------------------------- Удаление индикатора по индексу */

   else        {

                    idx=strtoul(cmd, &end, 10) ;

        if(*end!=  0      ||
            idx>=_IND_MAX   ) {
                        SEND_ERROR("Секция POINT: Неправильный индекс индикатора") ;
                                      return(-1) ;
                              }

        if(mIndicators[idx]==NULL) {
                        SEND_ERROR("Секция POINT: Такого индикатора не существует") ;
                                      return(-1) ;
                                   }

        if(this->mIndListWnd!=NULL)
            SendMessage(this->mIndListWnd, WM_USER, 
                                    (WPARAM)_TASK_POINT_KILL_INDICATOR,
                                     (LPARAM)mIndicators[idx]) ;

                      this->iIndicatorKill(mIndicators[idx]) ;
                       this->kernel->vShow("REFRESH") ;
               }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции TARGET                  */
/*      TARGET    <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Elev> <Roll> */
/*      TARGET/X  <Имя>[:<Неопр.>] <X>                              */
/*      TARGET/+X <Имя>[:<Неопр.>] <X>                              */
/*      TARGET.   <Имя>[:<Неопр.>]                                  */
/*      TARGET>   <Имя>[:<Неопр.>] <Код стрелочки> <Шаг>            */
/*      TARGET>>  <Имя>[:<Неопр.>] <Код стрелочки> <Шаг>            */
/*                                                                  */
/*  <Неопр.> - Критерий разрешения неопределенности:                */
/*              - пустой, если используется предыдущий вариант      */
/*              - последовательность цифр 0 и 1, задающая вариант   */

  int  RSS_Module_Point::cTarget(char *cmd, RSS_IFace *iface)

{ 

#define  _COORD_MAX   6
#define   _PARS_MAX   7

        RSS_IFace   iface_ ;
             char  *pars[_PARS_MAX] ;
              int   quiet_flag ;
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
     static  char   undef[128] ;        /* Критерий разрешения неопределенности */
              int   status ;
              int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------- Входной контроль */

      if(iface==NULL)  iface=&iface_ ;                              /* При отсутствии межмодульного интерфейса */
                                                                    /*     используем локальную заглушку       */ 
/*---------------------------------------- Разборка командной строки */

                      quiet_flag=0 ;
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
           else if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag= 1 ;

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
/*------------------------ Спецификация разрешения неопределенностей */

    while(name!=NULL) {                                             /* BLOCK.1 */      

              end=strchr(name, ':') ;
           if(end==NULL) {
                            if(!arrow_flag)  undef[0]=0 ; 
                                break ;
                         }
             *end=0 ;
              end++ ; 

           if(*end!=0)  strcpy(undef, end) ;

                               break ;
                      }                                             /* BLOCK.1 */      
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
/*------------------------------------------ Собственно АНАЛИЗ точки */

     if(quiet_flag)  object->ErrorEnable=0 ;                        /* Если надо - подавляем сообщения об ошибках */
     else            object->ErrorEnable=1 ; 
/*- - - - - - - - - - - - - - Решение обратной кинематической задачи */
                         object->vSetTargetPoint(&mTarget, 1) ;     /* Уст.целевую точку */

     if(!arrow_flag)     object->vSetAmbiguity  (undef) ;           /* Уст.критерия разрешения неопределенности */

                  status=object->vSolveByTarget () ;                /* Решаем обратную задачу */
     if(!status)  status=object->vCheckFeatures (NULL) ;            /* Проверяем непротиворечивость свойств */

     if( status)
      if(!quiet_flag)  SEND_CHECK(iErrorDecode(status)) ;

/*---------------- Выдаем результат по интерфейсу межмодульной связи */

     if(!status) {  iface->vSignal("DONE",  "0"                 ) ;  }
     else        {  iface->vSignal("ERROR", iErrorDecode(status)) ;  }

/*------------------------------------------------ Отображение сцены */

                       this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CHECK                   */
/*      CHECK   <Имя объекта>                                       */

  int  RSS_Module_Point::cCheck(char *cmd, RSS_IFace *iface)

{ 

#define  _COORD_MAX   6
#define   _PARS_MAX   7

             char  *pars[_PARS_MAX] ;
             char  *name ;
       RSS_Object  *object ;
             char  *end ;
     static  char   undef[128] ;        /* Критерий разрешения неопределенности */
              int   status ;
              int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- Разборка командной строки */
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
                                 "Например: CHECK <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {  object=OBJECTS[i] ;
                                                            break ;  }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Анализируемого объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*------------------------------------------ Собственно АНАЛИЗ точки */

                   status=object->vSolveByJoints() ;                /* Решаем прямую задачу */
     if(!status)   status=object->vCheckFeatures(NULL) ;            /* Проверяем непротиворечивость свойств */

     if( status)   SEND_ERROR(iErrorDecode(status)) ;

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

  int  RSS_Module_Point::cTargetInfo(char *cmd, RSS_IFace *iface)

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
/*		      Реализация инструкции ROUND                    */
/*    ROUND/A[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Elev> <Roll>       */
/*    ROUND/E[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Roll>       */
/*    ROUND/R[D][I] <Имя>[:<Неопр.>] <X> <Y> <Z> <Azim> <Elev>       */
/*    ROUND/A[D][I] <Имя>[:<Неопр.>] <Цель> <Elev> <Roll>            */
/*    ROUND/E[D][I] <Имя>[:<Неопр.>] <Цель> <Azim> <Roll>            */
/*    ROUND/R[D][I] <Имя>[:<Неопр.>] <Цель> <Azim> <Elev>            */
/*                                                                   */
/*  <Неопр.> - Критерий разрешения неопределенности:                 */
/*              - пустой, если используется предыдущий вариант       */
/*              - последовательность цифр 0 и 1, задающая вариант    */
/*         D - отрисовывать положения                                */
/*         I - создать индикатор в отдельном окне                    */

  int  RSS_Module_Point::cRound(char *cmd, RSS_IFace *iface)

{
#define  _COORD_MAX   6
#define   _PARS_MAX   7

                        char  *pars[_PARS_MAX] ;
                        char  *name ;
                        char **xyz ;
                      double   coord[_COORD_MAX] ;
                         int   coord_cnt ;
                        char   obj_name[256] ;
  RSS_Module_Point_Indicator  *Indicator ;
                   RSS_Point   Target_prv ;
                   RSS_Point   Target ;
                  RSS_Object  *object ;
                        char   ind_name[128] ;
               static    int   angle_code ;      /* Код анализируемого угла ориентации */
                         int   indicate_flag ;   /* Флаг создания выделенного индикатора */
//                      HWND   hInd ;            /* Окно индикатора */
                         int   draw_flag ;       /* Флаг отрисовки манипулятора */
                         int   object_flag ;     /* Флаг работы с привязкой к объекту */
                         int   prev_flag ;       /* Флаг работы с предудущей точкой */
                         int   step_cnt ;
                      double   step ;
               static   char   undef_decl[128] ;
                        char   undef[128] ;
                         int   undef_cnt ;
                         int   save_ind ;
                         int   status ;
                        char  *end ;
                        char  *error ;
                         int   i ;
                         int   j ;
                         int   k ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- Инициализация */

                      Target_prv=mTarget ;

/*---------------------------------------- Разборка командной строки */

                   indicate_flag=0 ;
                       draw_flag=0 ;
                     object_flag=0 ;
                       prev_flag=0 ;
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
       if(*cmd=='.'   ) {
                             prev_flag=1 ;
                                cmd++ ;

                  while(1) {
                          if(*cmd=='i' || *cmd=='I') {  indicate_flag= 1 ;   cmd++; }
                     else if(*cmd=='d' || *cmd=='D') {      draw_flag= 1 ;   cmd++; }
                     else if(*cmd=='a' || *cmd=='A') {     angle_code='A' ;  cmd++; }
                     else if(*cmd=='e' || *cmd=='E') {     angle_code='E' ;  cmd++; }
                     else if(*cmd=='r' || *cmd=='R') {     angle_code='R' ;  cmd++; }
                     else                                      break ;
                           }

                                cmd++ ;
                        }
  else if(*cmd=='/') { 

                              cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'i')!=NULL ||
                   strchr(cmd, 'I')!=NULL   )  indicate_flag= 1 ;
                if(strchr(cmd, 'd')!=NULL ||
                   strchr(cmd, 'D')!=NULL   )      draw_flag= 1 ;

                                                  angle_code= 0 ;
                if(strchr(cmd, 'a')!=NULL ||
                   strchr(cmd, 'A')!=NULL   )     angle_code='A' ;
           else if(strchr(cmd, 'e')!=NULL ||
                   strchr(cmd, 'E')!=NULL   )     angle_code='E' ;
           else if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )     angle_code='R' ;

                           cmd=end+1 ;
                        }

         if(angle_code==0) {
                      SEND_ERROR("Не задано анализируемый угол подхода. \n"
                                 "Например: ROUND/E <Имя_объекта> ...") ;
                                     return(-1) ;
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
/*- - - - - - - - - - - - - - - Проверка на задание объекта привязки */
     if(pars[1]!=NULL) {
                          strtod(pars[1], &end) ;

                  if(*end!=0) {  object_flag=  1 ;
                                         xyz=&pars[2] ;   }
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
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
      if(!object_flag)
        if(coord_cnt<3)  error="Не указана координата" ;

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

              if(angle_code=='A') {  mTarget.elev=coord[0] ;        /* Угловые координаты */
                                     mTarget.roll=coord[1] ;  }
              if(angle_code=='E') {  mTarget.azim=coord[0] ;
                                     mTarget.roll=coord[1] ;  }
              if(angle_code=='R') {  mTarget.azim=coord[0] ;
                                     mTarget.elev=coord[1] ;  }
                        }
/*- - - - - - - - - - - - - - - - - - - - Работа с предыдущей точкой */
   else if(  prev_flag) {
                        }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else                {

                                    mTarget.x   =coord[0] ;
                                    mTarget.y   =coord[1] ;
                                    mTarget.z   =coord[2] ;

              if(angle_code=='A') {  mTarget.elev=coord[0] ;        /* Угловые координаты */
                                     mTarget.roll=coord[1] ;  }
              if(angle_code=='E') {  mTarget.azim=coord[0] ;
                                     mTarget.roll=coord[1] ;  }
              if(angle_code=='R') {  mTarget.azim=coord[0] ;
                                     mTarget.elev=coord[1] ;  }
                       }
/*------------------------ Спецификация разрешения неопределенностей */

    while(name!=NULL) {                                             /* BLOCK.1 */      

              end=strchr(name, ':') ;
           if(end==NULL) {
                            undef_decl[0]=0 ; 
                                break ;
                         }
             *end=0 ;
              end++ ; 

           if(*end!=0)  strcpy(undef_decl, end) ;

                               break ;
                      }                                             /* BLOCK.1 */      
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ROUND <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {  object=OBJECTS[i] ;
                                                            break ;  }

         if(i==OBJECTS_CNT) {                                       /* Если объект не найден... */
             SEND_ERROR("Анализируемого объекта с таким именем НЕ существует") ;
                                return(-1) ;
                            }
/*----------------------------------------------- Подготовка анализа */

                           step_cnt=360./mRoundStep+0.5 ;
                               step=360./((double)step_cnt) ;

                          undef_cnt=object->vGetAmbiguity() ;
    if(undef_decl[0]!=0)  undef_cnt=  0 ;

/*--------------------------------------- Создание записи-индикатора */

     for(i=0 ; i<_IND_MAX ; i++)                                    /* Поиск свободного слота */
       if(mIndicators[i]==NULL)  break ;

       if(i>=_IND_MAX) {
             SEND_ERROR("Список проанализированных точек слишком велик.\n"
                        "Очистите его"                                    ) ;
                                return(-1) ;
                       }

          Indicator=(RSS_Module_Point_Indicator *)
                       calloc(1, sizeof(RSS_Module_Point_Indicator)) ;
       if(Indicator==NULL) {
                    SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                return(-1) ;
                           }

                       Indicator->context =mIndicators ;
                       Indicator->idx     = i ;

               strncpy(Indicator->name, ind_name, sizeof(Indicator->name)-1) ;
                       Indicator->type    =_CIRCLE_IND ;
                       Indicator->data_cnt= step_cnt ;

                       Indicator->data    =(int *)calloc(step_cnt, sizeof(int)) ;
                    if(Indicator->data==NULL) {
                         SEND_ERROR("Недостаточно памяти для индикатора анализа.") ;
                                                 return(-1) ;
                                              }

        mIndicators[i]=Indicator ;

/*-------------------------------- Формирование заголовка индикатора */

                       Indicator->point     =mTarget ;
                       Indicator->angle_code=angle_code ;
       
       if(angle_code=='A')  
            sprintf(Indicator->name, "A-scan(E=%.0lf R=%.0lf) %.3lf %.3lf %.3lf", 
                                        mTarget.elev, mTarget.roll,
                                        mTarget.x, mTarget.y, mTarget.z) ;
       if(angle_code=='E')  
            sprintf(Indicator->name, "E-scan(A=%4.0lf R=%4.0lf) %7.3lf %7.3lf %7.3lf", 
                                        mTarget.azim, mTarget.roll,
                                        mTarget.x, mTarget.y, mTarget.z) ;
       if(angle_code=='R')  
            sprintf(Indicator->name, "R-scan(A=%4.0lf E=%4.0lf) %7.3lf %7.3lf %7.3lf", 
                                        mTarget.azim, mTarget.elev,
                                        mTarget.x, mTarget.y, mTarget.z) ;

/*------------------------------------ Создание индикатора на экране */

    if(indicate_flag) {
                        strcpy(mContextAction, "TOP") ;
                               mContextObject=Indicator ;

                   SendMessage(RSS_Kernel::kernel_wnd, 
                                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                                 (LPARAM) this                ) ;
                      }
/*----------------------------------------------------- Цикл анализа */

                           save_ind = 0 ;
                              Target=mTarget ;

       for(i=0 ; i<step_cnt ; i++) {                                /* CIRCLE.1 - Сканирование сектора */
/*- - - - - - - - - - - - - - - - - - -  Задание анализируемой точки */
         if(angle_code=='A')  Target.azim=step*i ;
         if(angle_code=='E')  Target.elev=step*i ;
         if(angle_code=='R')  Target.roll=step*i ;
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
                             object->vSetAmbiguity  (undef) ;      /* Уст.критерия разрешения неопределенности */
                             object->vSetTargetPoint(&Target, 1) ; /* Уст.целевую точку */
                             object->vErrorMessage  (0) ;
                      status=object->vSolveByTarget () ;           /* Решаем обратную задачу */
         if(!status)  status=object->vCheckFeatures (NULL) ;       /* Проверяем непротиворечивость свойств */
                             object->vErrorMessage  (1) ;
/*- - - - - - - - - - - - - - - - - - - - Запись результатов анализа */
                                   Indicator->scan     =step*i ;
                                   Indicator->scan_flag=  1 ;

         if(status)                Indicator->data[i]=mBadColor ;
         else       {
              if(angle_code=='A')  Indicator->data[i]=mGoodAColor ;
              if(angle_code=='E')  Indicator->data[i]=mGoodEColor ;
              if(angle_code=='R')  Indicator->data[i]=mGoodRColor ;
                                            save_ind = 1 ;
                    }
/*- - - - - - - - - - - - - - - - - - - - -  Управление отображением */
         if(indicate_flag) {
                SendMessage(Indicator->hWnd, WM_PAINT, NULL, NULL) ;
                           }

         if(draw_flag) {
                         iIndicatorShow_(Indicator) ;
                           this->kernel->vShow(NULL) ;
                       }
         else          {
                         iIndicatorShow_(Indicator) ;
                           this->kernel->vShow("REFRESH") ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
         if(status==0)  break ;                                     /* Если наиден "успешный" вариант..." */
                                          }                         /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                   }                                /* CONTINUE.1 */
/*----------------------- Окончательное отображение данных на экране */

                   Indicator->scan_flag=0 ;

         if(indicate_flag)
                SendMessage(Indicator->hWnd, WM_PAINT, NULL, NULL) ;

            iIndicatorShow_(Indicator) ;

                  this->kernel->vShow(NULL) ;

/*---------------------------- Обработка сводных результатов анализа */

    if(save_ind) {
                   if(this->mIndListWnd!=NULL)
                       SendMessage(this->mIndListWnd, WM_USER, 
                                    (WPARAM)_TASK_POINT_ADD_INDICATOR,
                                     (LPARAM)Indicator) ;
                                   mIndicator_last=Indicator ;
                 }
    else         {
                     iIndicatorKill(Indicator) ;
                                   mIndicator_last=NULL ;
                 }
/*----------------------------------------- Восстановление контекста */

                      mTarget=Target_prv ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                     Расшифровка кода ошибки                       */

  char *RSS_Module_Point::iErrorDecode(int  error)

{
   if(error==_RSS_OBJECT_NOTARGETED        )  return("Анализируемый объект является НЕИЗМЕНЯЕМЫМ") ;
   if(error==_RSS_OBJECT_UNATTAINABLE_POINT)  return("Недостижимая точка") ;
   if(error==_RSS_OBJECT_DEGREES_LIMIT     )  return("Нарушение диапазона значений в степени подвижности") ;
   if(error==_RSS_OBJECT_LOW_ACTIVE        )  return("Число степеней подвижности меньше 6") ;
   if(error==_RSS_OBJECT_MATCH_ACTIVE      )  return("Число степеней подвижности больше 6") ;
   if(error==_RSS_OBJECT_BAD_SCHEME        )  return("Некорректная схема") ;
   if(error==_RSS_OBJECT_COLLISION_EXTERNAL)  return("Столкновение тел разных объектов сцены") ;
   if(error==_RSS_OBJECT_COLLISION_INTERNAL)  return("Столкновение тел одного из объектов сцены") ;
   if(error==_RSS_OBJECT_UNKNOWN_ERROR     )  return("Неизвестная ошибка") ;
                                              return("Код ошибки неопределен") ;
}


/********************************************************************/
/*								    */
/*             Отображение индикатора с передачей контекста         */

  void  RSS_Module_Point::iIndicatorShow_(RSS_Module_Point_Indicator *Indicator)

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

  void  RSS_Module_Point::iIndicatorShow(RSS_Module_Point_Indicator *Indicator)

{
   double  step ;
   double  start ;
   double  dir ;
   double  angle ;
   double  zoom ;
   double  shift ;
      int  status ;
      int  i ;

#define    I  Indicator

#define  _PI  3.1415926

/*---------------------------- Определение необходимости отображения */

            for(i=0 ; i<I->data_cnt ; i++)
              if(I->data[i]!=0)  break ;

              if(i==I->data_cnt)  return ;

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

/*---------------------------------- Формирование "Розы направлений" */

             glNewList(I->dlist2_idx, GL_COMPILE) ;                 /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
              glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
           glDepthMask(0) ;                                         /* Откл.запись Z-буфера */

                                     step=2.*_PI/I->data_cnt ;

          if(I->angle_code=='A') {  start=  0 ;
                                      dir=  1.   ;  }
          if(I->angle_code=='E') {  start=  0 ;
                                      dir=  1.   ;  }
          if(I->angle_code=='R') {  start= _PI/2 ;
                                      dir=  1.   ;  }

           for(i=0 ; i<I->data_cnt ; i++) {

                      if(I->data[i]==0)  continue ;

                         angle=start+dir*step*i ;
                         shift=dir*step/2. ;

              glColor4d(GetRValue(I->data[i])/256., 
                        GetGValue(I->data[i])/256.,
                        GetBValue(I->data[i])/256., 0.3) ;

                glBegin(GL_POLYGON) ;

          if(I->angle_code=='A') {
             glVertex3d( 0.,              0.,  0.             ) ;
             glVertex3d(sin(angle-shift), 0., cos(angle-shift)) ;
             glVertex3d(sin(angle+shift), 0., cos(angle+shift)) ;
                                 }
          if(I->angle_code=='E') {
             glVertex3d(0.,      0.,                   0.     ) ;
             glVertex3d(0., sin(angle-shift), cos(angle-shift)) ;
             glVertex3d(0., sin(angle+shift), cos(angle+shift)) ;
                                 }
          if(I->angle_code=='R') {
             glVertex3d(     0.,               0.         , 0.) ;
             glVertex3d(cos(angle-shift), sin(angle-shift), 0.) ;
             glVertex3d(cos(angle+shift), sin(angle+shift), 0.) ;
                                 }
	          glEnd();

                                          }

           glDepthMask(1) ;                                         /* Вкл.запись Z-буфера */
             glDisable(GL_BLEND) ;                                  /* Откл.смешивание цветов */
             glEndList() ;                                          /* Закрытие группы */

/*----------------------------- Формирование масштабирующей оболочки */

             glNewList(I->dlist1_idx, GL_COMPILE) ;                 /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;
//      glLoadIdentity() ;

          glTranslated(I->point.x, I->point.y, I->point.z) ;

     if(I->angle_code=='E' ||
        I->angle_code=='R'   )
             glRotated(I->point.azim, 0., 1., 0.) ;

     if(I->angle_code=='R'   )
             glRotated(-I->point.elev, 1., 0., 0.) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/mIndSize ;
              glScaled(zoom, zoom, zoom) ;

            glCallList(I->dlist2_idx) ;                             /* Отрисовка розы направлений */

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

  void  RSS_Module_Point::iIndicatorKill(RSS_Module_Point_Indicator *Indicator)

{
  RSS_Module_Point_Indicator **data_list ;
                         int   i ; 


        data_list   =(RSS_Module_Point_Indicator **)                 /* Убираем запись об индикаторе */
                          Indicator->context ;  
                  i =     Indicator->idx ;                           /*   из списка индикаторов      */
        data_list[i]= NULL ;           

      if(Indicator->hWnd)  EndDialog(Indicator->hWnd, 0) ;           /* Удаление окна индикатора */

                      iIndicatorHide(Indicator) ;

                                free(Indicator->data) ;              /* Освобождаем ресурсы индикатора */
                                free(Indicator) ;
}


/*********************************************************************/
/*								     */
/*                    Удаление индикатора из сцены                   */

  void  RSS_Module_Point::iIndicatorHide(RSS_Module_Point_Indicator *Indicator)

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
/*                 Отображение индикатора в отдельном окне           */

  void  RSS_Module_Point::iIndicatorTop(RSS_Module_Point_Indicator *Indicator)

{
    strcpy(mContextAction, "TOP") ;
           mContextObject=Indicator ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;
}

