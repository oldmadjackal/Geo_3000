/********************************************************************/
/*								    */
/*      МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ДИАПАЗОНА ПАРАМЕТРОВ"                 */
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

#include "d:\_Projects\_Libraries\OLE_Excel.h"

#include "..\T_Path\T_Path.h"
#include "T_Scan.h"

#pragma warning(disable : 4996)


#define  _SECTION_FULL_NAME   "SCAN"
#define  _SECTION_SHRT_NAME   "SCAN"


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)
#define  SEND_INFO(text)     SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_INFO_MESSAGE,       \
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

     static   RSS_Module_Scan  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 T_SCAN_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_SCAN_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**                   ОПИСАНИЕ КЛАССА ЗАДАЧИ                       **/
/**                "АНАЛИЗ ДИАПАЗОНА ПАРАМЕТРОВ"                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Scan_instr  
         RSS_Module_Scan_InstrList[]={

 { "help",      "?",  "#HELP (?) - список доступных команд", 
                       NULL,
                      &RSS_Module_Scan::cHelp   },
 { "config",    "c",  "#CONFIG (C) - задание параметров оптимизация", 
                       NULL,
                      &RSS_Module_Scan::cConfig   },
 { "run",       "r",  "#RUN (R) -  запуск процесса сканирования параметров",
                       NULL,
                      &RSS_Module_Scan::cRun   },
 { "bases",     "b",  "#BASES (B) -  отобразить/перерисовать метки базовых точек",
                       NULL,
                      &RSS_Module_Scan::cBases   },
 { "clear",     "cl", "#CLEAR (CL) -  удалить метки базовых точек",
                       NULL,
                      &RSS_Module_Scan::cClear   },
 { "excel",     "e",  "#EXCEL (E) -  выгрузка результатов сканирования в MS Excel",
                       NULL,
                      &RSS_Module_Scan::cExcel  },
 {  NULL }
                                       } ;


/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Scan_instr *RSS_Module_Scan::mInstrList  =NULL ;
                            long  RSS_Module_Scan::mDebug_stamp=  0 ;


/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Scan::RSS_Module_Scan(void)

{
                keyword="Geo" ;
         identification="Scan_task" ;

             mInstrList=RSS_Module_Scan_InstrList ;

//       strcpy(mBasics_marker[0].model, "Point") ;
         strcpy(mBasics_marker[0].model, "Spot") ;
                mBasics_marker[0].color_min=RGB(0, 255,   0) ;
                mBasics_marker[0].color_max=RGB(0,   0, 255) ;
                mBasics_marker[0].size     = 50 ;
         strcpy(mBasics_marker[1].model, "Point") ;
                mBasics_marker[1].color_min=RGB(255, 0, 0) ;
                mBasics_marker[1].color_max=RGB(255, 0, 0) ;
                mBasics_marker[1].size     = 50 ;
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Module_Scan::~RSS_Module_Scan(void)

{
}


/********************************************************************/
/*								    */
/*		      Инициализация связей      		    */

  void  RSS_Module_Scan::vInit(void)

{
}


/********************************************************************/
/*								    */
/*		        Выполнить команду       		    */

  int  RSS_Module_Scan::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Scan::vExecuteCmd(const char *cmd, RSS_IFace *iface)

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Scan:") ;
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
       if(status)  SEND_ERROR("Секция SCAN: Неизвестная команда") ;
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

    void  RSS_Module_Scan::vShow(char *layer)

{
     int  status ;
     int  i ;
 
/*----------------------------------------- Масштабирование картинки */

  if(!stricmp(layer, "ZOOM")) {

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - Перебор контекстов отображения */

     for(i=0 ; i<this->mBasics_cnt ; i++)                           /* Перебор меток базовых точек */
                PlaceMarker(&this->mBasics[i]) ;

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - Перебор контекстов отображения */

                              }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*             Выполнить действие в контексте потока                 */

    void  RSS_Module_Scan::vChangeContext(void)

{
   if(!stricmp(mContextAction, "FORM" ))  FormMarker () ;
   if(!stricmp(mContextAction, "PLACE"))  PlaceMarker(mContextObject) ;
}


/********************************************************************/
/*								    */
/*		        Считать данные из строки		    */

    void  RSS_Module_Scan::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
                       char *work ;
                       char *value ;
                       char *end ;
                       char *tmp ;
                        int  n ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE SCAN\n", 
                      strlen("#BEGIN MODULE SCAN\n")) )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Scan::vReadSave", 0, 0) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Параметры модуля */

                work=buff+strlen("#BEGIN MODULE SCAN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
/*- - - - - - - - - - - - - - - - - -  Выделение и подготовка строки */
          end=strchr(work, '\n') ;
       if(end==NULL)  break ;
         *end=0 ;

          value=strchr(work, '=') ;
       if(value!=NULL) {  *value=0 ;
                           value++ ;  }
          
/*- - - - - - - - - - - - - - - - - - - -  Управляющие данные модуля */
       if(!stricmp(work, "CHECK_CMD"        ))  strncpy(mCheck_cmd, value, sizeof(mCheck_cmd)-1) ;
       if(!stricmp(work, "BASE_POINT_SHOW"  ))  mBasePoints_show  =strtol(value, &end, 10) ;
       if(!stricmp(work, "BASE_POINT_REDRAW"))  mBasePoints_redraw=strtol(value, &end, 10) ;
/*- - - - - - - - - - - - - - - - - - - - - - Параметры сканирования */
       if(!memicmp(work, "PAR", strlen("PAR"))) {

#define  P   this->mScanPars

            n=strtoul(work+strlen("PAR"), &tmp, 10) ;               /* Номер параметра */
         if(n>=this->mScanPars_cnt)  this->mScanPars_cnt=n+1 ;      /* Корректируем счетчик параметров */

            value=strstr(work, "NAME:") ;                           /* Имя параметра */
         if(value!=NULL) {
                           strncpy(P[n].name, value+strlen("NAME:"), sizeof(P[n].name)) ;
                        tmp=strchr(P[n].name, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "CATEGORY:") ;                       /* Категория параметра */
         if(value!=NULL)  P[n].category=strtoul(value+strlen("CATEGORY:"), &tmp, 10) ;

            value=strstr(work, "OBJECT:") ;                          /* Название управляемого объекта */
         if(value!=NULL) {
                           strncpy(P[n].object, value+strlen("OBJECT:"), sizeof(P[n].object)) ;
                        tmp=strchr(P[n].object, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "OBJECT_PAR:") ;                      /* Ссылка на параметр управляемого объекта */
         if(value!=NULL) {
                           strncpy(P[n].par_link, value+strlen("OBJECT_PAR:"), sizeof(P[n].par_link)) ;
                        tmp=strchr(P[n].par_link, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }


            value=strstr(work, "KIND:") ;                           /* Вид параметра */
         if(value!=NULL)  P[n].kind=strtoul(value+strlen("KIND:"), &tmp, 10) ;

            value=strstr(work, "VALUE:") ;                          /* Формула значения */
         if(value!=NULL) {
                           strncpy(P[n].value.formula, value+strlen("VALUE:"), sizeof(P[n].value.formula)) ;
                        tmp=strchr(P[n].value.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_MIN:") ;                      /* Формула минимума диапазона */
         if(value!=NULL) {
                           strncpy(P[n].value_min.formula, value+strlen("VALUE_MIN:"), sizeof(P[n].value_min.formula)) ;
                        tmp=strchr(P[n].value_min.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_MAX:") ;                      /* Формула максимума диапазона */
         if(value!=NULL) {
                           strncpy(P[n].value_max.formula, value+strlen("VALUE_MAX:"), sizeof(P[n].value_max.formula)) ;
                        tmp=strchr(P[n].value_max.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_STEP:") ;                     /* Формула шага диапазона */
         if(value!=NULL) {
                           strncpy(P[n].value_step.formula, value+strlen("VALUE_STEP:"), sizeof(P[n].value_step.formula)) ;
                        tmp=strchr(P[n].value_step.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

#undef   P

                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- Освобождение ресурсов */

                        gMemFree(buff) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Scan::vWriteSave(std::string *text)

{
  char  value[1024] ;
   int  i ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE SCAN\n" ;

/*------------------------------------------------ Базовые установки */

    sprintf(value, "CHECK_CMD=%s\n", mCheck_cmd) ;
     *text+=value ;
    sprintf(value, "BASE_POINT_SHOW=%d\n", mBasePoints_show) ;
     *text+=value ;
    sprintf(value, "BASE_POINT_REDRAW=%d\n", mBasePoints_redraw) ;
     *text+=value ;

/*----------------------------------- Список параметров сканирования */

   for(i=0 ; i<this->mScanPars_cnt ; i++) {

       sprintf(value, "PAR%02d NAME:%s CATEGORY:%d KIND:%d\n", 
                                 i, this->mScanPars[i].name, 
                                    this->mScanPars[i].category,
                                    this->mScanPars[i].kind     ) ;
        *text+=value ;

    if(this->mScanPars[i].category==_OBJECT_PAR) {
       sprintf(value, "PAR%02d OBJECT:%s OBJECT_PAR:%s\n", 
                                 i, this->mScanPars[i].object, 
                                    this->mScanPars[i].par_link ) ;
        *text+=value ;
                                                }

       sprintf(value, "PAR%02d VALUE:%s VALUE_MIN:%s VALUE_MAX:%s VALUE_STEP:%s\n", 
                                 i, this->mScanPars[i].value.formula, 
                                    this->mScanPars[i].value_min.formula,
                                    this->mScanPars[i].value_max.formula,
                                    this->mScanPars[i].value_step.formula ) ;
        *text+=value ;      
                                          }
/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Получить параметр       		    */

     int  RSS_Module_Scan::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- Описание модуля */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - Анализ оптимальности значений параметров методом перебора", "SCAN") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Scan::cHelp(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Scan_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CONFIG                  */

  int  RSS_Module_Scan::cConfig(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirectParam(GetModuleHandle(NULL),
			   (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
			     GetActiveWindow(), Task_Scan_Config_dialog, 
                              (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции RUN                     */

  int  RSS_Module_Scan::cRun(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

//         char *pars[_PARS_MAX] ;
//         char  check_cmd[1024] ;
      RSS_IFace  iface_result ;
 RSS_ControlPar  ctrl_par ;
      RSS_Point  b_point[10] ;
            int  status ;
         double  result ;
            int  result_init ;
            int  error_flag ;
           char *basics[32] ;
            int  basics_cnt ;
           char  text[1024] ;
           char *end ;
            int  cnt ;
            int  i_deep ;              /* Индекс "нижнего" сканируемого параметра */
            int  i ;
            int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------- Контроль управляющих параметров */

     if(mCheck_cmd[0]==0) {
         SEND_ERROR("Не задана команда определения оптимальности набора данных.") ;
                                 return(0) ;
                          }

     if(mScanPars_cnt==0) {
         SEND_ERROR("Не назначены сканируемые значения.") ;
                                 return(0) ;
                          }
/*---------------------------------------- Разборка командной строки */
/*------------------------------- Подготовка параметров сканирования */

     for(i=0 ; i<mScanPars_cnt ; i++) {
                                mScanPars[i].value     .fixed= 0 ;
                                mScanPars[i].value_min .fixed= 0 ;
                                mScanPars[i].value_max .fixed= 0 ;
                                mScanPars[i].value_step.fixed= 0 ;
                                mScanPars[i].step            =-1 ;
                                      }
/*-------------------------- Подготовка массива результирующих точек */

         if(mResult     !=NULL)  free(mResult     ) ;
         if(mResult_pars!=NULL)  free(mResult_pars) ;

                                      mResult     =NULL ;
                                      mResult_pars=NULL ;
                                      mResult_cnt =  0 ;
                                      mResult_max =  0 ;

/*------------------------------------- Очистка списка базовых точек */

   if(mBasics_cnt) {

     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].dlist_idx) {
           RSS_Kernel::display.ReleaseList(mBasics[i].dlist_idx) ;
                                           mBasics[i].dlist_idx=0 ;
                                }

                           free(mBasics) ;
                                mBasics    =NULL ;
                                mBasics_cnt=  0 ;

                          this->kernel->vShow("REFRESH") ;          /* Перерисовка сцены */
                   } 
/*------------------------------------------ Собственно сканирование */

               result_init=0 ;
                    i_deep=0 ;

     do {                                                           /* CIRCLE.0 */
                mDebug_stamp++ ;
             if(mDebug_stamp==101) {
                                        mDebug_stamp+=0 ;
                                   }

/*----------------------------------- Перебор значений по параметрам */

         for(i=i_deep ; i<mScanPars_cnt ; i++)
           if(mScanPars[i].kind==_SLAVE_PAR) {                      /* Если расчетный параметр... */
                   status=iCalcSlaveValue(&mScanPars[i]) ;
                if(status==-1)  break ;
                                             }
           else                              {                      /* Если диапазонный параметр... */
                   i_deep= i ;                                      /*  Фиксируем его номер */
                   status=iCalcScanValue(&mScanPars[i]) ;           /*  Определяем очередное значение в диапазоне */
                if(status==-1)  break ;
                if(status== 0)  continue ;

                                         mScanPars[i].step=-1 ;     /*  Иниц.параметр */

              for(i-- ; i>=0 ; i--)                                 /*  Ищем ближайший "верхний" */
                if(mScanPars[i].kind==_SCAN_PAR)  break ;           /*    диапазонный параметр   */ 

                if(i<0)  break ;                                    /*  Если все "диапазоны" отработаны... */

                      i-- ;                                         /*  Даем коррекцию для попадания на нужный "диапазон" */
                                             }

                if(i<0)  break ;                                    /*  Если все "диапазоны" отработаны... */

                if(status==-1)  break ;                             /* Если ошибка при работе с параметрами... */

/*-------------------------------- "Раздача" по управляемым объектам */

         for(i=0 ; i<mScanPars_cnt ; i++)
           if(mScanPars[i].category==_OBJECT_PAR) {
            for(j=0 ; j<OBJECTS_CNT ; j++)
              if(!stricmp(OBJECTS[j]->Name, mScanPars[i].object)) {
                                      strcpy( ctrl_par.link, mScanPars[i].par_link) ;
                                      strcpy( ctrl_par.type, "double") ;
                                              ctrl_par.value=&mScanPars[i].value.value ;
                  OBJECTS[j]->vSetControlPar(&ctrl_par) ;
                                                break ;
                                                                  }
                                                  }
/*--------------------------------------------- Получение результата */

//        SEND_ERROR("Секция SCAN: Иттерация начинается") ;

                                          iface_result.vClear() ;
              iExecute(this->mCheck_cmd, &iface_result) ;

//        SEND_ERROR("Секция SCAN: Иттерация завершена") ;

         if(iface_result.vCheck("FATAL"))  break ;

         if(iface_result.vCheck("ERROR"))  error_flag= 1 ;
         else                              error_flag= 0 ;

               iface_result.vDecode(NULL, text) ;
                      result=strtod(text, &end) ;

/*----------------------------------- Коррекция диапазона результата */

   if(!error_flag) {

        if(result_init==0) {
                                    result_init= 1 ;
                              mResult_value_min=result ;
                              mResult_value_max=result ;
                           }
        else               {
              if(mResult_value_min>result)  mResult_value_min=result ;
              if(mResult_value_max<result)  mResult_value_max=result ;
                           }
                   }
/*---------------------------------------- Отображение базовых точек */

   if(mBasePoints_show) {
/*- - - - - - - - - - - - - - - - - - Определение "базовых" объектов */
                             basics_cnt=0 ;

     for(i=0 ; i<mScanPars_cnt ; i++)                               /* Среди параметров сканирования выявляем */
        if(mScanPars[i].category==_OBJECT_PAR)                      /*  базовые точки объектов и составляем   */
         if(!stricmp(mScanPars[i].par_link+                         /*   список таких объектов                */
                      strlen(mScanPars[i].par_link)-5, "_base")) {

          for(j=0 ; j<basics_cnt ; j++)
            if(!stricmp(basics[j], mScanPars[i].object))  break ;

            if(j<basics_cnt) continue ;

                     basics[basics_cnt]=mScanPars[i].object ;
                            basics_cnt++ ;
                                                                 }
/*- - - - - - - - - - - - - - - -  Формирование списка базовых точек */
     for(i=0 ; i<basics_cnt ; i++) {                                /* LOOP - Перебираем "параметризованные" базовые точки объектов */

        for(j=0 ; j<OBJECTS_CNT ; j++)                              /* Идентифицируем объект */
          if(!stricmp(OBJECTS[j]->Name, basics[i])) break ;
                
          if(j>=OBJECTS_CNT)  continue ;

                cnt=OBJECTS[j]->vGetBasePoint(b_point) ;            /* Извлекаем положение базовых точек объекта */

        for(cnt-- ; cnt>=0 ; cnt--) {                               /* LOOP - Перебераем базовые точки объекта */

         for(j=0 ; j<mBasics_cnt ; j++)                             /* Проверяем совпадение с одной         */
           if(mBasics[j].point.x   ==b_point[cnt].x   &&            /*  из зарегистрированных базовых точек */ 
              mBasics[j].point.y   ==b_point[cnt].y   &&
              mBasics[j].point.z   ==b_point[cnt].z     )  break ;

          if(j==mBasics_cnt) {                                       /* Если такой точки нет... */
             mBasics=(RSS_Module_Scan_basicp *)                      /*  Регистрируем ее */ 
                       realloc(mBasics, (mBasics_cnt+1)*sizeof(*mBasics)) ;

                mBasics[mBasics_cnt].point    =b_point[cnt] ;
                mBasics[mBasics_cnt].good     = 0 ;
                mBasics[mBasics_cnt].error    = 0 ;
                mBasics[mBasics_cnt].draw     = 1 ;
                mBasics[mBasics_cnt].dlist_idx= 0 ;
                        mBasics_cnt++ ;
                            }

            if(error_flag) {                                         /* Корректируем описание базовой точки   */
                              mBasics[j].error=1 ;                   /*  в соотвествии с результатами анализа */
                           }
            else           {
                    if(mBasics[j].good==0) {  mBasics[j].result_min=result ;
                                              mBasics[j].result_max=result ;  }
                   
                    if(mBasics[j].result_min>result)  mBasics[j].result_min=result ;
                    if(mBasics[j].result_max<result)  mBasics[j].result_max=result ;

                              mBasics[j].good=1 ;
                           } 

                              mBasics[j].draw=1 ;

                                    }                               /* ENDLOOP - Перебераем базовые точки объекта */

                                   }                                /* ENDLOOP - Перебираем "параметризованные" базовые точки объектов */
/*- - - - - - - - - - - - - - - - - - - - - -  Отрисовка новых точек */
     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].draw) {
                                      mBasics[i].draw=0 ;

                                     FormMarker_() ;                /* Формирование маркера */
                                    PlaceMarker_(&mBasics[i]) ;     /* Формирование позиционирующего заголовка маркера */
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        } 
/*-------------------------------------------- Сохранение результата */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Выделение памяти */
   if(mResult_cnt==mResult_max) {
                                        mResult_max+=100 ;

         mResult=(RSS_Module_Scan_result *)
                   realloc(mResult, mResult_max*sizeof(*mResult)) ;
      if(mResult==NULL) {
          SEND_ERROR("Секция SCAN: Недостаточно памяти для сохранения результата.") ;
                           break ;
                        }

         mResult_pars=(p_value *)
                   realloc(mResult_pars, mResult_max*mScanPars_cnt*sizeof(p_value)) ;
      if(mResult_pars==NULL) {
          SEND_ERROR("Секция SCAN: Недостаточно памяти для сохранения результата.") ;
                                break ;
                             }

        for(i=0 ; i<mResult_cnt ; i++)                              /* Перересчет ссылок */
              mResult[i].pars=mResult_pars+i*mScanPars_cnt ;

                                }
/*- - - - - - - - - - - - - - - - - - - - - -  Сохранение результата */
           mResult[mResult_cnt].result    =result ;
           mResult[mResult_cnt].error_flag=error_flag ;
           mResult[mResult_cnt].pars      =mResult_pars+mResult_cnt*mScanPars_cnt ;

        for(i=0 ; i<mScanPars_cnt ; i++)
          iCalcGetValue(&mScanPars[i], &mResult[mResult_cnt].pars[i]) ;

                   mResult_cnt++ ;

              iSaveBinResult(NULL, NULL, mResult_cnt-1) ;           /* Сохраняем промежуточный результата в файл */

/*------------------------------------------ Собственно сканирование */

   if(mBasePoints_redraw)
       for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;  /* Перерисовка маркеров базовых точек */

        } while(1) ;                                                /* CONTINUE.0 */

/*---------------------------------------------- Фиксация результата */

   if(mBasePoints_show)
       for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;  /* Перерисовка маркеров базовых точек */

         this->kernel->vShow("REFRESH") ;                           /* Отображение сцены */

              iSaveBinResult("Generate", NULL, 0) ;                 /* Сохранение результата в файл */

          SEND_INFO("Секция SCAN: перебор параметров завершен.") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BASES                   */

  int  RSS_Module_Scan::cBases(char *cmd, RSS_IFace *iface)

{ 
  int  i ;

                                     FormMarker_ () ;               /* Формирование маркера */
     for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;    /* Перерисовка маркеров базовых точек */

              this->kernel->vShow("REFRESH") ;                      /* Отображение сцены */

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CLEAR                   */

  int  RSS_Module_Scan::cClear(char *cmd, RSS_IFace *iface)

{ 
  int  i ;

     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].dlist_idx) {
           RSS_Kernel::display.ReleaseList(mBasics[i].dlist_idx) ;
                                           mBasics[i].dlist_idx=0 ;
                                }

              this->kernel->vShow("REFRESH") ;                      /* Отображение сцены */

   return(0) ;
}


/********************************************************************/
/*								    */
/*                  Реализация инструкции EXCEL                     */

  int  RSS_Module_Scan::cExcel(char *cmd, RSS_IFace *iface)

{ 
   char  path[FILENAME_MAX] ;

/*------------------------------------------- Формирование CSV-файла */

                     sprintf(path, "Generate") ;                    /* Задаем авто-генерацию пути файла */
              iSaveCsvResult(path) ;                                /* Сохранение результата в CSV-файл */

/*------------------------------------------------- Загрузка в Excel */

            iFormExcelResult(path) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Module_Scan::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         


    if(path==NULL) {
                             path="t_scan.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}


/********************************************************************/
/*								    */
/*	               Выполнение команды                           */

  void  RSS_Module_Scan::iExecute(char *execute, RSS_IFace *result)

{
   char  command[1024] ;
    int  status ;
   char  text[1024] ;
    int  i ;

/*------------------------------------------------------- Подготовка */

                            strcpy(command, execute) ;

         if(result!=NULL)   result->vClear() ; 

/*------------------------------ Опрос командных процессоров модулей */

    for(i=0 ; i<this->kernel->modules_cnt ; i++) {
      
      if(result!=NULL)  status=this->kernel->modules[i].entry->vExecuteCmd(command, result) ;
      else              status=this->kernel->modules[i].entry->vExecuteCmd(command,  NULL ) ;

      if(status==-1)  break ;
      if(status== 0)  break ;
                                                 }
/*---------------------------------- Обработка результата исполнения */

   if( result!=NULL         &&                                      /* Проверяем наличие какого-либо сигнала */
      !result->vCheck(NULL)   ) {

                result->vSignal("FATAL", "No iface") ;

     if(i<this->kernel->modules_cnt)
                        sprintf( text, "Обработчик команды не поддерживает интерфейс внутренних связей:\n"
                                       "  %s", command) ;
     else               sprintf( text, "Обработчик для команды не найден:\n"
                                       "  %s", command) ;
                     SEND_ERROR( text) ;
                             return ;
                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*	  Определение значения для "диапазонного" параметра         */

   int  RSS_Module_Scan::iCalcScanValue(RSS_Module_Scan_par *par)

{
    int  status ;
   char  text[1024] ; 

/*---------------------------------------------------- Инициализация */

   if(par->step==-1) {

              status=iCalcValue(&par->value_min) ;                  /* Опр.начальное значение диапазона */
           if(status) {                                             /* Если ошибка... */
                 sprintf(text, "Ошибка! Параметр %s (начальное значение) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

                          par->value.value=par->value_min.value ;   /* Присваеваем текущее значение */
                          par->step       = 0 ;                     /* Иниц.счетчик иттераций */

                             return(0) ;
                     }
/*--------------------------------------------- Выполнение иттерации */

              status=iCalcValue(&par->value_max) ;                  /* Опр.конечное значение диапазона */
           if(status) {                                             /* Если ошибка... */
                 sprintf(text, "Ошибка! Параметр %s (конечное значение) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

              status=iCalcValue(&par->value_step) ;                 /* Опр.шаг изменения значения */
           if(status) {                                             /* Если ошибка... */
                 sprintf(text, "Ошибка! Параметр %s (шаг изменения) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

           if(par->value_step.value<=0) {
                 sprintf(text, "Ошибка! Параметр %s (шаг изменения) : некорректное значение %lf", 
                                       par->name, par->value_step.value) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                                        } 

           if(    par->value.value+                                 /* Если диапазон пройден весь... */
              0.5*par->value_step.value>=
                  par->value_max.value   )  return(1) ;

                 par->value.value+=par->value_step.value ;          /* Опр. очередное значение */

           if(par->value.value>par->value_max.value)                /* При выходе за границу диапазона - */
                 par->value.value=par->value_max.value ;            /*  - подравниваем под нее           */

                 par->step++ ;                                      /* Модифицируем счетчик иттераций */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	  Определение значения для расчетного параметра             */

   int  RSS_Module_Scan::iCalcSlaveValue(RSS_Module_Scan_par *par)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*	      Выдача значения параметра для сохранения              */

   int  RSS_Module_Scan::iCalcGetValue(RSS_Module_Scan_par *par, 
                                                   p_value *data)

{
     data->f_value=par->value.value ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      Выдача значения параметра в текстовом виде            */

   int  RSS_Module_Scan::iCalcTextValue(RSS_Module_Scan_par *par, 
                                                    p_value *data, 
                                                       char *text )

{
     sprintf(text, "%.10f", data->f_value) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                  Определение значения параметра                  */

   int  RSS_Module_Scan::iCalcValue(RSS_Module_Scan_value *value)

{
   char *formula ;
   char *end ;
    int  i ;
    int  j ;

/*------------------------------------------------- Входной контроль */

    if(value->fixed)  return(0) ;
   
/*----------------------------------------------- Подготовка формулы */

                    formula=value->formula ;

     for(i=0, j=0 ; formula[i] ; i++)                               /* Убираем из формулы все пробелы */
       if(formula[i]!=' '  &&                                       /*   и табуляции                  */
          formula[i]!='\t'   ) {  formula[j]=formula[i] ;
                                          j++ ;            }

                                  formula[j]= 0 ;                   /* Терминируем "поджатую" формулу */

       if(j==0) {
           strcpy(mError, "Отсутствует формула расчета значения") ;
                            return(0) ;
                }
/*------------------------------- Проверка на фиксированное значение */

             value->value=strtod(formula, &end) ;                   /* Обраб.формулу как простое число */

         if(*end==0) {   value->fixed=1 ;                           /* Если удачно - то ставим признак */
                            return(0) ;    }                        /*  фиксированного числа и выходим */

/*-------------------------------------------- Вычисление по формуле */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      Сохранение технического файла результата              */

   int  RSS_Module_Scan::iSaveBinResult(char *path, char *title, int  silence)

{
       time_t  time_abs ;
    struct tm *hhmmss ;
         char  path_tmp[512] ;
         char  title_tmp[512] ;
         FILE *file ;
         char  value[128] ;
         char  text[1024] ;
          int  i ;
          int  j ;

/*------------------------------------------------------- Подготовка */

                time_abs=     time( NULL) ;
                  hhmmss=localtime(&time_abs) ;
        
/*------------------------------ Определение имени файла и заголовка */

   if(title==NULL) {                                                /* Если заголовок не задан... */
                          title=title_tmp ;
                  sprintf(title, "Технический файл результатов поиска %02d:%02d:%02d %02d_%02d_%02d",
                                    hhmmss->tm_hour, hhmmss->tm_min,  hhmmss->tm_sec,
                                    hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year-100 ) ;              
                   }

   if(path ==NULL) {                                                /* Если путь не задан... */
                          path=path_tmp ;
                  sprintf(path, "Temp.scan", time_abs) ;
                   }
   else
   if(!stricmp(path, "Generate")) {                                 /* Если авто-генерация имени... */
                          path=path_tmp ;
                  sprintf(path, "Temp_%lx.scan", time_abs) ;
                                  }
/*--------------------------------------------------- Открытие файла */

      file=fopen(path, "wt") ;
   if(file==NULL) {
                        sprintf(text, "Секция SCAN: Ошибка создания файла %s",
                                         path) ;
       if(!silence)  SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*--------------------------------------- Сохранение настроек модуля */

/*------------------------------- Сохранение спецификации результата */

   sprintf(text, "Title:%s\n", title) ;
    fwrite(text, 1, strlen(text), file) ;
   sprintf(text, "Points:%d\n", this->mResult_cnt) ;
    fwrite(text, 1, strlen(text), file) ;
   sprintf(text, "Parameters:%d\n", this->mScanPars_cnt) ;
    fwrite(text, 1, strlen(text), file) ;

/*------------------------------------- Сохранение данных результата */

      for(i=0 ; i<this->mResult_cnt ; i++) {                        /* Сохраняем массив точек результата построчно */

             sprintf(text, "%d;%.10lf", this->mResult[i].error_flag, 
                                        this->mResult[i].result     ) ;

       for(j=0 ; j<this->mScanPars_cnt ; j++) {
          iCalcTextValue(&mScanPars[j], &mResult[i].pars[j], value) ;
                  strcat( text, ";") ;
                  strcat( text, value) ;
                                              }

              strcat(text, "\n") ;
              fwrite(text, 1, strlen(text), file) ;

                                           }
/*--------------------------------------------------- Закрытие файла */

                 fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	          Сохранение CSV-файла результата                   */

   int  RSS_Module_Scan::iSaveCsvResult(char *path)

{
       time_t  time_abs ;
         char  folder[512] ;
         char  path_tmp[512] ;
         FILE *file ;
         char  value[128] ;
         char  text[1024] ;
          int  i ;
          int  j ;

/*------------------------------------------------------- Подготовка */

                 time_abs=time(NULL) ;

                       _getcwd(folder, sizeof(folder)-1) ;
        
/*------------------------------ Определение имени файла и заголовка */

   if(path ==NULL               ) {                                 /* Если путь не задан... */
                          path=path_tmp ;
                  sprintf(path, "Temp.scan", time_abs) ;
                                  }
   else
   if(!stricmp(path, "Generate")) {                                   /* Если  авто-генерация имени... */
                  sprintf(path, "%s/Temp_%lx.txt", folder, time_abs) ;
                                  }
/*--------------------------------------------------- Открытие файла */

      file=fopen(path, "wt") ;
   if(file==NULL) {
                        sprintf(text, "Секция SCAN: Ошибка создания файла %s",
                                         path) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*------------------------------------- Сохранение данных результата */

      for(i=0 ; i<this->mResult_cnt ; i++) {                        /* Сохраняем массив точек результата построчно */

             sprintf(text, "%d\t%.10lf", this->mResult[i].error_flag, 
                                         this->mResult[i].result     ) ;

       for(j=0 ; j<this->mScanPars_cnt ; j++) {
          iCalcTextValue(&mScanPars[j], &mResult[i].pars[j], value) ;
                  strcat( text, "\t") ;
                  strcat( text, value) ;
                                              }

              strcat(text, "\n") ;
              fwrite(text, 1, strlen(text), file) ;

                                           }
/*--------------------------------------------------- Закрытие файла */

                 fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	          Загрузка результата в MS Excel                    */

   int  RSS_Module_Scan::iFormExcelResult(char *path)

{
  OLE_Excel  Excel ;
       char  macro[32] ;
       char  format[1024] ;
        int  status ;
       char  tmp[2048] ;
        int  i ;

/*-------------------------------------------- Присоединение к Excel */

         Excel.Activate() ;
         Excel.OpenNew() ;

/*--------------------------------------- Удаление начальных страниц */

         Excel.CreateMacro("Sub Initial_Pages_delete()\n"
                           "Application.DisplayAlerts = False\n"
                           "Sheets(3).Delete\n"
                           "Sheets(2).Delete\n"
                           "Sheets(1).Name=\"Geo2D_Scan\"\n"
                           "End Sub\n") ;

         Excel.ExecuteMacro("Initial_Pages_delete") ;
         Excel.ShowLastError() ;

/*---------------------------- Формирование загрузочной спецификации */

                                           strcpy(format, "1") ;
   for(i=0 ; i<this->mScanPars_cnt ; i++)  strcat(format, ",1") ;

/*-------------------------------------- Загрузка данных на страницы */

                    sprintf(macro, "Data_load") ;
                    sprintf(tmp, 
"Sub %s\n"
"Sheets(\"%s\").Select\n"
"With ActiveSheet.QueryTables.Add(Connection:=\"TEXT;%s\", Destination:=Range(\"A1\"))\n"
".Name = \"%s\"\n"
".FieldNames = True\n"
".RowNumbers = False\n"
".FillAdjacentFormulas = False\n"
".PreserveFormatting = True\n"
".RefreshOnFileOpen = False\n"
".RefreshStyle = xlInsertDeleteCells\n"
".SavePassword = False\n"
".SaveData = True\n"
".AdjustColumnWidth = True\n"
".RefreshPeriod = 0\n"
".TextFilePromptOnRefresh = False\n"
//".TextFilePlatform = 1251\n"               // Не поддерживается MS Office 2000
".TextFileStartRow = 1\n"
".TextFileParseType = xlDelimited\n"
".TextFileTextQualifier = xlTextQualifierDoubleQuote\n"
".TextFileConsecutiveDelimiter = False\n"
".TextFileTabDelimiter = True\n"
".TextFileSemicolonDelimiter = False\n"
".TextFileCommaDelimiter = False\n"
".TextFileSpaceDelimiter = False\n"
".TextFileColumnDataTypes = Array(%s)\n"
//".TextFileTrailingMinusNumbers = False\n"  // Не поддерживается MS Office 2000
".TextFileDecimalSeparator = \"%c\"\n"
//".TextFileThousandsSeparator = \"%c\"\n"
".Refresh BackgroundQuery:=False\n"
"End With\n"
"End Sub\n",  
              macro, "Geo2D_Scan", path, "Geo2D_Scan", 
             format, '.'
) ;

         status=Excel.CreateMacro (tmp) ;
         status=Excel.ExecuteMacro(macro) ;
                Excel.ShowLastError() ;

/*-------------------------------------------- Отсоединение от Excel */

                Excel.Visible(1) ;
                Excel.Exit() ;

/*------------------------------------------- Удаление файлов данных */

                  unlink(path) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           Задание формы маркера с передачей контекста            */

  int  RSS_Module_Scan::FormMarker_(void)

{
    strcpy(mContextAction, "FORM") ;
           mContextObject=NULL ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Задание формы маркера                         */

  int  RSS_Module_Scan::FormMarker(void)

{
  int  status ;
  int  i ;


/*------------------------------------------ Перебор списка маркеров */

    for(i=0 ; i<_MARKER_MAX ; i++) {                                /* LOOP - Перебор списка маркеров */

#define  M  (this->mBasics_marker[i])

/*-------------------------------- Резервирование дисплейного списка */

     if(M.dlist_idx==0)
        M.dlist_idx=RSS_Kernel::display.GetList(1) ;

     if(M.dlist_idx==0)  continue ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - Перебор контекстов отображения */

/*--------------------------------------- Формирование метки маркера */

             glNewList(M.dlist_idx, GL_COMPILE) ;                   /* Открытие группы */
          glMatrixMode(GL_MODELVIEW) ;
//            glEnable(GL_BLEND) ;                                  /* Вкл.смешивание цветов */
//         glDepthMask(0) ;                                         /* Откл.запись Z-буфера */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Точка */
    if(!stricmp(M.model, "Point")) {

                       glBegin(GL_POINTS) ;
                    glVertex3d( 0,  0,  0) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  "Пятно" */
    if(!stricmp(M.model, "Spot" )) {

 double  h ;

                               h=1.0 ;

                       glBegin(GL_POLYGON) ;
                    glVertex3d( 0, -h, -h) ;
                    glVertex3d( 0, -h,  h) ;
                    glVertex3d( 0,  h,  h) ;
                    glVertex3d( 0,  h, -h) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d(-h,  0, -h) ;
                    glVertex3d(-h,  0,  h) ;
                    glVertex3d( h,  0,  h) ;
                    glVertex3d( h,  0, -h) ;
	                 glEnd();
                       glBegin(GL_POLYGON) ;
                    glVertex3d(-h, -h,  0) ;
                    glVertex3d(-h,  h,  0) ;
                    glVertex3d( h,  h,  0) ;
                    glVertex3d( h, -h,  0) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//         glDepthMask(1) ;                                         /* Вкл.запись Z-буфера */
//           glDisable(GL_BLEND) ;                                  /* Откл.смешивание цветов */
             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP - Перебор контекстов отображения */
/*------------------------------------------ Перебор списка маркеров */

#undef M

                                   }                                /* ENDLOOP - Перебор списка маркеров */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           Позиционирование маркера с передачей контекста         */

  int  RSS_Module_Scan::PlaceMarker_(RSS_Module_Scan_basicp *marker)

{
    strcpy(mContextAction, "PLACE") ;
           mContextObject=marker ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Позиционирование маркера                      */


  int  RSS_Module_Scan::PlaceMarker(RSS_Module_Scan_basicp *marker)

{
    double  zoom ;  
       int  status ;
       int  form_idx ;
    double  result ;
    double  R_color ;
    double  G_color ;
    double  B_color ;

#define   M   marker
#define   F   this->mBasics_marker[form_idx]

/*------------------------------------------------------- Подготовка */

             form_idx=!M->good ;

               result=M->result_min ;

          R_color=iColorMix(GetRValue(F.color_min), 
                            GetRValue(F.color_max), result) ;
          G_color=iColorMix(GetGValue(F.color_min), 
                            GetGValue(F.color_max), result) ;
          B_color=iColorMix(GetBValue(F.color_min), 
                            GetBValue(F.color_max), result) ;

/*-------------------------------- Резервирование дисплейного списка */

     if(M->dlist_idx==0)
           M->dlist_idx=RSS_Kernel::display.GetList(0) ;

     if(M->dlist_idx==0)  return(-1) ;

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*------------------ Формирование позиционирующей последовательности */

             glNewList(M->dlist_idx, GL_COMPILE) ;                  /* Открытие группы */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(M->point.x, M->point.y, M->point.z) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/F.size ;
              glScaled(zoom, zoom, zoom) ;

             glColor4d(R_color/256., G_color/256.,                  /* Установка цвета */ 
                                     B_color/256., 0.2) ;
            glCallList(F.dlist_idx) ;                               /* Отрисовка маркера */

             glEndList() ;                                          /* Закрытие группы */

/*----------------------------------- Перебор контекстов отображения */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   M
#undef   F

  return(0) ;
}

  double  RSS_Module_Scan::iColorMix(double  color_min, 
                                     double  color_max, 
                                     double  value     )
{

   if(this->mResult_value_min==
      this->mResult_value_max  )  return(color_max) ;
 
   return(  color_min+(color_max-color_min)*
             fabs(this->mResult_value_min-value)/
             fabs(this->mResult_value_max-this->mResult_value_min) ) ;

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

   RSS_Kernel *GetTaskScanEntry(void)

{
	return(&ProgramModule) ;
}