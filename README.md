# Simulación de Sistema de Archivos en C

Proyecto desarrollado para el curso **Principios de Sistemas Operativos**  
**Profesor:** Kenneth Obando Rodríguez  
**Estudiantes:** *Pablo Mesén Alvarado y Luis Urbina Salzar*  
**Instituto Tecnológico de Costa Rica – Sede Central Cartago*

---

## Descripción General

Este programa implementa un **sistema de archivos simulado en memoria**, utilizando bloques de datos de tamaño fijo y una tabla de directorio raíz.  
El sistema soporta operaciones básicas sobre archivos:

- **CREATE** – Crear un archivo con espacio previamente reservado
- **WRITE** – Escribir datos en posiciones específicas
- **READ** – Leer datos desde cualquier offset
- **DELETE** – Eliminar un archivo y liberar sus bloques
- **LIST** – Mostrar archivos almacenados

El sistema lee un archivo de texto con instrucciones, ejecuta los comandos y muestra los resultados en consola.

El objetivo es comprender conceptos como:

- **Gestión de bloques de almacenamiento**
- **Asignación no contigua**
- **Directorios y metadatos**
- **Simulación de acceso a disco**
- **Manejo de errores de lectura/escritura**

---

## Estructura del Proyecto

```
├── include/
│   └── includes.h
├── src/
│   ├── functions.c
│   └── main.c
├── test_files/
│   └── test.txt
└── README.md
```

### Descripción de Archivos

| Archivo | Descripción |
|---------|-------------|
| `includes.h` | Contiene constantes, estructuras (`FileEntry`), variables globales y prototipos. |
| `functions.c` | Implementa todas las operaciones del sistema de archivos: CREATE, WRITE, READ, DELETE, LIST, y funciones auxiliares. |
| `main.c` | Procesa argumentos y ejecuta el archivo de instrucciones vía `read_test_file()`. |
| `test_files/test.txt` | Archivo de comandos de prueba. |
| `README.md` | Documento de proyecto. |

---

## Funcionamiento del Sistema de Archivos

El sistema simula un “disco” en memoria utilizando:

```
unsigned char data_blocks[NUM_BLOCKS][BLOCK_SIZE];
```

Además mantiene un directorio sencillo mediante una tabla:

```
FileEntry file_table[MAX_FILES];
```

Cada archivo guarda:

- Nombre  
- Tamaño lógico  
- Tamaño máximo asignado  
- Lista de bloques físicos  
- Cantidad de bloques asignados  
- Estado (usado / libre)

La asignación de bloques **no requiere contigüidad**, ya que cada archivo posee su propio mapa de bloques físicos.

---

## Compilación

Ejecutar en Linux, macOS o Windows:

```bash
gcc src/*.c -Iinclude -o main
```

---

## Ejecución

El programa recibe **un argumento**:

```bash
./main <ruta_del_archivo_de_instrucciones>
```

Ejemplo:

```bash
./main test_files/test.txt
```

---

## Formato del Archivo de Entrada

El archivo debe contener una lista de comandos:

```txt
CREATE file1 1000
WRITE file1 0 "Hola Mundo"
READ file1 0 5
LIST
DELETE file1
```

---

## Comandos Disponibles

| Comando | Descripción |
|---------|-------------|
| `CREATE <nombre> <tamaño>` | Crea un archivo y reserva bloques. |
| `WRITE <nombre> <offset> "texto"` | Escribe texto en el archivo. |
| `READ <nombre> <offset> <size>` | Lee una cantidad de bytes desde un offset. |
| `DELETE <nombre>` | Elimina un archivo y libera sus bloques. |
| `LIST` | Muestra los archivos existentes. |
| `#` | Comentarios ignorados. |

---

## Ejemplo de Archivo de Prueba

```txt
# Crear archivos
CREATE notas 400
CREATE datos 1500

# Escribir en archivos
WRITE notas 0 "Hola mundo!"
WRITE datos 200 "Prueba de bloques no contiguos"

# Leer contenido
READ notas 0 5
READ datos 200 30

# Listar archivos
LIST

# Eliminar
DELETE notas
LIST
```

---

## Conceptos Simulados

- **Gestión manual de bloques**
- **Asignación no contigua**
- **Directorios mínimos**
- **Límites de tamaño y validación**
- **Errores comunes (overflow, archivos inexistentes, falta de espacio)**

---
