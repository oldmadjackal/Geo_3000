;
;  Тестовый командный файл анализа углов доступа к точке
;
#step 0.

srand 333
 
#file tests\keys.keys

#file tests\scene_1.object
#file tests\rob_1.object

z-
z-
z-
z-

show

look/a 1 0 -1
eye 3.863 5.381 2.362

rob base 0.7 0.0 -1.1

marker cr m1
m1 base 0.7 0.4 0.1

la 1 0 -1
!l

.point target rob:10 m1 180

* base>
>0.1
!save m1

m1

#remark Используйте стрелочки для управления целевой точкой манипулятора (Z - Ctrl+Left/Right)
