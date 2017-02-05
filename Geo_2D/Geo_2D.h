
#ifdef  __GEO_2D_MAIN__
#define   _EXTERNAL
#else
#define   _EXTERNAL   extern
#endif

/*------------------------------------ ������� ���������� ���������� */

  _EXTERNAL  char  __cwd[FILENAME_MAX] ;    /* ������� ���������� */

  _EXTERNAL  char  __direct_command[128] ;  /* �������� ������ ������� ���������� */
  _EXTERNAL  char  __object_def[32] ;       /* ������ �� ��������� */

#define    _KEY_QUIT   1001

/*------------------------------------------ ������������� ��������� */

  _EXTERNAL  double  __robot_step_joint ;  /* ���� ��������� �������� ����������� �������� */
  _EXTERNAL  double  __robot_step_link ;

/*------------------------- �������� ������ ���������� ����� ������� */

  class RSS_Module_Main : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Main_instr *mInstrList ;          /* ������ ������ */

    public:
     virtual         int  vExecuteCmd   (const char *) ;        /* ��������� ������� */
     virtual         int  vGetParameter (char *, char *) ;      /* �������� �������� */
     virtual        void  vProcess      (void) ;                /* ��������� ������� ���������� */
     virtual        void  vShow         (char *) ;              /* ���������� ��������� ������ */
     virtual        void  vReadSave     (std::string *) ;       /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;       /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;              /* ���������� HELP */
                     int  cShowScene    (char *) ;              /* ���������� SHOWSCENE */
                     int  cKill         (char *) ;              /* ���������� KILL */
                     int  cAll          (char *) ;              /* ���������� ALL */
                     int  cLookInfo     (char *) ;              /* ���������� LOOKINFO */
                     int  cLookPoint    (char *) ;              /* ���������� EYE */ 
                     int  cLookDirection(char *) ;              /* ���������� LOOK */ 
                     int  cLookAt       (char *) ;              /* ���������� LOOKAT */ 
                     int  cLookZoom     (char *) ;              /* ���������� ZOOM */
                     int  cRobotStep    (char *) ;              /* ���������� ROBOTSTEP */
                     int  cRead         (char *) ;              /* ���������� READ */
                     int  cWrite        (char *) ;              /* ���������� WRITE */
                     int  cThreads      (char *) ;              /* ���������� THREADS */
                     int  cStop         (char *) ;              /* ���������� STOP */
                     int  cModules      (char *) ;              /* ���������� MODULES */
                     int  cMemory       (char *) ;              /* ���������� MEMORY */
                     int  cSrand        (char *) ;              /* ���������� SRAND */

                    void  ShowExecute   (char *) ;              /* ����������� ������ */

                    void  iDebug        (char *, char *, int) ; /* ���������� ������ � ���� */ 
                    void  iDebug        (char *, char *) ;

    public:
	                  RSS_Module_Main() ;                   /* ����������� */
	                 ~RSS_Module_Main() ;                   /* ���������� */
                                            } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Main_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Main::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*-------------------------------------------------------- ��������� */

/* Geo_2D.cpp */
      int  G2D_system           (void) ;                  /* ��������� ��������� ��������� */
      int  G2D_error            (char *, int) ;           /* ��������� ��������� �� ������� */
      int  G2D_command_start    (char *) ;                /* ������ ������ */
      int  G2D_command_processor(char *) ;                /* ��������� ������ */
      int  G2D_command_read     (char **) ;               /* ���������� ���������� ����� */
      int  G2D_command_write    (char *) ;                /* ������ ���������� ����� */
     void  G2D_command_normalise(char *) ;                /* ������������ ��������� ������ */

     void  iDebug               (char *, char *, int) ;   /* ���������� ������ � ���� */ 
     void  iDebug               (char *, char *) ;

/* G2D_dialog.cpp */
    BOOL CALLBACK  Main_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� HELP */
    BOOL CALLBACK  Main_ObjectsList_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� OBJECTS LIST */

/* G2D_monitor.cpp */
      int  G2D_threads(char *, char *) ;      /* ���������� ������� ������� */ 
      int  G2D_modules(char *) ;              /* ����������� ������ �������������� ������� */ 
    BOOL CALLBACK  Main_Threads_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� THREADS */
    BOOL CALLBACK  Main_Modules_dialog(HWND, UINT, WPARAM, LPARAM) ;   /* ���������� ���� MODULES */
