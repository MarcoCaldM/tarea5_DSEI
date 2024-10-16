#ifndef DATASET_H
#define DATASET_H

#define NUM_POINTS 3003 // Numero de puntos en el dataset
#define K 10            // Valor de K en KNN
#define MAX_LINE_LENGTH 200

// Estructura para almacenar un punto (ahora con 3 dimensiones: Salon1, Salon2 y Salon3)
typedef struct {
    double x, y, z;  // Coordenadas del punto
    int label;       // Clase del punto
} Point;

extern Point dataset[NUM_POINTS];

#endif