
/********************************************************************/
/*								    */
/*                  ������ ���������� ��������        		    */
/*      ������� ����������� �� ����������� ��������� �              */
/*            2-� ��������� ���������������� ������                 */
/*								    */
/********************************************************************/

#ifdef O_PIVOT_EXPORTS
#define O_PIVOT_API __declspec(dllexport)
#else
#define O_PIVOT_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

#define            _NO_KEYS   0
#define            _LR_KEYS   1
#define            _UD_KEYS   2
#define       _CTRL_LR_KEYS   3
#define       _CTRL_UD_KEYS   4
#define         _SH_LR_KEYS   5
#define         _SH_UD_KEYS   6
#define    _SH_CTRL_LR_KEYS   7
#define    _SH_CTRL_UD_KEYS   8

/*--------------------------------------------- ���������� ��������� */

  typedef  struct {
                           char  name[256] ;               /* �������� ������� */
                           char  scheme[256] ;             /* ������������ ����� */
                  } RSS_Module_Pivot_Create_data ;

/*--------------------------------------------------- �������� ����� */

#define   _BODIES_MAX   10

  typedef  struct {
                       int  use_flag ;
                      char  desc[256] ;            /* �������� ���� */
                       int  change ;               /* ���� ������� ��������� � �������� ���� */

            RSS_Model_data  model ;                /* �������� ������ */
      struct RSS_Parameter *pars ;                 /* ������ ���������� */

                    double  x_base ;               /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                    double  zero_size ;            /* 0-����� */       

                  } RSS_Object_Pivot_Body ;

  typedef  struct {
                       int  use_flag ;
                      char  desc[256] ;            /* �������� ���� */
                       int  change ;               /* ���� ������� ��������� � �������� ���� */

                    double  mass ;                 /* ����� */
                    double  Jx  ;                  /* ������� ������� ������� */
                    double  Jy  ;
                    double  Jz  ;

                    double  x_base ;               /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                    double  move_coef ;            /* ����������� ������ ��� ��������� ����� ����� */       

                  } RSS_Object_Pivot_Mass ;

  typedef  struct {
                       int  use_flag ;             /* ���� ������������� */
                      char  name[256] ;            /* �������� ����� */

                    double  joint ;                /* ���� � ������� */
                       int  joint_fixed ;          /* ���� ���������� ������� */
                       int  joint_keys ;           /* ����� ����������� ������ */
                    double  joint_min ;            /* �������� ����� � ������� */
                    double  joint_max ;
                       int  joint_test ;           /* ���� ������������ ���� � ������� �� �������� */

                    double  link ;                 /* ����� ����� */
                       int  link_fixed ;           /* ���� ���������� ����� */
                       int  link_keys ;            /* ����� ����������� ������ */
                    double  link_min ;             /* �������� ����� ����� */
                    double  link_max ;
                       int  link_test ;            /* ���� ������������ ����� ����� �� �������� */

                       int  action ;

                    double  x_base ;               /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;
      
     RSS_Object_Pivot_Body  bodies[_BODIES_MAX] ;  /* ������ ��������� ��� */
     RSS_Object_Pivot_Mass  masses[_BODIES_MAX] ;  /* ������ ��������� ���� */

                RSS_Object *parent ;               /* ������ �� ���������� ������ */
                  } RSS_Object_Pivot_Link ;

/*------------------------------------------ �������� ������ ������� */

#define   _LINKS_MAX   8

  class O_PIVOT_API RSS_Object_Pivot : public RSS_Object {

    public:
                    double  x_base ;                       /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                       /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                      char  scheme[256] ;                  /* ������������ ����� */

     RSS_Object_Pivot_Link  link_base ;                    /* ���� */
     RSS_Object_Pivot_Link  link_leg ;                     /* ��������� ������� */

     RSS_Object_Pivot_Link  link_wrist ;                   /* �������� */
     RSS_Object_Pivot_Link  link_gripper ;                 /* ������ */

     RSS_Object_Pivot_Link  links[_LINKS_MAX] ;            /* ������� ������� */

                 RSS_Point  target ;                       /* ������� ����� */

                       int  ambiguity_done ;               /* ���� ���������� ��������� ���������� ���������������� */
                       int  ambiguity_cnt ;                /* ����� ����������������� ����� */
                       int  wrist_ambiguity ;              /* �������� ���������� ���������������� */
                       int  arm_ambiguity ;

    public:
                        int  DissectScheme  (void) ;          /* �������� ����� */

               virtual  int  vGetTarget     (char *,          /* ������ ���������� '�������' ����� */
                                             RSS_Point *) ;
               virtual  int  vGetTargetPoint(RSS_Point *) ;   /* ������ � ������� ������ */
               virtual void  vSetTargetPoint(RSS_Point *, int) ;
               virtual  int  vGetAmbiguity  (void) ;          /* ������ � ������������������ ����� */
               virtual void  vSetAmbiguity  (char *) ;
               virtual  int  vSolveByTarget (void) ;          /* ��������� ��������� �� ������� ����� */ 
               virtual  int  vSolveByJoints (void) ;          /* ��������� ��������� �� ����������� */
               virtual  int  vGetJoints     (RSS_Joint *) ;   /* ������ � ������������ */
               virtual void  vSetJoints     (RSS_Joint *, int) ;
               virtual void  vWriteSave     (std::string *) ; /* �������� ������ � ������ */

      RSS_Object_Pivot_Link *iGetLinkByName (char *) ;        /* ������ ����� ������� �� ����� */
      RSS_Object_Pivot_Body *iGetBodyByName (char *, char *); /* ������ ���� ����� �� ����� */
      RSS_Object_Pivot_Mass *iGetMassByName (char *, char *); /* ������ ����� ����� �� ����� */
                                            
                       void  iClearUpdates  (void) ;          /* ����� ������ ��������� */
                        int  iLinkFeatures  (RSS_Object_Pivot_Link *) ; 
                                                              /* ������� ������� ����� */
                       void  iRobotSkeleton (void) ;          /* ������� ����������� ��������� ������������ */ 
                       void  iLinkSkeleton  (RSS_Object_Pivot_Link *, Matrix2d *) ;
                                                              /* ������� ����������� ��������� ����� */ 
                        int  iRobotDegrees  (int) ;           /* ������������ �������� ����������� ������������ �� ���������� */ 
                        int  iLinkDegrees   (RSS_Object_Pivot_Link *,
                                             char *) ;        /* ������������ �������� ����������� ����� �� ���������� */ 
                        int  iAngleTest     (double,          /* �������� ������������ ���� ��������� */
                                             double, double) ;

	                     RSS_Object_Pivot() ;             /* ����������� */
	                    ~RSS_Object_Pivot() ;             /* ���������� */
                                                         } ;

/*------------------------------ �������� ������ ���������� �������� */

  class O_PIVOT_API RSS_Module_Pivot : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Pivot_instr *mInstrList ;           /* ������ ������ */

    public:
     virtual        void  vStart         (void) ;           /* ��������� �������� */
     virtual         int  vExecuteCmd    (const char *) ;   /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;   /* ������ ������ �� ������ */
     virtual        void  vWriteSave     (std::string *) ;  /* �������� ������ � ������ */

    public:
                     int  cHelp          (char *) ;         /* ���������� HELP */
                     int  cCreate        (char *) ;         /* ���������� CREATE */
                     int  cInfo          (char *) ;         /* ���������� INFO */
                     int  cCheck         (char *) ;         /* ���������� CHECK */
                     int  cBase          (char *) ;         /* ���������� BASE */
                     int  cAngle         (char *) ;         /* ���������� ANGLE */
                     int  cDegree        (char *) ;         /* ���������� DEGREE */
                     int  cLink          (char *) ;         /* ���������� LINK */
                     int  cJoint         (char *) ;         /* ���������� JOINT */
                     int  cRange         (char *) ;         /* ���������� RANGE */
                     int  cKeys          (char *) ;         /* ���������� KEYS */
                     int  cBody          (char *) ;         /* ���������� BODY */
                     int  cBodyAdd       (char *) ;         /* ���������� BODY_ADD */
                     int  cBodyModel     (char *) ;         /* ���������� BODY_MODEL */
                     int  cBodyShift     (char *) ;         /* ���������� BODY_SHIFT */
                     int  cBodyZero      (char *) ;         /* ���������� BODY_ZERO */
                     int  cMass          (char *) ;         /* ���������� MASS */
                     int  cMassAdd       (char *) ;         /* ���������� MASS_ADD */
                     int  cMassShift     (char *) ;         /* ���������� MASS_SHIFT */
                     int  cMassMove      (char *) ;         /* ���������� MASS_MOVE */

        RSS_Object_Pivot *FindObject     (char *) ;         /* ����� ������� ���� PIVOT �� ����� */
                     int  CreateObject   (RSS_Module_Pivot_Create_data *) ;
                                                            /* �������� ������� */ 
                     int  CreateBody     (RSS_Object_Pivot      *,
                                          RSS_Object_Pivot_Link *,
                                          RSS_Object_Pivot_Body * ) ;
                                                            /* �������� ���� */
                     int  CreateMass     (RSS_Object_Pivot      *,
                                          RSS_Object_Pivot_Link *,
                                          RSS_Object_Pivot_Mass * ) ;
                                                            /* �������� ����� */

                     int  AmbiguityDialog(RSS_Object_Pivot *) ;
                                                            /* ������ ���������� ����������������� ����� */

    public:
	                  RSS_Module_Pivot() ;              /* ����������� */
	                 ~RSS_Module_Pivot() ;              /* ���������� */
                                                         } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Pivot_instr {

           char                    *name_full ;         /* ������ ��� ������� */
           char                    *name_shrt ;         /* �������� ��� ������� */
           char                    *help_row ;          /* HELP - ������ */
           char                    *help_full ;         /* HELP - ������ */
            int (RSS_Module_Pivot::*process)(char *) ;  /* ��������� ���������� ������� */
                               }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Pivot.cpp */
   RSS_Kernel *GetObjectPivotEntry(void) ;    /* ��������� ������ �� ����������� ������ */

/* ����  O_Pivot_dialog.cpp */
    BOOL CALLBACK  Object_Pivot_Help_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Create_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Degree_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Range_dialog     (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Keys_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Bodies_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_BodyAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Masses_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_MassAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Ambiguity_dialog (HWND, UINT, WPARAM, LPARAM) ;

 LRESULT CALLBACK  Object_Pivot_Structure_prc    (HWND, UINT, WPARAM, LPARAM) ;
