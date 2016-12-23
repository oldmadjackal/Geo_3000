
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ТОЧКИ"    		    */
/*								    */
/********************************************************************/

#ifdef T_POINT_EXPORTS
#define T_POINT_API __declspec(dllexport)
#else
#define T_POINT_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*------------------------------------- Описание записей индикаторов */

  typedef  struct {
                     char  name[128] ;       /* Название */
                      int  type ;            /* Тип индикатора: */
#define                     _CIRCLE_IND   0   /* Круговой */
#define                     _SPHERE_IND   1   /* Сферический */
                RSS_Point  point ;           /* Точка приложения(анализа) */
                      int  angle_code ;      /* Код анализируемого угла для кругового индикатора */
                      int *data ;            /* Индикаторный список */
                      int  data_cnt ;

                   double  scan ;            /* Индикатор текущего положения */
                      int  scan_flag ;       /* Флаг отображения индикатора текущего состояния */

                     void *context ;         /* Контекст привязки */
                      int  idx ;             /* Индекс привязки */ 
                     HWND  hWnd ;            /* Окно индикатора */
                      int  dlist1_idx ;      /* Индексы дисплейных списков */
                      int  dlist2_idx ;

                  } RSS_Module_Point_Indicator ;

#define  _IND_MAX  128

/*---------------------------- Описание класса задачи "Анализ точки" */

  class T_POINT_API RSS_Module_Point : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Point_instr  *mInstrList ;            /* Список команд */

         RSS_Module_Point_Indicator *mContextObject ;         /* Обьект 'контекстной' операции */
                               char  mContextAction[64] ;     /* Действие 'контекстной' операции */

                static    RSS_Point   mTarget ;               /* Целевая точка */
                             double   mRoundStep ;            /* Шаг сканирования углов */

                               HWND   mIndListWnd ;           /* Окно списка индикаторов */
         RSS_Module_Point_Indicator  *mIndicators[_IND_MAX] ; /* Список индикаторов */
         RSS_Module_Point_Indicator  *mIndicator_last ;       /* Ссылак на последний созданный индикатор */
                           COLORREF   mBadColor ;             /* Цвет 'недоступности' */
                           COLORREF   mGoodAColor ;           /* Цвета доступности */ 
                           COLORREF   mGoodEColor ;
                           COLORREF   mGoodRColor ;
                             double   mIndSize ;              /* Коэф.размера индикатора */   
				     
    public:
     virtual         int  vExecuteCmd    (const char *) ;     /* Выполнить команду */
     virtual         int  vExecuteCmd    (const char *, RSS_IFace *) ;
     virtual        void  vShow          (char *) ;           /* Отобразить связанные данные */
     virtual        void  vWriteSave     (std::string *) ;    /* Записать данные в строку */
     virtual         int  vGetParameter  (char *, char *) ;   /* Получить параметр */
     virtual        void  vChangeContext (void)  ;            /* Выполнить действие в контексте потока */

    public:
                     int  cHelp          (char *, RSS_IFace *) ;   /* Инструкция HELP */
                     int  cCheck         (char *, RSS_IFace *) ;   /* Инструкция CHECK */
                     int  cTarget        (char *, RSS_IFace *) ;   /* Инструкция TARGET */
                     int  cTargetInfo    (char *, RSS_IFace *) ;   /* Инструкция TARGET_INFO */
                     int  cRound         (char *, RSS_IFace *) ;   /* Инструкция ROUND */
                     int  cIndicatorList (char *, RSS_IFace *) ;   /* Инструкция INDICATOR_LIST */
                     int  cIndicatorKill (char *, RSS_IFace *) ;   /* Инструкция INDICATOR_KILL */

                    void  iIndicatorTop  (RSS_Module_Point_Indicator *) ;
                                                              /* Отображение индикатора в отд.окне */
                    void  iIndicatorShow (RSS_Module_Point_Indicator *) ;
                                                              /* Отображение индикатора на сцене */
                    void  iIndicatorShow_(RSS_Module_Point_Indicator *) ;
                                                              /* Отображение индикатора с передачей контекста */
                    void  iIndicatorHide (RSS_Module_Point_Indicator *) ;
                                                              /* Удаление индикатора из сцены */
                    void  iIndicatorKill (RSS_Module_Point_Indicator *) ;
                                                              /* Удаление индикатора */
                    char *iErrorDecode   (int) ;              /* Расшифровка кода ошибки */

    public:
	                  RSS_Module_Point() ;                /* Конструктор */
	                 ~RSS_Module_Point() ;                /* Деструктор */
                                                         } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Point_instr {

           char                    *name_full ;                     /* Полное имя команды */
           char                    *name_shrt ;                     /* Короткое имя команды */
           char                    *help_row ;                      /* HELP - строка */
           char                    *help_full ;                     /* HELP - полный */
            int (RSS_Module_Point::*process)(char *, RSS_IFace *) ; /* Процедура выполнения команды */
                               }  ;
/*--------------------------------------- Команды диалогового обмена */

#define  _TASK_POINT_ADD_INDICATOR    1
#define  _TASK_POINT_KILL_INDICATOR   2

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Point.cpp */

/* Файл  T_Point_dialog.cpp */
    BOOL CALLBACK  Task_Point_Help_dialog       (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Point_Round_dialog      (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Point_IList_dialog      (HWND, UINT, WPARAM, LPARAM) ;
 LRESULT CALLBACK  Task_Point_RoundIndicator_prc(HWND, UINT, WPARAM, LPARAM) ;