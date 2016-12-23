/********************************************************************/
/*								    */
/*	������ ���������� ������������ ����������� ��������	    */
/*								    */
/********************************************************************/

#ifdef F_CROSS_EXPORTS
#define F_CROSS_API __declspec(dllexport)
#else
#define F_CROSS_API __declspec(dllimport)
#endif

/*---------------------------------------------- ��������� ��������� */

/*---------------- �������� ������ ���������� �������� "�����������" */

  class F_CROSS_API RSS_Module_Cross : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Cross_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* ��������� ������� */

    public:
                     int  cHelp         (char *) ;          /* ���������� Help */ 
                     int  cAssign       (char *) ;          /* ���������� Assign */ 
                     int  cExclude      (char *) ;          /* ���������� Exclude */ 

    public:
     virtual        void  vStart        (void) ;            /* ��������� �������� */
     virtual RSS_Feature *vCreateFeature(RSS_Object *) ;    /* ������� �������� */
     virtual        void  vWriteSave    (std::string *) ;   /* �������� ������ � ������ */
     virtual        void  vReadSave     (std::string *) ;   /* ������� ������ �� ������ */

	                  RSS_Module_Cross() ;              /* ����������� */
	                 ~RSS_Module_Cross() ;              /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Cross_instr {

           char                    *name_full ;         /* ������ ��� ������� */
           char                    *name_shrt ;         /* �������� ��� ������� */
           char                    *help_row ;          /* HELP - ������ */
           char                    *help_full ;         /* HELP - ������ */
            int (RSS_Module_Cross::*process)(char *) ;  /* ��������� ���������� ������� */
                               }  ;

/*---------------------------------------------- ���������� �������� */

 typedef struct {                                            /* �������� ����������� �������������� */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Cross_Dim ;

 typedef struct {                                            /* �������� ������� �������������� */
                             double  x_min ;
                }  RSS_Feature_Cross_Matrix ;

 typedef struct {                                            /* �������� ����� */
                    double  x ;                               /* X - �������� */
                      char *x_formula ;                       /* X - ���������� ��������� */
                      void *x_calculate ;                     /* X - �������� ����������� */

                    double  y ;                               /* Y - �������� */
                      char *y_formula ;                       /* Y - ���������� ��������� */
                      void *y_calculate ;                     /* Y - �������� ����������� */

                    double  z ;                               /* Z - �������� */
                      char *z_formula ;                       /* Z - ���������� ��������� */
                      void *z_calculate ;                     /* Z - �������� ����������� */

                    double  x_abs ;                           /* ���������� � ���������� �� */
                    double  y_abs ;
                    double  z_abs ;
                }  RSS_Feature_Cross_Vertex ;

#define  _VERTEX_PER_FACET_MAX   16

 typedef struct {                                            /* �������� ����� */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* ������ ����� */
                       int  vertexes_cnt ;

     RSS_Feature_Cross_Dim  overall ;                         /* ������� ����� */

  RSS_Feature_Cross_Matrix  in_facet_matrix ;                 /* ������� �������������� � ������� ��������� ����� */
                       int  in_facet_matrix_done ;            /* ���� �������� �������... */
                }  RSS_Feature_Cross_Facet ;

 typedef struct {                                            /* �������� ���� */
                               char  name[128] ;              /* �������� */
               struct RSS_Parameter *extrn_pars ;             /* ������ �� ��������� */

           RSS_Feature_Cross_Vertex *Vertexes ;               /* ������ ������ */
                                int  Vertexes_cnt ; 
            RSS_Feature_Cross_Facet *Facets ;                 /* ������ ������ */
                                int  Facets_cnt ;

              RSS_Feature_Cross_Dim  overall ;                /* ������� ���� */

                                int  recalc ;                 /* ���� ������������� ����������� ����� ���� */

                             double  x_base ;                 /* ���������� ������� ����� */
                             double  y_base ;
                             double  z_base  ;
                             double  x_base_s ;
                             double  y_base_s ;
                             double  z_base_s ;

                             double  a_azim ;                 /* ���� ���������� */
                             double  a_elev ;
                             double  a_roll  ;
                             double  a_azim_s ;
                             double  a_elev_s ;
                             double  a_roll_s ;

                             double  Matrix[4][4] ;           /* ������� ��������� ����� */
                                int  Matrix_flag ;            /* ���� �������������... */
      
                                int  list_idx ;               /* ������ ����������� ������ */
                }  RSS_Feature_Cross_Body ;

 typedef struct {                                            /* ������� ������ ���������� ��������� */
                   char  object[128] ;                        /* ��� ������� */
                   char  link[128] ;                          /* ��� ����� */
                   char  body[128] ;                          /* ��� ���� */
                   char  desc[256] ;                          /* ����� ����� */
                   void *pair ;                               /* ������ �� ������ ������� ������ */
                    int  execute ;                            /* ���� ����������/������������� �������� */
                    int  active ;                             /* ���� ���������� */

                    int  work ;                               

                }  RSS_Feature_Cross_Mark ;

/*--------------------------- �������� ������ �������� "�����������" */

  class F_CROSS_API RSS_Feature_Cross : public RSS_Feature {

    public:       
             RSS_Feature_Cross_Body  *Bodies ;                 /* ������ ��� */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

              RSS_Feature_Cross_Dim   overall ;                /* ������� ������� */

     static  RSS_Feature_Cross_Mark **marks_list ;             /* ������ ���������� ��������� */
     static                     int   marks_list_cnt ; 

    public:
            virtual void  vReadSave     (char *, std::string *,     /* ������� ������ �� ������ */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* ������ ���������� � �������� */
            virtual  int  vCheck        (void *) ;                  /* �������� ������������������ �������� */  
            virtual void  vBodyDelete   (char *) ;                  /* ������� ���� */
            virtual void  vBodyBasePoint(char *,                    /* ������� ������� ����� ���� */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* ������� ���������� ���� */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* ������� ������� ��������� ���� */
            virtual void  vBodyShifts   (char *,                    /* ������� �������� ��������� �  */
                                          double, double, double,   /*    ���������� ����            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* ������� ������ ���������� */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                    /* ���������� ����� ��� ������� */
              RSS_Kernel *iGetCalculator(void) ;                    /* ����������� ������� ����������� */
 static
  RSS_Feature_Cross_Mark *iSetMark      (RSS_Feature_Cross_Mark *,  /* �������� ���� ��������� ������ ���������� �������� ����������� */ 
                                         RSS_Feature_Cross_Mark * ) ;
 static              int  iGetMarkEx    (char *,                    /* ��������� ������ ���������� �������� ����������� */ 
                                         RSS_Feature_Cross_Mark **, int) ;
 static              int  iGetMarkIn    (char *,                    /* ��������� ������ ���������� �������� ����������� */ 
                                         RSS_Feature_Cross_Mark **, int) ;
                     int  iMarkForCheck (char *, char *) ;          /* �������� ���������� ������� ��� ��� ������ ��� �������� */
                     int  iMaskCheck    (char *, 
                                         RSS_Feature_Cross_Mark *) ;
                     int  iOverallTest  (RSS_Feature_Cross_Dim *,   /* �������� ����������� ��������� */
                                         RSS_Feature_Cross_Dim * ) ;

	                  RSS_Feature_Cross() ;                     /* ����������� */
	                 ~RSS_Feature_Cross() ;                     /* ���������� */
                                                           } ;

/*--------------------------------------------- ���������� ��������� */

/* ����  F_Cross.cpp */

/* ����  F_Cross_dialog.cpp */
  BOOL CALLBACK  Feature_Cross_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  BOOL CALLBACK  Feature_Cross_Assign_dialog(HWND, UINT, WPARAM, LPARAM) ;
