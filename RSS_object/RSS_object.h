
#ifndef  RSS_OBJECT_H 

#define  RSS_OBJECT_H 

#include "..\RSS_Feature\RSS_Feature.h"

#include <string>

#ifdef RSS_OBJECT_EXPORTS
#define RSS_OBJECT_API __declspec(dllexport)
#else
#define RSS_OBJECT_API __declspec(dllimport)
#endif

/*---------------------------------------------------- ���� �������� */

#define   _RSS_OBJECT_NOTARGETED           -1    /* ������ ���������� */
#define   _RSS_OBJECT_UNATTAINABLE_POINT   -2    /* ������������ ����� */
#define   _RSS_OBJECT_DEGREES_LIMIT        -3    /* ��������� ��������� �������� � ������� ����������� */
#define   _RSS_OBJECT_LOW_ACTIVE           -4    /* ����� �������� ����������� ������ 6 */
#define   _RSS_OBJECT_MATCH_ACTIVE         -5    /* ����� �������� ����������� ������ 6 */
#define   _RSS_OBJECT_BAD_SCHEME           -6    /* ������������ ����� */
#define   _RSS_OBJECT_COLLISION_EXTERNAL   -7    /* ������������ ��� ������ �������� ����� */
#define   _RSS_OBJECT_COLLISION_INTERNAL   -8    /* ������������ ��� ������ �� �������� ����� */

#define   _RSS_OBJECT_UNKNOWN_ERROR      -999    /* ����������� ������ */

/*------------------------------------- ���� ����� ������� ��������� */

#define   _RSS_ABSOLUTE_TARGET  0   /* �������� ����������� ������������ */ 
#define     _RSS_JOINTS_TARGET  1   /* �������� ���������� � ����������� */
#define     _RSS_OBJECT_TARGET  2   /* �������� ������� ������ ������� ������� */

/*----------------------------------- ��������� �������������� ����� */

#define   _GRD_TO_RAD   0.017453
#define   _RAD_TO_GRD  57.296
#define   _PI           3.1415926

/*---------------------------------------- �������� �������� "�����" */

   typedef struct {
                     double  x ;
                     double  y ;
                     double  z ;

                     double  azim ;
                     double  elev ;
                     double  roll ;

                        int  mark ;

                  } RSS_Point ;

/*----------------------------------- �������� �������� "����������" */

   typedef struct {
                     double   value ;
                     double   value_min ;
                     double   value_max ;
                        int   bounded ;
                     double   spec ;
                        int   type ;
#define                        _L_TYPE    0
#define                        _A_TYPE    1
                        int   fixed ;

                       char   name[64] ;     /* ������������ */
        struct  RSS_Context **contexts ;     /* ������ ����������� ������ ������� */
                  } RSS_Joint ;

/*-------------------- �������� �������� "��������������� ���������" */

   typedef struct {
                     char  object[64] ;   /* �������� ������� */
                     char  link[64] ;     /* �������� ����� */
                     char  body[64] ;     /* �������� ���� */
                     void *ptr ;          /* ������ �� ��������� */
                  } RSS_Morphology ;

/*------------------------- �������� �������� "����������� ��������" */

   typedef struct {
                     char  link[32] ;            /* �����-������ �� �������� */
                     char  section_name[32] ;    /* �������� ������ ���������� */
                     char  parameter_name[32] ;  /* �������� ��������� */
                     char  type[8] ;             /* ��� ��������� */  
                     void *value ;               /* ��������������� �������� */
                  } RSS_ControlPar ;

/*----------------------------------------- �������� ������ "������" */

  class RSS_OBJECT_API RSS_Object {

       public:
                       char   Name[128] ;      /* ��� ������� */
                       char   Type[128] ;      /* ��� ������� */
                       char   Decl[1024] ;     /* �������� ������� */

       struct RSS_Parameter  *Parameters ;     /* ������ ���������� */
                        int   Parameters_cnt ;

                RSS_Feature **Features ;       /* ������ ������� */
                        int   Features_cnt ;

             RSS_Morphology  *Morphology ;     /* ������ ��������������� ��������� */
                        int   Morphology_cnt ;

                        int   ErrorEnable ;    /* ���� ������ ��������� �� ������� */

       public:

   virtual             void  vErrorMessage   (int) ;                /* ���./����. ��������� �� ������� */

   virtual             void  vReadSave       (std::string *) ;      /* ������� ������ �� ������ */
   virtual             void  vWriteSave      (std::string *) ;      /* �������� ������ � ������ */
   virtual             void  vFree           (void) ;               /* ���������� ������� */

   virtual             void  vFormDecl       (void) ;               /* ����������� �������� */

   virtual             void  vEditStructure  (void) ;               /* ������������� ��������� */
   virtual             void  vAddMorphology  (RSS_Morphology *) ;   /* �������� ������� ���������� */
   virtual             void  vEditMorphology (RSS_Morphology *) ;   /* ��������� ������� ���������� */

   virtual              int  vListControlPars(RSS_ControlPar *) ;   /* �������� ������ ���������� ���������� */
   virtual              int  vSetControlPar  (RSS_ControlPar *) ;   /* ���������� �������� ��������� ���������� */

   virtual              int  vGetBasePoint   (RSS_Point *) ;        /* ������ � ������� ������ */
   virtual             void  vSetBasePoint   (RSS_Point *) ;
   virtual              int  vGetTargetPoint (RSS_Point *) ;        /* ������ � ������� ������ */
   virtual             void  vSetTargetPoint (RSS_Point *, int) ;
   virtual              int  vGetJoints      (RSS_Joint *) ;        /* ������ � ������������ */
   virtual             void  vSetJoints      (RSS_Joint *, int) ;
   virtual class RSS_Object *vGetSlice       (void) ;               /* ������ �� ������ ��������� */
   virtual             void  vSetSlice       (class RSS_Object *) ;
   virtual              int  vGetAmbiguity   (void) ;               /* ������ � ������������������ ����� */
   virtual             void  vSetAmbiguity   (char *) ;
   virtual              int  vSolveByTarget  (void) ;               /* ��������� ��������� �� ������� ����� */ 
   virtual              int  vSolveByJoints  (void) ;               /* ��������� ��������� �� ����������� */

   virtual              int  vGetTarget      (char *,               /* ������ ���������� '�������' ����� */
                                              RSS_Point *) ;

   virtual              int  vSpecial        (char *, void *) ;     /* ����������� �������� */

   virtual              int  vCheckFeatures  (void *) ;             /* ��������� ������������ ������� */
   virtual             void  vSetFeature     (RSS_Feature *) ;      /* ������ �� ���������� */
   virtual             void  vGetFeature     (RSS_Feature *) ;

			     RSS_Object      () ;                   /* ����������� */
			    ~RSS_Object      () ;                   /* ���������� */

                                  } ;
/*-------------------------------------------------------------------*/

#endif  // RSS_OBJECT_H
