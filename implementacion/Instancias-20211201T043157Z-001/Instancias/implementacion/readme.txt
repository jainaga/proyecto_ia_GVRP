Para ejecutar el código seguir los siguientes pasos:

1.- Ejecutar en terminal 
    $make

    Esto compilara el código obteniendo un ejecutable llamado GVRP que será ejecutado.

Para eliminar el ejecutable GVRP utilizar el siguiente paso:
 Ejecutar en terminal
    $make clean

    Esto elimina el ejecutable GVRP compilado en el paso 1.

    IMPORTANTE:
-Si se quiere ver los resultados del algoritmo greedy antes del uso de Hill Climbing
se debe de eliminar la siguiente línea del archivo makefile:

rm rutasGreedy.txt

Esto dejará el archivo txt con las rutas sin optimizar al momento de que se ejecute el programa. 
Esto con la finalidad de querer ver las rutas y compararlas con las rutas optimizadas.

-Para cambiar las intancias a ejecutar en el archivo GVRP.c cambiar el nombre del archivo en la línea 761:

FILE *file = fopen("AB118.dat","r");

Luego guarde el archivo GVRP.c y para ejecutarlo utilice los pasos ya mencionados.