# Taller: Comparación de Mecanismos de Sincronización en el Problema de Lectores y Escritores

Este repositorio contiene el código fuente y las herramientas para un taller práctico enfocado en la evaluación comparativa de la eficiencia de diferentes mecanismos de sincronización aplicados al clásico Problema de Lectores y Escritores en un entorno concurrente.

---

## 1. Introducción al Problema de Lectores y Escritores

El Problema de Lectores y Escritores es un escenario fundamental en la programación concurrente donde múltiples procesos o hilos necesitan acceder a un recurso compartido. La complejidad radica en que:
* Los **"lectores"** pueden acceder al recurso simultáneamente sin causar conflictos, ya que solo observan los datos.
* Los **"escritores"** requieren acceso exclusivo al recurso, ya que modifican los datos y deben hacerlo de forma individual para evitar inconsistencias o corrupción.

El desafío es diseñar un mecanismo de sincronización que permita la máxima concurrencia para los lectores, pero garantice la exclusividad para los escritores, balanceando así la eficiencia y la integridad de los datos.

## 2. Técnicas de Sincronización Implementadas

Este taller implementa y compara el rendimiento de tres soluciones distintas al problema:

* **Mutex y Variables de Condición (Prioridad a Lectores):**
    Esta solución utiliza un `mutex` para garantizar la exclusión mutua en el acceso a las variables de estado y `variables de condición` para permitir a los hilos esperar de forma eficiente cuando el recurso no está disponible. Se prioriza el acceso de los lectores, permitiendo que múltiples lectores accedan si no hay un escritor activo.

* **Espera Activa (Busy-Waiting):**
    En esta implementación, los hilos que no pueden acceder al recurso en un momento dado entran en un bucle continuo de "espera activa", revisando repetidamente la condición de disponibilidad del recurso. Este método consume ciclos de CPU de forma ineficiente mientras el hilo espera, ya que no cede el control del procesador.

* **Semaforos (Prioridad a Escritores):**
    Esta solución utiliza semáforos para controlar el acceso al recurso compartido. Los semáforos permiten que múltiples lectores accedan simultáneamente, pero garantizan que solo un escritor pueda acceder al recurso a la vez, priorizando así el acceso de los escritores cuando están presentes.

## 3. Métricas de Evaluación

Para cuantificar y comparar la eficiencia de cada solución, se recolectan las siguientes métricas clave durante la ejecución:

* **Tiempo de Ejecución del Programa (s):** Duración total que le toma al programa completar todas sus operaciones (desde el inicio hasta la finalización de todos los hilos).
    * *Interpretación:* Menor valor = Mayor eficiencia.
* **Throughput Total (ops/s):** Cantidad total de operaciones (lecturas + escrituras) completadas por segundo.
    * *Interpretación:* Mayor valor = Mayor eficiencia/productividad.
* **Ciclos de CPU (cpu-cycles):** Número total de ciclos de reloj de la CPU consumidos por el programa.
    * *Interpretación:* Menor valor = Menor consumo de CPU, mayor eficiencia.
* **Task Clock Time (ms):** Tiempo acumulado que el procesador ha dedicado activamente a la ejecución de la tarea o proceso, excluyendo tiempos de espera.
    * *Interpretación:* Menor valor = Menor tiempo efectivo de CPU consumido, mayor eficiencia.

## 4. Requisitos y Configuración del Entorno

Para compilar y ejecutar las pruebas, necesitarás:

* **Sistema Operativo:** Un entorno Linux compatible (preferiblemente **WSL con Ubuntu**).
* **Compilador:** **GCC** (GNU Compiler Collection) con soporte para Pthreads (`-pthread`).
* **Herramienta de Construcción:** **`make`**.
* **Herramienta de Rendimiento:** **`perf`** para la recolección de métricas de rendimiento.

## 5. Estructura del Proyecto

El repositorio está organizado de la siguiente manera:

.
├── src/
│   ├── le_semaphore.c         
│   ├── le_busy_wait.c       
│   └── le_mutex_cond.c      
├── bin/                     
├── Makefile                 
├── test.sh                  
└── README.md                

## 6. Guía de Ejecución y Reproducción

Sigue estos pasos para compilar los programas y ejecutar las pruebas en tu entorno:

1.  **Clonar el Repositorio:**
    ```bash
    git clone https://github.com/vladimirnavarro/lectores_escritores_sync
    cd lectores_escritores_sync
    ```

2.  **Compilar los Programas:**
    Simplemente ejecuta `make` en la raíz del repositorio. El `Makefile` se encargará de compilar los programas (`le_mutex_cond`, `le_busy_wait`, `le_semaphore`) y colocarlos en la carpeta `bin/`.
    ```bash
    make
    ```
    *(Si en algún momento necesitas limpiar los archivos compilados, puedes usar `make clean`)*

3.  **Hacer el Script Ejecutable:**
    ```bash
    chmod +x test.sh
    ```

4.  **Ejecutar las Pruebas:**
    Simplemente ejecuta el script `test.sh` desde la raíz del repositorio:
    ```bash
    ./test.sh
    ```
    El script ejecutará automáticamente las tres soluciones en los escenarios de carga predefinidos (misma cantidad de R/W, más R que W, más W que R), cada uno tres veces, y recolectará todas las métricas.

5.  **Ver los Resultados:**
    Los resultados de las pruebas se guardarán en un archivo csv llamado `summary_metrics.csv` donde el número al final representa la ronda de pruebas (1, 2 o 3). Puedes abrir este archivo con cualquier editor de texto o software de hojas de cálculo para analizar los resultados. 

### Personalización de Escenarios

Si deseas modificar el número de hilos lectores y escritores o añadir nuevos escenarios de prueba, puedes editar el script `test.sh`. Las configuraciones se definen mediante llamadas a la función `run_test_case`, siguiendo el formato:

```bash
run_test_case "nombre_del_ejecutable" "nombre_del_escenario" "numero_lectores" "numero_escritores"
```

### Realizado por:
* Vladimir Antonio Navarro Tejeda  
* Sebastián David Castro Arrieta