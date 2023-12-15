// Alunos: Giovanni Mandel e Lucas Erdmann

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Flor {
  int id;
  double sepal_length, sepal_width, petal_length, petal_width;
  int species;
} Flor;

typedef struct Cluster {
  int num_elements;
  int* vertices;
  struct Flor centroid;
  int majority_species;
} Cluster;

typedef struct ClusterResult {
  int num_clusters;
  Cluster* clusters;
} ClusterResult;

Flor* readFlowersFromCSV(const char* filename, int* num_flowers) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error opening file.\n");
    return NULL;
  }

  Flor* flowers = NULL;
  int count = 0;
  char line[255];

  while (fgets(line, sizeof(line), file) != NULL) {
    if (count == 0) {
      count++;
      continue;  // Skip header line
    }

    Flor flower;
    sscanf(line, "%lf,%lf,%lf,%lf,%d",
           &flower.sepal_length, &flower.sepal_width, &flower.petal_length, &flower.petal_width, &flower.species);

    flower.id = count;
    flowers = (Flor*)realloc(flowers, count * sizeof(Flor));
    flowers[count - 1] = flower;

    count++;
  }

  fclose(file);

  *num_flowers = count - 1;
  return flowers;
}

double euclideanDistance(Flor flower1, Flor flower2) {
  return sqrt(pow(flower1.sepal_length - flower2.sepal_length, 2) +
              pow(flower1.sepal_width - flower2.sepal_width, 2) +
              pow(flower1.petal_length - flower2.petal_length, 2) +
              pow(flower1.petal_width - flower2.petal_width, 2));
}

ClusterResult performClustering(Flor* flowers, int num_flowers, double threshold) {
  ClusterResult result;
  result.num_clusters = 0;
  result.clusters = NULL;

  int* visited = (int*)calloc(num_flowers, sizeof(int));

  int i, j, k;
  for (i = 0; i < num_flowers; i++) {
    if (visited[i] == 1) {
      continue;  // Skip if flower is already visited
    }

    visited[i] = 1;

    Cluster cluster;
    cluster.num_elements = 1;
    cluster.vertices = (int*)malloc(sizeof(int));
    cluster.vertices[0] = i;
    cluster.centroid = flowers[i];
    cluster.majority_species = flowers[i].species;

    for (j = i + 1; j < num_flowers; j++) {
      if (visited[j] == 1) {
        continue;  // Skip if flower is already visited
      }

      double distance = euclideanDistance(flowers[i], flowers[j]);
      if (distance <= threshold) {
        visited[j] = 1;
        cluster.num_elements++;
        cluster.vertices = (int*)realloc(cluster.vertices, cluster.num_elements * sizeof(int));
        cluster.vertices[cluster.num_elements - 1] = j;

        cluster.centroid.sepal_length += flowers[j].sepal_length;
        cluster.centroid.sepal_width += flowers[j].sepal_width;
        cluster.centroid.petal_length += flowers[j].petal_length;
        cluster.centroid.petal_width += flowers[j].petal_width;

        if (flowers[j].species == 1) {
          cluster.majority_species += 1;
        } else if (flowers[j].species == 2) {
          cluster.majority_species += 2;
        } else if (flowers[j].species == 3) {
          cluster.majority_species += 3;
        }
      }
    }

    cluster.centroid.sepal_length /= cluster.num_elements;
    cluster.centroid.sepal_width /= cluster.num_elements;
    cluster.centroid.petal_length /= cluster.num_elements;
    cluster.centroid.petal_width /= cluster.num_elements;

    cluster.majority_species = (int)round((double)cluster.majority_species / cluster.num_elements);

    result.num_clusters++;
    result.clusters = (Cluster*)realloc(result.clusters, result.num_clusters * sizeof(Cluster));
    result.clusters[result.num_clusters - 1] = cluster;
  }

  free(visited);

  return result;
}

void printConfusionMatrix(ClusterResult clusters, int num_flowers) {
  int num_classes = 3;
  int** confusion_matrix = (int**)calloc(num_classes, sizeof(int*));

  int i, j;
  for (i = 0; i < num_classes; i++) {
    confusion_matrix[i] = (int*)calloc(num_classes, sizeof(int));
  }

  for (i = 0; i < clusters.num_clusters; i++) {
    Cluster cluster = clusters.clusters[i];
    for (j = 0; j < cluster.num_elements; j++) {
      int vertex = cluster.vertices[j];
      int species = cluster.majority_species;
      confusion_matrix[flowers[vertex].species - 1][species - 1]++;
    }
  }

  printf("Confusion Matrix:\n");
  printf("        Setosa Versicolor Virginica\n");
  for (i = 0; i < num_classes; i++) {
    printf("Setosa     ");
    for (j = 0; j < num_classes; j++) {
      printf("%-9d ", confusion_matrix[i][j]);
    }
    printf("\n");
  }

  printf("\nAccuracy by Species:\n");
  char* species_names[] = {"Setosa", "Versicolor", "Virginica"};
  for (i = 0; i < num_classes; i++) {
    int total = 0;
    int correct = 0;
    for (j = 0; j < num_classes; j++) {
      total += confusion_matrix[i][j];
      if (i == j) {
        correct = confusion_matrix[i][j];
      }
    }
    double accuracy = (double)correct / total;
    printf("%s: %.3f\n", species_names[i], accuracy);
  }

  for (i = 0; i < num_classes; i++) {
    free(confusion_matrix[i]);
  }
  free(confusion_matrix);
}

int main() {
  int num_flowers;
  Flor* flowers = readFlowersFromCSV("iris.csv", &num_flowers);
  if (flowers == NULL) {
    return 1;
  }

  double threshold = 0.3;
  ClusterResult clusters = performClustering(flowers, num_flowers, threshold);

  printConfusionMatrix(clusters, num_flowers);

  // Free memory
  for (int i = 0; i < clusters.num_clusters; i++) {
    free(clusters.clusters[i].vertices);
  }
  free(clusters.clusters);
  free(flowers);

  return 0;
}
