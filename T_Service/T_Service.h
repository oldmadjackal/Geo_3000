
/********************************************************************/
/*								    */
/*		������ ������ "������ ������������"    		    */
/*								    */
/********************************************************************/

#ifdef T_SERVICE_EXPORTS
#define T_SERVICE_API __declspec(dllexport)
#else
#define T_SERVICE_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*---------------------------------------------- ��������� ��������� */

 typedef struct {                           /* �������� */
                   char  action[16] ;
                   char  command[256] ;
                   char  mark[8] ;
                } RSS_Module_Service_frame ;

#define  FRAME  RSS_Module_Service_frame

#define    _ERROR_FRAME  -1
#define     _NEXT_FRAME   0
#define     _EXIT_FRAME   1
#define  _GOTO_UP_FRAME   2
#define  _GOTO_DN_FRAME   3
#define  _JUMP_DN_FRAME   4

 typedef struct {                           /* �������� ���������� */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Service_variable ;

#define  VAR  RSS_Module_Service_variable

/*--------------------- �������� ������ ������ "������ ������������" */

  class T_SERVICE_API RSS_Module_Service : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Service_instr *mInstrList ;              /* ������ ������ */

                    static       FRAME *mScenario ;               /* �������� ������ ������������ */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;              /* ������ ���������� */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

                                  char  mContextAction[64] ;      /* �������� '�����������' �������� */

                    static   RSS_IFace  mExecIFace ;              /* ������������ ����� ���������� �������� */
                    static      double  mExecValue ;              /* �������� ��������� �������� */
                    static         int  mExecError ;              /* ������� ������ �������� */
                    static         int  mExecFail ;               /* ������� ����������� ���������� ���������� ������ */

    public:
     virtual         int  vExecuteCmd   (const char *) ;          /* ��������� ������� */
     virtual         int  vExecuteCmd   (const char *, RSS_IFace *) ;
     virtual        void  vReadSave     (std::string *) ;         /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;         /* �������� ������ � ������ */
     virtual         int  vGetParameter (char *, char *) ;        /* �������� �������� */
     virtual        void  vChangeContext(void)  ;                 /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp         (char *, RSS_IFace *) ;   /* ���������� HELP */
                     int  cList         (char *, RSS_IFace *) ;   /* ���������� LIST */
                     int  cExport       (char *, RSS_IFace *) ;   /* ���������� EXPORT */
                     int  cImport       (char *, RSS_IFace *) ;   /* ���������� IMPORT */
                     int  cRun          (char *, RSS_IFace *) ;   /* ���������� RUN */
                     int  cVariables    (char *, RSS_IFace *) ;   /* ���������� VARIABLES */

                     int  iScenarioEdit (char *, int) ;           /* ��������� �������������� �������� */
                     int  iFrameExecute (FRAME *,                 /* ���������� ���������� ��������� ������ */
                                         FRAME *, char *) ;       
              RSS_Kernel *iGetCalculator(void) ;                  /* ����������� ������� ����������� */
                    void  iGetBinary    (char *, int, int) ;      /* ����� ����� � �������� ���� */

    public:
	                  RSS_Module_Service() ;               /* ����������� */
	                 ~RSS_Module_Service() ;               /* ���������� */
                                                             } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Service_instr {

           char                      *name_full ;                      /* ������ ��� ������� */
           char                      *name_shrt ;                      /* �������� ��� ������� */
           char                      *help_row ;                       /* HELP - ������ */
           char                      *help_full ;                      /* HELP - ������ */
            int (RSS_Module_Service::*process)(char *, RSS_IFace *) ;  /* ��������� ���������� ������� */
                              }  ;
/*--------------------------------------------- ���������� ��������� */

/* ����  T_Service.cpp */

/* ����  T_Service_dialog.cpp */
    BOOL CALLBACK  Task_Service_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Service_Edit_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Service_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;
