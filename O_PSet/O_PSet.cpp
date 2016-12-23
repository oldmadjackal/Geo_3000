/********************************************************************/
/*								    */
/*             ������ ���������� �������� "����� �����"             */
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

#include "gl\gl.h"
#include "gl\glu.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "O_PSet.h"

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
/*		    	����������� ������                          */

     static   RSS_Module_PSet  ProgramModule ;


/********************************************************************/
/*								    */
/*		    	����������������� ����                      */

 O_PSET_API char *Identify(void)

{
	return(ProgramModule.keyword) ;
}


 O_PSET_API RSS_Kernel *GetEntry(void)

{
	return(&ProgramModule) ;
}

/********************************************************************/
/********************************************************************/
/**							           **/
/**    �������� ������ ������ ���������� �������� "����� �����"	   **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*                            ������ ������                         */

  struct RSS_Module_PSet_instr  RSS_Module_PSet_InstrList[]={

 { "help",   "?",  "#HELP   - ������ ��������� ������", 
                    NULL,
                   &RSS_Module_PSet::cHelp   },
 { "create", "cr", "#CREATE - ������� ����� �����",
                   " CREATE <���>\n"
                   "   ������� ����������� ����� �����",
                   &RSS_Module_PSet::cCreate },
 { "add",    "a",  "#ADD - ��������� ����� � ����� �����",
                   " ADD <��� ������>[:idx] X Y Z A E R\n"
                   "   �������� ����� � ����������� ������������\n"
                   " ADD <��� ������>[:idx] <��� ������-�������>\n"
                   "   �������� �����, ���������� ������������� ������-�������\n"
                   " ADD <��� ������>[:idx] @<��� �������>\n"
                   "   �������� ������� ����� �������\n"
                   "   ���� �������� idx - ������� ������������ � �������� �������,\n"
                   "  � � ��������� ������ -  � ����� ������ �����\n",
                   &RSS_Module_PSet::cAdd },
 {  NULL }
                                                            } ;

/********************************************************************/
/*								    */
/*		     ����� ����� ������             		    */

    struct RSS_Module_PSet_instr *RSS_Module_PSet::mInstrList=NULL ;

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Module_PSet::RSS_Module_PSet(void)

{
	   keyword="Geo" ;
    identification="Points_set_object" ;

        mInstrList=RSS_Module_PSet_InstrList ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Module_PSet::~RSS_Module_PSet(void)

{
}


/********************************************************************/
/*								    */
/*                ���������� ��������� ������                       */

    void  RSS_Module_PSet::vShow(char *layer)

{
}


/********************************************************************/
/*								    */
/*		        ��������� �������       		    */

  int  RSS_Module_PSet::vExecuteCmd(const char *cmd)

{
  static  int  direct_command ;   /* ���� ������ ������ ������� */
         char  command[1024] ;
         char *instr ;
         char *end ;
          int  status ;
          int  i ;

#define  _SECTION_FULL_NAME   "PSET"
#define  _SECTION_SHRT_NAME   "PSET"

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
                     (WPARAM)_USER_COMMAND_PREFIX, (LPARAM)"Object Points Set:") ;
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
                     *end!='/'    ; end++) ;

      if(*end!= 0 &&
         *end!=' '  )  memmove(end+1, end, strlen(end)+1) ;

      if(*end!=0) {  *end=0 ;  end++ ;  }
      else            end="" ;

   for(i=0 ; mInstrList[i].name_full!=NULL ; i++)                   /* ���� ������� � ������ */
     if(!stricmp(instr, mInstrList[i].name_full) ||
        !stricmp(instr, mInstrList[i].name_shrt)   )   break ;

     if(mInstrList[i].name_full==NULL) {                            /* ���� ����� ������� ���... */

          status=this->kernel->vExecuteCmd(cmd) ;                   /*  �������� �������� ������ ����... */
       if(status)  SEND_ERROR("������ POINTS SET: ����������� �������") ;
                                            return(-1) ;
                                       }
 
     if(mInstrList[i].process!=NULL)                                /* ���������� ������� */
                status=(this->*mInstrList[i].process)(end) ;
     else       status=  0 ;

/*-------------------------------------------------------------------*/

   return(status) ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Module_PSet::vReadSave(std::string *data)

{

/*----------------------------------------------- �������� ��������� */

   if(memicmp(data->c_str(), "#BEGIN MODULE PSET\n", 
                      strlen("#BEGIN MODULE PSET\n")) &&
      memicmp(data->c_str(), "#BEGIN OBJECT PSET\n", 
                      strlen("#BEGIN OBJECT PSET\n"))   )  return ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Module_PSet::vWriteSave(std::string *text)

{

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN MODULE PSET\n" ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� HELP                    */

  int  RSS_Module_PSet::cHelp(char *cmd)

{ 
    DialogBoxIndirect(GetModuleHandle(NULL),
			(LPCDLGTEMPLATE)Resource("IDD_HELP", RT_DIALOG),
			   GetActiveWindow(), Object_PSet_Help_dialog) ;

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� CREATE                  */
/*								    */
/*      CREATE <���>                                                */

  int  RSS_Module_PSet::cCreate(char *cmd)

{
           char *name ;
           char *end ;
            int  status ;

/*-------------------------------------- ���������� ��������� ������ */

   do {                                                             /* BLOCK.1 */
                  name=cmd ;                                        /* ��������� ��� ������� */
                   end=strchr(name, ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;

      } while(0) ;                                                  /* BLOCK.1 */

/*------------------------------------------------- �������� ������� */

      status=CreateObject(name) ;
   if(status!=0)  return(status) ;

/*-------------------------------------------------------------------*/

   return(0) ;
}


/********************************************************************/
/*								    */
/*		      ���������� ���������� ADD                     */
/*								    */
/*         ADD <��� ������>[:idx] X Y Z A E R                       */
/*         ADD <��� ������>[:idx] <��� ������-�������>              */
/*         ADD <��� ������>[:idx] @<��� �������>                    */

  int  RSS_Module_PSet::cAdd(char *cmd)

{
#define  _COORD_MAX   6
#define   _PARS_MAX   7

                  char  *pars[_PARS_MAX] ;
                  char  *name ;
                  char **xyz ;
                double   coord[_COORD_MAX] ;
                   int   coord_cnt ;
       RSS_Object_PSet  *set ;
  RSS_Object_PSet_elem   set_elem ;
                   int   idx ;
                   int   master_flag ;
                  char   object_name[128] ;
            RSS_Object  *object ;
             RSS_Joint   degrees[50] ;       /* ������ �������� ����������� ������� */
                   int   cnt ;
                  char  *sep ;
                  char  *error ;
                  char  *end ;
                  char   text[1024] ;
                   int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*---------------------------------------- �������� ��������� ������ */

                 memset(pars,  0, sizeof(pars)) ;
                 memset(coord, 0, sizeof(coord)) ;
/*- - - - - - - - - - - - - - - - - - - - - - - -  ������ ���������� */        
    for(end=cmd, i=0 ; i<_PARS_MAX ; end++, i++) {
      
                pars[i]=end ;
                   end =strchr(pars[i], ' ') ;
                if(end==NULL)  break ;
                  *end=0 ;
                                                 }

                     name= pars[0] ;
                      xyz=&pars[1] ;   

/*-------------------------------------------- �������� ����� ������ */

    if(name==NULL) {                                                /* ���� ��� �� ������... */
                      SEND_ERROR("�� ������ ��� ������. \n"
                                 "��������: ADD <��� ������> ...") ;
                                     return(-1) ;
                   }
/*- - - - - - - - - - - - - - - - - - - - - - - -  ��������� ������� */
       sep=strchr(name, ':') ;
    if(sep!=NULL) {
                      idx=strtoul(sep+1, &end, 10) ;
                     if(*end!=0) {
                          SEND_ERROR("������ ����� ������ ���� ������.\n"
                                     "��������: ADD SET:2 ...") ;
                                           return(-1) ;
                                 }                        
                  }
    else          {
                      idx=-1 ;
                  }
/*- - - - - - - - - - - - - - - - - - - - - -  ����� ������ �� ����� */
       set=FindObject(name) ;                                       /* ���� ������ �� ����� */
    if(set==NULL)  return(-1) ;

/*------------------------------------------- �������� ������� ����� */

   if(xyz[0]==NULL) {
               SEND_ERROR("�� ���������� ����������� �����.\n") ;
                                     return(-1) ;
                    }
/*------------------------------------------------- ������ �� ������ */

   if(!isdigit(*xyz[0])) {
/*- - - - - - - - - - - - - - - - ������ ������������� ������� ����� */
      if(*xyz[0]=='@') {
                          master_flag=0 ;
                               xyz[0]++ ;
                   strcpy(object_name, xyz[0]) ;
                
               sep=strchr(object_name, '.') ;                       /* �������� ������������ ������ ������� */
            if(sep!=NULL)  *sep=0 ;

                       }
      else             {
                          master_flag=1 ;
                           strcpy(object_name, xyz[0]) ;
                       }
/*- - - - - - - - - - - - - - - - - - - - - - ����� ������� �� ����� */
     for(i=0 ; i<OBJECTS_CNT ; i++)
       if(!stricmp(OBJECTS[i]->Name, object_name)) { 
                                            object=OBJECTS[i] ;
                                                       break ;     
                                                   }

       if(i==OBJECTS_CNT) {                                         /* ���� ������ �� ������... */
             SEND_ERROR("������� � ����� ������ �� ����������") ;
                                return(-1) ;
                          }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         }
/*------------------------------------------------- ������ ��������� */

   if(object==NULL) {

    for(i=0 ; xyz[i]!=NULL && i<_COORD_MAX ; i++) {

             coord[i]=strtod(xyz[i], &end) ;
        if(*end!=0) {  
                       SEND_ERROR("������������ �������� ����������") ;
                                       return(-1) ;
                    }
                                                  }

                             coord_cnt=  i ;

                                 error= NULL ;
         if(coord_cnt<6)  error="������ ���� ������� 6 ���������" ;

         if(error!=NULL) {  SEND_ERROR(error) ;
                                  return(-1) ;   }
                    }
/*------------------------------------------------- ���������� ����� */

                     memset(&set_elem, 0, sizeof(set_elem)) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ���������� �������� */
        if(object==NULL  ) {

               set_elem.type       =_RSS_ABSOLUTE_TARGET ;
               set_elem.target.x   =coord[0] ;
               set_elem.target.y   =coord[1] ;
               set_elem.target.z   =coord[2] ;
               set_elem.target.azim=coord[3] ;
               set_elem.target.elev=coord[4] ;
               set_elem.target.roll=coord[5] ;
                           }
/*- - - - - - - - - - - - - - - - - - - - - -  ������� ����� ������� */
   else if(master_flag==0) {

               set_elem.type  =_RSS_OBJECT_TARGET ;
               set_elem.object=(char *)calloc(1, strlen(xyz[0])+1) ;
        strcpy(set_elem.object, xyz[0]) ;
                           }
/*- - - - - - - - - - - - - - - - - - -  ������������ ������-������� */
   else                    {

         if(set->mMasterObject!= NULL &&                            /* �������� ����� ������-������� */
            set->mMasterObject!=object  ) {
                 sprintf(text, "��� ������� ������ ��� ��� "
                               "���������� ������ ������-������ - %s",
                                               set->mMasterObject->Name) ;
              SEND_ERROR(text) ;
                   return(-1) ;
                                          }

                      cnt=object->vGetJoints(degrees) ;             /* ��������� ��������� ������ �������� ����������� */

               set_elem.type  =_RSS_JOINTS_TARGET ;
               set_elem.joints=(RSS_Joint *)calloc(cnt, sizeof(*degrees)) ;
        memcpy(set_elem.joints, degrees, cnt*sizeof(*degrees)) ;

               set->mMasterObject=object ;
               set->mDegreesCnt  =  cnt ;
                           }
/*------------------------------------------------ ��������� ������� */

          if(idx==      -1       )  idx=set->mPointsCnt ;
          if(idx >set->mPointsCnt)  idx=set->mPointsCnt ;

/*------------------------------------------ ���������� ������ ����� */

        set->mPointsCnt++ ;
        set->mPointsList=(RSS_Object_PSet_elem *)
                           realloc(set->mPointsList, 
                                    set->mPointsCnt*sizeof(*set->mPointsList)) ;

     if(set->mPointsList==NULL) {
                                     set->mPointsCnt=0 ;
                      SEND_ERROR("������������ ������") ;
                                      return(-1) ;
                                }

     if(idx!=set->mPointsCnt-1)
          memmove(&set->mPointsList[idx+1], 
                  &set->mPointsList[idx], 
                   (set->mPointsCnt-idx-1)*sizeof(*set->mPointsList)) ;

           memcpy(&set->mPointsList[idx], &set_elem, sizeof(set_elem)) ;
                       
/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

#undef  _COORD_MAX   
#undef   _PARS_MAX    

   return(0) ;
}


/********************************************************************/
/*								    */
/*		   ����� ������� ���� PSET �� �����                 */

  RSS_Object_PSet *RSS_Module_PSet::FindObject(char *name)

{
     char   text[1024] ;
      int   i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 

/*------------------------------------------- ����� ������� �� ����� */ 

       for(i=0 ; i<OBJECTS_CNT ; i++)                               /* ���� ������ �� ����� */
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� �������... */
                           sprintf(text, "������� � ������ '%s' "
                                         "�� ����������", name) ;
                        SEND_ERROR(text) ;
                            return(NULL) ;
                       }
/*-------------------------------------------- �������� ���� ������� */ 

     if(stricmp(OBJECTS[i]->Type, "PSet")) {
           SEND_ERROR("������ �� �������� �������� ���� POINTS SET") ;
                            return(NULL) ;
                                           }
/*-------------------------------------------------------------------*/ 

   return((RSS_Object_PSet *)OBJECTS[i]) ;
  
#undef   OBJECTS
#undef   OBJECTS_CNT

}


/********************************************************************/
/*								    */
/*		      �������� �������                              */

  int  RSS_Module_PSet::CreateObject(char *name)

{
    RSS_Object_PSet *object ;
                int  i ;

#define   OBJECTS       this->kernel->kernel_objects 
#define   OBJECTS_CNT   this->kernel->kernel_objects_cnt 
 
/*--------------------------------------------------- �������� ����� */

    if(name[0]==0) {                                                /* ���� ��� �� ������ */
              SEND_ERROR("������ POINTS SET: �� ������ ��� �������") ;
                                return(-1) ;
                   }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name)) {
              SEND_ERROR("������ POINTS SET: ������ � ����� ������ ��� ����������") ;
                                return(-1) ;
                                              }
/*------------------------------------------------- �������� ������� */

       object=new RSS_Object_PSet ;
    if(object==NULL) {
              SEND_ERROR("������ POINTS SET: ������������ ������ ��� �������� �������") ;
                        return(-1) ;
                     }
/*---------------------------------- �������� ������� � ����� ������ */

       OBJECTS=(RSS_Object **)
                 realloc(OBJECTS, (OBJECTS_CNT+1)*sizeof(*OBJECTS)) ;
    if(OBJECTS==NULL) {
              SEND_ERROR("������ POINTS SET: ������������ ������") ;
                                return(-1) ;
                      }

              OBJECTS[OBJECTS_CNT]=object ;
       strcpy(OBJECTS[OBJECTS_CNT]->Name, name) ;
                      OBJECTS_CNT++ ;

        SendMessage(this->kernel_wnd, WM_USER,
                     (WPARAM)_USER_DEFAULT_OBJECT, (LPARAM)name) ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

  return(0) ;
}


/********************************************************************/
/********************************************************************/
/**							           **/
/**		  �������� ������ ������� "����� �����"	           **/
/**							           **/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/*								    */
/*		       ����������� ������			    */

     RSS_Object_PSet::RSS_Object_PSet(void)

{
   strcpy(Type, "PSet") ;

        mMasterObject=NULL ;
        mDegreesCnt  =  0 ;
        mPointsList  =NULL ;
        mPointsCnt   =  0 ;
}


/********************************************************************/
/*								    */
/*		        ���������� ������			    */

    RSS_Object_PSet::~RSS_Object_PSet(void)

{
  int  i ;


   if(mPointsList!=NULL) {

     for(i=0 ; i<mPointsCnt ; i++)
            if(mPointsList[i].type==_RSS_JOINTS_TARGET)  
                                free(mPointsList[i].joints) ;
       else if(mPointsList[i].type==_RSS_OBJECT_TARGET)  
                                free(mPointsList[i].object) ;

                                free(mPointsList) ;
                         }
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Object_PSet::vWriteSave(std::string *text)

{
  char  field[1024] ;

/*----------------------------------------------- ��������� �������� */

     *text="#BEGIN OBJECT PSET\n" ;

/*----------------------------------------------------------- ������ */

    sprintf(field, "NAME=%s\n",       this->Name  ) ;  *text+=field ;

/*------------------------------------------------ �������� �������� */

     *text+="#END\n" ;

/*-------------------------------------------------------------------*/
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Object_PSet::vSpecial(char *oper, void *data)
{
  int  idx ;

/*--------------------------------------------------- ������-������ */

   if(!stricmp(oper, "POINTS_SET:MASTER")) {

                *((RSS_Object **)data)=mMasterObject ;

                            return(0) ;
                                           }
/*----------------------------------------------------- ������ ����� */

   if(!memicmp(oper, "POINTS_SET:GET", 
              strlen("POINTS_SET:GET"))) {

         idx=atoi(oper+strlen("POINTS_SET:GET")) ;
      if(idx<0 || idx>=mPointsCnt)  return(-1) ;

      if(mPointsList[idx].type==_RSS_ABSOLUTE_TARGET)
            memcpy(data, &mPointsList[idx].target, 
                   sizeof(mPointsList[idx].target)) ;

      if(mPointsList[idx].type==  _RSS_JOINTS_TARGET)
            memcpy(data,  mPointsList[idx].joints, 
                   sizeof(mPointsList[idx].joints[0])*mDegreesCnt) ;

      if(mPointsList[idx].type==  _RSS_OBJECT_TARGET)
            memcpy(data,  mPointsList[idx].object, 
                   strlen(mPointsList[idx].object)+1) ;

                            return(mPointsList[idx].type) ;
                                         }
/*-------------------------------------------------------------------*/


  return(-1) ;
}


