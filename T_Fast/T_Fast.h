
/********************************************************************/
/*								    */
/*      ������ ������ "����������� ���������� �� ��������������"    */
/*								    */
/********************************************************************/

#ifdef T_FAST_EXPORTS
#define T_FAST_API __declspec(dllexport)
#else
#define T_FAST_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*-------------------------------------------------- �������� ������ */

 typedef struct {
                     char  dummy[64] ;         /* �������� ������� */

                } RSS_Module_Fast_Context ;

/*------- �������� ������ "����������� ���������� �� ��������������" */

typedef class RSS_Object_Path  PATH ;

  class T_FAST_API RSS_Module_Fast : public RSS_Kernel {

    public:
              class RSS_Module_Path *mModulePath ;                     /* ������ �� ������ Path */

    static
       struct RSS_Module_Fast_instr *mInstrList ;                       /* ������ ������ */

                               char  mContextAction[64] ;               /* �������� '�����������' �������� */
                         TRAJECTORY *mContextObject ;                   /* ������ '�����������' �������� */

                               char  mLineCheck_cmd[512] ;              /* ������� �������� ���������� �� �������������� ������������ */
                               char  mTimeCalc_cmd[512] ;               /* ������� ���������� ������� ����������� ���������� */
                               char  mLineDraw_cmd[512] ;               /* ������� ��������� ���������� */

                                int  mSimplification_opt ;              /* ����������� ����� ������������ �������� �� ������� */
                                int  mSimplification_method ;
#define                              _SEQUENCE_METHOD   0
#define                                _RANDOM_METHOD   1
#define                                  _FULL_METHOD   2
                                int  mSmoothing_opt ;                   /* ���������� �������� ������� */
                                int  mCutSegment_opt ;                  /* ��������� �������� */
                             double  mCutSegment_MinTime ;              /* ����������� ����� ����������� �������� �������� */

                                int  mDebug_level ;                     /* ������� ���������� ������ */

    public:
             virtual     void  vInit          (void) ;                  /* ������������� ������ */
             virtual      int  vExecuteCmd    (const char *) ;          /* ��������� ������� */
             virtual      int  vExecuteCmd    (const char *, RSS_IFace *) ;
             virtual     void  vShow          (char *) ;                /* ���������� ��������� ������ */
             virtual     void  vChangeContext (void)  ;                 /* ��������� �������� � ��������� ������ */
             virtual     void  vReadSave      (std::string *) ;         /* ������ ������ �� ������ */
             virtual     void  vWriteSave     (std::string *) ;         /* �������� ������ � ������ */
             virtual      int  vReadData      (RSS_Context ***,         /* ������� ������ ��������� ������ �� ������ */
                                               std::string *   ) ;
             virtual     void  vWriteData     (RSS_Context *,           /* �������� ������ ��������� ������ � ������ */
                                               std::string * ) ;
             virtual      int  vGetParameter  (char *, char *) ;        /* �������� �������� */
                    
    public:         
                          int  cHelp          (char *, RSS_IFace *) ;   /* ���������� HELP */ 
                          int  cConfig        (char *, RSS_IFace *) ;   /* ���������� CONFIG */ 
                          int  cOptimize      (char *, RSS_IFace *) ;   /* ���������� OPTIMIZE */ 

                         void  iDebug         (char *, char *) ;        /* ���������� ������ � ���� */ 
                         PATH *iCopyPath      (PATH *, char *) ;        /* ����������� ���������� */
                         void  iExecute       (char *, RSS_IFace *) ;   /* ���������� ������� */

                         void  iText_subst    (char *, char *, char *); /* ���������� ��������� ����������� */

                          int  iMethodSimplif (PATH **,   PATH **,      /* ����������� ����� ����������� ����� */
                                                int * ,    int * ,      /*   ������������ �� ������� ��������  */
                                               RSS_IFace *, double *  ) ;
                          int  iMethodCut     (PATH **,   PATH **,      /* ����������� ����� ��������� �������� */
                                                int * ,    int * ,
                                               RSS_IFace *, double *  ) ;

    public:
  	                       RSS_Module_Fast() ;                 /* ����������� */
	                      ~RSS_Module_Fast() ;                 /* ���������� */
                                                           } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Fast_instr {

            char                   *name_full ;                     /* ������ ��� ������� */
            char                   *name_shrt ;                     /* �������� ��� ������� */
            char                   *help_row ;                      /* HELP - ������ */
            char                   *help_full ;                     /* HELP - ������ */
             int (RSS_Module_Fast::*process)(char *, RSS_IFace *) ; /* ��������� ���������� ������� */
                              }  ;
/*---------------------------------------------- ����������� ������� */


/*--------------------------------------------- ���������� ��������� */

/* ����  T_Fast.cpp */

/* ����  T_Fast_dialog.cpp */
    BOOL CALLBACK  Task_Fast_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Fast_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
