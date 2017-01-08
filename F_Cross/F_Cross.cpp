/*********************************************************************/
/*								     */
/*	������ ���������� ������������ ����������� ��������          */
/*								     */
/*********************************************************************/


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

#include "d:\_Projects\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "F_Cross.h"

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


/*********************************************************************/
/*								     */
/*		    	����������� ������                           */

     static   RSS_Module_Cross  ProgramModule ;


/*********************************************************************/
/*								     */
/*		    	����������������� ����                       */

 F_CROSS_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 F_CROSS_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/*********************************************************************/
/*********************************************************************/
/**							            **/
/**               �������� ������ ������ ����������                 **/
/**	           ������������ ����������� ��������	            **/
/**							            **/
/*********************************************************************/
/*********************************************************************/

/*********************************************************************/
/*								     */
/*                            ������ ������                          */

  struct RSS_Module_Cross_instr  RSS_Module_Cross_InstrList[]={

 { "help",    "?",  "#HELP   - ������ ��������� ������", 
                     NULL,
                    &RSS_Module_Cross::cHelp       },
 { "assign",  "a",  "#ASSIGN (A) - ������� ���������� �����������", 
                     NULL,
                    &RSS_Module_Cross::cAssign     },
 { "exclude", "ex", "#EXCLUDE (EX) - ���������� ��� �� �������� �����������", 
                    " EXCLUDE <������������_1> <������������_2>\n"
                    "    ��� <������������_i> ��������� ���� ��� ����� ���\n"
                    "     � �������: <������>[.<�����>[.<����>]] \n",
                    &RSS_Module_Cross::cExclude     },
 {  NULL }
                                                              } ;

/*********************************************************************/
/*								     */
/*		     ����� ����� ������             		     */

    struct RSS_Module_Cross_instr *RSS_Module_Cross::mInstrList=NULL ;

/*********************************************************************/
/*								     */
/*		       ����������� ������			     */

     RSS_Module_Cross::RSS_Module_Cross(void)

{
	   keyword="Geo" ;
    identification="Cross_feature" ;

        mInstrList=RSS_Module_Cross_InstrList ;
}


/*********************************************************************/
/*								     */
/*		        ���������� ������			     */

    RSS_Module_Cross::~RSS_Module_Cross(void)

{
}


/*********************************************************************/
/* 								     */
/*		        ��������� ��������                           */

    void  RSS_Module_Cross::vStart(void)

{
/*-------------------------------------------- ������������ �������� */

   feature_modules=(RSS_Kernel **)
                     realloc(feature_modules, 
                              (feature_modules_cnt+1)*sizeof(feature_modules[0])) ;

      feature_modules[feature_modules_cnt]=&ProgramModule ;
                      feature_modules_cnt++ ;

/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*								     */
/*		        ������� ��������                	     */

    RSS_Feature *RSS_Module_Cross::vCreateFeature(RSS_Object *object)

{
  RSS_Feature *feature ;

         feature        =new RSS_Feature_Cross ;
         feature->Object=object ;

  return(feature) ;
}


/*********************************************************************/
/*								     */
/*		        ��������� �������       		     */

  int  RSS_Module_Cross::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "CROSS"
#define  _SECTION_SHRT_NAME   "CR"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Cross features:") ;
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
         end=strchr(instr, ' ') ;
      if(end!=NULL) {  *end=0 ;  end++ ;  }
      else              end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ CROSS: ����������� �������") ;
                                            return(0) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_Cross::vWriteSave(std::string *text)

{
  RSS_Feature_Cross_Mark *pair ;
                    char  row[1024] ;
                     int  i ;

#define  MARKS_LIST       RSS_Feature_Cross::marks_list
#define  MARKS_LIST_CNT   RSS_Feature_Cross::marks_list_cnt

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE CROSS\n" ;

/*--------------------------- ������ ���������� �������� ����������� */

   for(i=0 ; i<MARKS_LIST_CNT ; i++)  MARKS_LIST[i]->work=0 ;

   for(i=0 ; i<MARKS_LIST_CNT ; i++) {

              if(MARKS_LIST[i]->work)  continue ;

           pair=(RSS_Feature_Cross_Mark *)MARKS_LIST[i]->pair ;

       sprintf(row, "CROSS_MARK %s.%s.%s %s.%s.%s %d %d\n",
                           MARKS_LIST[i]->object,
                           MARKS_LIST[i]->link,
                           MARKS_LIST[i]->body,
                                 pair   ->object,
                                 pair   ->link,
                                 pair   ->body,
                           MARKS_LIST[i]->execute,
                           MARKS_LIST[i]->active   ) ;

                               *text+=row ;
       
                           MARKS_LIST[i]->work = 1 ;
                                 pair   ->work = 1 ;
                                    }
/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/

#undef   MARKS_LIST
#undef   MARKS_LIST_CNT

}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_Cross::vReadSave(std::string *data)

{
  RSS_Feature_Cross_Mark  mark_1 ;
  RSS_Feature_Cross_Mark  mark_2 ;
  RSS_Feature_Cross_Mark *mark  ;
                    char *buff ;
                     int  buff_size ;
             std::string  decl ;
                    char *work ;
                    char  text[1024] ;
                    char *beg ;
                    char *end ;
                    char *tmp ;

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE CROSS\n", 
                      strlen("#BEGIN MODULE CROSS\n")))  return ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

                work=buff+strlen("#BEGIN MODULE CROSS\n") ;

    for( ; ; work=end+1) {                                          /* CIRCLE.0 */
                                      end=strchr(work, '\n') ;
                                   if(end==NULL)  break ;
                                     *end=0 ;

/*------------------- �������� ���� ���������� ��������� ����������� */

          if(!memicmp(work, "CROSS_MARK", strlen("CROSS_MARK"))) {

                    memset(&mark_1, 0, sizeof(mark_1)) ;
                    memset(&mark_2, 0, sizeof(mark_2)) ;
        
                           beg=work+strlen("CROSS_MARK")+1 ;
                           tmp=strchr(beg, '.') ;
                          *tmp= 0 ;
                    strcpy(mark_1.object, beg) ;
                           beg=tmp+1 ;
                           tmp=strchr(beg, '.') ;
                          *tmp= 0 ;
                    strcpy(mark_1.link, beg) ;
                           beg=tmp+1 ;
                           tmp=strchr(beg, ' ') ;
                          *tmp= 0 ;
                    strcpy(mark_1.body, beg) ;
                           beg=tmp+1 ;
                           tmp=strchr(beg, '.') ;
                          *tmp= 0 ;
                    strcpy(mark_2.object, beg) ;
                           beg=tmp+1 ;
                           tmp=strchr(beg, '.') ;
                          *tmp= 0 ;
                    strcpy(mark_2.link, beg) ;
                           beg=tmp+1 ;
                           tmp=strchr(beg, ' ') ;
                          *tmp= 0 ;
                    strcpy(mark_2.body, beg) ;

                           mark_1.execute=tmp[1]-'0' ;
                           mark_2.execute=tmp[1]-'0' ;
                           mark_1.active =tmp[3]-'0' ;
                           mark_2.active =tmp[3]-'0' ;

               mark=RSS_Feature_Cross::iSetMark(&mark_2, &mark_1) ; /* ������������ ���� */
            if(mark==NULL) {
                 SEND_ERROR("�� ������� ���������������� �����") ;
                                        return ;
                           }
                                                                 }
/*---------------------------------------------------------- ??????? */

     else if(!memicmp(work, "??????", strlen("??????"))) {

                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Module CROSS: "
                              "����������� ������������ %20.20s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- ���������� �������� */
                         }                                          /* CONTINUE.0 */
/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� Help                    */

  int  RSS_Module_Cross::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Feature_Cross_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Assign                   */
/*     ASSIGN  <���>                                                */

  int  RSS_Module_Cross::cAssign(char *cmd)

{ 
#define   _PARS_MAX   4
  RSS_Object *object ;
        char *pars[_PARS_MAX] ;
        char *name ;
        char *end ;
        char  text[1024] ;
         int  status ;
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

                                     name=pars[0] ;

/*------------------------------------------- ����� ������� �� ����� */ 

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� �������. \n"
                                 "��������: ASSIGN <���_�������> ...") ;
                                     return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name)) {
                                         object=OBJECTS[i] ;
                                                 break ;
                                              }

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*----------------------------------------------- ���������� ������� */

      status=DialogBoxIndirectParam( GetModuleHandle(NULL),
                                    (LPCDLGTEMPLATE)Resource("IDD_CROSS_ASSIGN", RT_DIALOG),
			             GetActiveWindow(),
                                     Feature_Cross_Assign_dialog,
                                    (LPARAM)object               ) ;

/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/*								    */
/*                   ���������� ���������� Exclude                  */
/*								    */
/*     EXCLUDE <������������_1> <������������_2>                    */
/*          ��� <������������_i> - <������>[.<�����>[.<����>]]      */

  int  RSS_Module_Cross::cExclude(char *cmd)

{ 
#define   _PARS_MAX   4
                    char *pars[_PARS_MAX] ;
              RSS_Object *object ;
  RSS_Feature_Cross_Mark  spec[2] ;
  RSS_Feature_Cross_Mark *mark_1 ;
                    char *next ;
                    char *end ;
                    char  text[1024] ;
                     int  n ;
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
/*-------------------------------------- �������� ������� ������������ */

    if(*pars[0]== 0) {                                                /* ���� ��� �� ������... */
          SEND_ERROR("������ ���� ������ ������ ���� ������������\n"
                     "��������: EXCLUDE robot.link1") ;
                                     return(-1) ;
                     }

    if(pars[1]==NULL)  pars[1]="*" ;

/*---------------------------------------------- ������ ������������ */

     for(n=0 ; n<2 ; n++) {
/*- - - - - - - - - - - - - - - -  ������ ������������ �� ���������� */
       do {                                                         /* BLOCK.1 */
               end=strchr(pars[n], '.') ;
            if(end!=NULL)  *end=0 ;

                             strcpy(spec[n].object, pars[n]) ;

            if(end==NULL) {  strcpy(spec[n].link, "*") ;
                             strcpy(spec[n].body, "*") ;
                                        break ;            }

              next=end+1 ;
               end=strchr(next, '.') ;
            if(end!=NULL)  *end=0 ;

                             strcpy(spec[n].link, next) ;

            if(end==NULL) {  strcpy(spec[n].body, "*") ;
                                        break ;            }

                             strcpy(spec[n].body, end+1) ;
          } while(0) ;                                              /* BLOCK.1 */
/*- - - - - - - - - - - - - - - - - -  �������� ������������ ������� */
         if(spec[n].object[0]=='*')  continue ;

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, spec[n].object)) {
                                  object=OBJECTS[i] ;
                                             break ;
                                                        }

         if(i==OBJECTS_CNT) {                                       /* ���� ��� �� �������... */
                  sprintf(text, "������� � ������ '%s' "
                                 "�� ����������", spec[n].object) ;
               SEND_ERROR(text) ;
                               return(-1) ;
                            }
/*- - - - - - - - - - - - - - - - - - -  �������� ������������ ����� */
         if(spec[n].link[0]=='*')  continue ;

       for(i=0 ; i<object->Morphology_cnt ; i++)
         if(!stricmp(object->Morphology[i].link, spec[n].link))  break ;

         if(i==object->Morphology_cnt) {                            /* ���� ��� �� �������... */
                  sprintf(text, "������� '%s' �� ����� "
                                "����� � ������ '%s'", 
                                   spec[n].object, spec[n].link) ;
               SEND_ERROR(text) ;
                      return(-1) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - �������� ������������ ���� */
       if(spec[n].body[0]=='*')  continue ;

       for(i=0 ; i<object->Morphology_cnt ; i++)
         if(!stricmp(object->Morphology[i].link, spec[n].link) &&
            !stricmp(object->Morphology[i].body, spec[n].body)   )  break ;

         if(i==object->Morphology_cnt) {                            /* ���� ��� �� �������... */
                  sprintf(text, "����� '%s' ������� '%s' �� ����� "
                                "���� � ������ '%s'", 
                                 spec[n].link, spec[n].object, spec[n].body) ;
               SEND_ERROR(text) ;
                      return(-1) ;
                                       }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          }
/*------------------------------------------------- ����������� ���� */

          mark_1=RSS_Feature_Cross::iSetMark(&spec[0], &spec[1]) ;  /* ������������ ���� */
       if(mark_1==NULL) {
                 SEND_ERROR("�� ������� ���������������� �����") ;
                                        return(FALSE) ;
                        }
/*-------------------------------------------------------------------*/

#undef    OBJECTS
#undef    OBJECTS_CNT

   return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**           �������� ������ �������� �������                     **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ����������			    */

 F_CROSS_API  RSS_Feature_Cross_Mark **RSS_Feature_Cross::marks_list ; 
 F_CROSS_API                     int   RSS_Feature_Cross::marks_list_cnt ; 


/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Feature_Cross::RSS_Feature_Cross(void)

{
   strcpy(Type, "Cross") ;

     Bodies    =NULL ;
     Bodies_cnt=  0 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Feature_Cross::~RSS_Feature_Cross(void)

{
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Feature_Cross::vReadSave(       char *title,  
                                       std::string *data, 
                                              char *reference)

{
  RSS_Kernel *Calc ;
         int  body_num ;
        char *buff ;
         int  buff_size ;
        char *work ;
        char *decl ;
        char *end ;
        char *sep ;
        char *coord ;
         int  num ;
        char  error[1024] ;
        char  text[1024] ;
         int  status ;
         int  i ;

#define    VERTEXES      Bodies[body_num].Vertexes
#define    VERTEXES_CNT  Bodies[body_num].Vertexes_cnt
#define    FACETS        Bodies[body_num].Facets
#define    FACETS_CNT    Bodies[body_num].Facets_cnt


/*----------------------------------------------- �������� ��������� */

       if(stricmp(title, "FEATURE CROSS"))  return ;

/*---------------------------------- �������� ���������� ������ ���� */

                    vBodyDelete(reference) ;

/*------------------------------------------------ ���������� ������ */

              buff_size=data->size()+16 ;
              buff     =(char *)calloc(1, buff_size) ;

       strcpy(buff, data->c_str()) ;
        
/*---------------------------------------------- ���������� �������� */

//                     Bodies_cnt= 0 ;
                         body_num=Bodies_cnt-1 ;

    for(work=buff ; ; work=end+1) {                                 /* CIRCLE.0 */

           end=strchr(work, '\n') ;
        if(end==NULL)  break ;
          *end=0 ;

/*------------------------------------------------------------- ���� */

          if(!memicmp(work, "$BODY", strlen("$BODY"))) {

                    work+=strlen("$BODY") ;

                            body_num++ ;
                          Bodies_cnt++ ;

               Bodies=(RSS_Feature_Cross_Body *)
                        realloc(Bodies, Bodies_cnt*sizeof(Bodies[0])) ;
            if(Bodies==NULL) {
                  sprintf(text, "Section FEATURE CROSS: "
                                "������������ ������ ��� ������ ���\n") ;
               SEND_ERROR(text) ;
                                        break ;
                             }

                 memset(&Bodies[body_num], 0, sizeof(Bodies[0])) ;

                    for( ; *work==' ' || *work=='\t' ; work++) ;

            if(reference!=NULL)                                         /* ������� ��� ���� */ 
                     strcpy(Bodies[body_num].name, reference) ;
            else     strcpy(Bodies[body_num].name, work) ;

                            Bodies[body_num].extrn_pars=Pars_work ;
                            Bodies[body_num].recalc    = 1 ;

                                                       }
/*---------------------------------------------------------- ������� */

     else if(!memicmp(work, "$VERTEX", strlen("$VERTEX"))) {

           if(body_num==-1)  continue ;
/*- - - - - - - - - - - - - - - - - - - - - ��������� ������ ������� */
               decl=work+strlen("$VERTEX") ;
                num=strtol(decl, &decl, 10) ;

           if(num<0 || *decl!='=') {
                sprintf(text, "Section FEATURE CROSS: "
                              "�������� ������ ������������� VERTEX\n - %s", work) ;
             SEND_ERROR(text) ;
                                        break ;
                                   }

               decl++ ;
/*- - - - - - - - - - - - - - - - - - - - -  ���������� ������ ����� */
           if(num>=VERTEXES_CNT) {

               VERTEXES=(RSS_Feature_Cross_Vertex *)
                          realloc(VERTEXES, (num+1)*sizeof(*VERTEXES)) ;
            if(VERTEXES==NULL) {
                  SEND_ERROR("Section FEATURE CROSS: ������������ ������") ;
                                   break ;
                               }                    

                                    VERTEXES_CNT=num+1 ;
                                 }
/*- - - - - - - - - - - - - - - - - - - ��������� ���������� X, Y, Z */
               VERTEXES[num].x_formula=(char *)                     /* �������� ������ ��� �������� */
                                         calloc(1, strlen(decl)+8) ; 
            if(VERTEXES[num].x_formula==NULL) {
                  SEND_ERROR("Section FEATURE CROSS: ������������ ������") ;
                                   break ;
                                              }

                    strcpy(VERTEXES[num].x_formula, decl) ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE CROSS: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                             *sep=0 ;
                           VERTEXES[num].z_formula=sep+1 ;
               sep=strrchr(VERTEXES[num].x_formula, ';') ;
            if(sep==NULL) {
                  sprintf(text, "Section FEATURE CROSS: "
                                "�������� ������ ������������� VERTEX\n - %s", work) ;
               SEND_ERROR(text) ;
                                        break ;
                          }
                                               *sep= 0 ;
                            VERTEXES[num].y_formula=sep+1 ;
/*- - - - - - - - - - - - - - - - - - - - -  ����������� ����������� */
             Calc=iGetCalculator() ;
          if(Calc== NULL) {
               SEND_ERROR("Section FEATURE SHOW: �� �������� ����������� ����������� ���������") ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - ������ ��������� ����� */
                                          VERTEXES[num].x_calculate=NULL ;
                                          VERTEXES[num].y_calculate=NULL ;
                                          VERTEXES[num].z_calculate=NULL ;

      do {
              coord="X" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].x_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].x,
                                     &VERTEXES[num].x_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* ���� ���� ������... */
                      
              coord="Y" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].y_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].y,
                                     &VERTEXES[num].y_calculate,
                                            error) ;
          if(status==-1)  break ;                                   /* ���� ���� ������... */

              coord="Z" ;
             status=Calc->vCalculate("STD_EXPRESSION",
                                      VERTEXES[num].z_formula,
                                       Bodies[body_num].extrn_pars,
                                            NULL,
                                     &VERTEXES[num].z,
                                     &VERTEXES[num].z_calculate,
                                            error) ;
         } while(0) ;

          if(status==-1) {
                  sprintf(text, "Section FEATURE CROSS: ������ �����������\n\n"
                                "������� %d, ���������� %s\n"
                                "%s" ,
                                 num, coord, error) ;
               SEND_ERROR(text) ;
                             return ;
                         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                           }
/*------------------------------------------------------------ ����� */
  
     else if(!memicmp(work, "$FACET", strlen("$FACET"))) {

           if(body_num==-1)  continue ; ;
/*- - - - - - - - - - - - - - - - - - - - - - - - ������ �� �������� */
              decl=strchr(work, '=') ;
           if(decl==NULL) {
                sprintf(text, "Section FEATURE CROSS: "
                              "�������� ������ ������������� FACET\n - %s", work) ;
             SEND_ERROR(text) ;
                                 break ;
                          }

              decl++ ;
/*- - - - - - - - - - - - - - - - - - - - - - ���������� ����� ����� */
              FACETS=(RSS_Feature_Cross_Facet *)
                       realloc(FACETS, (FACETS_CNT+1)*sizeof(*FACETS)) ;
           if(FACETS==NULL) {
                  SEND_ERROR("Section FEATURE CROSS: ������������ ������") ;
                                break ;
                            }

                memset(&FACETS[FACETS_CNT], 0, sizeof(FACETS[0])) ;

                               FACETS_CNT++ ;
/*- - - - - - - - - - - - - - - - - - - - ������������ ������ ������ */
#define  F    FACETS[FACETS_CNT-1]

          for(i=0 ; i<_VERTEX_PER_FACET_MAX ; i++) {

                    F.vertexes[i]=strtol(decl, &decl, 10) ;

               if(*decl==0)  break ;
                   decl++ ;
                                                   }

              if(i==_VERTEX_PER_FACET_MAX) {
                  SEND_ERROR("Section FEATURE CROSS: ������� ����� ������ � �����") ;
                                break ;
                                           }

                    F.vertexes_cnt=i+1 ;
#undef   F
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                                         }
/*----------------------------------------- ����������� ������������ */

     else                                                {

                sprintf(text, "Section FEATURE CROSS: "
                              "����������� ������������ %s10.10", work) ;
             SEND_ERROR(text) ;
                                 break ;
                                                         }
/*---------------------------------------------- ���������� �������� */

                                  }                                 /* CONTINUE.0 */

/*-------------------------------------------- ������������ �������� */

                free(buff) ;

/*-------------------------------------------------------------------*/

#undef    VERTEXES
#undef    VERTEXES_CNT
#undef    FACETS
#undef    FACETS_CNT

}


/********************************************************************/
/*								    */
/*                     ������ ���������� � ��������   		    */

    void RSS_Feature_Cross::vGetInfo(std::string *text)

{
        *text="Cross: Yes\r\n" ;
}


/*********************************************************************/
/*								     */
/*                          ������� ����                             */

    void RSS_Feature_Cross::vBodyDelete(char *body)
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*---------------------------------------------------- �������� ���� */

       if(this->Bodies[n_body].Vertexes_cnt)                        /* ����������� ������ ������ */
            free(this->Bodies[n_body].Vertexes) ;

       if(this->Bodies[n_body].Facets_cnt)                          /* ����������� ������ ������ */
            free(this->Bodies[n_body].Facets) ;

       if(this->Bodies_cnt>1) {                                     /* ���� ����� ������ ���� - */
                                                                    /*   - ��������� ������ ��� */
         if(n_body!=this->Bodies_cnt-1) 
               memmove(&this->Bodies[n_body], 
                       &this->Bodies[n_body+1],
                          sizeof(this->Bodies[0])*(this->Bodies_cnt-n_body-1)) ;
                              }
       else                   {                                     /* ���� ���� ������ ���� -  */
                                  free(this->Bodies) ;              /*   - ������� ���          */
                                       this->Bodies=NULL ;
                              }

                                    this->Bodies_cnt-- ;            /* ������������ ������� ��� */

/*-------------------------------------------------------------------*/

}


/********************************************************************/
/*								    */
/*                   ������� ������� ����� ����                     */

    void RSS_Feature_Cross::vBodyBasePoint(  char *body,                   
                                           double  x, 
                                           double  y, 
                                           double  z )
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].x_base     =x ;
       Bodies[n_body].y_base     =y ;
       Bodies[n_body].z_base     =z ;

       Bodies[n_body].Matrix_flag=0 ;
       Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                     ������� ���������� ����                      */

    void RSS_Feature_Cross::vBodyAngles(  char *body,                   
                                        double  azim, 
                                        double  elev, 
                                        double  roll    )
{
   int  n_body ;


/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].a_azim     =azim ;
       Bodies[n_body].a_elev     =elev ;
       Bodies[n_body].a_roll     =roll ;

       Bodies[n_body].Matrix_flag=  0 ;
       Bodies[n_body].recalc     =  1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*               ������� ������� ��������� ����                     */

    void RSS_Feature_Cross::vBodyMatrix(  char *body,
                                        double  matrix[4][4] )
{
   int  n_body ;


/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       memcpy(Bodies[n_body].Matrix, matrix, sizeof(Bodies[n_body].Matrix)) ;

              Bodies[n_body].Matrix_flag=1 ;
              Bodies[n_body].recalc     =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*           ������� �������� ��������� � ���������� ����           */

    void RSS_Feature_Cross::vBodyShifts(   char *body,                   
                                         double  x, 
                                         double  y, 
                                         double  z, 
                                         double  azim, 
                                         double  elev, 
                                         double  roll    )
{
   int  n_body ;


/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }

           if(n_body==this->Bodies_cnt)  return ;

/*--------------------------------------------- ��������� ���������� */

       Bodies[n_body].x_base_s=x ;
       Bodies[n_body].y_base_s=y ;
       Bodies[n_body].z_base_s=z ;
       Bodies[n_body].a_azim_s=azim ;
       Bodies[n_body].a_elev_s=elev ;
       Bodies[n_body].a_roll_s=roll ;

       Bodies[n_body].recalc  =1 ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*              ������� ������ ���������� ����                      */

    void RSS_Feature_Cross::vBodyPars(char *body, 
                                       struct RSS_Parameter *pars)
{
   int  n_body ;

/*-------------------------------------- ������������� ���� �� ����� */

    if(body==NULL) {
                      n_body=0 ;
                   }
    else           {

        for(n_body=0 ; n_body<this->Bodies_cnt ; n_body++)
           if(!stricmp(this->Bodies[n_body].name, body))  break ;
                   }
/*--------------------------------------------- ��������� ���������� */

                                                Pars_work=pars ;

   if(n_body<this->Bodies_cnt)  Bodies[n_body].extrn_pars=pars ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*            ����������� ������� �����������                       */

  RSS_Kernel *RSS_Feature_Cross::iGetCalculator(void)

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
/*                �������� ������������������ ��������              */  

    int RSS_Feature_Cross::vCheck(void *data)

{
   int  n ;            /* ������ ������� � ������ ��������*/
   int  m ;            /* ������ �������� "�����������" � ������ ������� */
  char  mark_1[128] ;  /* ����� ��� �������� �������� ����������� */
  char  mark_2[128] ;
   int  i ;
   int  j ;

#define  OBJECTS_CNT                       RSS_Kernel::kernel_objects_cnt
#define  OBJECTS                           RSS_Kernel::kernel_objects
#define    CROSS     ((RSS_Feature_Cross *)RSS_Kernel::kernel_objects[n]->Features[m])

/*------------------------------------ ������ ��������� ������������ */

                          this->RecalcPoints() ;

/*---------------------------------- �������� ���������� ����������� */

/*------------------------------------- �������� ������� ����������� */

   for(n=0 ; n<OBJECTS_CNT ; n++) {                                 /* CIRCLE.1 - ������� �������� */

//    if(!strcmp(OBJECTS[n]->Name, this->Object->Name))  continue ; /* ��������� ����������� �� ����� ���������      */
//                                                                  /*   � ������, "���������������" ������������... */

      if(!strcmp(OBJECTS[n]->Name, this->Object->Name))  continue ; /* �� ��������� ����������� ������ � ����� */

    for(m=0 ; m<OBJECTS[n]->Features_cnt ; m++)                     /* ���������, ������� �� � ������ ��������         */
      if(!strcmp(OBJECTS[n]->Features[m]->Type,                     /*  �������� "�����������", ���� ��� - ������� ��� */
                                 this   ->Type ))  break ;
      if(m==OBJECTS[n]->Features_cnt)  continue ;

                sprintf(mark_1, "%s.*.*", this->Object->Name) ;
                sprintf(mark_2, "%s.*.*", OBJECTS[n]->Name) ;
      if(!iMarkForCheck(mark_1, mark_2))  continue ;                /* ���� ������ �� �������� ��� ��������... */

                        CROSS->RecalcPoints() ;                     /* ���� ���� - ������������� ����� ������� ������� */

      if(!iOverallTest(&CROSS->overall, &this->overall))  continue; /* ���� �������� �������� �� ������������... */
/*- - - - - - - - - - - - - - - - - - - - - �������� ����������� ��� */
     for(i=0 ; i< this->Bodies_cnt ; i++)
     for(j=0 ; j<CROSS->Bodies_cnt ; j++) {                         /* CIRCLE.2 - ������� ��� ��� �������� */

                 sprintf(mark_1, "%s.%s", this->Object->Name, 
                                          this->Bodies[i].name) ;
                 sprintf(mark_2, "%s.%s", OBJECTS[n]->Name,   
                                         CROSS->Bodies[j].name) ;
       if(!iMarkForCheck(mark_1, mark_2))  continue ;               /* ���� ���� �� ��������� ��� ��������... */

       if(!iOverallTest(& this->Bodies[i].overall,                  /* ���� �������� ��� �� ������������... */
                        &CROSS->Bodies[j].overall ))  continue;     

       if(!iFacetsTest(& this->Bodies[i],                           /* ���� ����� ��� �� ������������... */
                       &CROSS->Bodies[j] ))  continue;     

                   return(_RSS_OBJECT_COLLISION_EXTERNAL) ;

                                          }                         /* CONTINUE.2 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                  }                                 /* CONTINUE.1 */
/*-------------------------------------------------------------------*/

#undef   OBJECTS_CNT
#undef   OBJECTS
#undef     CROSS

   return(0) ;
}


/********************************************************************/
/*								    */
/*      �������� ���������� ������� ��� ��� ������ ��� ��������     */  

    int RSS_Feature_Cross::iMarkForCheck(char *outer, char *inner)

{
  int  i ;

#define  MARKS        RSS_Feature_Cross::marks_list 
#define  MARKS_CNT    RSS_Feature_Cross::marks_list_cnt 


   for(i=0 ; i<MARKS_CNT ; i+=2)
     if(MARKS[i]->active)
      if((iMaskCheck(outer, MARKS[i  ]) &&
          iMaskCheck(inner, MARKS[i+1])   ) ||
         (iMaskCheck(inner, MARKS[i  ]) &&
          iMaskCheck(outer, MARKS[i+1])   )   )  return(0) ;

#undef   MARKS
#undef   MARKS_CNT

  return(1) ;
}

    int RSS_Feature_Cross::iMaskCheck(                  char *mask, 
                                      RSS_Feature_Cross_Mark *check)
{
  int  i ;

/*------------------------------------------- ���������� ����� ����� */

   if(check->desc[0]==0) {

                        sprintf(check->desc, "%s.%s.%s",
                                                check->object,
                                                check->link,
                                                check->body ) ;
                         }
/*----------------------------------------------------- ������ ����� */

  for(i=0 ; mask[i]!=0             &&
            mask[i]==check->desc[i]  ; i++) ;

    if(check->desc[i]==mask[i] ||
       check->desc[i]== '*'      )  return(1) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/********************************************************************/
/*								    */
/*  �������� ���� ��������� ������ ���������� �������� �����������  */  

  RSS_Feature_Cross_Mark *
        RSS_Feature_Cross::iSetMark(RSS_Feature_Cross_Mark *outer, 
                                    RSS_Feature_Cross_Mark *inner ) 
{

/*------------------------------------------------- ��������� ������ */

      marks_list=(RSS_Feature_Cross_Mark **)
                   realloc(marks_list, (marks_list_cnt+2)*sizeof(*marks_list)) ;

   if(marks_list==NULL)  return(NULL) ;

      marks_list[marks_list_cnt  ]=(RSS_Feature_Cross_Mark *)
                                     calloc(1, sizeof(**marks_list)) ;
      marks_list[marks_list_cnt+1]=(RSS_Feature_Cross_Mark *)
                                     calloc(1, sizeof(**marks_list)) ;
   if(marks_list[marks_list_cnt  ]==NULL ||
      marks_list[marks_list_cnt+1]==NULL   )  return(NULL) ;

/*-------------------------------------- ������������ ��������� ���� */

       memcpy(marks_list[marks_list_cnt  ], inner, sizeof(*inner)) ;
       memcpy(marks_list[marks_list_cnt+1], outer, sizeof(*outer)) ;

              marks_list[marks_list_cnt  ]->desc[0]=0 ;
              marks_list[marks_list_cnt  ]->pair   =marks_list[marks_list_cnt+1] ;
              marks_list[marks_list_cnt  ]->execute= 1 ;
              marks_list[marks_list_cnt  ]->active = 1 ;
              marks_list[marks_list_cnt+1]->desc[0]=0 ;
              marks_list[marks_list_cnt+1]->pair   =marks_list[marks_list_cnt] ;
              marks_list[marks_list_cnt+1]->execute= 1 ;
              marks_list[marks_list_cnt+1]->active = 1 ;
                         marks_list_cnt+=2 ;

/*-------------------------------------------------------------------*/

  return(marks_list[marks_list_cnt-2]) ;
}


/********************************************************************/
/*								    */
/*       ��������� ������ ���������� �������� �����������           */ 
/*	       � ������� ��� ������� ��������		            */

  int  RSS_Feature_Cross::iGetMarkEx(                  char  *object,
                                     RSS_Feature_Cross_Mark **list, 
                                                        int   list_size)

{
  RSS_Feature_Cross_Mark *pair ;
                     int  cnt ;
                     int  i ;


   for(cnt=0, i=0 ; i<marks_list_cnt ; i++) {

           pair=(RSS_Feature_Cross_Mark *)marks_list[i]->pair ;

     if(!stricmp(marks_list[i]->object, object) &&
         stricmp(      pair   ->object, object)   ) {

                                     list[cnt]=marks_list[i] ;
                                          cnt++ ;
                                       if(cnt==list_size)  break ;
                                                    }
                                            }

   return(cnt) ;
}


/********************************************************************/
/*								    */
/*       ��������� ������ ���������� �������� �����������           */ 
/*	       � ������� ����������� ��� �������                    */

  int  RSS_Feature_Cross::iGetMarkIn(                  char  *object,
                                     RSS_Feature_Cross_Mark **list, 
                                                        int   list_size)

{
  RSS_Feature_Cross_Mark *pair ;
                     int  cnt ;
                     int  i ;


   for(i=0 ; i<marks_list_cnt ; i++)  marks_list[i]->work=0 ;

   for(cnt=0, i=0 ; i<marks_list_cnt ; i++) {

     if(marks_list[i]->work)  continue ;

           pair=(RSS_Feature_Cross_Mark *)marks_list[i]->pair ;

     if(!stricmp(marks_list[i]->object, object) &&
        !stricmp(      pair   ->object, object)   ) {

                                    list[cnt]=marks_list[i] ;
                                         cnt++ ;
                                      if(cnt==list_size)  break ;

                          marks_list[i]->work=1 ;
                                pair   ->work=1 ;
                                                    }
                                            }

   return(cnt) ;
}


/********************************************************************/
/*								    */
/*                    �������� ����������� ���������                */ 

    int RSS_Feature_Cross::iOverallTest(RSS_Feature_Cross_Dim *o1,
                                        RSS_Feature_Cross_Dim *o2 )
{
  if(o1->x_min>o2->x_max || 
     o2->x_min>o1->x_max ||
     o1->y_min>o2->y_max || 
     o2->y_min>o1->y_max ||
     o1->z_min>o2->z_max || 
     o2->z_min>o1->z_max   )  return(0) ;

  return(1) ;
}


/********************************************************************/
/*								    */
/*                    �������� ����������� ������                   */ 

    int RSS_Feature_Cross::iFacetsTest(RSS_Feature_Cross_Body *b1,
                                       RSS_Feature_Cross_Body *b2 )
{
    Matrix2d  SummaryA ;
    Matrix2d  SummaryP ;
    Matrix2d  Point ;
      double  x0, y0, z0 ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  c_x[2], c_z[2] ;    /* ����� ����������� ����� � ���������� ����� */
      double  y ;
      double  v1, v2 ;
         int  cross ;
         int  cross1, cross2 ;
         int  n1 ;
         int  n2 ;
         int  m1[2] ;  /* ������� ������ ����� "���������" ����, */
         int  m2[2] ;  /*  ������������ ����� "����������" ����  */
         int  m ;
         int  ma ;     /* ������ "������" ������� ����� "����������" ����, ������������� ����� "���������" ���� */
         int  i ;

#define  F1     (b1->Facets[n1])
#define  F2     (b2->Facets[n2])
#define  V1      b1->Vertexes
#define  V2      b2->Vertexes

/*------------------------------------------------------- ���������� */

/*---------------------------------- ������� ������ "���������" ���� */

    for(n1=0 ; n1<b1->Facets_cnt ; n1++) {

         F1.overall.x_min=V1[F1.vertexes[0]].x_abs ;                /* ����������� ����������� ������� */
         F1.overall.x_max=V1[F1.vertexes[0]].x_abs ;
         F1.overall.y_min=V1[F1.vertexes[0]].y_abs ;
         F1.overall.y_max=V1[F1.vertexes[0]].y_abs ;
         F1.overall.z_min=V1[F1.vertexes[0]].z_abs ;
         F1.overall.z_max=V1[F1.vertexes[0]].z_abs ;

     for(i=1 ; i<F1.vertexes_cnt ; i++) {
       if(V1[F1.vertexes[i]].x_abs<F1.overall.x_min)  F1.overall.x_min=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].x_abs>F1.overall.x_max)  F1.overall.x_max=V1[F1.vertexes[i]].x_abs ;
       if(V1[F1.vertexes[i]].y_abs<F1.overall.y_min)  F1.overall.y_min=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].y_abs>F1.overall.y_max)  F1.overall.y_max=V1[F1.vertexes[i]].y_abs ;
       if(V1[F1.vertexes[i]].z_abs<F1.overall.z_min)  F1.overall.z_min=V1[F1.vertexes[i]].z_abs ;
       if(V1[F1.vertexes[i]].z_abs>F1.overall.z_min)  F1.overall.z_max=V1[F1.vertexes[i]].z_abs ;
                                        }
/*--------------------------------- ������� ������ "����������" ���� */

      for(n2=0 ; n2<b2->Facets_cnt ; n2++) {
/*- - - - - - - - - - - - - - - - - - - - - -  �������� �� ��������� */
             F2.overall.x_min=V2[F2.vertexes[0]].x_abs ;            /* ����������� ����������� ������� */
             F2.overall.x_max=V2[F2.vertexes[0]].x_abs ;
             F2.overall.y_min=V2[F2.vertexes[0]].y_abs ;
             F2.overall.y_max=V2[F2.vertexes[0]].y_abs ;
             F2.overall.z_min=V2[F2.vertexes[0]].z_abs ;
             F2.overall.z_max=V2[F2.vertexes[0]].z_abs ;

         for(i=1 ; i<F2.vertexes_cnt ; i++) {
           if(V2[F2.vertexes[i]].x_abs<F2.overall.x_min)  F2.overall.x_min=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].x_abs>F2.overall.x_max)  F2.overall.x_max=V2[F2.vertexes[i]].x_abs ;
           if(V2[F2.vertexes[i]].y_abs<F2.overall.y_min)  F2.overall.y_min=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].y_abs>F2.overall.y_max)  F2.overall.y_max=V2[F2.vertexes[i]].y_abs ;
           if(V2[F2.vertexes[i]].z_abs<F2.overall.z_min)  F2.overall.z_min=V2[F2.vertexes[i]].z_abs ;
           if(V2[F2.vertexes[i]].z_abs>F2.overall.z_min)  F2.overall.z_max=V2[F2.vertexes[i]].z_abs ;
                                            }

           if(F1.overall.x_min>F2.overall.x_max || 
              F1.overall.x_max<F2.overall.x_min ||
              F1.overall.y_min>F2.overall.y_max ||
              F1.overall.y_max<F2.overall.y_min ||
              F1.overall.z_min>F2.overall.z_max || 
              F1.overall.z_max<F2.overall.z_min   )  continue ;
/*- - - - - - - - - - - - - - �������� ����������� ��������� �-����� */
                   SummaryA.LoadZero(4,4) ;

              iToFlat(&V1[F1.vertexes[0]],
                      &V1[F1.vertexes[1]],
                      &V1[F1.vertexes[2]], &SummaryA) ;

                          cross=0 ;

         for(i=0 ; i<F2.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {  ma=i ;  break ;  }
                       else            cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {  ma=i ;  break ;  }
                       else          cross=-1 ;
                    }
                                            }

              if(i>=F2.vertexes_cnt)  continue ;
/*- - - - - - - - - - - - - - �������� ����������� ��������� �-����� */
                   SummaryP.LoadZero(4,4) ;

              iToFlat(&V2[F2.vertexes[0]],
                      &V2[F2.vertexes[1]],
                      &V2[F2.vertexes[2]], &SummaryP) ;

                          cross=0 ;
                              m=0 ;

         for(i=0 ; i<F1.vertexes_cnt ; i++) {

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                 Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                 Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
               y=Point.GetCell (1, 0) ;

            if(y>0) {
                       if(cross<0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=1 ;
                    }
            if(y<0) {
                       if(cross>0 ) {
                                       m1[m]=i-1 ;
                                       m2[m]=i ;
                                          m++ ;
                                       if(m>1)  break ;
                                    }
                          cross=-1 ;
                    }
                                            }

              if(m==0)  continue ;

              if(m==1) {
                          m1[1]= 0 ;
                          m2[1]=F1.vertexes_cnt-1 ;
                       }
/*- - - - - - - - - - - ����� ����������� ����� � ���������� �-����� */
         for(i=0 ; i<2 ; i++) {
                                         m=F1.vertexes[m1[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F1.vertexes[m2[i]] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V1[m].x_abs) ;
                 Point.SetCell (1, 0, V1[m].y_abs) ;
                 Point.SetCell (2, 0, V1[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryP, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[i]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[i]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[i]=x2 ;
                         c_z[i]=z2 ;
                      }
                              }
/*- - - �������� ���������� ����� ����������� ������ ������� �-����� */
/* ���������, ��� ������� ������� �������� ����� ����� ������������  */
/*  ���� �� ����� �� ����� ����������� ����� ���� � ��� �� ����      */
                 cross1=1 ;
                 cross2=1 ;
                     v1=0 ;
                     v2=0 ;

         for(i=0 ; i<=F2.vertexes_cnt ; i++) {

           if(i<F2.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V2[F2.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V2[F2.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V2[F2.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryP, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;
              y2=(z1-c_z[1])*(x2-x1)-(x1-c_x[1])*(z2-z1) ;

           if(v1*y1<0)  cross1=0 ;
           if(v2*y2<0)  cross2=0 ;

              v1=y1 ; 
              v2=y2 ; 

           if(cross1+cross2==0)  break ;         
                                            }

           if(cross1+cross2>0)  return(1) ;        
/*- - - - - - - - - - - ����� ����������� ����� � ���������� �-����� */
                                         m=F2.vertexes[ma-1] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                                         m=F2.vertexes[ma] ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, V2[m].x_abs) ;
                 Point.SetCell (1, 0, V2[m].y_abs) ;
                 Point.SetCell (2, 0, V2[m].z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&SummaryA, &Point) ;
              x2=Point.GetCell (0, 0) ;
              y2=Point.GetCell (1, 0) ;
              z2=Point.GetCell (2, 0) ;

           if(y1!=y2) {
                         c_x[0]=x1-y1*(x2-x1)/(y2-y1) ;
                         c_z[0]=z1-y1*(z2-z1)/(y2-y1) ;
                      }
           else       {
                         c_x[0]=x2 ;
                         c_z[0]=z2 ;
                      }
/*- - - �������� ���������� ����� ����������� ������ ������� �-����� */
/* ���������, ��� ������� ������� �������� ����� ����� ������������  */
/*  ���� �� ����� �� ����� ����������� ����� ���� � ��� �� ����      */
                 cross1=1 ;
                     v1=0 ;

         for(i=0 ; i<=F1.vertexes_cnt ; i++) {

           if(i<F1.vertexes_cnt) {

                   Point.LoadZero(4, 1) ;
                   Point.SetCell (0, 0, V1[F1.vertexes[i]].x_abs) ;
                   Point.SetCell (1, 0, V1[F1.vertexes[i]].y_abs) ;
                   Point.SetCell (2, 0, V1[F1.vertexes[i]].z_abs) ;
                   Point.SetCell (3, 0,  1) ;
                   Point.LoadMul (&SummaryA, &Point) ;

                 x1=x2 ;
                 z1=z2 ;
                 x2=Point.GetCell (0, 0) ;
                 y =Point.GetCell (1, 0) ;
                 z2=Point.GetCell (2, 0) ;
                                 }
           else                  {
                 x1=x2 ;
                 z1=z2 ;
                 x2=x0 ;
                 z2=z0 ;
                                 }

           if(i==0) {   x0=x2 ;
                        z0=z2 ;
                       continue ;  }

              y1=(z1-c_z[0])*(x2-x1)-(x1-c_x[0])*(z2-z1) ;

           if(v1*y1<0) {  cross1=0 ;  break ;  }

              v1=y1 ; 
                                            }

           if(cross1>0)  return(1) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                           }
/*---------------------------------- ������� ������ "���������" ���� */
                                         }
/*-------------------------------------------------------------------*/

#undef  F1
#undef  F2
#undef  V1
#undef  V2

  return(0) ;
}


/********************************************************************/
/*								    */
/*             ������ ������� ��������� ��������� x0z               */ 
/*                   � ��������� ����� P0-P1-P2                     */

    int RSS_Feature_Cross::iToFlat(RSS_Feature_Cross_Vertex *p0,
                                   RSS_Feature_Cross_Vertex *p1,
                                   RSS_Feature_Cross_Vertex *p2,
                                                   Matrix2d *Transform)

{
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
      double  x1, y1, z1 ;
      double  x2, y2, z2 ;
      double  v ;
      double  angle ;

/*------------------------------------------------- ����� � ����� P0 */

                Summary.LoadE      (4, 4) ;
                                                                    /* ����� 0 ����� �� ������� */ 
                  Local.Load4d_base(-p0->x_abs,
                                    -p0->y_abs,
                                    -p0->z_abs ) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      x1=p1->x_abs-p0->x_abs ;
                      x2=p2->x_abs-p0->x_abs ;
                      y1=p1->y_abs-p0->y_abs ;
                      y2=p2->y_abs-p0->y_abs ;
                      z1=p1->z_abs-p0->z_abs ;
                      z2=p2->z_abs-p0->z_abs ;

   if(fabs(z1)<fabs(z2)) { 
                            v=x1 ;  x1=x2 ; x2=v ;
                            v=y1 ;  y1=y2 ; y2=v ;
                            v=z1 ;  z1=z2 ; z2=v ;
                         }
/*----------------------------------------- ������� ������ Y � ��� Z */

   if(z1!=0.) {
                          v=z1/sqrt(x1*x1+z1*z1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_azim(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 1 */
                      Point.SetCell (0, 0, x1) ;
                      Point.SetCell (1, 0, y1) ;
                      Point.SetCell (2, 0, z1) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x1=Point.GetCell (0, 0) ;
                   y1=Point.GetCell (1, 0) ;
                   z1=Point.GetCell (2, 0) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- ������� ������ Z � ��� X */

   if(y1!=0.) {
                          v=y1/sqrt(x1*x1+y1*y1) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_roll(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;

                      Point.LoadZero(4, 1) ;                        /* ���������������� ���������� ����� 2 */
                      Point.SetCell (0, 0, x2) ;
                      Point.SetCell (1, 0, y2) ;
                      Point.SetCell (2, 0, z2) ;
                      Point.SetCell (3, 0,  1) ;
                      Point.LoadMul (&Local, &Point) ;
                   x2=Point.GetCell (0, 0) ;
                   y2=Point.GetCell (1, 0) ;
                   z2=Point.GetCell (2, 0) ;
              }
/*----------------------------------------- ������� ������ X � ��� Y */

   if(y2!=0.) {
                          v=y2/sqrt(y2*y2+z2*z2) ;
                      angle=asin(v)*_RAD_TO_GRD ;

                  Local.Load4d_elev(-angle) ;
                Summary.LoadMul    (&Local, &Summary) ;
              }
/*--------------------------------------------------------- �������� */
/*
                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p0->x_abs) ;
                 Point.SetCell (1, 0, p0->y_abs) ;
                 Point.SetCell (2, 0, p0->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p1->x_abs) ;
                 Point.SetCell (1, 0, p1->y_abs) ;
                 Point.SetCell (2, 0, p1->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;

                 Point.LoadZero(4, 1) ;
                 Point.SetCell (0, 0, p2->x_abs) ;
                 Point.SetCell (1, 0, p2->y_abs) ;
                 Point.SetCell (2, 0, p2->z_abs) ;
                 Point.SetCell (3, 0,  1) ;
                 Point.LoadMul (&Summary, &Point) ;
              x1=Point.GetCell (0, 0) ;
              y1=Point.GetCell (1, 0) ;
              z1=Point.GetCell (2, 0) ;
*/
/*-------------------------------------------------------------------*/

          Transform->Copy(&Summary) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*                     �������� ����� ��� �������                   */  

    int RSS_Feature_Cross::RecalcPoints(void)

{
  RSS_Kernel *Calc ;
    Matrix2d  Summary ;
    Matrix2d  Local ;
    Matrix2d  Point ;
         int  overall_recalc ;
         int  i ;
         int  j ;

#define   B   Bodies
#define   V   Bodies[i].Vertexes

/*------------------------------------------------------- ���������� */

           Calc=iGetCalculator() ;                                  /* ����������� ����������� */
        if(Calc==NULL)  return(-1) ;

/*---------------------------------------------- ��������� ����� ��� */

        overall_recalc=0 ;

   for(i=0 ; i<Bodies_cnt ; i++) {                                  /* CIRCLE.1 */

       if(!B[i].recalc)  continue ;                                 /* ���� ���� ��� �����������... */
           B[i].recalc=0 ;
        overall_recalc=1 ;
/*- - - - - - - - - - - - - - - - - - ������� ������� �������������� */
       if(B[i].Matrix_flag) {                                       /* ���� ������ �������... */
                Summary.LoadArray(4, 4, (double *)B[i].Matrix) ;
                            }
       else                 {                                       /* ���� ������ ���������� */
                Summary.LoadE      (4, 4) ;
                  Local.Load4d_base(B[i].x_base, B[i].y_base, B[i].z_base) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll) ;
                Summary.LoadMul    (&Summary, &Local) ;
                            }

                  Local.Load4d_base(B[i].x_base_s, B[i].y_base_s, B[i].z_base_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_azim(-B[i].a_azim_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_elev(-B[i].a_elev_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
                  Local.Load4d_roll(-B[i].a_roll_s) ;
                Summary.LoadMul    (&Summary, &Local) ;
/*- - - - - - - - - - - - - - - - - - - - -  ������� ��������� ����� */
     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {

           if(V[j].x_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].x, &V[j].x_calculate, NULL ) ;

           if(V[j].y_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].y, &V[j].y_calculate, NULL ) ;

           if(V[j].z_calculate!=NULL)
                    Calc->vCalculate("STD_EXPRESSION", NULL,
                                       this->Bodies[i].extrn_pars, NULL,
                                       &V[j].z, &V[j].z_calculate, NULL ) ;

                      Point.LoadZero(4, 1) ;
                      Point.SetCell (0, 0, V[j].x) ;
                      Point.SetCell (1, 0, V[j].y) ;
                      Point.SetCell (2, 0, V[j].z) ;
                      Point.SetCell (3, 0,   1   ) ;

                      Point.LoadMul (&Summary, &Point) ;            /* ������������ ���������� ����� */
                                                                    /*  � ���������� �� */
           V[j].x_abs=Point.GetCell (0, 0) ;
           V[j].y_abs=Point.GetCell (1, 0) ;
           V[j].z_abs=Point.GetCell (2, 0) ;
           
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - ������� ��������� ���� */
                        B[i].overall.x_min=V[1].x_abs ;
                        B[i].overall.x_max=V[1].x_abs ;
                        B[i].overall.y_min=V[1].y_abs ;
                        B[i].overall.y_max=V[1].y_abs ;
                        B[i].overall.z_min=V[1].z_abs ;
                        B[i].overall.z_max=V[1].z_abs ;

     for(j=1 ; j<B[i].Vertexes_cnt ; j++) {
              if(B[i].overall.x_min>V[j].x_abs)  B[i].overall.x_min=V[j].x_abs ;
         else if(B[i].overall.x_max<V[j].x_abs)  B[i].overall.x_max=V[j].x_abs ;
              if(B[i].overall.y_min>V[j].y_abs)  B[i].overall.y_min=V[j].y_abs ;
         else if(B[i].overall.y_max<V[j].y_abs)  B[i].overall.y_max=V[j].y_abs ;
              if(B[i].overall.z_min>V[j].z_abs)  B[i].overall.z_min=V[j].z_abs ;
         else if(B[i].overall.z_max<V[j].z_abs)  B[i].overall.z_max=V[j].z_abs ;
                                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                       }                            /* CONTINUE.1 */
/*---------------------------------------------- ��������� ��������� */

    if(overall_recalc) {
                              overall.x_min= 1.e+99 ;
                              overall.x_max=-1.e+99 ;
                              overall.y_min= 1.e+99 ;
                              overall.y_max=-1.e+99 ;
                              overall.z_min= 1.e+99 ;
                              overall.z_max=-1.e+99 ;

      for(i=0 ; i<Bodies_cnt ; i++) {
        if(overall.x_min>B[i].overall.x_min)  overall.x_min=B[i].overall.x_min ;
        if(overall.x_max<B[i].overall.x_max)  overall.x_max=B[i].overall.x_max ;
        if(overall.y_min>B[i].overall.y_min)  overall.y_min=B[i].overall.y_min ;
        if(overall.y_max<B[i].overall.y_max)  overall.y_max=B[i].overall.y_max ;
        if(overall.z_min>B[i].overall.z_min)  overall.z_min=B[i].overall.z_min ;
        if(overall.z_max<B[i].overall.z_max)  overall.z_max=B[i].overall.z_max ;
                                    }
                       }
/*-------------------------------------------------------------------*/

  return(0) ;
}

