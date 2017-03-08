;
;  Тестовый командный файл рассчета времени прохождения пути
;
#step 0.

srand 333
 
#file tests\keys.keys

#file tests\chamber_2.object
#file tests\rob_1.object

z-
z-
z-
z-

show

look/a 1 0 -1
eye -1.385 4.728 2.949

rob base 0.7 0.0 -1.1

marker cr m1
m1 base 0.0 0.45 -0.4

marker cr m2
m2 base 1.6 0.45 -2.2

cross ex rob.base
cross ex rob.leg
;cross ex rob.link1
;cross ex rob.link2
;cross ex rob.link3
;cross ex rob.wrist

pset cr set1
pset cr set2

p t rob:10 m1 180
set1 add rob
set2 add rob

p t rob:10 m2 0
set1 add rob
set2 add @m2

la 1 0 -1

!l

path search/rdq:trag1 set1

spline d rob leg-a 120 120 "0.0002*V+0.0001*A*A"
spline d rob link1-a 120 240 "0.0002*V+0.0001*A*A"
spline d rob link2-a 120 240 "0.0002*V+0.0001*A*A"
spline d rob link3-a 120 240 "0.0002*V+0.0001*A*A"
spline d rob wrist-a 120 240 "0.0002*V+0.0001*A*A"
spline d rob gripper-a 240 480 "0.0002*V+0.0001*A*A"

#remark Простое сглаживание траектории с рассчетом времени прохождения
spline glide/i trag1
#pause 5

#remark Отображение трубки ошибок
spline glide/ie trag1
#pause 5

#remark Коррекция законов движения для гарантированного прохождения с учетом трубки ошибок
spline glide/iea trag1
