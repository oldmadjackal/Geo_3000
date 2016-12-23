/********************************************************************/
/*								    */
/*      ������ ������ "������ ��������� ����������"                 */
/*								    */
/********************************************************************/

#ifdef T_SCAN_EXPORTS
#define T_SCAN_API __declspec(dllexport)
#else
#define T_SCAN_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

#define     _SCAN_PARS_MAX   20      /* ������������ ����� ���������� ������������ */
#define       _FORMULA_LEN  128      /* ������������ ����� �������, �������� */

/*--------------------------------------------- ������� ������������ */

 typedef struct {                                    /* �������� ���������� �������� */
                     char  formula[_FORMULA_LEN] ;
                   double  value ;
                      int  fixed ;
                } RSS_Module_Scan_value ;

 typedef struct {                         /* �������� ������� ������������ */
                     char  name[64] ;      /* ��� ��������� */
                      int  category ;      /* ��������� ���������: */
#define                     _VAR_PAR   0    /* ���������� */
#define                  _OBJECT_PAR   1    /* �������� ������� */
                      int  kind ;          /* ��� ���������: */
#define                    _SCAN_PAR   0    /* ��������� �������� */
#define                   _SLAVE_PAR   1    /* ��������� �������� */

    RSS_Module_Scan_value  value ;         /* ������� �������� */
    RSS_Module_Scan_value  value_min ;     /* �������� ��������� */
    RSS_Module_Scan_value  value_max ;
    RSS_Module_Scan_value  value_step ;    /* ��� ��������� */
                      int  step ;          /* ����� ��������� */ 

                     char  object[128] ;   /* ��� ������� (��� _OBJECT_PAR) */ 
                     char  par_link[32] ;  /* ������ �� �������� ������� (��� _OBJECT_PAR) */

    class RSS_Module_Scan *module ;        /* ������ �� ������������ ������ */ 

                } RSS_Module_Scan_par ;

#define  ZONE_PAR  RSS_Module_Scan_par

/*------------------------------------------- ��������� ������������ */

 typedef  union { double  f_value ;                  /* �������� �������� ��������� */
                    long  l_value ;  }  p_value ;

 typedef struct {                                    /* �������� ��������� ����� */
                   p_value *pars ;                    /* ����� ���������� */
                    double  result ;                  /* "�������" */
                       int  error_flag ;              /* ������� ������ */
                } RSS_Module_Scan_result ;

 typedef struct {                                    /* �������� ������� ���������� */  
                       char  model[64] ;              /* ������ ������� */
                        int  dlist_idx ;              /* ������ ����������� ������ */
                     double  size ;                   /* ����.������� ������� */   
                   COLORREF  color_min ;              /* �������� ��������� �������� */
                   COLORREF  color_max ;
                } RSS_Module_Scan_marker ;

 typedef struct {                                    /* �������� ��������� ������� ����� */  
                    RSS_Point  point ;                /* ������ ������� */
                          int  dlist_idx ;            /* ������ ����������� ������ */
                       double  result_min ;           /* �������� ��������� ���������� */
                       double  result_max ;
                          int  good ;                 /* ���� ��������� ���������� */ 
                          int  error ;                /* ���� ������ */
                          int  draw ;
                } RSS_Module_Scan_basicp ;

/*-------------------------------------------------- �������� ������ */

 typedef struct {
                     char  dummy[64] ;

                } RSS_Module_Scan_Context ;

/*-------------------- �������� ������ "������ ��������� ����������" */

  class T_SCAN_API RSS_Module_Scan : public RSS_Kernel {

    public:

    static
       struct RSS_Module_Scan_instr *mInstrList ;                        /* ������ ������ */

                               char  mContextAction[64] ;                /* �������� '�����������' �������� */
             RSS_Module_Scan_basicp *mContextObject ;                    /* ������ '�����������' �������� */

                               char  mCheck_cmd[512] ;                   /* ������� ����������� ������������� */
                                int  mBasePoints_show ;                  /* ���� ����������� ������� ����� � ������� ������������ */
                                int  mBasePoints_redraw ;                /* ���� ����������� ������� ����� ����� ������� ���� */

                           ZONE_PAR  mScanPars[_SCAN_PARS_MAX] ;         /* ������ ���������� ������������ */
                                int  mScanPars_cnt ;

                                int  mDebug_level ;                      /* ������� ���������� ������ */
                               char  mError[1024] ;                      /* ��������� �� ������ */ 
                       static  long  mDebug_stamp ;                      /* ��������� ����� */

             RSS_Module_Scan_result *mResult ;                           /* ����� ����� ����������� */
                            p_value *mResult_pars ;                      /* ����� �������� ���������� */
                               long  mResult_cnt ;                       /* ����� ����� ���������� */
                               long  mResult_max ;
                             double  mResult_value_min ;                 /* �������� ���������� */
                             double  mResult_value_max ;

             RSS_Module_Scan_basicp *mBasics ;                           /* ����� ������������ ������� ����� */
                               long  mBasics_cnt ;                       /* ����� ������������ ������� ����� */

#define                                             _MARKER_MAX   2
             RSS_Module_Scan_marker  mBasics_marker[_MARKER_MAX] ;       /* ������� ������� ����� */

    public:
             virtual     void  vInit           (void) ;                   /* ������������� ������ */
             virtual      int  vExecuteCmd     (const char *) ;           /* ��������� ������� */
             virtual      int  vExecuteCmd     (const char *, RSS_IFace *) ;
             virtual     void  vShow           (char *) ;                 /* ���������� ��������� ������ */
             virtual     void  vChangeContext  (void)  ;                  /* ��������� �������� � ��������� ������ */
             virtual     void  vReadSave       (std::string *) ;          /* ������ ������ �� ������ */
             virtual     void  vWriteSave      (std::string *) ;          /* �������� ������ � ������ */
             virtual      int  vGetParameter   (char *, char *) ;         /* �������� �������� */
                    
    public:         
                          int  cHelp           (char *, RSS_IFace *) ;    /* ���������� HELP */
                          int  cConfig         (char *, RSS_IFace *) ;    /* ���������� CONFIG */
                          int  cRun            (char *, RSS_IFace *) ;    /* ���������� RUN */
                          int  cBases          (char *, RSS_IFace *) ;    /* ���������� ���������� BASES */
                          int  cClear          (char *, RSS_IFace *) ;    /* ���������� ���������� CLEAR */
                          int  cExcel          (char *, RSS_IFace *) ;    /* ���������� EXCEL */

                          int  FormMarker      (void) ;                    /* ������� ����� ������� */
                          int  FormMarker_     (void) ;                    /* ������� ����� ������� � ��������� ��������� */
                          int  PlaceMarker     (RSS_Module_Scan_basicp *); /* ���������������� ������� */
                          int  PlaceMarker_    (RSS_Module_Scan_basicp *); /* ���������������� ������� � ��������� ��������� */

                         void  iDebug          (char *, char *) ;         /* ���������� ������ � ���� */
                         void  iExecute        (char *, RSS_IFace *) ;    /* ���������� ������� */
                          int  iCalcScanValue  (RSS_Module_Scan_par *) ;  /* ���.�������� ��� "������������" ��������� */
                          int  iCalcSlaveValue (RSS_Module_Scan_par *) ;  /* ���.�������� ��� ���������� ��������� */
                          int  iCalcValue      (RSS_Module_Scan_value *); /* ���.�������� */
                          int  iCalcGetValue   (RSS_Module_Scan_par *,    /* ������ �������� ��������� ��� ���������� */
                                                           p_value * ) ;
                          int  iCalcTextValue  (RSS_Module_Scan_par *,    /* ������ �������� ��������� � ��������� ���� */
                                                   p_value *, char * ) ;
                          int  iSaveBinResult  (char *, char *, int) ;    /* ���������� ������������ ����� ���������� */
                          int  iSaveCsvResult  (char *) ;                 /* ���������� CSV-����� ���������� */
                          int  iFormExcelResult(char *) ;                 /* �������� ���������� � MS Excel */
                       double  iColorMix       (double, double, double) ; /* ������ ��������� ����������� */ 

    public:
  	                       RSS_Module_Scan() ;                 /* ����������� */
	                      ~RSS_Module_Scan() ;                 /* ���������� */
                                                           } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Scan_instr {

            char                   *name_full ;                      /* ������ ��� ������� */
            char                   *name_shrt ;                      /* �������� ��� ������� */
            char                   *help_row ;                       /* HELP - ������ */
            char                   *help_full ;                      /* HELP - ������ */
             int (RSS_Module_Scan::*process)(char *, RSS_IFace *) ;  /* ��������� ���������� ������� */
                              }  ;
/*---------------------------------------------- ����������� ������� */


/*--------------------------------------------- ���������� ��������� */

/* ����  T_Scan.cpp */
   RSS_Kernel *GetTaskScanEntry(void) ;    /* ��������� ������ �� ����������� ������ */

/* ����  T_Scan_dialog.cpp */
    BOOL CALLBACK  Task_Scan_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Scan_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Scan_Param_dialog (HWND, UINT, WPARAM, LPARAM) ;

/* ����  T_Scan_color.cpp */
         COLORREF  Task_Scan_color_choice (COLORREF) ;
         COLORREF  Task_Scan_set_color    (COLORREF) ;
    BOOL CALLBACK  Task_Scan_color_dialog (HWND, UINT, WPARAM, LPARAM) ;
