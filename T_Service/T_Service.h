
/********************************************************************/
/*								    */
/*		МОДУЛЬ ЗАДАЧИ "ЗАДАЧА ОБСЛУЖИВАНИЯ"    		    */
/*								    */
/********************************************************************/

#ifdef T_SERVICE_EXPORTS
#define T_SERVICE_API __declspec(dllexport)
#else
#define T_SERVICE_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*---------------------------------------------- Сервисный процессор */

 typedef struct {                           /* Оператор */
                   char  action[16] ;
                   char  command[256] ;
                   char  mark[8] ;
                } RSS_Module_Service_frame ;

#define  FRAME  RSS_Module_Service_frame

#define    _ERROR_FRAME  -1
#define     _NEXT_FRAME   0
#define     _EXIT_FRAME   1
#define  _GOTO_UP_FRAME   2
#define  _GOTO_DN_FRAME   3
#define  _JUMP_DN_FRAME   4

 typedef struct {                           /* описание переменной */
                   char  name[16] ;
                   char  value[256] ;
                } RSS_Module_Service_variable ;

#define  VAR  RSS_Module_Service_variable

/*--------------------- Описание класса задачи "Задача обслуживания" */

  class T_SERVICE_API RSS_Module_Service : public RSS_Kernel {

    public:

     static
       struct RSS_Module_Service_instr *mInstrList ;              /* Список команд */

                    static       FRAME *mScenario ;               /* Описание задачи обслуживания */
                    static         int  mScenario_cnt ;   

                    static         VAR *mVariables ;              /* Список переменных */
                    static         int  mVariables_cnt ;   
                    static        HWND  mVariables_Window ;

                                  char  mContextAction[64] ;      /* Действие 'контекстной' операции */

                    static   RSS_IFace  mExecIFace ;              /* Каноническая форма результата операции */
                    static      double  mExecValue ;              /* Числовой результат операции */
                    static         int  mExecError ;              /* Признак ошибки операции */
                    static         int  mExecFail ;               /* Признак неподдержки интерфейса внутренних связей */

    public:
     virtual         int  vExecuteCmd   (const char *) ;          /* Выполнить команду */
     virtual         int  vExecuteCmd   (const char *, RSS_IFace *) ;
     virtual        void  vReadSave     (std::string *) ;         /* Чтение данных из строки */
     virtual        void  vWriteSave    (std::string *) ;         /* Записать данные в строку */
     virtual         int  vGetParameter (char *, char *) ;        /* Получить параметр */
     virtual        void  vChangeContext(void)  ;                 /* Выполнить действие в контексте потока */

    public:
                     int  cHelp         (char *, RSS_IFace *) ;   /* Инструкция HELP */
                     int  cList         (char *, RSS_IFace *) ;   /* Инструкция LIST */
                     int  cExport       (char *, RSS_IFace *) ;   /* Инструкция EXPORT */
                     int  cImport       (char *, RSS_IFace *) ;   /* Инструкция IMPORT */
                     int  cRun          (char *, RSS_IFace *) ;   /* Инструкция RUN */
                     int  cVariables    (char *, RSS_IFace *) ;   /* Инструкция VARIABLES */

                     int  iScenarioEdit (char *, int) ;           /* Интерфейс редактирования сценария */
                     int  iFrameExecute (FRAME *,                 /* Реализация инструкций сервисной задачи */
                                         FRAME *, char *) ;       
              RSS_Kernel *iGetCalculator(void) ;                  /* Определение нужного вычислителя */
                    void  iGetBinary    (char *, int, int) ;      /* Вывод числа в бинарном виде */

    public:
	                  RSS_Module_Service() ;               /* Конструктор */
	                 ~RSS_Module_Service() ;               /* Деструктор */
                                                             } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Service_instr {

           char                      *name_full ;                      /* Полное имя команды */
           char                      *name_shrt ;                      /* Короткое имя команды */
           char                      *help_row ;                       /* HELP - строка */
           char                      *help_full ;                      /* HELP - полный */
            int (RSS_Module_Service::*process)(char *, RSS_IFace *) ;  /* Процедура выполнения команды */
                              }  ;
/*--------------------------------------------- Диалоговые процедуты */

/* Файл  T_Service.cpp */

/* Файл  T_Service_dialog.cpp */
    BOOL CALLBACK  Task_Service_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Service_Edit_dialog(HWND, UINT, WPARAM, LPARAM) ;
    BOOL CALLBACK  Task_Service_Vars_dialog(HWND, UINT, WPARAM, LPARAM) ;
