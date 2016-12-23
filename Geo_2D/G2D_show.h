/*********************************************************************/
/*                                                                   */
/*               ������� ������������ ����������                     */
/*                                                                   */
/*********************************************************************/

#include "gl\gl.h"
#include "gl\glu.h"

/*--------------------------------------------- ��������� ���������� */

 typedef  struct {
                      char  name[256] ;
                      HWND  hWnd ;
                     HGLRC  hOpenGL ;                 
                    double  Look_x ;
                    double  Look_y ;
                    double  Look_z ;
                    double  Look_azim ;
                    double  Look_elev ;
                    double  Look_roll ;
                    double  Zoom ;
                      char  AtObject[256] ;
                    
                 }  WndContext ;

/*-------------------------------------------------------- ��������� */

    int  G2D_show_init       (void) ;            /* ������������� ������� ����������� */
    int  G2D_show_getlist    (int) ;             /* �������������� ������ ����������� ������ */
   void  G2D_show_releaselist(int) ;             /* ������������ ������ ����������� ������ */
    int  G2D_first_context   (char *) ;          /* ���������� ������ �������� */
    int  G2D_next_context    (char *) ;          /* ���������� ��������� �������� */
   void  G2D_show_context    (char *) ;          /* ���������� ������� �������� */
 double  G2D_get_context     (char *) ;          /* ������ �������� ��������� */
   void  G2D_read_context    (std::string *) ;   /* ������� ��������� ��������� */
   void  G2D_write_context   (std::string *) ;   /* �������� ��������� ��������� */
    int  G2D_redraw          (char *) ;          /* ������������ ���� */
    int  G2D_look            (char *, char *,    /* ������ � ������ ������ ������ */
                               double *, double *, double *,
                               double *, double *, double * ) ;
    int  G2D_zoom            (char *, char *,    /* ������ � ����� ������ ������ */
                                    double * ) ;
    int  G2D_lookat          (char *, char *,    /* ������ � ������ ���������� ������ */ 
                                      char * ) ;
    int  G2D_lookat_point    (WndContext *) ;    /* ��������� ����� ���������� */

/*------------------------------------------------------ ����������� */

   LRESULT CALLBACK  G2D_show_prc(HWND, UINT, WPARAM, LPARAM) ;
