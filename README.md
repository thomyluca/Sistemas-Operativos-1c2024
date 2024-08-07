# TP Sistemas Operativos - 1er Cuatrimestre 2024

Simulación de un Sistema Operativo distribuido en 4 módulos:
 - Kernel
 - Memoria
 - Cpu
 - Entrada/Salida (Teclado, Monitor, File System, otros)

## Datos de Interés
 El Kernel cuenta con 3 algoritmos de planificación (FIFO, Round Robin, Virtual Round Robin).
 
 La Memoria cuenta con un algoritmo de paginacion simple.
 
 Se cuenta ademas con una TLB que puede reemplazar páginas por FIFO y LRU.

 El File System es capaz de Compactar cuando es necesario.

# Grupo "OALP"

| Apellido y Nombre | GitHub user |
|-------------------|-------------|
| Winik, Tobias | [@twinik](https://github.com/twinik) |
| Portnoi, Luka  | [@LukaPortnoi](https://github.com/LukaPortnoi) |
| Martinez, Manuel   | [@ManuMar28](https://github.com/ManuMar28) |
| Szorzoli, Santiago  | [@szorzoli](https://github.com/szorzoli) | 
| Luca, Thomas  | [@thomyluca](https://github.com/thomyluca) | 

## Enunciado

[Link](https://docs.google.com/document/d/1-AqFTroovEMcA1BfC2rriB5jsLE6SUa4mbcAox1rPec/edit)

## Pruebas

[Link](https://docs.google.com/document/d/1XsBsJynoN5A9PTsTEaZsj0q3zsEtcnLgdAHOQ4f_4-g/edit)

# Como ejecutar
## Requerimientos
- Entorno Unix (preferentemente alguna version de Ubuntu, ya que fue el entorno provisto por la catedra y el utilizado para su desarrollo)
- Libreria commons utnso - https://github.com/sisoputnfrba/so-commons-library
- Realizar build de cada modulo (utilizar makefile) y ejecutar en el siguiente orden: Memoria - CPU - Kernel - interfaces
Nota: Chequear direcciones IP en los configs si se levantan diferentes computadoras

Se recomienda utilizar la siguiente herramienta provista por la catedra, que facilita el proceso de deployment: https://github.com/sisoputnfrba/so-deploy
