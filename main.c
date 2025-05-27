#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX 100

int graph[MAX][MAX] = {{0}};
char placeNames[MAX][100];
char placeTypes[MAX][100];
int placeCount = 0;

typedef struct {
    char name[100];
    double lat;
    double lon;
} PlaceCoord;

PlaceCoord placeCoords[MAX];

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
        placeCoords[placeCount].lat = 0.0;
        placeCoords[placeCount].lon = 0.0;
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
        printf("Error opening file %s\n", filename);
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

// Read place coordinates from CSV (places.csv)
void readPlaceCoordinates(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file %s\n", filename);
        exit(1);
    }
    char line[256];
    fgets(line, sizeof(line), fp); // skip header

    while (fgets(line, sizeof(line), fp)) {
        char name[100];
        double lat, lon;
        sscanf(line, "%[^,],%lf,%lf", name, &lat, &lon);
        int idx = getIndex(name);
        if (idx != -1) {
            placeCoords[idx].lat = lat;
            placeCoords[idx].lon = lon;
        }
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
                printf("Visiting %s -> %s\n", placeNames[u], placeNames[v]);  // DEBUG
                dfsAllPaths(v, dest, visited, path, path_index);
            }
        }
    }

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
double calculateFare(int distance) {
    if (distance <= 5)
        return 50.0;
    else
        return 50.0 + (distance - 5) * 10.0;
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

void writePathToHTML(char* filename, int* path) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write HTML file.\n");
        return;
    }

    fprintf(fp, "<!DOCTYPE html><html><head><title>Shortest Route</title></head><body>");
    fprintf(fp, "<h2>Shortest Path</h2><p style='font-size:18px;'>");

    for (int i = 0; path[i] != -1; i++) {
        fprintf(fp, "%s", placeNames[path[i]]);
        if (path[i + 1] != -1)
            fprintf(fp, " → ");
    }

    fprintf(fp, "</p></body></html>");
    fclose(fp);
}

// Write shortest path route string to CSV (legacy)
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

// Write shortest_path.csv with lat/lon for folium map
void writeShortestPathCSV(char* filename, int* path) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write shortest_path.csv\n");
        return;
    }
    fprintf(fp, "lat,lon\n");
    for (int i = 0; path[i] != -1; i++) {
        int idx = path[i];
        fprintf(fp, "%lf,%lf\n", placeCoords[idx].lat, placeCoords[idx].lon);
    }
    fclose(fp);
}

// Write all_paths.csv with all edges for folium map
void writeAllPathsCSV(char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write all_paths.csv\n");
        return;
    }
    fprintf(fp, "path_id,lat,lon\n");
    int path_id = 0;
    for (int i = 0; i < placeCount; i++) {
        for (int j = i + 1; j < placeCount; j++) {
            if (graph[i][j] != 0) {
                fprintf(fp, "%d,%lf,%lf\n", path_id, placeCoords[i].lat, placeCoords[i].lon);
                fprintf(fp, "%d,%lf,%lf\n", path_id, placeCoords[j].lat, placeCoords[j].lon);
                path_id++;
            }
        }
    }
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
    // Reverse path
    int reversed[MAX];
    for (int i = 0; i < count; i++) {
        reversed[i] = path[count - i - 1];
    }
    reversed[count] = -1;

    printf("Shortest path distance: %d km\n", dist[end]);
    printf("Shortest path: ");
    for (int i = 0; reversed[i] != -1; i++) {
        printf("%s", placeNames[reversed[i]]);
        if (reversed[i + 1] != -1)
            printf(" -> ");
    }
    printf("\n");

    double fare = calculateFare(dist[end]);
    printf("Estimated travel fare: ₹%.2f\n", fare);

    writePathToCSV("route_output.csv", reversed);
    writePathToHTML("route_output.html", reversed);
    writeShortestPathCSV("shortest_path.csv", reversed);
}


// Find nearest place of given type from destination
void findNearestOfType(int destination, char* category) {
    int minDist = INT_MAX;
    int nearestIndex = -1;

    for (int i = 0; i < placeCount; i++) {
        if (strcmp(placeTypes[i], category) == 0) {
            if (graph[i][destination] != 0 && graph[i][destination] < minDist) {
                minDist = graph[i][destination];
                nearestIndex = i;
            }
        }
    }

    if (nearestIndex != -1) {
        printf("Nearest place of category '%s' from %s is %s at distance %d\n",
            category, placeNames[destination], placeNames[nearestIndex], minDist);
    } else {
        printf("No place of category '%s' found near %s\n", category, placeNames[destination]);
    }
}

int main() {
    readGraphFromCSV("graph.csv");
    readPlaceCoordinates("places.csv");
    printAdjacencyMatrix();

    char startName[100], endName[100], nearestCategory[100];
    printf("Enter the start location: ");
    fgets(startName, sizeof(startName), stdin);
    startName[strcspn(startName, "\n")] = 0;

    printf("Enter the destination location: ");
    fgets(endName, sizeof(endName), stdin);
    endName[strcspn(endName, "\n")] = 0;

    printf("Enter the category for nearest search: ");
    fgets(nearestCategory, sizeof(nearestCategory), stdin);
    nearestCategory[strcspn(nearestCategory, "\n")] = 0;

    int start = getIndex(startName);
    int end = getIndex(endName);

    if (start == -1 || end == -1) {
        printf("Invalid start or end location.\n");
        return 1;
    }

    printf("\nAll paths between %s and %s:\n", startName, endName);
    findAllPaths(start, end);

    printf("\nShortest path from %s to %s:\n", startName, endName);
    getShortestPath(start, end);

    writeAllPathsCSV("all_paths.csv");

    findNearestOfType(end, nearestCategory);

    return 0;
}


