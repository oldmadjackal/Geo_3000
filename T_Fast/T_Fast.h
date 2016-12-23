
/********************************************************************/
/*								    */
/*      МОДУЛЬ ЗАДАЧИ "ОПТИМИЗАЦИЯ ТРАЕКТОРИИ ПО БЫСТРОДЕЙСТВИЮ"    */
/*								    */
/********************************************************************/

#ifdef T_FAST_EXPORTS
#define T_FAST_API __declspec(dllexport)
#else
#define T_FAST_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*-------------------------------------------------- Контекст модуля */

 typedef struct {
                     char  dummy[64] ;         /* Название привода */

                } RSS_Module_Fast_Context ;

/*------- Описание класса "Оптимизация траектории по быстродействию" */

typedef class RSS_Object_Path  PATH ;

  class T_FAST_API RSS_Module_Fast : public RSS_Kernel {

    public:
              class RSS_Module_Path *mModulePath ;                     /* Сыслка на модуль Path */

    static
       struct RSS_Module_Fast_instr *mInstrList ;                       /* Список команд */

                               char  mContextAction[64] ;               /* Действие 'контекстной' операции */
                         TRAJECTORY *mContextObject ;                   /* Обьект 'контекстной' операции */

                               char  mLineCheck_cmd[512] ;              /* Команда проверки траектории на принципиальную проходимость */
                               char  mTimeCalc_cmd[512] ;               /* Команда вычисления времени прохождения траектории */
                               char  mLineDraw_cmd[512] ;               /* Команда отрисовки траектории */

                                int  mSimplification_opt ;              /* Минимизация числа используемых степеней на участке */
                                int  mSimplification_method ;
#define                              _SEQUENCE_METHOD   0
#define                                _RANDOM_METHOD   1
#define                                  _FULL_METHOD   2
                                int  mSmoothing_opt ;                   /* Уменьшение кривизны изломов */
                                int  mCutSegment_opt ;                  /* Дробление участков */
                             double  mCutSegment_MinTime ;              /* Минимальное время прохождения дробимых участков */

                                int  mDebug_level ;                     /* Уровень отладочной печати */

    public:
             virtual     void  vInit          (void) ;                  /* Инициализация связей */
             virtual      int  vExecuteCmd    (const char *) ;          /* Выполнить команду */
             virtual      int  vExecuteCmd    (const char *, RSS_IFace *) ;
             virtual     void  vShow          (char *) ;                /* Отобразить связанные данные */
             virtual     void  vChangeContext (void)  ;                 /* Выполнить действие в контексте потока */
             virtual     void  vReadSave      (std::string *) ;         /* Чтение данных из строки */
             virtual     void  vWriteSave     (std::string *) ;         /* Записать данные в строку */
             virtual      int  vReadData      (RSS_Context ***,         /* Считать данные контекста модуля из строки */
                                               std::string *   ) ;
             virtual     void  vWriteData     (RSS_Context *,           /* Записать данные контекста модуля в строку */
                                               std::string * ) ;
             virtual      int  vGetParameter  (char *, char *) ;        /* Получить параметр */
                    
    public:         
                          int  cHelp          (char *, RSS_IFace *) ;   /* Инструкция HELP */ 
                          int  cConfig        (char *, RSS_IFace *) ;   /* Инструкция CONFIG */ 
                          int  cOptimize      (char *, RSS_IFace *) ;   /* Инструкция OPTIMIZE */ 

                         void  iDebug         (char *, char *) ;        /* Отладочная печать в файл */ 
                         PATH *iCopyPath      (PATH *, char *) ;        /* Копирование траектории */
                         void  iExecute       (char *, RSS_IFace *) ;   /* Выполнение команды */

                         void  iText_subst    (char *, char *, char *); /* Выполнение форматной подстановки */

                          int  iMethodSimplif (PATH **,   PATH **,      /* Оптимизация путем минимизации числа */
                                                int * ,    int * ,      /*   используемых на участке степеней  */
                                               RSS_IFace *, double *  ) ;
                          int  iMethodCut     (PATH **,   PATH **,      /* Оптимизация путем дробления участков */
                                                int * ,    int * ,
                                               RSS_IFace *, double *  ) ;

    public:
  	                       RSS_Module_Fast() ;                 /* Конструктор */
	                      ~RSS_Module_Fast() ;                 /* Деструктор */
                                                           } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Fast_instr {

            char                   *name_full ;                     /* Полное имя команды */
            char                   *name_shrt ;                     /* Короткое имя команды */
            char                   *help_row ;                      /* HELP - строка */
            char                   *help_full ;                     /* HELP - полный */
             int (RSS_Module_Fast::*process)(char *, RSS_IFace *) ; /* Процедура выполнения команды */
                              }  ;
/*---------------------------------------------- Специальные статусы */


/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Fast.cpp */

/* Файл  T_Fast_dialog.cpp */
    BOOL CALLBACK  Task_Fast_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Fast_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
