;
;  Тестовый командный файл анализа углов доступа к точке
;
#step 0.

srand 333
 
#file tests\keys.keys

#file tests\scene_2.object
#file tests\rob_1.object

z-
z-
z-
z-

show

look/a 1 0 -1
eye 3.863 5.381 2.362

rob base 0.7 0.0 -1.1

la 1 0 -1
!l

area create Zone1 FLAT
area angle Zone1 -20 0 0
area table Zone1 tests\area_2_1.table
area execute/d Zone1 rob

area cr Zone2 CYL
area base Zone2 0.7 0 -1.1
area table Zone2 tests\area_2_2.table
area execute/d Zone2 rob
