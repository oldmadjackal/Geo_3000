/********************************************************************/
/*								    */
/*		������ ������ "����� ������� ����������" 	    */
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

#include "T_Path.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text)

#define  CREATE_DIALOG  CreateDialogIndirectParam

/*
#define      gMemFree(p1)                 free(p1)
#define    gMemCalloc(p1,p2,p3,p4,p5)   calloc(p1,p2)
#define   gMemRealloc(p1,p2,p3,p4,p5)  realloc(p1,p2)
*/

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

     static   RSS_Module_Path  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 T_PATH_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_PATH_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**	  �������� ������ ������ "����� ������� ����������"        **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Path_instr  RSS_Module_Path_InstrList[]={

 { "help",       "?",  "#HELP (?) - ������ ��������� ������", 
                        NULL,
                       &RSS_Module_Path::cHelp   },
 { "config",     "c",  "#CONFIG (C) - ��������� ��������� ���������", 
                        NULL,
                       &RSS_Module_Path::cConfig   },
 { "search",     "s",  "#SEARCH (S) - ����� ����������, ���������� �����\n"
                       "   �������� ����� �����/���������", 
                       " SEARCH[/DRQ][:<��� ����������>] <��� ������ �����> [<��� �������>]\n"
                       "     D - ������������ ������ � �������� ������\n"
                       "     R - ��������� ����� ��������� ����������\n"
                       "     Q - �� �������� �������� �� ��������� ����������\n",
                       &RSS_Module_Path::cSearch },
 { "trace",      "t",  "#TRACE (T) - ����������� �������� ����������", 
                       " TRACE[/DR] <��� ����������> [<��� �������>]\n"
                       "     D - ������������ ������ � �������� ��������\n"
                       "     R - �������� ���������� � �������� �������\n",
                       &RSS_Module_Path::cTrace },
 { "indicate",   "i",  "#INDICATE (I) - ��������� ����������", 
                       " INDICATE <��� ����������> [<��� �������>]\n",
                       &RSS_Module_Path::cIndicate },
 { "copy",       "cp", "#COPY - ������� ����� ����������",
                       " COPY[/OR] <���_�������> <���_�����>\n"
                       "     O - ���� ������ ���������� - ������������ ���\n"
                       "     R - ������� ����� �������� ����������\n",
                       &RSS_Module_Path::cCopy },
 { "color",      "co", "#COLOR   - ���������� ���� �������", 
                       " COLOR <��� ����������> <����>\n"
                       "     <����> - RED, GREEN, BLUE\n",
                       &RSS_Module_Path::cColor      },
 {  NULL }
                                                              } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Path_instr *RSS_Module_Path::mInstrList  =NULL ;
                            long  RSS_Module_Path::mDebug_stamp=  0 ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Path::RSS_Module_Path(void)

{
                keyword="Geo" ;
         identification="Path_task" ;

             mInstrList=RSS_Module_Path_InstrList ;

                  mDraw=  0 ;

       mRandomPerDirect=  1 ;
            mVectorsMax=100 ;
          mScanStepCoef=  3 ;
            mFailDirect= 20 ;
         mLineStep_user=  0. ;
        mAngleStep_user=  0. ;
          mLineAcc_user=  0. ;
         mAngleAcc_user=  0. ;

            mTraceColor=RGB(128, 0, 128) ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Path::~RSS_Module_Path(void)

{
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Path::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Path::vExecuteCmd(const char *cmd, RSS_IFace *iface)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char  redirect[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "PATH"
#define  _SECTION_SHRT_NAME   "PATH"

/*--------------------------------------------- ������������� ������ */

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
/*----------------------- ���������/���������� ������ ������ ������� */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
   if(end==NULL || end[0]==0) {
     if(!direct_command) {
                            direct_command=1 ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Path:") ;
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
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������������� */
#define   REDIR       this->command_redirect 
#define   REDIR_CNT   this->command_redirect_cnt 

       for(i=0 ; i<REDIR_CNT ; i++)                                 /* ���� ������� � ������ �������������,   */
         if(!stricmp(instr, REDIR[i].command)) {                    /*  ���� ������� - �������� �� ���������� */
                   sprintf(redirect, "%s %s %s", REDIR[i].master,   /*    � ������ �������������              */
                                                   instr, end) ;
               status=REDIR[i].module->vExecuteCmd(redirect) ;
                          return(status) ;
                                               }

#undef    REDIR
#undef    REDIR_CNT
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ������������� */
   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ PATH: ����������� �������") ;
                                            return(-1) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end, iface) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*                ���������� ��������� ������                       */

    void  RSS_Module_Path::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Path::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
            RSS_Object_Path *object ;
                 TRAJECTORY *trajectory ;
                       char  name[32] ;
                       char  title[256] ;
                       char  master[32] ;
                       char  field[32] ;
                        int  status ;
                       char *work ;
                       char *entry ;
                       char *value ;
                       char *end ;
                       char  desc[1024] ;
                       char  tmp[128] ;
                        int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

#define    T  trajectory
#define    D  trajectory->Path_degrees

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE PATH\n", 
                      strlen("#BEGIN MODULE PATH\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT PATH\n", 
                      strlen("#BEGIN OBJECT PATH\n"))   )  return ;

/*------------------------------------------------ ���������� ������ */

            buff_size=data->size()+16 ;
            buff     =(char *)
                        gMemCalloc(1, buff_size, 
                                   "RSS_Module_Path::vReadSave.buff", 0, 0) ;
     strcpy(buff, data->c_str()) ;

/*------------------------------------------------- ��������� ������ */

   if(!memicmp(buff, "#BEGIN MODULE PATH\n", 
              strlen("#BEGIN MODULE PATH\n"))) {                    /* IF.1 */

                work=buff+strlen("#BEGIN MODULE PATH\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */

          end=strchr(work, '\n') ;
       if(end==NULL)  break ;
         *end=0 ;

          value=strchr(work, '=') ;
       if(value==NULL)  continue ;
         *value=0 ;
          value++ ;

       if(!stricmp(work, "RND_PER_DIR" ))  mRandomPerDirect=strtol(value, &end, 10) ;
       if(!stricmp(work, "VECTORS_MAX" ))  mVectorsMax     =strtol(value, &end, 10) ;
       if(!stricmp(work, "LINE_STEP"   ))  mLineStep_user  =strtod(value, &end) ;
       if(!stricmp(work, "ANGLE_STEP"  ))  mAngleStep_user =strtod(value, &end) ;
       if(!stricmp(work, "LINE_ACC"    ))  mLineAcc_user   =strtod(value, &end) ;
       if(!stricmp(work, "ANGLE_ACC"   ))  mAngleAcc_user  =strtod(value, &end) ;
       if(!stricmp(work, "DOCKING_STEP"))  mScanStepCoef   =strtod(value, &end) ;
       if(!stricmp(work, "FAIL_DIRECT" ))  mFailDirect     =strtol(value, &end, 10) ;
                         }                                          /* CONTINUE.0 */
                                               }                    /* END.1 */
/*------------------------------------------------- �������� ������� */

   if(!memicmp(buff, "#BEGIN OBJECT PATH\n", 
              strlen("#BEGIN OBJECT PATH\n"))) {                    /* IF.2 */
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
              memset(name, 0, sizeof(name)) ;
                           entry=strstr(buff, "NAME=") ;            /* ��������� ��� ������� */
             strncpy(name, entry+strlen("NAME="), sizeof(name)-1) ;
          end=strchr(name, '\n') ;
         *end= 0 ;

              memset(title, 0, sizeof(title)) ;
                            entry=strstr(buff, "DESC=") ;           /* ��������� �������� ������� */
             strncpy(title, entry+strlen("DESC="), sizeof(title)-1) ;
          end=strchr(title, '\n') ;
         *end= 0 ;

              memset(master, 0, sizeof(master)) ;
                             entry=strstr(buff, "MASTER=") ;        /* ��������� �������� ������� */
             strncpy(master, entry+strlen("MASTER="), sizeof(master)-1) ;
          end=strchr(master, '\n') ;
         *end= 0 ;

                            entry=strstr(buff, "FRAME=") ;          /* ��������� ����������� ������� */
        mPath_frame=strtoul(entry+strlen("FRAME="), &end, 10) ;

                            entry=strstr(buff, "VECTORS=") ;        /* ��������� ����� �������� */
      mPath_vectors=strtoul(entry+strlen("VECTORS="), &end, 10) ;
/*- - - - - - - - - - - - - - - �������� ���������� �������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - -  �������� ���������� */
                      sprintf(desc, "[%s]->Trajectory", name) ;
              trajectory=(TRAJECTORY *)                             /* ��������� �������� ���������� */
                           gMemCalloc(1, sizeof(*trajectory), desc, 0, 0) ;

                      sprintf(desc, "[%s]->Trajectory->Path", name) ;
              trajectory->Path   =(double *)
                           gMemCalloc(mPath_vectors+1, mPath_frame*sizeof(double), desc, 0, 0) ;

                      sprintf(desc, "[%s]->Trajectory->Vectors", name) ;
              trajectory->Vectors=(double *)
                           gMemCalloc(mPath_vectors,   mPath_frame*sizeof(double), desc, 0, 0) ;
           if(trajectory->Path   ==NULL &&
              trajectory->Vectors==NULL   ) {
                  SEND_ERROR("������������ ������ ��� ���������� ����������") ;
                                return ;
                                            }

       strcpy(trajectory->name, name) ;
       strcpy(trajectory->desc, title) ;
              trajectory->Path_vectors=mPath_vectors ;
              trajectory->Path_frame  =mPath_frame  ;
/*- - - - - - - - - - - - - - - - - - - - -  �������� ������-������� */
       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, master)) {
                        trajectory->Path_object=OBJECTS[i] ;
                                                    break ;  
                                                }

         if(i>=OBJECTS_CNT) {
                        sprintf(tmp, "������-������ ���������� "
                                     "%s �� ������. ���������� "
                                     "�������������� ����������?", name) ;
              status=MessageBox(NULL, tmp, "", MB_ICONQUESTION | MB_YESNO) ;
           if(status!=IDYES) {  gMemFree(trajectory->Path) ;
                                gMemFree(trajectory->Vectors) ;
                                gMemFree(trajectory) ;
                                      return ;             }
                            }
/*- - - - - - - - - - - - ���������� ���������� ����� ������-������� */
      for(i=0 ; i<T->Path_frame ; i++) {

                                  sprintf(field, "DEGREE_%d_VALUE=", i) ;
                             entry=strstr(buff, field) ;
       if(entry!=NULL)  D[i].value=strtod(entry+strlen(field), &end) ;

                                     sprintf(field, "DEGREE_%d_V_MIN=", i) ;
                                 entry=strstr(buff, field) ;
       if(entry!=NULL)  D[i].value_min=strtod(entry+strlen(field), &end) ;

                                     sprintf(field, "DEGREE_%d_V_MAX=", i) ;
                                 entry=strstr(buff, field) ;
       if(entry!=NULL)  D[i].value_max=strtod(entry+strlen(field), &end) ;

                                  sprintf(field, "DEGREE_%d_TYPE=", i) ;
                             entry=strstr(buff, field) ;
       if(entry!=NULL)  D[i].type=strtoul(entry+strlen(field), &end, 10) ;

                                  sprintf(field, "DEGREE_%d_FIXED=", i) ;
                             entry=strstr(buff, field) ;
       if(entry!=NULL)  D[i].fixed=strtoul(entry+strlen(field), &end, 10) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - ���������� ������� ����� */
                 i= 0 ;
              entry=strstr(buff, "PATH=") ;
        while(entry!=NULL) {
                                        end=entry+strlen("PATH") ;
               do {
                     T->Path[i]=strtod(end+1, &end) ;
                             i++ ;
                  } while(*end==',') ;

              entry=strstr(entry+1, "PATH=") ;
                           }  
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
                 i= 0 ;
              entry=strstr(buff, "DIR=") ;
        while(entry!=NULL) {
                                        end=entry+strlen("DIR") ;
               do {
                     T->Vectors[i]=strtod(end+1, &end) ;
                                i++ ;
                  } while(*end==',') ;

              entry=strstr(entry+1, "DIR=") ;
                           }  
/*- - - - - - - - - - - - - - - - - - ���� ������� � ������ �������� */
        object=new RSS_Object_Path ;
     if(object==NULL) {
              SEND_ERROR("������ PATH: ������������ ������ ��� �������� �������-����������") ;
                                   return ;
                      }

                object->Trajectory=trajectory ;  
         strcpy(object->Name, trajectory->name) ;

        OBJECTS=(RSS_Object **)
                  gMemRealloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS), 
                                 "Kernel->kernel_objects", 0, 0) ;
     if(OBJECTS==NULL) {
              SEND_ERROR("������ PATH: ������������ ������") ;
                                return ;
                       }

              OBJECTS[OBJECTS_CNT]=object ;
                      OBJECTS_CNT++ ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                               }                    /* END.2 */
/*-------------------------------------------- ������������ �������� */

                gMemFree(buff) ;

/*-------------------------------------------------------------------*/

#undef  T
#undef  D

}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Path::vWriteSave(std::string *text)

{
  char  value[1024] ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE PATH\n" ;

    sprintf(value, "RND_PER_DIR=%d\n", mRandomPerDirect) ;
     *text+=value ;
    sprintf(value, "VECTORS_MAX=%d\n", mVectorsMax) ;
     *text+=value ;
    sprintf(value, "LINE_STEP=%lf\n", mLineStep_user) ;
     *text+=value ;
    sprintf(value, "ANGLE_STEP=%lf\n", mAngleStep_user) ;
     *text+=value ;
    sprintf(value, "LINE_ACC=%lf\n", mLineAcc_user) ;
     *text+=value ;
    sprintf(value, "ANGLE_ACC=%lf\n", mAngleAcc_user) ;
     *text+=value ;
    sprintf(value, "DOCKING_STEP=%lf\n", mScanStepCoef) ;
     *text+=value ;
    sprintf(value, "FAIL_DIRECT=%d\n", mFailDirect) ;
     *text+=value ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Path::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - ����� � �������� ����������", "PATH") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*             ��������� �������� � ��������� ������                 */

    void  RSS_Module_Path::vChangeContext(void)

{
/*------------------------------------ ����������� ������ ���������� */

   if(!stricmp(mContextAction, "SHOW")) {

                      iTopTraceShow(mContextObject) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Path::cHelp(char *cmd, RSS_IFace *iface)

{ 
/*
    RSS_Point  p1 ; 
    RSS_Point  p2 ;
       double  x_diff ;
       double  y_diff ;
       double  z_diff ;
         char  text[1024] ;

        memset(&p1, 0, sizeof(p1)) ;
        memset(&p2, 0, sizeof(p2)) ;
                p1.azim=0 ; 
                p1.elev=0 ; 
                p2.azim=15 ; 
                p2.elev=15 ; 
      iOrtsDif(&p1, &p2, &x_diff, &y_diff, &z_diff) ;

       sprintf(text, "%lf  %lf  %lf", x_diff, y_diff, z_diff) ;
    SEND_ERROR(text) ;
        return(0) ;
*/
    
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Path_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CONFIG                  */

  int  RSS_Module_Path::cConfig(char *cmd, RSS_IFace *iface)

{ 
    DialogBoxIndirectParam(GetModuleHandle(NULL),
			   (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
			     GetActiveWindow(), Task_Path_Config_dialog, 
                              (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� SEARCH                  */
/*  SEARCH[/DRQ][:<��� ������.>] <��� ������ �����> [<��� �������>] */

  int  RSS_Module_Path::cSearch(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   7
                       char  *pars[_PARS_MAX] ;
                        int   reduction_flag ;     /* ���� ���������� ���������� */
                        int   save_flag ;          /* ���� ���������� ���������� */
                       char   save_name[64] ;      /* ��� ����������� ���������� */
                        int   quit_flag ;          /* ���� ������������� ���������� */
                       char  *set_name ;
                       char  *object_name ;
                       char  *traj_name ;
                 RSS_Object  *set ;
                 RSS_Object  *master ;
                 RSS_Object  *object ;
                  RSS_Point   coord ;              /* ���������� ������� ����� */
                 TRAJECTORY  *trajectory ; 
            RSS_Object_Path  *path_object ;
                       char   action[128] ;
                       char  *end ;
                       char  *arg ;
                  RSS_Joint   degrees[50] ;        /* ������ �������� ����������� ������� */
                  RSS_Joint   degrees_prv[50] ;
                        int   cnt ;                /* ����� �������� ����������� */
                        int   cnt_prv ;
                        int   status ;
                       char   text[256] ;
                       char   keys[256] ;
                       char   desc[1024] ;
                        int   i ;
                        int   j ;

   union {  RSS_Joint   degrees[50] ;
            RSS_Point   absolute ;
                 char   object[256] ;  }  point ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- ������������� */

            reduction_flag=0 ;
                 quit_flag=0 ;
                 save_flag=0 ;

          memset(save_name, 0, sizeof(save_name)) ;

                     mDraw=0 ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                       memset(keys, 0, sizeof(keys)) ;
                      strncpy(keys, cmd, sizeof(keys)-1) ;
                   arg=strchr(keys, ':') ;
                if(arg!=NULL)  *arg=0 ;

                if(strchr(keys, 'd')!=NULL ||
                   strchr(keys, 'D')!=NULL   )  mDraw=1 ;
                if(strchr(keys, 'r')!=NULL ||
                   strchr(keys, 'R')!=NULL   )  reduction_flag=1 ;
                if(strchr(keys, 'q')!=NULL ||
                   strchr(keys, 'Q')!=NULL   )  quit_flag=1 ;

                   traj_name=strchr(cmd, ':') ;
                if(traj_name!=NULL) {  strcpy(save_name, traj_name+1) ;
                                              save_flag=1 ;              }

                           cmd=end+1 ;
                   }
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

                     set_name=pars[0] ;
                  object_name=pars[1] ;

/*-------------------------------------- �������� ����� ������ ����� */

    if(set_name==NULL) {                                            /* ���� ��� �� ������... */
                   SEND_ERROR("�� ������ ��� ������ �����.\n"
                              "��������: SEARCH <���_������> ...") ;
                                     return(-1) ;
                       }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, set_name)) {  set=OBJECTS[i] ;
                                                             break ;  }

         if(i==OBJECTS_CNT) {                                       /* ���� ����� ����� �� ������... */
             SEND_ERROR("������ ����� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }

            status=set->vSpecial("POINTS_SET:MASTER",               /* ��������� ������-������ ������ ����� */ 
                                    (void *)&master  ) ;
         if(status==-1) {
             SEND_ERROR("��������� ������ �� �������� ������� �����") ;
                                return(-1) ;                            
                        }
/*------------------------------------------- �������� ����� ������� */

    if(object_name==NULL &&                                         /* ���� ������������ ������ �� �����... */
            master==NULL   ) { 
                      SEND_ERROR("������������� ������ �� ����� "
                                 "�� � �������, �� � ������ �����") ;
                                     return(-1) ;
                             }

    if(object_name!=NULL) {                                         /* ���� ������ ��� ������� - */
                                                                    /*   - ���� ��� �� ������    */
       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, object_name)) {
                                  object=OBJECTS[i] ;  break ;  
                                                     }

         if(i==OBJECTS_CNT) {
             SEND_ERROR("�������������� ������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }
                          }
    else                  {                                         /* � ��������� ������ ����������� */
                                object=master ;                     /*  ������-������ ������ �����    */
                          }
/*----------------------- �������� ������������ ������� �� ��������e */

              cnt=object->vGetJoints(degrees) ;                     /* �������� ��������� ����.������� */

    if(master!= NULL &&                                             /* ���� ������-������ ����� �    */
       master!=object  ) {                                          /*  �� ��������� � ������������� */

      if(stricmp(master->Type, object->Type)) {
             SEND_ERROR("��������� ����� ����� �� ��������� � �������� ������� ����") ;
                                return(-1) ;
                                              }

         cnt_prv=master->vGetJoints(degrees_prv) ;
      if(cnt_prv!=cnt) {
             SEND_ERROR("��������� ����� ����� �� ��������� � �������� ������ ���������") ;
                                return(-1) ;
                       }

         status=MessageBox(NULL, "����� ����� ��� ����������� ���"
                                 "������� �������. ����������?", 
                                    "", MB_ICONWARNING | MB_YESNO) ;
      if(status!=IDYES)  return(-1) ;
                         }
/*------------------------------------------- �������� ������� ����� */

         cnt=object->vGetJoints(degrees) ;

     for(i=0 ; ; i++) {
                         sprintf(action, "POINTS_SET:GET%d", i) ;
            status=set->vSpecial(action, (void *)&point) ;          /* ��������� �������� ����� */ 
         if(status==-1)  break ;                           

         if(status==_RSS_ABSOLUTE_TARGET ||
            status==  _RSS_OBJECT_TARGET   )  continue ;

                             object->vSetJoints    (point.degrees, cnt) ;
                      status=object->vSolveByJoints() ;             /* ������ ������ ������ � �������� ����� */
         if(!status)  status=object->vCheckFeatures(NULL) ;         /* ��������� ������������������ ������� */
         if( status) {
                      sprintf(text, "����� %d ���������� ������ �����������", i+1) ; 
                   SEND_ERROR(text) ;
                         return(-1) ;
                     }
                      }

       if(i<2) {
                  SEND_ERROR("��������� ����� �������� ����� 2 ������� �����") ;
                                return(-1) ;
               }
/*------------------------------------------ ����� � ��������� ����� */

                    mCheck=  1 ;
                 mTraceTop=  0 ;
                  mVectors=NULL ;

        status=set->vSpecial("POINTS_SET:GET0", (void *)&point) ;   /* ��������� �������� 1-�� ����� */ 
/*- - - - - - - - - - - - - - - - - - - -  ���� ������ ������������� */
     if(status==_RSS_JOINTS_TARGET) {
      
               mPath_vectors=0 ;
               mPath_frame  =cnt ;

               mPath=(double *)
                       gMemCalloc(mPath_frame, sizeof(*mPath), 
                                      "RSS_Module_Path::mPath", 0, 0) ;

           for(i=0 ; i<mPath_frame ; i++) 
                        mPath[i]=point.degrees[i].value ;
                                    }
/*- - - - - - - - - - - - - - - - - - - - ���� ������ ������� ������ */
     else                           {
                                    }
/*---------------------------- ���������� ����� ������� ������������ */

                  mPath_object =object ;
           memcpy(mPath_degrees, degrees, cnt*sizeof(*degrees)) ;   /* �������� ���������� ��������� ����� */

                   mVectorsMax_=mVectorsMax ;

   for(i=1 ; ; i++) {                                               /* CIRCLE.1 - ������� ������� ����� */
                          sprintf(action, "POINTS_SET:GET%d", i) ;
             status=set->vSpecial(action, (void *)&point) ;         /* ��������� ��������� ������� ����� */ 
          if(status==-1)  break ;
/*- - - - - - - - - - - - - - - - - - - -  ���� ������ ������������� */
          if(status==_RSS_JOINTS_TARGET) {

                status=iSearchDirect(point.degrees) ;
             if(status==-1)  break ;
                                         }
/*- - - - - - - - - - - - - - - - ���� ������ ������� ������ ������� */
     else if(status==_RSS_OBJECT_TARGET) {

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���������� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, point.object))  break ;
            
         if(i==OBJECTS_CNT) {
                  sprintf(text, "[Path.Search] ����������� ������� ������ %s", point.object) ; 
               SEND_ERROR(text) ;
                    return(-1) ;
                            }

                    OBJECTS[i]->vGetTarget(point.object, &coord) ;  /* ��������� ���������� ������� ����� */

                status=iSearchReach(&coord) ;
             if(status==-1)  break ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - ���� ������ ������������ */
     else                                {

                                     coord=point.absolute ;
                status=iSearchReach(&coord) ;
             if(status==-1)  break ;
                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    }                                               /* CONTINUE.1 */

                       this->kernel->vShow(NULL) ;                  /* ����������� ����� */ 

/*-------------------------------------------- ���������� ���������� */

    if(!reduction_flag) {

          status=MessageBox(NULL, "��������� ����� ����������?", 
                                    "", MB_ICONQUESTION | MB_YESNO) ;
       if(status==IDYES)  reduction_flag=1 ;
                        }

    if( reduction_flag) {
                           iReducePath() ;                          /* ��������� ������� ������ ��������� ���������� */
                           iReducePath() ;
                        }
/*------------------------------------------ ������������ ���������� */

    if(!quit_flag) {

          status=MessageBox(NULL, "�������� ��������� ����������?", 
                                    "", MB_ICONQUESTION | MB_YESNO) ;
       if(status!=IDYES)  quit_flag=0 ;
                   }

    if(!quit_flag) {
                             mDraw=1 ;
                    status=iShowPath() ;
                 if(status)  SEND_ERROR("�� ���������� ���������� ������������ �����") ;
                   }
/*------------------------------------- ������ ���������� ���������� */

    if(!save_flag) {

          status=MessageBox(NULL, "��������� ��������� ����������?", 
                                    "", MB_ICONQUESTION | MB_YESNO) ;
       if(status==IDYES)  save_flag=1 ;
                   }
/*-------------------------------------------- ���������� ���������� */

    if( save_flag) {
                             sprintf(desc, "[%s]->Trajectory", save_name) ;
              trajectory=(TRAJECTORY *)                             /* ��������� �������� ���������� */
                           gMemCalloc(1, sizeof(*trajectory), desc, 0, 0) ;

                             sprintf(desc, "[%s]->Trajectory->Path", save_name) ;
              trajectory->Path   =(double *)
                           gMemCalloc(mPath_vectors+1, mPath_frame*sizeof(double), desc, 0, 0) ;

                             sprintf(desc, "[%s]->Trajectory->Vectors", save_name) ;
              trajectory->Vectors=(double *)
                           gMemCalloc(mPath_vectors,   mPath_frame*sizeof(double), desc, 0, 0) ;
           if(trajectory->Path   ==NULL &&
              trajectory->Vectors==NULL   ) {
                  SEND_ERROR("������������ ������ ��� ���������� ����������") ;
                                return(-1) ;
                                            }

              trajectory->module      =this ;
       strcpy(trajectory->name, save_name) ;
              trajectory->Path_vectors=mPath_vectors ;
              trajectory->Path_frame  =mPath_frame  ;
              trajectory->Path_object =mPath_object ;
       memcpy(trajectory->Path_degrees, mPath_degrees, mPath_frame*sizeof(mPath_degrees[0])) ;
       memcpy(trajectory->Path,         mPath,         mPath_frame*(mPath_vectors+1)*sizeof(double)) ;
       memcpy(trajectory->Vectors,      mVectors,      mPath_frame* mPath_vectors   *sizeof(double)) ;

                   }
/*------------------------- ���������� ���������� � �������� ������� */

   if(save_flag) do {
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
      if(save_name[0]==0) {
         DialogBoxIndirectParam(GetModuleHandle(NULL),
      	                        (LPCDLGTEMPLATE)Resource("IDD_SAVE", RT_DIALOG),
			          GetActiveWindow(), Task_Path_Save_dialog,
                                   (LPARAM)trajectory) ;
                          }
      else                {
               strncpy(trajectory->name, save_name,
                               sizeof(trajectory->name)) ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� ����� */
         if(trajectory->name[0]== 0  ||
            trajectory->name[0]==' '   ) {
               SEND_ERROR("������ PATH: �� ������ ��� ����������") ;
                                         save_name[0]=0 ;
                                              continue ;
                                         }
/*- - - - - - - - - - - - - - - ���� ������� � ����� ������ ��� ���� */
       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ � ����� ������ ���������� -  */
         if(!stricmp(OBJECTS[i]->Name, trajectory->name)) {         /*  - ������� ���                           */

                         OBJECTS[i]->vFree() ;                      /*  ������������ �������� */
                 delete  OBJECTS[i] ;

           for(j=i+1 ; j<OBJECTS_CNT ; j++)
                             OBJECTS[j-1]=OBJECTS[j] ;              /*  �������� ������ �������� */

                         OBJECTS_CNT-- ;

                 this->kernel->vShow("REFRESH") ;                   /*  ���������� �������� */
                                break ;
                                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
            path_object=new RSS_Object_Path ;
         if(path_object==NULL) {
              SEND_ERROR("������ PATH: ������������ ������ ��� �������� �������-����������") ;
                                   break ;
                               }

              path_object->Trajectory=trajectory ;  
       strcpy(path_object->Name, trajectory->name) ;

       OBJECTS=(RSS_Object **)
                 gMemRealloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS), 
                                              "Kernel->kernel_objects", 0, 0) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ PATH: ������������ ������") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=path_object ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)trajectory->name) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    } while(0) ;

/*-------------------------------------------- ������������ �������� */

    if(mPath   !=NULL)  gMemFree(mPath) ;
                                 mPath=NULL ; 

    if(mVectors!=NULL)  gMemFree(mVectors) ;
                                 mVectors=NULL ; 

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� TRACE                   */
/*  TRACE[/D] <��� ����������> [<��� �������>]                      */

  int  RSS_Module_Path::cTrace(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   7
                  RSS_IFace   iface_ ;
                       char  *pars[_PARS_MAX] ;
                       char  *object_name ;
                       char  *traj_name ;
                        int   Reverse_flag ;
                 RSS_Object  *master ;
                 RSS_Object  *object ;
            RSS_Object_Path  *path_object ;
                       char  *end ;
                  RSS_Joint   degrees[50] ;        /* ������ �������� ����������� ������� */
                  RSS_Joint   degrees_prv[50] ;
                        int   cnt ;                /* ����� �������� ����������� */
                        int   cnt_prv ;
                        int   status ;
                        int   i ;

#define   OBJECTS       this->kernel->kernel_objects
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------------------------- ������������� */

                     mDraw=0 ;

              Reverse_flag=0 ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'd')!=NULL ||
                   strchr(cmd, 'D')!=NULL   )  mDraw=1 ;

                if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )  Reverse_flag=1 ;

                           cmd=end+1 ;
                   }
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

                    traj_name=pars[0] ;
                  object_name=pars[1] ;

/*---------------------------------------- �������� ����� ���������� */

    if(traj_name==NULL) {                                           /* ���� ��� �� ������... */
                   SEND_ERROR("�� ������ ��� ����������.\n"
                              "��������: TRACE <���_����������> ...") ;
                                     return(-1) ;
                        }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, traj_name)) {
                        path_object=(RSS_Object_Path *)OBJECTS[i] ;
                                                      break ;  
                                                   }

         if(i==OBJECTS_CNT) {                                       /* ���� ���������� �� �������... */
             SEND_ERROR("���������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }

         if(stricmp(path_object->Type, "Path")) {                   /* ������������ ��� �������-���������� */
             SEND_ERROR("��������� ������ �� �������� �����������") ;
                                return(-1) ;                            
                                                }

               master=path_object->Trajectory->Path_object ;        /* ��������� ������-������ ���������� */

/*------------------------------------------- �������� ����� ������� */

    if(object_name==NULL &&                                         /* ���� ������������ ������ �� �����... */
            master==NULL   ) { 
                      SEND_ERROR("������������� ������ �� ����� "
                                 "�� � �������, �� � ����������") ;
                                     return(-1) ;
                             }

    if(object_name!=NULL) {                                         /* ���� ������ ��� ������� - */
                                                                    /*   - ���� ��� �� ������    */
       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, object_name)) {
                                  object=OBJECTS[i] ;  break ;  
                                                     }

         if(i==OBJECTS_CNT) {
             SEND_ERROR("�������������� ������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }
                          }
    else                  {                                         /* � ��������� ������ ����������� */
                                object=master ;                     /*  ������-������ ����������    */
                          }
/*----------------------- �������� ������������ ������� �� ��������e */

              cnt=object->vGetJoints(degrees) ;                     /* �������� ��������� ����.������� */

    if(master!= NULL &&                                             /* ���� ������-������ ����� �    */
       master!=object  ) {                                          /*  �� ��������� � ������������� */

      if(stricmp(master->Type, object->Type)) {
             SEND_ERROR("��������� ���������� �� ���������� � �������� ������� ����") ;
                                return(-1) ;
                                              }

         cnt_prv=master->vGetJoints(degrees_prv) ;
      if(cnt_prv!=cnt) {
             SEND_ERROR("��������� ���������� �� ���������� � �������� ������ ���������") ;
                                return(-1) ;
                       }

         status=MessageBox(NULL, "���������� ���� ����������� ���"
                                 "������� �������. ����������?", 
                                    "", MB_ICONWARNING | MB_YESNO) ;
      if(status!=IDYES)  return(-1) ;
                         }
/*---------------------------------------------- �������� ���������� */

#define  T   path_object->Trajectory

       if(Reverse_flag)  iReverse(T) ;                              /* ���� ������ ���������� */

              mPath_vectors=T->Path_vectors ;
              mPath_frame  =T->Path_frame ;
              mPath_object = object ;
              mPath        =T->Path ;
              mVectors     =T->Vectors ;
             
              mPath_object->vGetJoints(mPath_degrees) ;

                            mCheck=1 ;
                         mTraceTop=0 ;

                   iCulculateSteps() ;                              /* ����������� ����� ������������ */

                  status=iShowPath() ;                              /* �������� �� ���������� */
               if(status) {
                 if(iface==NULL)  SEND_ERROR("�� ���������� ���������� ������������ �����") ;
                              iface->vSignal("ERROR", 
                                             "�� ���������� ���������� ������������ �����") ;
                          }
               else       {
                              iface->vSignal("DONE", "") ;          /* ������ ��������� �� ���������� ������������ ����� */
                          }

       if(Reverse_flag)  iReverse(T) ;                              /* �������������� ����������������� ���������� */

#undef  T

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� INDICATE                */
/*  INDICATE <��� ����������> [<��� �������>]                       */

  int  RSS_Module_Path::cIndicate(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   7
                       char  *pars[_PARS_MAX] ;
                       char  *object_name ;
                       char  *traj_name ;
                 RSS_Object  *master ;
                 RSS_Object  *object ;
            RSS_Object_Path  *path_object ;
                       char  *end ;
                  RSS_Joint   degrees[50] ;        /* ������ �������� ����������� ������� */
                  RSS_Joint   degrees_prv[50] ;
                        int   cnt ;                /* ����� �������� ����������� */
                        int   cnt_prv ;
                        int   status ;
                        int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------------------- ������������� */

                     mDraw=0 ;

/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
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

                    traj_name=pars[0] ;
                  object_name=pars[1] ;

/*---------------------------------------- �������� ����� ���������� */

    if(traj_name==NULL) {                                           /* ���� ��� �� ������... */
                   SEND_ERROR("�� ������ ��� ����������.\n"
                              "��������: INDICATE <���_����������> ...") ;
                                     return(-1) ;
                        }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, traj_name)) {
                        path_object=(RSS_Object_Path *)OBJECTS[i] ;
                                                      break ;  
                                                   }

         if(i==OBJECTS_CNT) {                                       /* ���� ���������� �� �������... */
             SEND_ERROR("���������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }

         if(stricmp(path_object->Type, "Path")) {                   /* ������������ ��� �������-���������� */
             SEND_ERROR("��������� ������ �� �������� �����������") ;
                                return(-1) ;                            
                                                }

               master=path_object->Trajectory->Path_object ;        /* ��������� ������-������ ���������� */

/*------------------------------------------- �������� ����� ������� */

    if(object_name==NULL &&                                         /* ���� ������������ ������ �� �����... */
            master==NULL   ) { 
                      SEND_ERROR("������������� ������ �� ����� "
                                 "�� � �������, �� � ����������") ;
                                     return(-1) ;
                             }

    if(object_name!=NULL) {                                         /* ���� ������ ��� ������� - */
                                                                    /*   - ���� ��� �� ������    */
       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, object_name)) {
                                  object=OBJECTS[i] ;  break ;  
                                                     }

         if(i==OBJECTS_CNT) {
             SEND_ERROR("�������������� ������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }
                          }
    else                  {                                         /* � ��������� ������ ����������� */
                                object=master ;                     /*  ������-������ ����������    */
                          }
/*----------------------- �������� ������������ ������� �� ��������e */

              cnt=object->vGetJoints(degrees) ;                     /* �������� ��������� ����.������� */

    if(master!= NULL &&                                             /* ���� ������-������ ����� �    */
       master!=object  ) {                                          /*  �� ��������� � ������������� */

      if(stricmp(master->Type, object->Type)) {
             SEND_ERROR("��������� ���������� �� ���������� � �������� ������� ����") ;
                                return(-1) ;
                                              }

         cnt_prv=master->vGetJoints(degrees_prv) ;
      if(cnt_prv!=cnt) {
             SEND_ERROR("��������� ���������� �� ���������� � �������� ������ ���������") ;
                                return(-1) ;
                       }

         status=MessageBox(NULL, "���������� ���� ����������� ���"
                                 "������� �������. ����������?", 
                                    "", MB_ICONWARNING | MB_YESNO) ;
      if(status!=IDYES)  return(-1) ;
                         }
/*----------------------------------------------- ������ ���������� */

#define  T   path_object->Trajectory

              mPath_vectors=T->Path_vectors ;
              mPath_frame  =T->Path_frame ;
              mPath_object = object ;
              mPath        =T->Path ;
              mVectors     =T->Vectors ;
             
              mPath_object->vGetJoints(mPath_degrees) ;

           mCheck          =  0 ;
           mDraw           =  0 ;
           mTraceTop       =  1 ;
           mTracePoints    =NULL ;
           mTracePoints_cnt=  0 ;
           mTracePoints_max=  0 ;

                   iCulculateSteps() ;                              /* ����������� ����� ������������ */
                         iShowPath() ;                              /* �������� �� ���������� */

       if(T->Trace)  gMemFree(T->Trace) ;

                              T->Trace    =mTracePoints ;
                              T->Trace_cnt=mTracePoints_cnt ;

       if(T->TraceColor==0)  T->TraceColor=this->mTraceColor ;

#undef  T

/*--------------------------------------------- ��������� ���������� */

            iTopTraceShow_(path_object->Trajectory) ;

                  this->kernel->vShow(NULL) ;

/*-------------------------------------------- ������������ �������� */

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� COPY                    */
/*								    */
/*      COPY <��� �������> <��� �����>                              */

  int  RSS_Module_Path::cCopy(char *cmd, RSS_IFace *iface)

{
#define   _PARS_MAX   7
                       char *pars[_PARS_MAX] ;
            RSS_Object_Path *Src ;
            RSS_Object_Path *Dst ;
                        int   Reverse_flag ;
                 TRAJECTORY *traj ; 
                       char *name ;
                       char *copy ;
                        int  overwrite ;   /* ���� ���������� ������������� ������� */
                       char *end ;
                       char  desc[1024] ;
                        int  n ;
                        int  i ;

#define   OBJECTS       this->kernel->kernel_objects
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*-------------------------------------- ���������� ��������� ������ */

                   overwrite=0 ;

                Reverse_flag=0 ;
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'o')!=NULL ||
                   strchr(cmd, 'O')!=NULL   )  overwrite=1 ;

                if(strchr(cmd, 'r')!=NULL ||
                   strchr(cmd, 'R')!=NULL   )  Reverse_flag=1 ;

                           cmd=end+1 ;
                   }
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

                         name=pars[0] ;
                         copy=pars[1] ;

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������.\n"
                                 "��������: COPY <��� �������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                Src=(RSS_Object_Path *)OBJECTS[i] ;
                                                      break ;  
                                              }

         if(i==OBJECTS_CNT) {                                       /* ���� ������� �� �������... */
             SEND_ERROR("���������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }

         if(stricmp(Src->Type, "Path")) {                           /* ������������ ��� ������� */
             SEND_ERROR("��������� ������ �� �������� �����������") ;
                                return(-1) ;                            
                                        }
/*--------------------------------------------- �������� ����� ����� */

   if(copy[0]== 0  ||
      copy[0]==' '   ) {
             SEND_ERROR("������ PATH: �� �����o ��� �����") ;
                                return(-1) ;
                       }

       for(n=-1, i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, copy)) {

          if(!overwrite) {
              SEND_ERROR("������ PATH: ������ � ����� ������ ��� ����������") ;
                                return(-1) ;
                         }

          if(stricmp(OBJECTS[i]->Type, "Path")) {                   /* ������������ ��� ������� */
             SEND_ERROR("���������������� ������ �� �������� �����������") ;
                                return(-1) ;                            
                                                }

                                 n=i ;
                                              }
/*------------------------------- �������� ����������������� ������� */

     if(n>=0) {
                          OBJECTS[n]->vFree() ;
                  delete  OBJECTS[n] ;
              }
/*-------------------------------------------- ���������� ���������� */

#define  S   Src->Trajectory

                    sprintf(desc, "[%s]->Trajectory", copy) ;
              traj         =(TRAJECTORY *)                          /* ��������� �������� ���������� */
                              gMemCalloc(1, sizeof(*traj), desc, 0, 0) ;

                    sprintf(desc, "[%s]->Trajectory->Path", copy) ;
              traj->Path   =(double *)
                              gMemCalloc(S->Path_vectors+1, S->Path_frame*sizeof(double), desc, 0, 0) ;

                    sprintf(desc, "[%s]->Trajectory->Vectors", copy) ;
              traj->Vectors=(double *)
                              gMemCalloc(S->Path_vectors,   S->Path_frame*sizeof(double), desc, 0, 0) ;
           if(traj->Path   ==NULL &&
              traj->Vectors==NULL   ) {
                  SEND_ERROR("������������ ������ ��� ���������� ����������") ;
                                            return(-1) ;
                                      }

       strcpy(traj->name, copy) ;
              traj->module      = this ;
              traj->Path_vectors=S->Path_vectors ;
              traj->Path_frame  =S->Path_frame  ;
              traj->Path_object =S->Path_object ;
       memcpy(traj->Path_degrees, S->Path_degrees, S->Path_frame*sizeof(S->Path_degrees[0])) ;
       memcpy(traj->Path,         S->Path,         S->Path_frame*(S->Path_vectors+1)*sizeof(double)) ;
       memcpy(traj->Vectors,      S->Vectors,      S->Path_frame* S->Path_vectors   *sizeof(double)) ;

#undef  S

/*------------------------------------------------ ������ ���������� */

      if(Reverse_flag)  iReverse(traj) ;

/*------------------------------------- �������� ����������� ������� */

            Dst=new RSS_Object_Path ;                               /* �������� ������� */
         if(Dst==NULL) {
              SEND_ERROR("������ PATH: ������������ ������ ��� �������� �������-����������") ;
                            return(-1) ;
                       }

              Dst->Trajectory=traj ;                                /* ���������� ������� � ������ � ����������� */
       strcpy(Dst->Name, traj->name) ;
/*- - - - - - - - - - - - - - - - - - - - -  ���� ���������� ������� */
    if(n>=0) {
                    OBJECTS[n]=Dst ;
             }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���� ����� ������ */
    else     {

           OBJECTS=(RSS_Object **)                                  /* ������� �������-���������� � ����� ������ �������� */
                    gMemRealloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS), 
                                 "Kernel->kernel_objects", 0, 0) ;

        if(OBJECTS==NULL) {
              SEND_ERROR("������ PATH: ������������ ������") ;
                                return(-1) ;
                          }

                  OBJECTS[OBJECTS_CNT]=Dst ;
                          OBJECTS_CNT++ ;
             }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   SendMessage(this->kernel_wnd, WM_USER,                           /* �����.��� ������ ��-��������� */
                (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)traj->name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   _PARS_MAX

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� COLOR                    */
/*       COLOR  <������> <�������� �����>                           */
/*       COLOR  <������> <R> <G> <B>                                */

  int  RSS_Module_Path::cColor(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   4
             char *pars[_PARS_MAX] ;
             char *name ;
         COLORREF  color ;
//            int  red, green, blue ;
  RSS_Object_Path  *path_object ;
             char *end ;
              int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------ ������ ���������� */        

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

/*---------------------------------------- �������� ����� ���������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                     SEND_ERROR("�� ������ ��� ����������.\n"
                                "��������: COLOR <���_����������> GREEN") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
                        path_object=(RSS_Object_Path *)OBJECTS[i] ;
                                                      break ;  
                                              }

         if(i==OBJECTS_CNT) {                                       /* ���� ���������� �� �������... */
             SEND_ERROR("���������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }

         if(stricmp(path_object->Type, "Path")) {                   /* ������������ ��� �������-���������� */
             SEND_ERROR("��������� ������ �� �������� �����������") ;
                                return(-1) ;                            
                                                }
/*----------------------------------------------------- ������ ����� */
/*- - - - - - - - - - - - - - - - - - - - - - ������� ����� �� ����� */
     if(pars[2]==NULL) {

              if(!stricmp(pars[1], "RED"  ))  color=RGB(200,   0,   0) ;
         else if(!stricmp(pars[1], "GREEN"))  color=RGB(  0, 127,   0) ;
         else if(!stricmp(pars[1], "BLUE" ))  color=RGB(  0,   0, 200) ;
         else                                {

                         SEND_ERROR("����������� �������� �����") ;
                                        return(-1) ;
                                             }
                       }
/*- - - - - - - - - - - - - - - - - - - - ������� ����� ������������ */
     else              {

//      if(pars[2]!=NULL)  

//                            color=RGB(red, green, blue) ;                                          
                       }
/*--------------------------------------- ��������� ����� ���������� */

              path_object->Trajectory->TraceColor=color ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

#undef   _PARS_MAX

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                ����� ���� �� ��������� ������������               */

   int  RSS_Module_Path::iSearchDirect(RSS_Joint *degrees)

{
   double  target[50] ;        /* ���������� ������� ����� */
   double  interim[50] ;       /* ���������� ������������� ����� */
   double  vector[50] ;        /* ������ ���������� �������� ����������� */
      int  status ;
      int  jump_cnt ;
      int  i ;

/*------------------------------------------------------- ���������� */

     for(i=0 ; i<mPath_frame ; i++)  target[i]=degrees[i].value ;   /* ��������� ���������� ������� ����� */

                   iCulculateSteps() ;                              /* ����������� ����� ������������ */

/*------------------------------------------------------ ���� ������ */

                     jump_cnt=1 ;

    do {                                                            /* CIRCLE.0 */

/*-------------------------------- �������� ����� ��������� �������� */

     if(mPath_vectors>mVectorsMax_) {

         status=MessageBox(NULL, "����� �������� ��� ������ �������� �����������."
                                 "���������� �����?", "", 
                                  MB_YESNO | MB_ICONQUESTION) ;
      if(status==IDNO)  return(-1) ;

                                        mVectorsMax_+=mVectorsMax ;
                                    }
/*---------------------------------------------- ������������ ������ */

        status=iCalculateVector(&mPath[mPath_vectors*mPath_frame],  /* ���������� ������� */
                                 target, vector) ;
     if(status)  return(0) ;                                        /* ���� ��������� � �������� ����� */
                                                                    /*   ������� ���������             */
        status=iTraceVector(&mPath[mPath_vectors*mPath_frame],      /* ���������� ������ */
                             target, interim, vector) ;
     if(status==0) {                                                /* ���� ������� ����� ����������... */
                       iAddPathPoint(target, vector) ;              /*  ��������� ����� � ���������� */
                         return(0) ;
                   }

     if(jump_cnt==mRandomPerDirect) {                               /* ���� ���� - */
                               iAddPathPoint(interim, vector) ;     /*  - ��������� ����� � ���������� */
                                              jump_cnt=0 ;
                                    }
/*----------------------------------------- ��������� ������ ������� */

    do {
              iGenerateVector(vector) ;                             /* ���������� ������ ������� */
          status=iTraceVector(&mPath[mPath_vectors*mPath_frame],    /* ���������� ������ */
                                 NULL, interim, vector) ;
       } while(status>0) ;

              iAddPathPoint(interim, vector) ;                      /* ��������� ����� � ���������� */

                         jump_cnt++ ;

/*------------------------------------------------------ ���� ������ */

       } while(1) ;                                                 /* CONTINUE.0 */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                ����� ���� �� ������� �����                        */

   int  RSS_Module_Path::iSearchReach(RSS_Point *point)

{
   double  interim[50] ;       /* ���������� ������������� ����� */
   double  vector[50] ;        /* ������ ���������� �������� ����������� */
      int  jump_cnt ;
      int  save_flag ;
      int  status ;

/*------------------------------------------------------- ���������� */

                   iCulculateSteps() ;                              /* ����������� ����� ������������ */

/*------------------------------------------------------ ���� ������ */

                     jump_cnt=1 ;

    do {                                                            /* CIRCLE.0 */
                 mDebug_stamp++ ;

/*-------------------------------- �������� ����� ��������� �������� */

     if(mPath_vectors>mVectorsMax_) {

         status=MessageBox(NULL, "����� �������� ��� ������ �������� �����������."
                                 "���������� �����?", "", 
                                  MB_YESNO | MB_ICONQUESTION) ;
      if(status==IDNO)  return(-1) ;

                                        mVectorsMax_+=mVectorsMax ;
                                    }
/*---------------------------------------------- ������������ ������ */

     if(jump_cnt==mRandomPerDirect) {  save_flag=1 ;  jump_cnt=0 ;  }
     else                              save_flag=0 ;

        status=iReachVector(point, save_flag) ;                     /* ���������� ������ */
     if(status==0)  return(0) ;

/*----------------------------------------- ��������� ������ ������� */

    do {
              iGenerateVector(vector) ;                             /* ���������� ������ ������� */
          status=iTraceVector(&mPath[mPath_vectors*mPath_frame],    /* ���������� ������ */
                                 NULL, interim, vector) ;
       } while(status>0) ;

            iAddPathPoint(interim, vector) ;                      /* ��������� ����� � ���������� */

                         jump_cnt++ ;

/*------------------------------------------------------ ���� ������ */

       } while(1) ;                                                 /* CONTINUE.0 */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                    ���������� ����������                          */
/*								     */
/*     ������������ ������ ������ ����� - ������� ����������� ��     */
/*  '����������' �������� ��������� �����, � ����� ��� ����� �       */
/*  ����� �������.                                                   */

   void  RSS_Module_Path::iReducePath(void)

{
      int  step ;         /* ��� ���������� '�����������' ����� */
      int  start ;        /* ��������� ����� ��� ���������� */
      int  end ;          /* �������� ����� ��� ���������� */
   double  vector[50] ;   /* ������ ���������� �������� ����������� */
   double  interim[50] ;  /* ���������� ������������� ����� */
      int  status  ;
   double  shift ;
      int  i  ;
      int  j  ;
      int  k  ;

/*------------------------------------------------------- ���������� */

                      this->mDraw=0 ;

/*------------------------------------------------------- ���� ����� */

   for(step=mPath_vectors-1 ; step>1 ; step--) {                    /* CIRCLE.1 - �������� � �������� ����. */
                                                                    /*         ����� � �� '����� ����' ... */
/*-------------------------------------- ���� "�����������" �������� */

     for(start=0 ; start<=mPath_vectors-step ; start++) {          /* CIRCLE.2 - �������.����� ���������� */

/*------------------------------------ ������ "������������" ������� */

                                  end=start+step ;

              iCalculateVector(&mPath[start*mPath_frame],           /* ���������� ������� */
                               &mPath[  end*mPath_frame], vector) ;

           status=iTraceVector(&mPath[start*mPath_frame],           /* ���������� ������ */
                               &mPath[  end*mPath_frame],
                                                 interim, vector) ;
 
/*------------------------------------------ "����������" ���������� */

        if(status==0) {                                             /* ���� '��������' ���� ��������� -                    */
                                                                    /*  �������� ������� ���������� �� ������������ ������ */
                 memmove(&mPath[(start+1)*mPath_frame],
                         &mPath[   end   *mPath_frame],
                         (mPath_vectors-end+1)*mPath_frame*sizeof(*mPath)) ;

                 memmove(&mVectors[(start+1)*mPath_frame],
                         &mVectors[   end   *mPath_frame],
                         (mPath_vectors-end+1)*mPath_frame*sizeof(*mVectors)) ;

                for(i=0 ; i<mPath_frame ; i++) 
                         mVectors[start*mPath_frame+i]=vector[i] ;

                                   mPath_vectors-=(step-1) ;
                      }
/*-------------------------------------------------- �������� ������ */
                                                        }           /* CONTINUE.2 */
                                               }                    /* CONTINUE.1 */
/*------------------------------------ ��������� ���������� �������� */

   for(i=0 ; i<mPath_vectors ; i++) {

     for(j=0 ; j<mPath_frame ; j++) 
       if((mPath[(i+1)*mPath_frame+j]-
           mPath[ i   *mPath_frame+j] )*mVectors[i*mPath_frame+j]<0) {

         if(mVectors[i*mPath_frame+j]>0)  shift= 360. ;
         else                             shift=-360. ;

      for(k=i ; k<mPath_vectors ; k++)
                      mPath[(k+1)*mPath_frame+j]+=shift ;
                                                                     }

                                    }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*                    ������������ ����������                        */

   int  RSS_Module_Path::iShowPath(void)

{
      int  start ;
      int  end ;
   double  interim[50] ;  /* ���������� ������������� ����� */
      int  status ;
      int  i ;

/*------------------------------------------------------- ���������� */

/*---------------------------------------------------- ������� ����� */

   for(i=0 ; i<mPath_vectors ; i++) {                               /* CIRCLE.1 */

                                      start=i ;
                                        end=i+1 ;

         status=iTraceVector(&mPath[start*mPath_frame],           /* ���������� ������ */
                             &mPath[  end*mPath_frame],
                                              interim,
                          &mVectors[start*mPath_frame] ) ;
      if(status)  return(-1) ;
                                    }                               /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                    ������ ����������                              */

  void  RSS_Module_Path::iReverse(TRAJECTORY *traj)

{
   double  tmp ;
      int  i ;
      int  j ;
      int  k ;

#define  FRAME  traj->Path_frame

/*------------------------------------------------------- ���������� */

/*-------------------------------------------- ������������ �������� */

   for(i=0 ; i<traj->Path_vectors ; i++) {

           j=traj->Path_vectors-i-1 ;
        if(j<=i)  break ;

      for(k=0 ; k<FRAME ; k++) {
                                  tmp =traj->Vectors[i*FRAME+k] ;
              traj->Vectors[i*FRAME+k]=traj->Vectors[j*FRAME+k] ;
              traj->Vectors[j*FRAME+k]=tmp ;
                               }
                                         }
/*--------------------------------------------- ����� ����� �������� */

   for(i=0 ; i<traj->Path_vectors*FRAME ; i++)
                traj->Vectors[i]=-traj->Vectors[i] ;

/*--------------------------------------- ������������ ������� ����� */

   for(i=0 ; i<traj->Path_vectors+1 ; i++) {

           j=traj->Path_vectors-i ;
        if(j<=i)  break ;

      for(k=0 ; k<FRAME ; k++) {
                                 tmp =traj->Path[i*FRAME+k] ;
                traj->Path[i*FRAME+k]=traj->Path[j*FRAME+k] ;
                traj->Path[j*FRAME+k]=tmp ;
                               }
                                           }
/*-------------------------------------------------------------------*/

#undef  FRAME

   return ;
}


/*********************************************************************/
/*								     */
/*                    ��������� ���������� �������                   */

  void  RSS_Module_Path::iGenerateVector(double *vector)

{
  double  rad ;
  double  a_factor ;
  double  l_factor ;
  double  l_max ;
  double  d_l ;
  double  d_a ;
     int  i ;

/*------------------------------------- ��������� ���������� ������� */

      for(rad=0., i=0 ; i<mPath_frame ; i++) {                      /* ������ ��������� ������ */
        if(mPath_degrees[i].fixed)  vector[i]=  0. ;                /*  � ���������� ��� ����� */
        else                        vector[i]=rand()-RAND_MAX/2 ;                     

                               rad+=vector[i]*vector[i] ;
                                             }

                               rad=sqrt(rad) ;

      for(i=0 ; i<mPath_frame ; i++)  vector[i]/=rad ;              /* ����������� ������ */

/*----------------------------------- ���������� � ���� ������������ */

      for(i=0 ; i<mPath_frame ; i++)                   
        if(mPath_degrees[i].type==_L_TYPE)  vector[i]*=mLineStep ;
        else                                vector[i]*=mAngleStep ;

/*-------------------------------- ����������� ������� ������������� */

                                    a_factor=     0. ;
                                    l_factor=     0. ;
                                    l_max   =     0. ;

    for(i=0 ; i<mPath_frame ; i++)
      if(mPath_degrees[i].type==_L_TYPE) {
        if(mPath_degrees[i].fixed)  l_max+=fabs(mPath_degrees[i].value) ;
        else                        l_max+=fabs(mPath_degrees[i].value_max) ;

                           l_factor+=fabs(vector[i]) ;
                                         }
      else                               {
                           a_factor+=fabs(vector[i]) ;
                                         }
/*--------------------------------------- ����������� ������ ������� */

          d_l=mLineStep/(l_factor+sin(a_factor*3.14/180.)*l_max) ; 
          d_a=mAngleStep/a_factor ; 

       if(d_l>d_a)  d_l=d_a ;

      for(i=0 ; i<mPath_frame ; i++)  vector[i]*=d_l ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*                          ���������� �������                       */

   int  RSS_Module_Path::iCalculateVector(double *start, 
                                          double *end, 
                                          double *vector)

{
  double  rad ;
  double  a_factor ;
  double  l_factor ;
  double  l_max ;
  double  d_l ;
  double  d_a ;
     int  i ;

/*--------------------------------- ������� ���������������� ������� */

      for(rad=0., i=0 ; i<mPath_frame ; i++) {                      /* ���������� ������      */
                                                                    /*  � ��������� ��� ����� */
                              vector[i]=end[i]-start[i] ;

        if(mPath_degrees[i].type   ==_A_TYPE &&                     /* ��� �������������� ������������ �������� */
           mPath_degrees[i].bounded==    0     )                    /*   ��������� �����������                  */
          if(fabs(vector[i])>180.)
            if(vector[i]>0.)  vector[i]-=360. ;
            else              vector[i]+=360. ;
 
                                    rad+=vector[i]*vector[i] ;
                                             }

	               rad=sqrt(rad) ;
                    if(rad==0.) return(-1) ;

      for(i=0 ; i<mPath_frame ; i++)  vector[i]/=rad ;              /* ����������� ������ */

/*-------------------------------- ����������� ������� ������������� */

                                    a_factor= 0. ;
                                    l_factor= 0. ;
                                    l_max   = 0. ;

    for(i=0 ; i<mPath_frame ; i++)
      if(mPath_degrees[i].type==_L_TYPE) {
        if(mPath_degrees[i].fixed)  l_max+=fabs(mPath_degrees[i].value) ;
        else                        l_max+=fabs(mPath_degrees[i].value_max) ;
          
                           l_factor+=fabs(vector[i]) ;
                                         }
      else                               {
                           a_factor+=fabs(vector[i]) ;
                                         }
/*--------------------------------------- ����������� ������ ������� */

          d_l=mLineStep/(l_factor+sin(a_factor*3.14/180.)*l_max) ; 
          d_a=mAngleStep/a_factor ; 

       if(d_l>d_a)  d_l=d_a ;

      for(i=0 ; i<mPath_frame ; i++)  vector[i]*=d_l ;

/*-------------------------------------------------------------------*/

    return(0) ;
}


/*********************************************************************/
/*								     */
/*                     ���������� ����� � ����������                 */

   int  RSS_Module_Path::iAddPathPoint(double *point, 
                                       double *vector)
{
  int  i ;

/*----------------------------------------------- ��������� �������� */

               mPath_vectors++ ;

/*------------------------------------------------- ���������� ����� */
      
       mPath=(double *)
              gMemRealloc(mPath, (mPath_vectors+1)*mPath_frame*sizeof(*mPath),
                            "RSS_Module_Path::iAddPathPoint.mPath", 0, 0) ;
    if(mPath==NULL) {
                      SEND_ERROR("AddPathPoint: Memory over") ;
                        return(-1) ;
                    }

       for(i=0 ; i<mPath_frame ; i++) 
         mPath[mPath_vectors*mPath_frame+i]=point[i] ;

/*----------------------------------------------- ���������� ������� */

       mVectors=(double *)
                 gMemRealloc(mVectors, (mPath_vectors+1)*mPath_frame*sizeof(*mVectors), 
                              "RSS_Module_Path::iAddPathPoint.Vectors", 0, 0) ;
    if(mVectors==NULL) {
                      SEND_ERROR("AddPathPoint: Memory over") ;
                             return(-1) ;
                       }

       for(i=0 ; i<mPath_frame ; i++) 
         mVectors[(mPath_vectors-1)*mPath_frame+i]=vector[i] ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*								     */
/*                      ����������� �������                          */
/*								     */
/*    Return:   0 - ���������� �������� �����                        */
/*              1 - �� �������� �� ������ ����                       */
/*             -1 - �������� �e ����� ������ ����                    */

  int  RSS_Module_Path::iTraceVector(double *start, 
                                     double *end, 
                                     double *last, double *vector)
{
     double  rad_full ;    /* ������ ���������� ���������� ����������� */
     double  rad ;         /* ������� ���������� �� ��������� ����� */
     double  delta ;
     double  opt_coef ;    /* ����. ����������� ���������� */
        int  recalc ;      
  RSS_Point  Top ;         /* ������� ��������� ������� ����� ������� */
  RSS_Point  Top_prv ;
        int  first_step ;  /* ���� ������� ���� */
        int  status ;
        int  i ;

#define    OBJECT      mPath_object

/*-------------------------------- ����������� ���������� ���������� */

   if(end!=NULL) {                                                  /* IF.1 - ���� ������ �������� ����� */

     for(rad_full=0, i=0 ; i<mPath_frame ; i++) {

          if(mPath_degrees[i].fixed)  continue ;

                                         delta =end[i]-start[i] ;   /* ������� �������� */
           if(vector[i]>0. && delta<0.)  delta+=360. ;              /* ���� ����������� �������� */
           if(vector[i]<0. && delta>0.)  delta-=360. ;              /*   ������������ ���        */
	                              rad_full+=delta*delta ;
                                                }

	                              rad_full =sqrt(rad_full) ;
                 }                                                  /* END.1 */
/*------------------------------------------ ����� � ��������� ����� */

      for(i=0 ; i<mPath_frame ; i++)                                /* ������� � ��������� ����� */
           mPath_degrees[i].value=start[i] ; 

               OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
        status=OBJECT->vSolveByJoints() ;                           /* ������ ������ ������ */
     if(status && mCheck)  return(-1) ;                             /* ���� ����� �����������... */

           OBJECT->vGetTargetPoint(&Top_prv) ;                      /* �������� ��������� ������� ����� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ������ ����� */
     if(mCheck) {
                     status=OBJECT->vCheckFeatures(NULL) ;          /* ��������� ������������������ ������� */
                   if(status)  return(-1) ;                         /* ���� ��������� ������������������ �������... */
                }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
     if(mDraw)  this->kernel->vShow(NULL) ;
/*- - - - - - - - - - - - - - - - - - - -  ����������� ������� ����� */
     if(mTraceTop) {
                         status=iSaveTracePoint(&Top_prv) ;         /* ��������� ����� ���������� */
                      if(status)  return(-1) ;
                   }
/*----------------------------------------- ���� �������� �� ������� */

                first_step=1 ;
                  opt_coef=1. ;

   do {                                                             /* CIRCLE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
             for(i=0 ; i<mPath_frame ; i++)                          /* ��������� '�������' ����� */
                           last[i]=mPath_degrees[i].value ;

     do {
             for(i=0 ; i<mPath_frame ; i++)                          /* ������� � ��������� ����������� ����� */
                  mPath_degrees[i].value=last[i]+opt_coef*vector[i] ;

                    OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
             status=OBJECT->vSolveByJoints() ;                      /* ������ ������ ������ */

             OBJECT->vGetTargetPoint(&Top) ;                        /* �������� ��������� ������� ����� */  
                recalc=iOptimiseStep(&Top, &Top_prv, &opt_coef) ;   /* ��������� '�������������' ����� */
                                           
        } while(recalc) ;
/*- - - - - - - - - - - - - - - - �������� ���������� �������� ����� */
     if(end!=NULL) {                                                /* IF.1 - ���� ������ �������� ����� */

       for(rad=0, i=0 ; i<mPath_frame ; i++)                        /* ������������ ���������� ���������� */
         if(mPath_degrees[i].fixed==0) {
                                         delta = mPath_degrees[i].value-start[i] ;
//                                       delta =end[i]-last[i] ;
           if(vector[i]>0. && delta<0.)  delta+=360. ;          
           if(vector[i]<0. && delta>0.)  delta-=360. ;          
	                                   rad+=delta*delta ;
                                       }

              rad=sqrt(rad) ;
           if(rad>rad_full) {  status=0 ;  break ;  }               /* ���� �������� ����� �������� */
                   }                                                /* END.1 */
/*- - - - - - - - - - - - - - - - - - -  �������� ������������ ����� */
          if(status && mCheck)  break ;                             /* ���� ����� �����������... */
/*- - - - - - - - - - - - - - - - -  �������� ��������� ������������ */
     if(end==NULL) {                                                /* IF.2 - ���� �������� ����� �� ������ */
                   }                                                /* END.2 */
/*- - - - - - - - - - - - - - - - - - - - - - ������ ��������� ����� */
     if(mCheck) {
                     status=OBJECT->vCheckFeatures(NULL) ;          /* ��������� ������������������ ������� */
                   if(status)  break ;                              /* ���� ��������� ������������������ �������... */
                }  
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
     if(mDraw)  this->kernel->vShow(NULL) ;
/*- - - - - - - - - - - - - - - - - - - -  ����������� ������� ����� */
     if(mTraceTop) {
                         status=iSaveTracePoint(&Top) ;             /* ��������� ����� ���������� */
                      if(status)  break ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                           Top_prv=Top ;

                        first_step=0 ;
       } while(1) ;                                                 /* CONTINUE.1 */

/*------------------------------------------- ����� � �������� ����� */

   if(end!=NULL && status==0) {                                     /* IF.3 - ���� ������ �������� ����� */
                                                                    /*          � ��� ����������         */
      for(i=0 ; i<mPath_frame ; i++)                                /* ������� � �������� ����� */
           mPath_degrees[i].value=end[i] ; 

               OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
        status=OBJECT->vSolveByJoints() ;                           /* ������ ������ ������ */
     if(status && mCheck)  return(-1) ;                             /* ���� ����� �����������... */

           OBJECT->vGetTargetPoint(&Top) ;                          /* �������� ��������� ������� ����� */  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ������ ����� */
     if(mCheck) {
                     status=OBJECT->vCheckFeatures(NULL) ;          /* ��������� ������������������ ������� */
                   if(status)  return(-1) ;                         /* ���� ��������� ������������������ �������... */
                }  
/*- - - - - - - - - - - - - - - - - - - - - - - -  ����������� ����� */
     if(mDraw)  this->kernel->vShow(NULL) ;
/*- - - - - - - - - - - - - - - - - - - -  ����������� ������� ����� */
     if(mTraceTop) {
                         status=iSaveTracePoint(&Top) ;             /* ��������� ����� ���������� */
                      if(status)  return(-1) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                              }                                     /* END.3 */
/*-------------------------------------------------------------------*/

#undef    OBJECT

   if(status==0 )  return( 0) ;
   if(first_step)  return( 1) ;
                   return(-1) ;
}


/*********************************************************************/
/*								     */
/*             ����������� ������� �������� � ������� �����          */
/*								     */
/*       ������� ������������ � ���� �� ������� ����� ��� �����      */
/*    ��������� �����������.					     */
/*								     */
/*    Return:   0 - ���������� �������� �����                        */
/*             -1 - �������� ����� �� ����������                     */

  int  RSS_Module_Path::iReachVector(RSS_Point *target, int  save_flag)
{
         int  vectors_save ;      /* ����������� �������� ����� �������� */
   RSS_Point  Top ;               /* ������� ��������� ������� ����� ������� */
   RSS_Point  Top_prv ;
// RSS_Point  Top_check ;         /* ��������� ��������� �������� */
      double  vector[50] ;        /* ������ ���������� �������� ����������� */
      double  last[50] ;          /* ��������� '�������' ����������� �� ������� */
      double  interim[50] ;       /* ���������� ������������� ����� */
         int  status ;
         int  dummy_flag ;        /* "������" ������, �.�. ��� ��������� */
         int  stop_flag ;       
         int  done_flag ;         /* ���� ���������� ���� */
         int  fail_cnt ;
         int  jump_flag ;         /* ���� ���������� ������� �� ������� ����� */
         int  jump_steps ;        /* ����� ����� ������� ������� */
         int  check_cnt ;         /* ����� '�����������' �������� ������������������ ������� */
         int  back_step ;         /* ���� ������� �� ��� ����� */
      double  range ;             /* '�������' ��������� */
      double  range_prv ;
      double  v_size ;
      double  opt_coef ;          /* ����. ����������� ���������� */
         int  recalc ;
        char  tmp[1024] ;
         int  i ;

#define    OBJECT      mPath_object

/*------------------------------------------------------- ���������� */

                     vectors_save=mPath_vectors ;

      for(i=0 ; i<mPath_frame ; i++) {                              /* ������� � ��������� ����� */
              interim[i]      =mPath[mPath_vectors*mPath_frame+i] ; 
        mPath_degrees[i].value= interim[i] ;
                                     }

             OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
             OBJECT->vSolveByJoints() ;                             /* ������ ������ ������ � �������� ����� */
             OBJECT->vGetTargetPoint(&Top) ;                        /* �������� ��������� ������� ����� */  

      range_prv=iCalcRange(target, &Top, mLineStep, mAngleStep) ;   /* ������������ '������� ���������' */

/*-------------------------------------------- ����� � ������� ����� */

      sprintf(tmp, "Debug stump %d", mDebug_stamp) ;
       iDebug(tmp, NULL) ;

if(mDebug_stamp==24713) {
                     done_flag=0 ;
                        }

                     done_flag=0 ;
                     stop_flag=0 ;
                    check_cnt =0 ;
                     fail_cnt =0 ;
                     jump_flag=0 ;

     do {                                                           /* CIRCLE.1 - ������� �������� */

                iGenerateVector(vector) ;                           /* ���������� ������ ����������� */

                      dummy_flag=1 ;
                       back_step=0 ;

       do {                                                         /* CIRCLE.2 - �������� �� ������� */
              memmove(last, interim, sizeof(interim)) ;             /* ��������� ������� ����� */
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
               OBJECT->vGetTargetPoint(&Top_prv) ;                  /* �������� ������� ��������� ������� ����� */

           if(     fabs(Top_prv.x   -target->x   )<mLineAcc  &&
                   fabs(Top_prv.y   -target->y   )<mLineAcc  &&
                   fabs(Top_prv.z   -target->z   )<mLineAcc  &&
              iAngleDif(Top_prv.azim-target->azim)<mAngleAcc &&
              iAngleDif(Top_prv.elev-target->elev)<mAngleAcc &&
              iAngleDif(Top_prv.roll-target->roll)<mAngleAcc   ) {
                                   done_flag=1 ;
                                   stop_flag=1 ;
                                        break ;
                                                                 }
/*- - - - - - - - - - - - - - - - - - - - - �������� ���� �� ������� */
                             v_size=range_prv/this->mScanStepCoef  ;
//            if(v_size>1.)  v_size=1. ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
                      opt_coef=1. ;

     do {
             for(i=0 ; i<mPath_frame ; i++) {                        /* ������� � ��������� ����������� ����� */
                        interim[i]      =last[i]+opt_coef*v_size*vector[i] ;
                  mPath_degrees[i].value=     interim[i] ;
                                            }
  
          if(mPath_degrees[0].value> 1000 ||
             mPath_degrees[0].value<-1000   ) {
                                                      status=opt_coef ;
                                              }

//           if(mDebug_stamp==1071)  SEND_ERROR("T_Path: Debug stamp point")  ;
                mDebug_stamp++ ;

                    OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
             status=OBJECT->vSolveByJoints() ;                      /* ������ ������ ������ */
          if(status)  break ;                                       /* ���� ����� �����������... */

             OBJECT->vGetTargetPoint(&Top) ;                        /* �������� ��������� ������� ����� */  

          if(opt_coef==0.) {
                             opt_coef=1. ;
                           }
                recalc=iOptimiseStep(&Top, &Top_prv, &opt_coef) ;   /* ��������� '�������������' ����� */

          if(opt_coef==0.) {
                             opt_coef=1. ;
                           }

          if(opt_coef>1.)  break ;
                                           
        } while(recalc) ;
/*- - - - - - - - - - - - - - - - - - - - - - ������ ��������� ����� */
         if(status) {                                               /* ���� ����� �����������... */
              if(!jump_flag   )  check_cnt++ ;
              if( check_cnt==5)  stop_flag=1 ;                   
                                 back_step=1 ;
                                    break ;  
                    }

             range=iCalcRange(target, &Top, mLineStep, mAngleStep); /* ������������ '������� ���������' */

         if(!jump_flag)
          if(range>range_prv) {                                     /* ���� �� �������� ��������� -      */
                                  fail_cnt++ ;                      /*  ��������������� ���������� ����� */
                                 back_step=1 ;                      /*   � ���������� ������ ������      */
                                      break ;     
                              }
          
         if(jump_flag) {                                            /* �������� ���������� ������� */
                            jump_steps-- ;
                         if(jump_steps<0)  break ;
                       } 

             status=OBJECT->vCheckFeatures(NULL) ;                  /* ��������� ������������������ ������� */
          if(status) {                                              /* ���� ��������� ������������������ �������... */
              if(!jump_flag   )  check_cnt++ ;
              if( check_cnt==5)  stop_flag=1 ;                   
                                 back_step=1 ;
                                    break ;  
                     }                 
          
           if(range!=range_prv)                                     /* ���� ��������� ���������� - */
            if(this->mDraw)  this->kernel->vShow(NULL) ;            /*   - ������������ �����      */

                   range_prv =range ;
                   check_cnt =  0 ;
                    fail_cnt =  0 ;
                   dummy_flag=  0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          } while(1) ;                                              /* CONTINUE.2 */

                           jump_flag=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������� �� ��� ����� */
     if(back_step) {
                       memmove(interim, last, sizeof(interim)) ;

             for(i=0 ; i<mPath_frame ; i++)     
                  mPath_degrees[i].value=interim[i] ;
  
                    OBJECT->vSetJoints    (mPath_degrees, mPath_frame) ;
                    OBJECT->vSolveByJoints() ;                      /* ������ ������ ������ */

                        this->kernel->vShow(NULL) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
     if(!dummy_flag) {
              if(done_flag)  iAddPathPoint(interim, vector) ;       /* ��������� ����� � ���������� */
              else           iAddPathPoint(last,    vector) ;
                     }
/*- - - - - - - - - - - - - - - �������� ���������� �� ����� ������� */
//       if(fail_all>mFailMax ) {
//            status=MessageBox(NULL, "���������� ����������� ����� "
//                                    "��������� �������.\n����������?",
//                                    "����� � �����", 
//                                     MB_ICONQUESTION | MB_YESNO) ;
//         if(status==IDNO)  break ;
//                              }
/*- - - - - - - - - - - - - - - - - - - ������ �� ���������� ������� */
#define  _DIR_ATTEMPTS   10

         if(fail_cnt>_DIR_ATTEMPTS) {
                                        fail_cnt  =0 ;
                                        jump_flag =1 ;
                                        jump_steps=5 ;
#if 0
                iGenerateVector(vector) ;                           /* ���������� ������ ������� */

              for(j=0, i=0 ; i<cnt ; i++)                           /* ������ ��� �� ������� */
                if(degrees[i].fixed==0) {
                 if(degrees[i].type==_L_TYPE) 
                         degrees[i].value+=vector[j]*v_size*l_step*mJampRangeCoef  ;
                 else    degrees[i].value+=vector[j]*v_size*a_step*mJampRangeCoef ;
                                                  j++ ;
                                        }

                     object->vSetJoints     (degrees, cnt) ;        /* ������ ������ �������� ����������� */
              status=object->vSolveByJoints () ;                    /* ������ ������ ������ � �������� ����� */
                     object->vGetTargetPoint(&Top) ;                /* �������� ��������� ������� ����� */  

             range_prv=iCalcRange(&mTarget, &Top, l_step, a_step) ; /* ������������ '������� ���������' */
#endif
                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(stop_flag==0) ;                                     /* CONTINUE.1 */
/*------------------------------------------------- ����� ���������� */

          if(!save_flag)  mPath_vectors=vectors_save ;

/*-------------------------------------------------------------------*/

#undef    OBJECT

  if(done_flag)   return( 0) ;
                  return(-1) ;
}


/*********************************************************************/
/*								     */
/*                     ����������� ����� ������������                */

  void  RSS_Module_Path::iCulculateSteps(void)

{
   double  l_min ;
      int  i ;

/*------------------------------------ ����������� ��������� ������� */

                 l_min=999999. ;

    for(i=0 ; i<mPath_frame ; i++)                                  /* ����������� ������������ */
      if(mPath_degrees[i].type==_L_TYPE &&                          /*     ��������� ���������  */
         mPath_degrees[i].fixed           )       
         if(fabs(mPath_degrees[i].value)<l_min &&
            fabs(mPath_degrees[i].value)>   0.   )  
                    l_min=fabs(mPath_degrees[i].value) ;         

/*----------------------------- ����������� ����� � ������ ��������� */

      if(mAngleStep_user!=0)  mAngleStep=mAngleStep_user ;
      else                    mAngleStep=  10. ;

      if(mLineStep_user !=0)  mLineStep =mLineStep_user ;
      else                    mLineStep =  l_min/10. ;

/*------------------------- ����������� ��������� � ������ ��������� */

      if(mAngleAcc_user!=0)  mAngleAcc=mAngleAcc_user ;
      else                   mAngleAcc= mAngleStep/50. ;

      if(mLineAcc_user !=0)  mLineAcc =mLineAcc_user ;
      else                   mLineAcc = mLineStep/50. ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*								     */
/*                ���������� �������������� ������������             */

  int  RSS_Module_Path::iOptimiseStep(RSS_Point *point_1,
                                      RSS_Point *point_2, 
                                         double *factor  )

{
  static    int  uncicle ;  /* �������������������� ���������� */
         double  x_coef ;
         double  y_coef ;
         double  z_coef ;
         double  a_coef ;
         double  e_coef ;
         double  r_coef ;
         double  coef ;
  static double  coef_prv ;

/*-------------------------- ����������� ������������� ������������� */

        x_coef=fabs(point_1->x-point_2->x)/mLineStep ;
        y_coef=fabs(point_1->y-point_2->y)/mLineStep ;
        z_coef=fabs(point_1->z-point_2->z)/mLineStep ;

          iOrtsDif(point_1, point_2, &a_coef, &e_coef, &r_coef) ;

        a_coef=fabs(a_coef)/mAngleStep ;
        e_coef=fabs(e_coef)/mAngleStep ;
        r_coef=fabs(r_coef)/mAngleStep ;

/*--------------------------- ����������� ������������� ������������ */

                      coef=x_coef ;
      if(y_coef>coef) coef=y_coef ;
      if(z_coef>coef) coef=z_coef ;
      if(a_coef>coef) coef=a_coef ;
      if(e_coef>coef) coef=e_coef ;
      if(r_coef>coef) coef=r_coef ;

/*-------------------------------- ����������� ������� ������������� */

      if(coef==0.) {  uncicle=0 ;  return(0) ;  }

        *factor/=coef ;

      if(coef>0.75 &&
         coef<1.25   ) {  uncicle=0 ;  return(0) ;  }

      if(uncicle)
       if((coef<1. && coef_prv>1.) ||
          (coef>1. && coef_prv<1.)   ) {  *factor=1. ;
                                          uncicle=0 ; 
                                           return(0) ;  }

                 uncicle=  1 ; 
                coef_prv=coef ;

/*-------------------------------------------------------------------*/

   return(-1) ;
}


/*********************************************************************/
/*								     */
/*                    ���������� '�������' ���������                 */

  double  RSS_Module_Path::iCalcRange(RSS_Point *p1, 
                                      RSS_Point *p2,
                                         double  l_step,
                                         double  a_step )
{
   double  x_ort_diff ;
   double  y_ort_diff ;
   double  z_ort_diff ;
   double  rad ;
   double  dst ;


                 rad = 0 ;

                 dst =fabs(p1->x-p2->x)/l_step ;
                 rad+= dst*dst ;

                 dst =fabs(p1->y-p2->y)/l_step ;
                 rad+= dst*dst ;

                 dst =fabs(p1->z-p2->z)/l_step ;
                 rad+= dst*dst ;        

          iOrtsDif(p1, p2, &x_ort_diff, &y_ort_diff, &z_ort_diff) ;

                 rad+=(x_ort_diff*x_ort_diff+
                       y_ort_diff*y_ort_diff+
                       z_ort_diff*z_ort_diff )/(a_step*a_step) ;

                 rad = sqrt(rad) ;

  return(rad) ;
}


/*********************************************************************/
/*								     */
/*        ���������� ������� ������� ����� ������ ����������         */

    void  RSS_Module_Path::iOrtsDif(RSS_Point *p1, 
                                    RSS_Point *p2,
                                       double *x_diff,
                                       double *y_diff,
                                       double *z_diff )
{
   Matrix2d  Orts_1 ;
   Matrix2d  Orts_2 ;
   Matrix2d  Local ;
     double  range ;
     double  dx ;
     double  dy ;
     double  dz ;

/*-------------------------------------- ����������� ��������� ����� */

                 Orts_1.LoadE(4, 4) ;
                  Local.Load4d_azim( p1->azim) ;
                 Orts_1.LoadMul    (&Orts_1, &Local) ;
                  Local.Load4d_elev( p1->elev) ;
                 Orts_1.LoadMul    (&Orts_1, &Local) ;
                  Local.Load4d_roll( p1->roll) ;
                 Orts_1.LoadMul    (&Orts_1, &Local) ;

                 Orts_2.LoadE(4, 4) ;
                  Local.Load4d_azim( p2->azim) ;
                 Orts_2.LoadMul    (&Orts_2, &Local) ;
                  Local.Load4d_elev( p2->elev) ;
                 Orts_2.LoadMul    (&Orts_2, &Local) ;
                  Local.Load4d_roll( p2->roll) ;
                 Orts_2.LoadMul    (&Orts_2, &Local) ;

/*----------------------------------- ����������� ����� ����� ������ */
   
              dx= Orts_1.GetCell(0,0)-Orts_2.GetCell(0,0) ;
              dy= Orts_1.GetCell(0,1)-Orts_2.GetCell(0,1) ;
              dz= Orts_1.GetCell(0,2)-Orts_2.GetCell(0,2) ;
           range=dx*dx+dy*dy+dz*dz ;

         *x_diff=acos(1-0.5*range)*_RAD_TO_GRD ;

              dx= Orts_1.GetCell(1,0)-Orts_2.GetCell(1,0) ;
              dy= Orts_1.GetCell(1,1)-Orts_2.GetCell(1,1) ;
              dz= Orts_1.GetCell(1,2)-Orts_2.GetCell(1,2) ;
           range=dx*dx+dy*dy+dz*dz ;

         *y_diff=acos(1-0.5*range)*_RAD_TO_GRD ;

              dx= Orts_1.GetCell(2,0)-Orts_2.GetCell(2,0) ;
              dy= Orts_1.GetCell(2,1)-Orts_2.GetCell(2,1) ;
              dz= Orts_1.GetCell(2,2)-Orts_2.GetCell(2,2) ;
           range=dx*dx+dy*dy+dz*dz ;

         *z_diff=acos(1-0.5*range)*_RAD_TO_GRD ;

/*-------------------------------------------------------------------*/

}


/*********************************************************************/
/*								     */
/*                    ���������� �������� �����                      */

  double  RSS_Module_Path::iAngleDif(double  angle)
{
  double  dst ;

                 dst =fmod(fabs(angle), 360.) ;
   if(dst>180.)  dst =360.-dst ;

  return(dst) ;
}


/*********************************************************************/
/*								     */
/*                   ���������� ����� ����������                     */

  int  RSS_Module_Path::iSaveTracePoint(RSS_Point *point)
{
/*----------------------------------------------- ����������� ������ */

   if(mTracePoints_cnt==mTracePoints_max) {

        mTracePoints_max+=100 ;
        mTracePoints     =(RSS_Point *)
                           gMemRealloc(mTracePoints,
                                        mTracePoints_max*sizeof(RSS_Point), 
                                         "RSS_Module_Path::iSaveTracePoint::mTracePoints", 0, 0) ;

     if(mTracePoints==NULL) {
                 SEND_ERROR("Memory over for trace line points") ;
                                  return(-1) ;
                            }
                                          }
/*------------------------------------------------- ���������� ����� */

        mTracePoints[mTracePoints_cnt]=(*point) ;
                     mTracePoints_cnt++ ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ����������� ���������� � ��������� ���������         */

  void  RSS_Module_Path::iTopTraceShow_(TRAJECTORY *Trajectory)

{
    strcpy(mContextAction, "SHOW") ;
           mContextObject=Trajectory ;

  SendMessage(RSS_Kernel::kernel_wnd, 
                WM_USER, (WPARAM)_USER_CHANGE_CONTEXT, 
                         (LPARAM) this                ) ;
}


/*********************************************************************/
/*								     */
/*                     ����������� ����������                        */

  void  RSS_Module_Path::iTopTraceShow(TRAJECTORY *Trajectory)

{
       int  status ;
       int  i ;

#define    T  Trajectory

/*-------------------------------- �������������� ����������� ������ */

     if(T->dlist1_idx==0) {

           T->dlist1_idx=RSS_Kernel::display.GetList(2) ;
                          }

     if(T->dlist1_idx==0)  return ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- ��������� ��������� ����������� */

             glNewList(T->dlist1_idx, GL_COMPILE) ;                 /* �������� ������ */
          glMatrixMode(GL_MODELVIEW) ;

/*--------------------------------------------- ��������� ���������� */

              glColor4d(GetRValue(T->TraceColor)/256., 
                        GetGValue(T->TraceColor)/256.,
                        GetBValue(T->TraceColor)/256., 1.) ;

            glBegin(GL_LINE_STRIP) ;

       for(i=0 ; i<T->Trace_cnt ; i++)
         glVertex3d(T->Trace[i].x, T->Trace[i].y, T->Trace[i].z) ;
                                              
              glEnd();

/*----------------------------- �������������� ��������� ����������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef    T
}


/*********************************************************************/
/*								     */
/*                    �������� ���������� �� �����                   */

  void  RSS_Module_Path::iTraceHide(TRAJECTORY *Trajectory)

{
      if(Trajectory->dlist1_idx)
           RSS_Kernel::display.ReleaseList(Trajectory->dlist1_idx) ;

         Trajectory->dlist1_idx=0 ;
}


/********************************************************************/
/*								    */
/*                   ���������� ������ � �����                      */

   void  RSS_Module_Path::iDebug(char *text, char *path,  int  condition)
{
    if(condition)  iDebug(text, path) ;
}


   void  RSS_Module_Path::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         

//return ;

    if(path==NULL) {
                             path="t_path.log" ;
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
/** 	      �������� ������ ������� "����������"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Object_Path::RSS_Object_Path(void)

{
   strcpy(Type, "Path") ;

   Parameters    =NULL ;
   Parameters_cnt=  0 ;

   Trajectory    =NULL ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Object_Path::~RSS_Object_Path(void)

{
}


/********************************************************************/
/*								    */
/*		       ������������ ��������                        */

  void   RSS_Object_Path::vFree(void)

{
   int  i ;

/*------------------------- ������������ ����������� ������ ������� */

#define  C    this->Trajectory->contexts

   if(C!=NULL) {

        for(i=0 ; C[i]!=NULL ; i++)                                /* ���� �������� ����� ������ */
             C[i]->module->vReleaseData(C[i]) ;

                   ProgramModule.gMemFree(C) ;

                                          C=NULL ;
               }
#undef   C

/*------------------------------------- ������������ ������� ������ */

      this->Trajectory->module->iTraceHide(this->Trajectory) ;

                                ProgramModule.gMemFree(this->Trajectory->Path   ) ;
                                ProgramModule.gMemFree(this->Trajectory->Vectors) ;
   if(this->Trajectory->Trace)  ProgramModule.gMemFree(this->Trajectory->Trace  ) ;

                                ProgramModule.gMemFree(this->Trajectory         ) ;

/*------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Object_Path::vWriteSave(std::string *text)

{
  char  field[1024] ;
   int  i ;
   int  j ;
   int  k ;

#define  T  this->Trajectory
#define  D  this->Trajectory->Path_degrees

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT PATH\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n",    T->name             ) ;  *text+=field ;
    sprintf(field, "DESC=%s\n",    T->desc             ) ;  *text+=field ;
    sprintf(field, "MASTER=%s\n",  T->Path_object->Name) ;  *text+=field ;
    sprintf(field, "FRAME=%d\n",   T->Path_frame       ) ;  *text+=field ;
    sprintf(field, "VECTORS=%d\n", T->Path_vectors     ) ;  *text+=field ;

  for(i=0 ; i<T->Path_frame ; i++) {
    sprintf(field, "DEGREE_%d_VALUE=%.10lf\n", i, D[i].value    ) ;  *text+=field ;
    sprintf(field, "DEGREE_%d_V_MIN=%.10lf\n", i, D[i].value_min) ;  *text+=field ;
    sprintf(field, "DEGREE_%d_V_MAX=%.10lf\n", i, D[i].value_max) ;  *text+=field ;
    sprintf(field, "DEGREE_%d_TYPE=%d\n",      i, D[i].type     ) ;  *text+=field ;
    sprintf(field, "DEGREE_%d_FIXED=%d\n",     i, D[i].fixed    ) ;  *text+=field ;
                                   }

  for(k=0, i=0 ; i<T->Path_vectors+1 ; i++) {
                                                 *text+="PATH=" ;
   for(j=0 ; j<T->Path_frame ; j++, k++) {
                                      if(j!=0)  *text+="," ;
        sprintf(field, "%.10lf", T->Path[k]) ;  *text+=field ;
                                         }
                                                 *text+="\n" ;
                                            }

  for(k=0, i=0 ; i<T->Path_vectors   ; i++) {
                                                   *text+="DIR=" ;
   for(j=0 ; j<T->Path_frame ; j++, k++) {
                                         if(j!=0)  *text+="," ;
        sprintf(field, "%.10lf", T->Vectors[k]) ;  *text+=field ;
                                         }
                                                   *text+="\n" ;
                                            }
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef  T
#undef  D

}
