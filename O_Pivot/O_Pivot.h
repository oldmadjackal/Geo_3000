
/********************************************************************/
/*								    */
/*                  МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ        		    */
/*      ПЛОСКИЙ МАНИПУЛЯТОР НА ВРАЩАЮЩИМСЯ ОСНОВАНИИ С              */
/*            2-Х СТЕПЕННОЙ ПРОСТРАНСТВЕННОЙ КИСТЬЮ                 */
/*								    */
/********************************************************************/

#ifdef O_PIVOT_EXPORTS
#define O_PIVOT_API __declspec(dllexport)
#else
#define O_PIVOT_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

#define            _NO_KEYS   0
#define            _LR_KEYS   1
#define            _UD_KEYS   2
#define       _CTRL_LR_KEYS   3
#define       _CTRL_UD_KEYS   4
#define         _SH_LR_KEYS   5
#define         _SH_UD_KEYS   6
#define    _SH_CTRL_LR_KEYS   7
#define    _SH_CTRL_UD_KEYS   8

/*--------------------------------------------- Транзитные структуры */

  typedef  struct {
                           char  name[256] ;               /* Название обьекта */
                           char  scheme[256] ;             /* Спецификация схемы */
                  } RSS_Module_Pivot_Create_data ;

/*--------------------------------------------------- Описание звена */

#define   _BODIES_MAX   10

  typedef  struct {
                       int  use_flag ;
                      char  desc[256] ;            /* Описание тела */
                       int  change ;               /* Флаг наличия изменений в описании тела */

            RSS_Model_data  model ;                /* Описание модели */
      struct RSS_Parameter *pars ;                 /* Список параметров */

                    double  x_base ;               /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;

                    double  zero_size ;            /* 0-длина */       

                  } RSS_Object_Pivot_Body ;

  typedef  struct {
                       int  use_flag ;
                      char  desc[256] ;            /* Описание тела */
                       int  change ;               /* Флаг наличия изменений в описании тела */

                    double  mass ;                 /* Масса */
                    double  Jx  ;                  /* Главные моменты инерции */
                    double  Jy  ;
                    double  Jz  ;

                    double  x_base ;               /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;

                    double  move_coef ;            /* Коэффициент сдвига при изменении длины звена */       

                  } RSS_Object_Pivot_Mass ;

  typedef  struct {
                       int  use_flag ;             /* Флаг использования */
                      char  name[256] ;            /* Название звена */

                    double  joint ;                /* Углы в шарнире */
                       int  joint_fixed ;          /* Флаг подвижного шарнира */
                       int  joint_keys ;           /* Набор управляющих клавиш */
                    double  joint_min ;            /* Диапазон углов в шарнире */
                    double  joint_max ;
                       int  joint_test ;           /* Флаг тестирования угла в шарнире на диапазон */

                    double  link ;                 /* Длина звена */
                       int  link_fixed ;           /* Флаг подвижного звена */
                       int  link_keys ;            /* Набор управляющих клавиш */
                    double  link_min ;             /* Диапазон длины звена */
                    double  link_max ;
                       int  link_test ;            /* Флаг тестирования длины звена на диапазон */

                       int  action ;

                    double  x_base ;               /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;               /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;
      
     RSS_Object_Pivot_Body  bodies[_BODIES_MAX] ;  /* Список связанных тел */
     RSS_Object_Pivot_Mass  masses[_BODIES_MAX] ;  /* Список связанных масс */

                RSS_Object *parent ;               /* Ссылка на породивший объект */
                  } RSS_Object_Pivot_Link ;

/*------------------------------------------ Описание класса объекта */

#define   _LINKS_MAX   8

  class O_PIVOT_API RSS_Object_Pivot : public RSS_Object {

    public:
                    double  x_base ;                       /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                       /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;

                      char  scheme[256] ;                  /* Спецификация схемы */

     RSS_Object_Pivot_Link  link_base ;                    /* База */
     RSS_Object_Pivot_Link  link_leg ;                     /* Повротная колонна */

     RSS_Object_Pivot_Link  link_wrist ;                   /* Запястье */
     RSS_Object_Pivot_Link  link_gripper ;                 /* Схвата */

     RSS_Object_Pivot_Link  links[_LINKS_MAX] ;            /* Таблица звеньев */

                 RSS_Point  target ;                       /* Целевая точка */

                       int  ambiguity_done ;               /* Флаг заданности критериев разрешения неопределенности */
                       int  ambiguity_cnt ;                /* Число неопределенностей схемы */
                       int  wrist_ambiguity ;              /* Критерии разрешения неопределенности */
                       int  arm_ambiguity ;

    public:
                        int  DissectScheme  (void) ;          /* Разводка схемы */

               virtual  int  vGetTarget     (char *,          /* Выдать координаты 'целевой' точки */
                                             RSS_Point *) ;
               virtual  int  vGetTargetPoint(RSS_Point *) ;   /* Работа с целевой точкой */
               virtual void  vSetTargetPoint(RSS_Point *, int) ;
               virtual  int  vGetAmbiguity  (void) ;          /* Работа с неопределенностями схемы */
               virtual void  vSetAmbiguity  (char *) ;
               virtual  int  vSolveByTarget (void) ;          /* Разрешить структуру по целевой точке */ 
               virtual  int  vSolveByJoints (void) ;          /* Разрешить структуру по сочленениям */
               virtual  int  vGetJoints     (RSS_Joint *) ;   /* Работа с сочленениями */
               virtual void  vSetJoints     (RSS_Joint *, int) ;
               virtual void  vWriteSave     (std::string *) ; /* Записать данные в строку */

      RSS_Object_Pivot_Link *iGetLinkByName (char *) ;        /* Выдача звена обьекта по имени */
      RSS_Object_Pivot_Body *iGetBodyByName (char *, char *); /* Выдача тела звена по имени */
      RSS_Object_Pivot_Mass *iGetMassByName (char *, char *); /* Выдача массы звена по имени */
                                            
                       void  iClearUpdates  (void) ;          /* Сброс флагов изменений */
                        int  iLinkFeatures  (RSS_Object_Pivot_Link *) ; 
                                                              /* Пропись свойств звена */
                       void  iRobotSkeleton (void) ;          /* Пропись структурной геометрии манипулятора */ 
                       void  iLinkSkeleton  (RSS_Object_Pivot_Link *, Matrix2d *) ;
                                                              /* Пропись структурной геометрии звена */ 
                        int  iRobotDegrees  (int) ;           /* Тестирование степеней подвижности манипулятора по диапазонам */ 
                        int  iLinkDegrees   (RSS_Object_Pivot_Link *,
                                             char *) ;        /* Тестирование степеней подвижности звена по диапазонам */ 
                        int  iAngleTest     (double,          /* Проверка соответствия угла диапазону */
                                             double, double) ;

	                     RSS_Object_Pivot() ;             /* Конструктор */
	                    ~RSS_Object_Pivot() ;             /* Деструктор */
                                                         } ;

/*------------------------------ Описание класса управления объектом */

  class O_PIVOT_API RSS_Module_Pivot : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Pivot_instr *mInstrList ;           /* Список команд */

    public:
     virtual        void  vStart         (void) ;           /* Стартовая разводка */
     virtual         int  vExecuteCmd    (const char *) ;   /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;   /* Чтение данных из строки */
     virtual        void  vWriteSave     (std::string *) ;  /* Записать данные в строку */

    public:
                     int  cHelp          (char *) ;         /* Инструкция HELP */
                     int  cCreate        (char *) ;         /* Инструкция CREATE */
                     int  cInfo          (char *) ;         /* Инструкция INFO */
                     int  cCheck         (char *) ;         /* Инструкция CHECK */
                     int  cBase          (char *) ;         /* Инструкция BASE */
                     int  cAngle         (char *) ;         /* Инструкция ANGLE */
                     int  cDegree        (char *) ;         /* Инструкция DEGREE */
                     int  cLink          (char *) ;         /* Инструкция LINK */
                     int  cJoint         (char *) ;         /* Инструкция JOINT */
                     int  cRange         (char *) ;         /* Инструкция RANGE */
                     int  cKeys          (char *) ;         /* Инструкция KEYS */
                     int  cBody          (char *) ;         /* Инструкция BODY */
                     int  cBodyAdd       (char *) ;         /* Инструкция BODY_ADD */
                     int  cBodyModel     (char *) ;         /* Инструкция BODY_MODEL */
                     int  cBodyShift     (char *) ;         /* Инструкция BODY_SHIFT */
                     int  cBodyZero      (char *) ;         /* Инструкция BODY_ZERO */
                     int  cMass          (char *) ;         /* Инструкция MASS */
                     int  cMassAdd       (char *) ;         /* Инструкция MASS_ADD */
                     int  cMassShift     (char *) ;         /* Инструкция MASS_SHIFT */
                     int  cMassMove      (char *) ;         /* Инструкция MASS_MOVE */

        RSS_Object_Pivot *FindObject     (char *) ;         /* Поиск обьекта типа PIVOT по имени */
                     int  CreateObject   (RSS_Module_Pivot_Create_data *) ;
                                                            /* Создание объекта */ 
                     int  CreateBody     (RSS_Object_Pivot      *,
                                          RSS_Object_Pivot_Link *,
                                          RSS_Object_Pivot_Body * ) ;
                                                            /* Создание тела */
                     int  CreateMass     (RSS_Object_Pivot      *,
                                          RSS_Object_Pivot_Link *,
                                          RSS_Object_Pivot_Mass * ) ;
                                                            /* Создание массы */

                     int  AmbiguityDialog(RSS_Object_Pivot *) ;
                                                            /* Диалог разрешения неопределенностей схемы */

    public:
	                  RSS_Module_Pivot() ;              /* Конструктор */
	                 ~RSS_Module_Pivot() ;              /* Деструктор */
                                                         } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Pivot_instr {

           char                    *name_full ;         /* Полное имя команды */
           char                    *name_shrt ;         /* Короткое имя команды */
           char                    *help_row ;          /* HELP - строка */
           char                    *help_full ;         /* HELP - полный */
            int (RSS_Module_Pivot::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Pivot.cpp */
   RSS_Kernel *GetObjectPivotEntry(void) ;    /* Получение ссылки на программный модуль */

/* Файл  O_Pivot_dialog.cpp */
    BOOL CALLBACK  Object_Pivot_Help_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Create_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Degree_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Range_dialog     (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Keys_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Bodies_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_BodyAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Masses_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_MassAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Pivot_Ambiguity_dialog (HWND, UINT, WPARAM, LPARAM) ;

 LRESULT CALLBACK  Object_Pivot_Structure_prc    (HWND, UINT, WPARAM, LPARAM) ;
