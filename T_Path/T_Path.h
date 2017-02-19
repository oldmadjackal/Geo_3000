
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ПОИСК БАЗОВОЙ ТРАЕКТОРИИ" 	    */
/*								    */
/********************************************************************/

#ifdef T_PATH_EXPORTS
#define T_PATH_API __declspec(dllexport)
#else
#define T_PATH_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*---------------------------------------------- Описание траектории */

 struct RSS_Module_Path_Trajectory {

                     char   name[64] ;            /* Название траектории */
                     char   desc[512] ;           /* Пояснение */
                   double  *Path ;                /* Список точек рабочей траектории */
                   double  *Vectors ;             /* Список векторов рабочей траектории */
                      int   Path_vectors ;        /* Число векторов в траектории */
               RSS_Object  *Path_object ;         /* Ссылка на объект, для которого строилась траектория */
                RSS_Joint   Path_degrees[50] ;    /* Описание степеней подвижности анализируемого объекта */
                      int   Path_frame ;          /* Размер 'кадра' траектории */

                RSS_Point  *Trace ;               /* Набор точек линии трассировки */
                      int   Trace_cnt ;
                 COLORREF   TraceColor ;          /* Цвет линии траектории */

    class RSS_Module_Path  *module ;              /* Контекст привязки */
                      int   dlist1_idx ;          /* Индексы дисплейных списков */

       struct RSS_Context **contexts ;            /* Список контекстных данных модулей */

                                   }  ;

/*----------------------------- Описание класса объекта "Траектория" */

  class T_PATH_API RSS_Object_Path : public RSS_Object {

    public:
        RSS_Module_Path_Trajectory *Trajectory ;         /* Ссылка на индикатор */

    public:
               virtual void  vFree     (void) ;          /* Освободить ресурсы */
               virtual void  vWriteSave(std::string *) ; /* Записать данные в строку */

	                     RSS_Object_Path() ;         /* Конструктор */
	                    ~RSS_Object_Path() ;         /* Деструктор */
                                                       } ;

#define  TRAJECTORY  struct RSS_Module_Path_Trajectory 

/*------------------------ Описание класса задачи "Поиск траектории" */

  class T_PATH_API RSS_Module_Path : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Path_instr *mInstrList ;            /* Список команд */

         RSS_Module_Path_Trajectory *mContextObject ;        /* Обьект 'контекстной' операции */
                               char  mContextAction[64] ;    /* Действие 'контекстной' операции */

                                int  mDraw ;                 /* Отрисовывать в процессе поиска */

                                int  mRandomPerDirect ;      /* Число случайных векторов на один направленный */ 
                                int  mVectorsMax ;           /* Максимальное число пробных векторов */ 
                                int  mVectorsMax_ ;

                             double  mLineStep_user ;        /* Шаг по линейным координатам, заданный пользователем */ 
                             double  mAngleStep_user ;       /* Шаг по угловым координатам, заданный пользователем */ 
                             double  mLineStep ;             /* Шаг по линейным координатам, рабочий */ 
                             double  mAngleStep ;            /* Шаг по угловым координатам, рабочий */ 
 
                             double  mLineAcc_user ;         /* Точность определения линейных координат */ 
                             double  mAngleAcc_user ;        /* Точность определения угловых координат */ 
                             double  mLineAcc ;              /* Точность определения линейных координат */ 
                             double  mAngleAcc ;             /* Точность определения угловых координат */ 
                             double  mScanStepCoef ;         /* Делитель шага сближения */
                                int  mFailDirect ;           /* Предельное число неудачных векторов подряд */ 

                             double *mPath ;                 /* Список точек рабочей траектории */
                             double *mVectors ;              /* Список векторов рабочей траектории */
                                int  mPath_vectors ;         /* Число векторов в траектории */
                         RSS_Object *mPath_object ;          /* Ссылка на анализируемый объкт */
                          RSS_Joint  mPath_degrees[50] ;     /* Описание степеней подвижности анализируемого объекта */
                                int  mPath_frame ;           /* Размер 'кадра' траектории */

                                int  mCheck ;                /* Флаг проверки противоречивости свойств */
                                int  mTraceTop ;             /* Флаг сохранения траектории целевой точки объекта */
                          RSS_Point *mTracePoints ;          /* Набор точек линии трассировки */
                                int  mTracePoints_cnt ;
                                int  mTracePoints_max ;
                           COLORREF  mTraceColor ;           /* Цвет линии траектории */

                                int  mDebug_level ;          /* Уровень отладочной печати */
                       static  long  mDebug_stamp ;          /* Отадочная метка */

    public:
     virtual         int  vExecuteCmd     (const char *) ;      /* Выполнить команду */
     virtual         int  vExecuteCmd     (const char *, RSS_IFace *) ;
     virtual        void  vShow           (char *) ;            /* Отобразить связанные данные */
     virtual        void  vReadSave       (std::string *) ;     /* Чтение данных из строки */
     virtual        void  vWriteSave      (std::string *) ;     /* Записать данные в строку */
     virtual         int  vGetParameter   (char *, char *) ;    /* Получить параметр */
     virtual        void  vChangeContext  (void)  ;             /* Выполнить действие в контексте потока */

    public:
                     int  cHelp           (char *, RSS_IFace *) ;   /* Инструкция HELP */ 
                     int  cConfig         (char *, RSS_IFace *) ;   /* Инструкция CONFIG */ 
                     int  cSearch         (char *, RSS_IFace *) ;   /* Инструкция SEARCH */ 
                     int  cTrace          (char *, RSS_IFace *) ;   /* Инструкция TRACE */ 
                     int  cIndicate       (char *, RSS_IFace *) ;   /* Инструкция INDICATE */ 
                     int  cList           (char *, RSS_IFace *) ;   /* Инструкция LIST */ 
                     int  cUnit           (char *, RSS_IFace *) ;   /* Инструкция UNIT */ 
                     int  cCopy           (char *, RSS_IFace *) ;   /* Инструкция COPY */
                     int  cColor          (char *, RSS_IFace *) ;   /* Инструкция COLOR */

                     int  iSearchDirect   (RSS_Joint *) ;           /* Поиск пути до известной конфигурации */
                     int  iSearchReach    (RSS_Point *) ;           /* Поиск пути до целевой точки */
                    void  iReducePath     (void) ;                  /* Сократить траекторию */
                     int  iShowPath       (void) ;                  /* Показать траекторию */
                    void  iReverse        (TRAJECTORY *) ;          /* Реверс траектории */
                    void  iGenerateVector (double *) ;              /* Генерация случайного вектора */
         virtual     int  iCalculateVector(double *,                /* Вычисление вектора */
                                           double *, double *) ;
                     int  iAddPathPoint   (double *, double *) ;    /* Добавление точки в траекторию */
         virtual     int  iTraceVector    (double *, double *,      /* Трассировка вектора между конфигурациями */
                                           double *, double *) ;
                     int  iReachVector    (RSS_Point *, int);       /* Трассировка цепочки векторов к целевой точке */
         virtual    void  iCulculateSteps (void) ;                  /* Определение шагов сканирования */
                     int  iOptimiseStep   (RSS_Point *,             /* Управление оптимальностью сканирования */
                                           RSS_Point *, double *) ;
                  double  iCalcRange      (RSS_Point *,             /* Вычисление 'целевой' дистанции */
                                           RSS_Point *, double, double) ;
                    void  iOrtsDif        (RSS_Point *,             /* Вычисление угловой разницы между ортами ориентации */ 
                                           RSS_Point *, double *, double *, double *) ;
                  double  iAngleDif       (double) ;                /* Вычисление разности углов */
                     int  iSaveTracePoint (RSS_Point *) ;           /* Сохранение точки траектории */
                    void  iTopTraceShow   (TRAJECTORY *) ;          /* Отображение траектории на сцене */
                    void  iTopTraceShow_  (TRAJECTORY *) ;          /* Отображение траектории с передачей контекста */
                    void  iTraceHide      (TRAJECTORY *) ;          /* Удаление индикатора из сцены */

                    void  iDebug          (char *, char *, int) ;   /* Отладочная печать в файл */ 
                    void  iDebug          (char *, char *) ;

    public:
	                  RSS_Module_Path() ;                       /* Конструктор */
	                 ~RSS_Module_Path() ;                       /* Деструктор */
                                                         } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Path_instr {

            char                   *name_full ;                     /* Полное имя команды */
            char                   *name_shrt ;                     /* Короткое имя команды */
            char                   *help_row ;                      /* HELP - строка */
            char                   *help_full ;                     /* HELP - полный */
             int (RSS_Module_Path::*process)(char *, RSS_IFace *) ; /* Процедура выполнения команды */
                              }  ;
/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Path.cpp */

/* Файл  T_Path_dialog.cpp */
    BOOL CALLBACK  Task_Path_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Path_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Path_Save_dialog  (HWND, UINT, WPARAM, LPARAM) ;
