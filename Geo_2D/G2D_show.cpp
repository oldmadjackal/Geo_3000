/*********************************************************************/
/*                                                                   */
/*               ������� ������������ ����������                     */
/*                                                                   */
/*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <search.h>
#include <windows.h>

#include "d:\_Projects\_Libraries\matrix.h"

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"
#include "..\RSS_Kernel\RSS_Kernel.h"

#include "Geo_2D.h"
#include "G2D_show.h"

#pragma warning(disable : 4996)


/*--------------------------------------------- ��������� ���������� */

#define  SEND_ERROR(text)      SendMessage(RSS_Kernel::kernel_wnd, WM_USER,  \
                                         (WPARAM)_USER_ERROR_MESSAGE,        \
                                         (LPARAM) text)

/*--------------------------------------------- ��������� ���������� */

#define  _OPENGL_WND_MAX    10

   static  WndContext  g2d_contexts[_OPENGL_WND_MAX] ;

   static         int  g2d_context_idx ;
   static        HWND  g2d_context_hWnd ;
   static         HDC  g2d_context_hDC ;
   static       HGLRC  g2d_context_hOpenGL ;


/*------------------------------------------------ ���������� ������ */

#define                       _DRAW_LIST_MAX   10000
  static  char  g2d_draw_list[_DRAW_LIST_MAX] ;       /* FAT ���������� ������� */
  static   int  g2d_draw_list_max ;


/*********************************************************************/
/*                                                                   */
/*                ������������� ������� �����������                  */

 int  G2D_show_init(void)

{
  static  WNDCLASS  G2D_show_wnd ;
              char  message[1024] ;

/*-------------------------------------------- ������������� Open GL */

/*---------------------------- ����������� ������ ���� UD_Show_panel */

	G2D_show_wnd.lpszClassName="G2D_show_class" ;
	G2D_show_wnd.hInstance    = GetModuleHandle(NULL) ;
	G2D_show_wnd.lpfnWndProc  = G2D_show_prc ;
	G2D_show_wnd.hCursor      = LoadCursor(NULL, IDC_ARROW) ;
	G2D_show_wnd.hIcon        =  NULL ;
	G2D_show_wnd.lpszMenuName =  NULL ;
	G2D_show_wnd.hbrBackground=  NULL ;
	G2D_show_wnd.style        =    0 ;
	G2D_show_wnd.hIcon        =  NULL ;

    if(!RegisterClass(&G2D_show_wnd)) {
          sprintf(message, "G2D_Show register error %d", GetLastError()) ;
        G2D_error(message, MB_ICONERROR) ;
	                                  return(-1) ;
                                      }
/*-------------------------------------------------------------------*/

	return 0;
}


/*********************************************************************/
/*                                                                   */
/*                 ���� ������� �����������                          */

 LRESULT CALLBACK  G2D_show_prc(  HWND  hWnd,     UINT  Msg,
 			        WPARAM  wParam, LPARAM  lParam)
{
                    int  wnd_num ;
                    HDC  hDC ;
  PIXELFORMATDESCRIPTOR  Pfd ;
                    int  PixelFormat ;
                   RECT  wnd_rect ;
                 double  xy_aspect ;
                GLfloat  light_pos[4] ;
               Matrix2d  Sum_Matrix ;
               Matrix2d  Oper_Matrix ;  
               Matrix2d  Point_Matrix ;  
                    int  layer ;
                    int  status ;
                   char  text[1024] ;
                    int  i ;

#define  CONTEXT          g2d_contexts[wnd_num]
#define  OPENGL_CONTEXT   g2d_contexts[wnd_num].hOpenGL

/*--------------------------------------- ����������� ��������� ���� */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* ���� �� ������� ���������� */
     if(g2d_contexts[wnd_num].hWnd==hWnd)  break ;

     if(wnd_num==_OPENGL_WND_MAX) {                                 /* ���� ��� - �������� ��������� ���� */

      for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)
        if(g2d_contexts[wnd_num].hWnd==NULL)  break ;

        if(wnd_num==_OPENGL_WND_MAX) {
               MessageBox(NULL, "G2D_Show: Too many windows openned", 
                                "Fatal Error", MB_ICONSTOP) ;
                                        return(0) ;
                                     }

        if(CONTEXT.hWnd==NULL) {
                            memset(&CONTEXT, 0, sizeof(CONTEXT)) ;
                                    CONTEXT.Look_z   =   5. ;
                                    CONTEXT.Look_azim=-180. ;
                                    CONTEXT.Zoom     =   1. ;
                               }

                                    CONTEXT.hWnd=hWnd ;
                               }
/*---------------------------------------------- ����������� ������� */

/*--------------------------------------------------- ����� �������� */

  switch(Msg) {

/*---------------------------------------------------- �������� ���� */

    case WM_CREATE: {
/*- - - - - - - - - - - - - - - - - - - - - ����������� "�����" ���� */
         if(wnd_num==0) {
                    strcpy(g2d_contexts[wnd_num].name, "MAIN") ;
                        }
/*- - - - - - - - - - - - - - - - - -  �������� ������������ ������� */
           memset(&Pfd, 0, sizeof(Pfd)) ;
 
                   Pfd.nSize     =sizeof(Pfd) ;
                   Pfd.nVersion  = 1 ;
                   Pfd.dwFlags   =PFD_DOUBLEBUFFER   |
                                  PFD_SUPPORT_OPENGL |
                                  PFD_DRAW_TO_WINDOW  ;
                   Pfd.iPixelType=PFD_TYPE_RGBA ;
                   Pfd.cColorBits= 24 ;
                   Pfd.cDepthBits= 32 ;
                   Pfd.iLayerType= 24 ;
/*- - - - - - - - - - - - - - - - - - ��������� ������������ ������� */
                   hDC=            GetDC(hWnd) ;
           PixelFormat=ChoosePixelFormat(hDC, &Pfd) ; 
        if(PixelFormat==0) {
                  sprintf(text, "G2D_Show: Open-GL"
                                " format choose error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                           }

           status=SetPixelFormat(hDC, PixelFormat, &Pfd) ;
        if(status==0) {
                  sprintf(text, "G2D_Show: Open-GL"
                                " format set error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                      }
/*- - - - - - - - - - - - - - - - - - - - �������� Open-GL ��������� */
           OPENGL_CONTEXT=wglCreateContext(hDC) ;
        if(OPENGL_CONTEXT==NULL) {
                  sprintf(text, "G2D_Show: Open-GL"
                                " context create error: %d", GetLastError()) ;
               MessageBox(NULL, text, "Fatal Error", MB_ICONSTOP) ;
                                   break ;
                                 }

                               ReleaseDC(hWnd, hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                         break ;
                    }

/*---------------------------------------------------- �������� ���� */

    case WM_DESTROY: {
                           wglMakeCurrent(NULL, NULL) ;
                         wglDeleteContext(OPENGL_CONTEXT) ;

                               g2d_contexts[wnd_num].hWnd=NULL ;
  
                                            break ;
                     }
/*----------------------------------------------- �������� ��������� */

    case WM_COMMAND:  {
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - ������� ���� */
//   if(elm==IDK_QUIT) {
//                            return(FALSE) ;
//                     }            
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                                break ;
                      }
/*-------------------------------------------------------- ��������� */

    case WM_PAINT:   {
/*- - - - - - - - - - - - - - - - - - - - ������� �������� ��������� */
//              InvalidateRect(hWnd, NULL, false) ;
           hDC=          GetDC(hWnd) ;
                wglMakeCurrent(hDC, OPENGL_CONTEXT) ;
/*- - - - - - - - - - - - - - - - - - - - - ������� ���� ����������� */
         GetClientRect(hWnd, &wnd_rect) ;
             xy_aspect=((double)wnd_rect.bottom)/
                       ((double)wnd_rect.right) ;

            glViewport(0, 0, wnd_rect.right, wnd_rect.bottom) ;
/*- - - - - - - - - - - - - - - - - - -  ���������������� ���������� */
              glEnable(GL_DEPTH_TEST) ;
              glEnable(GL_POLYGON_OFFSET_LINE) ;
              glEnable(GL_POLYGON_OFFSET_FILL) ;

//            glEnable(GL_LIGHTING) ;
              glEnable(GL_COLOR_MATERIAL) ;
       glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ;
           glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ;

          glClearColor(0.7f, 0.7f, 0.7f, 0.0f) ;
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*- - - - - - - - - - - - - - - - - - - -  �������� ���������� ����� */
#pragma warning(disable : 4244)

                light_pos[0]=CONTEXT.Look_x ;
                light_pos[1]=CONTEXT.Look_y ;
                light_pos[2]=CONTEXT.Look_z ;
                light_pos[3]=   1. ;

#pragma warning(default : 4244)

              glEnable(GL_LIGHT0) ;
             glLightfv(GL_LIGHT0, GL_POSITION, light_pos) ;
/*- - - - - - - - - - - - - - - - - - - - - - - ������� ����� ������ */
        if(CONTEXT.AtObject[0]!=0)  G2D_lookat_point(&CONTEXT) ;    /* ���� ��������������� ����� ���������� */
  
          glMatrixMode(GL_PROJECTION) ;
        glLoadIdentity() ;
//      gluPerspective(30., xy_aspect, 5., 100.) ;
//           glFrustum(-2., 2., -2., 2, 5., 100.) ;
  
               glOrtho(-CONTEXT.Zoom*0.5, 
                        CONTEXT.Zoom*0.5, 
                       -CONTEXT.Zoom*0.5*xy_aspect,
                        CONTEXT.Zoom*0.5*xy_aspect, 1., 100.) ;
  
          glMatrixMode(GL_MODELVIEW) ;
        glLoadIdentity() ;

               Point_Matrix.LoadZero   (3, 1) ;
               Point_Matrix.SetCell    (2, 0, 5.) ;

                 Sum_Matrix.Load3d_azim(CONTEXT.Look_azim) ;
                Oper_Matrix.Load3d_elev(CONTEXT.Look_elev) ;
                 Sum_Matrix.LoadMul    (&Sum_Matrix, &Oper_Matrix) ;
               Point_Matrix.LoadMul    (&Sum_Matrix, &Point_Matrix) ;
            
             gluLookAt(CONTEXT.Look_x,
                       CONTEXT.Look_y,
                       CONTEXT.Look_z,
                       CONTEXT.Look_x+Point_Matrix.GetCell(0, 0),
                       CONTEXT.Look_y+Point_Matrix.GetCell(1, 0),
                       CONTEXT.Look_z+Point_Matrix.GetCell(2, 0),
                             0.,        1.,        0.  ) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  ��������� ��� ����� */
       for(layer=1 ; layer<3 ; layer++) 
        for(i=1 ; i<=g2d_draw_list_max ; i++)
          if(g2d_draw_list[i]==layer) {

                      glPushMatrix() ;

                        glCallList(i) ;
                 status=glGetError() ;
              if(status) {
                              sprintf(text, "OpenGL error: %d", status) ;
                           MessageBox(NULL, text, "", 0) ;
                         }

                       glPopMatrix() ;
                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - ����� �� ����� */
                                 glFlush() ;
                             SwapBuffers(hDC) ;
/*- - - - - - - - - - - - - - - - -  ������������ �������� ��������� */
                          wglMakeCurrent(NULL, NULL) ;
                               ReleaseDC(hWnd, hDC) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                          ValidateRect(hWnd,  NULL) ;

			     break ;
		     }
/*---------------------------------- ��������� ���������������� ���� */

/*------------------------------------------------- ������ ��������� */

    default :        {
		return( DefWindowProc(hWnd, Msg, wParam, lParam) ) ;
			    break ;
		     }
/*-------------------------------------------------------------------*/
	      }
/*-------------------------------------------------------------------*/

#undef  CONTEXT      
#undef  OPENGL_CONTEXT

    return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ���������� ������ ��������                        */
/*                 ���������� ��������� ��������                     */

 int  G2D_first_context(char *name)

{
   g2d_context_idx=-1 ;

  return(G2D_next_context(name)) ;
}

 int  G2D_next_context(char *name)

{
#define  CONTEXT   g2d_contexts[g2d_context_idx]

/*----------------------------------- �������� ���������� ���������� */

    if(g2d_context_hDC){
                          ReleaseDC(g2d_context_hWnd, g2d_context_hDC) ;
//                   wglMakeCurrent(NULL, NULL) ;
//                 wglDeleteContext(g2d_context_hOpenGL) ;

                           g2d_context_hWnd    =NULL ;
                           g2d_context_hDC     =NULL ;
                            g2d_context_hOpenGL=NULL ;
                       }
/*----------------------------------------- �������� ������ �������� */

         g2d_context_idx++ ;

  for( ; g2d_context_idx<_OPENGL_WND_MAX ; g2d_context_idx++) {

    if(CONTEXT.hWnd==NULL)  continue ;

           g2d_context_hWnd   =                 CONTEXT.hWnd ;
           g2d_context_hDC    =           GetDC(CONTEXT.hWnd) ;
//         g2d_context_hOpenGL=wglCreateContext(g2d_context_hDC) ;
           g2d_context_hOpenGL=                 CONTEXT.hOpenGL ;

                wglMakeCurrent(g2d_context_hDC, g2d_context_hOpenGL) ;
      
                            return(0) ;
                                                              }

/*-------------------------------------------------------------------*/

#undef  CONTEXT

  return(-1) ;
}


/*********************************************************************/
/*                                                                   */
/*                      ���������� ��������                          */

 void  G2D_show_context(char *name)

{
    SendMessage(g2d_context_hWnd, WM_PAINT, 0, 0) ;
}


/*********************************************************************/
/*                                                                   */
/*                     ������ �������� ���������                     */

  double  G2D_get_context(char *par)

{
#define  CONTEXT   g2d_contexts[g2d_context_idx]


    if(!stricmp(par, "ZOOM" ))  return(CONTEXT.Zoom) ;
    if(!stricmp(par, "RANGE"))  return(CONTEXT.Zoom) ;
 
#undef  CONTEXT

                               return(0.) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ������� ��������� ���������                     */

  void  G2D_read_context(std::string *buff)

{
   char  text[1024] ;
   char *name ;
   char *entry ;
   char *end ;

#define  CONTEXT   g2d_contexts[g2d_context_idx]

/*------------------------------------------------------- ���������� */

            memset(text, 0, sizeof(text)) ;
           strncpy(text, buff->c_str(), sizeof(text)-1) ;

/*-------------------------------------- ������������� ���� �� ����� */

         name =strstr(text, " NAME=") ;
      if(name==NULL)  return ;
         name+=strlen(" NAME=") ;

         end=strchr(name, ' ') ;
      if(end!=NULL)  *end=0 ;

     for(g2d_context_idx=0 ; 
         g2d_context_idx<_OPENGL_WND_MAX ; g2d_context_idx++) {

                 if(         CONTEXT.hWnd==NULL )  continue ;
                 if(!stricmp(CONTEXT.name, name))  break ;
                                                              }
/*---------------------------------------- �������� ������ ��������� */

    if(g2d_context_idx==_OPENGL_WND_MAX) {
                                             return ;
                                         }
/*------------------------------------------ ������� ���������� ���� */

    entry=strstr(end+1, "LOOK_X=") ;  CONTEXT.Look_x   =atof(entry+strlen("LOOK_X=")) ;
    entry=strstr(end+1, "LOOK_Y=") ;  CONTEXT.Look_y   =atof(entry+strlen("LOOK_Y=")) ;
    entry=strstr(end+1, "LOOK_Z=") ;  CONTEXT.Look_z   =atof(entry+strlen("LOOK_Z=")) ;
    entry=strstr(end+1, "LOOK_A=") ;  CONTEXT.Look_azim=atof(entry+strlen("LOOK_A=")) ;
    entry=strstr(end+1, "LOOK_E=") ;  CONTEXT.Look_elev=atof(entry+strlen("LOOK_E=")) ;
    entry=strstr(end+1, "LOOK_R=") ;  CONTEXT.Look_roll=atof(entry+strlen("LOOK_R=")) ;
    entry=strstr(end+1, "ZOOM="  ) ;  CONTEXT.Zoom     =atof(entry+strlen("ZOOM=")) ;

    entry=strstr(end+1, "LOOKAT=") ;
         strncpy(CONTEXT.AtObject, entry+strlen("LOOKAT="), sizeof(CONTEXT.AtObject)) ;

/*-------------------------------------------------------------------*/

#undef  CONTEXT
}


/*********************************************************************/
/*                                                                   */
/*                   �������� ��������� ���������                    */

  void  G2D_write_context(std::string *text)

{
  char  field[1024] ;

#define  CONTEXT   g2d_contexts[g2d_context_idx]

                                                           *text = "" ;
    sprintf(field, " NAME=%s",       CONTEXT.name     ) ;  *text+=field ;
    sprintf(field, " LOOK_X=%.10lf", CONTEXT.Look_x   ) ;  *text+=field ;
    sprintf(field, " LOOK_Y=%.10lf", CONTEXT.Look_y   ) ;  *text+=field ;
    sprintf(field, " LOOK_Z=%.10lf", CONTEXT.Look_z   ) ;  *text+=field ;
    sprintf(field, " LOOK_A=%.10lf", CONTEXT.Look_azim) ;  *text+=field ;
    sprintf(field, " LOOK_E=%.10lf", CONTEXT.Look_elev) ;  *text+=field ;
    sprintf(field, " LOOK_R=%.10lf", CONTEXT.Look_roll) ;  *text+=field ;
    sprintf(field, " ZOOM=%.10lf",   CONTEXT.Zoom     ) ;  *text+=field ;
    sprintf(field, " LOOKAT=%s",     CONTEXT.AtObject ) ;  *text+=field ;

#undef  CONTEXT
}


/*********************************************************************/
/*                                                                   */
/*            �������������� ������ ����������� ������               */
/*                                                                   */
/*     ���������:  mode=0  -  �������� ������ ����������             */
/*                              ����������� ���������                */
/*                 mode=2  -  �������� ������ ���������              */
/*                              ����������� ���������                */
/*                 mode=1  -  ��������� ������                       */

 int  G2D_show_getlist(int mode_flag)

{
  int  i ;


    for(i=1 ; i<_DRAW_LIST_MAX ; i++)
      if(!g2d_draw_list[i]) {

            if(mode_flag==2)  g2d_draw_list[i]= 2 ;      
       else if(mode_flag==1)  g2d_draw_list[i]=-1 ;      
       else                   g2d_draw_list[i]= 1 ;

       if(i>g2d_draw_list_max)  g2d_draw_list_max=i ;
                                    return(i) ;
                            }

        MessageBox(NULL, "Display lists numeration overflow", 
                         "Fatal Error", MB_ICONSTOP) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*               ������������ ������ ����������� ������              */

   void  G2D_show_releaselist(int  idx)

{
                              g2d_draw_list[idx]=0;   

    if(idx==g2d_draw_list_max)  
      while(              g2d_draw_list_max > 0 &&
            g2d_draw_list[g2d_draw_list_max]==0   )
                          g2d_draw_list_max-- ;
}


/*********************************************************************/
/*                                                                   */
/*              ������ � ������ ������ ������                        */

   int  G2D_look(  char *name, char *oper, 
                 double *x, double *y, double *z,
                 double *a, double *e, double *r ) 
{
   int  wnd_num ;

/*--------------------------------------- ����������� ��������� ���� */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* ���� �� ������� ���������� */
     if(!stricmp(g2d_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- ���������� */

   if(!stricmp(oper, "GET")) {

            if(x!=NULL)  *x=g2d_contexts[wnd_num].Look_x ;
            if(y!=NULL)  *y=g2d_contexts[wnd_num].Look_y ;
            if(z!=NULL)  *z=g2d_contexts[wnd_num].Look_z ;
            if(a!=NULL)  *a=g2d_contexts[wnd_num].Look_azim ;
            if(e!=NULL)  *e=g2d_contexts[wnd_num].Look_elev ;
            if(r!=NULL)  *r=g2d_contexts[wnd_num].Look_roll ;

                             }
/*-------------------------------------------------------- ��������� */

   if(!stricmp(oper, "SET")) {

            if(x!=NULL)  g2d_contexts[wnd_num].Look_x   =*x ;
            if(y!=NULL)  g2d_contexts[wnd_num].Look_y   =*y ;
            if(z!=NULL)  g2d_contexts[wnd_num].Look_z   =*z ;
            if(a!=NULL)  g2d_contexts[wnd_num].Look_azim=*a ;
            if(e!=NULL)  g2d_contexts[wnd_num].Look_elev=*e ;
            if(r!=NULL)  g2d_contexts[wnd_num].Look_roll=*r ;

                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ������ � ����� ������ ������                      */

   int  G2D_zoom(char *name, char *oper, double *zoom) 
{
   int  wnd_num ;

/*--------------------------------------- ����������� ��������� ���� */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* ���� �� ������� ���������� */
     if(!stricmp(g2d_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- ���������� */

   if(!stricmp(oper, "GET")) {

            if(zoom!=NULL)  *zoom=g2d_contexts[wnd_num].Zoom ;

                             }
/*-------------------------------------------------------- ��������� */

   if(!stricmp(oper, "SET")) {

            if(zoom!=NULL)  g2d_contexts[wnd_num].Zoom=*zoom ;

                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                 ������ � ������ ���������� ������                 */

   int  G2D_lookat(char *name, char *oper, char *look_at) 
{
   int  wnd_num ;

/*--------------------------------------- ����������� ��������� ���� */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* ���� �� ������� ���������� */
     if(!stricmp(g2d_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------- ���������� */

   if(!stricmp(oper, "GET")) {

     if(look_at!=NULL)  strcpy(look_at, g2d_contexts[wnd_num].AtObject) ;

                             }
/*-------------------------------------------------------- ��������� */

   if(!stricmp(oper, "SET")) {

     if(look_at!=NULL) {

         memset(g2d_contexts[wnd_num].AtObject, 0, 
                              sizeof(g2d_contexts[wnd_num].AtObject)) ;
        strncpy(g2d_contexts[wnd_num].AtObject, look_at,
                              sizeof(g2d_contexts[wnd_num].AtObject)-1) ;
                       } 
                             }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                         ������������ ����                         */

   int  G2D_redraw(char *name) 
{
   int  wnd_num ;

/*--------------------------------------- ����������� ��������� ���� */

   for(wnd_num=0 ; wnd_num<_OPENGL_WND_MAX ; wnd_num++)             /* ���� �� ������� ���������� */
     if(!stricmp(g2d_contexts[wnd_num].name, name))  break ;

     if(wnd_num==_OPENGL_WND_MAX)  return(-1) ;

/*------------------------------------------------------ ����������� */

      SendMessage(g2d_contexts[wnd_num].hWnd, WM_PAINT, 0, 0) ;

/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                   ��������� ����� ����������                      */

   int  G2D_lookat_point(WndContext *context) 
{
  RSS_Point  target ;
     double  x ;
     double  y ;
     double  z ;
     double  dy ;
     double  ds ;
       char  name[256] ;
       char *end ;
        int  i ;

#define  _RAD_TO_GRD  57.296  

/*------------------------------------------- ��������� �� ��������� */

                x=strtod(context->AtObject, &end) ;
                y=strtod(              end, &end) ;
                z=strtod(              end, &end) ;

/*---------------------------------------------- ��������� �� ������ */

   if(*end!=0) {

#define   OBJECTS       RSS_Kernel::kernel->kernel_objects 
#define   OBJECTS_CNT   RSS_Kernel::kernel->kernel_objects_cnt 

                    memset(name, 0, sizeof(name)) ;
                   strncpy(name, context->AtObject, sizeof(name)-1) ;

                end=strchr(name, '.') ;
             if(end!=NULL) {  *end=0 ;  end++ ;  }

       for(i=0 ; i<OBJECTS_CNT ; i++)
         if(!stricmp(OBJECTS[i]->Name, name))  break ;

    if(i==OBJECTS_CNT) {                                            /* ���� ��� �� ������... */
             SEND_ERROR("������ ���������� �� ����������") ;
                  context->AtObject[0]=0 ;
                             return(0) ;
                       }

            OBJECTS[i]->vGetTarget(end, &target) ;

                        x=target.x ;
                        y=target.y ;
                        z=target.z ;

/*-------------------------------------------------------------------*/

#undef   OBJECTS
#undef   OBJECTS_CNT

               } 
/*---------------------------------------- �������� ����� ���������� */

                dy=y-context->Look_y ;
                ds=sqrt((x-context->Look_x)*(x-context->Look_x)+
                        (z-context->Look_z)*(z-context->Look_z) ) ;

     context->Look_elev=-_RAD_TO_GRD*atan (dy/ds) ;
     context->Look_azim=-_RAD_TO_GRD*atan2(x-context->Look_x, 
                                           z-context->Look_z) ;

/*-------------------------------------------------------------------*/

#undef  _RAD_TO_GRD

  return(0) ;
}
