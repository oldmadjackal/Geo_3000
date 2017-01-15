
/********************************************************************/
/*								    */
/*		������ ������ "������ �����������"    		    */
/*								    */
/********************************************************************/

#ifdef T_AREA_EXPORTS
#define T_AREA_API __declspec(dllexport)
#else
#define T_AREA_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */


/*------------------------------------- �������� ������� ����������� */

#define       _ANGLES_MAX    5
#define  _ANGLES_LIST_MAX    8
#define          _IND_MAX  128

  typedef  struct {                     /* �������� �������� ��������� */
                     double  a_lst[_ANGLES_MAX] ;
                        int  a_lst_cnt ;
                     double  a_min ;
                     double  a_max ;
                       char  a_lst_text[128] ;
                       char  a_min_text[32] ;
                       char  a_max_text[32] ;
                        int  type ;
#define                        _INLIST_RANGE     0
#define                            _OR_RANGE     1
#define                           _AND_RANGE     2
                        int  base ;
#define                      _ABSOLUTE_VALUE     0
#define                        _NORMAL_VALUE     1
                  } RSS_Module_Area_Range ;

  typedef  struct {                     /* ���������� ������.�������� ��������� */
      RSS_Module_Area_Range *range ;                       
                     double *angle ;
                        int  idx ;
                        int  idx_max ;
                        int  type ;
//#define                          _OR_RANGE     1
//#define                         _AND_RANGE     2
                  } RSS_Module_Area_Scan ;

  typedef  struct {                     /* �������� 3D �������� ������� */
                        int  use_flag ;
                        int  done_flag ;
                   COLORREF  color ;

      RSS_Module_Area_Range  azim_range ;
      RSS_Module_Area_Range  elev_range ;
      RSS_Module_Area_Range  roll_range ;

                        int  priority_type ;
#define                        _A_E_R_PRIO     0
#define                        _A_R_E_PRIO     1
#define                        _E_A_R_PRIO     2
#define                        _E_R_A_PRIO     3
#define                        _R_A_E_PRIO     4
#define                        _R_E_A_PRIO     5

       RSS_Module_Area_Scan  scan[3] ;

                  } RSS_Module_Area_Angles ;

  typedef  struct {
                     char  name[128] ;                 /* �������� */
                      int  type ;                      /* ��� ����������: */
#define                       _FLAT_IND   0             /* ������� */
#define                   _CYLINDER_IND   1             /* �������������� */
                RSS_Point  base ;                      /* ������� ����� */
                   double  size1 ;                     /* ��������� ��������� */
                   double  size2 ;
                   double  cell ;                      /* ������ ������ */

   RSS_Module_Area_Angles  angles[_ANGLES_LIST_MAX] ;  /* ������� ������� 3D-�������� */
                   double  angle_step ;                /* ��� ����������� ����� */
                      int  angles_idx ;                /* ������ �������� 3D-������a */

                 COLORREF *data ;                      /* ������������ ������ */
                      int  data_cnt ;

//                    int  cnt_1 ;

                      int  move_flag ;       /* ���� ���������� ���������� �������� ��������� */
                      int  formed_flag ;     /* ���� ��������������� ���������� */

    class RSS_Module_Area *context ;         /* �������� �������� */
                      int  idx ;             /* ������ �������� */
                      int  dlist1_idx ;      /* ������� ���������� ������� */
                      int  dlist2_idx ;

                  } RSS_Module_Area_Indicator ;

/*-------------------- �������� ������ ������� "����������� �������" */

  class T_AREA_API RSS_Object_Area : public RSS_Object {

    public:
        RSS_Module_Area_Indicator *Indicator ;           /* ������ �� ��������� */

    public:
               virtual void  vFree     (void) ;          /* ���������� ������� */
               virtual void  vWriteSave(std::string *) ; /* �������� ������ � ������ */

	                     RSS_Object_Area() ;         /* ����������� */
	                    ~RSS_Object_Area() ;         /* ���������� */
                                                       } ;

/*---------------------- �������� ������ ������ "������ �����������" */

  class T_AREA_API RSS_Module_Area : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Area_instr  *mInstrList ;            /* ������ ������ */

          RSS_Module_Area_Indicator  *mContextObject ;        /* ������ '�����������' �������� */
                               char   mContextAction[64] ;    /* �������� '�����������' �������� */

                               HWND   mIndListWnd ;           /* ���� ������ ����������� */
          RSS_Module_Area_Indicator  *mIndicators[_IND_MAX] ; /* ������ ����������� */
          RSS_Module_Area_Indicator  *mIndicator_last ;       /* ������ �� ��������� ��������� ��������� */
                           COLORREF   mBadColor ;             /* ���� '�������������' */
                           COLORREF   mGoodColor ;            /* ���� ����������� */ 
                           COLORREF   mScanColor ;            /* ���� '����������' */ 

                                int   mNoShow ;               /* ���� ���������� ��������� �� ����� */
                             double   mRange ;                /* ��������� �������� */
				     
    public:
     virtual   int  vExecuteCmd     (const char *) ;  /* ��������� ������� */
     virtual  void  vShow           (char *) ;        /* ���������� ��������� ������ */
     virtual  void  vReadSave       (std::string *) ; /* ������ ������ �� ������ */
     virtual  void  vWriteSave      (std::string *) ; /* �������� ������ � ������ */
     virtual  void  vChangeContext  (void)  ;         /* ��������� �������� � ��������� ������ */

    public:
               int  cHelp           (char *) ;        /* ���������� HELP */
               int  cCreate         (char *) ;        /* ���������� CREATE */ 
               int  cCopy           (char *) ;        /* ���������� COPY */
               int  cBase           (char *) ;        /* ���������� BASE */
               int  cAngle          (char *) ;        /* ���������� ANGLE */
               int  cSize           (char *) ;        /* ���������� SIZE */
               int  cTable          (char *) ;        /* ���������� TABLE */
               int  cExecute        (char *) ;        /* ���������� EXECUTE */
               int  cIndicatorList  (char *) ;        /* ���������� INDICATOR_LIST */
               int  cIndicatorKill  (char *) ;        /* ���������� INDICATOR_KILL */

#define  INDICATOR  RSS_Module_Area_Indicator

         INDICATOR *FindIndicator   (char *) ;        /* ����� ���������� �� ����� */
              void  iIndicatorShow  (INDICATOR *) ;   /* ����������� ���������� �� ����� */
              void  iIndicatorShow_ (INDICATOR *) ;   /* ����������� ���������� � ��������� ��������� */
              void  iIndicatorHide  (INDICATOR *) ;   /* �������� ���������� �� ����� */
              void  iIndicatorKill  (INDICATOR *) ;   /* �������� ���������� */
               int  iIndicatorAlloc (INDICATOR *) ;   /* ���������� �������� ���������� */
              char *iErrorDecode    (int) ;           /* ����������� ���� ������ */
               int  iReadTableFile  (INDICATOR *,
                                          char * ) ;
               int  iGetNextPoint   (INDICATOR *,
                                      int, RSS_Point *,
                                      int *, COLORREF *, int) ;
              void  iPrepareAngles  (INDICATOR *, 
                                     RSS_Point * ) ;
              void  iResetAngles    (INDICATOR *) ;
               int  iAnalisePrevious(INDICATOR *, int,
                                      COLORREF *      ) ;
              void  iGetNextAngle   (INDICATOR *) ;

#undef  INDICATOR

    public:
	                  RSS_Module_Area() ;                 /* ����������� */
	                 ~RSS_Module_Area() ;                 /* ���������� */
                                                         } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Area_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Area::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;
/*--------------------------------------- ������� ����������� ������ */

#define  _TASK_AREA_ADD_INDICATOR    1
#define  _TASK_AREA_KILL_INDICATOR   2

/*--------------------------------------------- ���������� ��������� */

/* ����  T_Area.cpp */
       RSS_Kernel *GetTaskAreaEntry(void) ;

/* ����  T_Area_dialog.cpp */
    BOOL CALLBACK  Task_Area_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Area_IList_dialog (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Area_Angles_dialog(HWND, UINT, WPARAM, LPARAM) ;

/* ����  T_Area_color.cpp */
         COLORREF  Task_Area_color_choice (COLORREF) ;
         COLORREF  Task_Area_set_color    (COLORREF) ;
    BOOL CALLBACK  Task_Area_color_dialog (HWND, UINT, WPARAM, LPARAM) ;

