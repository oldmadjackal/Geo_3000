
/********************************************************************/
/*								    */
/*             ������ ���������� �������� "����� �����"             */
/*								    */
/********************************************************************/

#ifdef O_PSET_EXPORTS
#define O_PSET_API __declspec(dllexport)
#else
#define O_PSET_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*---------------------------- �������� ������ ������� "����� �����" */

typedef struct {
                  RSS_Point  target ;
                  RSS_Joint *joints ;
                       char *object ;
                        int  type ;

               } RSS_Object_PSet_elem ;

  class O_PSET_API RSS_Object_PSet : public RSS_Object {

    public:
                        RSS_Object *mMasterObject ;        /* ������-������ */
                               int  mDegreesCnt ;          /* ����� �������� ����������� ������-������� */
              RSS_Object_PSet_elem *mPointsList ;          /* ������ ����� ������ */
                               int  mPointsCnt ;

    public:
               virtual void  vWriteSave(std::string *) ;   /* �������� ������ � ������ */
               virtual  int  vSpecial  (char *, void *) ;  /* ����������� �������� */

	                     RSS_Object_PSet() ;           /* ����������� */
	                    ~RSS_Object_PSet() ;           /* ���������� */
                                                       } ;

/*---------------- �������� ������ ���������� �������� "����� �����" */

  class O_PSET_API RSS_Module_PSet : public RSS_Kernel {

    public:

     static
      struct RSS_Module_PSet_instr *mInstrList ;            /* ������ ������ */

    public:
     virtual        void  vShow       (char *) ;            /* ���������� ��������� ������ */
     virtual         int  vExecuteCmd (const char *) ;      /* ��������� ������� */
     virtual        void  vReadSave   (std::string *) ;     /* ������ ������ �� ������ */
     virtual        void  vWriteSave  (std::string *) ;     /* �������� ������ � ������ */

    public:
                     int  cHelp       (char *) ;            /* ���������� HELP */ 
                     int  cCreate     (char *) ;            /* ���������� CREATE */ 
                     int  cAdd        (char *) ;            /* ���������� ADD */ 

         RSS_Object_PSet *FindObject  (char *) ;            /* ����� ������� ���� PSET �� ����� */
                     int  CreateObject(char *) ;            /* �������� ������� */ 

    public:
	                  RSS_Module_PSet() ;               /* ����������� */
	                 ~RSS_Module_PSet() ;               /* ���������� */
                                                       } ;

/*-------------------------------------------- ���������� ���������� */

 struct RSS_Module_PSet_instr {

           char                   *name_full ;         /* ������ ��� ������� */
           char                   *name_shrt ;         /* �������� ��� ������� */
           char                   *help_row ;          /* HELP - ������ */
           char                   *help_full ;         /* HELP - ������ */
            int (RSS_Module_PSet::*process)(char *) ;  /* ��������� ���������� ������� */
                              }  ;

/*--------------------------------------------- ���������� ��������� */

/* ����  O_PSet.cpp */

/* ����  O_PSet_dialog.cpp */
  BOOL CALLBACK  Object_PSet_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
