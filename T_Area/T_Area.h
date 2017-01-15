
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ПОВЕРХНОСТИ"    		    */
/*								    */
/********************************************************************/

#ifdef T_AREA_EXPORTS
#define T_AREA_API __declspec(dllexport)
#else
#define T_AREA_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */


/*------------------------------------- Описание записей индикаторов */

#define       _ANGLES_MAX    5
#define  _ANGLES_LIST_MAX    8
#define          _IND_MAX  128

  typedef  struct {                     /* Описание углового диапазона */
                     double  a_lst[_ANGLES_MAX] ;
                        int  a_lst_cnt ;
                     double  a_min ;
                     double  a_max ;
                       char  a_lst_text[128] ;
                       char  a_min_text[32] ;
                       char  a_max_text[32] ;
                        int  type ;
#define                        _INLIST_RANGE     0
#define                            _OR_RANGE     1
#define                           _AND_RANGE     2
                        int  base ;
#define                      _ABSOLUTE_VALUE     0
#define                        _NORMAL_VALUE     1
                  } RSS_Module_Area_Range ;

  typedef  struct {                     /* Управление сканир.углового диапазона */
      RSS_Module_Area_Range *range ;                       
                     double *angle ;
                        int  idx ;
                        int  idx_max ;
                        int  type ;
//#define                          _OR_RANGE     1
//#define                         _AND_RANGE     2
                  } RSS_Module_Area_Scan ;

  typedef  struct {                     /* Описание 3D углового сектора */
                        int  use_flag ;
                        int  done_flag ;
                   COLORREF  color ;

      RSS_Module_Area_Range  azim_range ;
      RSS_Module_Area_Range  elev_range ;
      RSS_Module_Area_Range  roll_range ;

                        int  priority_type ;
#define                        _A_E_R_PRIO     0
#define                        _A_R_E_PRIO     1
#define                        _E_A_R_PRIO     2
#define                        _E_R_A_PRIO     3
#define                        _R_A_E_PRIO     4
#define                        _R_E_A_PRIO     5

       RSS_Module_Area_Scan  scan[3] ;

                  } RSS_Module_Area_Angles ;

  typedef  struct {
                     char  name[128] ;                 /* Название */
                      int  type ;                      /* Тип индикатора: */
#define                       _FLAT_IND   0             /* Плоский */
#define                   _CYLINDER_IND   1             /* Цилиндрический */
                RSS_Point  base ;                      /* Базовая точка */
                   double  size1 ;                     /* Размерные параметры */
                   double  size2 ;
                   double  cell ;                      /* Размер ячейки */

   RSS_Module_Area_Angles  angles[_ANGLES_LIST_MAX] ;  /* Таблица угловых 3D-секторов */
                   double  angle_step ;                /* Шаг сканировния углов */
                      int  angles_idx ;                /* Индекс рабочего 3D-секторa */

                 COLORREF *data ;                      /* Индикаторный список */
                      int  data_cnt ;

//                    int  cnt_1 ;

                      int  move_flag ;       /* Флаг подготовки индикатора текущего состояния */
                      int  formed_flag ;     /* Флаг сформированного индикатора */

    class RSS_Module_Area *context ;         /* Контекст привязки */
                      int  idx ;             /* Индекс привязки */
                      int  dlist1_idx ;      /* Индексы дисплейных списков */
                      int  dlist2_idx ;

                  } RSS_Module_Area_Indicator ;

/*-------------------- Описание класса объекта "Поверхность анализа" */

  class T_AREA_API RSS_Object_Area : public RSS_Object {

    public:
        RSS_Module_Area_Indicator *Indicator ;           /* Ссылка на индикатор */

    public:
               virtual void  vFree     (void) ;          /* Освободить ресурсы */
               virtual void  vWriteSave(std::string *) ; /* Записать данные в строку */

	                     RSS_Object_Area() ;         /* Конструктор */
	                    ~RSS_Object_Area() ;         /* Деструктор */
                                                       } ;

/*---------------------- Описание класса задачи "Анализ поверхности" */

  class T_AREA_API RSS_Module_Area : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Area_instr  *mInstrList ;            /* Список команд */

          RSS_Module_Area_Indicator  *mContextObject ;        /* Обьект 'контекстной' операции */
                               char   mContextAction[64] ;    /* Действие 'контекстной' операции */

                               HWND   mIndListWnd ;           /* Окно списка индикаторов */
          RSS_Module_Area_Indicator  *mIndicators[_IND_MAX] ; /* Список индикаторов */
          RSS_Module_Area_Indicator  *mIndicator_last ;       /* Ссылак на последний созданный индикатор */
                           COLORREF   mBadColor ;             /* Цвет 'недоступности' */
                           COLORREF   mGoodColor ;            /* Цвет доступности */ 
                           COLORREF   mScanColor ;            /* Цвет 'подготовки' */ 

                                int   mNoShow ;               /* Флаг запрещения отрисовки на экран */
                             double   mRange ;                /* Размерный параметр */
				     
    public:
     virtual   int  vExecuteCmd     (const char *) ;  /* Выполнить команду */
     virtual  void  vShow           (char *) ;        /* Отобразить связанные данные */
     virtual  void  vReadSave       (std::string *) ; /* Чтение данных из строки */
     virtual  void  vWriteSave      (std::string *) ; /* Записать данные в строку */
     virtual  void  vChangeContext  (void)  ;         /* Выполнить действие в контексте потока */

    public:
               int  cHelp           (char *) ;        /* Инструкция HELP */
               int  cCreate         (char *) ;        /* Инструкция CREATE */ 
               int  cCopy           (char *) ;        /* Инструкция COPY */
               int  cBase           (char *) ;        /* Инструкция BASE */
               int  cAngle          (char *) ;        /* Инструкция ANGLE */
               int  cSize           (char *) ;        /* Инструкция SIZE */
               int  cTable          (char *) ;        /* Инструкция TABLE */
               int  cExecute        (char *) ;        /* Инструкция EXECUTE */
               int  cIndicatorList  (char *) ;        /* Инструкция INDICATOR_LIST */
               int  cIndicatorKill  (char *) ;        /* Инструкция INDICATOR_KILL */

#define  INDICATOR  RSS_Module_Area_Indicator

         INDICATOR *FindIndicator   (char *) ;        /* Поиск индикатора по имени */
              void  iIndicatorShow  (INDICATOR *) ;   /* Отображение индикатора на сцене */
              void  iIndicatorShow_ (INDICATOR *) ;   /* Отображение индикатора с передачей контекста */
              void  iIndicatorHide  (INDICATOR *) ;   /* Удаление индикатора из сцены */
              void  iIndicatorKill  (INDICATOR *) ;   /* Удаление индикатора */
               int  iIndicatorAlloc (INDICATOR *) ;   /* Размещение ресурсов индикатора */
              char *iErrorDecode    (int) ;           /* Расшифровка кода ошибки */
               int  iReadTableFile  (INDICATOR *,
                                          char * ) ;
               int  iGetNextPoint   (INDICATOR *,
                                      int, RSS_Point *,
                                      int *, COLORREF *, int) ;
              void  iPrepareAngles  (INDICATOR *, 
                                     RSS_Point * ) ;
              void  iResetAngles    (INDICATOR *) ;
               int  iAnalisePrevious(INDICATOR *, int,
                                      COLORREF *      ) ;
              void  iGetNextAngle   (INDICATOR *) ;

#undef  INDICATOR

    public:
	                  RSS_Module_Area() ;                 /* Конструктор */
	                 ~RSS_Module_Area() ;                 /* Деструктор */
                                                         } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Area_instr {

           char                   *name_full ;         /* Полное имя команды */
           char                   *name_shrt ;         /* Короткое имя команды */
           char                   *help_row ;          /* HELP - строка */
           char                   *help_full ;         /* HELP - полный */
            int (RSS_Module_Area::*process)(char *) ;  /* Процедура выполнения команды */
                              }  ;
/*--------------------------------------- Команды диалогового обмена */

#define  _TASK_AREA_ADD_INDICATOR    1
#define  _TASK_AREA_KILL_INDICATOR   2

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Area.cpp */
       RSS_Kernel *GetTaskAreaEntry(void) ;

/* Файл  T_Area_dialog.cpp */
    BOOL CALLBACK  Task_Area_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Area_IList_dialog (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Area_Angles_dialog(HWND, UINT, WPARAM, LPARAM) ;

/* Файл  T_Area_color.cpp */
         COLORREF  Task_Area_color_choice (COLORREF) ;
         COLORREF  Task_Area_set_color    (COLORREF) ;
    BOOL CALLBACK  Task_Area_color_dialog (HWND, UINT, WPARAM, LPARAM) ;

