/********************************************************************/
/*								    */
/*      ������ ������ "����������� ���������� �� ��������������"    */
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
#include "T_Fast.h"

#pragma warning(disable : 4996)


#define  _BEST_PATH_NAME     "Fast_best_traj"
#define  _WORK_PATH_NAME     "Fast_work_traj"

#define  _SECTION_FULL_NAME   "FAST"
#define  _SECTION_SHRT_NAME   "FAST"


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

     static   RSS_Module_Fast  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 T_FAST_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 T_FAST_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**                   �������� ������ ������                       **/
/**          "����������� ���������� �� ��������������"            **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Fast_instr  
         RSS_Module_Fast_InstrList[]={

 { "help",      "?",  "#HELP (?) - ������ ��������� ������", 
                       NULL,
                      &RSS_Module_Fast::cHelp   },
 { "config",    "c",  "#CONFIG (C) - ������� ���������� �����������", 
                       NULL,
                      &RSS_Module_Fast::cConfig   },
 { "optimize",  "o",  "#OPTIMIZE (O) -  ����������� ����������",
                      " OPTIMIZE[/BD] <��� ����������>\n"
                      "   B - ���������� ������ ������� ������ ����������\n"
                      "   D - ���������� ������ ������� ����������\n",
                      &RSS_Module_Fast::cOptimize   },
 {  NULL }
                                       } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Fast_instr *RSS_Module_Fast::mInstrList=NULL ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_Fast::RSS_Module_Fast(void)

{
                keyword="Geo" ;
         identification="Fast_task" ;

             mInstrList=RSS_Module_Fast_InstrList ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_Fast::~RSS_Module_Fast(void)

{
}


/********************************************************************/
/*								    */
/*		      ������������� ������      		    */

  void  RSS_Module_Fast::vInit(void)

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
           MessageBox(NULL, "������ FAST ������� ������� ������ PATH", "Geo", 0) ;
                                return ;
                           }
#undef   MODULES
#undef   MODULES_CNT

/*------------------------ ���������� ��������� ������ ������-������ */

#define   REDIR       mModulePath->command_redirect 
#define   REDIR_CNT   mModulePath->command_redirect_cnt 

//  if(REDIR==NULL)
//          REDIR=(RSS_Redirect *) calloc(  1,   sizeof(*REDIR)*(REDIR_CNT+1)) ;
//  else    REDIR=(RSS_Redirect *)realloc(REDIR, sizeof(*REDIR)*(REDIR_CNT+1)) ;
//
//           memset(&REDIR[REDIR_CNT], 0, sizeof(*REDIR)*1) ;
//
//                   REDIR[REDIR_CNT].master =_SECTION_FULL_NAME ;
//                   REDIR[REDIR_CNT].command="glide" ;
//                   REDIR[REDIR_CNT].module = this ;
//                         REDIR_CNT++ ;
//
#undef    REDIR
#undef    REDIR_CNT

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Fast::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Fast::vExecuteCmd(const char *cmd, RSS_IFace *iface)

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Fast:") ;
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
       if(status)  SEND_ERROR("������ FAST: ����������� �������") ;
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

    void  RSS_Module_Fast::vShow(char *layer)

{
}


/*********************************************************************/
/*								     */
/*             ��������� �������� � ��������� ������                 */

    void  RSS_Module_Fast::vChangeContext(void)

{
/*------------------------------------ ����������� ������ ���������� */

   if(!stricmp(mContextAction, "SHOW")) {

//                    iErrorPipeShow(mContextObject) ;
                                        }
/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Fast::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
                       char *work ;
                       char *value ;
                       char *end ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE FAST\n", 
                      strlen("#BEGIN MODULE FAST\n")) )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Fast::vReadSave", 0, 0) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- ��������� ������ */

                work=buff+strlen("#BEGIN MODULE FAST\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */

          end=strchr(work, '\n') ;
       if(end==NULL)  break ;
         *end=0 ;

          value=strchr(work, '=') ;
       if(value==NULL)  continue ;
         *value=0 ;
          value++ ;

       if(!stricmp(work, "TIME_CALC"   ))   strncpy(mTimeCalc_cmd, value, sizeof(mTimeCalc_cmd)-1) ;
       if(!stricmp(work, "LINE_CHECK"  ))   strncpy(mLineCheck_cmd, value, sizeof(mLineCheck_cmd)-1) ;
       if(!stricmp(work, "LINE_DRAW"   ))   strncpy(mLineDraw_cmd, value, sizeof(mLineDraw_cmd)-1) ;
       if(!stricmp(work, "SIMPL_OPT"   ))  mSimplification_opt    =strtol(value, &end, 10) ;
       if(!stricmp(work, "SIMPL_METHOD"))  mSimplification_method =strtol(value, &end, 10) ;
       if(!stricmp(work, "SMOOTH_OPT"  ))       mSmoothing_opt    =strtol(value, &end, 10) ;
       if(!stricmp(work, "CUT_OPT"     ))      mCutSegment_opt    =strtol(value, &end, 10) ;
       if(!stricmp(work, "CUT_TIME"    ))      mCutSegment_MinTime=strtod(value, &end) ;
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- ������������ �������� */

                        gMemFree(buff) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Fast::vWriteSave(std::string *text)

{
  char  value[1024] ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE FAST\n" ;

    sprintf(value, "TIME_CALC=%s\n", mTimeCalc_cmd) ;
     *text+=value ;
    sprintf(value, "LINE_CHECK=%s\n", mLineCheck_cmd) ;
     *text+=value ;
    sprintf(value, "LINE_DRAW=%s\n", mLineDraw_cmd) ;
     *text+=value ;
    sprintf(value, "SIMPL_OPT=%d\n", mSimplification_opt) ;
     *text+=value ;
    sprintf(value, "SIMPL_METHOD=%d\n", mSimplification_method) ;
     *text+=value ;
    sprintf(value, "SMOOTH_OPT=%d\n", mSmoothing_opt) ;
     *text+=value ;
    sprintf(value, "CUT_OPT=%d\n", mCutSegment_opt) ;
     *text+=value ;
    sprintf(value, "CUT_TIME=%.10lf\n", mCutSegment_MinTime) ;
     *text+=value ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            ������� ������ ��������� ������ �� ������             */

   int  RSS_Module_Fast::vReadData(RSS_Context ***contexts,
                                   std::string   *text     )
{
                RSS_Context *context ;
//  RSS_Module_Fast_Context *data ;
//                     char *entry ;

#define  C   (*contexts)

/*------------------------------------------ ������������� ��������� */

   if(memicmp(text->c_str(), this->identification, 
                      strlen(this->identification)))  return(0) ;

/*------------------------------------- ���������� ������ ���������� */

          context=vAddData(contexts) ;
       if(context==NULL) {                                          /* ���� �� ������� ����������... */
            SEND_ERROR("������������ ������ ��� ������ ����������") ;
                               return(-1) ;
                         }
/*-------------------------------------------------------------------*/

#undef  C

   return(1) ;
}


/********************************************************************/
/*								    */
/*            �������� ������ ��������� ������ � ������             */

   void  RSS_Module_Fast::vWriteData(RSS_Context *context,
                                     std::string *text    )
{
#ifdef  _REMARK_

  RSS_Module_Fast_Context *data ;
                     char  field[1024] ;


        data=(RSS_Module_Fast_Context *)(context->data) ;

                                                     *text = "" ;
     sprintf(field, "%s ", this->identification ) ;  *text+=field ;
     sprintf(field, "V_MAX=%.10lf ", data->v_max) ;  *text+=field ;
     sprintf(field, "A_MAX=%.10lf ", data->a_max) ;  *text+=field ;
     sprintf(field, "F_ERR=%s",      data->f_err) ;  *text+=field ;

#endif
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Fast::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - ����������� �������-�������� ����������"
                                   " �� ������� ����� �� �����������", "FAST") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Fast::cHelp(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Fast_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CONFIG                  */

  int  RSS_Module_Fast::cConfig(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirectParam(GetModuleHandle(NULL),
			   (LPCDLGTEMPLATE)Resource("IDD_CONFIG", RT_DIALOG),
			     GetActiveWindow(), Task_Fast_Config_dialog, 
                              (LPARAM)this) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� OPTIMIZE                */

  int  RSS_Module_Fast::cOptimize(char *cmd, RSS_IFace *iface)

{ 
#define   _PARS_MAX   2

                  RSS_IFace   iface_ ;
                  RSS_IFace   result_iface ;
                  RSS_IFace   best_iface ;
                       char  *pars[_PARS_MAX] ;
                        int   quiet_flag ;       /* �� �������� �������� ��������� */
                        int   draw_best ;        /* ���������� ������ ���������� */
                        int   draw_curr ;        /* ���������� ������� ���������� */
                       char  *traj_name ;
            RSS_Object_Path  *path_object ;
            RSS_Object_Path  *best_object ;
            RSS_Object_Path  *work_object ;
                       char   line_check_cmd[1024] ;
                       char   time_calc_cmd[1024] ;
                       char   best_draw_cmd[1024] ;
                       char   curr_draw_cmd[1024] ;
                       char   result[1024] ;
                     double   best_time ;
                     double   path_time ;
                        int   status ;
                        int   new_best ;      /* ���� ��������� ����� ������ ���������� */
                        int   best_change ;   /* ������� ��������� � "������" � ����� ������� */
                       char  *end ;
                       char   text[1024] ;
                        int   method_use ;
                        int   n ;
                        int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------------- ������� �������� */

      if(iface==NULL)  iface=&iface_ ;                              /* ��� ���������� ������������� ���������� */
                                                                    /*     ���������� ��������� ��������       */ 
/*---------------------------------- �������� ����������� ���������� */

     if(mTimeCalc_cmd[0]==0) {
         SEND_ERROR("�� ������ ������� ������� ������� ����������� ����������.") ;
                                 return(0) ;
                             }

     if(mLineCheck_cmd[0]==0) {
         SEND_ERROR("�� ������ ������� �������� ������������ ����������.") ;
                                 return(0) ;
                              }

     if(mSimplification_opt==0 &&
        mSmoothing_opt     ==0 && 
        mCutSegment_opt    ==0   ) {
          SEND_ERROR("�� ����� �� ���� �� ������� �����������.") ;
                                      return(0) ;
                                   }
/*---------------------------------------- �������� ��������� ������ */
/*- - - - - - - - - - - - - - - - - - -  ��������� ������ ���������� */
                       quiet_flag=0 ;
                        draw_curr=0 ;
                        draw_best=0 ;

     if(*cmd=='/') {
 
                if(*cmd=='/')  cmd++ ;

                   end=strchr(cmd, ' ') ;
                if(end==NULL) {
                       SEND_ERROR("������������ ������ �������") ;
                                       return(-1) ;
                              }
                  *end=0 ;

                if(strchr(cmd, 'q')!=NULL ||
                   strchr(cmd, 'Q')!=NULL   )  quiet_flag=1 ;
                if(strchr(cmd, 'd')!=NULL ||
                   strchr(cmd, 'D')!=NULL   )   draw_curr=1 ;
                if(strchr(cmd, 'b')!=NULL ||
                   strchr(cmd, 'B')!=NULL   )   draw_best=1 ;

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
                              "��������: OPTIMIZE <���_����������> ...") ;
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
/*----------------- ���������� ������ ������� � ��������� ���������� */

              strcpy(line_check_cmd, this->mLineCheck_cmd) ;
         iText_subst(line_check_cmd, "$", _WORK_PATH_NAME) ;
              strcpy(time_calc_cmd,  this->mTimeCalc_cmd ) ;
         iText_subst(time_calc_cmd,  "$", _WORK_PATH_NAME) ;

              strcpy(best_draw_cmd,  this->mLineDraw_cmd ) ;
         iText_subst(best_draw_cmd,  "$", _BEST_PATH_NAME) ;
              strcpy(curr_draw_cmd,  this->mLineDraw_cmd ) ;
         iText_subst(curr_draw_cmd,  "$", _WORK_PATH_NAME) ;

/*--------------------------------------- ������� ������� ���������� */

       best_object=iCopyPath(path_object, _BEST_PATH_NAME) ;        /* ������� ����� �������� ���������� */
       work_object=iCopyPath(best_object, _WORK_PATH_NAME) ;        /* ������� ����� �������� ������� */

/*-------------------------------------- ������ ��������� ���������� */

                       path_time=-1. ;

     do {
              iExecute(line_check_cmd, &result_iface) ;             /* �������� ������������ ���������� */
          if(result_iface.vCheck("FATAL"))  break ;
          if(result_iface.vCheck("ERROR"))  break ;

              iExecute(time_calc_cmd, &result_iface) ;              /* ���.������� ����������� ���������� */
          if(result_iface.vCheck("FATAL"))  break ;
          if(result_iface.vCheck("ERROR"))  break ;

             result_iface.vDecode(NULL, result) ;
                 path_time=strtod(result, &end) ;

          if(draw_best)  iExecute(best_draw_cmd, NULL) ;            /* ��������� ������� ������ ���������� */

                 sprintf(text, "������ �����:%.3lf", path_time) ;

//        if(!quiet_flag)                                           /*  ���� �� ����� "������" - */
             SendMessage(this->kernel_wnd, WM_USER,                 /*   - ������ ������ �� ����� */
                        (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

        } while(0) ;

                best_iface.vPass(&result_iface) ;
                best_time=path_time ;

/*-------------------------------------- ������������� ������������� */

         iMethodSimplif(&best_object, NULL, NULL, NULL, &result_iface, &path_time) ;
         iMethodCut    (&best_object, NULL, NULL, NULL, &result_iface, &path_time) ;

/*--------------------------------------------- ��������������� ���� */

              method_use=0 ;
             best_change=0 ;
                       n=0 ;

   do {
                    n++ ;

          new_best=0 ;
/*- - - - - - - - - - - - - - - -  ��������� ��������������� ������� */
     if(method_use==0)
       if(this->mSimplification_opt)
          status=iMethodSimplif(&best_object,
                                &work_object, &n, &new_best, &result_iface, &path_time) ;
       else                           method_use++ ;

     if(method_use==1)
       if(this->mSmoothing_opt)
                                       status= 0 ;
//        status=iMethodSimplif(&best_object,
//                              &work_object, &n, &new_best, &path_time) ;
       else                           method_use++ ;
       
     if(method_use==2)
         if(this->mCutSegment_opt) 
              status=iMethodCut(&best_object, 
                                &work_object, &n, &new_best, &result_iface, &path_time) ;
         else                          status=-1 ;
/*- - - - - - - - - - - - - - - - - - - - ���������� ������ �������� */
     if(new_best) {
                      best_change=  1 ;
                      best_time  =path_time ;
                      best_iface.vPass(&result_iface) ;
                  }
/*- - - - - - - - - - - - - - - - -  ���������� �������� ����������� */
     if(status) {                                                   /* ���� ��������� ����� �������� �����������... */
                   if(method_use==0)  method_use++ ;                /* ���� ����� ������ ������� - ���� ����� */   
              else if(best_change  )  method_use=0 ;                /* ���� ��������� ��� ������� - ���� "����" */ 
              else                    method_use++ ;                /*   ����� � ������ "��������" ������ */

                   if(method_use >2)  break ;                       /* ���� ��� ������ ����������� �������� */

           result_iface.vPass(&best_iface) ;                        /* ��������������� "�������" ��������� */
                     path_time=best_time ;

     if(method_use==0)  iMethodSimplif(&best_object, NULL, NULL, NULL, &result_iface, &path_time) ;
//   if(method_use==1)  iMethodSimplif(&best_object, NULL, NULL, NULL, &result_iface, &path_time) ;
     if(method_use==2)  iMethodCut    (&best_object, NULL, NULL, NULL, &result_iface, &path_time) ;

                          best_change=0 ;
                              continue ;
                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - -  ��������� */
           if(draw_curr)  iExecute(curr_draw_cmd, NULL) ;           /* ��������� ������� ���������� */

       if(new_best) {                                               /* ���� ���������� ������� ����������... */
           if(draw_best)  iExecute(best_draw_cmd, NULL) ;           /*  ��������� ������� ������ ���������� */

                 sprintf(text, "������ �����:%.3lf", path_time) ;

//         if(!quiet_flag)                                          /*  ���� �� ����� "������" - */
             SendMessage(this->kernel_wnd, WM_USER,                 /*   - ������ ������ �� ����� */
                        (WPARAM)_USER_SHOW_INFO, (LPARAM)text) ;

                    }
/*- - - - - - - - - - - - - - - - - - - -  ������ ������� ���������� */
                       path_time=-1. ;

     do {

         if(stricmp(line_check_cmd, time_calc_cmd)) {
              iExecute(line_check_cmd, &result_iface) ;             /* �������� ������������ ���������� */
          if(result_iface.vCheck("FATAL"))  break ;
          if(result_iface.vCheck("ERROR"))  break ;
                                                    }

              iExecute(time_calc_cmd, &result_iface) ;              /* ���.������� ����������� ���������� */
          if(result_iface.vCheck("FATAL"))  break ;
          if(result_iface.vCheck("ERROR"))  break ;

             result_iface.vDecode(NULL, result) ;
                 path_time=strtod(result, &end) ;

        } while(0) ;

               iDebug(result, NULL) ;

       if(result_iface.vCheck("FATAL"))  break ;                    /* ��� ��������� ������� ���������� ����������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      } while(1) ;

           if(!quiet_flag)                                          /*  ���� �� ����� "������" - */
                SEND_ERROR("������ FAST: ����������� ���������.") ;

/*---------------- ������ ��������� �� ���������� ������������ ����� */

        if(best_time<0.) {
                            sprintf(result, "Negative best time%.10lf", best_time) ;
                     iface->vSignal("ERROR", result) ;
                         }
        else             { 
                            sprintf(result, "%.10lf", best_time) ;
                     iface->vSignal("DONE", result) ;
                         }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ����������� ����� ����������� �����                  */
/*                 ������������ �� ������� ��������                 */
/*								    */
/* Return:  0 - ��������� �������                                   */
/*         -1 - ����������� ����������� ���������                   */

    int  RSS_Module_Fast::iMethodSimplif(RSS_Object_Path **best_path,
                                         RSS_Object_Path **work_path,
                                                     int  *step,
                                                     int  *new_best,
                                               RSS_IFace  *result_iface,
                                                  double  *path_time )
{

typedef  struct {  int  move_point ;
                   int  base_point ;
                   int  degree ;
                   int  done  ;
                   int  best  ;      }  RSS_Module_Fast_MethodSimplif_rule ; 

#define  RULE  RSS_Module_Fast_MethodSimplif_rule

   static     int  init_flag ;
   static    RULE *rules ; 
   static     int  rules_cnt ; 
   static     int  rule_idx ; 
   static     int  attempt_cnt ;
   static  double  best_time ;
   static     int  best_idx ;
   static     int  success_flag ;
             char  result[1024] ;
             char  value[1024] ;
             char  tmp[1024] ;
             char *end ;
              int  next_flag ;
              int  i ;
              int  j ;
              int  k ;
              int  n ;

#define   METHOD   this->mSimplification_method

#define  T    (*work_path)->Trajectory
#define  P(I, J)  T->Path   [(I)*T->Path_frame+(J)]
#define  V(I, J)  T->Vectors[(I)*T->Path_frame+(J)]

/*---------------------------------------------------- ������������� */

    if(new_best!=NULL)  *new_best=0 ;

/*--------------------------------------- ������������� ������������ */

    if(work_path==NULL) {
/*- - - - - - - - - - - - - - - - - -  ������������� ������� ������� */
       do {
                             best_time=-1 ;

           if(result_iface->vCheck("FATAL"))  break ;
           if(result_iface->vCheck("ERROR"))  break ;

              result_iface->vDecode(NULL, result) ;     
                   best_time=strtod(result, &end) ;

          } while(0) ;

               sprintf(tmp, "iMethodSimplif> ��������� ����� %lf", best_time) ;
                iDebug(tmp, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������������� ������ */
             if(!this->srand_fixed)  srand(time(NULL)) ;
/*- - - - - - - - - - - - - - - - - - - - ������������ ������ ������ */
         rules_cnt=2*((*best_path)->Trajectory->Path_vectors-1)     /* ���������� ����� ������ */
                    * (*best_path)->Trajectory->Path_frame  ;

         rules=(RULE *)                                             /* ��������� ������� ������ */
                gMemRealloc(rules, rules_cnt*sizeof(RULE),
                            "RSS_Module_Fast::iMethodSimplif.rules", 0, 0) ;  
      if(rules==NULL) {
             SEND_ERROR("RSS_Module_Fast::iMethodSimplif\n"
                        "������������ ������ ��� ������� ������ ") ;
                                return(-1) ;
                      }

          n= 0 ;
      for(i= 0 ; i<(*best_path)->Trajectory->Path_vectors-1 ; i++)  /* ��������� ������� ������ */
      for(j=-1 ; j<=1 ; j+=2) 
      for(k= 0 ; k<(*best_path)->Trajectory->Path_frame     ; k++) {
                      rules[n].move_point=i+1 ;
                      rules[n].base_point=rules[n].move_point+j ;
                      rules[n].degree    = k ;
                      rules[n].done      = 0 ;
                            n++ ;
                                                                   } 
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
               iDebug("iMethodSimplif> **** ������� ������� ****", NULL) ;
              sprintf(tmp, "iMethodSimplif> �����:%d  ��������:%d", 
                                 (*best_path)->Trajectory->Path_vectors+1,
                                 (*best_path)->Trajectory->Path_frame     ) ;
               iDebug(tmp, NULL) ;

      for(i=0 ; i<rules_cnt ; i++) {
              sprintf(tmp, "iMethodSimplif> ����a:%2d  ����:%2d  �������:%2d", 
                                            rules[i].move_point,
                                            rules[i].base_point,
                                            rules[i].degree     ) ;
               iDebug(tmp, NULL) ;
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         init_flag= 1 ;
                         rule_idx =-1 ;
                      success_flag= 0 ;
//                    attempt_cnt = 0 ;

                           return(0) ;
                        }
/*------------------------------------- ���� ������ �������� ������� */

             next_flag=0 ;

  do {                                                              /* CIRCLE.1 - ���� ������ �������� ������� */

/*------------------------------------------------ ������ ���������� */

           if(next_flag==1)  sprintf(tmp, "iMethodSimplif> ������� [%d] - ��� ���������", rule_idx) ;
      else if(next_flag==2)  sprintf(tmp, "iMethodSimplif> ������� [%d] - ���������� �����", rule_idx) ;
      else                   sprintf(tmp, "iMethodSimplif> ������� [%d] - ����� %lf", rule_idx, *path_time) ;
                              iDebug(tmp, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
    if(  rule_idx >=  0         &&
         next_flag==  0         &&
        *path_time>   0.        &&
       (*path_time<=best_time ||
                0.> best_time   )  ) do {

      if(*path_time!=best_time) {                                   /* ��� ��������� ������� ������� -  */
            for(i=0 ; i<rules_cnt ; i++)  rules[i].best=0 ;         /*  - ���������� ����������� ������ */
                                }                                   /*   ��� ���������� �������         */
      else                      {                                   /* ���� ��� ������ �������                     */  
              if(rules[rule_idx].best)  break ;                     /*  ������� ������� ��� ��� ������ -           */ 
                                }                                   /*   ���������� ��� ��� ��������� ������������ */

                sprintf(tmp, "iMethodSimplif> ��������� ������� [%d] %lf<=%lf", rule_idx, *path_time, best_time) ;
                 iDebug(tmp, NULL) ;
      
                                 best_idx = rule_idx ;
                                 best_time=*path_time ;
                              success_flag=  1 ;

      if(METHOD==_SEQUENCE_METHOD ||                                /* ��� ������� � ������� ����������� - */
         METHOD==  _RANDOM_METHOD   ) {                             /*  - ��������� ���������� ���������   */
               (*best_path)=iCopyPath(*work_path, _BEST_PATH_NAME) ;  

            if(new_best!=NULL)  *new_best=1 ;
                                      }

                                        } while(0) ;
/*- - - - - - - - - - - - - - - - - -  ����������� ��������� ������� */
         if(rule_idx>=0)  rules[rule_idx].done=1 ;

         if(METHOD==_SEQUENCE_METHOD)   rule_idx++ ;
    else if(METHOD==  _RANDOM_METHOD) {
                                        rule_idx++ ;
                                         n=rules_cnt-rule_idx ;
                                      }
    else if(METHOD==    _FULL_METHOD)   rule_idx++ ;

/*---------------------------------------------- �������� ���������� */

     if(rule_idx>=rules_cnt) {

                sprintf(tmp, "iMethodSimplif> ********* ���� ������� �������� ***************") ;
                 iDebug(tmp, NULL) ;
/*- - - - - - - - - - - - - - - - - - - - - -  - ���� ����� �������� */
           if(!success_flag) {                                      /* ���� �� ���� ���������... */
                                 init_flag=0 ;                      /*  ���������� ������������� */
                                   return(-1) ;                     /*  ������ ������ ���������� */
                             }
/*- - - - - - - - - - - - - - - - - -  ������ ������������� �������� */
      if(METHOD==_FULL_METHOD) {

                sprintf(tmp, "iMethodSimplif> ������������� ��� ������ ������� %d", best_idx) ;
                 iDebug(tmp, NULL) ;
 
         (*work_path)=iCopyPath(*best_path, _WORK_PATH_NAME) ;      /* ������� ����� �������� ������� */

          P(rules[best_idx].move_point, rules[best_idx].degree)=    /* ��������������� ������ ������� */
          P(rules[best_idx].base_point, rules[best_idx].degree) ;

        this->mModulePath->iCalculateVector(&P(rules[best_idx].move_point,   0), 
                                            &P(rules[best_idx].move_point+1, 0), 
                                            &V(rules[best_idx].move_point,   0) ) ;
        this->mModulePath->iCalculateVector(&P(rules[best_idx].move_point-1, 0), 
                                            &P(rules[best_idx].move_point,   0), 
                                            &V(rules[best_idx].move_point-1, 0) ) ;

         (*best_path)=iCopyPath(*work_path, _BEST_PATH_NAME) ;      /* ��������� ������ ������� ��� ������ */

                          *path_time=best_time ;

            if(new_best!=NULL)  *new_best=1 ;
                               }
/*- - - - - - - - - - - - - - - - ��������������� ����������� ������ */
                 rules[best_idx].best= 1 ;                          /* ��������� ������� ��� ���������� */
                                                                    /*   ������ �������                 */
                            rule_idx = 0 ;
                         success_flag= 0 ;

         for(i=0 ; i<rules_cnt ; i++)  rules[i].done=0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                             }
/*----------------------------------------- �������� ����� "�������" */
 
         (*work_path)=iCopyPath(*best_path, _WORK_PATH_NAME) ;      /* ������� ����� �������� ������� */

            rules[rule_idx].done=1 ;

       if(P(rules[rule_idx].move_point, rules[rule_idx].degree)==   /* ���� ������� �� �������� ������... */
          P(rules[rule_idx].base_point, rules[rule_idx].degree)  ) {  next_flag= 1 ; 
                                                                       continue ;     }
          P(rules[rule_idx].move_point, rules[rule_idx].degree)=
          P(rules[rule_idx].base_point, rules[rule_idx].degree) ;

        this->mModulePath->iCalculateVector(&P(rules[rule_idx].move_point,   0), 
                                            &P(rules[rule_idx].move_point+1, 0), 
                                            &V(rules[rule_idx].move_point,   0) ) ;
        this->mModulePath->iCalculateVector(&P(rules[rule_idx].move_point-1, 0), 
                                            &P(rules[rule_idx].move_point,   0), 
                                            &V(rules[rule_idx].move_point-1, 0) ) ;

      for(k= 0 ; k<(*work_path)->Trajectory->Path_frame ; k++)      /* ��������� ���������� ������� � */     
        if(V(rules[rule_idx].move_point-1, k)!=0.)  break ;         /*   ���������� ������            */

        if(k>=(*work_path)->Trajectory->Path_frame) {  next_flag= 2 ; 
                                                       continue ;     }

      for(k= 0 ; k<(*work_path)->Trajectory->Path_frame ; k++)      /* ��������� ���������� ������� � */     
        if(V(rules[rule_idx].move_point, k)!=0.)  break ;           /*   ����������� ������           */

        if(k>=(*work_path)->Trajectory->Path_frame) {  next_flag= 2 ; 
                                                       continue ;     }

                                 next_flag=0 ;

/*------------------------------------- ���� ������ �������� ������� */

     } while(next_flag) ;                                           /* CONTINUE.1 */

/*-------------------------------------------- ���������� ���������� */

                         attempt_cnt++ ;

//  if(attempt_cnt==73)  SEND_ERROR("RSS_Module_Fast::iMethodSimplif - Critical attempt reached!") ;

               iDebug("iMethodSimplif> *************************", NULL) ;
              sprintf(tmp, "iMethodSimplif> ������� %d (%d)", rule_idx, attempt_cnt) ;
               iDebug(tmp, NULL) ;

               iDebug("=== Points ===", NULL) ;
      for(i=0 ; i<=T->Path_vectors ; i++) {
                                             *tmp=0 ;
       for(j=0 ; j< T->Path_frame ; j++) {
                    sprintf(value, "%12.6lf ", P(i,j)) ;
                     strcat(tmp, value) ;
                                         }
                                               iDebug(tmp, NULL) ;
                                          }

               iDebug("=== Vectors ===", NULL) ;
      for(i=0 ; i<T->Path_vectors ; i++) {
                                             *tmp=0 ;
       for(j=0 ; j<T->Path_frame ; j++) {
                    sprintf(value, "%12.6lf ", V(i,j)) ;
                     strcat(tmp, value) ;
                                        }
                                               iDebug(tmp, NULL) ;
                                          }
/*-------------------------------------------------------------------*/

#undef   RULE
#undef   METHOD

#undef   T
#undef   P
#undef   V

   return(0) ;
}


/********************************************************************/
/*								    */
/*             ����������� ����� ��������� �������                  */
/*								    */
/* Return:  0 - ��������� �������                                   */
/*         -1 - ����������� ����������� ���������                   */

    int  RSS_Module_Fast::iMethodCut(RSS_Object_Path **best_path,
                                     RSS_Object_Path **work_path,
                                                 int  *step,
                                                 int  *new_best,
                                           RSS_IFace  *result_iface,
                                              double  *path_time )
{
   static     int  init_flag ;
   static     int  attempt_cnt ;
   static  double  best_time ;
           double *segm_times ;
           double  segm_time_max ;
             char  result[1024] ;
             char  value[1024] ;
             char  desc[1024] ;
             char  tmp[1024] ;
             char *end ;
              int  i ;
              int  j ;
              int  k ;
              int  n ;

/*------------------------------------------------- ������� �������� */

    if(work_path==NULL)  return(0) ;

/*---------------------------------------------------- ������������� */

    if(new_best!=NULL)  *new_best=0 ;

/*------------------------------------------------ ��������� ������� */

    if(init_flag==0) {
/*- - - - - - - - - - - - - - - - - �������� ������ ������ ��������� */
       if(result_iface->vCheck("FATAL"))  return(-1) ;
       if(result_iface->vCheck("ERROR"))  return(-1) ;

              result_iface->vDecode(NULL, result) ;     
                   best_time=strtod(result, &end) ;

               sprintf(tmp, "iMethodCut> ��������� ����� %s", result) ;
                iDebug(tmp, NULL) ;

                  n=(*best_path)->Trajectory->Path_vectors ;        /* ���. ����� ��������� */
         segm_times=(double *)calloc(n, sizeof(double)) ;           /* ��������� ������ ������ ��������� */

             end=strchr(end, ':') ;                                 /* �������� �� ������ ������ ��������� */
          if(end==NULL)  end="" ;

        for(i=0 ; i<n ; i++) {                                      /* ��������� ����e�� ��������� */
          if(*end==0) {  SEND_ERROR("iMethodCut. �������� ������ ������ ���������") ;
                                 break ;  }
             segm_times[i]=strtod(end+1, &end) ;                    
                             }   
/*- - - - - - - - - - - - - - - - - - -  ���. �������� ��� ��������� */
        for(k=0, i=1 ; i<n ; i++)                                   /* ���.������� � ���������� �������� */
          if(segm_times[i]>segm_times[k])  k=i ;

             segm_time_max=segm_times[k] ;                          /* ����.���������� ����� �������� */ 

                      free(segm_times) ;                            /* �����.������ ������ ��������� */

               sprintf(tmp, "iMethodCut> ��� ��������� ������ ������� %d "
                                        "�� �������� %lf", k, segm_time_max) ;
                iDebug(tmp, NULL) ;

         if(segm_time_max<this->mCutSegment_MinTime) {              /* ���� �������� ������� ������... */

               sprintf(tmp, "iMethodCut> ********* ���� ��������� �������� ***************") ;
                iDebug(tmp, NULL) ;
                    return(-1) ;
                                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - - ��������� �������� */
#define  T               ((*work_path)->Trajectory)
#define  P(I, J)  (T->Path   [(I)*T->Path_frame+(J)])
#define  V(I, J)  (T->Vectors[(I)*T->Path_frame+(J)])

         (*work_path)=iCopyPath(*best_path, _WORK_PATH_NAME) ;      /* ������� ����� �������� ������� */

      T->Path_vectors+=1 ;                                          /* ��������� ��� ���� ������� */

                      sprintf(desc, "[%s].Path", (*work_path)->Name) ;
      T->Path   =(double *)
                  gMemRealloc( T->Path,
                              (T->Path_vectors+1)* 
                               T->Path_frame*sizeof(double), desc, 0, 0) ;

                      sprintf(desc, "[%s].Vectors", (*work_path)->Name) ;
      T->Vectors=(double *)
                   gMemRealloc(T->Vectors,
                               T->Path_vectors*
                               T->Path_frame*sizeof(double), desc, 0, 0) ;
   if(T->Path   ==NULL &&
      T->Vectors==NULL   ) {
           SEND_ERROR("iMethodCut. ������������ ������ ��� ����� ����������") ;
                                            return(-1) ;
                           }

      memmove(&V(k+1,0), &V(k,0), (T->Path_vectors-k-1)*            /* �������� ������ ��������   */
                                   T->Path_frame*sizeof(double)) ;  /*   "������������" �� ���� K */
      memmove(&P(k+1,0), &P(k,0), (T->Path_vectors-k  )*            /* �������� ������ �����      */
                                   T->Path_frame*sizeof(double)) ;  /*   "������������" �� ���� K */

    for(i=0 ; i<T->Path_frame ; i++)                                /* ������� K-�� ����� �� ����� */
                          P(k+1, i)=(P(k, i)+P(k+2, i))/2. ;        /*   ��������� �������         */

/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
                         attempt_cnt++ ;

//    if(attempt_cnt==109)  SEND_ERROR("RSS_Module_Fast::iMethodSimplif - Critical attempt reached!") ;

               iDebug("iMethodCut> *************************", NULL) ;
              sprintf(tmp, "iMethodCut> �������� (%d)", attempt_cnt) ;
               iDebug(tmp, NULL) ;

               iDebug("=== Points ===", NULL) ;
      for(i=0 ; i<=T->Path_vectors ; i++) {
                                             *tmp=0 ;
       for(j=0 ; j< T->Path_frame ; j++) {
                    sprintf(value, "%12.6lf ", P(i,j)) ;
                     strcat(tmp, value) ;
                                         }
                                               iDebug(tmp, NULL) ;
                                          }

               iDebug("=== Vectors ===", NULL) ;
      for(i=0 ; i<T->Path_vectors ; i++) {
                                             *tmp=0 ;
       for(j=0 ; j<T->Path_frame ; j++) {
                    sprintf(value, "%12.6lf ", V(i,j)) ;
                     strcat(tmp, value) ;
                                        }
                                               iDebug(tmp, NULL) ;
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         init_flag=1 ;

                       return(0) ;
                   }

#undef   T
#undef   P
#undef   V

/*-------------------------------------- ������ ���������� ��������� */

/*       (*path_time<=best_time ||
                  0.> best_time   )  ) do {*/
/* ������ ��� ������ BEST_TIME=-1 */

    if(*path_time>best_time ||
       *path_time<      0     ) {
          SEND_ERROR("RSS_Module_Fast::iMethodCut - ��������� ������� ��� ���������!") ;
                                }

         (*best_path)=iCopyPath(*work_path, _BEST_PATH_NAME) ;      /* ��������� ������ ������� ��� ������ */

                         *new_best=1 ;
                         init_flag=0 ;

/*-------------------------------------------------------------------*/

   return(-1) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ������ � �����                      */

   void  RSS_Module_Fast::iDebug(char *text, char *path)
{
   static int  init_flag ;
         FILE *file ;         


    if(path==NULL) {
                             path="t_fast.log" ;
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
/*		      ����������� ����������                        */

  RSS_Object_Path *
       RSS_Module_Fast::iCopyPath(RSS_Object_Path *path_object,
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
/*	               ���������� �������                           */

  void  RSS_Module_Fast::iExecute(char *execute, RSS_IFace *result)

{
   char  command[1024] ;
    int  status ;
   char  text[1024] ;
    int  i ;

/*------------------------------------------------------- ���������� */

                     strcpy(command, execute) ;

   if(result!=NULL)  result->vClear() ; 

/*------------------------------ ����� ��������� ����������� ������� */

    for(i=0 ; i<this->kernel->modules_cnt ; i++) {
      
         status=this->kernel->modules[i].entry->vExecuteCmd(command, result) ;
      if(status==-1)  break ;
      if(status== 0)  break ;
                                                 }
/*---------------------------------- ��������� ���������� ���������� */

   if( result   !=    NULL  &&                                      /* ��������� ������� ������-���� ������� */
      !result->vCheck(NULL)   ) {

                result->vSignal("FATAL", "No iface") ;

     if(i<this->kernel->modules_cnt)
                        sprintf( text, "���������� ������� �� ������������ ��������� ���������� ������:\n"
                                       "  %s", command) ;
     else               sprintf( text, "���������� ��� ������� �� ������:\n"
                                       "  %s", command) ;
                     SEND_ERROR( text) ;
                             return ;

                                 }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*                ���������� ��������� �����������                   */

  void  RSS_Module_Fast::iText_subst(char *text, char *name, char *value)

{
  char  mask[128] ;
   int  mask_size ;
   int  field_size ;
   int  value_size ;
   int  out_size ;
   int  space_char ;
  char *beg ;
  char *end ;

/*------------------------------------------------------- ���������� */

                     sprintf(mask, "%s", name) ;
            mask_size=strlen(mask) ;
           value_size=strlen(value) ;

                  space_char=' ' ;

/*------------------------------------- ��������-�������������� ���� */

   do {                                                             /* CIRCLE.0 */
           beg=strstr(text, mask) ;                                 /* ���� ���� ����� */
        if(beg==NULL)  break ;

        if(beg[mask_size]==':') {                                   /* ���.������ ���� ������ */
                   out_size=strtol(beg+mask_size+1, &end, 10) ;
                 field_size= end-beg ;
                                }
        else                    {
                   out_size=value_size ;
                field_size= mask_size ;
                                }

                memmove(beg+abs(out_size), beg+field_size,          /* ����� ������ */
                                       strlen(beg+field_size)+1) ;

        if(abs(out_size)>value_size)                                /* ���������� ��������� */
         if(out_size>0) {
                 memset(beg+value_size, space_char, out_size-value_size) ;
                        }
         else           {
                                         out_size=abs(out_size) ;
                 memset(beg, space_char, out_size-value_size) ;
                        beg+=out_size-value_size ;
                        }

        if(out_size>value_size)  memmove(beg, value, value_size) ;  /* ����������� �������� */
        else                     memmove(beg, value,   out_size) ;

      } while(1) ;                                                  /* CONTINUE.0 */

/*-------------------------------------------------------------------*/
}


