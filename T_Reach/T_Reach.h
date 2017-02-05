
/********************************************************************/
/*								    */
/*		������ ������ "����� � �����"    		    */
/*								    */
/********************************************************************/

#ifdef T_REACH_EXPORTS
#define T_REACH_API __declspec(dllexport)
#else
#define T_REACH_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*--------------------------- �������� ������ ������ "����� � �����" */

  class T_REACH_API RSS_Module_Reach : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Reach_instr *mInstrList ;            /* ������ ������ */

                static    RSS_Point  mTarget ;               /* ������� ����� */

                             double  mLineAcc ;              /* �������� ����������� �������� ��������� */ 
                             double  mAngleAcc ;             /* �������� ����������� ������� ��������� */ 
                                int  mResetStart ;           /* ���� ������������� ��������� ����� ������     */
			                                     /*   �� �������� ���������� �������� ����������� */
                                int  mCheckRanges ;          /* �������� ����������� � �������� ����������� */
                                int  mDraw ;                 /* ������������ � �������� ������ */
                             double  mScanStepCoef ;         /* �������� ���� ��������� */
                             double  mJampRangeCoef ;        /* ����.����� ������� ������� */
                                int  mFailMax ;              /* ���������� ����� ��������� �������� ������ */ 

    public:
     virtual         int  vExecuteCmd    (const char *) ;     /* ��������� ������� */
     virtual        void  vWriteSave     (std::string *) ;    /* �������� ������ � ������ */

    public:
                     int  cHelp          (char *) ;           /* ���������� HELP */ 
                     int  cConfig        (char *) ;           /* ���������� CONFIG */ 
                     int  cTarget        (char *) ;           /* ���������� TARGET */ 
                     int  cTargetInfo    (char *) ;           /* ���������� TARGET_INFO */ 
                     int  cRotate        (char *) ;           /* ���������� ROTATE */ 

                    char *iErrorDecode   (int) ;              /* ����������� ���� ������ */
                    void  iGenerateVector(double *, int) ;    /* ��������� ���������� ������� */
                  double  iCalcRange     (RSS_Point *,        /* ���������� '�������' ��������� */
                                          RSS_Point *,
                                               double, double) ;
                  double  iAngleDif      (double) ;           /* ���������� �������� ����� */

    public:
	                  RSS_Module_Reach() ;                /* ����������� */
	                 ~RSS_Module_Reach() ;                /* ���������� */
                                                         } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Reach_instr {

           char                    *name_full ;         /* ������ ��� ������� */
           char                    *name_shrt ;         /* �������� ��� ������� */
           char                    *help_row ;          /* HELP - ������ */
           char                    *help_full ;         /* HELP - ������ */
            int (RSS_Module_Reach::*process)(char *) ;  /* ��������� ���������� ������� */
                               }  ;
/*--------------------------------------------- ���������� ��������� */

/* ����  T_Reach.cpp */

/* ����  T_Reach_dialog.cpp */
    BOOL CALLBACK  Task_Reach_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Reach_Config_dialog     (HWND, UINT, WPARAM, LPARAM) ;
