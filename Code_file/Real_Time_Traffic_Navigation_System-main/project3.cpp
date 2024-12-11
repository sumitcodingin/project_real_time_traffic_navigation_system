// Real time traffic Navigation system
// This projects aims to determine a path from one place to another by choosing the path which have the least distance and the traffic combination 
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <climits>
#include <string>
#include <algorithm>

using namespace std;

// Node structure for AVL Tree
// Avl tree used to insert the dynamic data such as traffic congestion data in the logn time so insertion and deletion will be easy 
struct AVLNode {
    int key;    // Unique edge id for each node  which depicts the edge of the graph for ex - an edge between 1 to 2 so key will be 1 * No of vertices + 2 = 1* 30 + 2 = 32
      
    // traffic < 2 -> low traffic , < 4 -> moderate traffic , < 6 -> high traffic , > 8 -> very high traffic  
    int traffic; // traffic associated with the each unique edges
    AVLNode* left; 
    AVLNode* right;
    int height;

    AVLNode(int k, int t) : key(k), traffic(t), left(nullptr), right(nullptr), height(1) {}
};

// Function to get the height of the AVL Node
int getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

// Function to get the balance factor of the AVL Node
int getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

// Function to perform right rotation on AVL Tree
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + max(getHeight(x->left), getHeight(x->right));

    return x;
}

// Function to perform left rotation on AVL Tree
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + max(getHeight(y->left), getHeight(y->right));

    return y;
}

// Function to insert a new traffic data point into the AVL Tree
AVLNode* insert(AVLNode* node, int key, int traffic) {
    if (!node) return new AVLNode(key, traffic); // if the tree is empty return the same node as the root node

    if (key < node->key)
        node->left = insert(node->left, key, traffic); 
    else if (key > node->key)
        node->right = insert(node->right, key, traffic);
    else
        return node;

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Graph structure
class Graph {
public:
    int V; // Number of vertices that depicts the number of cities in the graph
    vector<pair<int, int>>* adj; // Adjacency list 
    vector<string> cityNames; // City names for each vertex

    Graph(int V);
    void addEdge(int u, int v, int w);
    void addedge(int u, int v, int w);
    void dijkstra(int src, int dest, AVLNode* trafficData);
};

// Constructor to initialize graph
Graph::Graph(int V) {
    this->V = V;
    adj = new vector<pair<int, int>>[V];
    cityNames.resize(V); // Initialise the city names vector
}

// Function to add an edge to the graph with the desired weight which depicts the the travel distanca / time / cost from one place to another

// In an undirected graph
void Graph::addEdge(int u, int v, int w) {
    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w)); // Undirected graph
}

 // in an directed graph
 void Graph::addedge(int u, int v, int w) {
     adj[u].push_back(make_pair(v, w));
 }
// Function to perform Dijkstra's algorithm to find the shortest path
void Graph::dijkstra(int src, int dest, AVLNode* trafficData) {
    // make a priority queue for performing dikstra algo
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; 
    vector<int> dist(V, INT_MAX); // it track the minimum cost from the source destination to final destination
    vector<int> parent(V, -1); // To track the shortest path 

    dist[src] = 0; // distance from source to source ios 0
    pq.push(make_pair(0, src)); // push it into the the priority queue

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        // Traverse adjacent vertices
        for (auto& edge : adj[u]) {
            int v = edge.first;
            int w = edge.second;

            // Get traffic data
            int trafficCongestion = 0;
            int edge_id = min(u, v) * V + max(u, v); // Calculate edge ID
            AVLNode* current = trafficData;

            // to get the traffic data from the one place to another from avl tree 
            while (current) { 
                if (current->key == edge_id) {
                    trafficCongestion = current->traffic;
                    break;
                }
                current = (edge_id < current->key) ? current->left : current->right;
            }

            // Update distance if found a shorter path
            if (dist[u] + w + trafficCongestion < dist[v]) {
                dist[v] = dist[u] + w + trafficCongestion;
                parent[v] = u; // Update parent to reconstruct path
                pq.push(make_pair(dist[v], v));
            }
        }
    }

    // Print the shortest path from source to destination
    cout << "Shortest path from " << cityNames[src] << " to " << cityNames[dest] << " is: ";

    // If there is no path exist from one place to another then print NO path exist 
    if(dist[dest] == INT_MAX){ 
        cout << " NO Path Exist"<<endl;
    }
    vector<int> path;
    
    // print the path from source to destination 
    for (int v = dest; v != -1; v = parent[v]) {
        path.push_back(v);
    }

    reverse(path.begin(), path.end());

    for (int i = 0; i < path.size(); ++i) {
        cout << cityNames[path[i]];
        if (i < path.size() - 1) cout << " -> ";
    }
    
    // print the total time required to move from one place to another
    cout << "\nTotal Cost/ Time: " << dist[dest] << " km" << endl;

    // for the required path print the path that have extreme traffic
        for(int i = 1 ; i < path.size() ; i++){
        int v = path[i];
        int u = path[i - 1];

        int key = min(u , v) * V + max(u , v);
        AVLNode* current = trafficData;
        int trafficCongestion;
        while (current) { 
            if (current->key == key) {
                trafficCongestion = current->traffic;
                break;
            }
            current = (key < current->key) ? current->left : current->right;
        }

        if(trafficCongestion > 2){
            cout << "Traffic congestion detected on the road from " << cityNames[u] << " to " << cityNames[v] << " is " << "extreme / high "<<endl;
        }


    }
}

int main() {
    // Create a graph with 30 vertices

    int vertices = 30;
    Graph g(vertices);
    
    // City names initialization 
    string cities[vertices] = {
        "Delhi", "Mumbai", "Bangalore", "Chennai", "Kolkata", "Hyderabad", "Ahmedabad", 
        "Pune", "Jaipur", "Surat", "Kanpur", "Nagpur", "Lucknow", "Visakhapatnam", 
        "Patna", "Bhopal", "Thane", "Indore", "Vadodara", "Coimbatore", "Vijayawada", 
        "Mysore", "Nashik", "Rajkot", "Kalyan", "Varanasi", "Aurangabad", "Dhanbad", 
        "Srinagar", "Amritsar"
    };

    for (int i = 0; i < vertices; ++i) {
        g.cityNames[i] = cities[i];
    }

    // Example edges with distances
    g.addEdge(0, 1, 1400); // Delhi to Mumbai
    g.addEdge(0, 2, 2000); // Delhi to Bangalore
    g.addEdge(1, 2, 980);  // Mumbai to Bangalore
    g.addEdge(0, 3, 280);  // Delhi to Chennai
    g.addEdge(1, 3, 1200); // Mumbai to Chennai
    g.addEdge(2, 4, 800);  // Bangalore to Kolkata
    g.addEdge(2, 5, 600);  // Bangalore to Hyderabad
    g.addEdge(5, 6, 500);  // Hyderabad to Ahmedabad
    g.addEdge(6, 7, 300);  // Ahmedabad to Pune
    g.addEdge(7, 8, 300);  // Pune to Jaipur
    g.addEdge(8, 1, 1160); // Jaipur to Mumbai
    g.addEdge(4, 9, 1000); // Kolkata to Surat
    g.addEdge(1, 10, 900); // Mumbai to Kanpur
    g.addEdge(10, 11, 600); // Kanpur to Nagpur
    g.addEdge(11, 12, 700); // Nagpur to Lucknow
    g.addEdge(5, 13, 800); // Hyderabad to Visakhapatnam
    g.addEdge(13, 14, 400); // Visakhapatnam to Patna
    g.addEdge(0, 15, 1500); // Delhi to Bhopal
    g.addEdge(1, 16, 1300); // Mumbai to Thane
    g.addEdge(3, 17, 200); // Chennai to Indore
    g.addedge(4, 18, 300); // Kolkata to Vadodara
    g.addEdge(18, 19, 700); // Vadodara to Coimbatore
    g.addEdge(19, 20, 200); // Coimbatore to Vijayawada
    g.addedge(21, 22, 500); // Mysore to Nashik
    g.addEdge(22, 23, 400); // Nashik to Rajkot
    g.addEdge(24, 25, 300); // Kalyan to Varanasi
    g.addEdge(25, 26, 300); // Varanasi to Aurangabad
    g.addEdge(26, 27, 600); // Aurangabad to Dhanbad
    g.addEdge(28, 29, 800); // Srinagar to Amritsar
    g.addEdge(4, 9, 1000); // Kolkata to Surat
    g.addEdge(1, 10, 900); // Mumbai to Kanpur
    g.addEdge(10, 11, 600); // Kanpur to Nagpur
    g.addEdge(11, 12, 700); // Nagpur to Lucknow
    g.addEdge(5, 13, 800); // Hyderabad to Visakhapatnam
    g.addEdge(26, 14, 600); // Aurangabad to Patna
    g.addEdge(0, 25, 1540); // Delhi to Kalyan
    g.addEdge(1, 5, 1800); // Mumbai to Hyderabad
    g.addEdge(3, 17, 200); // Chennai to Indore
    g.addEdge(4, 18, 300); // Kolkata to Vadodara
    g.addEdge(13, 19, 700); // Vadodara to Coimbatore
    g.addEdge(15, 20, 200); // Coimbatore to Vijayawada
    g.addEdge(11, 22, 500); // Mysore to Nashik
    g.addEdge(19, 2, 190); // Nashik to Rajkot
    g.addEdge(24, 25, 250); // Kalyan to Varanasi
    g.addEdge(16, 20, 200); // Varanasi to Aurangabad
    g.addEdge(19, 7, 500); // Aurangabad to Dhanbad
    g.addEdge(8, 29, 700); // Srinagar to Amritsar
    

    // Create AVL Tree for traffic data
    AVLNode* trafficData = nullptr;
    trafficData = insert(trafficData, 0 * vertices + 1, 10);  // Edge (0, 1) has traffic level 10
    trafficData = insert(trafficData, 0 * vertices + 2, 5);   // Edge (0, 2) has traffic level 5
    trafficData = insert(trafficData, 1 * vertices + 2, 3);   // Edge (1, 2) has traffic level 3
    trafficData = insert(trafficData, 0 * vertices + 3, 2);   // Edge (0, 3) has traffic level 2
    trafficData = insert(trafficData, 8 * vertices + 1, 4);   // Edge (8, 1) has traffic level 4
    trafficData = insert(trafficData, 2 * vertices + 5, 1);   // Edge (2, 5) has traffic level 1
    trafficData = insert(trafficData, 0 * vertices + 1, 6);  // Edge (0, 1) has traffic level 6
    trafficData = insert(trafficData, 11 * vertices + 2, 3);   // Edge (11, 2) has traffic level 3
    trafficData = insert(trafficData, 3 * vertices + 11, 8);   // Edge (3, 11) has traffic level 8
    trafficData = insert(trafficData, 5 * vertices + 13, 1);   // Edge (5, 13) has traffic level 1
    trafficData = insert(trafficData, 7 * vertices + 8, 2);   // Edge (8, 1) has traffic level 2
    trafficData = insert(trafficData, 9 * vertices + 17, 4);   // Edge (9, 17) has traffic level 4

    // Run Dijkstra's algorithm from Delhi (0) to Pune(7)
    string source , destination;
    cout << "source ";
    cin >> source;
    cout << "destination ";
    cin >> destination;

    int sorce_index , dest_index; // find the source and destination index from the graph
    for(int i = 0 ; i < vertices ; i++){
        if(source == cities[i]){
            sorce_index = i; 
        }
        if(destination == cities[i]){
            dest_index = i;
        }
    }
    g.dijkstra(sorce_index, dest_index, trafficData); // Start from Delhi to Pune

    return 0;
}
