
/********************************************************************/
/*								    */
/*		������ ������ "������ �����"    		    */
/*								    */
/********************************************************************/

#ifdef T_POINT_EXPORTS
#define T_POINT_API __declspec(dllexport)
#else
#define T_POINT_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*------------------------------------- �������� ������� ����������� */

  typedef  struct {
                     char  name[128] ;       /* �������� */
                      int  type ;            /* ��� ����������: */
#define                     _CIRCLE_IND   0   /* �������� */
#define                     _SPHERE_IND   1   /* ����������� */
                RSS_Point  point ;           /* ����� ����������(�������) */
                      int  angle_code ;      /* ��� �������������� ���� ��� ��������� ���������� */
                      int *data ;            /* ������������ ������ */
                      int  data_cnt ;

                   double  scan ;            /* ��������� �������� ��������� */
                      int  scan_flag ;       /* ���� ����������� ���������� �������� ��������� */

                     void *context ;         /* �������� �������� */
                      int  idx ;             /* ������ �������� */ 
                     HWND  hWnd ;            /* ���� ���������� */
                      int  dlist1_idx ;      /* ������� ���������� ������� */
                      int  dlist2_idx ;

                  } RSS_Module_Point_Indicator ;

#define  _IND_MAX  128

/*---------------------------- �������� ������ ������ "������ �����" */

  class T_POINT_API RSS_Module_Point : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Point_instr  *mInstrList ;            /* ������ ������ */

         RSS_Module_Point_Indicator *mContextObject ;         /* ������ '�����������' �������� */
                               char  mContextAction[64] ;     /* �������� '�����������' �������� */

                static    RSS_Point   mTarget ;               /* ������� ����� */
                             double   mRoundStep ;            /* ��� ������������ ����� */

                               HWND   mIndListWnd ;           /* ���� ������ ����������� */
         RSS_Module_Point_Indicator  *mIndicators[_IND_MAX] ; /* ������ ����������� */
         RSS_Module_Point_Indicator  *mIndicator_last ;       /* ������ �� ��������� ��������� ��������� */
                           COLORREF   mBadColor ;             /* ���� '�������������' */
                           COLORREF   mGoodAColor ;           /* ����� ����������� */ 
                           COLORREF   mGoodEColor ;
                           COLORREF   mGoodRColor ;
                             double   mIndSize ;              /* ����.������� ���������� */   
				     
    public:
     virtual         int  vExecuteCmd    (const char *) ;     /* ��������� ������� */
     virtual         int  vExecuteCmd    (const char *, RSS_IFace *) ;
     virtual        void  vShow          (char *) ;           /* ���������� ��������� ������ */
     virtual        void  vWriteSave     (std::string *) ;    /* �������� ������ � ������ */
     virtual         int  vGetParameter  (char *, char *) ;   /* �������� �������� */
     virtual        void  vChangeContext (void)  ;            /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp          (char *, RSS_IFace *) ;   /* ���������� HELP */
                     int  cCheck         (char *, RSS_IFace *) ;   /* ���������� CHECK */
                     int  cTarget        (char *, RSS_IFace *) ;   /* ���������� TARGET */
                     int  cTargetInfo    (char *, RSS_IFace *) ;   /* ���������� TARGET_INFO */
                     int  cRound         (char *, RSS_IFace *) ;   /* ���������� ROUND */
                     int  cIndicatorList (char *, RSS_IFace *) ;   /* ���������� INDICATOR_LIST */
                     int  cIndicatorKill (char *, RSS_IFace *) ;   /* ���������� INDICATOR_KILL */

                    void  iIndicatorTop  (RSS_Module_Point_Indicator *) ;
                                                              /* ����������� ���������� � ���.���� */
                    void  iIndicatorShow (RSS_Module_Point_Indicator *) ;
                                                              /* ����������� ���������� �� ����� */
                    void  iIndicatorShow_(RSS_Module_Point_Indicator *) ;
                                                              /* ����������� ���������� � ��������� ��������� */
                    void  iIndicatorHide (RSS_Module_Point_Indicator *) ;
                                                              /* �������� ���������� �� ����� */
                    void  iIndicatorKill (RSS_Module_Point_Indicator *) ;
                                                              /* �������� ���������� */
                    char *iErrorDecode   (int) ;              /* ����������� ���� ������ */

    public:
	                  RSS_Module_Point() ;                /* ����������� */
	                 ~RSS_Module_Point() ;                /* ���������� */
                                                         } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Point_instr {

           char                    *name_full ;                     /* ������ ��� ������� */
           char                    *name_shrt ;                     /* �������� ��� ������� */
           char                    *help_row ;                      /* HELP - ������ */
           char                    *help_full ;                     /* HELP - ������ */
            int (RSS_Module_Point::*process)(char *, RSS_IFace *) ; /* ��������� ���������� ������� */
                               }  ;
/*--------------------------------------- ������� ����������� ������ */

#define  _TASK_POINT_ADD_INDICATOR    1
#define  _TASK_POINT_KILL_INDICATOR   2

/*--------------------------------------------- ���������� ��������� */

/* ����  T_Point.cpp */

/* ����  T_Point_dialog.cpp */
    BOOL CALLBACK  Task_Point_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Point_Round_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Point_IList_dialog      (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Task_Point_RoundIndicator_prc(HWND, UINT, WPARAM, LPARAM) ;