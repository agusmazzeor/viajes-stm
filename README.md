### Local

Para limpiar el proyecto (asegurarse de que no hay archivos antiguos), compilar el programa y ejecutarlo (ej. con 4 nodos):

```
make clean
make
mpirun -np 4 ./bin/main
```

### Maquinas de facultad

Para correr el proyecto en maquinas de facultad, ingresar con tu usuario a una computadora de facultad, dirigirse a la carpeta del proyecto viajes-stm y correr (ej. con 4 nodos):

```
sh ./ejecutar_programa.sh 4
```
