#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue> 
#include <cmath>
#include <unordered_map>
#include <chrono> 
#include <iostream>
#include <iomanip>

constexpr double PI = 3.14159265358979323846;
constexpr double EARTH_RADIUS = 6371.0; // Radius of the Earth in kilometers


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
        return a.distance < b.distance; // Max heap
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
    void findKNearestRec(Node* root, double x, double y, int k, std::priority_queue<KdNodeDistance, std::vector<KdNodeDistance>, Compare>& pq, unsigned depth) {
        if (root == nullptr) return;

        double dist = haversineDistance(x, y, root->x, root->y);

        if (pq.size() < k) {
            pq.push(KdNodeDistance(root, dist));
        } else if (dist < pq.top().distance) {
            pq.pop();
            pq.push(KdNodeDistance(root, dist));
        }


        // Determine which subtree to search
        unsigned cd = depth % 2;
        Node* nextBranch = (cd == 0) ? ((x < root->x) ? root->left : root->right) : ((y < root->y) ? root->left : root->right);
        Node* otherBranch = (cd == 0) ? ((x < root->x) ? root->right : root->left) : ((y < root->y) ? root->right : root->left);

        findKNearestRec(nextBranch, x, y, k, pq, depth + 1);

        // Check if we need to explore the other branch
        double distToPlane = (cd == 0) ? std::abs(root->x - x) : std::abs(root->y - y);
        if (pq.size() < k || distToPlane < pq.top().distance) {
            findKNearestRec(otherBranch, x, y, k, pq, depth + 1);
        }
    }

    //Define the function that calculates the distance
    static double rad(double d)
    {   
        return d * PI /180.0;
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
    
    std::vector<KdNodeDistance> findKNearest(double x, double y, int k) {
        std::priority_queue<KdNodeDistance, std::vector<KdNodeDistance>, Compare> pq;
        findKNearestRec(root, x, y, k, pq, 0);

        std::vector<KdNodeDistance> result;
        while (!pq.empty()) {
            result.push_back(pq.top());
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./load <input_file>" << std::endl;
        return 1;
    }
    using namespace std::chrono; 
    auto start = high_resolution_clock::now();
    KDTree tree;
    std::string inputFilePath = argv[1];
    std::ifstream inputFile(inputFilePath); // input
    if (!inputFile) {
        std::cerr << "Error opening input file: " << inputFilePath << std::endl;
        return 1;
    }
    //create output path
    std::size_t slashPos = inputFilePath.find_last_of("/\\");
    std::string fileName = (slashPos == std::string::npos) ? inputFilePath : inputFilePath.substr(slashPos + 1);
    std::size_t dotPos = fileName.find_last_of(".");
    std::string baseName = (dotPos == std::string::npos) ? fileName : fileName.substr(0, dotPos);
    std::string outputFilePath = "./output/" + baseName + "_output.txt";


    std::ofstream outfile(outputFilePath); // output
    if (!outfile) {
        std::cerr << "Error opening output file: " << outputFilePath << std::endl;
        return 1;
    }
    // Open your CSV file
    std::ifstream dataFile("Data/2023_Gaz_counties_national.csv");
    if (!dataFile) {
        std::cerr << "Error opening data file" << std::endl;
        return 1;
    }

    std::string dataline, cell;

    // Read the first line to discard it if it contains headers
    std::getline(dataFile, dataline);
            // Read lines from the CSV file
    while (std::getline(dataFile, dataline)) {
        std::stringstream lineStream(dataline);
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
        // get input from txt file
        std::string inputLine;
    while (std::getline(inputFile, inputLine)) {
        double testLatitude = std::stod(inputLine);

        if (!std::getline(inputFile, inputLine)) break;
        double testLongitude = std::stod(inputLine);

        if (!std::getline(inputFile, inputLine)) break;
        int k = std::stoi(inputLine);
    /*   This is get input from user type
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
    */



        // Test the findKNearest method with a value entered by the user
        std::cout << "Finding " << k << " nearest neighbors..." << std::endl; // show in consle 
        outfile << "Finding " << k << " nearest neighbors..." << std::endl; // in output.txt

        auto nearestNeighbors = tree.findKNearest(testLatitude, testLongitude, k);
        for (const auto& neighborDist : nearestNeighbors) {
            Node* neighbor = neighborDist.node;
            double dist = neighborDist.distance;
            std::cout << "Neighbor: " << neighbor->state << ", " << neighbor->county
                    << ", " << std::fixed << std::setprecision(3) << neighbor->x << "°, "
                    << std::fixed << std::setprecision(3) << neighbor->y << "° Distance: "
                    << std::fixed << std::setprecision(4) << dist << "km" << std::endl;
            // write in output.txt
            outfile << "Neighbor: " << neighbor->state << ", " << neighbor->county
                    << ", " << std::fixed << std::setprecision(3) << neighbor->x << "°, "
                    << std::fixed << std::setprecision(3) << neighbor->y << "° Distance: "
                    << std::fixed << std::setprecision(4) << dist << "km" << std::endl;
        }
        std::vector<Node*> nodesForMajorityVote;
        for (const auto& neighborDist : nearestNeighbors) {
            nodesForMajorityVote.push_back(neighborDist.node);
        }
        // test majorityVote 
        if (k >= 5) {
            auto [majorityState, majorityCounty] = tree.majorityVote(nodesForMajorityVote);
            std::cout << "Majority State: " << majorityState << std::endl;
            outfile << "Majority State: " << majorityState << std::endl;

        } else {
            std::cout << "K is less than 5, skipping majorityVote test." << std::endl;
            outfile << "K is less than 5, skipping majorityVote test." << std::endl;

        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        std::cout << "Time taken by function: "
                << duration.count() << " microseconds" << std::endl;
        outfile << "Time taken by function: "
                << duration.count() << " microseconds" << std::endl;
    }
        dataFile.close();
        inputFile.close();
        outfile.close();
        //print all node
        //tree.printInOrder();
        // Print the total count of nodes in the KDTree
    // std::cout << "Total nodes in KDTree: " << tree.getCount() << std::endl;

        return 0;

}


