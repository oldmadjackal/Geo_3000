
/********************************************************************/
/*								    */
/*          МОДУЛЬ ЗАДАЧИ "ПОСТОРЕНИЕ СГЛАЖЕННОЙ ТРАЕКТОРИИ"        */
/*								    */
/********************************************************************/

#ifdef T_SPLINE_EXPORTS
#define T_SPLINE_API __declspec(dllexport)
#else
#define T_SPLINE_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

#define     _TIME_MINIMIZE      0
#define        _V_CORRECTION    1

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*------------------------------------------------------- Траектория */

 typedef struct {                           /* Раскладка закона движения на участке */
                   double  a1 ;
                   double  t1 ;
                   double  a2 ;
                   double  t2 ;
                   double  a3 ;
                } RSS_Module_Spline_Segment ;

#define  SEGMENT  RSS_Module_Spline_Segment

 typedef struct {                           /* Элемент трубки траектории */
                    RSS_Point  p0 ;
                    RSS_Point  p1 ;
                          int  rad ;
                          int  mark ;
                } RSS_Module_Spline_Pipe ;

/*-------------------------------------------------- Контекст модуля */

 typedef struct {
                     char  name[64] ;         /* Название привода */
                   double  v_max ;            /* Макс.скорость привода */
                   double  a_max ;            /* Макс.ускорение привода */
                     char  f_err[1024] ;      /* Функция точности привода */ 

                      int  err_reset ;        /* Признак сброса контекста рассчета ошибки */
                     void *calculate ;        /* Контекст вычислителя */
                   double  d_err ;            /* Величина ошибки */
                   double  v ;                /* Скорость привода */
                   double  a ;                /* Ускорение привода */

   RSS_Module_Spline_Pipe *ErrorPipe ;        /* Данный трубки ошибок */
                      int  ErrorPipe_cnt ;
                 COLORREF  ErrorPipeColor ;   /* Цвет трубки ошибок */
                      int  dlist1_idx ;       /* Индексы дисплейных списков */

                } RSS_Module_Spline_Context ;

/*-------- Описание класса задачи "Построение сглаженной траектории" */

typedef class RSS_Object_Path  PATH ;

  class T_SPLINE_API RSS_Module_Spline : public RSS_Kernel {

    public:
              class RSS_Module_Path *mModulePath ;                     /* Сыслка на модуль Path */
                          RSS_Joint  mDrives[50] ;                     /* Описание степеней подвижности анализируемого объекта */
                                int  mDrives_cnt ;

    static
     struct RSS_Module_Spline_instr *mInstrList ;                       /* Список команд */

                               char  mContextAction[64] ;               /* Действие 'контекстной' операции */
                         TRAJECTORY *mContextObject ;                   /* Обьект 'контекстной' операции */

                                int  mTurnZone_coef ;                   /* Относительная длина зоны перехода */
                             double  mLineStep ;                        /* Шаг по линейным координатам */ 
                             double  mAngleStep ;                       /* Шаг по угловым координатам */ 

                             double  mAccuracy[32] ;                    /* Контрольная точность рассчета */

                                int  mShowErrorPipe ;                   /* Показывать трубку ошибок или нет */
                                int  mAdjustErrorPipe ;                 /* Подстраивать трубку ошибок или нет */
                                int  mSaveGlided ;                      /* Сохранять или нет сглаженную траекторию */
                                int  mIndicate ;                        /* Показывать информацию о процессе или нет */

             RSS_Module_Spline_Pipe *mErrorPipe ;                       /* Набор точек трубки ошибок */
                                int  mErrorPipe_cnt ;
                                int  mErrorPipe_max ;
                           COLORREF  mErrorPipeColor ;                  /* Цвет линии траектории */

                                int  mAttempt ;                         /* Счетчик "попыток" */

                                int  mDebug_level ;                     /* Уровень отладочной печати */
                       static  long  mDebug_stamp ;                     /* Отадочная метка */

    public:
             virtual     void  vInit          (void) ;                  /* Инициализация связей */
             virtual      int  vExecuteCmd    (const char *) ;          /* Выполнить команду */
             virtual      int  vExecuteCmd    (const char *, RSS_IFace *) ;
             virtual     void  vShow          (char *) ;                /* Отобразить связанные данные */
             virtual     void  vReadSave       (std::string *) ;        /* Чтение данных из строки */
             virtual     void  vWriteSave      (std::string *) ;        /* Записать данные в строку */
             virtual     void  vChangeContext (void)  ;                 /* Выполнить действие в контексте потока */
             virtual      int  vReadData      (RSS_Context ***,         /* Считать данные контекста модуля из строки */
                                               std::string *   ) ;
             virtual     void  vWriteData     (RSS_Context *,           /* Записать данные контекста модуля в строку */
                                               std::string * ) ;
             virtual     void  vReleaseData   (RSS_Context *) ;         /* Освободить ресурсы данных контекста модуля */
             virtual      int  vGetParameter  (char *, char *) ;        /* Получить параметр */
                   
    public:         
                          int  cHelp          (char *, RSS_IFace *) ;   /* Инструкция HELP */ 
                          int  cConfig        (char *, RSS_IFace *) ;   /* Инструкция CONFIG */ 
                          int  cDrive         (char *, RSS_IFace *) ;   /* Инструкция DRIVE */ 
                          int  cGlide         (char *, RSS_IFace *) ;   /* Инструкция GLIDE */ 
                          int  cTrace         (char *, RSS_IFace *) ;   /* Инструкция TRACE */ 

                          int  iDrawPath      (PATH *, RSS_Object *);   /* Отрисовка траектории */
                          int  iCheckPath     (PATH *) ;                /* Контрольная трассировка траектории */
                         PATH *iCopyPath      (PATH *, char * ) ;       /* Копирование траектории */
                          int  iTurnZone_check(RSS_Object_Path *,       /* Определение допустимых переходных зон */
                                               RSS_Object_Path * ) ;  
                          int  iLineSegment   (int, double *, double *, /* Построение движения на линейном участке */
                                                    double *,
                                                    double  , double *,
                                                   SEGMENT *, double *,
                                                 RSS_Module_Spline_Context **) ;  
                          int  iTurnSegment   (int, double *, double *, /* Построение движения на спряжении прямых участков */
                                                    double *, double *,
                                                    double  , double *,
                                                   SEGMENT *, double *,
                                                 RSS_Module_Spline_Context **) ;  
                          int  iTraceSegment  (int, double  , double *, /* Движение по участку траектории */
                                                    double *, SEGMENT *,
                                                 RSS_Module_Spline_Context **,
                                                                RSS_Object *  ) ;
                          int  iSaveErrorPipe (int, RSS_Point *,        /* Сохранение точки трубки ошибок */ 
                                                    RSS_Point * ) ;
                          int  iCutErrorPipe  (int) ;                   /* Обрезание трубки ошибок до данного сегмента */
                         void  iErrorPipeShow_(TRAJECTORY *) ;          /* Отображение трубки ошибок */
                         void  iErrorPipeShow (TRAJECTORY *) ;
                                                                        /* Работа с формулой ошибки привода */
                          int  iErrorCalc     (RSS_Module_Spline_Context *,  
                                                               char * ) ;
                       double  iAngleDif      (double) ;                /* Вычисление разности углов */
                         void  iDebug         (char *, char *, int) ;   /* Отладочная печать в файл */ 
                         void  iDebug         (char *, char *) ;

    public:
  	                       RSS_Module_Spline() ;                 /* Конструктор */
	                      ~RSS_Module_Spline() ;                 /* Деструктор */
                                                           } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Spline_instr {

            char                     *name_full ;                      /* Полное имя команды */
            char                     *name_shrt ;                      /* Короткое имя команды */
            char                     *help_row ;                       /* HELP - строка */
            char                     *help_full ;                      /* HELP - полный */
             int (RSS_Module_Spline::*process)(char *, RSS_IFace *) ;  /* Процедура выполнения команды */
                                }  ;
/*---------------------------------------------- Специальные статусы */

#define   _FATAL_ERROR   -1    /* Критическая ошибка */
#define    _PATH_FAIL    -2    /* Коллизия траектории */
#define    _PIPE_FAIL    -3    /* Коллизия трубки ошибок */

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Spline.cpp */

/* Файл  T_Spline_dialog.cpp */
    BOOL CALLBACK  Task_Spline_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Spline_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Spline_Drives_dialog(HWND, UINT, WPARAM, LPARAM) ;
