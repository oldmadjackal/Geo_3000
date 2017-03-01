
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <malloc.h>


#include "..\RSS_feature\RSS_feature.h"
#include "RSS_object.h"

#pragma warning(disable : 4996)


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
/*		      ���������� ������ "������"	             */
/*								     */
/*********************************************************************/

/*********************************************************************/
/*								     */
/*		       ����������� ������ "������"      	     */

     RSS_Object::RSS_Object(void)

{
     Parameters    =NULL ;
     Parameters_cnt=  0 ;
       Features    =NULL ;
       Features_cnt=  0 ;
     Morphology    =NULL ;
     Morphology_cnt=  0 ;

        ErrorEnable=1 ;
}


/*********************************************************************/
/*								     */
/*		        ���������� ������ "������"      	     */

    RSS_Object::~RSS_Object(void)

{
}


/********************************************************************/
/*								    */
/*		  ���./����. ��������� �� �������		    */

    void  RSS_Object::vErrorMessage(int  on_off)

{
         ErrorEnable=on_off ;
}


/********************************************************************/
/*								    */
/*		        ������� ������ �� ������		    */

    void  RSS_Object::vReadSave(std::string *text)

{
}


/********************************************************************/
/*								    */
/*		        �������� ������ � ������		    */

    void  RSS_Object::vWriteSave(std::string *text)

{
     *text="" ;
}


/********************************************************************/
/*								    */
/*		      ���������� �������        		    */

    void  RSS_Object::vFree(void)

{
}


/********************************************************************/
/*								    */
/*		      ����������� ��������      		    */

    void  RSS_Object::vFormDecl(void)

{
   sprintf(Decl, "%s  %s", Name, Type) ;
}


/********************************************************************/
/*								    */
/*		      ������ � �������� �������   		    */
/*								    */
/*   Return: ����� ������� �����                                    */

     int  RSS_Object::vGetBasePoint(RSS_Point *points)

{
    return(0) ;
}

    void  RSS_Object::vSetBasePoint(RSS_Point *points)

{
}


/********************************************************************/
/*								    */
/*		      ������ � �������� �������   		    */
/*								    */
/*   Return: ����� ������� �����                                    */

     int  RSS_Object::vGetTargetPoint(RSS_Point *points)

{
    return(0) ;
}

    void  RSS_Object::vSetTargetPoint(RSS_Point *points, int  points_cnt)

{
}


/********************************************************************/
/*								    */
/*		      ������ � ������������       		    */
/*								    */
/*   Return: ����� ����������                                       */

     int  RSS_Object::vGetJoints(RSS_Joint *joints)

{
    return(0) ;
}

    void  RSS_Object::vSetJoints(RSS_Joint *joints, int  joints_cnt)

{
}


/********************************************************************/
/*								    */
/*                 ������ � ������������������ �����                */

    int  RSS_Object::vGetAmbiguity(void)
{
   return(0) ;
}

   void  RSS_Object::vSetAmbiguity(char *spec)
{
}


/********************************************************************/
/*								    */
/*           ��������� ������������ ������� �� ������� �����        */

     int  RSS_Object::vSolveByTarget(void)

{
    return(_RSS_OBJECT_NOTARGETED) ;
}


/********************************************************************/
/*								    */
/*           ��������� ������������ ������� �� �����������          */

     int  RSS_Object::vSolveByJoints(void)

{
    return(0) ;
}


/********************************************************************/
/*								    */
/*     ������ ���������� ������� �����, ����������� � �������       */

     int  RSS_Object::vGetTarget(char *part, RSS_Point *target)

{
    memset(target, 0, sizeof(*target)) ;

    return(0) ;
}


/********************************************************************/
/*								    */
/*		�������� ������������������ �������                 */

     int  RSS_Object::vCheckFeatures(void *data)

{
  int  status ;
  int  i ;


   for(i=0 ; i<this->Features_cnt ; i++) {
           status=this->Features[i]->vCheck(data) ;
        if(status)  break ;
                                         }
   return(status) ;
}


/********************************************************************/
/*								    */
/*		      ������ �� ����������                          */

    void  RSS_Object::vSetFeature(RSS_Feature *feature)

{
}

    void  RSS_Object::vGetFeature(RSS_Feature *feature)

{
}


/********************************************************************/
/*								    */
/*                  ������ �� ������ ���������                      */

    RSS_Object *RSS_Object::vGetSlice(void)

{
   return(NULL) ;
}


    void  RSS_Object::vSetSlice(RSS_Object *slice)

{
}


/********************************************************************/
/*								    */
/*		      ������� ��������� �������   		    */

    void  RSS_Object::vEditStructure(void)
{
}


/********************************************************************/
/*								    */
/*                        ����������� ��������                      */

     int  RSS_Object::vSpecial(char *oper, void *data)
{
  return(-1) ;
}


/********************************************************************/
/*								    */
/*                  �������� ������� ����������                     */

    void  RSS_Object::vAddMorphology(RSS_Morphology *elem)
{
#define      M     Morphology[Morphology_cnt-1] 

        Morphology_cnt++ ;
        Morphology=(RSS_Morphology *)
                     realloc(Morphology, Morphology_cnt*            /* ���������� ������ ��������� */
                                           sizeof(*Morphology)) ;
     if(Morphology==NULL) {
                              Morphology_cnt=0 ;
                                  return ;
                          }

                         memset(&M, 0, sizeof(M)) ;                 /* ������������� �������� */

                         strcpy(M.object, this->Name) ;
   if(elem->link!=NULL)  strcpy(M.link,   elem->link) ;
   if(elem->body!=NULL)  strcpy(M.body,   elem->body) ;
        
#undef   M
}


/********************************************************************/
/*								    */
/*                 ��������� ������� ����������                     */

    void  RSS_Object::vEditMorphology(RSS_Morphology *elem)
{
   int  i ;


    for(i=0 ; i<Morphology_cnt ; i++)
      if(Morphology[i].ptr==elem->ptr)  break ;
                               
      if(i>=Morphology_cnt) {
                              vAddMorphology(elem) ;
                            }
      else                  {
                if(elem->link!=NULL)  strcpy(Morphology[i].link, elem->link) ;
                if(elem->body!=NULL)  strcpy(Morphology[i].body, elem->body) ;
                            }
}


/********************************************************************/
/*								    */
/*                �������� ������ ���������� ����������             */

   int  RSS_Object::vListControlPars(RSS_ControlPar *list)

{
   return(0) ;
}


/********************************************************************/
/*								    */
/*               ���������� �������� ��������� ����������           */

   int  RSS_Object::vSetControlPar(RSS_ControlPar *par)   

{
   return(-1) ;
}
