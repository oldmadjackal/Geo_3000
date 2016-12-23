/********************************************************************/
/*								    */
/*      ������ ������ "������ ��������� ����������"                 */
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
/*		    	����������� ������                          */

     static   RSS_Module_Scan  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

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
/**                   �������� ������ ������                       **/
/**                "������ ��������� ����������"                   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_Scan_instr  
         RSS_Module_Scan_InstrList[]={

 { "help",      "?",  "#HELP (?) - ������ ��������� ������", 
                       NULL,
                      &RSS_Module_Scan::cHelp   },
 { "config",    "c",  "#CONFIG (C) - ������� ���������� �����������", 
                       NULL,
                      &RSS_Module_Scan::cConfig   },
 { "run",       "r",  "#RUN (R) -  ������ �������� ������������ ����������",
                       NULL,
                      &RSS_Module_Scan::cRun   },
 { "bases",     "b",  "#BASES (B) -  ����������/������������ ����� ������� �����",
                       NULL,
                      &RSS_Module_Scan::cBases   },
 { "clear",     "cl", "#CLEAR (CL) -  ������� ����� ������� �����",
                       NULL,
                      &RSS_Module_Scan::cClear   },
 { "excel",     "e",  "#EXCEL (E) -  �������� ����������� ������������ � MS Excel",
                       NULL,
                      &RSS_Module_Scan::cExcel  },
 {  NULL }
                                       } ;


/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_Scan_instr *RSS_Module_Scan::mInstrList  =NULL ;
                            long  RSS_Module_Scan::mDebug_stamp=  0 ;


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

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
/*		        ���������� ������			    */

    RSS_Module_Scan::~RSS_Module_Scan(void)

{
}


/********************************************************************/
/*								    */
/*		      ������������� ������      		    */

  void  RSS_Module_Scan::vInit(void)

{
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_Scan::vExecuteCmd(const char *cmd)

{
   return( vExecuteCmd(cmd, NULL) ) ;
}


  int  RSS_Module_Scan::vExecuteCmd(const char *cmd, RSS_IFace *iface)

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Task Scan:") ;
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
       if(status)  SEND_ERROR("������ SCAN: ����������� �������") ;
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

    void  RSS_Module_Scan::vShow(char *layer)

{
     int  status ;
     int  i ;
 
/*----------------------------------------- ��������������� �������� */

  if(!stricmp(layer, "ZOOM")) {

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - ������� ���������� ����������� */

     for(i=0 ; i<this->mBasics_cnt ; i++)                           /* ������� ����� ������� ����� */
                PlaceMarker(&this->mBasics[i]) ;

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP  - ������� ���������� ����������� */

                              }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*             ��������� �������� � ��������� ������                 */

    void  RSS_Module_Scan::vChangeContext(void)

{
   if(!stricmp(mContextAction, "FORM" ))  FormMarker () ;
   if(!stricmp(mContextAction, "PLACE"))  PlaceMarker(mContextObject) ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Scan::vReadSave(std::string *data)

{
                       char *buff ;
                        int  buff_size ;
                       char *work ;
                       char *value ;
                       char *end ;
                       char *tmp ;
                        int  n ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE SCAN\n", 
                      strlen("#BEGIN MODULE SCAN\n")) )  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)gMemCalloc(1, buff_size, "RSS_Module_Scan::vReadSave", 0, 0) ;

       strcpy(buff, data->c_str()) ;

/*------------------------------------------------- ��������� ������ */

                work=buff+strlen("#BEGIN MODULE SCAN\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
/*- - - - - - - - - - - - - - - - - -  ��������� � ���������� ������ */
          end=strchr(work, '\n') ;
       if(end==NULL)  break ;
         *end=0 ;

          value=strchr(work, '=') ;
       if(value!=NULL) {  *value=0 ;
                           value++ ;  }
          
/*- - - - - - - - - - - - - - - - - - - -  ����������� ������ ������ */
       if(!stricmp(work, "CHECK_CMD"        ))  strncpy(mCheck_cmd, value, sizeof(mCheck_cmd)-1) ;
       if(!stricmp(work, "BASE_POINT_SHOW"  ))  mBasePoints_show  =strtol(value, &end, 10) ;
       if(!stricmp(work, "BASE_POINT_REDRAW"))  mBasePoints_redraw=strtol(value, &end, 10) ;
/*- - - - - - - - - - - - - - - - - - - - - - ��������� ������������ */
       if(!memicmp(work, "PAR", strlen("PAR"))) {

#define  P   this->mScanPars

            n=strtoul(work+strlen("PAR"), &tmp, 10) ;               /* ����� ��������� */
         if(n>=this->mScanPars_cnt)  this->mScanPars_cnt=n+1 ;      /* ������������ ������� ���������� */

            value=strstr(work, "NAME:") ;                           /* ��� ��������� */
         if(value!=NULL) {
                           strncpy(P[n].name, value+strlen("NAME:"), sizeof(P[n].name)) ;
                        tmp=strchr(P[n].name, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "CATEGORY:") ;                       /* ��������� ��������� */
         if(value!=NULL)  P[n].category=strtoul(value+strlen("CATEGORY:"), &tmp, 10) ;

            value=strstr(work, "OBJECT:") ;                          /* �������� ������������ ������� */
         if(value!=NULL) {
                           strncpy(P[n].object, value+strlen("OBJECT:"), sizeof(P[n].object)) ;
                        tmp=strchr(P[n].object, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "OBJECT_PAR:") ;                      /* ������ �� �������� ������������ ������� */
         if(value!=NULL) {
                           strncpy(P[n].par_link, value+strlen("OBJECT_PAR:"), sizeof(P[n].par_link)) ;
                        tmp=strchr(P[n].par_link, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }


            value=strstr(work, "KIND:") ;                           /* ��� ��������� */
         if(value!=NULL)  P[n].kind=strtoul(value+strlen("KIND:"), &tmp, 10) ;

            value=strstr(work, "VALUE:") ;                          /* ������� �������� */
         if(value!=NULL) {
                           strncpy(P[n].value.formula, value+strlen("VALUE:"), sizeof(P[n].value.formula)) ;
                        tmp=strchr(P[n].value.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_MIN:") ;                      /* ������� �������� ��������� */
         if(value!=NULL) {
                           strncpy(P[n].value_min.formula, value+strlen("VALUE_MIN:"), sizeof(P[n].value_min.formula)) ;
                        tmp=strchr(P[n].value_min.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_MAX:") ;                      /* ������� ��������� ��������� */
         if(value!=NULL) {
                           strncpy(P[n].value_max.formula, value+strlen("VALUE_MAX:"), sizeof(P[n].value_max.formula)) ;
                        tmp=strchr(P[n].value_max.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

            value=strstr(work, "VALUE_STEP:") ;                     /* ������� ���� ��������� */
         if(value!=NULL) {
                           strncpy(P[n].value_step.formula, value+strlen("VALUE_STEP:"), sizeof(P[n].value_step.formula)) ;
                        tmp=strchr(P[n].value_step.formula, ' ') ;
                     if(tmp!=NULL)  *tmp=0 ;
                         }

#undef   P

                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- ������������ �������� */

                        gMemFree(buff) ;

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Scan::vWriteSave(std::string *text)

{
  char  value[1024] ;
   int  i ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE SCAN\n" ;

/*------------------------------------------------ ������� ��������� */

    sprintf(value, "CHECK_CMD=%s\n", mCheck_cmd) ;
     *text+=value ;
    sprintf(value, "BASE_POINT_SHOW=%d\n", mBasePoints_show) ;
     *text+=value ;
    sprintf(value, "BASE_POINT_REDRAW=%d\n", mBasePoints_redraw) ;
     *text+=value ;

/*----------------------------------- ������ ���������� ������������ */

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
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ��������       		    */

     int  RSS_Module_Scan::vGetParameter(char *name, char *value)

{
/*-------------------------------------------------- �������� ������ */

    if(!stricmp(name, "$$MODULE_NAME")) {

         sprintf(value, "%-20.20s - ������ ������������� �������� ���������� ������� ��������", "SCAN") ;
                                        }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_Scan::cHelp(char *cmd, RSS_IFace *iface)

{ 

    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Task_Scan_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CONFIG                  */

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
/*		      ���������� ���������� RUN                     */

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
            int  i_deep ;              /* ������ "�������" ������������ ��������� */
            int  i ;
            int  j ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------- �������� ����������� ���������� */

     if(mCheck_cmd[0]==0) {
         SEND_ERROR("�� ������ ������� ����������� ������������� ������ ������.") ;
                                 return(0) ;
                          }

     if(mScanPars_cnt==0) {
         SEND_ERROR("�� ��������� ����������� ��������.") ;
                                 return(0) ;
                          }
/*---------------------------------------- �������� ��������� ������ */
/*------------------------------- ���������� ���������� ������������ */

     for(i=0 ; i<mScanPars_cnt ; i++) {
                                mScanPars[i].value     .fixed= 0 ;
                                mScanPars[i].value_min .fixed= 0 ;
                                mScanPars[i].value_max .fixed= 0 ;
                                mScanPars[i].value_step.fixed= 0 ;
                                mScanPars[i].step            =-1 ;
                                      }
/*-------------------------- ���������� ������� �������������� ����� */

         if(mResult     !=NULL)  free(mResult     ) ;
         if(mResult_pars!=NULL)  free(mResult_pars) ;

                                      mResult     =NULL ;
                                      mResult_pars=NULL ;
                                      mResult_cnt =  0 ;
                                      mResult_max =  0 ;

/*------------------------------------- ������� ������ ������� ����� */

   if(mBasics_cnt) {

     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].dlist_idx) {
           RSS_Kernel::display.ReleaseList(mBasics[i].dlist_idx) ;
                                           mBasics[i].dlist_idx=0 ;
                                }

                           free(mBasics) ;
                                mBasics    =NULL ;
                                mBasics_cnt=  0 ;

                          this->kernel->vShow("REFRESH") ;          /* ����������� ����� */
                   } 
/*------------------------------------------ ���������� ������������ */

               result_init=0 ;
                    i_deep=0 ;

     do {                                                           /* CIRCLE.0 */
                mDebug_stamp++ ;
             if(mDebug_stamp==101) {
                                        mDebug_stamp+=0 ;
                                   }

/*----------------------------------- ������� �������� �� ���������� */

         for(i=i_deep ; i<mScanPars_cnt ; i++)
           if(mScanPars[i].kind==_SLAVE_PAR) {                      /* ���� ��������� ��������... */
                   status=iCalcSlaveValue(&mScanPars[i]) ;
                if(status==-1)  break ;
                                             }
           else                              {                      /* ���� ����������� ��������... */
                   i_deep= i ;                                      /*  ��������� ��� ����� */
                   status=iCalcScanValue(&mScanPars[i]) ;           /*  ���������� ��������� �������� � ��������� */
                if(status==-1)  break ;
                if(status== 0)  continue ;

                                         mScanPars[i].step=-1 ;     /*  ����.�������� */

              for(i-- ; i>=0 ; i--)                                 /*  ���� ��������� "�������" */
                if(mScanPars[i].kind==_SCAN_PAR)  break ;           /*    ����������� ��������   */ 

                if(i<0)  break ;                                    /*  ���� ��� "���������" ����������... */

                      i-- ;                                         /*  ���� ��������� ��� ��������� �� ������ "��������" */
                                             }

                if(i<0)  break ;                                    /*  ���� ��� "���������" ����������... */

                if(status==-1)  break ;                             /* ���� ������ ��� ������ � �����������... */

/*-------------------------------- "�������" �� ����������� �������� */

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
/*--------------------------------------------- ��������� ���������� */

//        SEND_ERROR("������ SCAN: ��������� ����������") ;

                                          iface_result.vClear() ;
              iExecute(this->mCheck_cmd, &iface_result) ;

//        SEND_ERROR("������ SCAN: ��������� ���������") ;

         if(iface_result.vCheck("FATAL"))  break ;

         if(iface_result.vCheck("ERROR"))  error_flag= 1 ;
         else                              error_flag= 0 ;

               iface_result.vDecode(NULL, text) ;
                      result=strtod(text, &end) ;

/*----------------------------------- ��������� ��������� ���������� */

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
/*---------------------------------------- ����������� ������� ����� */

   if(mBasePoints_show) {
/*- - - - - - - - - - - - - - - - - - ����������� "�������" �������� */
                             basics_cnt=0 ;

     for(i=0 ; i<mScanPars_cnt ; i++)                               /* ����� ���������� ������������ �������� */
        if(mScanPars[i].category==_OBJECT_PAR)                      /*  ������� ����� �������� � ����������   */
         if(!stricmp(mScanPars[i].par_link+                         /*   ������ ����� ��������                */
                      strlen(mScanPars[i].par_link)-5, "_base")) {

          for(j=0 ; j<basics_cnt ; j++)
            if(!stricmp(basics[j], mScanPars[i].object))  break ;

            if(j<basics_cnt) continue ;

                     basics[basics_cnt]=mScanPars[i].object ;
                            basics_cnt++ ;
                                                                 }
/*- - - - - - - - - - - - - - - -  ������������ ������ ������� ����� */
     for(i=0 ; i<basics_cnt ; i++) {                                /* LOOP - ���������� "�����������������" ������� ����� �������� */

        for(j=0 ; j<OBJECTS_CNT ; j++)                              /* �������������� ������ */
          if(!stricmp(OBJECTS[j]->Name, basics[i])) break ;
                
          if(j>=OBJECTS_CNT)  continue ;

                cnt=OBJECTS[j]->vGetBasePoint(b_point) ;            /* ��������� ��������� ������� ����� ������� */

        for(cnt-- ; cnt>=0 ; cnt--) {                               /* LOOP - ���������� ������� ����� ������� */

         for(j=0 ; j<mBasics_cnt ; j++)                             /* ��������� ���������� � �����         */
           if(mBasics[j].point.x   ==b_point[cnt].x   &&            /*  �� ������������������ ������� ����� */ 
              mBasics[j].point.y   ==b_point[cnt].y   &&
              mBasics[j].point.z   ==b_point[cnt].z     )  break ;

          if(j==mBasics_cnt) {                                       /* ���� ����� ����� ���... */
             mBasics=(RSS_Module_Scan_basicp *)                      /*  ������������ �� */ 
                       realloc(mBasics, (mBasics_cnt+1)*sizeof(*mBasics)) ;

                mBasics[mBasics_cnt].point    =b_point[cnt] ;
                mBasics[mBasics_cnt].good     = 0 ;
                mBasics[mBasics_cnt].error    = 0 ;
                mBasics[mBasics_cnt].draw     = 1 ;
                mBasics[mBasics_cnt].dlist_idx= 0 ;
                        mBasics_cnt++ ;
                            }

            if(error_flag) {                                         /* ������������ �������� ������� �����   */
                              mBasics[j].error=1 ;                   /*  � ����������� � ������������ ������� */
                           }
            else           {
                    if(mBasics[j].good==0) {  mBasics[j].result_min=result ;
                                              mBasics[j].result_max=result ;  }
                   
                    if(mBasics[j].result_min>result)  mBasics[j].result_min=result ;
                    if(mBasics[j].result_max<result)  mBasics[j].result_max=result ;

                              mBasics[j].good=1 ;
                           } 

                              mBasics[j].draw=1 ;

                                    }                               /* ENDLOOP - ���������� ������� ����� ������� */

                                   }                                /* ENDLOOP - ���������� "�����������������" ������� ����� �������� */
/*- - - - - - - - - - - - - - - - - - - - - -  ��������� ����� ����� */
     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].draw) {
                                      mBasics[i].draw=0 ;

                                     FormMarker_() ;                /* ������������ ������� */
                                    PlaceMarker_(&mBasics[i]) ;     /* ������������ ���������������� ��������� ������� */
                           }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                        } 
/*-------------------------------------------- ���������� ���������� */
/*- - - - - - - - - - - - - - - - - - - - - - - - - ��������� ������ */
   if(mResult_cnt==mResult_max) {
                                        mResult_max+=100 ;

         mResult=(RSS_Module_Scan_result *)
                   realloc(mResult, mResult_max*sizeof(*mResult)) ;
      if(mResult==NULL) {
          SEND_ERROR("������ SCAN: ������������ ������ ��� ���������� ����������.") ;
                           break ;
                        }

         mResult_pars=(p_value *)
                   realloc(mResult_pars, mResult_max*mScanPars_cnt*sizeof(p_value)) ;
      if(mResult_pars==NULL) {
          SEND_ERROR("������ SCAN: ������������ ������ ��� ���������� ����������.") ;
                                break ;
                             }

        for(i=0 ; i<mResult_cnt ; i++)                              /* ���������� ������ */
              mResult[i].pars=mResult_pars+i*mScanPars_cnt ;

                                }
/*- - - - - - - - - - - - - - - - - - - - - -  ���������� ���������� */
           mResult[mResult_cnt].result    =result ;
           mResult[mResult_cnt].error_flag=error_flag ;
           mResult[mResult_cnt].pars      =mResult_pars+mResult_cnt*mScanPars_cnt ;

        for(i=0 ; i<mScanPars_cnt ; i++)
          iCalcGetValue(&mScanPars[i], &mResult[mResult_cnt].pars[i]) ;

                   mResult_cnt++ ;

              iSaveBinResult(NULL, NULL, mResult_cnt-1) ;           /* ��������� ������������� ���������� � ���� */

/*------------------------------------------ ���������� ������������ */

   if(mBasePoints_redraw)
       for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;  /* ����������� �������� ������� ����� */

        } while(1) ;                                                /* CONTINUE.0 */

/*---------------------------------------------- �������� ���������� */

   if(mBasePoints_show)
       for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;  /* ����������� �������� ������� ����� */

         this->kernel->vShow("REFRESH") ;                           /* ����������� ����� */

              iSaveBinResult("Generate", NULL, 0) ;                 /* ���������� ���������� � ���� */

          SEND_INFO("������ SCAN: ������� ���������� ��������.") ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� BASES                   */

  int  RSS_Module_Scan::cBases(char *cmd, RSS_IFace *iface)

{ 
  int  i ;

                                     FormMarker_ () ;               /* ������������ ������� */
     for(i=0 ; i<mBasics_cnt ; i++)  PlaceMarker_(&mBasics[i]) ;    /* ����������� �������� ������� ����� */

              this->kernel->vShow("REFRESH") ;                      /* ����������� ����� */

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CLEAR                   */

  int  RSS_Module_Scan::cClear(char *cmd, RSS_IFace *iface)

{ 
  int  i ;

     for(i=0 ; i<mBasics_cnt ; i++)
       if(mBasics[i].dlist_idx) {
           RSS_Kernel::display.ReleaseList(mBasics[i].dlist_idx) ;
                                           mBasics[i].dlist_idx=0 ;
                                }

              this->kernel->vShow("REFRESH") ;                      /* ����������� ����� */

   return(0) ;
}


/********************************************************************/
/*								    */
/*                  ���������� ���������� EXCEL                     */

  int  RSS_Module_Scan::cExcel(char *cmd, RSS_IFace *iface)

{ 
   char  path[FILENAME_MAX] ;

/*------------------------------------------- ������������ CSV-����� */

                     sprintf(path, "Generate") ;                    /* ������ ����-��������� ���� ����� */
              iSaveCsvResult(path) ;                                /* ���������� ���������� � CSV-���� */

/*------------------------------------------------- �������� � Excel */

            iFormExcelResult(path) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ������ � �����                      */

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
/*	               ���������� �������                           */

  void  RSS_Module_Scan::iExecute(char *execute, RSS_IFace *result)

{
   char  command[1024] ;
    int  status ;
   char  text[1024] ;
    int  i ;

/*------------------------------------------------------- ���������� */

                            strcpy(command, execute) ;

         if(result!=NULL)   result->vClear() ; 

/*------------------------------ ����� ��������� ����������� ������� */

    for(i=0 ; i<this->kernel->modules_cnt ; i++) {
      
      if(result!=NULL)  status=this->kernel->modules[i].entry->vExecuteCmd(command, result) ;
      else              status=this->kernel->modules[i].entry->vExecuteCmd(command,  NULL ) ;

      if(status==-1)  break ;
      if(status== 0)  break ;
                                                 }
/*---------------------------------- ��������� ���������� ���������� */

   if( result!=NULL         &&                                      /* ��������� ������� ������-���� ������� */
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


/********************************************************************/
/*								    */
/*	  ����������� �������� ��� "������������" ���������         */

   int  RSS_Module_Scan::iCalcScanValue(RSS_Module_Scan_par *par)

{
    int  status ;
   char  text[1024] ; 

/*---------------------------------------------------- ������������� */

   if(par->step==-1) {

              status=iCalcValue(&par->value_min) ;                  /* ���.��������� �������� ��������� */
           if(status) {                                             /* ���� ������... */
                 sprintf(text, "������! �������� %s (��������� ��������) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

                          par->value.value=par->value_min.value ;   /* ����������� ������� �������� */
                          par->step       = 0 ;                     /* ����.������� ��������� */

                             return(0) ;
                     }
/*--------------------------------------------- ���������� ��������� */

              status=iCalcValue(&par->value_max) ;                  /* ���.�������� �������� ��������� */
           if(status) {                                             /* ���� ������... */
                 sprintf(text, "������! �������� %s (�������� ��������) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

              status=iCalcValue(&par->value_step) ;                 /* ���.��� ��������� �������� */
           if(status) {                                             /* ���� ������... */
                 sprintf(text, "������! �������� %s (��� ���������) : %s", 
                                       par->name, mError) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                      }

           if(par->value_step.value<=0) {
                 sprintf(text, "������! �������� %s (��� ���������) : ������������ �������� %lf", 
                                       par->name, par->value_step.value) ;
              SEND_ERROR(text) ;
                        return(-1) ;
                                        } 

           if(    par->value.value+                                 /* ���� �������� ������� ����... */
              0.5*par->value_step.value>=
                  par->value_max.value   )  return(1) ;

                 par->value.value+=par->value_step.value ;          /* ���. ��������� �������� */

           if(par->value.value>par->value_max.value)                /* ��� ������ �� ������� ��������� - */
                 par->value.value=par->value_max.value ;            /*  - ������������ ��� ���           */

                 par->step++ ;                                      /* ������������ ������� ��������� */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	  ����������� �������� ��� ���������� ���������             */

   int  RSS_Module_Scan::iCalcSlaveValue(RSS_Module_Scan_par *par)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*	      ������ �������� ��������� ��� ����������              */

   int  RSS_Module_Scan::iCalcGetValue(RSS_Module_Scan_par *par, 
                                                   p_value *data)

{
     data->f_value=par->value.value ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      ������ �������� ��������� � ��������� ����            */

   int  RSS_Module_Scan::iCalcTextValue(RSS_Module_Scan_par *par, 
                                                    p_value *data, 
                                                       char *text )

{
     sprintf(text, "%.10f", data->f_value) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                  ����������� �������� ���������                  */

   int  RSS_Module_Scan::iCalcValue(RSS_Module_Scan_value *value)

{
   char *formula ;
   char *end ;
    int  i ;
    int  j ;

/*------------------------------------------------- ������� �������� */

    if(value->fixed)  return(0) ;
   
/*----------------------------------------------- ���������� ������� */

                    formula=value->formula ;

     for(i=0, j=0 ; formula[i] ; i++)                               /* ������� �� ������� ��� ������� */
       if(formula[i]!=' '  &&                                       /*   � ���������                  */
          formula[i]!='\t'   ) {  formula[j]=formula[i] ;
                                          j++ ;            }

                                  formula[j]= 0 ;                   /* ����������� "��������" ������� */

       if(j==0) {
           strcpy(mError, "����������� ������� ������� ��������") ;
                            return(0) ;
                }
/*------------------------------- �������� �� ������������� �������� */

             value->value=strtod(formula, &end) ;                   /* �����.������� ��� ������� ����� */

         if(*end==0) {   value->fixed=1 ;                           /* ���� ������ - �� ������ ������� */
                            return(0) ;    }                        /*  �������������� ����� � ������� */

/*-------------------------------------------- ���������� �� ������� */

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	      ���������� ������������ ����� ����������              */

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

/*------------------------------------------------------- ���������� */

                time_abs=     time( NULL) ;
                  hhmmss=localtime(&time_abs) ;
        
/*------------------------------ ����������� ����� ����� � ��������� */

   if(title==NULL) {                                                /* ���� ��������� �� �����... */
                          title=title_tmp ;
                  sprintf(title, "����������� ���� ����������� ������ %02d:%02d:%02d %02d_%02d_%02d",
                                    hhmmss->tm_hour, hhmmss->tm_min,  hhmmss->tm_sec,
                                    hhmmss->tm_mday, hhmmss->tm_mon+1, hhmmss->tm_year-100 ) ;              
                   }

   if(path ==NULL) {                                                /* ���� ���� �� �����... */
                          path=path_tmp ;
                  sprintf(path, "Temp.scan", time_abs) ;
                   }
   else
   if(!stricmp(path, "Generate")) {                                 /* ���� ����-��������� �����... */
                          path=path_tmp ;
                  sprintf(path, "Temp_%lx.scan", time_abs) ;
                                  }
/*--------------------------------------------------- �������� ����� */

      file=fopen(path, "wt") ;
   if(file==NULL) {
                        sprintf(text, "������ SCAN: ������ �������� ����� %s",
                                         path) ;
       if(!silence)  SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*--------------------------------------- ���������� �������� ������ */

/*------------------------------- ���������� ������������ ���������� */

   sprintf(text, "Title:%s\n", title) ;
    fwrite(text, 1, strlen(text), file) ;
   sprintf(text, "Points:%d\n", this->mResult_cnt) ;
    fwrite(text, 1, strlen(text), file) ;
   sprintf(text, "Parameters:%d\n", this->mScanPars_cnt) ;
    fwrite(text, 1, strlen(text), file) ;

/*------------------------------------- ���������� ������ ���������� */

      for(i=0 ; i<this->mResult_cnt ; i++) {                        /* ��������� ������ ����� ���������� ��������� */

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
/*--------------------------------------------------- �������� ����� */

                 fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	          ���������� CSV-����� ����������                   */

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

/*------------------------------------------------------- ���������� */

                 time_abs=time(NULL) ;

                       _getcwd(folder, sizeof(folder)-1) ;
        
/*------------------------------ ����������� ����� ����� � ��������� */

   if(path ==NULL               ) {                                 /* ���� ���� �� �����... */
                          path=path_tmp ;
                  sprintf(path, "Temp.scan", time_abs) ;
                                  }
   else
   if(!stricmp(path, "Generate")) {                                   /* ����  ����-��������� �����... */
                  sprintf(path, "%s/Temp_%lx.txt", folder, time_abs) ;
                                  }
/*--------------------------------------------------- �������� ����� */

      file=fopen(path, "wt") ;
   if(file==NULL) {
                        sprintf(text, "������ SCAN: ������ �������� ����� %s",
                                         path) ;
                     SEND_ERROR(text) ;
                          return(-1) ;
                  }
/*------------------------------------- ���������� ������ ���������� */

      for(i=0 ; i<this->mResult_cnt ; i++) {                        /* ��������� ������ ����� ���������� ��������� */

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
/*--------------------------------------------------- �������� ����� */

                 fclose(file) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*	          �������� ���������� � MS Excel                    */

   int  RSS_Module_Scan::iFormExcelResult(char *path)

{
  OLE_Excel  Excel ;
       char  macro[32] ;
       char  format[1024] ;
        int  status ;
       char  tmp[2048] ;
        int  i ;

/*-------------------------------------------- ������������� � Excel */

         Excel.Activate() ;
         Excel.OpenNew() ;

/*--------------------------------------- �������� ��������� ������� */

         Excel.CreateMacro("Sub Initial_Pages_delete()\n"
                           "Application.DisplayAlerts = False\n"
                           "Sheets(3).Delete\n"
                           "Sheets(2).Delete\n"
                           "Sheets(1).Name=\"Geo2D_Scan\"\n"
                           "End Sub\n") ;

         Excel.ExecuteMacro("Initial_Pages_delete") ;
         Excel.ShowLastError() ;

/*---------------------------- ������������ ����������� ������������ */

                                           strcpy(format, "1") ;
   for(i=0 ; i<this->mScanPars_cnt ; i++)  strcat(format, ",1") ;

/*-------------------------------------- �������� ������ �� �������� */

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
//".TextFilePlatform = 1251\n"               // �� �������������� MS Office 2000
".TextFileStartRow = 1\n"
".TextFileParseType = xlDelimited\n"
".TextFileTextQualifier = xlTextQualifierDoubleQuote\n"
".TextFileConsecutiveDelimiter = False\n"
".TextFileTabDelimiter = True\n"
".TextFileSemicolonDelimiter = False\n"
".TextFileCommaDelimiter = False\n"
".TextFileSpaceDelimiter = False\n"
".TextFileColumnDataTypes = Array(%s)\n"
//".TextFileTrailingMinusNumbers = False\n"  // �� �������������� MS Office 2000
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

/*-------------------------------------------- ������������ �� Excel */

                Excel.Visible(1) ;
                Excel.Exit() ;

/*------------------------------------------- �������� ������ ������ */

                  unlink(path) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*           ������� ����� ������� � ��������� ���������            */

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
/*		      ������� ����� �������                         */

  int  RSS_Module_Scan::FormMarker(void)

{
  int  status ;
  int  i ;


/*------------------------------------------ ������� ������ �������� */

    for(i=0 ; i<_MARKER_MAX ; i++) {                                /* LOOP - ������� ������ �������� */

#define  M  (this->mBasics_marker[i])

/*-------------------------------- �������������� ����������� ������ */

     if(M.dlist_idx==0)
        M.dlist_idx=RSS_Kernel::display.GetList(1) ;

     if(M.dlist_idx==0)  continue ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* LOOP - ������� ���������� ����������� */

/*--------------------------------------- ������������ ����� ������� */

             glNewList(M.dlist_idx, GL_COMPILE) ;                   /* �������� ������ */
          glMatrixMode(GL_MODELVIEW) ;
//            glEnable(GL_BLEND) ;                                  /* ���.���������� ������ */
//         glDepthMask(0) ;                                         /* ����.������ Z-������ */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  ����� */
    if(!stricmp(M.model, "Point")) {

                       glBegin(GL_POINTS) ;
                    glVertex3d( 0,  0,  0) ;
	                 glEnd();
                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - -  "�����" */
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
//         glDepthMask(1) ;                                         /* ���.������ Z-������ */
//           glDisable(GL_BLEND) ;                                  /* ����.���������� ������ */
             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetNextContext("Show") ;
                     }                                              /* ENDLOOP - ������� ���������� ����������� */
/*------------------------------------------ ������� ������ �������� */

#undef M

                                   }                                /* ENDLOOP - ������� ������ �������� */

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*           ���������������� ������� � ��������� ���������         */

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
/*		      ���������������� �������                      */


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

/*------------------------------------------------------- ���������� */

             form_idx=!M->good ;

               result=M->result_min ;

          R_color=iColorMix(GetRValue(F.color_min), 
                            GetRValue(F.color_max), result) ;
          G_color=iColorMix(GetGValue(F.color_min), 
                            GetGValue(F.color_max), result) ;
          B_color=iColorMix(GetBValue(F.color_min), 
                            GetBValue(F.color_max), result) ;

/*-------------------------------- �������������� ����������� ������ */

     if(M->dlist_idx==0)
           M->dlist_idx=RSS_Kernel::display.GetList(0) ;

     if(M->dlist_idx==0)  return(-1) ;

/*----------------------------------- ������� ���������� ����������� */

          status=RSS_Kernel::display.SetFirstContext("Show") ;
    while(status==0) {                                              /* CIRCLE.1 */

/*------------------ ������������ ��������������� ������������������ */

             glNewList(M->dlist_idx, GL_COMPILE) ;                  /* �������� ������ */

          glMatrixMode(GL_MODELVIEW) ;

          glTranslated(M->point.x, M->point.y, M->point.z) ;

                       zoom=RSS_Kernel::display.GetContextPar("Zoom") ;
                       zoom=zoom/F.size ;
              glScaled(zoom, zoom, zoom) ;

             glColor4d(R_color/256., G_color/256.,                  /* ��������� ����� */ 
                                     B_color/256., 0.2) ;
            glCallList(F.dlist_idx) ;                               /* ��������� ������� */

             glEndList() ;                                          /* �������� ������ */

/*----------------------------------- ������� ���������� ����������� */

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
/**		  ��������������� ���������                        **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*           ��������� ������ �� ����������� ������                 */

   RSS_Kernel *GetTaskScanEntry(void)

{
	return(&ProgramModule) ;
}