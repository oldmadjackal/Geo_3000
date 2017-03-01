
/********************************************************************/
/*								    */
/*          ������ ������ "���������� ���������� ����������"        */
/*								    */
/********************************************************************/

#ifdef T_SPLINE_EXPORTS
#define T_SPLINE_API __declspec(dllexport)
#else
#define T_SPLINE_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

#define     _TIME_MINIMIZE      0
#define        _V_CORRECTION    1

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*------------------------------------------------------- ���������� */

 typedef struct {                           /* ��������� ������ �������� �� ������� */
                   double  a1 ;
                   double  t1 ;
                   double  a2 ;
                   double  t2 ;
                   double  a3 ;
                } RSS_Module_Spline_Segment ;

#define  SEGMENT  RSS_Module_Spline_Segment

 typedef struct {                           /* ������� ������ ���������� */
                    RSS_Point  p0 ;
                    RSS_Point  p1 ;
                          int  rad ;
                          int  mark ;
                } RSS_Module_Spline_Pipe ;

/*-------------------------------------------------- �������� ������ */

 typedef struct {
                     char  name[64] ;         /* �������� ������� */
                   double  v_max ;            /* ����.�������� ������� */
                   double  a_max ;            /* ����.��������� ������� */
                     char  f_err[1024] ;      /* ������� �������� ������� */ 

                      int  err_reset ;        /* ������� ������ ��������� �������� ������ */
                     void *calculate ;        /* �������� ����������� */
                   double  d_err ;            /* �������� ������ */
                   double  v ;                /* �������� ������� */
                   double  a ;                /* ��������� ������� */

   RSS_Module_Spline_Pipe *ErrorPipe ;        /* ������ ������ ������ */
                      int  ErrorPipe_cnt ;
                 COLORREF  ErrorPipeColor ;   /* ���� ������ ������ */
                      int  dlist1_idx ;       /* ������� ���������� ������� */

                } RSS_Module_Spline_Context ;

/*-------- �������� ������ ������ "���������� ���������� ����������" */

typedef class RSS_Object_Path  PATH ;

  class T_SPLINE_API RSS_Module_Spline : public RSS_Kernel {

    public:
              class RSS_Module_Path *mModulePath ;                     /* ������ �� ������ Path */
                          RSS_Joint  mDrives[50] ;                     /* �������� �������� ����������� �������������� ������� */
                                int  mDrives_cnt ;

    static
     struct RSS_Module_Spline_instr *mInstrList ;                       /* ������ ������ */

                               char  mContextAction[64] ;               /* �������� '�����������' �������� */
                         TRAJECTORY *mContextObject ;                   /* ������ '�����������' �������� */

                                int  mTurnZone_coef ;                   /* ������������� ����� ���� �������� */
                             double  mLineStep ;                        /* ��� �� �������� ����������� */ 
                             double  mAngleStep ;                       /* ��� �� ������� ����������� */ 

                             double  mAccuracy[32] ;                    /* ����������� �������� �������� */

                                int  mShowErrorPipe ;                   /* ���������� ������ ������ ��� ��� */
                                int  mAdjustErrorPipe ;                 /* ������������ ������ ������ ��� ��� */
                                int  mSaveGlided ;                      /* ��������� ��� ��� ���������� ���������� */
                                int  mIndicate ;                        /* ���������� ���������� � �������� ��� ��� */

             RSS_Module_Spline_Pipe *mErrorPipe ;                       /* ����� ����� ������ ������ */
                                int  mErrorPipe_cnt ;
                                int  mErrorPipe_max ;
                           COLORREF  mErrorPipeColor ;                  /* ���� ����� ���������� */

                                int  mAttempt ;                         /* ������� "�������" */

                                int  mDebug_level ;                     /* ������� ���������� ������ */
                       static  long  mDebug_stamp ;                     /* ��������� ����� */

    public:
             virtual     void  vInit          (void) ;                  /* ������������� ������ */
             virtual      int  vExecuteCmd    (const char *) ;          /* ��������� ������� */
             virtual      int  vExecuteCmd    (const char *, RSS_IFace *) ;
             virtual     void  vShow          (char *) ;                /* ���������� ��������� ������ */
             virtual     void  vReadSave       (std::string *) ;        /* ������ ������ �� ������ */
             virtual     void  vWriteSave      (std::string *) ;        /* �������� ������ � ������ */
             virtual     void  vChangeContext (void)  ;                 /* ��������� �������� � ��������� ������ */
             virtual      int  vReadData      (RSS_Context ***,         /* ������� ������ ��������� ������ �� ������ */
                                               std::string *   ) ;
             virtual     void  vWriteData     (RSS_Context *,           /* �������� ������ ��������� ������ � ������ */
                                               std::string * ) ;
             virtual     void  vReleaseData   (RSS_Context *) ;         /* ���������� ������� ������ ��������� ������ */
             virtual      int  vGetParameter  (char *, char *) ;        /* �������� �������� */
                   
    public:         
                          int  cHelp          (char *, RSS_IFace *) ;   /* ���������� HELP */ 
                          int  cConfig        (char *, RSS_IFace *) ;   /* ���������� CONFIG */ 
                          int  cDrive         (char *, RSS_IFace *) ;   /* ���������� DRIVE */ 
                          int  cGlide         (char *, RSS_IFace *) ;   /* ���������� GLIDE */ 
                          int  cTrace         (char *, RSS_IFace *) ;   /* ���������� TRACE */ 

                          int  iDrawPath      (PATH *, RSS_Object *);   /* ��������� ���������� */
                          int  iCheckPath     (PATH *) ;                /* ����������� ����������� ���������� */
                         PATH *iCopyPath      (PATH *, char * ) ;       /* ����������� ���������� */
                          int  iTurnZone_check(RSS_Object_Path *,       /* ����������� ���������� ���������� ��� */
                                               RSS_Object_Path * ) ;  
                          int  iLineSegment   (int, double *, double *, /* ���������� �������� �� �������� ������� */
                                                    double *,
                                                    double  , double *,
                                                   SEGMENT *, double *,
                                                 RSS_Module_Spline_Context **) ;  
                          int  iTurnSegment   (int, double *, double *, /* ���������� �������� �� ��������� ������ �������� */
                                                    double *, double *,
                                                    double  , double *,
                                                   SEGMENT *, double *,
                                                 RSS_Module_Spline_Context **) ;  
                          int  iTraceSegment  (int, double  , double *, /* �������� �� ������� ���������� */
                                                    double *, SEGMENT *,
                                                 RSS_Module_Spline_Context **,
                                                                RSS_Object *  ) ;
                          int  iSaveErrorPipe (int, RSS_Point *,        /* ���������� ����� ������ ������ */ 
                                                    RSS_Point * ) ;
                          int  iCutErrorPipe  (int) ;                   /* ��������� ������ ������ �� ������� �������� */
                         void  iErrorPipeShow_(TRAJECTORY *) ;          /* ����������� ������ ������ */
                         void  iErrorPipeShow (TRAJECTORY *) ;
                                                                        /* ������ � �������� ������ ������� */
                          int  iErrorCalc     (RSS_Module_Spline_Context *,  
                                                               char * ) ;
                       double  iAngleDif      (double) ;                /* ���������� �������� ����� */
                         void  iDebug         (char *, char *, int) ;   /* ���������� ������ � ���� */ 
                         void  iDebug         (char *, char *) ;

    public:
  	                       RSS_Module_Spline() ;                 /* ����������� */
	                      ~RSS_Module_Spline() ;                 /* ���������� */
                                                           } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Spline_instr {

            char                     *name_full ;                      /* ������ ��� ������� */
            char                     *name_shrt ;                      /* �������� ��� ������� */
            char                     *help_row ;                       /* HELP - ������ */
            char                     *help_full ;                      /* HELP - ������ */
             int (RSS_Module_Spline::*process)(char *, RSS_IFace *) ;  /* ��������� ���������� ������� */
                                }  ;
/*---------------------------------------------- ����������� ������� */

#define   _FATAL_ERROR   -1    /* ����������� ������ */
#define    _PATH_FAIL    -2    /* �������� ���������� */
#define    _PIPE_FAIL    -3    /* �������� ������ ������ */

/*--------------------------------------------- ���������� ��������� */

/* ����  T_Spline.cpp */

/* ����  T_Spline_dialog.cpp */
    BOOL CALLBACK  Task_Spline_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Spline_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Spline_Drives_dialog(HWND, UINT, WPARAM, LPARAM) ;
