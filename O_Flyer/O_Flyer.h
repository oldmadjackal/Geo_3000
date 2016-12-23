
/********************************************************************/
/*								    */
/*                  ������ ���������� ��������        		    */
/*                   "��������" ��������� ���                       */
/*								    */
/********************************************************************/

#ifdef O_FLYER_EXPORTS
#define O_FLYER_API __declspec(dllexport)
#else
#define O_FLYER_API __declspec(dllimport)
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
//                         char  scheme[256] ;             /* ������������ ����� */
                  } RSS_Module_Flyer_Create_data ;

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

                  } RSS_Object_Flyer_Body ;

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

                  } RSS_Object_Flyer_Mass ;

  typedef  struct {
                       int   use_flag ;             /* ���� ������������� */
                      char   name[256] ;            /* �������� ����� */

                    double   value ;                /* �������� */
                       int   value_fixed ;          /* ���� ����������� */
                       int   value_keys ;           /* ����� ����������� ������ */
                    double   value_min ;            /* �������� ������� */
                    double   value_max ;
                       int   value_test ;           /* ���� ������������ */

                       int   action ;

                    double   x_base ;               /* ���������� ������� ����� */
                    double   y_base ;
                    double   z_base ;

                    double   a_azim ;               /* ���� ���������� */
                    double   a_elev ;
                    double   a_roll ;
      
     RSS_Object_Flyer_Body   bodies[_BODIES_MAX] ;  /* ������ ��������� ��� */
     RSS_Object_Flyer_Mass   masses[_BODIES_MAX] ;  /* ������ ��������� ���� */

                RSS_Object  *parent ;               /* ������ �� ���������� ������ */

        struct RSS_Context **contexts ;             /* ������ ����������� ������ ������� */

                  } RSS_Object_Flyer_Link ;

/*------------------------------------------ �������� ������ ������� */

//#define   _LINKS_MAX   8

  class O_FLYER_API RSS_Object_Flyer : public RSS_Object {

    public:
                    double  x_base ;                       /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                       /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                      char  scheme[256] ;                  /* ������������ ����� */

     RSS_Object_Flyer_Link  link_base ;                    /* ����� - ���� */
     RSS_Object_Flyer_Link  link_x ;
     RSS_Object_Flyer_Link  link_y ;
     RSS_Object_Flyer_Link  link_z ;
     RSS_Object_Flyer_Link  link_azim ;
     RSS_Object_Flyer_Link  link_elev ;
     RSS_Object_Flyer_Link  link_roll ;
     RSS_Object_Flyer_Link  link_head ;                    /* ����� - ����� */

                 RSS_Point  target ;                       /* ������� ����� */

    public:
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

      RSS_Object_Flyer_Link *iGetLinkByName (char *) ;        /* ������ ����� ������� �� ����� */
      RSS_Object_Flyer_Body *iGetBodyByName (char *, char *); /* ������ ���� ����� �� ����� */
      RSS_Object_Flyer_Mass *iGetMassByName (char *, char *); /* ������ ����� ����� �� ����� */
                                            
                       void  iClearUpdates  (void) ;          /* ����� ������ ��������� */
                        int  iLinkFeatures  (RSS_Object_Flyer_Link *) ; 
                                                              /* ������� ������� ����� */
                       void  iRobotSkeleton (void) ;          /* ������� ����������� ��������� ������������ */ 
                       void  iLinkSkeleton  (RSS_Object_Flyer_Link *, Matrix2d *) ;
                                                              /* ������� ����������� ��������� ����� */ 
                        int  iRobotDegrees  (int) ;           /* ������������ �������� ����������� ������������ �� ���������� */ 
                        int  iLinkDegrees   (RSS_Object_Flyer_Link *,
                                             char *) ;        /* ������������ �������� ����������� ����� �� ���������� */ 
                        int  iAngleTest     (double,          /* �������� ������������ ���� ��������� */
                                             double, double) ;

                       void  iDebug         (char *,          /* ���������� ������ � ���� */ 
                                             char *, int) ;   
                       void  iDebug         (char *, char *) ;
  
	                     RSS_Object_Flyer() ;             /* ����������� */
	                    ~RSS_Object_Flyer() ;             /* ���������� */
                                                         } ;

/*------------------------------ �������� ������ ���������� �������� */

  class O_FLYER_API RSS_Module_Flyer : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Flyer_instr *mInstrList ;                  /* ������ ������ */

    public:
     virtual        void  vStart         (void) ;                  /* ��������� �������� */
     virtual         int  vExecuteCmd    (const char *) ;          /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;          /* ������ ������ �� ������ */
     virtual        void  vWriteSave     (std::string *) ;         /* �������� ������ � ������ */

    public:
                     int  cHelp          (char *) ;                /* ���������� HELP */
                     int  cCreate        (char *) ;                /* ���������� CREATE */
                     int  cInfo          (char *) ;                /* ���������� INFO */
                     int  cBase          (char *) ;                /* ���������� BASE */
                     int  cAngle         (char *) ;                /* ���������� ANGLE */
                     int  cDegree        (char *) ;                /* ���������� DEGREE */
                     int  cLink          (char *) ;                /* ���������� LINK */
                     int  cJoint         (char *) ;                /* ���������� JOINT */
                     int  cRange         (char *) ;                /* ���������� RANGE */
                     int  cKeys          (char *) ;                /* ���������� KEYS */
                     int  cBody          (char *) ;                /* ���������� BODY */
                     int  cBodyAdd       (char *) ;                /* ���������� BODY_ADD */
                     int  cBodyModel     (char *) ;                /* ���������� BODY_MODEL */
                     int  cBodyShift     (char *) ;                /* ���������� BODY_SHIFT */
                     int  cBodyZero      (char *) ;                /* ���������� BODY_ZERO */
                     int  cMass          (char *) ;                /* ���������� MASS */
                     int  cMassAdd       (char *) ;                /* ���������� MASS_ADD */
                     int  cMassShift     (char *) ;                /* ���������� MASS_SHIFT */
                     int  cMassMove      (char *) ;                /* ���������� MASS_MOVE */

        RSS_Object_Flyer *FindObject     (char *) ;                /* ����� ������� ���� FLYER �� ����� */
                     int  CreateObject   (RSS_Module_Flyer_Create_data *) ;
                                                                   /* �������� ������� */ 
                     int  CreateBody     (RSS_Object_Flyer      *,
                                          RSS_Object_Flyer_Link *,
                                          RSS_Object_Flyer_Body * ) ;
                                                                   /* �������� ���� */
                     int  CreateMass     (RSS_Object_Flyer      *,
                                          RSS_Object_Flyer_Link *,
                                          RSS_Object_Flyer_Mass * ) ;
                                                                   /* �������� ����� */
                    void  iDebug         (char *, char *, int) ;   /* ���������� ������ � ���� */                                              
                    void  iDebug         (char *, char *) ;

    public:
	                  RSS_Module_Flyer() ;                     /* ����������� */
	                 ~RSS_Module_Flyer() ;                     /* ���������� */
                                                         } ;
/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Flyer_instr {

           char                    *name_full ;         /* ������ ��� ������� */
           char                    *name_shrt ;         /* �������� ��� ������� */
           char                    *help_row ;          /* HELP - ������ */
           char                    *help_full ;         /* HELP - ������ */
            int (RSS_Module_Flyer::*process)(char *) ;  /* ��������� ���������� ������� */
                               }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Flyer.cpp */
   RSS_Kernel *GetObjectFlyerEntry(void) ;    /* ��������� ������ �� ����������� ������ */

/* ����  O_Flyer_dialog.cpp */
    BOOL CALLBACK  Object_Flyer_Help_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Degree_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Range_dialog     (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Keys_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Bodies_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_BodyAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Masses_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_MassAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
