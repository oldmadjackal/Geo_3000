
/********************************************************************/
/*								    */
/*		������ ���������� �������� "����"  		    */
/*								    */
/********************************************************************/

#ifdef O_BODY_EXPORTS
#define O_BODY_API __declspec(dllexport)
#else
#define O_BODY_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------------------- �������� ������ ������� "����" */

  class O_BODY_API RSS_Object_Body : public RSS_Object {

    public:
                    double  x_base ;                          /* ���������� ������� ����� */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                          /* ���� ���������� */
                    double  a_elev ;
                    double  a_roll ;

                      char  model_path[FILENAME_MAX] ;        /* ���� ������ */

    public:
               virtual  int  vGetTarget     (char *,          /* ������ ���������� '�������' ����� */
                                             RSS_Point *) ;
               virtual void  vWriteSave     (std::string *) ; /* �������� ������ � ������ */

	                     RSS_Object_Body() ;              /* ����������� */
	                    ~RSS_Object_Body() ;              /* ���������� */
                                                       } ;

/*----------------------- �������� ������ ���������� �������� "����" */

  class O_BODY_API RSS_Module_Body : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Body_instr *mInstrList ;          /* ������ ������ */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* ��������� ������� */
     virtual        void  vReadSave     (std::string *) ;   /* ������ ������ �� ������ */
     virtual        void  vWriteSave    (std::string *) ;   /* �������� ������ � ������ */

    public:
                     int  cHelp         (char *) ;          /* ���������� HELP */ 
                     int  cCreate       (char *) ;          /* ���������� CREATE */ 
                     int  cInfo         (char *) ;          /* ���������� INFO */ 
                     int  cVisible      (char *) ;          /* ���������� VISIBLE */
                     int  cBase         (char *) ;          /* ���������� BASE */ 
                     int  cAngle        (char *) ;          /* ���������� ANGLE */ 

         RSS_Object_Body *FindObject    (char *) ;          /* ����� ������� ���� BODY �� ����� */
                     int  CreateObject  (RSS_Model_data *); /* �������� ������� */ 

    public:
	                  RSS_Module_Body() ;               /* ����������� */
	                 ~RSS_Module_Body() ;               /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_Body_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_Body::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_Body.cpp */

/* ����  O_Body_dialog.cpp */
  BOOL CALLBACK  Object_Body_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  BOOL CALLBACK  Object_Body_Create_dialog(HWND, UINT, WPARAM, LPARAM) ;
