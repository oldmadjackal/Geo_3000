
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ВЫХОД В ТОЧКУ"    		    */
/*								    */
/********************************************************************/

#ifdef T_REACH_EXPORTS
#define T_REACH_API __declspec(dllexport)
#else
#define T_REACH_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*--------------------------- Описание класса задачи "Выход в точку" */

  class T_REACH_API RSS_Module_Reach : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Reach_instr *mInstrList ;            /* Список команд */

                static    RSS_Point  mTarget ;               /* Целевая точка */

                             double  mLineAcc ;              /* Точность определения линейных координат */ 
                             double  mAngleAcc ;             /* Точность определения угловых координат */ 
                                int  mResetStart ;           /* Флаг переустановки начальной точки поиска     */
			                                     /*   на середину диапазонов степеней подвижности */
                                int  mCheckRanges ;          /* Проверка ограничений в степенях подвижности */
                                int  mDraw ;                 /* Отрисовывать в процессе поиска */
                             double  mScanStepCoef ;         /* Делитель шага сближения */
                             double  mJampRangeCoef ;        /* Коэф.длины вектора отскока */
                                int  mFailMax ;              /* Предельное число неудачных векторов подряд */ 

    public:
     virtual         int  vExecuteCmd    (const char *) ;     /* Выполнить команду */
     virtual        void  vWriteSave     (std::string *) ;    /* Записать данные в строку */

    public:
                     int  cHelp          (char *) ;           /* Инструкция HELP */ 
                     int  cConfig        (char *) ;           /* Инструкция CONFIG */ 
                     int  cTarget        (char *) ;           /* Инструкция TARGET */ 
                     int  cTargetInfo    (char *) ;           /* Инструкция TARGET_INFO */ 
                     int  cRotate        (char *) ;           /* Инструкция ROTATE */ 

                    char *iErrorDecode   (int) ;              /* Расшифровка кода ошибки */
                    void  iGenerateVector(double *, int) ;    /* Генерация случайного вектора */
                  double  iCalcRange     (RSS_Point *,        /* Вычисление 'целевой' дистанции */
                                          RSS_Point *,
                                               double, double) ;
                  double  iAngleDif      (double) ;           /* Вычисление разности углов */

    public:
	                  RSS_Module_Reach() ;                /* Конструктор */
	                 ~RSS_Module_Reach() ;                /* Деструктор */
                                                         } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Reach_instr {

           char                    *name_full ;         /* Полное имя команды */
           char                    *name_shrt ;         /* Короткое имя команды */
           char                    *help_row ;          /* HELP - строка */
           char                    *help_full ;         /* HELP - полный */
            int (RSS_Module_Reach::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;
/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Reach.cpp */

/* Файл  T_Reach_dialog.cpp */
    BOOL CALLBACK  Task_Reach_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Reach_Config_dialog     (HWND, UINT, WPARAM, LPARAM) ;
