
/********************************************************************/
/*								    */
/*             МОДУЛЬ УПРАВЛЕНИЯ ОБЪЕКТОМ "НАБОР ТОЧЕК"             */
/*								    */
/********************************************************************/

#ifdef O_PSET_EXPORTS
#define O_PSET_API __declspec(dllexport)
#else
#define O_PSET_API __declspec(dllimport)
#endif


/*---------------------------------------------- Параметры генерации */

/*---------------------------- Описание класса объекта "Набор точек" */

typedef struct {
                  RSS_Point  target ;
                  RSS_Joint *joints ;
                       char *object ;
                        int  type ;

               } RSS_Object_PSet_elem ;

  class O_PSET_API RSS_Object_PSet : public RSS_Object {

    public:
                        RSS_Object *mMasterObject ;        /* Мастер-обьект */
                               int  mDegreesCnt ;          /* Число степеней подвижности мастер-объекта */
              RSS_Object_PSet_elem *mPointsList ;          /* Список точек набора */
                               int  mPointsCnt ;

    public:
               virtual void  vWriteSave(std::string *) ;   /* Записать данные в строку */
               virtual  int  vSpecial  (char *, void *) ;  /* Специальные действия */

	                     RSS_Object_PSet() ;           /* Конструктор */
	                    ~RSS_Object_PSet() ;           /* Деструктор */
                                                       } ;

/*---------------- Описание класса управления объектом "Набор точек" */

  class O_PSET_API RSS_Module_PSet : public RSS_Kernel {

    public:

     static
      struct RSS_Module_PSet_instr *mInstrList ;            /* Список команд */

    public:
     virtual        void  vShow       (char *) ;            /* Отобразить связанные данные */
     virtual         int  vExecuteCmd (const char *) ;      /* Выполнить команду */
     virtual        void  vReadSave   (std::string *) ;     /* Чтение данных из строки */
     virtual        void  vWriteSave  (std::string *) ;     /* Записать данные в строку */

    public:
                     int  cHelp       (char *) ;            /* Инструкция HELP */ 
                     int  cCreate     (char *) ;            /* Инструкция CREATE */ 
                     int  cAdd        (char *) ;            /* Инструкция ADD */ 

         RSS_Object_PSet *FindObject  (char *) ;            /* Поиск обьекта типа PSET по имени */
                     int  CreateObject(char *) ;            /* Создание объекта */ 

    public:
	                  RSS_Module_PSet() ;               /* Конструктор */
	                 ~RSS_Module_PSet() ;               /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_PSet_instr {

           char                   *name_full ;         /* Полное имя команды */
           char                   *name_shrt ;         /* Короткое имя команды */
           char                   *help_row ;          /* HELP - строка */
           char                   *help_full ;         /* HELP - полный */
            int (RSS_Module_PSet::*process)(char *) ;  /* Процедура выполнения команды */
                              }  ;

/*--------------------------------------------- Диалоговые процедуты */

/* Файл  O_PSet.cpp */

/* Файл  O_PSet_dialog.cpp */
  BOOL CALLBACK  Object_PSet_Help_dialog(HWND, UINT, WPARAM, LPARAM) ;
