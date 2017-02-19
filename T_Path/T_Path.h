
/********************************************************************/
/*								    */
/*		������ ������ "����� ������� ����������" 	    */
/*								    */
/********************************************************************/

#ifdef T_PATH_EXPORTS
#define T_PATH_API __declspec(dllexport)
#else
#define T_PATH_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*---------------------------------------------- �������� ���������� */

 struct RSS_Module_Path_Trajectory {

                     char   name[64] ;            /* �������� ���������� */
                     char   desc[512] ;           /* ��������� */
                   double  *Path ;                /* ������ ����� ������� ���������� */
                   double  *Vectors ;             /* ������ �������� ������� ���������� */
                      int   Path_vectors ;        /* ����� �������� � ���������� */
               RSS_Object  *Path_object ;         /* ������ �� ������, ��� �������� ��������� ���������� */
                RSS_Joint   Path_degrees[50] ;    /* �������� �������� ����������� �������������� ������� */
                      int   Path_frame ;          /* ������ '�����' ���������� */

                RSS_Point  *Trace ;               /* ����� ����� ����� ����������� */
                      int   Trace_cnt ;
                 COLORREF   TraceColor ;          /* ���� ����� ���������� */

    class RSS_Module_Path  *module ;              /* �������� �������� */
                      int   dlist1_idx ;          /* ������� ���������� ������� */

       struct RSS_Context **contexts ;            /* ������ ����������� ������ ������� */

                                   }  ;

/*----------------------------- �������� ������ ������� "����������" */

  class T_PATH_API RSS_Object_Path : public RSS_Object {

    public:
        RSS_Module_Path_Trajectory *Trajectory ;         /* ������ �� ��������� */

    public:
               virtual void  vFree     (void) ;          /* ���������� ������� */
               virtual void  vWriteSave(std::string *) ; /* �������� ������ � ������ */

	                     RSS_Object_Path() ;         /* ����������� */
	                    ~RSS_Object_Path() ;         /* ���������� */
                                                       } ;

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*------------------------ �������� ������ ������ "����� ����������" */

  class T_PATH_API RSS_Module_Path : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Path_instr *mInstrList ;            /* ������ ������ */

         RSS_Module_Path_Trajectory *mContextObject ;        /* ������ '�����������' �������� */
                               char  mContextAction[64] ;    /* �������� '�����������' �������� */

                                int  mDraw ;                 /* ������������ � �������� ������ */

                                int  mRandomPerDirect ;      /* ����� ��������� �������� �� ���� ������������ */ 
                                int  mVectorsMax ;           /* ������������ ����� ������� �������� */ 
                                int  mVectorsMax_ ;

                             double  mLineStep_user ;        /* ��� �� �������� �����������, �������� ������������� */ 
                             double  mAngleStep_user ;       /* ��� �� ������� �����������, �������� ������������� */ 
                             double  mLineStep ;             /* ��� �� �������� �����������, ������� */ 
                             double  mAngleStep ;            /* ��� �� ������� �����������, ������� */ 
 
                             double  mLineAcc_user ;         /* �������� ����������� �������� ��������� */ 
                             double  mAngleAcc_user ;        /* �������� ����������� ������� ��������� */ 
                             double  mLineAcc ;              /* �������� ����������� �������� ��������� */ 
                             double  mAngleAcc ;             /* �������� ����������� ������� ��������� */ 
                             double  mScanStepCoef ;         /* �������� ���� ��������� */
                                int  mFailDirect ;           /* ���������� ����� ��������� �������� ������ */ 

                             double *mPath ;                 /* ������ ����� ������� ���������� */
                             double *mVectors ;              /* ������ �������� ������� ���������� */
                                int  mPath_vectors ;         /* ����� �������� � ���������� */
                         RSS_Object *mPath_object ;          /* ������ �� ������������� ����� */
                          RSS_Joint  mPath_degrees[50] ;     /* �������� �������� ����������� �������������� ������� */
                                int  mPath_frame ;           /* ������ '�����' ���������� */

                                int  mCheck ;                /* ���� �������� ���������������� ������� */
                                int  mTraceTop ;             /* ���� ���������� ���������� ������� ����� ������� */
                          RSS_Point *mTracePoints ;          /* ����� ����� ����� ����������� */
                                int  mTracePoints_cnt ;
                                int  mTracePoints_max ;
                           COLORREF  mTraceColor ;           /* ���� ����� ���������� */

                                int  mDebug_level ;          /* ������� ���������� ������ */
                       static  long  mDebug_stamp ;          /* ��������� ����� */

    public:
     virtual         int  vExecuteCmd     (const char *) ;      /* ��������� ������� */
     virtual         int  vExecuteCmd     (const char *, RSS_IFace *) ;
     virtual        void  vShow           (char *) ;            /* ���������� ��������� ������ */
     virtual        void  vReadSave       (std::string *) ;     /* ������ ������ �� ������ */
     virtual        void  vWriteSave      (std::string *) ;     /* �������� ������ � ������ */
     virtual         int  vGetParameter   (char *, char *) ;    /* �������� �������� */
     virtual        void  vChangeContext  (void)  ;             /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp           (char *, RSS_IFace *) ;   /* ���������� HELP */ 
                     int  cConfig         (char *, RSS_IFace *) ;   /* ���������� CONFIG */ 
                     int  cSearch         (char *, RSS_IFace *) ;   /* ���������� SEARCH */ 
                     int  cTrace          (char *, RSS_IFace *) ;   /* ���������� TRACE */ 
                     int  cIndicate       (char *, RSS_IFace *) ;   /* ���������� INDICATE */ 
                     int  cList           (char *, RSS_IFace *) ;   /* ���������� LIST */ 
                     int  cUnit           (char *, RSS_IFace *) ;   /* ���������� UNIT */ 
                     int  cCopy           (char *, RSS_IFace *) ;   /* ���������� COPY */
                     int  cColor          (char *, RSS_IFace *) ;   /* ���������� COLOR */

                     int  iSearchDirect   (RSS_Joint *) ;           /* ����� ���� �� ��������� ������������ */
                     int  iSearchReach    (RSS_Point *) ;           /* ����� ���� �� ������� ����� */
                    void  iReducePath     (void) ;                  /* ��������� ���������� */
                     int  iShowPath       (void) ;                  /* �������� ���������� */
                    void  iReverse        (TRAJECTORY *) ;          /* ������ ���������� */
                    void  iGenerateVector (double *) ;              /* ��������� ���������� ������� */
         virtual     int  iCalculateVector(double *,                /* ���������� ������� */
                                           double *, double *) ;
                     int  iAddPathPoint   (double *, double *) ;    /* ���������� ����� � ���������� */
         virtual     int  iTraceVector    (double *, double *,      /* ����������� ������� ����� �������������� */
                                           double *, double *) ;
                     int  iReachVector    (RSS_Point *, int);       /* ����������� ������� �������� � ������� ����� */
         virtual    void  iCulculateSteps (void) ;                  /* ����������� ����� ������������ */
                     int  iOptimiseStep   (RSS_Point *,             /* ���������� �������������� ������������ */
                                           RSS_Point *, double *) ;
                  double  iCalcRange      (RSS_Point *,             /* ���������� '�������' ��������� */
                                           RSS_Point *, double, double) ;
                    void  iOrtsDif        (RSS_Point *,             /* ���������� ������� ������� ����� ������ ���������� */ 
                                           RSS_Point *, double *, double *, double *) ;
                  double  iAngleDif       (double) ;                /* ���������� �������� ����� */
                     int  iSaveTracePoint (RSS_Point *) ;           /* ���������� ����� ���������� */
                    void  iTopTraceShow   (TRAJECTORY *) ;          /* ����������� ���������� �� ����� */
                    void  iTopTraceShow_  (TRAJECTORY *) ;          /* ����������� ���������� � ��������� ��������� */
                    void  iTraceHide      (TRAJECTORY *) ;          /* �������� ���������� �� ����� */

                    void  iDebug          (char *, char *, int) ;   /* ���������� ������ � ���� */ 
                    void  iDebug          (char *, char *) ;

    public:
	                  RSS_Module_Path() ;                       /* ����������� */
	                 ~RSS_Module_Path() ;                       /* ���������� */
                                                         } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Path_instr {

            char                   *name_full ;                     /* ������ ��� ������� */
            char                   *name_shrt ;                     /* �������� ��� ������� */
            char                   *help_row ;                      /* HELP - ������ */
            char                   *help_full ;                     /* HELP - ������ */
             int (RSS_Module_Path::*process)(char *, RSS_IFace *) ; /* ��������� ���������� ������� */
                              }  ;
/*--------------------------------------------- ���������� ��������� */

/* ����  T_Path.cpp */

/* ����  T_Path_dialog.cpp */
    BOOL CALLBACK  Task_Path_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Path_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Path_Save_dialog  (HWND, UINT, WPARAM, LPARAM) ;
