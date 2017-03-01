/********************************************************************/
/*								    */
/*                  МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ        		    */
/*                   "ЛЕТАЮЩАЯ" СТРУКТУРА ТЕЛ                       */
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
#include <math.h>

#include "d:\_Projects\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"
#include "..\RSS_Model\RSS_Model.h"

#include "O_Flyer.h"

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

     static   RSS_Module_Flyer  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	Идентификационный вход                      */

 O_FLYER_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_FLYER_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**          ОПИСАНИЕ КЛАССА МОДУЛЯ УПРАВЛЕНИЯ ОБЪЕКТОМ            **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            Список команд                         */

  struct RSS_Module_Flyer_instr  RSS_Module_Flyer_InstrList[]={

 { "help",       "?",  "#HELP (?)  - список доступных команд", 
                        NULL,
                       &RSS_Module_Flyer::cHelp   },
 { "create",     "cr", "#CREATE (CR) - создать объект",
                       " CREATE {<Имя>\n"
                       "   Создает именованный обьект",
                       &RSS_Module_Flyer::cCreate },
 { "info",       "i",  "#INFO (I) - выдать информацию по объекту",
                       " INFO <Имя> \n"
                       "   Выдать основную нформацию по объекту в главное окно\n"
                       " INFO/ <Имя> \n"
                       "   Выдать полную информацию по объекту в отдельное окно",
                       &RSS_Module_Flyer::cInfo },
 { "base",       "b", "#BASE (B) - задать базовую точку объекта",
                       " BASE <Имя> <x> <y> <z>\n"
                       "   Задает базовую точку объекта\n"
                       " BASE/x <Имя> <x>\n"
                       "   Задает координату X базовой точки объекта\n"
                       "       (аналогично для Y и Z)\n"
                       " BASE/+x <Имя> <x>\n"
                       "   Задает приращение координаты X базовой точки объекта\n"
                       "       (аналогично для Y и Z)\n"
                       " BASE> <Имя>\n"
                       "   Задает клавиатурное управление базовой точкой объекта\n",
                       &RSS_Module_Flyer::cBase },
 { "angle",      "a", "#ANGLE (A) - задать углы ориентации объекта",
                       "           A (AZIMUTH)   - азимут\n"
                       "           E (ELEVATION) - возвышение\n"
                       " ANGLE <Имя> <a> <e>\n"
                       "   Задает углы ориентации объекта\n"
                       " ANGLE/a <Имя> <a>\n"
                       "   Задает угол ориентации A объекта\n"
                       "       (аналогично для E)\n"
                       " ANGLE/+a <Имя> <a>\n"
                       "   Задает приращение угла ориентации A объекта\n"
                       "       (аналогично для E)\n"
                       " ANGLE> <Имя>\n"
                       "   Задает клавиатурное управление углами ориентации объекта\n",
                       &RSS_Module_Flyer::cAngle },
 { "degree",     "d", "#DEGREE (D) - таблица управления степенями подвижности",
                       " DEGREE <Имя> \n",
                       &RSS_Module_Flyer::cDegree },
 { "link",       "l", "#LINK (L) - задание длины звена",
                       " LINK  <Имя объекта> <Имя звена> <Величина>\n" 
                       "             Задает длину звена\n" 
                       " LINK+ <Имя объекта> <Имя звена> <Величина>\n" 
                       "             Задает приращение длины звена\n",
                       &RSS_Module_Flyer::cLink },
 { "joint",      "j", "#JOINT (J) - задание угла в шарнире звена",
                       " JOINT  <Имя объекта> <Имя звена> <Величина>\n" 
                       "             Задает угла в шарнире звена\n" 
                       " JOINT+ <Имя объекта> <Имя звена> <Величина>\n" 
                       "             Задает приращение угла в шарнире звена\n",
                       &RSS_Module_Flyer::cJoint },
 { "range",      "r", "#RANGE (R) - задание диапазонов изменения степеней подвижности",
                       " RANGE <Имя объекта>\n" 
                       "             Вызов диалогового окна задания диапазонов\n" 
                       " RANGE <Имя объекта> <Имя звена> <Флаг проверки> <Минимум> <Максимум>\n"
                       "             Задает диапазон для величины длины/угла в шарнире\n",
                       &RSS_Module_Flyer::cRange },
 { "keys",       "k", "#KEYS (K) - назначение управляющих клавиш",
                       " KEYS <Имя объекта> \n",
                       &RSS_Module_Flyer::cKeys },
 { "body",       "bo", "#BODY (BO) - назначение тел на звенья",
                       " BODY <Имя> \n",
                       &RSS_Module_Flyer::cBody },
 { "bodyadd",    "ba", "#BODYADD (BA) - добавить тело",
                       " BODYADD <Имя объекта> <Имя звена> <Имя тела>\n",
                       &RSS_Module_Flyer::cBodyAdd },
 { "bodymodel",  "bm", "#BODYMODEL (BM) - назначение модели тела",
                       " BODYMODEL <Имя объекта> <Имя звена> <Имя тела> <Файл модели> <Параметры>\n",
                       &RSS_Module_Flyer::cBodyModel },
 { "bodyshift",  "bs", "#BODYSHIFT (BS) - положение тела относительно скелета звена",
                       " BODYSHIFT <Имя объекта> <Имя звена> <Имя тела> <Параметры сдвига>\n",
                       &RSS_Module_Flyer::cBodyShift },
 { "bodyzero",   "bz", "#BODYZERO (BZ) - 'нулевая' длина тела",
                       " BODYZERO <Имя объекта> <Имя звена> <Имя тела> <Нулевая длина>\n",
                       &RSS_Module_Flyer::cBodyZero },
 { "mass",       "m",  "#MASS (M) - назначение масс на звенья",
                       " MASS <Имя> \n",
                       &RSS_Module_Flyer::cMass },
 { "massadd",    "ma", "#MASSADD (MA) - добавить массу",
                       " MASSADD <Имя объекта> <Имя звена> <Имя массы> <Масса> <Главные моменты инерции>\n",
                       &RSS_Module_Flyer::cMassAdd },
 { "massshift",  "ms", "#MASSSHIFT (MS) - положение массы относительно скелета звена",
                       " MASSSHIFT <Имя объекта> <Имя звена> <Имя массы> <Параметры сдвига>\n",
                       &RSS_Module_Flyer::cMassShift },
 { "massmove",   "mm", "#MASSMOVE (MZ) - коэффициент сдвига массы",
                       " MASSMOVE <Имя объекта> <Имя звена> <Имя массы> <Коэффициент сдвига>\n",
                       &RSS_Module_Flyer::cMassMove },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     Общие члены класса             		    */

    struct RSS_Module_Flyer_instr *RSS_Module_Flyer::mInstrList=NULL ;

/********************************************************************/
/*								    */
/*		       Конструктор класса			    */

     RSS_Module_Flyer::RSS_Module_Flyer(void)

{
	   keyword="Geo" ;
    identification="Robot_Flyer_object" ;

        mInstrList=RSS_Module_Flyer_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        Деструктор класса			     */

    RSS_Module_Flyer::~RSS_Module_Flyer(void)

{
}


/*********************************************************************/
/* 								     */
/*		        Стартовая разводка                           */

    void  RSS_Module_Flyer::vStart(void)

{
}


/*********************************************************************/
/*								     */
/*		        Выполнить команду       		     */

  int  RSS_Module_Flyer::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* Флаг режима прямой команды */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "FLYER"
#define  _SECTION_SHRT_NAME   "FL"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Flyer:") ;
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
       if(status)  SEND_ERROR("Секция FLYER: Неизвестная команда") ;
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

    void  RSS_Module_Flyer::vReadSave(std::string *data)

{
                         char *buff ;
                          int  buff_size ;
             RSS_Object_Flyer *object ;
        RSS_Object_Flyer_Link *link ; 
        RSS_Object_Flyer_Body  body_tmp ; 
        RSS_Object_Flyer_Mass  mass_tmp ; 
 RSS_Module_Flyer_Create_data  create_data ;
                  std::string  text ;
                         char *name ;
                          int  n_link ;
                          int  n_link_prv ;
                          int  n_body ;
                          int  n_body_prv ;
                          int  status ;
                         char *entry ;
                         char *value ;
                         char *end ;
                          int  i ;

/*----------------------------------------------- Контроль заголовка */

   if(memicmp(data->c_str(), "#BEGIN MODULE FLYER\n", 
                      strlen("#BEGIN MODULE FLYER\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT FLYER\n", 
                      strlen("#BEGIN OBJECT FLYER\n"))   )  return ;

/*------------------------------------------------ Извлечение данных */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- Создание объекта */

   if(!memicmp(buff, "#BEGIN OBJECT FLYER\n", 
              strlen("#BEGIN OBJECT FLYER\n"))) {                   /* IF.1 */
/*- - - - - - - - - - - - - - -  Извлечение параметров схемы объекта */
              memset(&create_data, 0, sizeof(create_data)) ;

                                     entry=strstr(buff, "\nNAME=") ;  
           strncpy(create_data.name, entry+strlen("\nNAME="),       /* Извлекаем имя объекта */
                                       sizeof(create_data.name)-1) ;
        end=strchr(create_data.name, '\n') ;
       *end= 0 ;
/*- - - - - - - - - - - - - - - Проверка повторного создания объекта */
/*- - - - - - - - - - - - - - - - - - - - - - - - - Создание объекта */
                status=CreateObject(&create_data) ;
             if(status)  return ;

        object=(RSS_Object_Flyer *)this->kernel->kernel_objects[this->kernel->kernel_objects_cnt-1] ;
/*- - - - - - - - - - - - Пропись базовой точки и ориентации объекта */
       entry=strstr(buff, "\nX_BASE=") ; object->x_base=atof(entry+strlen("\nX_BASE=")) ;
       entry=strstr(buff, "\nY_BASE=") ; object->y_base=atof(entry+strlen("\nY_BASE=")) ;
       entry=strstr(buff, "\nZ_BASE=") ; object->z_base=atof(entry+strlen("\nZ_BASE=")) ;
       entry=strstr(buff, "\nA_AZIM=") ; object->a_azim=atof(entry+strlen("\nA_AZIM=")) ;
       entry=strstr(buff, "\nA_ELEV=") ; object->a_elev=atof(entry+strlen("\nA_ELEV=")) ;
       entry=strstr(buff, "\nA_ROLL=") ; object->a_roll=atof(entry+strlen("\nA_ROLL=")) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  Пропись звеньев */
      for(entry=buff ; ; entry=end) {

             entry=strstr(entry, "\nL") ;                           /* Ищем описание параметра звена */
          if(entry==NULL)  break ;
             entry+=strlen("\nL") ;

                         n_link=strtol(entry, &name, 10) ;          /* Извлекаем 'номер' звена */

                      if(n_link==0)  link=&object->link_base ;      /* Определяем ссылку на описание звена */
                 else if(n_link==1)  link=&object->link_x ;
                 else if(n_link==2)  link=&object->link_y ;
                 else if(n_link==3)  link=&object->link_z ;
                 else if(n_link==4)  link=&object->link_azim ;
                 else if(n_link==5)  link=&object->link_elev ;
                 else if(n_link==6)  link=&object->link_roll ;
                 else if(n_link==7)  link=&object->link_head ;

                         value=strchr(name, '=') ;                  /* Проходим на значение параметра */
                      if(value==NULL) {  end=name;  continue ;  }
                        *value=0 ;
                         value++ ;

               if(!stricmp(name, "_VALUE"     ))  link->value     =strtod(value, &end) ;
          else if(!stricmp(name, "_VALUE_KEYS"))  link->value_keys=strtol(value, &end, 10) ;
          else if(!stricmp(name, "_VALUE_MIN" ))  link->value_min =strtod(value, &end) ;
          else if(!stricmp(name, "_VALUE_MAX" ))  link->value_max =strtod(value, &end) ;
          else if(!stricmp(name, "_VALUE_TEST"))  link->value_test=strtol(value, &end, 10) ;
          else if(!stricmp(name, "_X_BASE"    ))  link->x_base    =strtod(value, &end) ;
          else if(!stricmp(name, "_Y_BASE"    ))  link->y_base    =strtod(value, &end) ;
          else if(!stricmp(name, "_Z_BASE"    ))  link->z_base    =strtod(value, &end) ;
          else if(!stricmp(name, "_A_AZIM"    ))  link->a_azim    =strtod(value, &end) ;
          else if(!stricmp(name, "_A_ELEV"    ))  link->a_elev    =strtod(value, &end) ;
          else if(!stricmp(name, "_A_ROLL"    ))  link->a_roll    =strtod(value, &end) ;

                                        value[-1]='=' ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - -  Пропись тел звеньев */
                          memset(&body_tmp, 0, sizeof(body_tmp)) ;

                    n_link_prv=-1 ;
                    n_body_prv=-1 ;

      for(entry=buff ; ; ) {

             entry=strstr(entry, "\nB") ;                           /* Ищем описание параметра звена */
          if(entry==NULL)  break ;
             entry+=strlen("\nB") ;

                         n_link=strtol(entry, &name, 10) ;          /* Извлекаем 'номер' звена */
                         n_body=strtol(name+1, &name, 10) ;         /* Извлекаем 'номер' тела */

          if(n_link!=n_link_prv ||                                  /* Если описание тела считано полностью - */
             n_body!=n_body_prv   )                                 /*  - формируем описание тела             */
           if(body_tmp.desc[0]) {
                      CreateBody(object, link, &body_tmp) ;
                          memset(&body_tmp, 0, sizeof(body_tmp)) ;
                                }  

                      if(n_link==0)  link=&object->link_base ;      /* Определяем ссылку на описание звена */
                 else if(n_link==1)  link=&object->link_x ;
                 else if(n_link==2)  link=&object->link_y ;
                 else if(n_link==3)  link=&object->link_z ;
                 else if(n_link==4)  link=&object->link_azim ;
                 else if(n_link==5)  link=&object->link_elev ;
                 else if(n_link==6)  link=&object->link_roll ;
                 else if(n_link==7)  link=&object->link_head ;

                    n_link_prv=n_link ;
                    n_body_prv=n_body ;

                         value=strchr(name, '=') ;                  /* Проходим на значение параметра */
                      if(value==NULL) {  end=name;  continue ;  }
                        *value=0 ;
                         value++ ;

               if(!stricmp(name, "_DESC"      )) {
                       strncpy(body_tmp.desc, value, sizeof(body_tmp.desc)-1) ;
                    end=strchr(body_tmp.desc, '\n') ;
                 if(end!=NULL)  *end=0 ;
                                                 }
          else if(!stricmp(name, "_X_BASE"    ))  body_tmp.   x_base=strtod(value, &end) ;
          else if(!stricmp(name, "_Y_BASE"    ))  body_tmp.   y_base=strtod(value, &end) ;
          else if(!stricmp(name, "_Z_BASE"    ))  body_tmp.   z_base=strtod(value, &end) ;
          else if(!stricmp(name, "_A_AZIM"    ))  body_tmp.   a_azim=strtod(value, &end) ;
          else if(!stricmp(name, "_A_ELEV"    ))  body_tmp.   a_elev=strtod(value, &end) ;
          else if(!stricmp(name, "_A_ROLL"    ))  body_tmp.   a_roll=strtod(value, &end) ;
          else if(!stricmp(name, "_ZERO"      ))  body_tmp.zero_size=strtod(value, &end) ;
          else if(!stricmp(name, "_MODEL"     )) { 
                       strncpy(body_tmp.model.path, value, sizeof(body_tmp.model.path)-1) ;
                    end=strchr(body_tmp.model.path, '\n') ;
                 if(end!=NULL)  *end=0 ;
                                                 }
          else if(!memicmp(name, "_PAR_", 5   )) {
                                i=strtoul(name+strlen("_PAR_"), &end, 10) ;
                             if(i<0 || i>=_MODEL_PARS_MAX)  continue ;

                       strncpy(body_tmp.model.pars[i].value, value, 
                                         sizeof(body_tmp.model.pars[i].value)-1) ;
                    end=strchr(body_tmp.model.pars[i].value, '\n') ;
                 if(end!=NULL)  *end=0 ;
                                                 }

                                value[-1]='=' ;
                           }

           if(body_tmp.desc[0])
                      CreateBody(object, link, &body_tmp) ;
/*- - - - - - - - - - - - - - - - - - - - - - - Пропись масс звеньев */
                          memset(&mass_tmp, 0, sizeof(mass_tmp)) ;

      for(entry=buff ; ; ) {

             entry=strstr(entry, "\nM") ;                           /* Ищем описание параметра звена */
          if(entry==NULL)  break ;
             entry+=strlen("\nM") ;

                         n_link=strtol(entry, &name, 10) ;          /* Извлекаем 'номер' звена */
                         n_body=strtol(name+1, &name, 10) ;         /* Извлекаем 'номер' тела */

          if(n_link!=n_link_prv ||                                  /* Если описание массы считано полностью - */
             n_body!=n_body_prv   )                                 /*  - формируем описание массы             */
           if(mass_tmp.desc[0]) {

                      CreateMass(object, link, &mass_tmp) ;
                          memset(&mass_tmp, 0, sizeof(mass_tmp)) ;
                                }  

                      if(n_link==0)  link=&object->link_base ;      /* Определяем ссылку на описание звена */
                 else if(n_link==1)  link=&object->link_x ;
                 else if(n_link==2)  link=&object->link_y ;
                 else if(n_link==3)  link=&object->link_z ;
                 else if(n_link==4)  link=&object->link_azim ;
                 else if(n_link==5)  link=&object->link_elev ;
                 else if(n_link==6)  link=&object->link_roll ;
                 else if(n_link==7)  link=&object->link_head ;

                    n_link_prv=n_link ;
                    n_body_prv=n_body ;

                         value=strchr(name, '=') ;                  /* Проходим на значение параметра */
                      if(value==NULL) {  end=name;  continue ;  }
                        *value=0 ;
                         value++ ;

               if(!stricmp(name, "_DESC"      )) {
                       strncpy(mass_tmp.desc, value, sizeof(mass_tmp.desc)-1) ;
                    end=strchr(mass_tmp.desc, '\n') ;
                 if(end!=NULL)  *end=0 ;
                                                 }
          else if(!stricmp(name, "_MASS"      ))  mass_tmp.mass     =strtod(value, &end) ;
          else if(!stricmp(name, "_J_X"       ))  mass_tmp.Jx       =strtod(value, &end) ;
          else if(!stricmp(name, "_J_Y"       ))  mass_tmp.Jy       =strtod(value, &end) ;
          else if(!stricmp(name, "_J_Z"       ))  mass_tmp.Jz       =strtod(value, &end) ;
          else if(!stricmp(name, "_X_BASE"    ))  mass_tmp.x_base   =strtod(value, &end) ;
          else if(!stricmp(name, "_Y_BASE"    ))  mass_tmp.y_base   =strtod(value, &end) ;
          else if(!stricmp(name, "_Z_BASE"    ))  mass_tmp.z_base   =strtod(value, &end) ;
          else if(!stricmp(name, "_A_AZIM"    ))  mass_tmp.a_azim   =strtod(value, &end) ;
          else if(!stricmp(name, "_A_ELEV"    ))  mass_tmp.a_elev   =strtod(value, &end) ;
          else if(!stricmp(name, "_A_ROLL"    ))  mass_tmp.a_roll   =strtod(value, &end) ;
          else if(!stricmp(name, "_MOVE"      ))  mass_tmp.move_coef=strtod(value, &end) ;

                                value[-1]='=' ;
                           }

           if(mass_tmp.desc[0])
                      CreateMass(object, link, &mass_tmp) ;
/*- - - - - - - - - - - - - - - - - - - - Пропись контекстов модулей */
      for(entry=buff ; ; ) {

             entry=strstr(entry, "\nC") ;                           /* Ищем описание контекста */
          if(entry==NULL)  break ;
             entry+=strlen("\nC") ;

                         n_link=strtol(entry, &name, 10) ;          /* Извлекаем 'номер' звена */

                      if(n_link==0)  link=&object->link_base ;      /* Определяем ссылку на описание звена */
                 else if(n_link==1)  link=&object->link_x ;
                 else if(n_link==2)  link=&object->link_y ;
                 else if(n_link==3)  link=&object->link_z ;
                 else if(n_link==4)  link=&object->link_azim ;
                 else if(n_link==5)  link=&object->link_elev ;
                 else if(n_link==6)  link=&object->link_roll ;
                 else if(n_link==7)  link=&object->link_head ;

             entry=strchr(entry, '=') ;
          if(entry==NULL)  break ;
             entry++ ;

             end=strchr(entry, '\n') ;
          if(end==NULL)  text.assign(entry, strlen(entry)) ;
          else           text.assign(entry, end-entry) ;

       for(i=0 ; i<this->kernel->modules_cnt ; i++)                 /* Чтение данных контекстов модулей */
            this->kernel->modules[i].entry->
                                   vReadData(&link->contexts, &text) ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Перерасчет робота */
                       object->iRobotSkeleton() ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }                    /* END.1 */
/*-------------------------------------------- Освобождение ресурсов */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Module_Flyer::vWriteSave(std::string *text)

{

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN MODULE FLYER\n" ;

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции HELP                    */

  int  RSS_Module_Flyer::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_Flyer_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции CREATE                  */
/*								    */
/*      CREATE <Имя>                                                */

  int  RSS_Module_Flyer::cCreate(char *cmd)

{
 RSS_Module_Flyer_Create_data  data ;
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

/*--------------------------------- Подготовка блока данных создания */

         memset(&data, 0, sizeof(data)) ;

        strncpy(data.name,   name,   sizeof(data.name)) ;

/*------------------------------------------------- Создание объекта */

                status=CreateObject(&data) ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции DEGREE                  */
/*								    */
/*      DEGREE    <Имя>                                             */
/*      DEGREE>   <Имя> <Код стрелочки> <Шаг>                       */
/*      DEGREE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Flyer::cDegree(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
             char *pars[_PARS_MAX] ;
             char *name ;
             char *end ;
              int  arrow_flag ;        /* Флаг стрелочного режима */
             char *arrows ;
              int  arrows_code ;       /* Код набора стрелочек */
           double  inverse ;
           double  joint_step ;
           double  link_step ;
              int  status ;
             char  tmp[32] ;
              int  i ;


/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      arrow_flag=0 ;

       if(*cmd=='>') {
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

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: DEGREE <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------- Обработка стрелочного режима */

    if(arrow_flag) {
/*- - - - - - - - - - - - - - - - - - - -  Получение шагов изменения */
        this->kernel->vGetParameter("ROBOT_STEP_JOINT", tmp) ;
                    joint_step=atof(tmp) ;
        this->kernel->vGetParameter("ROBOT_STEP_LINK", tmp) ;
                     link_step=atof(tmp) ;

         if(joint_step<=0.)  joint_step=10. ;
         if( link_step<=0.)   link_step= 0.1 ;
/*- - - - - - - - - - - - - - - - -  Определение кода набора стрелок */
                         arrows     =pars[1] ;
                         arrows_code=  0 ;

      if(strstr(arrows, "shift")!=NULL)    arrows_code+=4 ;
      if(strstr(arrows, "ctrl" )!=NULL)    arrows_code+=2 ;

      if(strstr(arrows, "left" )!=NULL) {  arrows_code+=1 ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL) {  arrows_code+=1 ;  inverse= 1. ;  }  
      if(strstr(arrows, "down" )!=NULL) {  arrows_code+=2 ;  inverse=-1. ;  }   
      if(strstr(arrows, "up"   )!=NULL) {  arrows_code+=2 ;  inverse= 1. ;  }  
/*- - - - - - - - - - - - - - - -  Определение изменяемого параметра */
#define  O  object

      if(O->link_x   .value_keys==arrows_code)  O->link_x   .value+=inverse*link_step ;
      if(O->link_y   .value_keys==arrows_code)  O->link_y   .value+=inverse*link_step ;
      if(O->link_z   .value_keys==arrows_code)  O->link_z   .value+=inverse*link_step ;
      if(O->link_azim.value_keys==arrows_code)  O->link_azim.value+=inverse*joint_step ;
      if(O->link_elev.value_keys==arrows_code)  O->link_elev.value+=inverse*joint_step ;
      if(O->link_roll.value_keys==arrows_code)  O->link_roll.value+=inverse*joint_step ;

#undef  O
/*- - - - - - - - - - - - - - - - - - - - - - - -  Перерасчет робота */
                       object->iRobotSkeleton() ;
                       object->iRobotDegrees (1) ;                  /* Проверка диапазонов степеней подвижности */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          return(0) ;
                   }
/*----------------------------------------------- Проведение диалога */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_DEGREES", RT_DIALOG),
			             GetActiveWindow(),
                                     Object_Flyer_Degree_dialog,
                                    (LPARAM)object              ) ;

/*------------------------------------------------ Перерасчет робота */

                       object->iRobotSkeleton() ;
                       object->iRobotDegrees (1) ;                  /* Проверка диапазонов степеней подвижности */

                        this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции LINK                         */
/*								    */
/*      LINK  <Имя объекта> <Имя звена> <Величина>                  */
/*      LINK+ <Имя объекта> <Имя звена> <Величина>                  */

  int  RSS_Module_Flyer::cLink(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                   int  delta_flag ;        /* Флаг режима приращений */
                  char *end ;
                   int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      delta_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL)  delta_flag=1 ;

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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение звена */
           
   if( pars[1]==NULL ||
      *pars[1]==  0    ) {
                           SEND_ERROR("Не задано звено") ;
                                     return(-1) ;
                         }

       link=object->iGetLinkByName(pars[1]) ;
    if(link==NULL)  return(-1) ;

/*---------------------------------------------- Пропись длины звена */

    if(pars[2]==NULL) {
          SEND_ERROR("Не задана длина звена или ее приращение") ;
                               return(-1) ;
                      }

    if(delta_flag)  link->value+=atof(pars[2]) ;
    else            link->value =atof(pars[2]) ;

/*------------------------------------------------ Перерасчет робота */

              object->iRobotSkeleton() ;
              object->iRobotDegrees (1) ;                           /* Проверка диапазонов степеней подвижности */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции JOINT                        */
/*								    */
/*     JOINT  <Имя объекта> <Имя звена> <Величина>                  */
/*     JOINT+ <Имя объекта> <Имя звена> <Величина>                  */

  int  RSS_Module_Flyer::cJoint(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                   int  delta_flag ;        /* Флаг режима приращений */
                  char *end ;
                   int  i ;

/*-------------------------------------- Дешифровка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                      delta_flag=0 ;

       if(*cmd=='/' ||
          *cmd=='+'   ) {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, '+')!=NULL)  delta_flag=1 ;

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

                     name= pars[0] ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение звена */
           
   if( pars[1]==NULL ||
      *pars[1]==  0    ) {
                           SEND_ERROR("Не задано звено") ;
                                     return(-1) ;
                         }

       link=object->iGetLinkByName(pars[1]) ;
    if(link==NULL)  return(-1) ;

/*---------------------------------------------- Пропись длины звена */

    if(pars[2]==NULL) {
          SEND_ERROR("Не задана угол в шарнире звена или его приращение") ;
                               return(-1) ;
                      }

    if(delta_flag)  link->value+=atof(pars[2]) ;
    else            link->value =atof(pars[2]) ;

/*------------------------------------------------ Перерасчет робота */

              object->iRobotSkeleton() ;
              object->iRobotDegrees (1) ;                           /* Проверка диапазонов степеней подвижности */

                this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции RANGE                   */
/*								    */
/*      RANGE <Имя объекта>                                         */
/*      RANGE <Имя объекта> <Имя звена> <Минимум> <Максимум>        */

  int  RSS_Module_Flyer::cRange(char *cmd)

{
#define   _PARS_MAX   8
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
                   int  max_flag ;
                double  value ;
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
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: RANGE <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Проведение диалога */

   if( pars[1]==NULL ||
      *pars[1]==  0    ) {

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_RANGES", RT_DIALOG),
			             GetActiveWindow(),
                                     Object_Flyer_Range_dialog,
                                    (LPARAM)object              ) ;

                            return(status) ;
                         } 
/*------------------------------------------------- Определение звена */
           
       link=object->iGetLinkByName(pars[1]) ;
    if(link==NULL)  return(-1) ;

/*------------------------------------------------ Разборка диапазона */

                max_flag=0 ;

    for(i=2 ; pars[i]!=NULL && *pars[i]!=0 ; i++) {

         if(strchr(pars[i], 'n')!=NULL ||
            strchr(pars[i], 'N')!=NULL   ) {
                                             link->value_test=0 ;
                                                    continue ;
                                           }

         if(strchr(pars[i], 'y')!=NULL ||
            strchr(pars[i], 'Y')!=NULL   ) {
                                             link->value_test=1 ;
                                                    continue ;
                                           }

                value=atof(pars[i]) ;

           if(max_flag)  link->value_max=value ; 
           else          link->value_min=value ;

                   max_flag=1 ;
                                                  }    
/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции KEYS                    */
/*								    */
/*      KEYS <Имя>                                                  */

  int  RSS_Module_Flyer::cKeys(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
             char *pars[_PARS_MAX] ;
             char *name ;
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
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: KEYS <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Проведение диалога */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_KEYS", RT_DIALOG),
			             GetActiveWindow(),
                                     Object_Flyer_Keys_dialog,
                                    (LPARAM)object              ) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BODY                    */
/*								    */
/*      BODY <Имя>                                                  */

  int  RSS_Module_Flyer::cBody(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
             char *pars[_PARS_MAX] ;
             char *name ;
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
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: BODY <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Проведение диалога */

               object->iClearUpdates() ;

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_BODIES", RT_DIALOG),
			             GetActiveWindow(),
                                     Object_Flyer_Bodies_dialog,
                                    (LPARAM)object              ) ;

/*------------------------------------------------------ Пропись тел */

          object->iLinkFeatures(&object->link_base) ;
          object->iLinkFeatures(&object->link_head) ;

          object->iRobotSkeleton() ;

          object->iClearUpdates() ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BODY_ADD                */
/*								    */
/*      BODYADD <Имя объекта> <Имя звена> <Имя тела>                */

  int  RSS_Module_Flyer::cBodyAdd(char *cmd)

{
#define   _PARS_MAX   4
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
        RSS_Morphology  morpho ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение звена */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

             link=object->iGetLinkByName(pars[1]) ;
          if(link==NULL)  return(-1) ;

/*----------------------------------------------------- Пропись тела */

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано название тела") ;
                                             return(-1) ;
                            }

          for(i=0 ; i<_BODIES_MAX ; i++)                            /* Ищем свободный слот */
            if(link->bodies[i].use_flag==0)  break ;

            if(i==_BODIES_MAX) {
                                 SEND_ERROR("Переполнение списка тел") ;
                                    return(-1) ;
                               }

           memset(&link->bodies[i], 0, sizeof(link->bodies[i])) ;
           strcpy( link->bodies[i].desc, pars[2]) ;
                   link->bodies[i].use_flag=1 ;

/*----------------------------- Добавление морфологического элемента */

                           strcpy(morpho.link, pars[1]) ;
                           strcpy(morpho.body, pars[2]) ;
                                  morpho.ptr =&link->bodies[i] ;
          object->vAddMorphology(&morpho) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BODY_MODEL              */
/*								    */
/*  BODYMODEL <Имя объекта> <Имя звена> <Имя тела> ...              */
/*                         ... <Файл модели> <Параметры>            */

  int  RSS_Module_Flyer::cBodyModel(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
 RSS_Object_Flyer_Body *body ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
                   int  i ;

#define  M  body->model

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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение тела */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано тело в конфигурации звена") ;
                                             return(-1) ;
                            }

             body=object->iGetBodyByName(pars[1], pars[2]) ;
          if(body==NULL)  return(-1) ;

                  object->iClearUpdates() ;
                    body->change=1 ;

/*-------------------------------------------------- Привязка модели */

          if(pars[3]==NULL) {
                              SEND_ERROR("Не задан файл модели тела") ;
                                             return(-1) ;
                            }

            strcpy(M.path, pars[3]) ;                               /* Прописываем путь к файлу модели */
            strcat(M.path, ".obj")  ;

            strcpy(M.lib_path, pars[3]) ;                           /* Вырезяем раздел библиотеки */
       end=strrchr(M.lib_path, '\\') ;
    if(end!=NULL)  *end=0 ;

                    RSS_Model_ReadPars(&M) ;                        /* Считываем параметры модели */

/*---------------------------------------- Пропись параметров модели */

    for(i=0 ; i<_MODEL_PARS_MAX && M.pars[i].text[0] ; i++) {

         if(pars[i+4]==NULL) {
                               SEND_ERROR("Недостаточно параметров модели") ;
                                             return(-1) ;
                             }

             strcpy(M.pars[i].value, pars[i+4]) ;
                                                            }
/*------------------------------------------- Загрузка описания тела */

                 link=object->iGetLinkByName(pars[1]) ;
                       object->iLinkFeatures(link) ;

                       object->iRobotSkeleton() ;

                       object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX
#undef    M

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции BODY_SHIFT                   */
/*								    */
/*  BODYSHIFT <Имя объекта> <Имя звена> <Имя тела> ...              */
/*                  ... <X> <Y> <Z> <Азимут> <Наклон> <Крен>        */

  int  RSS_Module_Flyer::cBodyShift(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
 RSS_Object_Flyer_Body *body ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение тела */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано тело в конфигурации звена") ;
                                             return(-1) ;
                            }

             body=object->iGetBodyByName(pars[1], pars[2]) ;
          if(body==NULL)  return(-1) ;

                  object->iClearUpdates() ;
                    body->change=1 ;

/*---------------------------------------- Пропись параметров модели */

    for(i=0 ; i<_PARS_MAX && pars[i+3]!=NULL ; i++) {

           if(i==0)  body->x_base=atof(pars[i+3]) ;
      else if(i==1)  body->y_base=atof(pars[i+3]) ;
      else if(i==2)  body->z_base=atof(pars[i+3]) ;
      else if(i==3)  body->a_azim=atof(pars[i+3]) ;
      else if(i==4)  body->a_elev=atof(pars[i+3]) ;
      else if(i==5)  body->a_roll=atof(pars[i+3]) ;
                                                    }
/*------------------------------------------- Загрузка описания тела */

                 link=object->iGetLinkByName(pars[1]) ;
                       object->iLinkFeatures(link) ;

                       object->iRobotSkeleton() ;

                       object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции BODY_ZERO                    */
/*								    */
/*  BODYZERO <Имя объекта> <Имя звена> <Имя тела> <Нуль-длина>      */

  int  RSS_Module_Flyer::cBodyZero(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
 RSS_Object_Flyer_Body *body ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
                   int  i ;

#define  M  body->model

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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение тела */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано тело в конфигурации звена") ;
                                             return(-1) ;
                            }

             body=object->iGetBodyByName(pars[1], pars[2]) ;
          if(body==NULL)  return(-1) ;

                  object->iClearUpdates() ;
                    body->change=1 ;

/*---------------------------------------- Пропись параметров модели */

                     body->zero_size=atof(pars[3]) ;

/*------------------------------------------- Загрузка описания тела */

                 link=object->iGetLinkByName(pars[1]) ;
                       object->iLinkFeatures(link) ;

                       object->iRobotSkeleton() ;

                       object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MASS                    */
/*								    */
/*      MASS <Имя>                                                  */

  int  RSS_Module_Flyer::cMass(char *cmd)

{
#define   _PARS_MAX   4
 RSS_Object_Flyer *object ;
             char *pars[_PARS_MAX] ;
             char *name ;
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
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: MASS <Имя_объекта>") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*----------------------------------------------- Проведение диалога */

               object->iClearUpdates() ;

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_MASSES", RT_DIALOG),
			             GetActiveWindow(),
                                     Object_Flyer_Masses_dialog,
                                    (LPARAM)object              ) ;

/*----------------------------------------------------- Пропись масс */

          object->iLinkFeatures(&object->link_base) ;
          object->iLinkFeatures(&object->link_head) ;

          object->iRobotSkeleton() ;

          object->iClearUpdates() ;

            this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(status) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции MASS_ADD                */
/*								    */
/*      MASSADD <Имя объекта> <Имя звена> <Имя массы> ...           */
/*                     ... <Масса> <Главные моменты инерции>        */

  int  RSS_Module_Flyer::cMassAdd(char *cmd)

{
#define   _PARS_MAX   8
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
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
                           SEND_ERROR("Не задано имя объекта.\n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение звена */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

             link=object->iGetLinkByName(pars[1]) ;
          if(link==NULL)  return(-1) ;

/*----------------------------------------------------- Пропись тела */

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано название тела") ;
                                             return(-1) ;
                            }

          for(i=0 ; i<_BODIES_MAX ; i++)                            /* Ищем массу по имени */
            if(!stricmp(pars[2], link->masses[i].desc))  break ;

        if(i==_BODIES_MAX) {                                        /* Если такой массы еще нет - */
          for(i=0 ; i<_BODIES_MAX ; i++)                            /*   - ищем свободный слот    */
            if(link->masses[i].use_flag==0)  break ;
                           }

        if(i==_BODIES_MAX) {
                             SEND_ERROR("Переполнение списка масс") ;
                                 return(-1) ;
                           }

           memset(&link->masses[i], 0, sizeof(link->masses[i])) ;
           strcpy( link->masses[i].desc, pars[2]) ;
                   link->masses[i].use_flag=1 ;

    for(i=3 ; i<_PARS_MAX && pars[i]!=NULL ; i++) {
           if(i==3)  link->masses[i].mass=atof(pars[i]) ;
      else if(i==4)  link->masses[i].Jx  =atof(pars[i]) ;
      else if(i==5)  link->masses[i].Jy  =atof(pars[i]) ;
      else if(i==5)  link->masses[i].Jz  =atof(pars[i]) ;
                                                  }

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции MASS_SHIFT                   */
/*								    */
/*  MASSSHIFT <Имя объекта> <Имя звена> <Имя массы> ...             */
/*                  ... <X> <Y> <Z> <Азимут> <Наклон> <Крен>        */

  int  RSS_Module_Flyer::cMassShift(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
 RSS_Object_Flyer_Mass *mass ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение тела */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано тело в конфигурации звена") ;
                                             return(-1) ;
                            }

             mass=object->iGetMassByName(pars[1], pars[2]) ;
          if(mass==NULL)  return(-1) ;

                  object->iClearUpdates() ;
                    mass->change=1 ;

/*---------------------------------------- Пропись параметров модели */

    for(i=0 ; i<_PARS_MAX && pars[i]!=NULL ; i++) {

           if(i==3)  mass->x_base=atof(pars[i]) ;
      else if(i==4)  mass->y_base=atof(pars[i]) ;
      else if(i==5)  mass->z_base=atof(pars[i]) ;
      else if(i==6)  mass->a_azim=atof(pars[i]) ;
      else if(i==7)  mass->a_elev=atof(pars[i]) ;
      else if(i==8)  mass->a_roll=atof(pars[i]) ;
                                                  }
/*------------------------------------------- Загрузка описания тела */

                 link=object->iGetLinkByName(pars[1]) ;
                       object->iLinkFeatures(link) ;

                       object->iRobotSkeleton() ;

                       object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*               Реализация инструкции MASS_MOVE                    */
/*								    */
/*  MASSMOVE <Имя объекта> <Имя звена> <Имя массы> <Коэф.сдвига>    */

  int  RSS_Module_Flyer::cMassMove(char *cmd)

{
#define   _PARS_MAX   16
      RSS_Object_Flyer *object ;
 RSS_Object_Flyer_Link *link ;
 RSS_Object_Flyer_Mass *mass ;
                  char *pars[_PARS_MAX] ;
                  char *name ;
                  char *end ;
                   int  i ;

#define  M  body->model

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
                           SEND_ERROR("Не задано имя объекта. \n") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*------------------------------------------------- Определение тела */
           
          if(pars[1]==NULL) {
                              SEND_ERROR("Не задано звено") ;
                                             return(-1) ;
                            }

          if(pars[2]==NULL) {
                              SEND_ERROR("Не задано имя массы") ;
                                             return(-1) ;
                            }

             mass=object->iGetMassByName(pars[1], pars[2]) ;
          if(mass==NULL)  return(-1) ;

                  object->iClearUpdates() ;
                    mass->change=1 ;

/*---------------------------------------- Пропись параметров модели */

     if(pars[3]!=NULL)  mass->move_coef=atof(pars[3]) ;

/*------------------------------------------- Загрузка описания тела */

                 link=object->iGetLinkByName(pars[1]) ;
                       object->iLinkFeatures(link) ;

                       object->iRobotSkeleton() ;

                       object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции INFO                    */
/*								    */
/*        INFO   <Имя>                                              */
/*        INFO/  <Имя>                                              */

  int  RSS_Module_Flyer::cInfo(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *name ;
 RSS_Object_Flyer  *object ;
              int   all_flag ;   /* Флаг режима полной информации */
             char  *end ;
      std::string   info ;
      std::string   f_info ;
             char   part[4096] ;
             char   text[4096] ;
              int   i ;

/*---------------------------------------- Разборка командной строки */
/*- - - - - - - - - - - - - - - - - - -  Выделение ключей управления */
                           all_flag=0 ;

       if(*cmd=='/') {
                           all_flag=1 ;
 
                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Некорректный формат команды") ;
                                       return(-1) ;
                              }

                           cmd=end+1 ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Разбор параметров */        
                  name=cmd ;
                   end=strchr(name, ' ') ;
                if(end!=NULL)  *end=0 ;

/*------------------------------------------- Контроль имени объекта */

    if(name   ==NULL ||
       name[0]==  0    ) {                                          /* Если имя не задано... */
                           SEND_ERROR("Не задано имя объекта. \n"
                                      "Например: INFO <Имя_объекта> ...") ;
                                     return(-1) ;
                         }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

/*-------------------------------------------- Формирование описания */

       strcpy(text, "") ;
      sprintf(part, "%s\r\n%s\r\n"
                    "Base X  %7.3lf\r\n" 
                    "     Y  %7.3lf\r\n" 
                    "     Z  %7.3lf\r\n"
                    "Ang. A %8.3lf\r\n" 
                    "     E %8.3lf\r\n" 
                    "\r\n",
                        object->Name, object->Type, 
                        object->x_base, object->y_base, object->z_base,
                        object->a_azim, object->a_elev                  ) ;
       strcat(text, part) ;

      sprintf(part, " X  %8.3lf\r\n", object->link_x.value) ;
       strcat(text, part) ;
      sprintf(part, " Y  %8.3lf\r\n", object->link_y.value) ;
       strcat(text, part) ;
      sprintf(part, " Z  %8.3lf\r\n", object->link_z.value) ;
       strcat(text, part) ;

      sprintf(part, " A  %8.3lf\r\n", object->link_azim.value) ;
       strcat(text, part) ;
      sprintf(part, " E  %8.3lf\r\n", object->link_elev.value) ;
       strcat(text, part) ;
      sprintf(part, " R  %8.3lf\r\n", object->link_roll.value) ;
       strcat(text, part) ;

           info=text ;

/*----------------------------------------------- Запрос на Свойства */

   for(i=0 ; i<object->Features_cnt ; i++) {

                      object->Features[i]->vGetInfo(&f_info) ;
                                               info+=f_info ;
                                               info+="\r\n" ;
                                           }
/*-------------------------------------------------- Выдача описания */
/*- - - - - - - - - - - - - - - - - - - - - - - - - В отдельное окно */
     if(all_flag) {

                  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - В главное окно */
     else         {

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_SHOW_INFO, (LPARAM)info.c_str()) ;
                  }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции BASE                    */
/*								    */
/*        BASE    <Имя> <X> <Y> <Z>                                 */
/*        BASE/X  <Имя> <X>                                         */
/*        BASE/+X <Имя> <X>                                         */
/*        BASE>   <Имя> <Код стрелочки> <Шаг>                       */
/*        BASE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Flyer::cBase(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
 RSS_Object_Flyer  *object ;
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
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: BASE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

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

          if(xyz_flag=='X')   object->x_base+=inverse*coord[0] ;
     else if(xyz_flag=='Y')   object->y_base+=inverse*coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base+=inverse*coord[0] ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='X')   object->x_base=coord[0] ;
     else if(xyz_flag=='Y')   object->y_base=coord[0] ;                 
     else if(xyz_flag=='Z')   object->z_base=coord[0] ;
     else                   {
                              object->x_base=coord[0] ;
                              object->y_base=coord[1] ;
                              object->z_base=coord[2] ;
                            }
                  }
/*------------------------------------------------ Перерасчет робота */

                       object->iRobotSkeleton() ;

                         this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Реализация инструкции ANGLE                   */
/*								    */
/*       ANGLE    <Имя> <A> <E>                                     */
/*       ANGLE/A  <Имя> <A>                                         */
/*       ANGLE/+A <Имя> <A>                                         */
/*       ANGLE>   <Имя> <Код стрелочки> <Шаг>                       */
/*       ANGLE>>  <Имя> <Код стрелочки> <Шаг>                       */

  int  RSS_Module_Flyer::cAngle(char *cmd)

{
#define  _COORD_MAX   3
#define   _PARS_MAX   4

             char  *pars[_PARS_MAX] ;
             char  *name ;
             char **xyz ;
           double   coord[_COORD_MAX] ;
              int   coord_cnt ;
           double   inverse ;
 RSS_Object_Flyer  *object ;
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

      if(strstr(arrows, "left" )!=NULL) {  xyz_flag='A' ;  inverse=-1. ;  }
      if(strstr(arrows, "right")!=NULL) {  xyz_flag='A' ;  inverse= 1. ;  }  

      if(strstr(arrows, "up"   )!=NULL) {  xyz_flag='E' ;  inverse= 1. ;  }
      if(strstr(arrows, "down" )!=NULL) {  xyz_flag='E' ;  inverse=-1. ;  }
          
                          xyz=&pars[2] ;   
                   }
/*------------------------------------------- Контроль имени объекта */

    if(name==NULL) {                                                /* Если имя не задано... */
                      SEND_ERROR("Не задано имя объекта. \n"
                                 "Например: ANGLE <Имя_объекта> ...") ;
                                     return(-1) ;
                   }

       object=FindObject(name) ;                                    /* Ищем объект по имени */
    if(object==NULL)  return(-1) ;

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
               if(coord_cnt <2)  error="Должно быть указаны 2 координаты" ;
                   }

      if(error!=NULL) {  SEND_ERROR(error) ;
                               return(-1) ;   }

/*------------------------------------------------ Пропись координат */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - Приращения */
   if(delta_flag) {

          if(xyz_flag=='A')   object->a_azim+=inverse*coord[0] ;
     else if(xyz_flag=='E')   object->a_elev+=inverse*coord[0] ;                 
                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  Абсолютные значения */
   else           {

          if(xyz_flag=='A')   object->a_azim=coord[0] ;
     else if(xyz_flag=='E')   object->a_elev=coord[0] ;                 
     else                   {
                              object->a_azim=coord[0] ;
                              object->a_elev=coord[1] ;
                            }
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  Нормализация значений */
     while(object->a_azim> 180.)  object->a_azim-=360. ;
     while(object->a_azim<-180.)  object->a_azim+=360. ;

     while(object->a_elev> 180.)  object->a_elev-=360. ;
     while(object->a_elev<-180.)  object->a_elev+=360. ;

/*------------------------------------------------ Перерасчет робота */

                       object->iRobotSkeleton() ;

                         this->kernel->vShow(NULL) ;

/*-------------------------------------------------------------------*/

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*                Поиск обьекта типа FLYER по имени                 */

  RSS_Object_Flyer *RSS_Module_Flyer::FindObject(char *name)

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

     if(strcmp(OBJECTS[i]->Type, "Flyer")) {

           SEND_ERROR("Объект не является объектом типа FLYER") ;
                            return(NULL) ;
                                           }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_Flyer *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		      Создание объекта                              */

  int  RSS_Module_Flyer::CreateObject(RSS_Module_Flyer_Create_data *data)

{
  RSS_Object_Flyer *object ;
               int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
 
/*--------------------------------------------------- Проверка имени */

    if(data->name[0]==0) {                                           /* Если имя НЕ задано */
              SEND_ERROR("Секция FLYER: Не задано имя объекта") ;
                                return(-1) ;
                         }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, data->name)) {
              SEND_ERROR("Секция FLYER: Объект с таким именем уже существует") ;
                                return(-1) ;
                                                    }
/*------------------------------------------------- Создание обьекта */

       object=new RSS_Object_Flyer ;
    if(object==NULL) {
              SEND_ERROR("Секция FLYER: Недостаточно памяти для создания объекта") ;
                        return(-1) ;
                     }
/*---------------------------------- Создание списка свойств обьекта */

      object->Features_cnt=this->feature_modules_cnt ;
      object->Features    =(RSS_Feature **)
                             calloc(this->feature_modules_cnt, 
                                     sizeof(object->Features[0])) ;

   for(i=0 ; i<this->feature_modules_cnt ; i++)
      object->Features[i]=this->feature_modules[i]->vCreateFeature(object) ;

/*---------------------------------- Введение объекта в общий список */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("Секция FLYER: Переполнение памяти") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
       strcpy(OBJECTS[OBJECTS_CNT]->Name, data->name) ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)data->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Создание тела                                 */

  int  RSS_Module_Flyer::CreateBody(RSS_Object_Flyer      *object,
                                    RSS_Object_Flyer_Link *link,
                                    RSS_Object_Flyer_Body *body_model)
{
   RSS_Morphology  morpho ;
              int  i ;

/*------------------------------------------ Включение тела в список */

    for(i=0 ; i<_BODIES_MAX ; i++)                                  /* Ищем свободный слот */
      if(link->bodies[i].use_flag==0)  break ;

      if(i==_BODIES_MAX) {
                           SEND_ERROR("Переполнение списка тел") ;
                                    return(-1) ;
                         }

           memset(&link->bodies[i], 0, sizeof(link->bodies[i])) ;

           memcpy(&link->bodies[i], body_model, sizeof(*body_model)) ;

                   link->bodies[i].use_flag=1 ;
                   link->bodies[i].change  =1 ;

/*----------------------------- Добавление морфологического элемента */

                           strcpy(morpho.link, link->name) ;
                           strcpy(morpho.body, body_model->desc) ;
                                  morpho.ptr =&link->bodies ;
          object->vAddMorphology(&morpho) ;

/*----------------------------------------- Параметры положения тела */

             link->bodies[i].x_base   =body_model->x_base ;
             link->bodies[i].y_base   =body_model->y_base ;
             link->bodies[i].z_base   =body_model->z_base ;
             link->bodies[i].a_azim   =body_model->a_azim ;
             link->bodies[i].a_elev   =body_model->a_elev ;
             link->bodies[i].a_roll   =body_model->a_roll ;
             link->bodies[i].zero_size=body_model->zero_size ;

/*----------------------------------------- Формирование модели тела */

                    RSS_Model_ReadPars(&link->bodies[i].model) ;    /* Считываем параметры модели */

                 object->iLinkFeatures(link) ;
                 object->iClearUpdates() ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Создание массы                                */

  int  RSS_Module_Flyer::CreateMass(RSS_Object_Flyer      *object,
                                    RSS_Object_Flyer_Link *link,
                                    RSS_Object_Flyer_Mass *mass_model)
{
              int  i ;

/*----------------------------------------- Включение массы в список */

    for(i=0 ; i<_BODIES_MAX ; i++)                                  /* Ищем свободный слот */
      if(link->masses[i].use_flag==0)  break ;

      if(i==_BODIES_MAX) {
                           SEND_ERROR("Переполнение списка масс") ;
                                    return(-1) ;
                         }

           memset(&link->masses[i], 0, sizeof(link->masses[i])) ;

           memcpy(&link->masses[i], mass_model, sizeof(*mass_model)) ;

                   link->masses[i].use_flag=1 ;
                   link->masses[i].change  =1 ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Module_Flyer::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Module_Flyer::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         


    if(path==NULL) {
                             path="m_flyer.log" ;
      if(!init_flag)  unlink(path) ;
          init_flag=1 ;
                   }

       file=fopen(path, "at") ;
    if(file==NULL)  return ;

           fwrite(text, 1, strlen(text), file) ;
           fwrite("\n", 1, strlen("\n"), file) ;
           fclose(file) ;
}


/*********************************************************************/
/*********************************************************************/
/** 							            **/
/**                  ОПИСАНИЕ КЛАССА ОБЪЕКТА                        **/
/** 							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*		       Конструктор класса			     */

     RSS_Object_Flyer::RSS_Object_Flyer(void)

{

/*--------------------------------------------- Пропись типа объекта */

   strcpy(Type, "Flyer") ;

/*---------------------------------- Инициализация списка параметров */

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

/*-------------------------------------- Инициализация базовой точки */

      x_base=0 ;
      y_base=0 ;
      z_base=0 ;
      a_azim=0 ;
      a_elev=0 ;
      a_roll=0 ;

/*----------------------------------- Инициализация описаний звеньев */

      memset(&link_base, 0, sizeof(link_base)) ;
      memset(&link_x,    0, sizeof(link_x)) ;
      memset(&link_y,    0, sizeof(link_y)) ;
      memset(&link_z,    0, sizeof(link_z)) ;
      memset(&link_azim, 0, sizeof(link_azim)) ;
      memset(&link_elev, 0, sizeof(link_elev)) ;
      memset(&link_roll, 0, sizeof(link_roll)) ;
      memset(&link_head, 0, sizeof(link_head)) ; 

             link_base.value_fixed=1 ;
             link_x   .value_fixed=0 ;
             link_y   .value_fixed=0 ;
             link_z   .value_fixed=0 ;
             link_azim.value_fixed=0 ;
             link_elev.value_fixed=0 ;
             link_roll.value_fixed=0 ;
             link_head.value_fixed=1 ;

             link_base.parent=this ;
             link_x   .parent=this ;
             link_y   .parent=this ;
             link_z   .parent=this ;
             link_azim.parent=this ;
             link_elev.parent=this ;
             link_roll.parent=this ;
             link_head.parent=this ;

/*----------------------------------------- Простановка имен звеньев */

      strcpy(link_base.name, "base") ;
      strcpy(link_x   .name, "x") ;
      strcpy(link_y   .name, "y") ;
      strcpy(link_z   .name, "z") ;
      strcpy(link_azim.name, "a") ;
      strcpy(link_elev.name, "e") ;
      strcpy(link_roll.name, "r") ;
      strcpy(link_head.name, "head") ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        Деструктор класса			    */

    RSS_Object_Flyer::~RSS_Object_Flyer(void)

{
}


/********************************************************************/
/*								    */
/*     Выдать координаты целевой точки, привязанной к объекту       */

     int  RSS_Object_Flyer::vGetTarget(char *part, RSS_Point *target)

{
// RSS_Object_Flyer_Link *link ;

/*------------------------------------------------------- Подготовка */

/*--------------------------- Определение "глобальной точки" объекта */

   if( part==NULL ||
      *part==  0    ) {
                           target->x=this->x_base ;
                           target->y=this->y_base ;
                           target->z=this->z_base ;
                               return(0) ;
                      }
/*-------------------------------- Определение базовой точки объекта */

   if(!stricmp(part, "Base")) {
                                   target->x=this->x_base ;
                                   target->y=this->y_base ;
                                   target->z=this->z_base ;
                                       return(0) ;
                              }
/*-------------------------------- Определение целевой точки объекта */

   if(!stricmp(part, "Target")) {
                                   target->x=this->x_base ;
                                   target->y=this->y_base ;
                                   target->z=this->z_base ;
                                       return(0) ;
                                }
/*-------------------------------------------------------------------*/

    return(0) ;
}


/********************************************************************/
/*								    */
/*             Выдача звена обьекта типа FLYER по имени             */

  RSS_Object_Flyer_Link *
        RSS_Object_Flyer::iGetLinkByName(char *name)

{
  RSS_Object_Flyer_Link *link ;


          if(!stricmp(name, "base"))  link=&this->link_base ;
     else if(!stricmp(name, "x"   ))  link=&this->link_x ;
     else if(!stricmp(name, "y"   ))  link=&this->link_y ;
     else if(!stricmp(name, "z"   ))  link=&this->link_z ;
     else if(!stricmp(name, "a"   ))  link=&this->link_azim ;
     else if(!stricmp(name, "e"   ))  link=&this->link_elev ;
     else if(!stricmp(name, "r"   ))  link=&this->link_roll ;
     else if(!stricmp(name, "head"))  link=&this->link_head ;
     else                                  {
                             SEND_ERROR("Неизвестное звено") ;
                                             return(NULL) ;
                                           }

  return(link) ;
}


/********************************************************************/
/*								    */
/*          Выдача тела звена обьекта типа FLYER по имени           */

  RSS_Object_Flyer_Body *
        RSS_Object_Flyer::iGetBodyByName(char *link_name, 
                                         char *body_name )
{
  RSS_Object_Flyer_Link *link ;
                    int  i ;


          link=iGetLinkByName(link_name) ;
       if(link==NULL)  return(NULL) ;

     for(i=0 ; i<_BODIES_MAX ; i++) 
       if(         link->bodies[i].use_flag &&
          !stricmp(link->bodies[i].desc, body_name))  break ;

     if(i==_BODIES_MAX) {
                           SEND_ERROR("Неизвестное тело в конфигурации звена") ;
                                     return(NULL) ;
                        }

  return(&link->bodies[i]) ;
}


/********************************************************************/
/*								    */
/*          Выдача массы звена обьекта типа FLYER по имени          */

  RSS_Object_Flyer_Mass *
        RSS_Object_Flyer::iGetMassByName(char *link_name, 
                                         char *mass_name )
{
  RSS_Object_Flyer_Link *link ;
                    int  i ;


          link=iGetLinkByName(link_name) ;
       if(link==NULL)  return(NULL) ;

     for(i=0 ; i<_BODIES_MAX ; i++) 
       if(         link->masses[i].use_flag &&
          !stricmp(link->masses[i].desc, mass_name))  break ;

     if(i==_BODIES_MAX) {
                           SEND_ERROR("Неизвестная масса в конфигурации звена") ;
                                     return(NULL) ;
                        }

  return(&link->masses[i]) ;
}


/*********************************************************************/
/*								     */
/*		      Сброс флагов изменений                         */

  void  RSS_Object_Flyer::iClearUpdates(void)

{
   int  j ;


     for(j=0 ; j<_BODIES_MAX ; j++)  this->link_base.bodies[j].change=0 ;
     for(j=0 ; j<_BODIES_MAX ; j++)  this->link_head.bodies[j].change=0 ;
}


/*********************************************************************/
/*								     */
/*		      Пропись свойств звена                          */

  int  RSS_Object_Flyer::iLinkFeatures(RSS_Object_Flyer_Link *link)

{
  char  body_name[128] ;
  char *end ;
   int  pars_cnt ;
   int  i ;
   int  j ;
   int  k ;

#define  B          link->bodies[i]
#define  PAR        link->bodies[i].pars

/*------------------------------------------------------ Перебор тел */

   for(i=0 ; i<_BODIES_MAX ; i++) if(B.change) {                    /* CIRCLE.1 - Для измененных тел */

           RSS_Model_ReadSect(&B.model) ;                           /* Считываем секции описаний модели */

              sprintf(body_name, "%s.%s", link->name, B.desc) ;     /* Определяем имя тела */

/*----------------------------------- Формирование списка параметров */

    if(PAR==NULL) {                                                 /* IF.1 */
                        pars_cnt=0 ;
/*- - - - - - - - - - - - - - - - - Формируем список параметров тела */
     for(j=0 ; j<_MODEL_PARS_MAX ; j++)
       if(B.model.pars[j].text[0]!=0) {

           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (pars_cnt+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция FLYER: Переполнение памяти") ;
                                            return(-1) ;
                      }

             memset(&PAR[pars_cnt], 0, sizeof(PAR[pars_cnt])) ;
            sprintf( PAR[pars_cnt].name, "PAR%d", j+1) ;
                     PAR[pars_cnt].value=strtod(B.model.pars[j].value, &end) ;
                         pars_cnt++ ;
                                      }
/*- - - - - - - - - - - - - - - Добавление в список параметров звена */
           PAR=(struct RSS_Parameter *)
                 realloc(PAR, (pars_cnt+2)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция FLYER: Переполнение памяти") ;
                                            return(-1) ;
                      }

             memset(&PAR[pars_cnt], 0, sizeof(PAR[pars_cnt])) ;
             strcpy( PAR[pars_cnt].name, "LINK") ;
                     PAR[pars_cnt].ptr  = &link->value ;
                         pars_cnt++ ;

             memset(&PAR[pars_cnt], 0, sizeof(PAR[pars_cnt])) ;
             strcpy( PAR[pars_cnt].name, "BABY") ;
                     PAR[pars_cnt].ptr  = &B.zero_size ;
                         pars_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - Терминируем пустой записью */
           PAR=(struct RSS_Parameter *)                             
                 realloc(PAR, (pars_cnt+1)*sizeof(*PAR)) ;
        if(PAR==NULL) {
                         SEND_ERROR("Секция FLYER: Переполнение памяти") ;
                                            return(-1) ;
                      }

             memset(&PAR[pars_cnt], 0, sizeof(PAR[pars_cnt])) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  }                                                 /* END.1 */
/*--------------------------------------------- Пропись описаний тел */

     for(j=0 ; B.model.sections[j].title[0] ; j++) {

       for(k=0 ; k<Features_cnt ; k++) {
                  Features[k]->vBodyPars(body_name, PAR) ;
                  Features[k]->vReadSave(B.model.sections[j].title,
                                         B.model.sections[j].decl,
                                         body_name ) ;
                Features[k]->vBodyShifts(body_name,
                                         B.x_base, B.y_base, B.z_base,
                                         B.a_azim, B.a_elev, B.a_roll ) ;
                                       }

                                         B.model.sections[j].title[0]= 0 ;
                                 delete  B.model.sections[j].decl ;
                                         B.model.sections[j].decl=NULL ;
                                                   }
/*------------------------------------------------------ Перебор тел */
                                               }                    /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef  B
#undef  PAR
#undef  PAR_CNT

    return(0) ;
}


/*********************************************************************/
/*								     */
/*             Пропись структурной геометрии манипулятора            */

  void  RSS_Object_Flyer::iRobotSkeleton(void)

{
   Matrix2d  Summary ;
   Matrix2d  Local ;

/*---------------------------------------------------- Инициализация */

                Summary.LoadE(4, 4) ;

/*---------------------------------------------------- Базовое звено */

                  Local.Load4d_base(this->x_base, 
                                    this->y_base,
                                    this->z_base ) ;
                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_azim(-this->a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_elev(-this->a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_roll(-this->a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_base, &Summary) ;

/*---------------------------------------------------------- X-звено */

                  Local.Load4d_base(link_x.value, 0., 0.) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_x, &Summary) ;

/*---------------------------------------------------------- Y-звено */

                  Local.Load4d_base(0., link_y.value, 0.) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_y, &Summary) ;

/*---------------------------------------------------------- Z-звено */

                  Local.Load4d_base(0., 0., link_z.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_z, &Summary) ;

/*------------------------------------------------------- AZIM-звено */

                  Local.Load4d_azim(-link_azim.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_azim, &Summary) ;

/*------------------------------------------------------- ELEV-звено */

                  Local.Load4d_elev(-link_elev.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_elev, &Summary) ;

/*------------------------------------------------------- ROLL-звено */

                  Local.Load4d_roll(-link_roll.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_roll, &Summary) ;

/*------------------------------------------------------------ Схват */

                      iLinkSkeleton(&link_head, &Summary) ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*								     */
/*             Пропись структурной геометрии звена                   */

  void  RSS_Object_Flyer::iLinkSkeleton(RSS_Object_Flyer_Link *link,
                                                     Matrix2d *matrix)

{
   char  body_name[128] ;
 double  matrix_tmp[4][4] ;
    int  i ;
    int  j ;
    int  k ;

/*------------------------------------- Обработка матричного задания */

   if(matrix!=NULL) {
/*- - - - - - - - - - - - - - - - - - - - - - - -  Подготовка данных */
           for(i=0 ; i<4 ; i++) 
           for(j=0 ; j<4 ; j++)
                  matrix_tmp[i][j]=matrix->GetCell(i, j) ;
/*- - - - - - - - - - - - - - - - - - - - Пропись данных на свойства */
   for(i=0 ; i<_BODIES_MAX ; i++)
      if(link->bodies[i].use_flag) {

              sprintf(body_name, "%s.%s", link->name,               /* Определяем имя тела */
                                          link->bodies[i].desc) ;  

       for(k=0 ; k<Features_cnt ; k++)
                  Features[k]->vBodyMatrix(body_name, matrix_tmp) ;

                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    }
/*----------------------------------- Обработка нематричного задания */
   else             {
/*- - - - - - - - - - - - - -  Учет ориентации и положения платформы */
              link->x_base+=this->x_base ;
              link->y_base+=this->y_base ;
              link->z_base+=this->z_base ;
/*- - - - - - - - - - - - - - - - - - - - Пропись данных на свойства */
   for(i=0 ; i<_BODIES_MAX ; i++)
      if(link->bodies[i].use_flag) {

              sprintf(body_name, "%s.%s", link->name,               /* Определяем имя тела */
                                          link->bodies[i].desc) ;

       for(k=0 ; k<Features_cnt ; k++) {
                  Features[k]->vBodyBasePoint(body_name,
                                              link->x_base, 
                                              link->y_base, 
                                              link->z_base ) ;
                  Features[k]->vBodyAngles   (body_name, 
                                              link->a_azim, 
                                              link->a_elev, 
                                              link->a_roll ) ;
                                       }
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*      Проверка степеней подвижности маниоулятора по диапазонам     */

  int  RSS_Object_Flyer::iRobotDegrees(int  indicate)

{
  char  text[1024] ; 
  char *title ;  
   int  status ;


                  sprintf(text, "%s", this->Name) ;

      if(indicate)  title=text ;
      else          title=NULL ;

         status=iLinkDegrees(&link_x, title) ;
      if(status) return(status) ;

         status=iLinkDegrees(&link_y, title) ;
      if(status) return(status) ;

         status=iLinkDegrees(&link_z, title) ;
      if(status) return(status) ;

         status=iLinkDegrees(&link_azim, title) ;
      if(status) return(status) ;

         status=iLinkDegrees(&link_elev, title) ;
      if(status) return(status) ;

         status=iLinkDegrees(&link_roll, title) ;
      if(status) return(status) ;

  return(0) ;
}


/*********************************************************************/
/*								     */
/*          Проверка степеней подвижности звена по диапазонам        */

  int  RSS_Object_Flyer::iLinkDegrees(RSS_Object_Flyer_Link *link, 
                                                       char *title)

{
  char  text[1024] ;

/*-------------------------------------------- Контроль угла шарнира */

  if(link->name[0]=='a' ||
     link->name[0]=='e' ||
     link->name[0]=='r'   )
   if(link->value_fixed==0 &&
      link->value_test ==1   ) {

     if(iAngleTest(link->value, 
                   link->value_min,
                   link->value_max )) {

        if(title!=NULL) {
         if(this->ErrorEnable) {
               sprintf(text, "%s, %s - нарушение диапазона угла", 
                                title, link->name) ;
            SEND_ERROR(text) ;
                               }
                        }
                                            return(-1) ;
                                      }

                               }
/*--------------------------------------------- Контроль длины звена */

  if(link->name[0]=='x' ||
     link->name[0]=='y' ||
     link->name[0]=='z'   )
   if(link->value_fixed==0 &&
      link->value_test ==1   ) {

     if(link->value<link->value_min ||
        link->value>link->value_max   ) {

        if(title!=NULL) {
         if(this->ErrorEnable) {
               sprintf(text, "%s, %s - нарушение диапазона длины", 
                                title, link->name) ;
            SEND_ERROR(text) ;
                               }
                        }
                                            return(-1) ;
                                        }

                              }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               Проверка соответствия угла диапазону                */

  int  RSS_Object_Flyer::iAngleTest(double  value, 
                                    double  value_min, 
                                    double  value_max )

{

    if(value_min<=value_max) {
	 if(value<value_min ||
	    value>value_max   ) return(1) ;
			     }
    else                     {
	 if(value>value_min &&
	    value<value_max   ) return(1) ;
			     }
  return(0) ;
}


/********************************************************************/
/*								    */
/*		      Работа с целевыми точками   		    */
/*								    */
/*   Return: число целевых точек                                    */

     int  RSS_Object_Flyer::vGetTargetPoint(RSS_Point *points)

{
      *points=this->target ;

    return(1) ;
}

    void  RSS_Object_Flyer::vSetTargetPoint(RSS_Point *points, int  joints_cnt)

{
      target=points[0] ;
}


/********************************************************************/
/*								    */
/*                 Работа с неопределенностями схемы                */

    int  RSS_Object_Flyer::vGetAmbiguity(void)
{
   return(0) ;
}


   void  RSS_Object_Flyer::vSetAmbiguity(char *spec)
{
}


/********************************************************************/
/*								    */
/*           Разрешить конфигурацию объекта по целевой точке        */

     int  RSS_Object_Flyer::vSolveByTarget(void)

{ 
        double  x_tgt ;   /* Координаты целевой точки */
        double  y_tgt ;   
        double  z_tgt ;
        double  a_ang ;   /* Углы подхода к целевой точке */
        double  e_ang ;
        double  r_ang ;

     RSS_Point  target_prv ;         /* Целевая точка, сохраненное значение */
           int  status ;

/*---------------------- Привод угла возвышения в диапазон -90...+90 */

    if(this->target.elev> 90. ) {  this->target.elev = 180.-this->target.elev ;
                                   this->target.azim+= 180. ;
                                   this->target.roll+= 180. ;       }
    if(this->target.elev<-90. ) {  this->target.elev =-180.-this->target.elev ;
                                   this->target.azim+= 180. ;
                                   this->target.roll+= 180. ;       }

    if(this->target.elev> 89.9)    this->target.elev =  89.9 ;
    if(this->target.elev<-89.9)    this->target.elev = -89.9 ;

/*------------------------- Перевод точки в систему координат робота */

           target_prv=this->target ;

                x_tgt=this->target.x-this->x_base ;
                y_tgt=this->target.y-this->y_base ;
                z_tgt=this->target.z-this->z_base ;

                a_ang=this->target.azim*_GRD_TO_RAD ;
                e_ang=this->target.elev*_GRD_TO_RAD ;
                r_ang=this->target.roll*_GRD_TO_RAD ;

/*----------------------- Определение координат степеней подвижности */

                this->link_x   .value=x_tgt ;
                this->link_y   .value=y_tgt ;
                this->link_z   .value=z_tgt ;

                this->link_azim.value=a_ang*_RAD_TO_GRD ;
                this->link_elev.value=e_ang*_RAD_TO_GRD ;
                this->link_roll.value=r_ang*_RAD_TO_GRD ;

/*------------------------------------------------ Перерасчет робота */

              this->iRobotSkeleton() ;
       status=this->iRobotDegrees (1) ;                             /* Проверка диапазонов степеней подвижности */
    if(status)  return(_RSS_OBJECT_DEGREES_LIMIT) ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/********************************************************************/
/*								    */
/*           Разрешить конфигурацию объекта по сочленениям          */

     int  RSS_Object_Flyer::vSolveByJoints(void)

{ 
     Matrix2d  Summary ;
     Matrix2d  Local ;
       double  angle ;
          int  status ;

// static int  attempt ;
//       char  text[1024] ;

//                                                                           attempt++ ;
//              sprintf(text, "RSS_Object_Flyer::vSolveByJoints.0 - %d, %d", attempt, _heapchk()) ;
//               iDebug(text, NULL) ;

/*---------------------------------------------------- Инициализация */

                Summary.LoadE(4, 4) ;

/*---------------------------------------------------- Базовое звено */

                  Local.Load4d_base(this->x_base,
                                    this->y_base,
                                    this->z_base ) ;

                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_azim(-this->a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_elev(-this->a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;

                  Local.Load4d_roll(-this->a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;

/*---------------------------------------------------------- X-звено */

                  Local.Load4d_base(link_x.value, 0., 0.) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_x, &Summary) ;

/*---------------------------------------------------------- Y-звено */

                  Local.Load4d_base(0., link_y.value, 0.) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_y, &Summary) ;

/*---------------------------------------------------------- Z-звено */

                  Local.Load4d_base(0., 0., link_z.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_z, &Summary) ;

/*------------------------------------------------------- AZIM-звено */

                  Local.Load4d_azim(-link_azim.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_azim, &Summary) ;

/*------------------------------------------------------- ELEV-звено */

                  Local.Load4d_elev(-link_elev.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_elev, &Summary) ;

/*------------------------------------------------------- ROLL-звено */

                  Local.Load4d_roll(-link_roll.value) ;
                Summary.LoadMul    (&Summary, &Local) ;

                      iLinkSkeleton(&link_roll, &Summary) ;

/*----------------------------------------- Извлечение целевой точки */

        this->target.x   =Summary.GetCell(0, 3) ;
        this->target.y   =Summary.GetCell(1, 3) ;
        this->target.z   =Summary.GetCell(2, 3) ;

                    angle=Summary.GetCell(0, 2) ;
                    angle=Summary.GetCell(2, 2) ;

                    angle=atan2(Summary.GetCell(0, 2),
                                Summary.GetCell(2, 2))*_RAD_TO_GRD ;
        this->target.azim=180.+angle ;

             Local.Load4d_azim(angle) ;                             /* Доворот векторов ориентации по азимуту */
           Summary.LoadMul    (&Local, &Summary) ;

                    angle=atan2(Summary.GetCell(1, 2),
                                Summary.GetCell(2, 2))*_RAD_TO_GRD ;
        this->target.elev=-angle ;

             Local.Load4d_elev(angle) ;                             /* Доворот векторов ориентации по тангажу */
           Summary.LoadMul    (&Local, &Summary) ;

                    angle=atan2(Summary.GetCell(0, 1),
                                Summary.GetCell(1, 1))*_RAD_TO_GRD ;
        this->target.roll=angle ;

/*------------------------- Приведение углов в канонический диапазон */

//     while(link_azim.value<-180.)  link_azim.value+=360. ;
//     while(link_azim.value> 180.)  link_azim.value-=360. ;
//     while(link_elev.value<-180.)  link_elev.value+=360. ;
//     while(link_elev.value> 180.)  link_elev.value-=360. ;
//     while(link_roll.value<-180.)  link_roll.value+=360. ;
//     while(link_roll.value> 180.)  link_roll.value-=360. ;

/*------------------------------------------------ Перерасчет робота */

              this->iRobotSkeleton() ;
       status=this->iRobotDegrees (0) ;                             /* Проверка диапазонов степеней подвижности */
    if(status)  return(_RSS_OBJECT_DEGREES_LIMIT) ;

/*-------------------------------------------------------------------*/

//              sprintf(text, "RSS_Object_Flyer::vSolveByJoints.E - %d, %d", attempt, _heapchk()) ;
//               iDebug(text, NULL) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      Работа с сочленениями       		    */
/*								    */
/*   Return: число сочленений                                       */

     int  RSS_Object_Flyer::vGetJoints(RSS_Joint *joints)

{
 RSS_Object_Flyer_Link *link ;
                   int  i ;


   for(i=0 ; i<6 ; i++) {

           if(i==0)  link=&this->link_x ;
      else if(i==1)  link=&this->link_y ;
      else if(i==2)  link=&this->link_z ;
      else if(i==3)  link=&this->link_azim ;
      else if(i==4)  link=&this->link_elev ;
      else if(i==5)  link=&this->link_roll ;

           if(i>2)  joints[i].type     = _A_TYPE ;
           else     joints[i].type     = _L_TYPE ;

                    joints[i].value    =link->value ;
                    joints[i].value_min=link->value_min ;
                    joints[i].value_max=link->value_max  ;
                    joints[i].fixed    =link->value_fixed ;
                    joints[i].bounded  =link->value_test ;

            strncpy(joints[i].name,     link->name, sizeof(joints[i].name)-1) ;
                    joints[i].contexts =link->contexts ;
                        }     

    return(6) ;
}


    void  RSS_Object_Flyer::vSetJoints(RSS_Joint *joints, 
                                             int  joints_cnt)
{
 RSS_Object_Flyer_Link *link ;
                   int  i ;


   for(i=0 ; i<joints_cnt ; i++) {


           if(i==0)  link=&this->link_x ;
      else if(i==1)  link=&this->link_y ;
      else if(i==2)  link=&this->link_z ;
      else if(i==3)  link=&this->link_azim ;
      else if(i==4)  link=&this->link_elev ;
      else if(i==5)  link=&this->link_roll ;
      else            break ;

                   link->value    =joints[i].value ;
                   link->value_min=joints[i].value_min ;
                   link->value_max=joints[i].value_max ;

                   link->contexts =joints[i].contexts ;
                                 }     
}
/********************************************************************/
/*								    */
/*		        Записать данные в строку		    */

    void  RSS_Object_Flyer::vWriteSave(std::string *text)

{
  RSS_Object_Flyer_Link *link ; 
  RSS_Object_Flyer_Body *body ; 
  RSS_Object_Flyer_Mass *mass ; 
            std::string  data ;
                   char  field[1024] ;
                    int  i ;
                    int  j ;
                    int  k ;

/*----------------------------------------------- Заголовок описания */

     *text="#BEGIN OBJECT FLYER\n" ;

/*----------------------------------------------------------- Данные */

       sprintf(field, "NAME=%s\n", this->Name) ;  *text+=field ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - База */
       sprintf(field, "X_BASE=%.10lf\n", this->x_base    ) ;  *text+=field ;
       sprintf(field, "Y_BASE=%.10lf\n", this->y_base    ) ;  *text+=field ;
       sprintf(field, "Z_BASE=%.10lf\n", this->z_base    ) ;  *text+=field ;
       sprintf(field, "A_AZIM=%.10lf\n", this->a_azim    ) ;  *text+=field ;
       sprintf(field, "A_ELEV=%.10lf\n", this->a_elev    ) ;  *text+=field ;
       sprintf(field, "A_ROLL=%.10lf\n", this->a_roll    ) ;  *text+=field ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Звенья */
   for(i=0 ; i<8 ; i++) {

                      if(i==0)  link=&this->link_base ;             /* Определяем ссылку на описание звена */
                 else if(i==1)  link=&this->link_x ;
                 else if(i==2)  link=&this->link_y ;
                 else if(i==3)  link=&this->link_z ;
                 else if(i==4)  link=&this->link_azim ;
                 else if(i==5)  link=&this->link_elev ;
                 else if(i==6)  link=&this->link_roll ;
                 else if(i==7)  link=&this->link_head ;

       sprintf(field, "L%d_VALUE=%.10lf\n",     i, link->value     ) ;  *text+=field ;
       sprintf(field, "L%d_VALUE_KEYS=%d\n",    i, link->value_keys) ;  *text+=field ;
       sprintf(field, "L%d_VALUE_MIN=%.10lf\n", i, link->value_min ) ;  *text+=field ;
       sprintf(field, "L%d_VALUE_MAX=%.10lf\n", i, link->value_max ) ;  *text+=field ;
       sprintf(field, "L%d_VALUE_TEST=%d\n",    i, link->value_test) ;  *text+=field ;

       sprintf(field, "L%d_X_BASE=%.10lf\n",    i, link->x_base    ) ;  *text+=field ;
       sprintf(field, "L%d_Y_BASE=%.10lf\n",    i, link->y_base    ) ;  *text+=field ;
       sprintf(field, "L%d_Z_BASE=%.10lf\n",    i, link->z_base    ) ;  *text+=field ;
       sprintf(field, "L%d_A_AZIM=%.10lf\n",    i, link->a_azim    ) ;  *text+=field ;
       sprintf(field, "L%d_A_ELEV=%.10lf\n",    i, link->a_elev    ) ;  *text+=field ;
       sprintf(field, "L%d_A_ROLL=%.10lf\n",    i, link->a_roll    ) ;  *text+=field ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - Тела звеньев */
   for(i=0 ; i<8 ; i++) {

                      if(i==0)  link=&this->link_base ;             /* Определяем ссылку на описание звена */
                 else if(i==1)  link=&this->link_x ;
                 else if(i==2)  link=&this->link_y ;
                 else if(i==3)  link=&this->link_z ;
                 else if(i==4)  link=&this->link_azim ;
                 else if(i==5)  link=&this->link_elev ;
                 else if(i==6)  link=&this->link_roll ;
                 else if(i==7)  link=&this->link_head ;

     for(j=0 ; j<_BODIES_MAX ; j++) {

              body=&link->bodies[j] ;
          if(!body->use_flag)  continue ;

       sprintf(field, "B%d_%d_DESC=%s\n",       i, j, body->desc) ;        *text+=field ;
       sprintf(field, "B%d_%d_X_BASE=%.10lf\n", i, j, body->x_base    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_Y_BASE=%.10lf\n", i, j, body->y_base    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_Z_BASE=%.10lf\n", i, j, body->z_base    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_A_AZIM=%.10lf\n", i, j, body->a_azim    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_A_ELEV=%.10lf\n", i, j, body->a_elev    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_A_ROLL=%.10lf\n", i, j, body->a_roll    ) ;  *text+=field ;
       sprintf(field, "B%d_%d_ZERO=%.10lf\n",   i, j, body->zero_size ) ;  *text+=field ;

       sprintf(field, "B%d_%d_MODEL=%s\n",      i, j, body->model.path) ;  *text+=field ;
  
     for(k=0 ; k<_MODEL_PARS_MAX ; k++)
        if(body->model.pars[k].value[0]) {
            sprintf(field, "B%d_%d_PAR_%d=%s\n", 
                                   i, j, k, body->model.pars[k].value) ;  *text+=field ;
                                         }

                                    }
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  Массы звеньев */
   for(i=0 ; i<8 ; i++) {

                      if(i==0)  link=&this->link_base ;             /* Определяем ссылку на описание звена */
                 else if(i==1)  link=&this->link_x ;
                 else if(i==2)  link=&this->link_y ;
                 else if(i==3)  link=&this->link_z ;
                 else if(i==4)  link=&this->link_azim ;
                 else if(i==5)  link=&this->link_elev ;
                 else if(i==6)  link=&this->link_roll ;
                 else if(i==7)  link=&this->link_head ;

     for(j=0 ; j<_BODIES_MAX ; j++) {

              mass=&link->masses[j] ;
          if(!mass->use_flag)  continue ;

       sprintf(field, "M%d_%d_DESC=%s\n",       i, j, mass->desc      ) ;  *text+=field ;
       sprintf(field, "M%d_%d_MASS=%.10lf\n",   i, j, mass->mass      ) ;  *text+=field ;
       sprintf(field, "M%d_%d_J_X=%.10lf\n",    i, j, mass->Jx        ) ;  *text+=field ;
       sprintf(field, "M%d_%d_J_Y=%.10lf\n",    i, j, mass->Jy        ) ;  *text+=field ;
       sprintf(field, "M%d_%d_J_Z=%.10lf\n",    i, j, mass->Jz        ) ;  *text+=field ;
       sprintf(field, "M%d_%d_X_BASE=%.10lf\n", i, j, mass->x_base    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_Y_BASE=%.10lf\n", i, j, mass->y_base    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_Z_BASE=%.10lf\n", i, j, mass->z_base    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_A_AZIM=%.10lf\n", i, j, mass->a_azim    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_A_ELEV=%.10lf\n", i, j, mass->a_elev    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_A_ROLL=%.10lf\n", i, j, mass->a_roll    ) ;  *text+=field ;
       sprintf(field, "M%d_%d_MOVE=%.10lf\n",   i, j, mass->move_coef ) ;  *text+=field ;

                                    }
                        }
/*- - - - - - - - - - - - - - - - - - - - - - - -  Контексты модулей */
#define  C link->contexts

   for(i=0 ; i<8 ; i++) {

                      if(i==0)  link=&this->link_base ;             /* Определяем ссылку на описание звена */
                 else if(i==1)  link=&this->link_x ;
                 else if(i==2)  link=&this->link_y ;
                 else if(i==3)  link=&this->link_z ;
                 else if(i==4)  link=&this->link_azim ;
                 else if(i==5)  link=&this->link_elev ;
                 else if(i==6)  link=&this->link_roll ;
                 else if(i==7)  link=&this->link_head ;

        if(C==NULL)  continue ;

     for(j=0 ; C[j]!=NULL ; j++) {
               C[j]->module->vWriteData(C[j], &data) ;
       sprintf(field, "C%d_%d=%s\n", i, j, data.c_str()) ;  *text+=field ;
                              }
                        }

#undef    C

/*------------------------------------------------ Концовка описания */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                   Отладочная печать в файла                      */

   void  RSS_Object_Flyer::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Object_Flyer::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         

    if(        ProgramModule.debug_flag)  return ;
    if(stricmp(ProgramModule.debug_list, "ALL"    )!=  0  &&
        strstr(ProgramModule.debug_list, "O_Flyer")==NULL   )  return ;

    if(path==NULL) {
                             path="o_flyer.log" ;
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
/********************************************************************/
/**							           **/
/**		  ВСПОМОГАТЕЛЬНЫЕ ПРОЦЕДУРЫ                        **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*           Получение ссылки на программный модуль                 */

   RSS_Kernel *GetObjectFlyerEntry(void)

{
	return(&ProgramModule) ;
}

 
