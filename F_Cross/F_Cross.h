/********************************************************************/
/*								    */
/*	МОДУЛЬ УПРАВЛЕНИЯ ОБНАРУЖЕНИЕМ ПЕРЕСЕЧЕНИЯ ОБЪЕКТОВ	    */
/*								    */
/********************************************************************/

#ifdef F_CROSS_EXPORTS
#define F_CROSS_API __declspec(dllexport)
#else
#define F_CROSS_API __declspec(dllimport)
#endif

/*---------------------------------------------- Параметры генерации */

/*---------------- Описание класса управления объектом "Пересечение" */

  class F_CROSS_API RSS_Module_Cross : public RSS_Kernel {

    public:

     static
      struct RSS_Module_Cross_instr *mInstrList ;          /* Список команд */
				     
    public:
     virtual         int  vExecuteCmd   (const char *) ;    /* Выполнить команду */

    public:
                     int  cHelp         (char *) ;          /* Инструкция Help */ 
                     int  cAssign       (char *) ;          /* Инструкция Assign */ 
                     int  cExclude      (char *) ;          /* Инструкция Exclude */ 

    public:
     virtual        void  vStart        (void) ;            /* Стартовая разводка */
     virtual RSS_Feature *vCreateFeature(RSS_Object *) ;    /* Создать свойство */
     virtual        void  vWriteSave    (std::string *) ;   /* Записать данные в строку */
     virtual        void  vReadSave     (std::string *) ;   /* Считать данные из строки */

	                  RSS_Module_Cross() ;              /* Конструктор */
	                 ~RSS_Module_Cross() ;              /* Деструктор */
                                                       } ;

/*-------------------------------------------- Инструкции управления */

 struct RSS_Module_Cross_instr {

           char                    *name_full ;         /* Полное имя команды */
           char                    *name_shrt ;         /* Короткое имя команды */
           char                    *help_row ;          /* HELP - строка */
           char                    *help_full ;         /* HELP - полный */
            int (RSS_Module_Cross::*process)(char *) ;  /* Процедура выполнения команды */
                               }  ;

/*---------------------------------------------- Компоненты описания */

 typedef struct {                                            /* Описание габаритного прямоугольника */
                             double  x_min ;
                             double  y_min ;
                             double  z_min ;
                             double  x_max ;
                             double  y_max ;
                             double  z_max ;
                }  RSS_Feature_Cross_Dim ;

 typedef struct {                                            /* Описание матрицы преобразования */
                             double  x_min ;
                }  RSS_Feature_Cross_Matrix ;

 typedef struct {                                            /* Описание точки */
                    double  x ;                               /* X - значение */
                      char *x_formula ;                       /* X - рассчетное выражение */
                      void *x_calculate ;                     /* X - контекст вычислителя */

                    double  y ;                               /* Y - значение */
                      char *y_formula ;                       /* Y - рассчетное выражение */
                      void *y_calculate ;                     /* Y - контекст вычислителя */

                    double  z ;                               /* Z - значение */
                      char *z_formula ;                       /* Z - рассчетное выражение */
                      void *z_calculate ;                     /* Z - контекст вычислителя */

                    double  x_abs ;                           /* Координаты в абсолютной СК */
                    double  y_abs ;
                    double  z_abs ;
                }  RSS_Feature_Cross_Vertex ;

#define  _VERTEX_PER_FACET_MAX   16

 typedef struct {                                            /* Описание грани */

                       int  vertexes[_VERTEX_PER_FACET_MAX] ; /* Список точек */
                       int  vertexes_cnt ;

     RSS_Feature_Cross_Dim  overall ;                         /* Габарит грани */

  RSS_Feature_Cross_Matrix  in_facet_matrix ;                 /* Матрица преобразования в систему координат грани */
                       int  in_facet_matrix_done ;            /* Флаг рассчета матрицы... */
                }  RSS_Feature_Cross_Facet ;

 typedef struct {                                            /* Описание тела */
                               char  name[128] ;              /* Название */
               struct RSS_Parameter *extrn_pars ;             /* Ссылак на параметры */

           RSS_Feature_Cross_Vertex *Vertexes ;               /* Список вершин */
                                int  Vertexes_cnt ; 
            RSS_Feature_Cross_Facet *Facets ;                 /* Список вершин */
                                int  Facets_cnt ;

              RSS_Feature_Cross_Dim  overall ;                /* Габарит тела */

                                int  recalc ;                 /* Флаг необходимости перерасчета точек тела */

                             double  x_base ;                 /* Координаты базовой точки */
                             double  y_base ;
                             double  z_base  ;
                             double  x_base_s ;
                             double  y_base_s ;
                             double  z_base_s ;

                             double  a_azim ;                 /* Углы ориентации */
                             double  a_elev ;
                             double  a_roll  ;
                             double  a_azim_s ;
                             double  a_elev_s ;
                             double  a_roll_s ;

                             double  Matrix[4][4] ;           /* Матрица положения звена */
                                int  Matrix_flag ;            /* Флаг использования... */
      
                                int  list_idx ;               /* Индекс дисплейного списка */
                }  RSS_Feature_Cross_Body ;

 typedef struct {                                            /* Элемент списка управления проверкой */
                   char  object[128] ;                        /* Имя объекта */
                   char  link[128] ;                          /* Имя звена */
                   char  body[128] ;                          /* Имя тела */
                   char  desc[256] ;                          /* Общая маска */
                   void *pair ;                               /* Ссылка на парный элемент списка */
                    int  execute ;                            /* Флаг выполнения/игнорирования проверки */
                    int  active ;                             /* Флаг активности */

                    int  work ;                               

                }  RSS_Feature_Cross_Mark ;

/*--------------------------- Описание класса свойства "Пересечение" */

  class F_CROSS_API RSS_Feature_Cross : public RSS_Feature {

    public:       
             RSS_Feature_Cross_Body  *Bodies ;                 /* Список тел */
                                int   Bodies_cnt ;

               struct RSS_Parameter  *Pars_work ;

              RSS_Feature_Cross_Dim   overall ;                /* Габарит объекта */

     static  RSS_Feature_Cross_Mark **marks_list ;             /* Список управления проверкой */
     static                     int   marks_list_cnt ; 

    public:
            virtual void  vReadSave     (char *, std::string *,     /* Считать данные из строки */
                                                        char * ) ;
            virtual void  vGetInfo      (std::string *) ;           /* Выдать информацию о свойстве */
            virtual  int  vCheck        (void *) ;                  /* Проверка непротиворечивости свойства */  
            virtual void  vBodyDelete   (char *) ;                  /* Удалить тело */
            virtual void  vBodyBasePoint(char *,                    /* Задание базовой точки тела */
                                         double, double, double) ;
            virtual void  vBodyAngles   (char *,                    /* Задание ориентации тела */
                                         double, double, double) ;
            virtual void  vBodyMatrix   (char *, double[4][4]) ;    /* Задание матрицы положения тела */
            virtual void  vBodyShifts   (char *,                    /* Задание смещения положения и  */
                                          double, double, double,   /*    ориентации тела            */
                                          double, double, double ) ;
            virtual void  vBodyPars     (char *,                    /* Задание списка параметров */
                                             struct RSS_Parameter *) ;

                     int  RecalcPoints  (void) ;                    /* Перерасчет точек тел объекта */
              RSS_Kernel *iGetCalculator(void) ;                    /* Определение нужного вычислителя */
 static
  RSS_Feature_Cross_Mark *iSetMark      (RSS_Feature_Cross_Mark *,  /* Создание пары элементов списка управления контроля пересечения */ 
                                         RSS_Feature_Cross_Mark * ) ;
 static              int  iGetMarkEx    (char *,                    /* Получение списка управления контроля пересечения */ 
                                         RSS_Feature_Cross_Mark **, int) ;
 static              int  iGetMarkIn    (char *,                    /* Получение списка управления контроля пересечения */ 
                                         RSS_Feature_Cross_Mark **, int) ;
                     int  iMarkForCheck (char *, char *) ;          /* Проверка назначения объекта или его частей для проверки */
                     int  iMaskCheck    (char *, 
                                         RSS_Feature_Cross_Mark *) ;
                     int  iOverallTest  (RSS_Feature_Cross_Dim *,   /* Проверка пересечение габаритов */
                                         RSS_Feature_Cross_Dim * ) ;

	                  RSS_Feature_Cross() ;                     /* Конструктор */
	                 ~RSS_Feature_Cross() ;                     /* Деструктор */
                                                           } ;

/*--------------------------------------------- Диалоговые процедуры */

/* Файл  F_Cross.cpp */

/* Файл  F_Cross_dialog.cpp */
  BOOL CALLBACK  Feature_Cross_Help_dialog  (HWND, UINT, WPARAM, LPARAM) ;
  BOOL CALLBACK  Feature_Cross_Assign_dialog(HWND, UINT, WPARAM, LPARAM) ;
