#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue> 
#include <cmath>
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

struct Compare {
    bool operator()(const KdNodeDistance& a, const KdNodeDistance& b) {
        return a.distance > b.distance; // Min heap
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
    //search nearest 
void findKNearestRec(Node* root, double x, double y, int k, 
                     std::priority_queue<KdNodeDistance, std::vector<KdNodeDistance>, Compare>& pq, 
                     unsigned depth) {
    if (root == nullptr) return;

    double dist = haversineDistance(x, y, root->x, root->y);

    if (pq.size() < k) {
        pq.push(KdNodeDistance(root, dist));
    } else if (dist < pq.top().distance) {
        pq.pop();
        pq.push(KdNodeDistance(root, dist));
    }

    unsigned cd = depth % 2;
    Node* nextBranch = (cd == 0) ? ((x < root->x) ? root->left : root->right) : ((y < root->y) ? root->left : root->right);
    Node* otherBranch = (cd == 0) ? ((x < root->x) ? root->right : root->left) : ((y < root->y) ? root->right : root->left);

    // First, visit the subtree that is closer to the point
    findKNearestRec(nextBranch, x, y, k, pq, depth + 1);

    // Then, check if we need to visit the other subtree
    double distToPlane = (cd == 0) ? std::abs(root->x - x) : std::abs(root->y - y);
    if (pq.size() < k || distToPlane < pq.top().distance) {
        findKNearestRec(otherBranch, x, y, k, pq, depth + 1);
    }
}





    //Define the function that calculates the distance
    static double rad(double d)
    {   
        return d * pi /180.0;
    }

    static double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
        double dLat = rad(lat2 - lat1);
        double dLon = rad(lon2 - lon1);
        lat1 = rad(lat1);
        lat2 = rad(lat2);

        double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
        double c = 2 * asin(sqrt(a));
        return EARTH_RADIUS * c;
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
    std::vector<Node*> findKNearest(double x, double y, int k) {
        std::priority_queue<KdNodeDistance, std::vector<KdNodeDistance>, Compare> pq;
        findKNearestRec(root, x, y, k, pq, 0);

        std::vector<Node*> result;
        while (!pq.empty()) {
            auto& kdNodeDist = pq.top();
            Node* node = kdNodeDist.node;
            double dist = kdNodeDist.distance;

            // 
            std::cout << "Distance to Node at (" << node->x << ", " << node->y << "): " << dist << " km" << std::endl;

            result.push_back(node);
            pq.pop();
        }
        return result;
    }
    std::pair<std::string, std::string> majorityVote(const std::vector<Node*>& neighbors) {
    std::unordered_map<std::string, int> stateCount, countyCount;
    for (const auto & neighbor : neighbors) {
        stateCount[neighbor->state]++;
        countyCount[neighbor->county]++;
    }

    std::string majorityState, majorityCounty;
    int maxStateCount = 0, maxCountyCount = 0;
    for (const auto & kv : stateCount) {
        if (kv.second > maxStateCount) {
            maxStateCount = kv.second;
            majorityState = kv.first;
        }
    }
    for (const auto& kv : countyCount) {
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
    double testLatitude, testLongitude;
    std::cout << "Enter latitude: ";
    std::cin >> testLatitude;
    std::cout << "Enter longitude: ";
    std::cin >> testLongitude;

    // get K
    int k;
    std::cout << "Enter K (5-10 of nearest neighbors to find): ";
    std::cin >> k;


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
    auto nearestNeighbors = tree.findKNearest(testLatitude, testLongitude, k);
    for (const auto& neighbor : nearestNeighbors) {
        std::cout << "Neighbor: " << neighbor->state << ", " << neighbor->county << ", " << neighbor->x << ", " << neighbor->y << std::endl;
    }

    // test majorityVote 
    if (k >= 5) {
        std::cout << "Testing majorityVote" << std::endl;
        auto [majorityState, majorityCounty] = tree.majorityVote(nearestNeighbors);
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


