;
;  Тестовый командный файл поиска пути
;
#step 0.
 
#file tests\keys.keys

z-
z-
z-
z-

body cr obst плоскость 2.0 1.0
obst angle 45 0 0

flyer cr muh
muh ba head flat
muh bm head flat .\Body.lib\Flat  0.7 1.5
muh bs head flat  0. 0. 0.

show

muh b 1.0 -0.2 -0.8
robotstep 10

la 1 0.5 0
!l

muh angle>>

point c muh
