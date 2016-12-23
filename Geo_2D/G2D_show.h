/*********************************************************************/
/*                                                                   */
/*               СИСТЕМА ГРАФИЧЕСКОГО ИНТЕРФЕЙСА                     */
/*                                                                   */
/*********************************************************************/

#include "gl\gl.h"
#include "gl\glu.h"

/*--------------------------------------------- Контексты Рендеринга */

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

/*-------------------------------------------------------- Интерфейс */

    int  G2D_show_init       (void) ;            /* Инициализация системы отображения */
    int  G2D_show_getlist    (int) ;             /* Резервирование номера дисплейного списка */
   void  G2D_show_releaselist(int) ;             /* Освобождение номера дисплейного списка */
    int  G2D_first_context   (char *) ;          /* Установить первый контекст */
    int  G2D_next_context    (char *) ;          /* Установить следующий контекст */
   void  G2D_show_context    (char *) ;          /* Отобразить текущий контекст */
 double  G2D_get_context     (char *) ;          /* Выдать параметр контекста */
   void  G2D_read_context    (std::string *) ;   /* Считать параметры контекста */
   void  G2D_write_context   (std::string *) ;   /* Записать параметры контекста */
    int  G2D_redraw          (char *) ;          /* Перерисовать окно */
    int  G2D_look            (char *, char *,    /* Работа с точкой зрения камеры */
                               double *, double *, double *,
                               double *, double *, double * ) ;
    int  G2D_zoom            (char *, char *,    /* Работа с полем зрения камеры */
                                    double * ) ;
    int  G2D_lookat          (char *, char *,    /* Работа с точкой наблюдения камеры */ 
                                      char * ) ;
    int  G2D_lookat_point    (WndContext *) ;    /* Обработка точки наблюдения */

/*------------------------------------------------------ Обработчики */

   LRESULT CALLBACK  G2D_show_prc(HWND, UINT, WPARAM, LPARAM) ;
