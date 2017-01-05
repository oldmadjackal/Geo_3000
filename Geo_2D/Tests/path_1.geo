;
;  Тестовый командный файл поиска пути
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

path search/d:trag1 set1
