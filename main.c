// Alunos: Giovanni Mandel e Lucas Erdmann
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SAMPLES 1000
#define NUM_CLASSES 3
#define MAX_LINE_LENGTH 256
#define NUM_SAMPLES 150
typedef struct {
    char label[10];
    int true_positive;
    int false_positive;
    int false_negative;
} ClassStats;
typedef struct Flor {
  int id;
  double sepal_lenght, sepal_widht, petal_lenght, petal_widht;
  int type;
} Flor;
typedef struct Node {
  int vertex;
  struct Node *next;
} Node;
typedef struct Graph {
  int numVertices;
  Node **adjLists;
} Graph;
typedef struct Aresta {
  int vertice1, vertice2;
  struct Aresta *prox;
} Aresta;
typedef struct ListaArestas {
  Aresta *primeira;
  int quantidade;
} ListaArestas;
#define MAX_LINE_LENGTH 100


void filtrar_clusters(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int cluster_counts[5] = {0}; // Considerando um máximo de 5 clusters

    // Contar o número de elementos em cada cluster
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char* token = strtok(line, ",");
        int cluster = atoi(token);
        cluster_counts[cluster]++;
    }

    // Fechar o arquivo
    fclose(file);

    // Filtrar os clusters com menos de 2 elementos
    printf("Clusters com menos de 2 elementos:\n");
    for (int i = 0; i < 5; i++) {
        if (cluster_counts[i] < 2) {
            printf("%d\n", i);
        }
    }
}
Flor *getFlores() {

  Flor *flores = malloc(150 * sizeof(Flor));
  FILE *arq = fopen("IrisDataset.csv", "rt");
  char *result;
  char *token;
  char string[255];
  char *lixo = (char *)malloc(sizeof(char) * 100);
  fgets(lixo, 100, arq);
  free(lixo);

  int i = 0;
  while (!feof(arq)) {
    result = fgets(string, 150, arq);

    if (result) {
      token = strtok(result, ",");
    }

    Flor novo;
    int j = 0;
    while (token != NULL) {
      switch (j) {
      case 0:
        novo.sepal_lenght = atof(token);
        break;
      case 1:
        novo.sepal_widht = atof(token);
        break;
      case 2:
        novo.petal_lenght = atof(token);
        break;
      case 3:
        novo.petal_widht = atof(token);
        break;
      case 4:
        if (i <= 50) {
          novo.type = 0;
        } else if (i > 50 && i <= 100) {
          novo.type = 1;
        } else if (i > 100 && i <= 150) {
          novo.type = 2;
        }
      }
      token = strtok(NULL, ",");
      j++;
    }
    novo.id = i + 1;
    flores[i] = novo;
    i++;
  }

  return flores;
}

double **criaMatriz() {

  double **euclidiana;
  int i, j;
  euclidiana = (double **)malloc(150 * sizeof(double *));

  for (i = 0; i < 150; i++) {
    euclidiana[i] = (double *)malloc(150 * sizeof(double));
  }

  for (i = 0; i < 150; i++) {
    for (j = 0; j < 150; j++) {
      euclidiana[i][j] = 0;
    }
  }

  return euclidiana;
}
void extrairUltimaColunaCSV(const char* arquivo1, const char* arquivo2, const char* arquivo3) {
    FILE* file1 = fopen(arquivo1, "r");
    FILE* file2 = fopen(arquivo2, "r");
    FILE* file3 = fopen(arquivo3, "w");

    if (file1 == NULL || file2 == NULL || file3 == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return;
    }

    char linha1[100];
    char linha2[100];

    while (fgets(linha1, sizeof(linha1), file1) != NULL && fgets(linha2, sizeof(linha2), file2) != NULL) {
        // Encontrar a última coluna de cada linha
        char* ultimaColuna1 = strrchr(linha1, ',');
        char* ultimaColuna2 = strrchr(linha2, ',');

        if (ultimaColuna1 != NULL && ultimaColuna2 != NULL) {
            // Pular a vírgula da última coluna
            ultimaColuna1 += 1;
            ultimaColuna2 += 1;

            // Remover caracteres de nova linha e espaços em branco da última coluna
            ultimaColuna1[strcspn(ultimaColuna1, "\r\n")] = '\0';
            ultimaColuna2[strcspn(ultimaColuna2, "\r\n")] = '\0';

            fprintf(file3, "%s,%s\n", ultimaColuna1, ultimaColuna2);
        }
    }

    fclose(file1);
    fclose(file2);
    fclose(file3);


}
ListaArestas *resolveEnormaliza(Flor *flores) {

  double **matrizEuclidiana = criaMatriz();
  int i, j;
  double max = 0;
  double min = 1;

  for (i = 0; i < 150; i++) {
    for (j = 0; j < 150; j++) {
      if (i != j) {
        double euclidiana =
            sqrt(pow(flores[i].sepal_lenght - flores[j].sepal_lenght, 2) +
                 pow(flores[i].sepal_widht - flores[j].sepal_widht, 2) +
                 pow(flores[i].petal_lenght - flores[j].petal_lenght, 2) +
                 pow(flores[i].petal_widht - flores[j].petal_widht, 2));
        matrizEuclidiana[i][j] = euclidiana;

        if (euclidiana > max) {
          max = euclidiana;
        }
        if (euclidiana < min) {
          min = euclidiana;
        }
      }
    }
  }

  for (i = 0; i < 150; i++) {
    for (j = 0; j < 150; j++) {
      if (i != j) {
        matrizEuclidiana[i][j] = (matrizEuclidiana[i][j] - min) / (max - min);
      }
    }
  }

  ListaArestas *lista = malloc(sizeof(ListaArestas));
  Aresta *aresta;
  lista->quantidade = 0;
  for (i = 0; i < 150; i++) {
    for (j = 0; j < 150; j++) {
      if (matrizEuclidiana[i][j] <= 0.12 && i != j && i < j) {
        lista->quantidade++;

        if (lista->quantidade == 1) {
          lista->primeira = malloc(sizeof(Aresta));
          aresta = lista->primeira;
        } else {
          aresta->prox = malloc(sizeof(Aresta));
          aresta = aresta->prox;
        }

        aresta->vertice1 = i;
        aresta->vertice2 = j;
      }
    }
  }

  return lista;
}
char* getClassName(int class_id);
void adjustLastColumnCSV(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "r");
    FILE* output = fopen(outputFile, "w");

    if (input == NULL || output == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), input) != NULL) {
        char* token = strtok(line, ",");
        char lastColumn[10];

        // Avança até a última coluna
        while (token != NULL) {
            strcpy(lastColumn, token);
            token = strtok(NULL, ",");
        }

        // Converte o valor da última coluna para inteiro
        int value = atoi(lastColumn);

        // Verifica se o valor é maior que 2 e ajusta para 2 se necessário
        if (value > 2)
            value = 2;
		else if (value == 2){
			
			value = 1;
		}

        // Escreve a linha no arquivo de saída com o valor ajustado na última coluna
        fprintf(output, "%s,%d\n", line, value);
    }

    fclose(input);
    fclose(output);

}
char* getClassName(int class_id) {
    switch (class_id) {
        case 0:
            return "Setosa";
        case 1:
            return "Versicolor";
        case 2:
            return "Virginica";
        default:
            return "";
    }
}
void readCSV(const char* filename, int values[], int n) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    int i = 0;

    while (fgets(line, MAX_LINE_LENGTH, file)) {
        char* token = strtok(line, ",");
        int col = 0;
        char last_value[MAX_LINE_LENGTH];

        while (token != NULL) {
            if (col == (n - 1)) {
                strcpy(last_value, token);
            }

            token = strtok(NULL, ",");
            col++;
        }

        values[i] = atoi(last_value);
        i++;
    }

    fclose(file);
}
void arquivoGrafo(ListaArestas *lista) {

  FILE *arquivo = fopen("GrafoArestas.csv", "wt");
  Aresta *aux = lista->primeira;

  fprintf(arquivo, "%d\n", lista->quantidade);
  int k = 0;
  while (k < lista->quantidade) {
    fprintf(arquivo, "%d %d\n", aux->vertice1, aux->vertice2);
    aux = aux->prox;
    k++;
  }

  fclose(arquivo);
}
void arquivoGrafoPersisencia(ListaArestas *lista) {

  FILE *arquivo = fopen("GrafoPersistencia.txt", "wt");
  Aresta *aux = lista->primeira;

  fprintf(arquivo, "%d\n", lista->quantidade);
  int k = 0;
  while (k < lista->quantidade) {
    fprintf(arquivo, "%d, %d\n", aux->vertice1, aux->vertice2);
    aux = aux->prox;
    k++;
  }

  fclose(arquivo);
}
Node *createNode(int v) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->vertex = v;
  newNode->next = NULL;
  return newNode;
}
void save_clusters_to_csv(int *cluster, int numVertices, const char *file_path) {
  FILE *file = fopen(file_path, "w");
  if (file == NULL) {
    printf("Erro ao abrir o arquivo %s.\n", file_path);
    return;
  }

  fprintf(file, "Index,Cluster\n");
  for (int i = 0; i < numVertices; i++) {
    fprintf(file, "%d,%d\n", i, cluster[i]);
  }

  fclose(file);
  
}
// Função para criar o grafo com base no número de vértices
Graph *createGraph(int numVertices) {
  Graph *graph = (Graph *)malloc(sizeof(Graph));
  graph->numVertices = numVertices;
  graph->adjLists = (Node **)malloc(numVertices * sizeof(Node *));
  for (int i = 0; i < numVertices; i++) {
    graph->adjLists[i] = NULL;
  }
  return graph;
}
// Função para adicionar uma aresta entre dois vértices do grafo
void addEdge(Graph *graph, int src, int dest) {
  // Adicionando a aresta do vértice de origem ao vértice de destino
  Node *newNode = createNode(dest);
  newNode->next = graph->adjLists[src];
  graph->adjLists[src] = newNode;

  // Adicionando a aresta do vértice de destino ao vértice de origem
  newNode = createNode(src);
  newNode->next = graph->adjLists[dest];
  graph->adjLists[dest] = newNode;
}
void bfs(Graph *graph, int startVertex, int *visited, int *cluster,
         int clusterId) {
  // Criação da fila para o algoritmo BFS
  Node *queue = createNode(startVertex);
  visited[startVertex] = 1;
  cluster[startVertex] = clusterId;

  while (queue != NULL) {
    int currentVertex = queue->vertex;
    Node *temp = queue;
    queue = queue->next;
    free(temp);

    Node *adjList = graph->adjLists[currentVertex];
    while (adjList != NULL) {
      int adjVertex = adjList->vertex;
      if (visited[adjVertex] == 0) {
        queue = createNode(adjVertex);
        visited[adjVertex] = 1;
        cluster[adjVertex] = clusterId;
      }
      adjList = adjList->next;
    }
  }
}
void plot_confusion_matrix(const char* csv_file) {
    char true_labels[MAX_SAMPLES][10];
    char predicted_labels[MAX_SAMPLES][10];
    int confusion_matrix[3][3] = {0};  // Matriz de confusão de 3x3 (valores variam de 0 a 2)

    FILE* file = fopen(csv_file, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    int sample_count = 0;
    char true_label[10], predicted_label[10];

    while (fscanf(file, "%[^,],%s\n", true_label, predicted_label) == 2 && sample_count < MAX_SAMPLES) {
        strcpy(true_labels[sample_count], true_label);
        strcpy(predicted_labels[sample_count], predicted_label);

        // Converte as strings para inteiros
        int true_value = atoi(true_label);
        int predicted_value = atoi(predicted_label);

        // Incrementa o contador correspondente na matriz de confusão
        confusion_matrix[true_value][predicted_value]++;

        sample_count++;
    }

    fclose(file);

    // Imprime a matriz de confusão
    printf("Matriz de Confusao:\n");
    printf("             |  setosa  | versicolor | virginica |\n");
    printf("-----------------------------------------------\n");
    printf("   setosa    |    %d     |     %d      |    %d     |\n", confusion_matrix[0][0], confusion_matrix[0][1], confusion_matrix[0][2]);
    printf("-----------------------------------------------\n");
    printf(" versicolor  |    %d     |     %d      |    %d     |\n", confusion_matrix[1][0], confusion_matrix[1][1], confusion_matrix[1][2]);
    printf("-----------------------------------------------\n");
    printf("  virginica  |    %d     |     %d      |    %d     |\n", confusion_matrix[2][0], confusion_matrix[2][1], confusion_matrix[2][2]);
    printf("-----------------------------------------------\n");

    // Calcula a acurácia individual de cada classe
    ClassStats classes[3];
    strcpy(classes[0].label, "setosa");
    strcpy(classes[1].label, "virginica");
    strcpy(classes[2].label, "versicolor");

    for (int i = 0; i < 3; i++) {
        classes[i].true_positive = confusion_matrix[i][i];

        // Calcula os falsos positivos e falsos negativos
        for (int j = 0; j < 3; j++) {
            if (j != i) {
                classes[i].false_positive += confusion_matrix[j][i];
                classes[i].false_negative += confusion_matrix[i][j];
            }
        }

        // Calcula a acurácia de acerto para a classe
        double accuracy = (double)classes[i].true_positive / (classes[i].true_positive + classes[i].false_positive);
        printf("Acuracia de Acerto - %s: %.2f%%\n", classes[i].label, accuracy * 100);
    }
}




int main(int argc, char const *argv[]) {
  Flor *flores;
  ListaArestas *listaArestas;


  /* consome o arquivo .csv retornando uma lista das flores */
  flores = getFlores();
  /* recebe a lista de flores, resolve a distância euclidiana, normaliza e forma
   * uma lista com as distâncias euclidianas menores que 0.3 */
  listaArestas = resolveEnormaliza(flores);
  /* constrói o arquivo do grafo .csv */
  arquivoGrafo(listaArestas);
  /* constrói o arquivo TXT de persistência */
  arquivoGrafoPersisencia(listaArestas);

  // Criação do grafo com base no número de vértices
  Graph *graph = createGraph(150);
	
  // Adição das arestas no grafo com base na lista de arestas obtida
  Aresta *aresta = listaArestas->primeira;
  for (int i = 0; i < listaArestas->quantidade; i++) {
    addEdge(graph, aresta->vertice1, aresta->vertice2);
    aresta = aresta->prox;
  }

  // Arrays para controlar os vértices visitados e os clusters
  int visited[graph->numVertices];
  int cluster[graph->numVertices];

  // Inicializando os arrays
  for (int i = 0; i < graph->numVertices; i++) {
    visited[i] = 0;
    cluster[i] = -1;
  }

  int clusterId = 0;

  // Percorrendo todos os vértices do grafo
  for (int i = 0; i < graph->numVertices; i++) {
    // Se o vértice não foi visitado, executa o algoritmo BFS para encontrar o
    // cluster
    if (!visited[i]) {
      bfs(graph, i, visited, cluster, clusterId);
      clusterId++;
    }
  }
	
  // Exibindo os clusters encontrados
  printf("Clusters:\n");
  for (int i = 0; i < graph->numVertices; i++) {
    printf("Vertex %d: Cluster %d\n", i, cluster[i]);
  }

  int flowerCount[clusterId];
  memset(flowerCount, 0, sizeof(flowerCount));

  for (int i = 0; i < graph->numVertices; i++) {
    flowerCount[cluster[i]]++;
  }
	
 
  // Liberação da memória alocada
  for (int i = 0; i < graph->numVertices; i++) {
    Node *temp = graph->adjLists[i];
    while (temp != NULL) {
      Node *prev = temp;
      temp = temp->next;
      free(prev);
    }
  }
  save_clusters_to_csv(cluster, graph->numVertices, "clusters.csv");
  const char* arquivo_origem = "Original.csv";
  const char* arquivo_destino = "clusters.csv";
  const char* arquivo_resultado = "matriz.csv";

  extrairUltimaColunaCSV(arquivo_origem, arquivo_destino, arquivo_resultado);
  adjustLastColumnCSV("matriz.csv", "matrizNormal.csv");
  plot_confusion_matrix("matrizNormal.csv");
  free(graph->adjLists);
  free(graph);
  free(listaArestas);

  
  return 0;
}