
#include  <stddef.h>
#include  <stdlib.h>
#include  <string.h>
#include  <math.h>

#include "..\RSS_Feature\RSS_Feature.h"
#include "..\RSS_Object\RSS_Object.h"

#include  "L_Robot2D.h"

/*--------------------------------------------- Системные постоянные */

#define  _R_ANGLE_ACC    0.1
#define   _R_LINE_ACC    0.0001

/*---------------------------------------- Описание рабочих структур */

 typedef struct {
                   int  link ;
                   int  type ;
		} Robot2D_active_degree ;

 typedef struct {
		   int  scheme ;
		   int  undef ;
		   int  type[3] ;
                } Robot2D_scheme ;

 static Robot2D_active_degree  rbt_act[20] ;
 static          Robot2D_link *rbt_links ;
 static          Robot2D_link *rbt_cnn ;
 static                   int  rbt_scheme ;
 static                double  rbt_x_corr ;
 static                double  rbt_y_corr ;
 static                double  rbt_kx1 ;
 static                double  rbt_kx2 ;
 static                double  rbt_ky1 ;
 static                double  rbt_ky2 ;

/*--------------------------------------- Описание канонических схем */

#define _SHEME_MAX    7

#define _R_HHH_SHEME    1
#define _R_LHH_SHEME    2
#define _R_HLH_SHEME    3
#define _R_HHL_SHEME    4
#define _R_LLH_SHEME    5
#define _R_HLL_SHEME    6
#define _R_LHL_SHEME    7

 static Robot2D_scheme  rbt_schemes_list[_SHEME_MAX]={
		     _R_LLH_SHEME,  0,  'L', 'L', 'H',
		     _R_LHL_SHEME,  0,  'L', 'H', 'L',
		     _R_HLL_SHEME,  0,  'H', 'L', 'L',
		     _R_LHH_SHEME,  1,  'L', 'H', 'H',
		     _R_HLH_SHEME,  0,  'H', 'L', 'H',
		     _R_HHL_SHEME,  1,  'H', 'H', 'L',
		     _R_HHH_SHEME,  1,  'H', 'H', 'H'
					             } ;
/*-------------------------------------------------------------------*/



/*********************************************************************/
/*                                                                   */
/*             Решение прямой кинематической задачи                  */

  void  Robot2D_direct_problem(         int  regime,
                               Robot2D_link *links,
                                        int  links_cnt )
{
  double  angle ;
     int  i ;

/*--------------------------------- Решение ПКЗ в прямом направлении */

  if(regime==_R_FORWD    ||
     regime==_R_FORWD_PASS ) {

    for(angle=0, i=0 ; i<links_cnt ; i++) {

                  angle+=_GRD_TO_RAD*links[i].joint ;

	  links[i].x2   =links[i].x1+links[i].link*cos(angle) ;
	  links[i].y2   =links[i].y1+links[i].link*sin(angle) ;
	  links[i].angle=angle*_RAD_TO_GRD ;

    if(regime==_R_FORWD && i==links_cnt-1) continue ;

	  links[i+1].x1=links[i].x2 ;
	  links[i+1].y1=links[i].y2 ;
                                          }
			     }
/*------------------------------- Решение ПКЗ в обратном направлении */

  if(regime==_R_BACK    ||
     regime==_R_BACK_PASS ) {

            angle=_GRD_TO_RAD*links[links_cnt-1].angle ;

    for(i=links_cnt-1 ; i>=0 ; i--) {

	  links[i].x1=links[i].x2-links[i].link*cos(angle) ;
	  links[i].y1=links[i].y2-links[i].link*sin(angle) ;

            angle-=_GRD_TO_RAD*links[i].joint ;

      if(regime==_R_BACK && i==0) continue ;

	  links[i-1].x2   =links[i].x1 ;
	  links[i-1].y2   =links[i].y1 ;
	  links[i-1].angle=angle*_RAD_TO_GRD ;

                                    }
			    }
/*-------------------------------------------------------------------*/
}


/*********************************************************************/
/*                                                                   */
/*          Переход от полной схемы к канонической и обратно         */
/*                                                                   */
/*   _R_UNDEF   -  проверить схему на кинематическую определимость   */
/*   _R_SQUEEZE -  привести схему к канонической                     */

  int  Robot2D_scheme_prepare(         int  oper_code, 
                              Robot2D_link *links, 
                                       int  links_cnt,
                              Robot2D_link *canonical )
{
    int  undef ;
    int  act_cnt ;
    int  status ;
    int  i ;

/*------------------------------------------------------- Подготовка */

                      rbt_links=links ;

/*--------------------------------------- Сжатие/Идентификация схемы */

  if(oper_code==_R_UNDEF ||
     oper_code==_R_SQUEEZE ) {
/*- - - - - - - - - - - - - - - - - - - Определяем активные степеней */
	act_cnt=0 ;

   for(i=0 ; i<links_cnt ; i++) {

    if(!links[i].joint_fixed) {  rbt_act[act_cnt].link= i ;
                                 rbt_act[act_cnt].type='H' ;
                                         act_cnt++ ;         }
    if(!links[i].link_fixed ) {  rbt_act[act_cnt].link= i ;
                                 rbt_act[act_cnt].type='L' ;
                                         act_cnt++ ;         }

	    if(act_cnt>3)  return(_RE_MATCH_ACTIVE) ;
				}

	    if(act_cnt<3)  return(_RE_LOW_ACTIVE) ;
/*- - - - - - - - - - - - - - - - - - - - - - -  Идентификация схемы */
    for(i=0 ; i<_SHEME_MAX ; i++)
      if(rbt_act[0].type==rbt_schemes_list[i].type[0] &&
	 rbt_act[1].type==rbt_schemes_list[i].type[1] &&
	 rbt_act[2].type==rbt_schemes_list[i].type[2]   )  break ;

	 if(i<_SHEME_MAX)   rbt_scheme=rbt_schemes_list[i].scheme ;
	 else              return(_RE_BAD_SCHEME) ;
/*- - - - - - - - - -  Определение кинематич. неопределенности схемы */
			    undef=rbt_schemes_list[i].undef ;

           if(oper_code==_R_UNDEF)  return(undef) ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - "Сжатие" схемы */
                            rbt_x_corr=0. ;
                            rbt_y_corr=0. ;

   if(rbt_act[0].link!=0)
       Robot2D_direct_problem(_R_FORWD_PASS, &links[0], rbt_act[0].link) ;

   if(links_cnt-1!=rbt_act[2].link)
       Robot2D_direct_problem(_R_BACK, &links[rbt_act[2].link],
			                 links_cnt-rbt_act[2].link) ;

	  Robot2D_base_prepare(_R_SQUEEZE, canonical) ;
	 Robot2D_robot_prepare(_R_SQUEEZE, canonical) ;

   if(rbt_act[0].link!=0)
	    canonical[0].joint+=rbt_links[rbt_act[0].link-1].angle ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    return(rbt_scheme) ;
			     }
/*----------------------------------- "Обратное развертывание" схемы */

  if(oper_code==_R_EXTEND) {

   if(rbt_act[0].link!=0)       // **** ????? *****
	    canonical[0].joint-=rbt_links[rbt_act[0].link-1].angle ;

	 status=Robot2D_robot_prepare(_R_EXTEND, canonical) ;

 return(status) ;
			   }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*          Переход от полной схемы к канонической и обратно         */

  int  Robot2D_robot_prepare(         int  oper,
                             Robot2D_link *canonical )

{
  int  status ;


               status= 0 ;
	      rbt_cnn=canonical ;

        if(rbt_scheme==_R_HHH_SHEME) {  Robot2D_hh_prepare (oper, 0) ;
                                        Robot2D_hh_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 2) ;  }

   else if(rbt_scheme==_R_HLH_SHEME) {  status=Robot2D_hlh_prepare(oper) ;
                                               Robot2D_top_prepare(oper, 1) ;  }

   else if(rbt_scheme==_R_LHH_SHEME) {  Robot2D_lh_prepare (oper, 0) ;
                                        Robot2D_hh_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 2) ;  }

   else if(rbt_scheme==_R_HHL_SHEME) {  Robot2D_hh_prepare (oper, 0) ;
                                        Robot2D_hl_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 1) ;  }

   else if(rbt_scheme==_R_HLL_SHEME) {  Robot2D_hl_prepare (oper, 0) ;
                                        Robot2D_hl_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 1) ;  }

   else if(rbt_scheme==_R_LLH_SHEME) {  Robot2D_lh_prepare (oper, 0) ;
                                        Robot2D_lh_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 2) ;  }

   else if(rbt_scheme==_R_LHL_SHEME) {  Robot2D_lh_prepare (oper, 0) ;
                                        Robot2D_hl_prepare (oper, 1) ;
                                        Robot2D_top_prepare(oper, 1) ;  }

  return(status) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Обработка базовой точки                        */
/*      при переходе от полной схемы к канонической и обратно        */

  int  Robot2D_base_prepare(         int  oper_code,
                            Robot2D_link *canonical )
{
     int  base ;

				  base=rbt_act[0].link ;

    if(oper_code==_R_SQUEEZE) {

                     canonical[0].x1   =rbt_links[base].x1 ;
                     canonical[0].y1   =rbt_links[base].y1 ;
                     canonical[0].joint=rbt_links[base].angle ;
			      }

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                    Обработка точки схвата                         */
/*      при переходе от полной схемы к канонической и обратно        */

  int  Robot2D_top_prepare(int  oper_code,
                           int  n_link    )

{
     int  top ;

/*------------------------------------------------------- Подготовка */

	     top=rbt_act[2].link ;

/*------------------------------------- Переход к канонической схеме */

   if(oper_code==_R_SQUEEZE) {

    if(rbt_scheme!=_R_HHL_SHEME &&
       rbt_scheme!=_R_HLL_SHEME &&
       rbt_scheme!=_R_LHL_SHEME   )
		  memcpy(&rbt_cnn[n_link], &rbt_links[top],
				     sizeof(rbt_links[0])  ) ;
    else                {
			  rbt_cnn[n_link].x2   =rbt_links[top].x2 ;
			  rbt_cnn[n_link].y2   =rbt_links[top].y2 ;
			  rbt_cnn[n_link].angle=rbt_links[top].angle ;
		       }

			  rbt_cnn[n_link].x2-=rbt_x_corr ;
			  rbt_cnn[n_link].y2-=rbt_y_corr ;
			      }
/*------------------------------------------- Переход к полной схеме */

   if(oper_code==_R_EXTEND) {

    if(rbt_scheme!=_R_HHL_SHEME &&
       rbt_scheme!=_R_HLL_SHEME &&
       rbt_scheme!=_R_LHL_SHEME   )
	      memcpy(&rbt_links[top], &rbt_cnn[n_link],
				   sizeof(rbt_links[0])) ;
			     }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Обработка фрагмента шарнир-шарнир                */
/*      при переходе от полной схемы к канонической и обратно        */

  int  Robot2D_hh_prepare(int  oper_code,
                          int  n_link    )

{
     int  base ;
     int  top ;
  double  x ;
  double  y ;
  double  angle ;
     int  i ;

/*------------------------------------------------------- Подготовка */

	     base=rbt_act[n_link  ].link ;
	     top =rbt_act[n_link+1].link ;

/*------------------------------------- Переход к канонической схеме */

    if(oper_code==_R_SQUEEZE) {

	 rbt_cnn[n_link].link     =rbt_links[base].link ;
	 rbt_cnn[n_link].joint_min=  0. ;
	 rbt_cnn[n_link].joint_max=  0. ;

       if(base==top-1) return(-1) ;

		 x=rbt_links[base].link ;
		 y= 0. ;
	     angle= 0. ;

       for(i=base+1 ; i<top ; i++) {
		  angle+=rbt_links[i].joint ;
		      x+=rbt_links[i].link*cos(angle*_GRD_TO_RAD) ;
		      y+=rbt_links[i].link*sin(angle*_GRD_TO_RAD) ;
				   }

		  rbt_cnn[n_link].link     =_hypot(y, x) ;
		  rbt_cnn[n_link].joint_min=atan2(y, x)*_RAD_TO_GRD ;
		  rbt_cnn[n_link].joint_max=angle ;
			      }
/*------------------------------------------- Переход к полной схеме */

    if(oper_code==_R_EXTEND) {

	rbt_links[  base  ].joint =rbt_cnn[n_link].joint
				  -rbt_cnn[n_link].joint_min ;
	  rbt_cnn[n_link+1].joint-=rbt_cnn[n_link].joint_max
				  -rbt_cnn[n_link].joint_min ;
			     }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Обработка фрагмента шарнир-ползун                */
/*      при переходе от полной схемы к канонической и обратно        */


  int  Robot2D_hl_prepare(int  oper_code,
                          int  n_link    )

{
     int  base ;
     int  top ;
  double  x ;
  double  y ;
  double  angle  ;
     int  i ;

/*------------------------------------------------------- Подготовка */

   if(        n_link     !=0   &&
      rbt_act[n_link].type=='L'  )  base=rbt_act[n_link].link+1 ;
   else                             base=rbt_act[n_link].link  ;

				     top=rbt_act[n_link+1].link ;

/*------------------------------------- Переход к канонической схеме */

   if(oper_code==_R_SQUEEZE) {

			 x=0. ;
			 y=0. ;
     if(base==top) {
		     angle=0. ;
		   }
     else          {
		     angle=-rbt_links[top].joint ;

       for(i=top-1 ; i>=base ; i--) {
		      x+=rbt_links[i].link*cos(angle*_GRD_TO_RAD) ;
		      y+=rbt_links[i].link*sin(angle*_GRD_TO_RAD) ;
     if(i!=base)  angle-=rbt_links[i].joint ;
				   }
		   }

	 rbt_cnn[n_link].joint    =rbt_links[base].joint-angle ;
	 rbt_cnn[n_link].joint_min=angle ;
	 rbt_cnn[n_link].link_min = x ;

     if(n_link==0)      rbt_y_corr=y ;
     else         {

      if(rbt_act[n_link].type=='L') {

	       angle=_GRD_TO_RAD*(angle-rbt_links[base].joint) ;
		   x= -rbt_y_corr*sin(angle) ;
		   y=y+rbt_y_corr*cos(angle) ;
          rbt_y_corr= 0. ;
				    }
      else         x= 0. ;

		     angle =_GRD_TO_RAD*rbt_links[top].angle ;
		rbt_x_corr+=x*cos(angle)-y*sin(angle) ;
		rbt_y_corr+=y*cos(angle)+x*sin(angle) ;
		  }

			     }
/*------------------------------------------- Переход к полной схеме */

   if(oper_code==_R_EXTEND) {

	   rbt_links[ top].link =rbt_cnn[n_link].link
				-rbt_cnn[n_link].link_min ;
	   rbt_links[base].joint=rbt_cnn[n_link].joint
				+rbt_cnn[n_link].joint_min ;
			    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*                  Обработка фрагмента ползун-шарнир                */
/*      при переходе от полной схемы к канонической и обратно        */

  int  Robot2D_lh_prepare(int  oper_code, 
                          int  n_link    )

{
     int  base ;
     int  top ;
  double  x ;
  double  y ;
  double  angle  ;
     int  i ;

/*------------------------------------------------------- Подготовка */

	     base=rbt_act[n_link].link ;
	     top =rbt_act[n_link+1].link ;

/*------------------------------------- Переход к канонической схеме */

   if(oper_code==_R_SQUEEZE) {

                rbt_cnn[n_link].joint    =rbt_links[base].joint ;
    if(n_link)  rbt_cnn[n_link].joint   +=rbt_cnn[0].joint_min ;
                rbt_cnn[n_link].joint_min=  0. ;
                rbt_cnn[n_link].link_min =  0. ;

       if(base==top-1) return(0) ;

                      x = 0. ;
                      y = 0. ;
                  angle = 0. ;

       for(i=base+1 ; i<top ; i++) {
		  angle+=rbt_links[i].joint ;
		      x+=rbt_links[i].link*cos(angle*_GRD_TO_RAD) ;
		      y+=rbt_links[i].link*sin(angle*_GRD_TO_RAD) ;
				   }

             rbt_cnn[n_link].joint_min=angle ;
             rbt_cnn[n_link].link_min =  x ;

                angle =rbt_links[base].joint+90. ;
    if(n_link)  angle+=rbt_links[0].joint+rbt_cnn[0].joint_min ;
           rbt_x_corr+=y*cos(_GRD_TO_RAD*angle) ;
           rbt_y_corr+=y*sin(_GRD_TO_RAD*angle) ;

			     }
/*------------------------------------------- Переход к полной схеме */

   if(oper_code==_R_EXTEND) {

        rbt_links[base].link  =rbt_cnn[n_link].link
                              -rbt_cnn[n_link].link_min ;
      rbt_cnn[n_link+1].joint-=rbt_cnn[n_link].joint_min ;

			    }
/*-------------------------------------------------------------------*/

   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*            Обработка фрагмента шарнир-ползун-шарнир               */
/*      при переходе от полной схемы к канонической и обратно        */

  int  Robot2D_hlh_prepare(int  oper_code)

{
     int  base ;
     int  move ;
     int  top ;
  double  d ;
  double  x ;
  double  y ;
  double  angle  ;
  double  angle_1 ;
  double  angle_2 ;
     int  i ;

/*------------------------------------------------------- Подготовка */

	     base=rbt_act[0].link ;
	     move=rbt_act[1].link ;
	     top =rbt_act[2].link ;

/*------------------------------------- Переход к канонической схеме */

   if(oper_code==_R_SQUEEZE) {
                             }
/*------------------------------------------- Переход к полной схеме */

   if(oper_code==_R_EXTEND) {

		 angle_1=0. ;
		 angle_2=0. ;
		       x=0. ;
		       y=0. ;
    if(base!=move)
       for(i=move-1 ; i>=base ; i--) {
		angle_1-=rbt_links[i+1].joint ;
		      x+=rbt_links[i].link*cos(angle_1*_GRD_TO_RAD) ;
		      y+=rbt_links[i].link*sin(angle_1*_GRD_TO_RAD) ;
				     }
    if(move!=top-1)
       for(i=move+1 ; i<top ; i++) {
		angle_2+=rbt_links[i].joint ;
		      x+=rbt_links[i].link*cos(angle_2*_GRD_TO_RAD) ;
		      y+=rbt_links[i].link*sin(angle_2*_GRD_TO_RAD) ;
				   }

		     d=rbt_cnn[0].link*rbt_cnn[0].link-y*y ;
 	          if(d<0.) return(_RE_UNACHIEVED) ;

     if(rbt_cnn[0].link>0.)  rbt_links[move].link= sqrt(d)-x ;
     else                    rbt_links[move].link=-sqrt(d)-x ;

                        angle=atan2(y, rbt_links[move].link+x)*_RAD_TO_GRD ;
        rbt_links[base].joint=rbt_cnn[0].joint+angle_1-angle ;
          rbt_cnn[  1 ].joint=rbt_cnn[1].joint-angle_2+angle ;

			    }
/*-------------------------------------------------------------------*/
   return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*   Решение обратной кинематической задачи для канонических схем    */

  int  Robot2D_inverse_problem(         int  scheme, 
                               Robot2D_link *robot, 
                                        int  undef_code)

{
   double  angle ;
   double  x ;
   double  y ;
   double  f ;
   double  a ;
   double  b ;
   double  d ;
   double  tmp ;

/*--------------------------------------------------- Проверка схемы */

   if(scheme!=_R_LLH_SHEME &&
      scheme!=_R_LHL_SHEME &&
      scheme!=_R_HLL_SHEME &&
      scheme!=_R_HLH_SHEME &&
      scheme!=_R_LHH_SHEME &&
      scheme!=_R_HHL_SHEME &&
      scheme!=_R_HHH_SHEME   )  return(_RE_BAD_SCHEME) ;
                             
/*------------------------------------------------------- Подготовка */

   if(scheme==_R_LLH_SHEME ||
      scheme==_R_LHH_SHEME ||
      scheme==_R_HHH_SHEME   ) {  x =robot[2].x2-robot[0].x1 ;
                                  y =robot[2].y2-robot[0].y1 ;
                              angle =robot[2].angle*_GRD_TO_RAD ;
                                  x-=robot[2].link*cos(angle) ;
                                  y-=robot[2].link*sin(angle) ;
                                  f =robot[2].angle ;            }
   else                        {  x =robot[1].x2-robot[0].x1 ;
                                  y =robot[1].y2-robot[0].y1 ;
                                  f =robot[1].angle ;            }

              if(undef_code)  tmp=-1. ;
              else            tmp= 1. ;

/*--------------------------------------------- Одно-шарнирные схемы */

   if(scheme==_R_LLH_SHEME ||
      scheme==_R_LHL_SHEME ||
      scheme==_R_HLL_SHEME   ) {

       if(scheme==_R_LLH_SHEME)  robot[2].joint=f-robot[0].joint
	 			 	 	 -robot[1].joint ;
       if(scheme==_R_LHL_SHEME)  robot[1].joint=f-robot[0].joint ;
       if(scheme==_R_HLL_SHEME)  robot[0].joint=f-robot[1].joint ;

     if(Robot2D_test_angle(robot[1].joint, 0.))
       if(scheme==_R_LHL_SHEME)  return(_RE_DEGRAD_POINT) ;
       else                      return(_RE_DEGRAD_SCHEME) ;

		     Robot2D_tsk_ll(robot) ;

		robot[0].link=rbt_kx1*x-rbt_ky1*y ;
		robot[1].link=rbt_kx2*x-rbt_ky2*y ;

                                     return(0) ;
			       }
/*--------------------------------------- Схемя шарнир-ползун-шарнир */

   if(scheme==_R_HLH_SHEME) {
                              angle =_GRD_TO_RAD*robot[1].angle ;
                                  x-=robot[1].link*cos(angle) ;
                                  y-=robot[1].link*sin(angle) ;
                      robot[0].link =_hypot(y, x) ;
                      robot[0].joint=atan2(y, x)*_RAD_TO_GRD ;
                      robot[1].joint=f-robot[0].joint ;

                                     return(0) ;
                            }
/*--------------------------------------- Схемя ползун-шарнир-шарнир */

   if(scheme==_R_LHH_SHEME) { 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          if(Robot2D_test_angle(robot[0].joint, 90.)) {

				d=x/robot[1].link ;
	    if(fabs(d)>1.) return(_RE_UNACHIEVED) ;
			    angle=tmp*acos(d) ;
		    robot[0].link=(y-robot[1].link*sin(angle))
				 *sin(_GRD_TO_RAD*robot[0].joint) ;
                                                      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     else if(Robot2D_test_angle(robot[0].joint,  0.)) {

				d=y/robot[1].link ;
	    if(fabs(d)>1.) return(_RE_UNACHIEVED) ;
		  if(tmp>0.) angle=asin(d) ;
		  else       angle=_PI-asin(d) ;

           robot[0].link=(x-robot[1].link*cos(angle))
	                      *cos(_GRD_TO_RAD*robot[0].joint) ;
						      }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     else                                             {
				 a=tan(_GRD_TO_RAD*robot[0].joint) ;
				 b=(y-a*x)/robot[1].link ;
				 d=a*a-b*b+1 ;
		      if(d<0.) return(_RE_UNACHIEVED) ;

			       tmp=(b+tmp*a*sqrt(d))/(a*a+1) ;
			     angle=asin(tmp) ;
			       tmp=tmp-a*cos(angle)-b ;
   if(fabs(tmp)>_R_LINE_ACC) angle=_PI-angle ;

		    robot[0].link =(x-robot[1].link*cos(angle))
				  /cos(_GRD_TO_RAD*robot[0].joint) ;
					              }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		    robot[1].joint=angle*_RAD_TO_GRD-robot[0].joint ;
		    robot[2].joint=f-robot[1].joint-robot[0].joint ;

                                     return(0) ;
                            }
/*--------------------------------------- Схемя шарнир-шарнир-ползун */

   if(scheme==_R_HHL_SHEME) { 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	  if(Robot2D_test_angle(f, 90.)) {

		d=x/robot[0].link ;

       if(fabs(d)>1.) return(_RE_UNACHIEVED) ;

		 angle=tmp*acos(d) ;
	 robot[1].link=robot[0].link*sin(angle)-y ;
				         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     else if(Robot2D_test_angle(f,  0.)) {

	     d=y/robot[0].link ;

      if(fabs(d)>1.) return(_RE_UNACHIEVED) ;

      if(tmp>0.) angle=asin(d) ;
      else       angle=_PI-asin(d) ;
	 robot[1].link=(x-robot[0].link*cos(angle))
		      *cos(f*_GRD_TO_RAD) ;
				         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     else                                {

				 a=tan(_GRD_TO_RAD*f) ;
				 b=(y-a*x)/robot[0].link ;
				 d=a*a-b*b+1 ;
		      if(d<0.) return(_RE_UNACHIEVED) ;

                                       tmp=(b+tmp*a*sqrt(d))/(a*a+1) ;
                                     angle=asin(tmp) ;
                                       tmp=tmp-a*cos(angle)-b ;
           if(fabs(tmp)>_R_LINE_ACC) angle=_PI-angle ;

		    robot[1].link =(x-robot[0].link*cos(angle))
                                                /cos(_GRD_TO_RAD*f) ;
				         }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
		    robot[1].joint=-(angle*_RAD_TO_GRD-f) ;
		    robot[0].joint=f-robot[1].joint ;/* angle*_RAD_TO_GRD ? */

                                  return(0) ;
		            }
/*--------------------------------------- Схемя шарнир-шарнир-шарнир */

   if(scheme==_R_HHH_SHEME) { 

			 b=_hypot(y, x) ;
			 a= atan2(y, x) ;
			 d=(robot[0].link*robot[0].link+b*b
			   -robot[1].link*robot[1].link)
			  /(2*b*robot[0].link) ;

	      if(fabs(d)>1.) return(_RE_UNACHIEVED) ;

		     angle =a+tmp*acos(d) ;
	    robot[0].joint =angle*_RAD_TO_GRD ;
			 x-=robot[0].link*cos(angle) ;
			 y-=robot[0].link*sin(angle) ;
	    robot[1].joint =atan2(y, x)*_RAD_TO_GRD-robot[0].joint ;
	    robot[2].joint =f-robot[0].joint-robot[1].joint ;

                                  return(0) ;
		            }
/*-------------------------------------------------------------------*/

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*     Решение частного фрагмента обратной кинематической задачи     */

  int  Robot2D_tsk_ll(Robot2D_link *robot)

{
   double  angle_1 ;
   double  angle_2 ;
   double  tg_1 ;
   double  tg_2 ;
      int  flag ;

	  angle_1=robot[0].joint ;
	  angle_2=robot[0].joint+robot[1].joint ;

                                              flag=0 ;
	 if(Robot2D_test_angle(angle_1, 90))  flag=1 ;
	 if(Robot2D_test_angle(angle_2, 90))  flag=2 ;

	      angle_1*=_GRD_TO_RAD ;
	      angle_2*=_GRD_TO_RAD ;

    if(flag!=1) {      tg_1= tan(angle_1) ;
		    rbt_ky2=1./(cos(angle_2)*tg_1-sin(angle_2)) ;
		    rbt_kx2=tg_1*rbt_ky2 ;          }
     else       {   rbt_ky2=0. ;
		    rbt_kx2=1./cos(angle_2) ;       }

    if(flag!=2) {      tg_2= tan(angle_2) ;
		    rbt_ky1=1./(cos(angle_1)*tg_2-sin(angle_1)) ;
		    rbt_kx1=tg_2*rbt_ky1 ;          }
     else       {   rbt_ky1=0. ;
		    rbt_kx1=1./cos(angle_1) ;       }

  return(0) ;
}



/*********************************************************************/
/*                                                                   */
/*                   Проверка "близости" углов                       */

  int  Robot2D_test_angle(double  angle, double  test)

{
  int  i ;

    angle=Robot2D_cnv_angle(angle) ;

  for(i=-1 ; i<=1 ; i++)
     if( angle>(test+180.*i-_R_ANGLE_ACC) &&
	 angle<(test+180.*i+_R_ANGLE_ACC)    ) return(1) ;

  return(0) ;
}


/*********************************************************************/
/*                                                                   */
/*       Приведение угла к диапазону -180...180                      */

    double  Robot2D_cnv_angle(double  angle)

{
   while(angle<-180. || angle>180.)
				    if(angle<-180.) angle+=360. ;
			       else if(angle> 180.) angle-=360. ;
  return(angle) ;
}

