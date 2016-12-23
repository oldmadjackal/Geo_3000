
/********************************************************************/
/*								    */
/*                  МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ        		    */
/*                   "ЛЕТАЮЩАЯ" СТРУКТУРА ТЕЛ                       */
/*								    */
/********************************************************************/

#ifdef O_FLYER_EXPORTS
#define O_FLYER_API __declspec(dllexport)
#else
#define O_FLYER_API __declspec(dllimport)
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
//                         char  scheme[256] ;             /* Спецификация схемы */
                  } RSS_Module_Flyer_Create_data ;

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

                  } RSS_Object_Flyer_Body ;

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

                  } RSS_Object_Flyer_Mass ;

  typedef  struct {
                       int   use_flag ;             /* Флаг использования */
                      char   name[256] ;            /* Название звена */

                    double   value ;                /* Величина */
                       int   value_fixed ;          /* Флаг подвижности */
                       int   value_keys ;           /* Набор управляющих клавиш */
                    double   value_min ;            /* Диапазон величин */
                    double   value_max ;
                       int   value_test ;           /* Флаг тестирования */

                       int   action ;

                    double   x_base ;               /* Координаты базовой точки */
                    double   y_base ;
                    double   z_base ;

                    double   a_azim ;               /* Углы ориентации */
                    double   a_elev ;
                    double   a_roll ;
      
     RSS_Object_Flyer_Body   bodies[_BODIES_MAX] ;  /* Список связанных тел */
     RSS_Object_Flyer_Mass   masses[_BODIES_MAX] ;  /* Список связанных масс */

                RSS_Object  *parent ;               /* Ссылка на породивший объект */

        struct RSS_Context **contexts ;             /* Список контекстных данных модулей */

                  } RSS_Object_Flyer_Link ;

/*------------------------------------------ Описание класса объекта */

//#define   _LINKS_MAX   8

  class O_FLYER_API RSS_Object_Flyer : public RSS_Object {

    public:
                    double  x_base ;                       /* Координаты базовой точки */
                    double  y_base ;
                    double  z_base ;

                    double  a_azim ;                       /* Углы ориентации */
                    double  a_elev ;
                    double  a_roll ;

                      char  scheme[256] ;                  /* Спецификация схемы */

     RSS_Object_Flyer_Link  link_base ;                    /* Звено - база */
     RSS_Object_Flyer_Link  link_x ;
     RSS_Object_Flyer_Link  link_y ;
     RSS_Object_Flyer_Link  link_z ;
     RSS_Object_Flyer_Link  link_azim ;
     RSS_Object_Flyer_Link  link_elev ;
     RSS_Object_Flyer_Link  link_roll ;
     RSS_Object_Flyer_Link  link_head ;                    /* Звено - схват */

                 RSS_Point  target ;                       /* Целевая точка */

    public:
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

      RSS_Object_Flyer_Link *iGetLinkByName (char *) ;        /* Выдача звена обьекта по имени */
      RSS_Object_Flyer_Body *iGetBodyByName (char *, char *); /* Выдача тела звена по имени */
      RSS_Object_Flyer_Mass *iGetMassByName (char *, char *); /* Выдача массы звена по имени */
                                            
                       void  iClearUpdates  (void) ;          /* Сброс флагов изменений */
                        int  iLinkFeatures  (RSS_Object_Flyer_Link *) ; 
                                                              /* Пропись свойств звена */
                       void  iRobotSkeleton (void) ;          /* Пропись структурной геометрии манипулятора */ 
                       void  iLinkSkeleton  (RSS_Object_Flyer_Link *, Matrix2d *) ;
                                                              /* Пропись структурной геометрии звена */ 
                        int  iRobotDegrees  (int) ;           /* Тестирование степеней подвижности манипулятора по диапазонам */ 
                        int  iLinkDegrees   (RSS_Object_Flyer_Link *,
                                             char *) ;        /* Тестирование степеней подвижности звена по диапазонам */ 
                        int  iAngleTest     (double,          /* Проверка соответствия угла диапазону */
                                             double, double) ;

                       void  iDebug         (char *,          /* Отладочная печать в файл */ 
                                             char *, int) ;   
                       void  iDebug         (char *, char *) ;
  
	                     RSS_Object_Flyer() ;             /* Конструктор */
	                    ~RSS_Object_Flyer() ;             /* Деструктор */
                                                         } ;

/*------------------------------ Описание класса управления объектом */

  class O_FLYER_API RSS_Module_Flyer : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Flyer_instr *mInstrList ;                  /* Список команд */

    public:
     virtual        void  vStart         (void) ;                  /* Стартовая разводка */
     virtual         int  vExecuteCmd    (const char *) ;          /* Выполнить команду */
     virtual        void  vReadSave     (std::string *) ;          /* Чтение данных из строки */
     virtual        void  vWriteSave     (std::string *) ;         /* Записать данные в строку */

    public:
                     int  cHelp          (char *) ;                /* Инструкция HELP */
                     int  cCreate        (char *) ;                /* Инструкция CREATE */
                     int  cInfo          (char *) ;                /* Инструкция INFO */
                     int  cBase          (char *) ;                /* Инструкция BASE */
                     int  cAngle         (char *) ;                /* Инструкция ANGLE */
                     int  cDegree        (char *) ;                /* Инструкция DEGREE */
                     int  cLink          (char *) ;                /* Инструкция LINK */
                     int  cJoint         (char *) ;                /* Инструкция JOINT */
                     int  cRange         (char *) ;                /* Инструкция RANGE */
                     int  cKeys          (char *) ;                /* Инструкция KEYS */
                     int  cBody          (char *) ;                /* Инструкция BODY */
                     int  cBodyAdd       (char *) ;                /* Инструкция BODY_ADD */
                     int  cBodyModel     (char *) ;                /* Инструкция BODY_MODEL */
                     int  cBodyShift     (char *) ;                /* Инструкция BODY_SHIFT */
                     int  cBodyZero      (char *) ;                /* Инструкция BODY_ZERO */
                     int  cMass          (char *) ;                /* Инструкция MASS */
                     int  cMassAdd       (char *) ;                /* Инструкция MASS_ADD */
                     int  cMassShift     (char *) ;                /* Инструкция MASS_SHIFT */
                     int  cMassMove      (char *) ;                /* Инструкция MASS_MOVE */

        RSS_Object_Flyer *FindObject     (char *) ;                /* Поиск обьекта типа FLYER по имени */
                     int  CreateObject   (RSS_Module_Flyer_Create_data *) ;
                                                                   /* Создание объекта */ 
                     int  CreateBody     (RSS_Object_Flyer      *,
                                          RSS_Object_Flyer_Link *,
                                          RSS_Object_Flyer_Body * ) ;
                                                                   /* Создание тела */
                     int  CreateMass     (RSS_Object_Flyer      *,
                                          RSS_Object_Flyer_Link *,
                                          RSS_Object_Flyer_Mass * ) ;
                                                                   /* Создание массы */
                    void  iDebug         (char *, char *, int) ;   /* Отладочная печать в файл */                                              
                    void  iDebug         (char *, char *) ;

    public:
	                  RSS_Module_Flyer() ;                     /* Конструктор */
	                 ~RSS_Module_Flyer() ;                     /* Деструктор */
                                                         } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Flyer_instr {

           char                    *name_full ;         /* Полное имя команды */
           char                    *name_shrt ;         /* Короткое имя команды */
           char                    *help_row ;          /* HELP - строка */
           char                    *help_full ;         /* HELP - полный */
            int (RSS_Module_Flyer::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_Flyer.cpp */
   RSS_Kernel *GetObjectFlyerEntry(void) ;    /* Получение ссылки на программный модуль */

/* Файл  O_Flyer_dialog.cpp */
    BOOL CALLBACK  Object_Flyer_Help_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Degree_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Range_dialog     (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Keys_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Bodies_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_BodyAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_Masses_dialog    (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Object_Flyer_MassAssign_dialog(HWND, UINT, WPARAM, LPARAM) ;
