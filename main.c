#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100

int graph[MAX][MAX] = {{0}};
char placeNames[MAX][100];
char placeTypes[MAX][100];
int placeCount = 0;

// Map place name to index
int getIndex(char* name) {
    for (int i = 0; i < placeCount; i++) {
        if (strcmp(placeNames[i], name) == 0)
            return i;
    }
    return -1;
}

// Add a new place
int addPlace(char* name, char* type) {
    int index = getIndex(name);
    if (index == -1) {
        strcpy(placeNames[placeCount], name);
        strcpy(placeTypes[placeCount], type);
        // Initialize new row and column in graph to 0
        for (int i = 0; i <= placeCount; i++) {
            graph[placeCount][i] = 0;
            graph[i][placeCount] = 0;
        }
        placeCount++;
        return placeCount - 1;
    }
    return index;
}

// Read graph from CSV
void readGraphFromCSV(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file\n");
        exit(1);
    }

    char line[256];
    fgets(line, sizeof(line), fp); // Skip header

    while (fgets(line, sizeof(line), fp)) {
        char src[100], dest[100], srcType[100], destType[100];
        int dist;

        sscanf(line, "%[^,],%[^,],%d,%[^,],%[^\n]", src, dest, &dist, srcType, destType);

        int srcIndex = addPlace(src, srcType);
        int destIndex = addPlace(dest, destType);

        graph[srcIndex][destIndex] = dist;
        graph[destIndex][srcIndex] = dist;
    }

    fclose(fp);
}

// Print adjacency matrix
void printAdjacencyMatrix() {
    printf("Adjacency Matrix:\n\t");
    for (int i = 0; i < placeCount; i++)
        printf("%s\t", placeNames[i]);
    printf("\n");

    for (int i = 0; i < placeCount; i++) {
        printf("%s\t", placeNames[i]);
        for (int j = 0; j < placeCount; j++) {
            if (graph[i][j] == 0)
                printf("-\t");
            else
                printf("%d\t", graph[i][j]);
        }
        printf("\n");
    }
}

// DFS to find all paths
void dfsAllPaths(int u, int dest, int visited[], int path[], int path_index) {
    visited[u] = 1;
    path[path_index] = u;
    path_index++;

    if (u == dest) {
        for (int i = 0; i < path_index; i++) {
            printf("%s", placeNames[path[i]]);
            if (i != path_index - 1)
                printf(" -> ");
        }
        printf("\n");
    } else {
        for (int v = 0; v < placeCount; v++) {
            if (graph[u][v] && !visited[v]) {
                dfsAllPaths(v, dest, visited, path, path_index);
            }
        }
    }

    path_index--;
    visited[u] = 0;
}

void findAllPaths(int start, int end) {
    int visited[MAX] = {0};
    int path[MAX];
    dfsAllPaths(start, end, visited, path, 0);
}

// Dijkstra
int minDistance(int dist[], int sptSet[]) {
    int min = INT_MAX, minIndex = -1;
    for (int v = 0; v < placeCount; v++) {
        if (!sptSet[v] && dist[v] <= min) {
            min = dist[v];
            minIndex = v;
        }
    }
    return minIndex;
}

void dijkstra(int src, int dist[], int parent[]) {
    int sptSet[MAX] = {0};

    for (int i = 0; i < placeCount; i++) {
        dist[i] = INT_MAX;
        parent[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < placeCount - 1; count++) {
        int u = minDistance(dist, sptSet);
        if (u == -1) break;
        sptSet[u] = 1;

        for (int v = 0; v < placeCount; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }
}

// Write shortest path to CSV
void writePathToCSV(char* filename, int* path) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write path to file.\n");
        return;
    }

    fprintf(fp, "Route\n");
    for (int i = 0; path[i] != -1; i++) {
        fprintf(fp, "%s", placeNames[path[i]]);
        if (path[i + 1] != -1)
            fprintf(fp, " -> ");
    }
    fprintf(fp, "\n");

    fclose(fp);
}

// Reconstruct and display shortest path
void getShortestPath(int start, int end) {
    int dist[MAX], parent[MAX];
    dijkstra(start, dist, parent);

    if (dist[end] == INT_MAX) {
        printf("No path exists.\n");
        return;
    }

    int path[MAX];
    int count = 0;
    int crawl = end;

    while (crawl != -1) {
        path[count++] = crawl;
        crawl = parent[crawl];
    }

    int reversed[MAX];
    for (int i = 0; i < count; i++)
        reversed[i] = path[count - 1 - i];
    reversed[count] = -1;

    printf("\nShortest path from %s to %s:\n", placeNames[start], placeNames[end]);
    for (int i = 0; i < count; i++) {
        printf("%s", placeNames[reversed[i]]);
        if (i != count - 1)
            printf(" -> ");
    }
    printf("\n");

    writePathToCSV("route_output.csv", reversed);
    printf("Shortest path written to route_output.csv\n");
}

// Find nearest location of given type
void findNearestOfType(int src, char* category) {
    int dist[MAX], parent[MAX];
    dijkstra(src, dist, parent);

    int minDist = INT_MAX;
    int nearest = -1;

    for (int i = 0; i < placeCount; i++) {
        if (strcmp(placeTypes[i], category) == 0 && dist[i] < minDist) {
            minDist = dist[i];
            nearest = i;
        }
    }

    if (nearest == -1) {
        printf("No location of category '%s' found near %s.\n", category, placeNames[src]);
        return;
    }

    printf("\nNearest %s to %s is %s (%d units away).\n", category, placeNames[src], placeNames[nearest], minDist);
}

int main() {
    char destName[100];
    char nearestCategory[100];

    readGraphFromCSV("graph.csv");
    printAdjacencyMatrix();

    // User input for destination and nearest category
    printf("\nEnter destination location: ");
    scanf(" %[^\n]", destName);

    printf("Enter category to find nearest (e.g., hospital, bus stop): ");
    scanf(" %[^\n]", nearestCategory);

    char startName[100];
printf("Enter starting location: ");
scanf(" %[^\n]", startName);
int start = getIndex(startName);

    int end = getIndex(destName);

    if (start == -1 || end == -1) {
        printf("Invalid start or destination.\n");
        return 1;
    }

    printf("\nAll paths from %s to %s:\n\n", placeNames[start], placeNames[end]);
    findAllPaths(start, end);

    getShortestPath(start, end);

    findNearestOfType(end, nearestCategory);

    return 0;
}
