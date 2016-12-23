/********************************************************************/
/*								    */
/*      МОДУЛЬ ЗАДАЧИ "АНАЛИЗ ДИАПАЗОНА ПАРАМЕТРОВ"                 */
/*								    */
/********************************************************************/

#ifdef T_SCAN_EXPORTS
#define T_SCAN_API __declspec(dllexport)
#else
#define T_SCAN_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

#define     _SCAN_PARS_MAX   20      /* Максимальное число параметров сканирования */
#define       _FORMULA_LEN  128      /* Максимальная длина формулы, символов */

/*--------------------------------------------- Область сканирования */

 typedef struct {                                    /* Описание расчетного значения */
                     char  formula[_FORMULA_LEN] ;
                   double  value ;
                      int  fixed ;
                } RSS_Module_Scan_value ;

 typedef struct {                         /* Параметр области сканирования */
                     char  name[64] ;      /* Имя параметра */
                      int  category ;      /* Категория параметра: */
#define                     _VAR_PAR   0    /* Переменная */
#define                  _OBJECT_PAR   1    /* Параметр объекта */
                      int  kind ;          /* Вид параметра: */
#define                    _SCAN_PAR   0    /* Свободный параметр */
#define                   _SLAVE_PAR   1    /* Расчетный параметр */

    RSS_Module_Scan_value  value ;         /* Текущее значение */
    RSS_Module_Scan_value  value_min ;     /* Диапазон изменения */
    RSS_Module_Scan_value  value_max ;
    RSS_Module_Scan_value  value_step ;    /* Шаг изменения */
                      int  step ;          /* Номер иттерации */ 

                     char  object[128] ;   /* Имя объекта (для _OBJECT_PAR) */ 
                     char  par_link[32] ;  /* Ссылка на параметр объекта (для _OBJECT_PAR) */

    class RSS_Module_Scan *module ;        /* Ссылка на родительский модуль */ 

                } RSS_Module_Scan_par ;

#define  ZONE_PAR  RSS_Module_Scan_par

/*------------------------------------------- Результат сканирования */

 typedef  union { double  f_value ;                  /* Описание значения параметра */
                    long  l_value ;  }  p_value ;

 typedef struct {                                    /* Описание расчетной точки */
                   p_value *pars ;                    /* Набор параметров */
                    double  result ;                  /* "Функция" */
                       int  error_flag ;              /* Признак ошибки */
                } RSS_Module_Scan_result ;

 typedef struct {                                    /* Описание маркера результата */  
                       char  model[64] ;              /* Модель маркера */
                        int  dlist_idx ;              /* Индекс дисплейного списка */
                     double  size ;                   /* Коэф.размера маркера */   
                   COLORREF  color_min ;              /* Диапазон раскраски маркеров */
                   COLORREF  color_max ;
                } RSS_Module_Scan_marker ;

 typedef struct {                                    /* Описание положения базовой точки */  
                    RSS_Point  point ;                /* Модель маркера */
                          int  dlist_idx ;            /* Индекс дисплейного списка */
                       double  result_min ;           /* Диапазон успещного результата */
                       double  result_max ;
                          int  good ;                 /* Флаг успешного результата */ 
                          int  error ;                /* Флаг ошибки */
                          int  draw ;
                } RSS_Module_Scan_basicp ;

/*-------------------------------------------------- Контекст модуля */

 typedef struct {
                     char  dummy[64] ;

                } RSS_Module_Scan_Context ;

/*-------------------- Описание класса "Анализ диапазона параметров" */

  class T_SCAN_API RSS_Module_Scan : public RSS_Kernel {

    public:

    static
       struct RSS_Module_Scan_instr *mInstrList ;                        /* Список команд */

                               char  mContextAction[64] ;                /* Действие 'контекстной' операции */
             RSS_Module_Scan_basicp *mContextObject ;                    /* Обьект 'контекстной' операции */

                               char  mCheck_cmd[512] ;                   /* Команда определения оптимальности */
                                int  mBasePoints_show ;                  /* Флаг отображения базовых точек в рабочем пространстве */
                                int  mBasePoints_redraw ;                /* Флаг перерисовки базовых точек после каждого шага */

                           ZONE_PAR  mScanPars[_SCAN_PARS_MAX] ;         /* Список параметров сканирования */
                                int  mScanPars_cnt ;

                                int  mDebug_level ;                      /* Уровень отладочной печати */
                               char  mError[1024] ;                      /* Сообщение об ошибке */ 
                       static  long  mDebug_stamp ;                      /* Отадочная метка */

             RSS_Module_Scan_result *mResult ;                           /* Набор точек результатов */
                            p_value *mResult_pars ;                      /* Буфер значений параметров */
                               long  mResult_cnt ;                       /* Число точек результата */
                               long  mResult_max ;
                             double  mResult_value_min ;                 /* Диапазон результата */
                             double  mResult_value_max ;

             RSS_Module_Scan_basicp *mBasics ;                           /* Набор отображаемых базовых точек */
                               long  mBasics_cnt ;                       /* Число отображаемых базовых точек */

#define                                             _MARKER_MAX   2
             RSS_Module_Scan_marker  mBasics_marker[_MARKER_MAX] ;       /* Маркеры базовых точек */

    public:
             virtual     void  vInit           (void) ;                   /* Инициализация связей */
             virtual      int  vExecuteCmd     (const char *) ;           /* Выполнить команду */
             virtual      int  vExecuteCmd     (const char *, RSS_IFace *) ;
             virtual     void  vShow           (char *) ;                 /* Отобразить связанные данные */
             virtual     void  vChangeContext  (void)  ;                  /* Выполнить действие в контексте потока */
             virtual     void  vReadSave       (std::string *) ;          /* Чтение данных из строки */
             virtual     void  vWriteSave      (std::string *) ;          /* Записать данные в строку */
             virtual      int  vGetParameter   (char *, char *) ;         /* Получить параметр */
                    
    public:         
                          int  cHelp           (char *, RSS_IFace *) ;    /* Инструкция HELP */
                          int  cConfig         (char *, RSS_IFace *) ;    /* Инструкция CONFIG */
                          int  cRun            (char *, RSS_IFace *) ;    /* Инструкция RUN */
                          int  cBases          (char *, RSS_IFace *) ;    /* Реализация инструкции BASES */
                          int  cClear          (char *, RSS_IFace *) ;    /* Реализация инструкции CLEAR */
                          int  cExcel          (char *, RSS_IFace *) ;    /* Инструкция EXCEL */

                          int  FormMarker      (void) ;                    /* Задание формы маркера */
                          int  FormMarker_     (void) ;                    /* Задание формы маркера с передачей контекста */
                          int  PlaceMarker     (RSS_Module_Scan_basicp *); /* Позиционирование маркера */
                          int  PlaceMarker_    (RSS_Module_Scan_basicp *); /* Позиционирование маркера с передачей контекста */

                         void  iDebug          (char *, char *) ;         /* Отладочная печать в файл */
                         void  iExecute        (char *, RSS_IFace *) ;    /* Выполнение команды */
                          int  iCalcScanValue  (RSS_Module_Scan_par *) ;  /* Опр.значения для "диапазонного" параметра */
                          int  iCalcSlaveValue (RSS_Module_Scan_par *) ;  /* Опр.значения для расчетного параметра */
                          int  iCalcValue      (RSS_Module_Scan_value *); /* Опр.значения */
                          int  iCalcGetValue   (RSS_Module_Scan_par *,    /* Выдача значения параметра для сохранения */
                                                           p_value * ) ;
                          int  iCalcTextValue  (RSS_Module_Scan_par *,    /* Выдача значения параметра в текстовом виде */
                                                   p_value *, char * ) ;
                          int  iSaveBinResult  (char *, char *, int) ;    /* Сохранение технического файла результата */
                          int  iSaveCsvResult  (char *) ;                 /* Сохранение CSV-файла результата */
                          int  iFormExcelResult(char *) ;                 /* Загрузка результата в MS Excel */
                       double  iColorMix       (double, double, double) ; /* Расчет цветового эквивалента */ 

    public:
  	                       RSS_Module_Scan() ;                 /* Конструктор */
	                      ~RSS_Module_Scan() ;                 /* Деструктор */
                                                           } ;
/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Scan_instr {

            char                   *name_full ;                      /* Полное имя команды */
            char                   *name_shrt ;                      /* Короткое имя команды */
            char                   *help_row ;                       /* HELP - строка */
            char                   *help_full ;                      /* HELP - полный */
             int (RSS_Module_Scan::*process)(char *, RSS_IFace *) ;  /* Процедура выполнения команды */
                              }  ;
/*---------------------------------------------- Специальные статусы */


/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Scan.cpp */
   RSS_Kernel *GetTaskScanEntry(void) ;    /* Получение ссылки на программный модуль */

/* Файл  T_Scan_dialog.cpp */
    BOOL CALLBACK  Task_Scan_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Scan_Config_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Scan_Param_dialog (HWND, UINT, WPARAM, LPARAM) ;

/* Файл  T_Scan_color.cpp */
         COLORREF  Task_Scan_color_choice (COLORREF) ;
         COLORREF  Task_Scan_set_color    (COLORREF) ;
    BOOL CALLBACK  Task_Scan_color_dialog (HWND, UINT, WPARAM, LPARAM) ;
