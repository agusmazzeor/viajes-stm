### Local

Para limpiar el proyecto (asegurarse de que no hay archivos antiguos), compilar el programa y ejecutarlo (ej. con 4 nodos):

```
make clean
make
mpirun -np 4 ./bin/main
```

### Cluster

Para correr en el Cluster primero editar el archivo `ejecucion_cluster/ejecutar_programa.sh` para especificar la cantidad de cores (--ntasks=), memoria ram (--mem-per-cpu=) y cantidad de procesos recurrentes para la ejecucion.

Hacer ssh al cluster, entrar a la carpeta del proyecto y ejecutar:

```
interactivo -g
sbatch ejecucion_cluster/ejecutar_programa.sh
```

### Maquinas de facultad

Para correr el proyecto en maquinas de facultad, ingresar a una computadora de facultad, dirigirse a la carpeta del proyecto viajes-stm y correr (ej. con 4 nodos):

```
python3 scripts_python/encontrar_hosts.py
sh ./ejecucion_pcunix/ejecutar_programa.sh 4
```

Para correrlo con varias repeticiones:

```
python3 scripts_python/encontrar_hosts.py
sh ./ejecucion_pcunix/ejecutar_programa_varias_veces.sh
```
