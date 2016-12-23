/********************************************************************/
/*								    */
/*      ������ ������ "������ ��������� ����������"                 */
/*								    */
/*                   ���������� ���������                           */
/*                                                                  */
/********************************************************************/

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <math.h>

#include "d:\_Projects\_Libraries\controls.h"
#include "d:\_Projects\_Libraries\UserDlg.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "T_Scan.h"

#pragma warning(disable : 4996)


#define  SEND_ERROR(text)    SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,      \
                                         (LPARAM) text) ;

#include "resource.h"

/*------------------------------------ ����������� ��������� ������� */

  union WndProc_par {
                        long            par ;
                     LRESULT (CALLBACK *call)(HWND, UINT, WPARAM, LPARAM) ; 
                    } ;

  static union WndProc_par  Pars_WndProc ;
  static union WndProc_par  Tmp_WndProc ;

   LRESULT CALLBACK  Task_Scan_Pars_prc(HWND, UINT, WPARAM, LPARAM) ;

/*-------------------------------------------------------- ��������� */

/*---------------------------------------------------- ��������� �/� */

  void  iTScan_Pars_format(ZONE_PAR *, char *) ;   /* �������������� ��������� � ������ */


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� HELP	             */

    BOOL CALLBACK  Task_Scan_Help_dialog(  HWND hDlg,     UINT Msg, 
 		  	                 WPARAM wParam, LPARAM lParam) 
{
  RSS_Module_Scan  Module ;
              int  elm ;         /* ������������� �������� ������� */
              int  status ;
              int  index ;
              int  insert_flag ;
             char *help ;
             char  text[512] ;
             char *end ;
              int  i ;

/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
   for(i=0 ; Module.mInstrList[i].name_full!=NULL ; i++) {

     if(Module.mInstrList[i].help_row==NULL)  continue ;

         index=LB_ADD_ROW (IDC_HELP_LIST, Module.mInstrList[i].help_row) ;
               LB_SET_ITEM(IDC_HELP_LIST, index, Module.mInstrList[i].help_full) ;
                                                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}

/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
       if(elm==IDC_HELP_LIST) {

        if(status==LBN_DBLCLK) {

             index=        LB_GET_ROW (elm) ;
              help=(char *)LB_GET_ITEM(elm) ;
                           LB_GET_TEXT(elm, index, text) ;
           if(text[0]!='#' || 
              help   ==NULL  )  return(FALSE) ;

               status=LB_GET_TEXT(elm, index+1, text) ;
           if( status==LB_ERR ||
              text[0]==  '#'    )  insert_flag=1 ;
           else                    insert_flag=0 ;

         do {
                   memset(text, 0, sizeof(text)) ;
                  strncpy(text, help, sizeof(text)-1) ;
               end=strchr(text, '\n') ;
           if( end!=NULL)  *end=0 ;

           if(insert_flag) {  LB_INS_ROW(elm, index+1, text) ;
                                              index++ ;        }
              help=strchr(help, '\n') ;
           if(help==NULL)  break ;
              help++ ;
              
            } while(1) ;
                               }

			            return(FALSE) ;
                              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/* 	     ���������� ��������� ����������� ���� CONFIG            */

    BOOL CALLBACK  Task_Scan_Config_dialog(  HWND hDlg,     UINT Msg, 
 		  	                   WPARAM wParam, LPARAM lParam) 
{
 static  UD_color_panel  colors[3] ;  /* ������� ������ */
             RSS_Kernel *Module_ ;
 static RSS_Module_Scan *Module ;
               ZONE_PAR  parameter ;
                    int  elm ;         /* ������������� �������� ������� */
          static  HFONT  font ;          /* ����� */
                    int  status ;
                   char  text[1024] ;
                    int  n ;
                    int  i ;


/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
                              Module=(RSS_Module_Scan *)lParam ;
/*- - - - - - - - - - - - - - - - - - - - - - - - -  ������� ������� */
             if(font==NULL)
                font=CreateFont(14, 0, 0, 0, FW_THIN, 
                                 false, false, false,
                                  ANSI_CHARSET,
                                   OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS,
                                     DEFAULT_QUALITY,
                                      VARIABLE_PITCH,
                                       "Courier New Cyr") ;
         SendMessage(ITEM(IDC_PARS_LIST), WM_SETFONT, (WPARAM)font, 0) ;
/*- - - - - - - - - - - - - - - - -  �������� ������������ ��������� */
           Pars_WndProc.par=GetWindowLong(ITEM(IDC_PARS_LIST), GWL_WNDPROC) ;
           Tmp_WndProc.call=Task_Scan_Pars_prc ;
        SetWindowLong(ITEM(IDC_PARS_LIST), GWL_WNDPROC, Tmp_WndProc.par) ;
/*- - - - - - - - - - - - - - - - - - - -  ������ �������� ��������� */
             CB_CLEAR(IDC_GOOD_TYPE) ;
          CB_ADD_LIST(IDC_GOOD_TYPE, "Point") ;
          CB_ADD_LIST(IDC_GOOD_TYPE, "Spot") ;
             CB_CLEAR(IDC_BAD_TYPE) ;
          CB_ADD_LIST(IDC_BAD_TYPE, "Point") ;
          CB_ADD_LIST(IDC_BAD_TYPE, "Spot") ;
             CB_CLEAR(IDC_GOOD_PRIORITY) ;
          CB_ADD_LIST(IDC_GOOD_PRIORITY, "MIN") ;
          CB_ADD_LIST(IDC_GOOD_PRIORITY, "MAX") ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
                colors[0].color      = Module->mBasics_marker[0].color_min ;
                colors[0].choice_proc=Task_Scan_color_choice ;
                colors[1].color      = Module->mBasics_marker[0].color_max ;
                colors[1].choice_proc=Task_Scan_color_choice ;
                colors[2].color      = Module->mBasics_marker[1].color_min ;
                colors[2].choice_proc=Task_Scan_color_choice ;

        SendMessage(ITEM(IDC_GOOD_COLOR_MIN), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[0])) ;
        SendMessage(ITEM(IDC_GOOD_COLOR_MAX), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[1])) ;
        SendMessage(ITEM(IDC_BAD_COLOR), WM_SETTEXT, NULL, 
                          (LPARAM)UD_ptr_incode((void *)&colors[2])) ;

                 SETs(IDC_SERVICE_TASK, Module->mCheck_cmd) ;
                 SETx(IDC_SHOW_BASE,    Module->mBasePoints_show) ;
                 SETx(IDC_REDRAW_BASE,  Module->mBasePoints_redraw) ;
                 SETc(IDC_GOOD_TYPE,    Module->mBasics_marker[0].model) ;
                 SETc(IDC_BAD_TYPE,     Module->mBasics_marker[1].model) ;
                 SETi(IDC_SIZE_FACTOR,  Module->mBasics_marker[0].size) ;

                         LB_CLEAR(IDC_PARS_LIST) ;
        for(i=0 ; i<Module->mScanPars_cnt ; i++) {                  /* ������ ���������� ������������ */
               iTScan_Pars_format(&Module->mScanPars[i], text) ;
                       LB_ADD_ROW( IDC_PARS_LIST, text) ;
                                                 }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;

/*- - - - - - - - - - - - - - - - - - - - - �������������� ��������� */
       if(   elm==IDC_PARS_LIST  &&
          status==LBN_DBLCLK       ) {

                 n=LB_GET_ROW(IDC_PARS_LIST) ;                      /* ���������� ����� ������ */

                     memset(&parameter, 0, sizeof(parameter)) ;     /* ������� � �������� � �������������� */
                             parameter       =Module->mScanPars[n] ;
                             parameter.module=Module ;

                             Module_=GetTaskScanEntry() ;

           status=DialogBoxIndirectParam(GetModuleHandle(NULL),     /* �������� ������ �������������� */ 
                                         (LPCDLGTEMPLATE)Module_->Resource("IDD_PARAMETER", RT_DIALOG),
			                   GetActiveWindow(), Task_Scan_Param_dialog, 
                                           (LPARAM)&parameter) ;
         if(status!=0)  return(FALSE) ;                             /* ���� �������� �������� */

                                   Module->mScanPars[n]=parameter ; /* ������� ����� ������ */ 
               iTScan_Pars_format(&Module->mScanPars[n], text) ;    /* ��������� �������� */ 
                       LB_DEL_ROW( IDC_PARS_LIST, n) ;              /* �������� �������� � ������ */
                       LB_INS_ROW( IDC_PARS_LIST, n, text) ;

                                            return(FALSE) ;
                                     }
/*- - - - - - - - - - - - - - - - - - - - - - - -  �������� �������� */
       if(elm==IDC_PAR_ADD) {

         if(Module->mScanPars_cnt==_SCAN_PARS_MAX) {
                     SEND_ERROR("������� ������� ����� ����������") ;
                                   return(FALSE) ;
                                                   }

                     memset(&parameter, 0, sizeof(parameter)) ;
                             parameter.module=Module ;

                             Module_=GetTaskScanEntry() ;

           status=DialogBoxIndirectParam(GetModuleHandle(NULL),
                                         (LPCDLGTEMPLATE)Module_->Resource("IDD_PARAMETER", RT_DIALOG),
			                   GetActiveWindow(), Task_Scan_Param_dialog, 
                                           (LPARAM)&parameter) ;
         if(status!=0)  return(FALSE) ;

                  Module->mScanPars[Module->mScanPars_cnt]=parameter ;
                                    Module->mScanPars_cnt++ ;

               iTScan_Pars_format(&parameter, text) ;
                       LB_ADD_ROW( IDC_PARS_LIST, text) ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������� �������� */
       if(elm==IDC_PAR_DEL) {

                 n=LB_GET_ROW(IDC_PARS_LIST) ;                      /* ���������� ����� ������ */
                   LB_DEL_ROW(IDC_PARS_LIST, n) ;                   /* ������� �������� �� ��������� ������ */

          for(i=n+1 ; i<Module->mScanPars_cnt ; i++)                /* �������� ������ */
                  Module->mScanPars[i-1]=Module->mScanPars[i] ;

                  Module->mScanPars_cnt-- ;

			            return(FALSE) ;
                            }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {

                                 GETs(IDC_SERVICE_TASK,  Module->mCheck_cmd) ;
      Module->mBasePoints_show  =GETx(IDC_SHOW_BASE) ;
      Module->mBasePoints_redraw=GETx(IDC_REDRAW_BASE) ;


                                 GETc(IDC_GOOD_TYPE, Module->mBasics_marker[0].model) ;
                                 GETc(IDC_BAD_TYPE,  Module->mBasics_marker[1].model) ;

       Module->mBasics_marker[0].color_min=colors[0].color ;
       Module->mBasics_marker[0].color_max=colors[1].color ;
       Module->mBasics_marker[1].color_min=colors[2].color ;
       Module->mBasics_marker[1].color_max=colors[2].color ;

         Module->mBasics_marker[0].size=GETi(IDC_SIZE_FACTOR) ;
      if(Module->mBasics_marker[0].size<=0)
         Module->mBasics_marker[0].size=50 ;

         Module->mBasics_marker[1].size=Module->mBasics_marker[0].size ;

                            EndDialog(hDlg, 0) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*          ���������� ��������� ����������� ���� PARAMETER          */

    BOOL CALLBACK  Task_Scan_Param_dialog(  HWND hDlg,     UINT Msg, 
 		  	                  WPARAM wParam, LPARAM lParam) 
{
 static RSS_Module_Scan *Module ;
        static ZONE_PAR *parameter ;
  static RSS_ControlPar *ctrl_pars ;          /* ������ ����������� ���������� �������� */
  static            int  ctrl_pars_cnt ;
  static           char  ctrl_object[128] ;   /* ������ �� ����������� ������� */
  static           char  ctrl_link[64] ;      /* ������ �� ����������� �������� ������� */
                   char *list[1024] ;         /* ������ �������� */
                    int  list_cnt ;
                    int  elm ;                /* ������������� �������� ������� */
                    int  status ;
                   char  value[1024] ;
                   char  group[1024] ;
                   char *error ;
                    int  i ;
                    int  j ;

    static  char *category_list[2]={ "����������",
                                     "�������� �������"       } ;
    static  char *    kind_list[2]={ "����������-����������",
                                     "����������"             } ;

#define  P parameter
#define  M Module

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

#define   RAISE_EVENT(elm, status)  SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(elm, status), 0)

/*------------------------------------------------------- ���������� */

      if(ctrl_pars==NULL) {

         ctrl_pars=(RSS_ControlPar *)
                        calloc(1000, sizeof(RSS_ControlPar)) ;
                          }
/*------------------------------------------------- ������� �������� */

  switch(Msg) {

/*---------------------------------------------------- ������������� */

    case WM_INITDIALOG: {
                           parameter=(ZONE_PAR *)lParam ;
                              Module= parameter->module ;
/*- - - - - - - - - - - - - - - - - �������������  ������� ��������� */
                              CB_CLEAR(IDC_CATEGORY) ;
     for(i=0 ; i<2 ; i++)  CB_ADD_LIST(IDC_CATEGORY, category_list[i]) ;

                              CB_CLEAR(IDC_KIND) ;
     for(i=0 ; i<2 ; i++)  CB_ADD_LIST(IDC_KIND, kind_list[i]) ;

                              CB_CLEAR(IDC_OBJECT) ;
     for(i=0 ; i<OBJECTS_CNT ; i++)
       if(OBJECTS[i]->vListControlPars(NULL))
                           CB_ADD_LIST(IDC_OBJECT, OBJECTS[i]->Name) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������������� ��������� */
              SETs(IDC_NAME,                   P->name     ) ;
              SETc(IDC_CATEGORY, category_list[P->category]) ;
              SETc(IDC_KIND,         kind_list[P->kind    ]) ;

          if(P->category==_OBJECT_PAR) {                            /* ����������� ������ � ����������� �������� */
                        SETc(IDC_OBJECT, P->object) ;
                 RAISE_EVENT(IDC_OBJECT, CBN_SELCHANGE) ;
               
            for(i=0 ; i<ctrl_pars_cnt ; i++)                        
              if(!stricmp(P->par_link, ctrl_pars[i].link)) {
                        SETc(IDC_TYPE, ctrl_pars[i].section_name) ;
                 RAISE_EVENT(IDC_TYPE, CBN_SELCHANGE) ;
                        SETc(IDC_ELEMENT, ctrl_pars[i].parameter_name) ;
                 RAISE_EVENT(IDC_ELEMENT, CBN_SELCHANGE) ;
                                           break ;
                                                           }
                                       }

              SETs(IDC_VALUE,                  P->value.formula) ;
              SETs(IDC_VALUE_FROM,             P->value_min.formula) ;
              SETs(IDC_VALUE_TO,               P->value_max.formula) ;
              SETs(IDC_VALUE_STEP,             P->value_step.formula) ;
/*- - - - - - - - - - - - - - - - - - - - - - ����������� ���������� */
          if(P->name[0]!=0)  DISABLE(IDC_NAME) ;                    /* ���� ��� �� ������ - ��������� ��� */

          if(P->category== _VAR_PAR) {  DISABLE(IDC_OBJECT) ;       /* ��� ���������� - ��������� ���� */ 
                                        DISABLE(IDC_TYPE) ;         /*  �������� �������               */
                                        DISABLE(IDC_ELEMENT) ;  }

          if(P->kind    ==_SCAN_PAR) {  DISABLE(IDC_VALUE) ;      } /* ��� ����������� - ��������� ����    */
          else                       {  DISABLE(IDC_VALUE_FROM) ;   /*  ������� ��������, ��� ���������� - */
                                        DISABLE(IDC_VALUE_TO) ;     /*    "�����������" ����               */
                                        DISABLE(IDC_VALUE_STEP) ; }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  			  return(FALSE) ;
  			     break ;
  			}
/*------------------------------------------------ ��������� ������� */

    case WM_COMMAND:    {

	status=HIWORD(wParam) ;
	   elm=LOWORD(wParam) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��������� */
     if(elm==IDC_CATEGORY && status==CBN_SELCHANGE) {
                                GETc(IDC_CATEGORY, value) ;

        if(!stricmp(category_list[0], value)) {                     /* ��� ���������� - ��������� ���� */ 
                       DISABLE(IDC_OBJECT) ;                        /*  �������� �������               */
                       DISABLE(IDC_TYPE) ;
                       DISABLE(IDC_ELEMENT) ;
                                              }
        else                                  { 
                        ENABLE(IDC_OBJECT) ;
                        ENABLE(IDC_TYPE) ;
                        ENABLE(IDC_ELEMENT) ;
                                              }

                               return(FALSE) ;
                                                    }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ��������� ���� */
     if(elm==IDC_KIND && status==CBN_SELCHANGE) {

                              GETc(IDC_KIND, value) ;

          if(!stricmp(kind_list[0], value)) {                       /* ��� ����������� - ��������� ����    */
                  DISABLE(IDC_VALUE) ;                              /*  ������� ��������, ��� ���������� - */
                   ENABLE(IDC_VALUE_FROM) ;                         /*    "�����������" ����               */
                   ENABLE(IDC_VALUE_TO) ;     
                   ENABLE(IDC_VALUE_STEP) ;  
                                            }
          else                              {
                   ENABLE(IDC_VALUE) ;
                  DISABLE(IDC_VALUE_FROM) ;
                  DISABLE(IDC_VALUE_TO) ;
                  DISABLE(IDC_VALUE_STEP) ;
                                            }

                            return(FALSE) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - -  ����� ������� */
     if(elm==IDC_OBJECT && status==CBN_SELCHANGE) {

                                  *ctrl_link=0 ;                    /* ����� ������� ������ */

                                GETc(IDC_OBJECT, value) ;

                            CB_CLEAR(IDC_TYPE) ;
                            CB_CLEAR(IDC_ELEMENT) ;

        for(i=0 ; i<OBJECTS_CNT ; i++)
          if(!stricmp(value, OBJECTS[i]->Name))  break ;

          if(i>=OBJECTS_CNT) {
                                SETc(IDC_OBJECT, "???") ;
                          SEND_ERROR("�������������� ������") ;
  			       return(FALSE) ;
                             }

             ctrl_pars_cnt=OBJECTS[i]->vListControlPars(ctrl_pars); /* ����������� ������ ���������� ���������� */
          if(ctrl_pars_cnt<=0) {
                      SETc(IDC_OBJECT, "???") ;
                SEND_ERROR("������ �� ����� ����������� ����������") ;
  			            return(FALSE) ;
                               }

              for(list_cnt=0, i=0 ; i<ctrl_pars_cnt ; i++) {        /* ��������� ������ ������ */
                for(j=0 ; j<list_cnt ; j++)
                  if(!stricmp(list[j], ctrl_pars[i].section_name)) break ;

                  if(j>=list_cnt) {  list[list_cnt]=ctrl_pars[i].section_name ;
                                          list_cnt++ ;                          }
                                                           } 

               for(j=0 ; j<list_cnt ; j++)                          /* ������ ������ ������ � ������ �������� */
                             CB_ADD_LIST(IDC_TYPE, list[j]) ;

                              return(FALSE) ;
                                                  }
/*- - - - - - - - - - - - - - - - - - - - - - -  ����� ���� �������� */
     if(elm==IDC_TYPE && status==CBN_SELCHANGE) {

                                  *ctrl_link=0 ;                    /* ����� ������� ������ */

                                GETc(IDC_TYPE, value) ;
                            CB_CLEAR(IDC_ELEMENT) ;

            for(list_cnt=0, i=0 ; i<ctrl_pars_cnt ; i++)            /* ��������� ������ ��������� */
              if(!stricmp(value, ctrl_pars[i].section_name)) {  
                for(j=0 ; j<list_cnt ; j++)
                  if(!stricmp(list[j], ctrl_pars[i].parameter_name)) break ;

                  if(j>=list_cnt) {  list[list_cnt]=ctrl_pars[i].parameter_name ;
                                          list_cnt++ ;                            }
                                                             } 

               for(j=0 ; j<list_cnt ; j++)                          /* ������ ������ ��������� � ������ �������� */
                             CB_ADD_LIST(IDC_ELEMENT, list[j]) ;

                              return(FALSE) ;
                                                }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ����� �������� */
     if(elm==IDC_ELEMENT && status==CBN_SELCHANGE) {

                                GETc(IDC_OBJECT,  ctrl_object) ;
                                GETc(IDC_TYPE,    group) ;
                                GETc(IDC_ELEMENT, value) ;

            for(i=0 ; i<ctrl_pars_cnt ; i++)                        /* �������������� ������� */
              if(!stricmp(group, ctrl_pars[i].section_name  ) &&  
                 !stricmp(value, ctrl_pars[i].parameter_name)   ) {
                        strcpy(ctrl_link, ctrl_pars[i].link)  ;
                                               break ;
                                                                  } 

                              return(FALSE) ;
                                                   }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ������� ������ */
     if(elm==IDC_SET) {

     if(P->name[0]==0) {                                            /* ���������� � �������� ����� */
                            GETs(IDC_NAME, P->name) ;

         if(P->name[0]==0) {
               SEND_ERROR("��� ��������� �� ����� ���� ������") ;
  			       return(FALSE) ;
                           }

       if(Module!=NULL) {                                           /* ��������� ����������� ����� */

           for(i=0 ; i<M->mScanPars_cnt ; i++) 
             if(!stricmp(M->mScanPars[i].name, P->name))  break ;

         if(i<M->mScanPars_cnt) {
                                    P->name[0]=0 ;
               SEND_ERROR("����� ��� ��������� ��� ������������") ;
  			          return(FALSE) ;
                                }
                        }
                       }

              GETc(IDC_CATEGORY, value) ;                           /* ���������� ��������� */

             for(i=0 ; i<2 ; i++)
               if(!stricmp(category_list[i], value))  P->category=i ;

         if(P->category==_OBJECT_PAR) {                             /* ��������� ������ �� ����������� �������� ������� */
          if(ctrl_link[0]==0) {  SEND_ERROR("�� ����� �������� �������") ;
  			                         return(FALSE) ;           }

               memset(P->object,   0, sizeof(P->object)) ;
               memset(P->par_link, 0, sizeof(P->par_link)) ;
              strncpy(P->object,   ctrl_object, sizeof(P->object)-1) ;
              strncpy(P->par_link, ctrl_link,   sizeof(P->par_link)-1) ;
                                      }

              GETc(IDC_KIND, value) ;                               /* ���������� ���� */

             for(i=0 ; i<2 ; i++)
               if(!stricmp(kind_list[i], value))  P->kind=i ;

              GETs(IDC_VALUE,      P->value.formula) ;              /* ���������� � �������� �������� */
              GETs(IDC_VALUE_FROM, P->value_min.formula) ;
              GETs(IDC_VALUE_TO,   P->value_max.formula) ;
              GETs(IDC_VALUE_STEP, P->value_step.formula) ;

                                           error= NULL ;

         if(P->kind==_SCAN_PAR) {
          if(P->value_min.formula[0] ==0)  error="�� ������ ������� �������� ������������ ��������" ;
          if(P->value_max.formula[0] ==0)  error="�� ������ ������� �������� ������������� ��������" ;
          if(P->value_step.formula[0]==0)  error="�� ������ ������� �������� ���� ��������" ;
                                }
         else                   {
          if(P->value.formula[0]==0)  error="�� ������ ������� �������� ��������" ;
                                }

          if(error!=NULL) {    SEND_ERROR(error) ;
  			          return(FALSE) ;   }


                            EndDialog(hDlg, 0) ;                    /* ���������� ������� */

                            return(FALSE) ;
                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
			  return(FALSE) ;
			     break ;
			}
/*--------------------------------------------------------- �������� */

    case WM_CLOSE:      {
                            EndDialog(hDlg, -1) ;
  			       return(FALSE) ;
			              break ;
			}
/*----------------------------------------------------------- ������ */

    default :        {
			  return(FALSE) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  P
#undef  M

    return(TRUE) ;
}


/*********************************************************************/
/*								     */
/*	          ����e ����������� ���������                        */    

  LRESULT CALLBACK  Task_Scan_Pars_prc(  HWND  hWnd,     UINT  Msg,
	                               WPARAM  wParam, LPARAM  lParam) 
{
   HMENU  hMenu ;
 LRESULT  result ;
     int  menu_flag ;
    RECT  Rect ;
     int  x ;
     int  y ;

/*----------------------------------- ������� ������ ������ �� ����� */

                              menu_flag= 0 ;

   if(Msg==WM_RBUTTONDOWN)          Msg=WM_LBUTTONDOWN ;
   if(Msg==WM_RBUTTONUP  ) {        Msg=WM_LBUTTONUP ;
                              menu_flag= 1 ;            }
                          
/*-------------------------------- ����� ������� ��������� ��������� */

      result=CallWindowProc(Pars_WndProc.call,
                                hWnd, Msg, wParam, lParam) ;

/*------------------------------------- ��������� ���� ������ ������ */

   if(menu_flag) {
                    GetWindowRect(hWnd, &Rect) ;

                         x=LOWORD(lParam) ; 
                         y=HIWORD(lParam) ; 

                     hMenu=CreatePopupMenu() ;
          AppendMenu(hMenu, MF_STRING, IDC_PAR_ADD, "�������� ��������") ;
          AppendMenu(hMenu, MF_STRING, IDC_PAR_DEL, "������� ��������") ;

      TrackPopupMenu(hMenu, 0, Rect.left+x, Rect.top+y+4,
                                0, GetParent(hWnd), NULL) ;

         DestroyMenu(hMenu) ;
                 }
/*-------------------------------------------------------------------*/

  return(result) ;
}


/*********************************************************************/
/*								     */
/*	          �������������� ��������� � ������                  */

  void  iTScan_Pars_format(ZONE_PAR *par, char *text)

{
            char  link[128] ;
            char  value[1024] ;
    static  char *category_list[2]={ "VAR", "OBJ" } ;
    static  char *    kind_list[2]={ "FRE", "SLV" } ;

/*------------------------------------------------ ����������� ����� */

                 memset(link, 0, sizeof(link)) ;

   if(par->category==_VAR_PAR) {
                               }
/*---------------------------------------- ������������ ����� ������ */

                 memset(value, 0, sizeof(value)) ;

   if(par->kind==_SCAN_PAR) {
           sprintf(value, "%s...%s : %s", par->value_min.formula, 
                                          par->value_max.formula, 
                                          par->value_step.formula) ;
                            }
   else                     {
           sprintf(value, "=%s", par->value.formula) ;
                            }
/*---------------------------------------- ������������ ����� ������ */

  sprintf(text, "%10.10s %3.3s %3.3s  %s  %s", 
                                   par->name, 
                     category_list[par->category],
                         kind_list[par->kind    ],
                                   link,
                                   value           ) ;

/*-------------------------------------------------------------------*/
}
