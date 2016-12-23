/********************************************************************/
/*								    */
/*		������ ������������ �����������        	            */
/*								    */
/********************************************************************/

#ifdef C_STD_EXPORTS
#define C_STD_API __declspec(dllexport)
#else
#define C_STD_API __declspec(dllimport)
#endif


/*---------------------------------------------- ��������� ��������� */

/*----------------------------------------------- �������� ��������� */

 typedef struct {  
                   struct L_module  module_decl ; 

                               int  compiled ;
 
                }  RSS_Calc_Std_context ;

/*------------------------- �������� ������ ������������ ����������� */

  class C_STD_API RSS_Calc_Std : public RSS_Kernel {

    public:
     virtual         int  vCalculate  (char *, char *,         /* ��������� ��������� */                                       
                                       struct RSS_Parameter *,
                                       struct RSS_Parameter *,
                                       double *,
                                       void **, char *) ;

    public:
     virtual        void  vStart      (void) ;                 /* ��������� �������� */

                    void  iDebug      (char *, char *, int) ;  /* ���������� ������ � ���� */ 
                    void  iDebug      (char *, char *) ;

	                  RSS_Calc_Std() ;                     /* ����������� */
	                 ~RSS_Calc_Std() ;                     /* ���������� */
                                                   } ;

/*---------------------------------------------------- ��������� �/� */

  void  RSS_Calc_Std_text_norm(char *) ; /* ������������ ������ ��������� */
  char *RSS_Calc_Std_get_text (void) ;   /* ��������� ���������� ������ ������ ����������� */
  char *RSS_Calc_Std_error    (int) ;    /* ������ ������ ������ �� ���� */
  char *RSS_Calc_Std_warning  (int) ;    /* ������ ������ �������������� �� ���� */
  void  RSS_Calc_Std_debug    (int) ;    /* ��������� ������� ����������� */
