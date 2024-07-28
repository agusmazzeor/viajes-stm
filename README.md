### Local

Para limpiar el proyecto (asegurarse de que no hay archivos antiguos), compilar el programa y ejecutarlo (ej. con 4 nodos):

```
make clean
make
mpirun -np 4 ./bin/main
```

### Cluster

Para correr en el Cluster primero editar el archivo `ejecucion_cluster/ejecutar_programa.sh` para especificar la cantidad de cores (--ntasks=), memoria ram (--mem-per-cpu=) y cantidad de procesos recurrentes para la ejecucion.

Entrar a /clusteruy/home03/antel-optical-network/stm-viajes/viajes-stm

```
interactivo -g
source ~/miniconda3/bin/activate
conda activate cpp_env
sbatch ejecucion_cluster/ejecutar_programa.sh
```

Para ver si el trabajo esta corriendo:

```
squeue -u santiago.olmedo
```

El trabajo tiene un jobid. Para ver el output del trabajo se te guarda en el mismo directorio en un archivo slurm-jobid.out. Por ejemplo: slurm-3892043.out

### Maquinas de facultad

Para correr el proyecto en maquinas de facultad, ingresar con tu usuario a una computadora de facultad, dirigirse a la carpeta del proyecto viajes-stm y correr (ej. con 4 nodos):

```
python3 scripts_python/encontrar_hosts.py
sh ./ejecucion_pcunix/ejecutar_programa.sh 4
```

Para correrlo con varias repeticiones:

```
python3 scripts_python/encontrar_hosts.py
sh ./ejecucion_pcunix/ejecutar_programa_varias_veces.sh
```
