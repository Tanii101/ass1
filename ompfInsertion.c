#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#include "coordReader.h"

#define MAX_COORDS 1000

double euclidean_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void create_distance_matrix(double **distance_matrix, double **coords, int num_coords) {
    for (int i = 0; i < num_coords; ++i) {
        for (int j = 0; j < num_coords; ++j) {
            distance_matrix[i][j] = euclidean_distance(coords[i][0], coords[i][1], coords[j][0], coords[j][1]);
        }
    }
}

void parallel_minmax_insertion(double **distance_matrix, int num_coords, int *tour) {
    int visited[MAX_COORDS] = {0};
    tour[0] = 0;
    visited[0] = 1;
    int tour_length = 1;

    while (tour_length < num_coords) {
        int best_vertex = -1;
        double best_max_cost = INFINITY;

        #pragma omp parallel for
        for (int i = 1; i < num_coords; ++i) {
            if (!visited[i]) {
                double max_cost = 0;
                for (int j = 0; j < tour_length; ++j) {
                    double cost = distance_matrix[tour[j]][i];
                    if (cost > max_cost) {
                        max_cost = cost;
                    }
                }
                #pragma omp critical
                {
                    if (max_cost < best_max_cost) {
                        best_max_cost = max_cost;
                        best_vertex = i;
                    }
                }
            }
        }

        double best_insert_cost = INFINITY;
        int best_insert_pos = -1;

        #pragma omp parallel for
        for (int i = 0; i < tour_length; ++i) {
            int j = (i + 1) % tour_length;
            double insert_cost = distance_matrix[tour[i]][best_vertex] +
                                 distance_matrix[best_vertex][tour[j]] -
                                 distance_matrix[tour[i]][tour[j]];
            #pragma omp critical
            {
                if (insert_cost < best_insert_cost) {
                    best_insert_cost = insert_cost;
                    best_insert_pos = j;
                }
            }
        }

        for (int i = tour_length; i > best_insert_pos; --i) {
            tour[i] = tour[i - 1];
        }
        tour[best_insert_pos] = best_vertex;
        visited[best_vertex] = 1;
        tour_length++;
    }
    tour[num_coords] = tour[0];
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <coords_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *coords_file = argv[1];
    const char *output_file = argv[2];

    int num_coords = readNumOfCoords(coords_file);
    if (num_coords < 0) {
        return EXIT_FAILURE;
    }

    double **coords = readCoords(coords_file, num_coords);
    if (coords == NULL) {
        return EXIT_FAILURE;
    }

    double **distance_matrix = (double **)malloc(num_coords * sizeof(double *));
    for (int i = 0; i < num_coords; i++) {
        distance_matrix[i] = (double *)malloc(num_coords * sizeof(double));
    }
    create_distance_matrix(distance_matrix, coords, num_coords);

    int *tour = (int *)malloc((num_coords + 1) * sizeof(int));
    parallel_minmax_insertion(distance_matrix, num_coords, tour);

    writeTourToFile(tour, num_coords + 1, output_file);

    for (int i = 0; i < num_coords; i++) {
        free(coords[i]);
        free(distance_matrix[i]);
    }
    free(coords);
    free(distance_matrix);
    free(tour);

    return EXIT_SUCCESS;
}
