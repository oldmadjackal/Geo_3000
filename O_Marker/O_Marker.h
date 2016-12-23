
/********************************************************************/
/*								    */
/*		������ ���������� �������� "������"  		    */
/*								    */
/********************************************************************/

#ifdef O_MARKER_EXPORTS
#define O_MARKER_API __declspec(dllexport)
#else
#define O_MARKER_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*--------------------------------- �������� ������ ������� "������" */

  class O_MARKER_API RSS_Object_Marker : public RSS_Object {

    public:
                    double  x_base ;                    /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                    /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                      char  model[64] ;                 /* ������ ������� */
                  COLORREF  color ;                     /* ���� ������ */
                       int  dlist1_idx ;                /* ������� ���������� ������� */
                       int  dlist2_idx ;
                    double  size ;                      /* ����.������� ������� */   

    public:
               virtual  int  vGetBasePoint   (RSS_Point *) ;         /* ������ � ������� ������ */
               virtual  int  vGetTarget      (char *, RSS_Point *) ; /* ������ ���������� '�������' ����� */
               virtual void  vWriteSave      (std::string *) ;       /* �������� ������ � ������ */
               virtual  int  vListControlPars(RSS_ControlPar *) ;    /* �������� ������ ���������� ���������� */
               virtual  int  vSetControlPar  (RSS_ControlPar *) ;    /* ���������� �������� ��������� ���������� */

	                     RSS_Object_Marker() ;                   /* ����������� */
	                    ~RSS_Object_Marker() ;                   /* ���������� */
                                                           } ;

/*--------------------- �������� ������ ���������� �������� "������" */

  class O_MARKER_API RSS_Module_Marker : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Marker_instr *mInstrList ;           /* ������ ������ */

                   RSS_Object_Marker *mContextObject ;       /* ������ '�����������' �������� */
                                char  mContextAction[64] ;   /* �������� '�����������' �������� */
                          
                              double  mSize ;                /* ����.������� ������� */   

    public:
     virtual        void  vShow          (char *) ;          /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd    (const char *) ;    /* ��������� ������� */
     virtual        void  vReadSave      (std::string *) ;   /* ������ ������ �� ������ */
     virtual        void  vWriteSave     (std::string *) ;   /* �������� ������ � ������ */
     virtual        void  vChangeContext (void)  ;           /* ��������� �������� � ��������� ������ */

    public:
                     int  cHelp       (char *) ;             /* ���������� HELP */ 
                     int  cCreate     (char *) ;             /* ���������� CREATE */ 
                     int  cInfo       (char *) ;             /* ���������� INFO */ 
                     int  cBase       (char *) ;             /* ���������� BASE */ 
                     int  cAngle      (char *) ;             /* ���������� ANGLE */ 

       RSS_Object_Marker *FindObject  (char *) ;             /* ����� ������� ���� MARKER �� ����� */
                     int  CreateObject(RSS_Model_data *);    /* �������� ������� */ 
                     int  FormMarker  (RSS_Object_Marker *); /* ������� ����� ������� */
                     int  FormMarker_ (RSS_Object_Marker *); /* ������� ����� ������� � ��������� ��������� */
                     int  PlaceMarker (RSS_Object_Marker *); /* ���������������� ������� */
                     int  PlaceMarker_(RSS_Object_Marker *); /* ���������������� ������� � ��������� ��������� */

    public:
	                  RSS_Module_Marker() ;              /* ����������� */
	                 ~RSS_Module_Marker() ;              /* ���������� */
                                                           } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Marker_instr {

           char                     *name_full ;         /* ������ ��� ������� */
           char                     *name_shrt ;         /* �������� ��� ������� */
           char                     *help_row ;          /* HELP - ������ */
           char                     *help_full ;         /* HELP - ������ */
            int (RSS_Module_Marker::*process)(char *) ;  /* ��������� ���������� ������� */
                                }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Marker.cpp */

/* ����  O_Marker_dialog.cpp */
  BOOL CALLBACK  Object_Marker_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
