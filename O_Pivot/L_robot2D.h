
/*-------------------------------------- �������� ����� ������������ */

typedef struct {
                   int  joint_fixed ;  /* ���� �������������� ������� */
                   int  link_fixed ;   /* ���� ������������� ����� ����� */

                double  joint ;        /* ���� � ��p��p� */
                double  joint_min ;
                double  joint_max ;
                double  link ;         /* ����� ����� */
                double  link_min ;
                double  link_max ;

                double  baby ;         /* ��������� ��������� ����� */
                double  angle ;        /* ���� �p�������� */
                double  length ;       /* ��������� */
                double  x1 ;           /* ���p������  ������ ����� */
                double  y1 ;
                double  x2 ;
                double  y2 ;

	       } Robot2D_link ;

/*----------------------------------------- ���� �������� � �������� */

#define   _R_FORWD         1
#define   _R_FORWD_PASS    2
#define   _R_BACK          3
#define   _R_BACK_PASS     4

#define   _R_UNDEF         5
#define   _R_SQUEEZE       6
#define   _R_EXTEND        7

/*------------------------------------------------------ ���� ������ */

#define _RE_LOW_ACTIVE     -3001    /* ����� �������� ����������� ������ 3 */
#define _RE_MATCH_ACTIVE   -3002    /* ����� �������� ����������� ������ 3 */
#define _RE_BAD_SCHEME     -3003    /* ������������ ����� */
#define _RE_DEGRAD_SCHEME  -3004
#define _RE_DEGRAD_POINT   -3005
#define _RE_UNACHIEVED     -3006
#define _RE_RANGE_OUT      -3007
#define _RE_OBST_CROSS     -3008
#define _RE_LINK_CROSS     -3009
#define _RE_ZONE           -3010

/*---------------------------------------------------- ��������� �/� */

   void  Robot2D_direct_problem (int, Robot2D_link *, int) ;  /* ������� ������ �������������� ������ */ 
    int  Robot2D_inverse_problem(int, Robot2D_link *, int) ;  /* ������� �������� �������������� ������ ��� ������������ ���� */
    int  Robot2D_scheme_prepare (int, Robot2D_link *,         /* ������� �� ������ ����� � ������������ � ������� */
                                 int, Robot2D_link * ) ;

    int  Robot2D_robot_prepare  (int, Robot2D_link *) ;
    int  Robot2D_base_prepare   (int, Robot2D_link *) ;
    int  Robot2D_top_prepare    (int, int) ;
    int  Robot2D_hh_prepare     (int, int) ;
    int  Robot2D_hl_prepare     (int, int) ;
    int  Robot2D_lh_prepare     (int, int) ;
    int  Robot2D_hlh_prepare    (int) ;
    int  Robot2D_tsk_ll         (Robot2D_link *) ;

    int  Robot2D_test_angle     (double, double) ;            /* �������� "��������" ����� */
 double  Robot2D_cnv_angle      (double) ;                    /* ���������� ���� � ��������� -180...180 */

