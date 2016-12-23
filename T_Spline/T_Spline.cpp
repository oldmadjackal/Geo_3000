/********************************************************************/
/*								    */
/*          ������ ������ "���������� ���������� ����������"        */
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
/*		    	����������� ������                          */

     static   RSS_Module_Spline  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

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
/**           �������� ������ ������ "������ ����������"           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Spline_instr  
         RSS_Module_Spline_InstrList[]={

 { "help",   "?",  "#HELP (?) - ������ ��������� ������", 
                    NULL,
                   &RSS_Module_Spline::cHelp   },
 { "config", "c",  "#CONFIG (C) - ������� ���������� �����������", 
                    NULL,
                   &RSS_Module_Spline::cConfig   },
 { "drive",  "d",  "#DRIVE (D) - ������� ���������� �������� � �������� �����������", 
                   " DRIVE <��� �������>\n"
                   " DRIVE <��� �������> <����� �������> <Vmax> <Amax> <������� ������� ������>\n",
                   &RSS_Module_Spline::cDrive   },
 { "glide",  "g",  "#GLIDE (G)- ���������� ����������� ���������� ����������",
                   " GLIDE[/AESIQ] <��� ����������>\n"
                   "   A - ��������� ����� �������� �� ������ ������\n"
                   "   E - �������� ������ ������\n"
                   "   S - ��������� ���������� ����������\n"
                   "   I - ���������� ���������� � ��������\n"
                   "   Q - ��������� ��� ����������\n",
                   &RSS_Module_Spline::cGlide },
 {  NULL }
                                       } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Spline_instr *RSS_Module_Spline::mInstrList  =NULL ;
                              long  RSS_Module_Spline::mDebug_stamp=  0 ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

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
/*		        ���������� ������			    */

    RSS_Module_Spline::~RSS_Module_Spline(void)

{
}


/********************************************************************/
/*								    */
/*		      ������������� ������      		    */

  void  RSS_Module_Spline::vInit(void)

{
  int  i ;

/*---------------------------------------------- ����� ������-������ */

#define   MODULES       this->kernel->modules 
#define   MODULES_CNT   this->kernel->modules_cnt 

   for(i=0 ; i<MODULES_CNT ; i++)
     if(!stricmp(MODULES[i].entry->identification, "Path_task")) {
          mModulePath=(class RSS_Module_Path *)MODULES[i].entry ;
                                        break ;
                                                                 }

     if(mModulePath==NULL) {
           MessageBox(NULL, "������ SPLINE ������� ������� ������ PATH", "Geo", 0) ;
                                return ;
                           }
#undef   MODULES
#undef   MODULES_CNT

/*------------------------ ���������� ��������� ������ ������-������ */

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
/*		        ��������� �������       		    */

  int  RSS_Module_Spline::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Spline::vExecuteCmd(const char *cmd, RSS_IFace *iface)

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Spline:") ;
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
       if(status)  SEND_ERROR("������ SPLINE: ����������� �������") ;
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
/*                ���������� ��������� ������                       */

    void  RSS_Module_Spline::vShow(char *layer)

{
}


/*********************************************************************/
/*								     */
/*             ��������� �������� � ��������� ������                 */

    void  RSS_Module_Spline::vChangeContext(void)

{
/*------------------------------------ ����������� ������ ���������� */

   if(!stricmp(mContextAction, "SHOW")) {

                      iErrorPipeShow(mContextObject) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            ������� ������ ��������� ������ �� ������             */

   int  RSS_Module_Spline::vReadData(RSS_Context ***contexts,
                                     std::string   *text     )
{
                       char *buff ;
                        int  buff_size ;
                RSS_Context *context ;
  RSS_Module_Spline_Context *data ;
                       char *entry ;

#define  C   (*contexts)

/*------------------------------------------ ������������� ��������� */

   if(memicmp(text->c_str(), this->identification, 
                      strlen(this->identification)))  return(0) ;

/*------------------------------------- ���������� ������ ���������� */

          context=vAddData(contexts) ;
       if(context==NULL) {                                          /* ���� �� ������� ����������... */
            SEND_ERROR("Spline:������������ ������ ��� ������ ����������") ;
                               return(-1) ;
                         }
/*-------------------------------------- ���������� ������ ��������� */
/*- - - - - - - - - - - - - - - - - - - - - ���������� ������ ������ */
              buff_size=text->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Spline::vReadData.buff", 0, 0) ;

       strcpy(buff, text->c_str()) ;
/*- - - - - - - - - - - - - - - - - - - -  �������� ��������� ������ */
        data=(RSS_Module_Spline_Context *)context->data ;
     if(data==NULL)
        data=(RSS_Module_Spline_Context *)                          /* ���������� ������ ��������� */
               gMemCalloc(1, sizeof(RSS_Module_Spline_Context), 
                              "RSS_Module_Spline::vReadData.context->data", 0, 0) ;
     if(data==NULL) {                                               /* ���� �� ������� ����������... */
            SEND_ERROR("Spline:������������ ������ ��� ��������� ������") ;
                           return(-1) ;
                    }

             context->data=data ;                                   /* ������� ������ ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - �������� ����� */
     entry=strstr(buff, "V_MAX=") ;         data->v_max=atof(entry+strlen("V_MAX=")) ;
     entry=strstr(buff, "A_MAX=") ;         data->a_max=atof(entry+strlen("A_MAX=")) ;

     entry=strstr(buff, "F_ERR=") ;  
            memset(data->f_err, 0, sizeof(data->f_err)) ;
           strncpy(data->f_err, entry+strlen("F_ERR="), sizeof(data->f_err)-1) ;

                   iErrorCalc(data, data->f_err) ;                  /* ����������� ������� */

/*-------------------------------------------- ������������ �������� */

                   gMemFree(buff) ;

/*-------------------------------------------------------------------*/

#undef  C

   return(1) ;
}


/********************************************************************/
/*								    */
/*            �������� ������ ��������� ������ � ������             */

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
/*            ���������� ������� ������ ����������� ������          */

    void  RSS_Module_Spline::vReleaseData(RSS_Context *context)

{
  RSS_Module_Spline_Context *data ;

/*------------------------------------------------- ������� �������� */

     if(context==NULL)  return ;

/*------------------------------------------- �������� ������ ������ */

       data=(RSS_Module_Spline_Context *)context->data ;

     if(data->ErrorPipe !=NULL)  gMemFree(data->ErrorPipe) ;       

     if(data->dlist1_idx!=  0 ) RSS_Kernel::display.ReleaseList(data->dlist1_idx) ;

/*---------------------------------------- ������������ ����� ������ */

        free(context->data) ;
        free(context) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Spline::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - ������������ ������� ����������, "
                                   "���������������� �� �������", "SPLINE") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Spline::cHelp(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Spline_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CONFIG                  */

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
/*		      ���������� ���������� DRIVE                   */
/*								    */
/*  DRIVE <��� �������>                                             */
/*  DRIVE <��� �������> <����� �������> ...                         */
/*                   ...<Vmax> <Amax> <������� ������� ������>      */

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

                         name=pars[0] ;

/*------------------------------------------- �������� ����� ������� */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                     SEND_ERROR("�� ������ ��� ����������.\n"
                                "��������: DRIVE <���_�������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                                object=OBJECTS[i] ;
                                                      break ;  
                                              }

         if(i==OBJECTS_CNT) {                                       /* ���� ������ �� ������... */
             SEND_ERROR("������� � ����� ������ �� ����������") ;
                                return(-1) ;
                            }
/*------------------------- ���������� �������� �������� ����������� */

        mDrives_cnt=object->vGetJoints(mDrives) ;
     if(mDrives_cnt<=0) {
             SEND_ERROR("������ �� ����� �������� �����������") ;
                                return(-1) ;
                        }
/*-------------------------------------- ���������� ��������� ������ */

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* CIRCLE.1 - ���������� ������� ����������� */

     if(mDrives[i].fixed)  continue ;                               /* ���� ������������� �������... */
/*- - - - - - - - - - - - - - - - - - - ���������� ������ ���������� */
        context=vAddData(&mDrives[i].contexts) ;                    /* ���������� ������ ���������� */
     if(context==NULL) {                                            /* ���� �� ������� ����������... */
            SEND_ERROR("������������ ������ ��� ������ ����������") ;
                               return(-1) ;
                       }
/*- - - - - - - - - - - - - - - - - - - -  �������� ��������� ������ */
            sprintf(desc, "RSS_Module_Spline_Context: Object[%s]->drive[%s]", name, mDrives[i].name) ;

        data=(RSS_Module_Spline_Context *)context->data ;
     if(data==NULL)
        data=(RSS_Module_Spline_Context *)                          /* ���������� ������ ��������� */
               gMemCalloc(1, sizeof(RSS_Module_Spline_Context), desc, i, 0) ;
     if(data==NULL) {                                               /* ���� �� ������� ����������... */
            SEND_ERROR("������������ ������ ��� ��������� ������") ;
                           return(-1) ;
                    }

         strcpy(data->name, mDrives[i].name) ;                      /* ������� �������� ������� */

             context->data=data ;                                   /* ������� ������ ��������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }                                 /* CONTINUE.1 */
/*--------------------------------------------------- ������ ������� */

   if(pars[1]!=NULL) {

     do {                                                           /* BLOCK */
/*- - - - - - - - - - - - - -  ���������� ������ ������� ����������� */
               i=strtoul(pars[1], &end, 10) ;                       /* ��������� ����� ������� */
         if(*end!=0)  break ;                                       /* ���� ������������ ������... */

            i-- ;
         if(i<        0   ||
            i>=mDrives_cnt  ) {
                      SEND_ERROR("SPLINE::DRIVE - �������������� ������� �����������") ;
                                  return(-1) ;
                              }

         if(mDrives[i].fixed)  break ;                              /* ���� ������� ����������... */
/*- - - - - - - - - - - - - - - - - - -  ���������� ��������� ������ */
             context= vAddData(&mDrives[i].contexts) ;
                data=(RSS_Module_Spline_Context *)context->data ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
         if(pars[2]!=NULL)  data->v_max=strtod(pars[2], &end) ;     /* ������������ �������� */
         if(*end!=0)  break ;                                       /* ���� ������������ ������... */

         if(pars[3]!=NULL)  data->a_max=strtod(pars[3], &end) ;     /* ������������ ��������� */
         if(*end!=0)  break ;                                       /* ���� ������������ ������... */

         if(pars[4]!=NULL)  strcpy(data->f_err, pars[4]) ;          /* ������� ������� ������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        } while(0) ;                                                /* BLOCK */

           if(*end!=0) {                                            /* ���� ������������ ������... */
                      SEND_ERROR("SPLINE::DRIVE - ������������ ������ ������") ;
                                  return(-1) ;
                       }
                     }
/*----------------------------------------------- ���������� ������� */
   else              {

     DialogBoxIndirectParam(GetModuleHandle(NULL),
                            (LPCDLGTEMPLATE)Resource("IDD_DRIVES", RT_DIALOG),
			      GetActiveWindow(), Task_Spline_Drives_dialog, 
                               (LPARAM)this) ;
                     }
/*------------------------- ���������� �������� �������� ����������� */

         object->vSetJoints(mDrives, mDrives_cnt) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� GLIDE                   */
/*								    */
/*         GLIDE <��� ����������>                                   */

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
                        int   trace_regime ;      /* ����� ����������� ������� */
                     double  *segment_time ;      /* ������ ������ �� ��������� */
                     double   full_time ;
                     double  *v ;                 /* ������ ���������� ��������� �� ��������� */
                     double  *a_available ;       /* ������ ����.��������� �� ��������� */
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

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------- �������� ����������� ���������� */

     if(mTurnZone_coef<2) {
         SEND_ERROR("Spline.Glide:�������� ����������� ����� ����������� ������� �����������."
                    "���������� ����������� 5.") ;
                               mTurnZone_coef=5 ;
                          }
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                                mAdjustErrorPipe=0 ;
                                  mShowErrorPipe=0 ;
                                     mSaveGlided=0 ;
                                       mIndicate=0 ;
                                      quiet_flag=0 ;

     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("Spline.Glide:������������ ������ �������") ;
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

/*------------------------- ���������� �������� �������� ����������� */

        mDrives_cnt=master->vGetJoints(mDrives) ;
     if(mDrives_cnt<=0) {
             SEND_ERROR("������ �� ����� �������� �����������") ;
                                return(-1) ;
                        }
/*------------------------------------ ����������� ������ ���������� */

        status=iCheckPath(path_object) ;                            /* ����������� ����������� ���������� */
     if(status) {
       if(iface==NULL)  SEND_ERROR("������� ���������� �����������") ;
                    iface->vSignal("ERROR",
                                   "������� ���������� �����������") ;
                                return(-1) ;
                }
/*------------------------------------------ �������� ������� ������ */

#define  C     mDrives[i].contexts

    for(i=0 ; i<mDrives_cnt ; i++) {                                /* CIRCLE.1 - ���������� ������� ����������� */

       if(mDrives[i].fixed)  continue ;                             /* ���� ������������� �������... */

       if(mDrives[i].contexts==NULL) {                              /* ���� ������ ������ ���������� */
             SEND_ERROR("� ������� �� ������ ��������� ��������.\n"
                        "����������� ������� SPLINE DRIVE <��� �������>.") ;
                                         return(-1) ;
                                     }

      for(j=0 ; C[j]!=NULL ; j++)                                   /* ���� �������� ����� ������ */
        if(!strcmp(C[j]->name, this->identification))  break ;

        if(C[j]==NULL) {
             SEND_ERROR("� ������� �� ������ ��������� ��������.\n"
                        "����������� ������� SPLINE DRIVE <��� �������>.") ;
                                         return(-1) ;
                       }

               data=(RSS_Module_Spline_Context *)C[j]->data ;

        if(data->v_max<=0. ||
           data->a_max<=0.   ) {
             SEND_ERROR("� ������� �� ������ ������������ ��������/��������� ��������.\n"
                        "����������� ������� SPLINE DRIVE <��� �������>.") ;
                                         return(-1) ;
                               }

                                        drives[i]=data ;
                                   }
#undef  C

/*-------------------------------------- �������� ������� ���������� */

          work_object=iCopyPath(path_object, _TEMP_NAME) ;          /* ������� ����� �������� ���������� */

/*---------------------------- ����������� �������������� ���������� */

#define  P   path_object->Trajectory

                this->mModulePath->mPath_object=P->Path_object ;
         memcpy(this->mModulePath->mPath_degrees, P->Path_degrees,           
                               P->Path_frame*sizeof(P->Path_degrees[0])) ;

               this->mModulePath->iCulculateSteps() ;

#undef  P

/*---------------------------- ����������� ���������� ���������� ��� */

     if(mDebug_stamp==7708) {
//                              SEND_ERROR("T_Spline: iTurnZone_check")  ;
                            }

          status=iTurnZone_check(work_object, path_object) ;
       if(status==-1)  return(0) ;

//                iDrawPath(work_object, master) ;

#define  W   work_object->Trajectory

/*-------------------------------------- ���������� ������� �������� */

       v           =(double *)gMemCalloc((W->Path_vectors+1)*W->Path_frame, sizeof(double), 
                                                   "RSS_Module_Spline::cGlide.v", 0, 0) ;
       a_available =(double *)gMemCalloc( W->Path_vectors,                  sizeof(double),
                                                   "RSS_Module_Spline::cGlide.a_available", 0, 0) ;
       segment_time=(double *)gMemCalloc( W->Path_vectors,                  sizeof(double), 
                                                   "RSS_Module_Spline::cGlide.segment_time", 0, 0) ;

         for(j=0 ; j<W->Path_vectors ; j++)  a_available[j]=1. ;

/*------------------------------- ���.����������� �������� ��������� */

   for(i=0 ; i<mDrives_cnt ; i++) {

    for(range_max=0, j=0 ; j<W->Path_vectors ; j++) {

          range=fabs( W->Path[(j+1)*W->Path_frame+i]-
                      W->Path[(j  )*W->Path_frame+i] ) ;
       if(range>range_max)  range_max=range ;
                                                    }

                 mAccuracy[i]=range_max/1000. ;
                                  }
/*------------------------------------------------ ������ ���������� */

                                   mErrorPipe    =NULL ;            /* ����. ������ ������ ������ */
                                   mErrorPipe_cnt=  0 ;
                                   mErrorPipe_max=  0 ;

                                     trace_regime=_TIME_MINIMIZE ;

                                         mAttempt=  0 ;

   for(n=0 ; n<W->Path_vectors ; n++) {                             /* CIRCLE.1 - ������� �������� ���������� */

         mAttempt++ ;
//    if(mAttempt==6)  SEND_ERROR("Critical attempt reached!") ;

                sprintf(text, "������� %d", n) ;
                 iDebug(text, NULL) ;

           memset(&drive_forces[n], 0, sizeof(drive_forces[n])) ;
/*- - - - - - - - - - - - - - - - - - - ��������� ��������� �������� */
       if(mIndicate) {
                          sprintf(text, "Vectors:%3d\r\n"
                                        "Vector :%3d\r\n"
                                        "Coef   :%lf\r\n"
                                        "Attempt:%5d\r\n",
                                          W->Path_vectors, n, a_available[n], mAttempt) ; 
                        SEND_INFO(text) ;
                     } 
/*- - - - - - - - - - - - - - - -  ���.������ �������� �� ���������� */
        if(n==0) {                                                  /* ��������� ������� */
                           iDebug("iLineSegment.start", NULL) ;
              status=iLineSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }
        else
        if(n==W->Path_vectors-1) {                                  /* �������� ������� */

                           iDebug("iLineSegment.finish", NULL) ;
              status=iLineSegment(_V_CORRECTION, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                                 }
        else
        if(n%2)  {                                                  /* ��������� ������ �������� */

              if(trace_regime!=_V_CORRECTION)                       /* ���� �� ��������� �� �������� �������� - */
                  for(j=0 ; j<W->Path_frame ; j++)                  /*  - ������ ������ "�������� ��������"     */ 
                       V2(n)[j]=(W->Vectors+(n+1)*W->Path_frame)[j] ;

                           iDebug("iTurnSegment", NULL) ;
              status=iTurnSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }
        else     {                                                  /* ������ ������� */
                           iDebug("iLineSegment", NULL) ;
              status=iLineSegment(trace_regime, &segment_time[n],
                                     W->Path+(n  )*W->Path_frame,
                                     W->Path+(n+1)*W->Path_frame,
                                        a_available[n],
                                         V1(n), &drive_forces[n], V2(n), drives) ;
                 }

                           trace_regime =_TIME_MINIMIZE ;
/*- - - - - - - - - - - - - - - - - - - ��������� �������� ��������� */
         if(status) {
                           iDebug("BackCorrection", NULL) ;

           if(status==_V_CORRECTION) {
                  sprintf(text, "�������� ��������� �������� �� ������� %d", n) ;
                   iDebug(text, NULL) ;
                                        trace_regime =_V_CORRECTION ;
                                       a_available[n]= 1. ;
                                                   n-= 2 ;
                                     }
           else                      {
                  sprintf(text, "����������� ��� ��������� %d �� ������� %d (������� %d)", status, n, mAttempt) ;
               SEND_ERROR(text) ;
                                     }
 
                        continue ;
                    }

            sprintf(text, "����� ����������� ������� %lf", segment_time[n]) ;
//       SEND_ERROR(text) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */
                    iCutErrorPipe(n) ;                              /* �������� ������ ����������� ��������� */

             status=iTraceSegment(n, segment_time[n], 
                                   W->Path+n*W->Path_frame,
                                     V1(n), &drive_forces[n], 
                                               drives, master) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  ������ ������ */
          if(status==_FATAL_ERROR) {                                /* ���� ����������� ������... */
                sprintf(text, "����������� ������ �� ������� %d ", n) ;
                 iDebug(text, NULL) ;

              iface->vSignal("ERROR", "�����������") ;
                                        break ;               
                                   }

          if(status==_PIPE_FAIL) {                                  /* ���� �������� ������... */
                sprintf(text, "��������� �� ��������� %lf �� ������� %d ", a_available[n], n) ;
                 iDebug(text, NULL) ;

                                    a_available[n]*=0.95 ;
                                                  n-- ; 
                                               continue ;
                                 }
 
          if(status==_PATH_FAIL) {                                  /* ���� �������� ����������... */

                if(n==0) {
                              sprintf(text, "�������� ���������� �� ��������� ������� (������� %d)", mAttempt) ;
                           SEND_ERROR(text) ;

                             iface->vSignal("ERROR", "�����������") ;
                                        break ;               
                         }

                sprintf(text, "��������� �� �������� �� ������� %d", n) ;
                 iDebug(text, NULL) ;

                  for(j=0 ; j<W->Path_frame ; j++)  V1(n)[j]*=0.95 ;

                                        trace_regime =_V_CORRECTION ;
                                       a_available[n]= 1. ;
                                                   n-= 2 ;

                                               continue ;
                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      }                             /* CONTINUE.1 */
/*------------------------------------- ����������� ��������� ������ */

/*- - - - - - - - - - - - - - - ��������/���������� ��������� ������ */
          context=vAddData(&path_object->Trajectory->contexts) ;
       if(context==NULL) {                                          /* ���� �� ������� ����������... */
            SEND_ERROR("������������ ������ ��� ������ ����������") ;
                               return(-1) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - -  ������� ������ ������ */
     if(context->data!=NULL) {

          data=(RSS_Module_Spline_Context *)context->data ;

                                gMemFree(data->ErrorPipe) ;       
                             }
/*- - - - - - - - - - - - - - - - -  ����������� ������ ����� ������ */
     else                    {

            sprintf(desc, "RSS_Module_Spline_Context: Object[%s]", work_object->Name) ;

          data=(RSS_Module_Spline_Context *)context->data ;
       if(data==NULL)
          data=(RSS_Module_Spline_Context *)                        /* ���������� ������ ��������� */
                 gMemCalloc(1, sizeof(RSS_Module_Spline_Context),
                             desc, 0, 0) ;
       if(data==NULL) {                                             /* ���� �� ������� ����������... */
            SEND_ERROR("������������ ������ ��� ��������� ������") ;
                           return(-1) ;
                      }
                             }
/*- - - - - - - - - - - - - - - - - - ���������� ������ �� ��������� */
             context->data=data ;                                   /* ������� ������ ������ ������ � �������� */
                           data->ErrorPipe     =mErrorPipe ; 
                           data->ErrorPipe_cnt =mErrorPipe_cnt ; 
                           data->ErrorPipeColor=mErrorPipeColor ; 

/*------------------------------------ ��o�������� ������ ���������� */

       for(full_time=0., i=0 ; i<W->Path_vectors ; i++)             /* ���������� ����� ����� ����������� */
              full_time+=segment_time[i] ;

            sprintf(text, "����� �����: %lf\r\n", full_time) ;

       for(i=0 ; i<W->Path_vectors-1 ; i+=2) {                      /* ��������� ������ ���������� */
            sprintf(tmp, "%2d  L:%.2lf  T:%.2lf\r\n", i+1, segment_time[i], segment_time[i+1]) ;
             strcat(text, tmp) ;
                                             }

            sprintf(tmp, "%2d  L:%.2lf", W->Path_vectors, segment_time[W->Path_vectors-1]) ;
             strcat(text, tmp) ;

     if(!quiet_flag)                                                /* ���� �� ����� "������" */
        SendMessage(this->kernel_wnd, WM_USER,                      /*  ������ ������ �� ����� */
                    (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

/*----------------------- ������ ���������� � ������������ ��������� */

                     sprintf(text, "%-20.10lf:", full_time) ;

    if(W->Path_vectors==1) {
                     sprintf(tmp, "%.4lf", segment_time[0]) ;
                      strcat(text, tmp) ;
                           }
    else                   {
       for(i=0 ; i<W->Path_vectors-1 ; i+=2) {                      /* ��������� ������ ���������� */
           if(i==0)  sprintf(tmp, "%.4lf,", segment_time[i]+segment_time[i+1]/2.) ;
           else      sprintf(tmp, "%.4lf,", segment_time[i]+segment_time[i-1]/2.+segment_time[i+1]/2.) ;
                      strcat(text, tmp) ;
                                             }

                                                        i=W->Path_vectors-1 ;
                     sprintf(tmp, "%.4lf", segment_time[i]+segment_time[i-1]/2.) ;
                      strcat(text, tmp) ;
                           }
 
                iface->vSignal("DONE", text) ;                      /* ������ ��������� �� ���������� ������������ ����� */

#undef   W

/*-------------------------------------- ��������� ������ ���������� */

     if(!quiet_flag) {

            iErrorPipeShow_(path_object->Trajectory) ;

                           this->kernel->vShow(NULL) ;
                     }
/*-------------------------------------- �������� ������� ���������� */

                             sprintf(text, "kill %s", _TEMP_NAME) ;
           this->kernel->vExecuteCmd(text) ;

/*-------------------------------------------- ������������ �������� */

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
/*		      ��������� ����������                          */

  int  RSS_Module_Spline::iDrawPath(RSS_Object_Path *path_object,
                                         RSS_Object *master      )

{
   char  cmd[1024] ;

                           sprintf(cmd, "Path color %s green",      /* ���������� ����� */
                                            path_object->Name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

                           sprintf(cmd, "Path indicate %s %s",      /* ��������� ���� */
                                            path_object->Name, 
                                                 master->Name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

  return(0) ;
}


/********************************************************************/
/*								    */
/*		 ����������� ����������� ����������                 */

  int  RSS_Module_Spline::iCheckPath(RSS_Object_Path *path_object)

{
       char  cmd[1024] ;
  RSS_IFace  iface ;
       char  tmp[1024] ;

/*------------------------------------------------------- ���������� */

                  memset(tmp, 0, sizeof(tmp)) ;
 
/*------------------------------------------- ����������� ���������� */

                           sprintf(cmd, "Path trace %s", path_object->Name) ;
    this->mModulePath->vExecuteCmd(cmd, &iface) ;

/*---------------------------------------------------- ������ ������ */

      if(!iface.vCheck("DONE"))  return(-1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*		      ����������� ����������                        */

  RSS_Object_Path *
       RSS_Module_Spline::iCopyPath(RSS_Object_Path *path_object,
                                               char *new_name    )

{
   char  cmd[1024] ;
    int  i ;

/*------------------------------------------- ����������� ���������� */

                           sprintf(cmd, "Path copy/o %s %s", 
                                           path_object->Name, 
                                                    new_name ) ;
    this->mModulePath->vExecuteCmd(cmd) ;

/*------------------------------------------------------ ����� ����� */

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
/*                 ����������� ���������� ���������� ���            */

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

/*----------------------------------------------- ����������� ������ */

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
           SEND_ERROR("������������ ������ ��� ���������� ����������") ;
                                            return(-1) ;
                           }
/*----------------------------------- ������ ���������� � 1 �������� */

    if(W->Path_vectors==1) {
                               SEND_ERROR("���������� � ����� ��������!") ;
                                            return(-1) ;
                           }
/*---------------------- ���������� ���������� �� ���������� ������� */

   for(i=1 ; i<B->Path_vectors ; i++) {                             /* CIRCLE.1 - �� ����� "���������� �����" */

     for(zone=this->mTurnZone_coef ; ; zone*=2.) {                  /* CIRCLE.2 - ���������� ����� ����������� ������� */
                                                                    /*               ������� �� ������������           */
/*- - - - - - - - - - - - - - - - - - -  �������� ���������� ������� */
       for(j=0 ; j<B->Path_frame ; j++) {                           /* ���������� ����� ������ � ����� �������� */
           WP(2*i-1,j)=BP(i,j)+(BP(i-1,j)-BP(i,j))/zone ;
           WP(2*i,  j)=BP(i,j)+(BP(i+1,j)-BP(i,j))/zone ;
                                        }

        this->mModulePath->iCalculateVector(&WP(2*i-1,0),           /* ������ ������ �������� */
                                            &WP(2*i  ,0), 
                                            &WV(2*i-1,0) ) ;
/*- - - - - - - - - - - - - - - - - - - - -  ��������� ����� ������� */
                this->mModulePath->mPath_object=B->Path_object ;
                this->mModulePath->mPath_frame =B->Path_frame ;
         memcpy(this->mModulePath->mPath_degrees, B->Path_degrees,
                               B->Path_frame*sizeof(B->Path_degrees[0])) ;

                this->mModulePath->mDraw    =0 ;                    /* �� ������ ������ */
                this->mModulePath->mTraceTop=0 ;                    /* �� ������ ������� ����� */
                this->mModulePath->mCheck   =1 ;                    /* ��������� �������� */

         status=this->mModulePath->iTraceVector(&WP(2*i-1, 0),      /* ���������� ������ �� ����� ������� */
                                                &WP(2*i,   0),
                                                 dummy,
                                                &WV(2*i-1, 0) ) ;
      if(status!=-1)  break ;                                       /* ���� ������ ������� - ���� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                 }                  /* CONTINUE.2 */

                                      }                             /* CONTINUE.1 */
/*------------------------------------ ���������� ����������� ������ */

     for(j=0 ; j<W->Path_frame   ; j++)                             /* ������� �������� ����� */
           WP(W->Path_vectors,j)=BP(B->Path_vectors,j) ;

     for(i=0 ; i<W->Path_vectors ; i++)                             /* ������� �������� �������� */
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
/*               ���������� �������� �� �������� �������            */
/*                                                                  */
/*   �������� �������� �� ����� �� ��������� ����:                  */
/*     - �������� � ���������� ����������                           */
/*     - �������� � ���������� ���������� �� ������������ ��������  */
/*           ����� �� ��������, ����� - � ���������� ���������      */
/*                                                                  */
/*   ����� _TIME_MINIMIZE - ������� �� ������������ �������         */
/*                            ����������� �������                   */
/*            ������� ��������� - start, finish, v1                 */
/*                                                                  */
/*   ����� _V_CORRECTION - ������� �� ������������ �������          */
/*                            ����������� ������� ��� ������������  */
/*                              ���������� �������� ��������        */
/*            ������� ��������� - start, finish, v1, v2             */
/*                                                                  */
/*   Return: ����� ����������� �������                              */

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
  double  v_available ;      /* ����������� "�������������" ���������� �������� */
  double  L[50] ;
  double  Amax[50] ;         /* ���������� ��������� */
  double  Vmax[50] ;         /* ���������� �������� */
  double  Lcr ;
  double  K ;
  double  K_r ;
  double  K_t ;
  double  t ;
  double  t_r ;
  double  t_t ;
  double  t_cr ;
  double  Vo ;
  double  Vo_correction ;    /* ����.��������� ��������� �������� */
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


/*----------------------- ����������� ���������-���������� ��������� */

          v_available=a_available ;

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* ���������-���������� ��������    */
                                                                    /*   �� ����� ���� ������ ��������� */
       if(drives[i]==NULL)  continue ;

       if(V_MAX(i)<fabs(v1[i])) {
              v_available=1.001*fabs(v1[i])/drives[i]->v_max ;
                        i= -1 ;
                                }
                                  }
/*-------------------------------------------- ����������� ��������� */

   for(i=0 ; i<mDrives_cnt ; i++)
           if(drives[i]!=NULL)  L[i]=fabs(finish[i]-start[i]) ;

/*--------------------------------- ����������� ���������� ��������� */

            i_base=-1 ;

   for(i=0 ; i<mDrives_cnt ; i++) {

           if(drives[i]==NULL)  continue ;                          /* ��������� ������� ���������� */
           if(     L[i]==  0.)  continue ;                          /* ����������� ������� ���������� */

           if(i_base==-1)  i_base=i ;

              K=A_MAX(i_base)*L[i]/(A_MAX(i)*L[i_base]) ;           /* ���. ����.���������� ��������� */
           if(K>1.)  i_base=i ;                                     /* � �������� ������ �������� �������      */
                                                                    /*  � ���������� ����.���������� ��������� */
                                  }

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* ���.������������ ������������� ��������� */
          if(     L[i]==  0.)  Amax[i]= 0. ;
     else if(drives[i]!=NULL)  Amax[i]=A_MAX(i_base)*L[i]/L[i_base] ;
                                  }
/*---------------------------------- ����������� ���������� �������� */

            i_base=0 ;

   for(i_base=0, i=0 ; i<mDrives_cnt ; i++) {                       /* ����������� ������� ������� */

           if(drives[i]==NULL)  continue ;                          /* ��������� ������� ���������� */
           if(     L[i]==  0.)  continue ;                          /* ����������� ������� ���������� */

           if(i_base==-1)  i_base=i ;
      
              K=V_MAX(i_base)*L[i]/(V_MAX(i)*L[i_base]) ;           /* ���. ����.���������� �������� */
           if(K>1.)  i_base=i ;                                     /* � �������� ������ �������� �������     */
                                                                    /*  � ���������� ����.���������� �������� */
                                            }

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* ����������� ���������� �������� */
          if(     L[i]==  0.)  Vmax[i]= 0. ;
     else if(drives[i]!=NULL)  Vmax[i]=V_MAX(i_base)*L[i]/L[i_base] ;
                                  }
/*-------------------------------- �������� ������� ������� �������� */

   for(i=0 ; i<mDrives_cnt ; i++) {

       if(drives[i]==NULL)  continue ;                          /* ��������� ������� ���������� */

       if(v1[i]==0. && v1[i_base]==0.)  continue ;

            c_v= v1[i]/v1[i_base] ;
            c_l=(finish[i]-start[i])/(finish[i_base]-start[i_base]) ;
       if( fabs(c_v-c_l)>0.001*(fabs(c_v)+fabs(c_l)) ) {
                  SEND_ERROR("������������ ������� �������� �� �������� �������") ;
                                        break ;
                                                       }
                                  }
/*------------------------- ������� �� �������� ������������ ������� */

   if(regime==_TIME_MINIMIZE) {
/*- - - - - - - - - - - ������� ��������� ���������� ������.�������� */
                i=i_base ;
              Lcr=(Vmax[i]*Vmax[i]-v1[i]*v1[i])/(2.*Amax[i]) ;      /* ���. ��������� ������� */
                K=Lcr/L[i] ;                                        /* ���. ����.������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - -  - ���.������ �������� */
                         i =i_base ;

       if(K<1.) {                                                   /* ������� �������������� ������ */
                      Vo=fabs(v1[i]) ;
                      t_cr=(Vmax[i]-Vo)/Amax[i] ;
         if(t_cr<0.)  t_cr=0. ;
                      t   =t_cr+L[i]*(1.-K)/Vmax[i] ;
                }
       else     {                                                   /* ������� �������������� ������ */
                      Vo  =fabs(v1[i]) ;
                      t   =(-Vo+sqrt(Vo*Vo+2.*Amax[i]*L[i]))/Amax[i] ;
                      t_cr=  t ;
                }
/*- - - - - - - - - - - - - - - - - -  ������� ��������� � ��������� */
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
/*---------------------------- ������� �� �������� �������� �������� */

   if(regime==_V_CORRECTION) {
/*- - - - - - - - - - - - - - �������� ���������� ��������� �������� */
     do {
             i =i_base ;

          if(fabs(v1[i])<=fabs(v2[i]))  break ;                     /* ���� �� ���� � ��������, �� �� �������������,      */
                                                                    /*  �.�. ����� ������� "�� �������� ��������" ����    */
                                                                    /*   � ����������� v2, ������������� "�� min �������" */                    
             Vo=sqrt(2.*Amax[i]*L[i]+v2[i]*v2[i]) ; 
          if(Vo<fabs(v1[i])) {                                      /* ���� ��������� �������� ������� ������ - */
                                 Vo_correction=fabs(Vo/v1[i]) ;     /*   - ��������� �� �� ����������           */ 
                                                                    /*       � ���� �� ���������� �������       */
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) v1[j]*=Vo_correction*0.95 ;

                                  return(_V_CORRECTION) ;
                             }
        } while(0) ;
/*- - - - - - - - - - - - - - - - - - - - ���.������������� �������� */
                                      i     = i_base ;
                                  v_coef    = 1. ;
                                  v_coef_prv=fabs(v1[i]/Vmax[i]);   /* ���.����.��������� �������� */

     for(n=0 ; n<20 ; n++) {                                        /* CIRCLE.1 */
 
          Vo =Vmax[i]*v_coef ;                                      /* ���.������������� �������� */
          Lcr=fabs(Vo*Vo-v1[i]*v1[i])/(2.*Amax[i]) ;                /* ���.��������� "�������" */
          K_r=Lcr/L[i] ;                                            /* ���.����.������� "�������" */
          Lcr=fabs(Vo*Vo-v2[i]*v2[i])/(2.*Amax[i]) ;                /* ���.��������� "����������" */
          K_t=Lcr/L[i] ;                                            /* ���.����.������� "����������" */

        if(K_r+K_t<=1. && v_coef==1.)  break ;                      /* ���� ���������� ���������� ��������... */ 

                          temp =v_coef ;
        if(K_r+K_t>1.)  v_coef-=fabs(v_coef-v_coef_prv)/2. ;        /* � ����������� �� ������������� -    */
        else            v_coef+=fabs(v_coef-v_coef_prv)/2. ;        /*  ����������� ��� ��������� �������� */

                        v_coef_prv=temp ;

                           }                                        /* CONTINUE.1 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
                       i= i_base ;
                     Lcr=fabs(Vo*Vo-v1[i]*v1[i])/(2.*Amax[i]) ;     /* ���.��������� "�������" */
                     K_r=Lcr/L[i] ;                                 /* ���.����.������� "�������" */
                     Lcr=fabs(Vo*Vo-v2[i]*v2[i])/(2.*Amax[i]) ;     /* ���.��������� "����������" */
                     K_t=Lcr/L[i] ;                                 /* ���.����.������� "����������" */

                     t_r=fabs(fabs(Vo)-fabs(v1[i]))/Amax[i] ;
         if(t_r<0.)  t_r=0. ;

                     t_t=fabs(fabs(Vo)-fabs(v2[i]))/Amax[i] ;

                     t  =t_r+t_t+L[i]*(1.-K_r-K_t)/fabs(Vo);
/*- - - - - - - - - - - - - - - - - -  ������� ��������� � ��������� */
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
/*--------------------------------------------- ����������� �������� */

     for(i=0 ; i<mDrives_cnt ; i++) {

        if(drives[i]==NULL)  continue ;

        if(fabs(forces[i].a1)>A_MAX(i)*1.001) {
                                     sprintf(tmp, "SPLINE.iLineSegment@"
                                                  "���������� ������������� "
                                                  "��������� (%d:%d)", mAttempt, i) ;
                                  SEND_ERROR(tmp) ;
                                                    break ;
                                              }
//      if(fabs(v2[i])>V_MAX(i)*1.001) {
//                      SEND_ERROR("���������� ������������ ��������") ;
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
/*          ���������� �������� �� ��������� ������ ��������        */
/*                                                                  */
/*   ����� _TIME_MINIMIZE - ������� �� ������������ �������         */
/*                            ����������� �������                   */
/*            ������� ��������� - start, finish, v1, ������ v2      */
/*                                                                  */
/*   ����� _V_CORRECTION - ������� �� ������������ �������          */
/*                            ����������� ������� ��� ������������  */
/*                              ���������� �������� ��������        */
/*            ������� ��������� - start, finish, v1, v2             */
/*                                                                  */
/*   Return: ����� ����������� �������                              */

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
  double  v_available ;      /* ����������� "�������������" ���������� �������� */
  double  Vend[50] ;         /* �������������� �������� �������� */
  double  Vk_max ;           /* ���������� �������� �������� ������� */
  double  Vo_max ;           /* ���������� ��������� �������� ������� */
  double  Vk_correction ;    /* ����.��������� �������� �������� */
  double  Vo_correction ;    /* ����.��������� ��������� �������� */
  double  L ;                /* ��������� */
  double  Lt;                /* ��������� ���������� */
  double  Lr;                /* ��������� ������� */
  double  Vk ;               /* �������� �������� */
  double  Vo ;               /* ��������� �������� */
  double  Tmin[50] ;         /* ����������� ��������� ����� ������� ������� �������� */
  double  t ;
  double  t_2 ;
  double  t_3 ;
  double  T ;                /* ����� ����� ����������� ������� */
  double  Tr;                /* ����� ������� */
  double  Tt;                /* ����� ���������� */
  double  K ;
  double  a ;                /* ��������� */
  double  sgn ;              /* ���� �������� */
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

/*--------------------------------------- ������ � ���������� ������ */

                                                   mDebug_stamp++ ;
         sprintf(tmp, "Debug stamp %ld (heap %d)", mDebug_stamp, _heapchk()) ;
          iDebug(tmp, NULL) ;

//     if(mDebug_stamp==12499)  SEND_ERROR("T_Spline: Debug stamp point")  ;

/*----------------------- ����������� ���������-���������� ��������� */

          v_available=a_available ;

   for(i=0 ; i<mDrives_cnt ; i++) {                                 /* ���������-���������� ��������    */
                                                                    /*   �� ����� ���� ������ ��������� */
       if(drives[i]==NULL)  continue ;

       if(V_MAX(i)<fabs(v1[i])) {
              v_available=1.001*fabs(v1[i])/drives[i]->v_max ;
                        i= -1 ;
                                }
                                  }
/*------------------------- ����������� ���������� �������� �������� */


   for(i_base=0, i=0 ; i<mDrives_cnt ; i++) {                       /* ����������� ������� ������� */

       if(drives[i]==NULL)  continue ;                              /* ��������� ������� ���������� */
      
               K=V_MAX(i_base)*v2[i]/(V_MAX(i)*v2[i_base]) ;        /* ���. ����.���������� �������� */
       if(fabs(K)>1.)  i_base=i ;                                   /* � �������� ������� �������� �������    */
                                                                    /*  � ���������� ����.���������� �������� */
                                            }
/*- - - - - - - - - - - - - ����������� ���������� �������� �������� */
  if(regime==_V_CORRECTION) {                                       /* ��� ��������� �� �������� �������� �   */
                                                                    /*  �������� ���������� �������� �������� */
         for(i=0 ; i<mDrives_cnt ; i++)                             /*   ����� �������� �������� ��������     */
            if(drives[i]!=NULL)  Vend[i]=v2[i] ;
                            }
  else                      {

         for(i=0 ; i<mDrives_cnt ; i++)
           if(drives[i]!=NULL)
             Vend[i]=V_MAX(i_base)*v2[i]/fabs(v2[i_base]) ;
                            }
/*------------------------------------------ ������� ���������� ���� */

     do {                                                           /* CIRCLE.0 */

/*---------------------- ����������� ���������� ��������� ���������� */

                    Vk_correction=0. ;
                    Vo_correction=0. ;
 
       for(i=0 ; i<mDrives_cnt ; i++) {                             /* CIRCLE.1 */ 

                 if(drives[i]==NULL)  continue ;                    /* ��������� ������� ���������� */

                L =finish[i]-start[i] ;                             /* ���������� ��������� */
                Vo=v1[i] ;
                Vk=Vend[i] ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
         if(Vo==0. && Vk==0.) {
                                           forces[i].a1= 0. ;
                                           forces[i].a2= 0. ;
                                           forces[i].a3= 0. ;
                                           forces[i].t1= 0. ;
                                           forces[i].t2= 0. ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
         else 
         if(Vo==0.) {

               sgn   =Vk>0.?1.:-1. ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vk_max=sqrt(2.*A_MAX(i)*L) ;                         /* ���.���������� ���������� �������� */
            if(Vk_max<Vk) {                                         /* ���� �������� �������� ������� ������ - */
                                 Vk_correction=0.99*Vk_max/Vk ;     /*   - ��������� �� �� ����������          */ 
                                                                    /*       � ���� �� ��������� ������        */
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
/*- - - - - - - - - - - - - - - - - - - -  ����������������� ������� */
         else 
         if(Vk==0.) {

               sgn   =Vo>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               L     =fabs(L) ;
               Vo_max=sqrt(2.*A_MAX(i)*L) ;                         /* ���.���������� ��������������� �������� */
            if(Vo_max<Vo) {                                         /* ���� ��������� �������� ������� ������ - */
                                 Vo_correction=fabs(Vo_max/Vo) ;    /*   - ��������� �� �� ����������           */ 
                                                                    
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
/*- - - - - - - - - - - - - - - - - - ������� � ����������� �������� */
         else 
         if(Vk==Vo) {
                                 Tmin[i]=L/Vo ;

                            forces[i].a1= 0. ;
                            forces[i].a2= 0. ;
                            forces[i].a3= 0. ;
                            forces[i].t1= 0. ;
                            forces[i].t2= 0. ;
                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - ������� � �������� */
         else 
         if(Vk*Vo>0. && fabs(Vk)>fabs(Vo) && L*Vo>0.) {

               sgn   =Vk>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vk_max=sqrt(2.*A_MAX(i)*L+Vo*Vo) ;                   /* ���.���������� ���������� �������� */
            if(Vk_max<Vk) {                                         /* ���� �������� �������� ������� ������ - */
                                 Vk_correction=0.99*Vk_max/Vk ;     /*   - ��������� �� �� ����������          */ 
                                                                    /*       � ���� �� ��������� ������        */
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
/*- - - - - - - - - - - - - - - - - - - - - -  ������� � ����������� */
         else 
         if(Vk*Vo>0. && fabs(Vk)<fabs(Vo) && L*Vo>0.) {

               sgn   =Vo>0.?1.:-1. ;
               Vo    =fabs(Vo) ;
               Vk    =fabs(Vk) ;
               L     =fabs(L) ;
               Vo_max=sqrt(2.*A_MAX(i)*L+Vk*Vk) ;                   /* ���.���������� ��������� �������� */
            if(Vo_max<Vo) {                                         /* ���� ��������� �������� ������� ������ - */
                                 Vo_correction=fabs(Vo_max/Vo) ;    /*   - ��������� �� �� ����������           */ 
                                                                    
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
/*- - - - - - - - - - - - - - - - - - - - - - - - ������� � �������� */
         else 
         if(Vk*Vo<0.) {

                                sgn=Vo>0.?1.:-1. ;
                                 L =-L*Vo/fabs(Vo) ;                /* ������������ ���� ��������� */
                                 Lt=Vo*Vo/(2.*A_MAX(i)) ;           /* ���.��������� ���������� */
                                 Lr=Vk*Vk/(2.*A_MAX(i)) ;           /* ���.��������� ������� */
                                 Vo=fabs(Vo) ;
                                 Vk=fabs(Vk) ;
 
                if(Lr-Lt<=L) {                                      /* ���� ���� ��������� ����� ������� */
                                  Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)   /*   �� ������������ ��������        */
                                         +(L-(Lr-Lt))/Vk ;
                                        t=(Vk+Vo)/A_MAX(i) ;

                                 forces[i].a1=-sgn*A_MAX(i) ;
                                 forces[i].a2=-sgn*A_MAX(i) ;
                                 forces[i].a3= 0. ;
                                 forces[i].t1= 0. ;
                                 forces[i].t2= t ;
                             }
                else         {                                      /* ���� ��� ��������� ��� �������    */ 
                                  Tr=Vk/A_MAX(i) ;                  /*   �� ������������ ��������        */
                                  Lt=fabs(Lr-L) ;                   /*  - ����������� �� �� ����         */
                                   A=A_MAX(i) ;                     /*   �������� ���������� �������     */
                                   B=2.*Vo ;                        /*    ����������� �����, ����������� */
                                   C=Vo*Vo/(2.*A_MAX(i))-Lt ;       /*     � ����������� ��������        */

                                   t=(-B+sqrt(B*B-4.*A*C))/(2.*A) ; /* ����� ����������� ������� */

                    if((Vo+A_MAX(i)*t)<=V_MAX(i)) {                 /* ���� �� �� ��������� ���������� ��������... */

                           Tmin[i]=Vk/A_MAX(i)+t+(t+Vo/A_MAX(i)) ;
                                 forces[i].a1= sgn*A_MAX(i) ;
                                 forces[i].a2= sgn*A_MAX(i) ;
                                 forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  0. ;
                                 forces[i].t2=  t ;
                                                  }
                    else                          {                 /* ���� ���������� �������� �����������... */

                                  t  =(V_MAX(i)-Vo)/A_MAX(i) ;      /* ����� ������� �� ���������� �������� */
                                  t_2= V_MAX(i)/A_MAX(i) ;          /* ����� �������� �� ���������� �������� */
                                  t_3=(Lt-Vo*t-0.5*A_MAX(i)*t*t     /* ����� �������� � ������������ ��������� */
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
/*- - - - - - - - - - - - - - - - - - - - - - - ������� �� "�������" */
         else 
         if(Vk*Vo>0. && L*Vo<0.) {

                 sgn   =Vo>0.?1.:-1. ;
                 L     =fabs(L) ;
                 Lt    =Vo*Vo/(2.*A_MAX(i)) ;                       /* ���.��������� ���������� */
                 Lr    =Vk*Vk/(2.*A_MAX(i)) ;                       /* ���.��������� ������� */
                 Vo    =fabs(Vo) ;
                 Vk    =fabs(Vk) ;
                 Vk_max=sqrt(A_MAX(i)*(L+Lt+Lr)) ;                  /* ���.���������� ���������� �������� */

              if(Vk_max>V_MAX(i)) {                                 /* �������� � "��������" */
                                    t=V_MAX(i)/A_MAX(i) ;           /* ����� ������� �� ���������� �������� */
                              Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)+2*t
                                     +(L+Lt+Lr-A_MAX(i)*t*t)/V_MAX(i) ;
                                  }
              else                {                                 /* �������� ��� "�������" */
                                   t =Vk_max/A_MAX(i) ; 
                              Tmin[i]=Vo/A_MAX(i)+Vk/A_MAX(i)+2*t ;
                                  }

                           forces[i].a1=-sgn*A_MAX(i) ;
                           forces[i].a2=  0 ;
                           forces[i].a3= sgn*A_MAX(i) ;
                           forces[i].t1= Vo/A_MAX(i)+t ;
                           forces[i].t2= Tmin[i]-Vk/A_MAX(i)-t ;

                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ���������� ������ */
         else              {
 
             SEND_ERROR("SPLINE.iTurnSegment@"
                        "���������������� ������ ����������� ���������� ��������") ;
                                     return(-1) ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                      }                             /* CONTINUE.1 */

             if(Vk_correction!=0.)  continue ;                      /* ���� ��������� �������� �������� */
                                                                    /*   - ���� �� ��������� ������     */
             if(Vo_correction!=0.) {                                /* ���� ��������� ��������� ��������      */
                                      return(_V_CORRECTION) ;       /*   - ������ ��������� �������� �������� */
                                   }                                /*       ����������� ������               */
/*------------------------------ ����� �������� ���������� ��������� */

       for(i=0 ; i<mDrives_cnt ; i++) {

                 if(drives[i]==NULL)  continue ;                    /* ��������� ������� ���������� */

          if(i==0 || T<Tmin[i])  T=Tmin[i] ;
                                      }
/*------------------------------------- ����������� ������� �������� */

                    Vk_correction=0. ;

       for(i=0 ; i<mDrives_cnt ; i++) {                             /* CIRCLE.2 */ 

                 if(drives[i]==NULL)  continue ;                    /* ��������� ������� ���������� */

                 if(T==Tmin[i])  continue ;                         /* ���� ����� ������� �� ������������... */

                L =finish[i]-start[i] ;                             /* ���������� ��������� */
                Vo=v1[i] ;
                Vk=Vend[i] ;
          
/*- - - - - - - - - - - - - - - - - - - - - - -  ����������� ������� */
         if(Vo==0. && Vk==0.) {
                                      
                      memset(&forces[i], 0, sizeof(*forces)) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - ���������� ������� */
         else 
         if(Vo==0.) {
                                     L =fabs(L) ;
                                   sgn =Vk>0.?1.:-1. ;

                                     a =0.5*Vk*Vk/L ;               /* ������������ ��������������� �������� */ 
                                     t =fabs(Vk/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2= 0. ;
                           forces[i].a3= a*sgn ;
                           forces[i].t1= 0. ;
                           forces[i].t2=T-t ;

                   if(T>t)  continue ;

                                     a =A_MAX(i) ;                  /* ������������ ��������������� � �������� */
                                     t =fabs(Vk/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2= a*sgn ;
                           forces[i].a3= 0. ;
                           forces[i].t2=T-(L-0.5*Vk*Vk/a)/fabs(Vk) ;
                           forces[i].t1=forces[i].t2-t ;                        
                    }
/*- - - - - - - - - - - - - - - - - - - -  ����������������� ������� */
         else 
         if(Vk==0.) {
                                     L =fabs(L) ;
                                   sgn =Vo>0.?1.:-1. ;

                                     a =0.5*Vo*Vo/L ;               /* ������������ ��������������� �������� */ 
                                     t =fabs(Vo/a) ;
                           forces[i].a1=-a*sgn ;
                           forces[i].a2= 0. ;
                           forces[i].a3= 0. ;
                           forces[i].t1= t ;
                           forces[i].t2= t ;

                   if(T>t)  continue ;

                                     a =A_MAX(i) ;                  /* ������������ ��������������� � �������� */
                                     t =fabs(Vo/a) ;
                           forces[i].a1= 0. ;
                           forces[i].a2=-a*sgn ;
                           forces[i].a3= 0. ;
                           forces[i].t1=(L-0.5*Vo*Vo/a)/fabs(Vo) ;
                           forces[i].t2=forces[i].t1+t ;
                    } 
/*- - - - - - - -  ������� � �������������� ��������� � ��� "������" */
         else 
         if(Vk*Vo>0. && L*Vo>0.) {
                                     L =fabs(L) ;
                                   sgn =Vk>0.?1.:-1. ;
                                     Vo=fabs(Vo) ;
                                     Vk=fabs(Vk) ;

                                      t=2.*L/(Vo+Vk) ;             /* ���. ����� ������������ ��������� */

                if(Vo!=Vk &&                                       /* ���� �������� � ���������� �� ����� �������... */
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

                                      a=(Vo*Vo+Vk*Vk)/(2.*L) ;     /* ���.��������� ��� �����-����� */
                                      t=(Vo+Vk)/a ;                /* ���. ����� ������� �����-����� */

                if(T<t) {                                          /* ���� �� �����-����� �� �� �����... */
                            A=T*T ;
                            B=2.*(2*L-T*(Vo+Vk)) ;
                            C=-(Vk-Vo)*(Vk-Vo) ;
                            a=(-B+sqrt(B*B-4.*A*C))/(2.*A) ;

                   if(a>A_MAX(i)) {                                /* ���� ��������� ������� ������  */
                                        Vk_correction=0.95 ;       /*  - ��������� �������� �������� */
                                                                   /*   � ���� �� ��������� ������   */  
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

                 if(a>A_MAX(i)) {                                  /* ���� ��������� �����-����� ������� ������  */
                                        Vk_correction=0.95 ;       /*  - ��������� �������� ��������             */
                                                                   /*   � ���� �� ��������� ������               */  
                   for(j=0 ; j<mDrives_cnt ; j++)
                     if(drives[j]!=NULL) Vend[j]*=Vk_correction ;
                                    break ;
                                }

                              forces[i].a1=-a*sgn ;                /* �����-���� */
                              forces[i].a2= 0. ;
                              forces[i].a3= a*sgn ;
                              forces[i].t1= Vo/a ;
                              forces[i].t2= T-Vk/a ;
                               }
/*- - - - - - - - - - - - - - - - - - - - - - - ������� �� "�������" */
         else 
         if(Vk*Vo>0. && L*Vo<0.) {

                    sgn=Vo>0.?1.:-1. ;
                    L  =fabs(L) ;
                    Lt =Vo*Vo/(2.*A_MAX(i)) ;                       /* ���.��������� ���������� */
                    Lr =Vk*Vk/(2.*A_MAX(i)) ;                       /* ���.��������� ������� */
                    Vo =fabs(Vo) ;
                    Vk =fabs(Vk) ;

                    Tt =Vo/A_MAX(i) ;
                    Tr =Vk/A_MAX(i) ;
                    t  =T-Tr-Tt ;
                    t_2=0.5*t-sqrt(0.25*t*t-(L+Lt+Lr)/A_MAX(i)) ;   /* ����� �������/���������� �� ����� */

                           forces[i].a1=-sgn*A_MAX(i) ;
                           forces[i].a2=  0 ;
                           forces[i].a3= sgn*A_MAX(i) ;
                           forces[i].t1= Tt+t_2 ;
                           forces[i].t2= T-Tr-t_2 ;

                                 }
/*- - - - - - - - - - - - -  ������� � ������������������� ��������� */
         else              {
                                sgn=Vo>0.?1.:-1. ;
                                 L =-L*Vo/fabs(Vo) ;                /* ������������ ���� ��������� */
                                 Vo=fabs(Vo) ;
                                 Vk=fabs(Vk) ;

              if(forces[i].a3!=0) {                                 /* ���� ������ ������� Lt+L<=0. */
                                      Lr=Vk*Vk/(2.*A_MAX(i)) ;      /* ���.��������� ������� */
                                      Lt=fabs(Lr-L) ;               /* ���.��������� ���������� */
                                      Tr=Vk/A_MAX(i) ;
                                      Tt=T-Tr ; 

                  t=2.*Lt/Vo ;                                      /* ���.����� ��������.���������� */

               if(t<=Tt) {                                          /* ���� ������ � ���������� � ������ ����� ��������� */
                                 forces[i].a1=-sgn*Vo/t ;
                                 forces[i].a2=  0 ;
//                               forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  Tt ;
                                        continue ;
                        } 

                  t=Vo/A_MAX(i)+(Lt-Vo*Vo/(2.*A_MAX(i)))/Vo ;       /* ���.����� ���������� � "�����" �� �������� */
               if(t<=Tt) {                                          /* ���� �� � �������� ����� ������... */
                                           t =2.*Lt/Vo-Tt ;
                                 forces[i].a1=  0 ;
                                 forces[i].a2=-sgn*Vo/(Tt-t) ;
//                               forces[i].a3=-sgn*A_MAX(i) ;
                                 forces[i].t1=  t ;
                                 forces[i].t2=  Tt ;
                                        continue ;
                         }            
                                                                    /* ���.����� � �������� �� ����� � ����������� �� ��������� */

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
                                                  "������ ������� ������ �������� "
                                                  "���������� (%d:%d)", mAttempt, i) ;
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

                             Lt=Vo*Vo/(2.*A_MAX(i)) ;               /* ���.��������� ���������� */
                             Lr=L+Lt ;                              /* ���.��������� ������� */
                              t=Vo/A_MAX(i)+2.*fabs(Lr)/Vk ;        /* ���������� ����� ��� �����-���������� ������� */

                 if(t<=T) {                                         /* ���� ��������������� ������ � ���������� */
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

//                               forces[i].a1=-sgn*A_MAX(i) ;      /* ���� ������ � �������� */
                                 forces[i].a2=-sgn*a ;
                                 forces[i].a3=  0. ;
                                 forces[i].t1= Vo/A_MAX(i)  ;
                                 forces[i].t2= forces[i].t1+t ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/             
                                      }                             /* CONTINUE.1 */

             if(Vk_correction!=0.)  continue ;                      /* ���� ��������� �������� �������� */
                                                                    /*   - ���� �� ��������� ������     */
/*------------------------------------------ ������� ���������� ���� */
                      
                      break ;

        } while(1) ;                                                /* CONTINUE.0 */

/*--------------------------------------------------------- �������� */

/*----------------------------------------- ������ ���������� ������ */

   for(i=0 ; i<mDrives_cnt ; i++)                                   /* ����������� ���������� �������� */
     if(drives[i]!=NULL)  v2[i]=Vend[i] ;

                    *time_all=T ;

/*-------------------------------------------------------------------*/

  return(0) ;

#undef  V_MAX
#undef  A_MAX

}


/********************************************************************/
/*                                                                  */
/*               �������� �� ������� ����������                     */

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
          double  v_acc[50] ;         /* ������ �������� �� �������� */
          double  p_acc[50] ;         /* ������ �������� �� ��������� */
          double  v1 ;
          double  v2 ;
          double  a ;
  static  double *pipe ;              /* ������ ����� ����� ������ ������ */
             int  pipe_cnt ;
       RSS_Point  Top ;               /* ������� ��������� ������� ����� ������� */
       RSS_Point  Top_pipe ;          /* ��������� ������� ����� ������� �� ����� ������ ������ */
             int  status ;
            char  tmp[1024] ;
             int  i ;
             int  j ;

#define  V_MAX(i)               drives[i]->v_max
#define  A_MAX(i)  (a_available*drives[i]->a_max)

/*---------------------------------------------------- ������������� */

           memset(v_acc, 0, sizeof(v_acc)) ;
           memset(p_acc, 0, sizeof(p_acc)) ;

           memcpy(v, v0,    mDrives_cnt*sizeof(v[0])) ;
           memcpy(p, start, mDrives_cnt*sizeof(p[0])) ;

    if(pipe==NULL)  pipe=(double *)
                           gMemCalloc(64, 50*sizeof(*pipe), 
                                       "RSS_Module_Spline::iTraceSegment.pipe", 0, 0) ;

/*---------------------------------------- �������� �������� ������� */

   for(i=0 ; i<mDrives_cnt ; i++) {                                 
                                                                    
       if(drives[i]==NULL)  continue ;                    

       if(forces[i].t1<    0.      ||
          forces[i].t2<forces[i].t1  ) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "������������ ��������� ������� ��� "
                                "������� ���������� ��� ����������� (%d:%d:%d)", mAttempt, n_segm, i) ;
                SEND_ERROR(tmp) ;
                     return(_FATAL_ERROR) ;
                                       }  

                                  }

/*---------------------------------------- ��������� ��������� ����� */

      for(i=0 ; i<mDrives_cnt ; i++)              
         if(drives[i]!=NULL)  mDrives[i].value=p[i] ;

              object->vSetJoints     (mDrives, mDrives_cnt) ;
              object->vSolveByJoints () ;                           /* ������ ������ ������ */
              object->vGetTargetPoint(&Top) ;                       /* �������� ��������� ������� ����� */  

                       iSaveErrorPipe(n_segm, &Top, &Top) ;         /* ��������� ����� ���������� */

/*------------------------------------------------ ���� ������������ */

   for(t=0. ; t<time_all ; t+=dt) {                                 /* CIRCLE.0 */

           memcpy(p_prv, p,  mDrives_cnt*sizeof(p[0])) ;            /* ������� ���������� ����� */

/*------------------------------ ������ ������������ ���� �� ������� */         

                          dt=time_all/10. ;                         /* ������������� ��������� ��� */
        do {
/*- - - - - - - - - - - - - - - - - -  ������� �� ��������� �������� */
              for(i=0 ; i<mDrives_cnt ; i++) {                      /* ���� ���������� ����� ������������ - */
                                                                    /*   �������� ����� �� ���              */
                 if(drives[i]==NULL)  continue ;                    

                 if( t    < forces[i].t1 &&
                    (t+dt)>=forces[i].t1   )  dt=(forces[i].t1-t)*1.00001 ;
            else if( t    < forces[i].t2 &&
                    (t+dt)>=forces[i].t2   )  dt=(forces[i].t2-t)*1.00001 ;
                                             }
 
                if((t+dt)>time_all)  dt=(time_all-t)*1.00001 ;      /* ���� ���������� ������� ������� */
/*- - - - - - - - - - - - - -  ������� �������� �������� ����������� */
              for(i=0 ; i<mDrives_cnt ; i++) {                      /* ������� ������ ��������� */

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
/*- - - - - - - - - - - - - - - - - - �������� ��������� � ��������� */
/*- - - - - - - - - - - - - - - - - - - -  ������� ��������� ������� */
                    object->vSetJoints    (mDrives, mDrives_cnt) ;
             status=object->vSolveByJoints() ;                      /* ������ ������ ������ */
          if(status) {                                              /* ���� ����� �����������... */
                            iDebug("��������� ���������� ����������� �� ����������", NULL) ;
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "��������� ���������� ����������� �� ����������") ;
                             return(_PATH_FAIL) ;
                     }

                    object->vGetTargetPoint(&Top) ;                 /* �������� ��������� ������� ����� */  
                               break ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
           } while(1) ;

/*--------------------------------- ��������� "����������" ��������� */

                     iSaveErrorPipe(n_segm, &Top, &Top) ;           /* ��������� ����� ���������� */

       if(mAdjustErrorPipe) {
             status=object->vCheckFeatures(NULL) ;                  /* ��������� ������������������ ������� */
          if(status) {                                              /* ���� ��������� ������������������ �������... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "�������� ����������") ;
                             return(_PATH_FAIL) ;
                     } 
                            }
/*--------------------------------- ������������ ����� ������ ������ */

   if(mAdjustErrorPipe ||
        mShowErrorPipe   ) {

           memcpy(pipe, p,  mDrives_cnt*sizeof(p[0])) ;             /* ��������� ����������� ����� */
                  pipe_cnt=1 ;                   

     for(i=0 ; i<mDrives_cnt ; i++) {                               /* ��������� ������ */

            if(drives[i]==NULL)  continue ;

                  iErrorCalc(drives[i], NULL) ;                     /* ������� ������ */

            if(drives[i]->d_err==0.)  continue ;

           memmove(&pipe[mDrives_cnt*pipe_cnt], pipe,               /* ����������� ����� ����� */
                            mDrives_cnt*pipe_cnt*sizeof(*pipe)) ;

       for(j=0 ; j<pipe_cnt ; j++) {                                /* ������ ����/����� ������ */
         pipe[mDrives_cnt* j          +i]=p[i]-drives[i]->d_err ;
         pipe[mDrives_cnt*(j+pipe_cnt)+i]=p[i]+drives[i]->d_err ;
                                   }
                                         pipe_cnt*=2 ;
                                    }

                           }
/*--------------------------------- ������ ����� ����� ������ ������ */

   if(mAdjustErrorPipe ||
        mShowErrorPipe   ) {

     for(i=0 ; i<pipe_cnt ; i++) {
/*- - - - - - - - - - - - - - - - - - - - ������� ��������� �������  */
       for(j=0 ; j<mDrives_cnt ; j++)
                     mDrives[j].value=pipe[i*mDrives_cnt+j] ;

                    object->vSetJoints    (mDrives, mDrives_cnt) ;
             status=object->vSolveByJoints() ;                      /* ������ ������ ������ */
          if(status && mAdjustErrorPipe) {                          /* ���� ����� �����������... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "��������� ���������� ����������� ��� ������ ������") ;
                                            return(_PIPE_FAIL) ;
                                         } 
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ��������� ����� */
                  object->vGetTargetPoint(&Top_pipe) ;              /* �������� ��������� ������� ����� */

       if(mShowErrorPipe)  iSaveErrorPipe(n_segm, &Top, &Top_pipe); /* ��������� ����� ������ ������ */

       if(mAdjustErrorPipe) {
            status=object->vCheckFeatures(NULL) ;                   /* ��������� ������������������ ������� */
         if(status) {                                               /* ���� ��������� ������������������ �������... */
//                      SEND_ERROR("SPLINE.iTraceSegment@"
//                                 "�������� ������ ������") ;
                             return(_PIPE_FAIL) ;
                    } 
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                 }

                           }
/*------------------------------------------------ ���� ������������ */
                                  }                                 /* CONTINUE.0 */
/*---------------------------- �������� ���������� � �������� ������ */

   for(i=0 ; i<mDrives_cnt ; i++) {
                           p_acc[i]*=0.001 ;
                           v_acc[i]*=0.001 ;
                                  }

   for(i=0 ; i<mDrives_cnt ; i++) {
                                                                    
                 if(drives[i]==NULL)  continue ;                    

       if(fabs(p[i]-start[i+mDrives_cnt])>p_acc[i]) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "����������� � ���������� ����� "
                                "�������� ���������� ��� ����������� (%d:%d:%d)", mAttempt, n_segm, i) ;
                SEND_ERROR(tmp) ;
                     return(_FATAL_ERROR) ;
                                                    }  

       if(fabs( v[i]-v0[i+mDrives_cnt])>
          fabs(v0[i]-v0[i+mDrives_cnt])/500.) {
                   sprintf(tmp, "SPLINE.iTraceSegment@"
                                "����������� � ���������� �������� "
                                "�������� ���������� ��� ����������� (%d:%d:%d)", mAttempt, n_segm, i) ;
//              SEND_ERROR(tmp) ;
//                   return(_FATAL_ERROR) ;
                                               }
                                  }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*								     */
/*                   ���������� ����� ������ ������                  */

  int  RSS_Module_Spline::iSaveErrorPipe(      int  n_segm,
                                         RSS_Point *p0, 
                                         RSS_Point *p1 )
{
/*----------------------------------------------- ����������� ������ */

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
/*------------------------------------------------- ���������� ����� */
                  
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
/*              ��������� ������ ������ �� ������� ��������          */

  int  RSS_Module_Spline::iCutErrorPipe(int  n_segm)

{
   for( ; mErrorPipe_cnt>0 ; mErrorPipe_cnt--)
     if(mErrorPipe[mErrorPipe_cnt-1].mark<n_segm)  break ;
    
   return(0) ;
}


/********************************************************************/
/*								    */
/*          ����������� ������ ������ � ��������� ���������         */

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
/*                     ����������� ������ ������                     */

  void  RSS_Module_Spline::iErrorPipeShow(TRAJECTORY *Trajectory)

{
                RSS_Context *context ;
  RSS_Module_Spline_Context *data ;
                        int  status ;
                        int  i ;

#define    T  Trajectory
#define    D  data

/*--------------------------------------------- ���������� ��������� */

          context=vAddData(&Trajectory->contexts) ;
       if(context      ==NULL ||
          context->data==NULL   ) {                                 /* ���� �� ������� ����������... */
                             SEND_ERROR("SPLINE.iErrorPipeShow@"
                                        "������ ������ ���� ���������� �� ��������������") ;
                                             return ;
                                  }

          data=(RSS_Module_Spline_Context *)context->data ;

/*---------------------------------------- ����������� ������ � ���� */

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

/*-------------------------------- �������������� ����������� ������ */

     if(D->dlist1_idx==0) {

           D->dlist1_idx=RSS_Kernel::display.GetList(2) ;
                          }

     if(D->dlist1_idx==0)  return ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*---------------------------------- ��������� ��������� ����������� */

             glNewList(D->dlist1_idx, GL_COMPILE) ;                 /* �������� ������ */
          glMatrixMode(GL_MODELVIEW) ;

/*--------------------------------------------- ��������� ���������� */

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
/*----------------------------- �������������� ��������� ����������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef    T
}


/********************************************************************/
/*								    */
/*            ������ � �������� ������ �������                      */

   int  RSS_Module_Spline::iErrorCalc(RSS_Module_Spline_Context *drive,
                                                           char *formula)
{
    static  RSS_Kernel *Calc ;
  struct RSS_Parameter  pars[10] ;
                   int  status ;
                  char  text[1024] ;
                  char  error[1024] ;
                   int  i ;

/*------------------------------------------ ����������� ����������� */

   if(Calc==NULL) {

#define  CALC_CNT   RSS_Kernel::calculate_modules_cnt
#define  CALC       RSS_Kernel::calculate_modules

         for(i=0 ; i<CALC_CNT ; i++) {

             status=CALC[i]->vCalculate("STD_EXPRESSION", NULL, NULL, NULL, 
                                                          NULL, NULL, NULL ) ;
         if(status==0)  break ;
                                     }

         if(i>=CALC_CNT) {
               SEND_ERROR("SPLINE@�� �������� ����������� ����������� ���������") ;
                             return(-1) ;
                         }

                                Calc=CALC[i] ;

#undef   CALC_CNT
#undef   CALC
                  }
/*----------------------------------------------- ���������� ������� */

   if(formula!=NULL) {
/*- - - - - - - - - - - - - - - - - - - - - - - - ������� ���������� */
                        memset(pars, 0, sizeof(pars)) ;

                        strcpy(pars[0].name, "V") ;
                               pars[0].ptr=&drive->v ;
                        strcpy(pars[1].name, "A") ;
                               pars[1].ptr=&drive->a ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - �������� ������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - ���������� */
                          drive->calculate=NULL ;  // ???
 
          if(*formula==0)  return(0) ;
              
             status=Calc->vCalculate("STD_EXPRESSION",              /* ����������� ������� */
                                      formula, pars, NULL,
                                     &drive->d_err,
                                     &drive->calculate, error) ;
          if(status==-1) {                                          /* ���� ������... */
                              sprintf(text, "������ �����������\n\n%s", error) ;
                           SEND_ERROR(text) ;
                                return(-1) ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     }
/*----------------------------------------------- ���������� ������� */

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
/*                   ���������� ������ � �����                      */

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
