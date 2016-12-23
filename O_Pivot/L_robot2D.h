
/*-------------------------------------- Описание звена манипулятора */

typedef struct {
                   int  joint_fixed ;  /* Флаг фиксированного шарнира */
                   int  link_fixed ;   /* Флаг фиксированной длины звена */

                double  joint ;        /* Угол в шаpниpе */
                double  joint_min ;
                double  joint_max ;
                double  link ;         /* Длина звена */
                double  link_min ;
                double  link_max ;

                double  baby ;         /* Начальное удлинение звена */
                double  angle ;        /* Угол оpиентации */
                double  length ;       /* Удлинение */
                double  x1 ;           /* Кооpдинаты  концов звена */
                double  y1 ;
                double  x2 ;
                double  y2 ;

	       } Robot2D_link ;

/*----------------------------------------- Коды операций и статусов */

#define   _R_FORWD         1
#define   _R_FORWD_PASS    2
#define   _R_BACK          3
#define   _R_BACK_PASS     4

#define   _R_UNDEF         5
#define   _R_SQUEEZE       6
#define   _R_EXTEND        7

/*------------------------------------------------------ Коды ошибок */

#define _RE_LOW_ACTIVE     -3001    /* Число степеней подвижности меньше 3 */
#define _RE_MATCH_ACTIVE   -3002    /* Число степеней подвижности больше 3 */
#define _RE_BAD_SCHEME     -3003    /* Некорректная схема */
#define _RE_DEGRAD_SCHEME  -3004
#define _RE_DEGRAD_POINT   -3005
#define _RE_UNACHIEVED     -3006
#define _RE_RANGE_OUT      -3007
#define _RE_OBST_CROSS     -3008
#define _RE_LINK_CROSS     -3009
#define _RE_ZONE           -3010

/*---------------------------------------------------- Прототипы п/п */

   void  Robot2D_direct_problem (int, Robot2D_link *, int) ;  /* Решение прямой кинематической задачи */ 
    int  Robot2D_inverse_problem(int, Robot2D_link *, int) ;  /* Решение обратной кинематической задачи для канонических схем */
    int  Robot2D_scheme_prepare (int, Robot2D_link *,         /* Переход от полной схемы к канонической и обратно */
                                 int, Robot2D_link * ) ;

    int  Robot2D_robot_prepare  (int, Robot2D_link *) ;
    int  Robot2D_base_prepare   (int, Robot2D_link *) ;
    int  Robot2D_top_prepare    (int, int) ;
    int  Robot2D_hh_prepare     (int, int) ;
    int  Robot2D_hl_prepare     (int, int) ;
    int  Robot2D_lh_prepare     (int, int) ;
    int  Robot2D_hlh_prepare    (int) ;
    int  Robot2D_tsk_ll         (Robot2D_link *) ;

    int  Robot2D_test_angle     (double, double) ;            /* Проверка "близости" углов */
 double  Robot2D_cnv_angle      (double) ;                    /* Приведение угла к диапазону -180...180 */

