#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue> 
#include <cmath>
#include <unordered_map>
#define pi 3.1415926535897932384626433832795
const double EARTH_RADIUS = 6371.0; //Radius of the Earth in kilometers

struct Node {
    double x; // Latitude
    double y; // Longitude
    std::string state; // State
    std::string county; // County
    Node* left;
    Node* right;
    Node(std::string state, std::string county, double x, double y)
        : state(state), county(county), x(x), y(y), left(nullptr), right(nullptr) {}
};

struct KdNodeDistance {
    Node* node;
    double distance;
    KdNodeDistance(Node* node, double distance) : node(node), distance(distance) {}
};

struct CompareDist {
    bool operator()(const std::pair<Node*, double>& a, const std::pair<Node*, double>& b) const {
        return a.second < b.second; // Comparison logic for distances
    }
};

class KDTree {
private:
    Node* root;
    int count; // Add a counter to keep track of the number of nodes
    void printInOrder(Node* root) const{
    if (root != nullptr) {
        printInOrder(root->left);
        std::cout << "Node: " << root->state << ", " << root->county << ", " << root->x << ", " << root->y << std::endl;
        printInOrder(root->right);
        }
    }

    Node* insertRec(Node* root, std::string state, std::string county, double x, double y, unsigned depth) {
        if (root == nullptr) {
            count++; // Increment the counter when a new node is created
            return new Node(state, county, x, y);
        }

        // Calculate current dimension (cd)
        unsigned cd = depth % 2;

        if (cd == 0) {
            if (x < root->x) {
                root->left = insertRec(root->left, state, county, x, y, depth + 1);
            } else {
                root->right = insertRec(root->right, state, county, x, y, depth + 1);
            }
        } else {
            if (y < root->y) {
                root->left = insertRec(root->left, state, county, x, y, depth + 1);
            } else {
                root->right = insertRec(root->right, state, county, x, y, depth + 1);
            }
        }

        return root;
    }
    double distance(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}
    
    
public:
    KDTree() : root(nullptr), count(0) {}

    void insert(std::string state, std::string county, double x, double y) {
        root = insertRec(root, state, county, x, y, 0);
    }
    void printInOrder() const {
        printInOrder(root);
    }

    int getCount() const {
        return count;
    }

    Node* getRoot() const {
        return root;
    }

    //search nearest 
    std::vector<Node*> kNearestNeighbors(Node* root, double queryX, double queryY, int k) {
        std::priority_queue<std::pair<Node*, double>, std::vector<std::pair<Node*, double> >, CompareDist> maxHeap;

        std::stack<Node*> nodeStack;
        std::vector<Node*> kNearestNodes; // Changed to store Node* instead of coordinates

        nodeStack.push(root);

        while (!nodeStack.empty()) {
            Node* current = nodeStack.top();
            nodeStack.pop();

            if (!current) continue;

            double currentDistance = distance(queryX, queryY, current->x, current->y);

            if (maxHeap.size() < k) {
                maxHeap.push(std::make_pair(current, currentDistance));
            } else {
                if (currentDistance < maxHeap.top().second) {
                    maxHeap.pop();
                    maxHeap.push(std::make_pair(current, currentDistance));
                }
            }

            int axis = 0; // Alternating between x and y coordinates
            if (queryX < current->x) {
                nodeStack.push(current->right);
                nodeStack.push(current->left);
            } else {
                nodeStack.push(current->left);
                nodeStack.push(current->right);
            }
        }

        while (!maxHeap.empty()) {
            kNearestNodes.push_back(maxHeap.top().first);
            maxHeap.pop();
        }

        return kNearestNodes;
    }

    std::pair<std::string, std::string> majorityVote(const std::vector<Node*>& neighbors) {
    std::unordered_map<std::string, int> stateCount, countyCount;
    for (auto & neighbor : neighbors) {
        stateCount[neighbor->state]++;
        countyCount[neighbor->county]++;
    }

    std::string majorityState, majorityCounty;
    int maxStateCount = 0, maxCountyCount = 0;
    for (auto & kv : stateCount) {
        if (kv.second > maxStateCount) {
            maxStateCount = kv.second;
            majorityState = kv.first;
        }
    }
    for (auto& kv : countyCount) {
        if (kv.second > maxCountyCount) {
            maxCountyCount = kv.second;
            majorityCounty = kv.first;
        }
    }
    return std::make_pair(majorityState, majorityCounty);
}

};

int main() {
    KDTree tree;
    std::string line, cell;

    // Open your CSV file
    std::ifstream file("/Data/2023_Gaz_counties_national.csv");

    // Read the first line to discard it if it contains headers
    std::getline(file, line);

    // get user input
    double testLatitude = -90;
    double testLongitude = 90;
    int k = 8;
    // std::cout << "Enter latitude: ";
    // std::cin >> testLatitude;
    // std::cout << "Enter longitude: ";
    // std::cin >> testLongitude;

    // // get K
    // int k;
    // std::cout << "Enter K (5-10 of nearest neighbors to find): ";
    // std::cin >> k;


    // Read lines from the CSV file
    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::vector<std::string> cells;

        // Split the line into cells/elements
        while (std::getline(lineStream, cell, ',')) {
            cells.push_back(cell);
        }

        // Parse the latitude and longitude from the cells
        // Assuming latitude is in the 8th column and longitude in the 9th
        double latitude = std::stod(cells[8]);
        double longitude = std::stod(cells[9]);
        std::string state = cells[0]; 
        std::string county = cells[3];

        // Insert the point into the KD-tree
        tree.insert(state, county, latitude, longitude);
    }

    // Test the findKNearest method with a value entered by the user
    std::cout << "Finding " << k << " nearest neighbors..." << std::endl;
    Node* root = tree.getRoot();
    auto nearestNeighbors = tree.kNearestNeighbors(root, testLatitude, testLongitude, k);
    if (root == NULL) {
        std::cout << "Nearest neighbors not found! root null" << std::endl;
        //std::cout << "root is:" << " x: " << root->x << " y: " << root->y << std::endl;

    }
    for (const auto& neighbor : nearestNeighbors) {
        std::cout << "Neighbor: " << neighbor->state << ", " << neighbor->county << ", " << neighbor->x << ", " << neighbor->y << std::endl;
    }

    // test majorityVote 
    if (k >= 5) {
        std::cout << "Testing majorityVote" << std::endl;
        std::pair<std::string, std::string> result = tree.majorityVote(nearestNeighbors);
        auto majorityState = result.first;
        auto majorityCounty = result.second;
        std::cout << "Majority State: " << majorityState << ", Majority County: " << majorityCounty << std::endl;
    } else {
        std::cout << "K is less than 5, skipping majorityVote test." << std::endl;
    }

    file.close();

    //print all node
    //tree.printInOrder();
    // Print the total count of nodes in the KDTree
   // std::cout << "Total nodes in KDTree: " << tree.getCount() << std::endl;

    return 0;

}


