/********************************************************************/
/*								    */
/*		������ ������ "������ ������������"    		    */
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
/*		    	����������� ������                          */

     static   RSS_Module_Service  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

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
/**	       �������� ������ ������ "����� � �����"              **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Service_instr  RSS_Module_Service_InstrList[]={

 { "help",       "?",  "#HELP (?) - ������ ��������� ������", 
                        NULL,
                       &RSS_Module_Service::cHelp   },
 { "list",       "l",  "#LIST (L) - �������������� �������� ��������� ������", 
                        NULL,
                       &RSS_Module_Service::cList   },
 { "export",     "e",  "#EXPORT (E) - �������� �������� ��������� ������ � ����", 
                       " EXPORT <��� �����>\n",
                       &RSS_Module_Service::cExport },
 { "import",     "i",  "#IMPORT (E) - �������� �������� ��������� ������ �� �����", 
                       " IMPORT <��� �����>\n",
                       &RSS_Module_Service::cImport },
 { "run",        "r",  "#RUN (R) - ���������� ��������� ������", 
                       " RUN             - ���������� ������� ��������� ������\n"
                       " RUN <��� �����> - ���������� ��������� ������ �� �����\n",
                       &RSS_Module_Service::cRun },
 { "vars",       "v",  "#VARS (V) - ����������� ���� ����������", 
                        NULL,
                       &RSS_Module_Service::cVariables },
 {  NULL }
                                                              } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

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
/*		       ����������� ������			    */

     RSS_Module_Service::RSS_Module_Service(void)

{
                keyword="Geo" ;
         identification="Service_task" ;

             mInstrList=RSS_Module_Service_InstrList ;

//             mLineAcc=  0. ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Service::~RSS_Module_Service(void)

{
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Service::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Service::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

/*--------------------------------------------- ������������� ������ */

             memset(command, 0, sizeof(command)) ;
            strncpy(command, cmd, sizeof(command)-1) ;
/*- - - - - - - - - - - - - - - - - - - -  ��������� �������� ������ */
        if(command[0]=='&') {
                                end=command+1 ;
                            }
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������ ������ */
   else if(!direct_command) {

         end=strchr(command, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }

      if(stricmp(command, _SECTION_FULL_NAME) &&
         stricmp(command, _SECTION_SHRT_NAME)   )  return(1) ;
                            }
/*- - - - - - - - - - - - - - - - - - - -  ��������� �������� ������ */
   else                     {
                                end=command ;
                            }
/*----------------------- ���������/���������� ������ ������ ������� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
   if(end!=NULL && !strcmp(end, "..")) {

                            direct_command=0 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"") ;
        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DIRECT_COMMAND, (LPARAM)"") ;

                                           return(0) ;
                                       }
/*--------------------------------------------- ��������� ���������� */

       instr=end ;                                                  /* �������� ����� � ��������� ������� */

     for(end=instr ; *end!= 0  &&
                     *end!=' ' &&
                     *end!='>' &&
                     *end!='.' &&
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd, iface) ;            /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ SERVICE: ����������� �������") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end, iface) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Service::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
                       char *work ;
                       char *end ;
                        int  i ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE SERVICE\n", 
                      strlen("#BEGIN MODULE SERVICE\n")) )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Service::vReadSave", 0, 0) ;

       strcpy(buff, data->c_str()) ;

/*-------------------------------------------------- ������ �������� */

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
              SEND_ERROR("������ SERVICE: ������������ ������") ;
  			       return ;
                         }

             memset(&mScenario[mScenario_cnt], 0, sizeof(FRAME)) ;
             memcpy( mScenario[mScenario_cnt].mark,    work  ,   2) ;
             memcpy( mScenario[mScenario_cnt].action,  work+ 2, 16) ;
            strncpy( mScenario[mScenario_cnt].command, work+18, sizeof(mScenario->command)-1) ;
                               mScenario_cnt++ ; 
                                        }                           /* CONTINUE.0 */
/*------------------------- �������������� ��������� ������ �������� */

    for(i=0 ; i<mScenario_cnt ; i++) {
                 end=strchr(mScenario[i].action, ' ') ;
              if(end!=NULL)  *end=0 ;
                                     }
/*-------------------------------------------- ������������ �������� */

                        gMemFree(buff) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Service::vWriteSave(std::string *text)

{
  char  value[1024] ;
   int  i ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE SERVICE\n" ;

/*-------------------------------------------------- ������ �������� */

   for(i=0 ; i<this->mScenario_cnt ; i++)
     if(        this->mScenario[i].action[0]!=0   &&
        stricmp(this->mScenario[i].action, "NULL")  ) {
            
          sprintf(value, "%2.2s%-16.16s%s\n", 
                                  this->mScenario[i].mark,
                                  this->mScenario[i].action,
                                  this->mScenario[i].command) ;
           *text+=value ;
                                                      }
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Service::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - �������� ������ ������������", "SERVICE") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ��������� �������� � ��������� ������                */

    void  RSS_Module_Service::vChangeContext(void)

{
/*------------------------------------------ ����� ������ ���������� */

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
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Service::cHelp(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Service_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� LIST                    */

  int  RSS_Module_Service::cList(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			    (LPCDLGTEMPLATE)Resource("IDD_EDIT", RT_DIALOG),
			       GetActiveWindow(), Task_Service_Edit_dialog, (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� EXPORT                  */

  int  RSS_Module_Service::cExport(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

       char *pars[_PARS_MAX] ;
       char  path[FILENAME_MAX] ;
       FILE *file ;
       char *end ;
       char  tmp[1024] ;
        int  i ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
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

/*--------------------------------------------- �������� ����� ����� */

    if(pars[0]==NULL) {                                             /* ���� ��� ����� �� ������... */
                    SEND_ERROR("�� ������ ��� �����.\n"
                              "��������: EXPORT <���_�����> ...") ;
                                     return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* ���� ��� �� �������� ���������� - */
         if(       end       ==NULL ||                              /*   - ��������� �������             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".service") ;

/*---------------------------------------- �������� ������� �������� */

     if(         this->mScenario_cnt==0 ||
        (        this->mScenario_cnt==1            &&
         stricmp(this->mScenario[0].action, "NULL")  )) {
               SEND_ERROR("�� ��������� ��������� ������ ��������") ;
                          return(-1) ;
                                                        }
/*----------------------------------------------- ������������ ����� */

        file=fopen(path, "wt") ;                                    /* ��������� ����� */
     if(file==NULL) {
                         sprintf(tmp, "������ �������� �����: %s", path) ;
                      SEND_ERROR(tmp) ;
                          return(-1) ;
                    }

   for(i=0 ; i<this->mScenario_cnt ; i++)                           /* ����� ������ �������� */
     if(        this->mScenario[i].action[0]!=0   &&
        stricmp(this->mScenario[i].action, "NULL")  ) {
            
          sprintf(tmp, "%2.2s%-16.16s%s\n", 
                                  this->mScenario[i].mark,
                                  this->mScenario[i].action,
                                  this->mScenario[i].command) ;
          fwrite(tmp, 1, strlen(tmp), file) ;
                                                      }

             fclose(file) ;                                         /* ��������� ����� */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� IMPORT                  */

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

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
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

/*--------------------------------------------- �������� ����� ����� */

    if(pars[0]==NULL) {                                             /* ���� ��� ����� �� ������... */
                           sprintf(tmp, "[Service.cImport] �� ������ ��� �����. ��������: IMPORT <���_�����>") ;
                        SEND_ERROR(tmp) ;
                    iface->vSignal("FATAL", tmp) ;
                            return(-1) ;
                      }

                        strcpy(path, pars[0]) ;
                   end=strrchr(path, '.') ;                         /* ���� ��� �� �������� ���������� - */
         if(       end       ==NULL ||                              /*   - ��������� �������             */
            strchr(end, '/' )!=NULL ||
            strchr(end, '\\')!=NULL   )  strcat(path, ".service") ;

/*------------------------------------------------- ���������� ����� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� ����� */
        file=fopen(path, "rt") ;                                    /* ��������� ����� */
     if(file==NULL) {
                           sprintf(tmp, "[Service.cImport] ������ �������� �����: %s", path) ;
                        SEND_ERROR(tmp) ;
                    iface->vSignal("FATAL", tmp) ;
                          return(-1) ;
                    }
/*- - - - - - - - - - - - - - - - - - - - -  ����������� ����� ����� */
          for(cnt=0 ; ; cnt++) {
                  end=fgets(tmp, sizeof(tmp)-1, file) ;             /* ��������� ��������� ������ */
               if(end==NULL)  break ;                               /* ���� ��� �������... */
                               }
/*- - - - - - - - - - - - - - - - - - - -  ���������� ������� ������ */
        this->mScenario=(FRAME *)
                          this->gMemRealloc(this->mScenario, cnt*sizeof(FRAME), 
                                             "RSS_Module_Service::mScenario", 0, 0) ;
     if(this->mScenario==NULL) {
                   sprintf(tmp, "[Service.cImport] ������������ ������") ;
                SEND_ERROR(tmp) ;
            iface->vSignal("FATAL", tmp) ;
                 return(-1) ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
                     rewind(file) ;                                 /* ������������ ���� �� ������ */

       for(this->mScenario_cnt=0  ; ; ) {                           /* CIRCLE.1 */

                     memset(tmp, 0, sizeof(tmp)) ;
                  end=fgets(tmp, sizeof(tmp)-1, file) ;             /* ��������� ��������� ������ */
               if(end==NULL)  break ;                               /* ���� ��� �������... */

                  end=strchr(tmp, '\n') ;                           /* ������� ����-������� */
               if(end!=NULL)  *end=0 ;

            for(space=1, i=0, j=0 ; tmp[i] ; i++ ) {                /* ������� ���������, ��������  */
              if(tmp[i]==' ') space++ ;                             /*  � ������ �������            */
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

              if(stricmp(tmp, "break"   ) &&                        /* ��������� ������������ ��������� ����� */
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
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� ����� */
             fclose(file) ;                                         /* ��������� ����� */

/*-------------------------------------------------------------------*/

                    iface->vSignal("DONE", "") ;                    /* ������ ��������� �� ���������� ������������ ����� */

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� RUN                     */

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

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
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

/*----------------------------------------- �������� ������ �� ����� */

    if(pars[0]!=NULL) {                                             /* ���� ������ ��� �����... */

           status=cImport(pars[0], iface) ;                         /* ��������� ��������� ������ �� ����� */
        if(status)  return(-1) ;                                    /* ���� ������ �������� */
                      }
/*------------------------------------------- ���������� ����������� */

                 this->mVariables_cnt=0 ;
                 this->mExecFail     =1 ;

/*----------------------------------------------- ��������� �������� */

                                     iface->vClear() ;              /* ����� ������ ������������� ���������� */

   for(n=0 ; n<mScenario_cnt ; ) {                                  /* CIRCLE.1 */

            status=iFrameExecute(&mScenario[n], &next, result) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
         if(status==   _EXIT_FRAME   ) {

                              iface->vPass(&this->mExecIFace) ;
                                                 break ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - ������ � ������� */
         if(status==  _ERROR_FRAME   ) {

                              iface->vSignal("FATAL", result) ;
                                                  break ;
                                       }
/*- - - - - - - - - - - - - - - - - - - ������� � ���������� "�����" */
         if(status==   _NEXT_FRAME   ) {
                                         n++ ;  continue ;
                                       }
/*- - - - - - - - - - - - - - - - - ������� � �������� "�����" ����� */
         if(status==_GOTO_UP_FRAME   ) {

            for(n-- ; n>=0 ; n--) 
              if(         mScenario[n].mark[0]!='/'          &&
                 !stricmp(mScenario[n].action,  next.action) &&
                 !memicmp(mScenario[n].command, next.command, 
                                         strlen(next.command)) )  break ;

                if(n<0) {
                          sprintf(result, "[Service.cRun] �� ������ ������� ��������") ;
                       SEND_ERROR(result) ;
                   iface->vSignal("FATAL", result) ;
                            break ;
                        }
                                            continue ;
                                       }
/*- - - - - - - - - - - - - - - - -  ������� � �������� "�����" ���� */
         if(status==_GOTO_DN_FRAME ||
            status==_JUMP_DN_FRAME   ) {

            for(n++ ; n<mScenario_cnt ; n++)
              if(         mScenario[n].mark[0]!='/'          &&
                 !stricmp(mScenario[n].action,  next.action) &&
                 !memicmp(mScenario[n].command, next.command, 
                                         strlen(next.command)) )  break ;

                if(n>=mScenario_cnt) {
                          sprintf(result, "[Service.cRun] �� ������ ������� ��������") ;
                       SEND_ERROR(result) ;
                   iface->vSignal("FATAL", result) ;
                            break ;
                                     }

                if(status==_JUMP_DN_FRAME)  n++ ;

                                            continue ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }                                  /* CONTINUE.1 */

/*------------------------- ��������� ���������� ���������� �������� */


/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� VARIABLES               */

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
/*              ���������� ���������� ��������� ������              */
/*                                                                  */
/*  rem          -  �����������                                     */
/*  return       -  ���������� ��������� ������                     */
/*  declare      -  ���������� ����������                           */
/*  formula      -  ���������� ���������                            */
/*  execute      -  ���������� �������                              */
/*  if_done      -  ��������� "��������" ���������� ������.�������  */
/*  if_error     -  ��������� "�������" ���������� ������.�������   */
/*  for          -  ���� ��������                                   */
/*  next         -  ����������� ��������                            */
/*  break        -  ���������� ��������                             */

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

/*---------------------------------------------------- ������������� */

          sprintf(tmp, "Scenario> %s %s", cmd->action, cmd->command) ;
   this->iLogFile(tmp) ;

/*------------------------------------------------------- ���������� */

          Calc=iGetCalculator() ;                                   /* ����������� ����������� */
       if(Calc==NULL) {
                         sprintf(result, "[Service.cRun] ���������� ��������� ����������� ���������") ;
                      SEND_ERROR(result) ;
                        return(_ERROR_FRAME) ;
                      }
/*------------------------------------------- ��������������� ������ */

       if(!stricmp(cmd->mark, "//"))  return(_NEXT_FRAME) ;         /* ���� ������ ��������������... */

/*----------------------------------------------------- �������� REM */

       if(!stricmp(cmd->action, "rem"    )) {

                                                return(_NEXT_FRAME) ;
                                            }
/*-------------------------------------------------- �������� RETURN */

  else if(!stricmp(cmd->action, "return" )) {
                                               return(_EXIT_FRAME) ;
                                            }
/*------------------------------------------------- �������� DECLARE */

  else if(!stricmp(cmd->action, "declare")) {
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
                 memset(tmp, 0, sizeof(tmp)) ;
                strncpy(tmp, cmd->command, sizeof(tmp)-1) ;         /* ��������� ������ �� ������� */

             end=strtok(tmp, " \t/.;:") ;                           /* ��������� �� ������ ������������ */
//               strupr(tmp) ;                                      /* ��������� � ������� ������� */

         if(*tmp==0) {                                              /* ��������� ������� ����� ���������� */
                 sprintf(result, "[Service.cRun] ���������� ��� ����������: %s%s %s",
                                    cmd->mark, cmd->action, cmd->command ) ;
              SEND_ERROR(result) ;
                    return(_ERROR_FRAME) ;
                     }
/*- - - - - - - - - - - - - - - - - - - - - -  �������� ������������ */
         for(i=0 ; i<mVariables_cnt ; i++)                          /* ��������� �� ��������� ��� */
           if(!stricmp(mVariables[i].name, tmp))  break ;

         if(i<mVariables_cnt) {                                     /* ���� ����� ��� ��� ������������... */
                 sprintf(result, "[Service.cRun] ���������� ��� ����������: %s%s %s",
                                    cmd->mark, cmd->action, cmd->command ) ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
        mVariables=(VAR *)
                    gMemRealloc(mVariables, 
                                (mVariables_cnt+1)*sizeof(VAR), 
                                 "RSS_Module_Service::mVariables", 0, 0) ;
     if(mVariables==NULL) {
                 sprintf(result, "[Service.cRun] ������������ ������") ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                          }

             memset(&mVariables[mVariables_cnt], 0, sizeof(mVariables[0])) ;
            strncpy( mVariables[mVariables_cnt].name, tmp, sizeof(mVariables[0].name)-1) ;
                                mVariables_cnt++ ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ���� ���������� */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            return(_NEXT_FRAME) ;
                                            }
/*------------------------------------------------- �������� FORMULA */

  else if(!stricmp(cmd->action, "formula")) {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������ ������� */
                 memset(tmp, 0, sizeof(tmp)) ;
                strncpy(tmp, cmd->command, sizeof(tmp)-1) ;         /* ��������� ������ �� ������� */ 

             end=strchr(tmp, '=') ;                                 /* ���� �������� ���������� */
          if(end==NULL) {
                            result_name=NULL ;
                                formula=tmp ;
                        }       
          else          {
                            result_name=tmp  ;
                                formula=end+1 ;
                                   *end= 0   ;
                        }
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
          if(mVariables_cnt) {

                vars_list=(struct RSS_Parameter *)
                             calloc(mVariables_cnt+2, sizeof(*vars_list)) ;

             for(i=0 ; i<mVariables_cnt ; i++) {
                   strncpy(vars_list[i].name, mVariables[i].name, sizeof(vars_list[i].name)-1) ;
                           vars_list[i].value=strtod(mVariables[i].value, &end) ;
                                               }

            if(!mExecFail) {                                        /* ���� ���� ������� �� ���������� ���������� ������... */
                   strncpy(vars_list[i].name, "VALUE", sizeof(vars_list[i].name)-1) ;
                           vars_list[i].value=mExecValue ;
                           }
                             } 
          else               {                                      /* ���� ���������� ��� */
                                   vars_list=NULL ;
                             }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
                          calc_context=NULL ;

            status=Calc->vCalculate("STD_EXPRESSION",               /* ���������� �� ������� */
                                      formula, vars_list, NULL,
                                        &result_value,
                                        &calc_context, error) ;
         if(status==-1) {
                  sprintf(result, "[Service.cRun] ������ �����������\n\n"
                                  " %s\n %s",
                                   cmd->command, error) ;
               SEND_ERROR(result) ;
                   return(_ERROR_FRAME) ;
                        }

          Calc->vCalculate("CLEAR", NULL, NULL, NULL, NULL,         /* ������� ��������� ����������� */
                                       &calc_context, error) ;

        if(mVariables_cnt) free(vars_list) ;                        /* ������������ ������� ���������� */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
       if(result_name!=NULL) {

         for(i=0 ; i<mVariables_cnt ; i++)                          /* ���� ���������� ���������� */
           if(!stricmp(mVariables[i].name, result_name))  break ;

         if(i==mVariables_cnt) {                                    /* ��������� ������� ����� ���������� */
                 sprintf(result, "[Service.cRun] ���������� ���������� �� �������: %s\n%s",
                                   result_name, cmd->command ) ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                               }      

                   sprintf(mVariables[i].value, "%-40.20f", result_value) ;
                    strcpy(result, mVariables[i].value) ;
                             }
/*- - - - - - - - - - - - - - - - - - - - ���������� ���� ���������� */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                            return(_NEXT_FRAME) ;
                                            }
/*---------------------------------------- �������� EXECUTE � RESULT */

  else if(!stricmp(cmd->action, "execute" ) ||
          !stricmp(cmd->action, "result"  )   ) {
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
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
/*- - - - - - - - - - - - - - - - - - ��������������� ������-������� */
     do {                                                           /* BLOCK.1 */
                 memset(name, 0, sizeof(name)) ;
                strncpy(name, command, sizeof(name)-1) ;
             end=strchr(name, ' ') ;                                /* �������� ������ ����� ������� */
          if(end!=NULL)  *end=0 ;

        for(i=0 ; i<OBJECTS_CNT ; i++)                              /* ���� ������ �� ����� */
          if(!stricmp(OBJECTS[i]->Name, name))  break ;

          if(i==OBJECTS_CNT)  break ;                               /* ���� ������ ���... */

       do {                                                         /* ������ ���������� ��������� ������� */
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

           sprintf(tmp, "%s %s %s %s", OBJECTS[i]->Type,           /* ����������������� ������� */
                                        word2,
                                        name,
                                        word3            ) ;
            strcpy(command, tmp) ;

        } while(0) ;                                                /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
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
                 sprintf(result, "[Service.cRun] ��� ������ ��� ��������� �������") ;
              SEND_ERROR(result) ;
                  return(_ERROR_FRAME) ;
                         } 
/*- - - - - - - - - - - - - - - - - - - - - - - ��������� ���������� */
      if(!stricmp(cmd->action, "result")) {                         /* ���� ������ - ��������� ������� ��������� */
                       this->mExecIFace.vPass(&iface) ;             /*   ��� ��������                            */
                                          }
  
                     mExecValue=0. ;
                     mExecError=0 ;
                     mExecFail =0 ;

           if(!iface.vCheck( NULL)     )  mExecFail=1 ;             /* ���������� ������� �������� */
      else if( iface.vCheck("FATAL") ||                             /*  � �� ��������              */
               iface.vCheck("ERROR")   )  mExecError=1 ;

                         iface.vDecode(NULL, tmp) ;                 /* ��������� ����������� ������� */
                     mExecValue=strtod(tmp, &end) ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ���� ���������� */
     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        return(_NEXT_FRAME) ;
                                                }
/*----------------------------------------------------- �������� FOR */

  else if(!stricmp(cmd->action, "for" )) {
/*- - - - - - - - - - - - - - - - - - - - - - -  ������ ������������ */
              memset(name, 0, sizeof(name)) ;
              memset(tmp,  0, sizeof(tmp)) ;
             strncpy(name, cmd->command, sizeof(name)-1) ;
          end=strchr(name, '=') ;                                   /* �������� ��� ��������� ���������� */
       if(end==NULL) {
                       sprintf(result, "[Service.cRun] ������ ���������� ��������� FOR") ;
                    SEND_ERROR(result) ;
                        return(_ERROR_FRAME) ;
                     }
                          *end=0 ;
             strncpy(tmp,  end+1, sizeof(tmp)-1) ;                  /* �������� ��������� ��������� */        
/*- - - - - - - - - - - - - - - - - - ��������� ��������� ���������� */
       for(search_value=NULL, i=0 ; i<mVariables_cnt ; i++)         /* ���� ���������� �� ������ */
         if(!stricmp(mVariables[i].name, name)) {
                                search_value=mVariables[i].value ;
                                                    break ;
                                                }
         if(search_value==NULL) {
                sprintf(result, "[Service.cRun] ��������� ���������� ��������� FOR �� ���������") ;
             SEND_ERROR(result) ;
                return(_ERROR_FRAME) ;
                                }
/*- - - - - - - - - - - - - - - - - - -  ������ ���������� ��������� */
       for(object=NULL, i=0 ; i<OBJECTS_CNT ; i++)                  /* ��������� �� ����� �� � ��������� ��������� */
         if(!stricmp(OBJECTS[i]->Name, tmp)) {                      /*  ������ �������� �� ����������������        */
                                                object=OBJECTS[i] ;
                                                        break ;      
                                             }
/*- - - - - - - -  ���������� ������ �������� (��� ������� ��������) */
       if(object!=NULL) {                                           /* ��� ������� ������� �������� */
                          search_deep=object->vGetAmbiguity() ;

         if(search_deep==0) {                                       /* ���� ������ ������������ ����������... */
              if(search_value[0]==0)  strcpy(search_value, "0") ;   /*  ������ ���� ����� �� ����� */
              else                           search_deep=-1 ;
                            }
         else               {                                       /* ���� ������� ���������������... */
              if(search_value[0]==0)  n=0 ;                         /*  ���������� ����� ���������� ����� */ 
              else                    n=strtoul(search_value, &end, 2)+1 ;

              if(n>=(1<<search_deep)) {                             /*  ���� ��� ���� ����������... */
                                        search_deep=-1 ;
                                      }
              else                    {                             /*  ���������� ��� ��� ���������� ����... */
                       iGetBinary(search_value, search_deep, n) ;
                                      }
                            }
                        }
/*- - - - - - - - - - - - - - - - - - - - ���������� ���� ���������� */
       if(search_deep==-1)  search_value[0]=0 ;

     if(mVariables_Window!=NULL)
            SendMessage(mVariables_Window, WM_USER, 
                             (WPARAM)_USER_SHOW, (LPARAM)NULL) ;
/*- - - - - - - - - - - - - - - - - - -  ��������� ���������� ������ */
       if(search_deep==-1) {
                               strcpy(next->action, "NEXT") ;
                               strcpy(next->command, name) ;

                                 return(_JUMP_DN_FRAME) ;
                           } 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 return(_NEXT_FRAME) ;
                                         }
/*---------------------------------------------------- �������� NEXT */

  else if(!stricmp(cmd->action, "next" )) {

                   strcpy(next->action,  "FOR") ;
                  sprintf(next->command, "%s=", cmd->command) ;

                      return(_GOTO_UP_FRAME) ;

                                          }
/*--------------------------------------------------- �������� BREAK */

  else if(!stricmp(cmd->action, "break" )) {

                       strcpy(next->action, "NEXT") ;
                       strcpy(next->command, cmd->command) ;

                                 return(_JUMP_DN_FRAME) ;

                                          }
/*------------------------------------ �������� IF_ERROR ��� IF_DONE */

  else if(!stricmp(cmd->action, "if_error") || 
          !stricmp(cmd->action, "if_done" )   ) {

         if( mExecFail) {
                  sprintf(result, "[Service.cRun] ������������� �������� ��"
                                  " ������������ ��������� ������������ ������") ;
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
/*--------------------------------------------- ����������� �������� */

  else                                      {

                 sprintf(result, "[Service.cRun] ����������� ��������: %s%s %s", 
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
/*            ����������� ������� �����������                       */

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
/*                   ����� ����� � �������� ����                    */

  void  RSS_Module_Service::iGetBinary(char *text, int  digits, int  value)

{
   int  i ;
     
       memset(text, 0, digits+1) ;

  for(i=0 ; i<digits ; i++) 
    if((1<<i) & value)  text[digits-i-1]='1' ;
    else                text[digits-i-1]='0' ;
}
