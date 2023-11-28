#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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
};

int main() {
    KDTree tree;
    std::string line, cell;

    // Open your CSV file
    std::ifstream file("/Users/ricardoxu/Desktop/EC504/Data/2023_Gaz_counties_national.csv");

    // Read the first line to discard it if it contains headers
    std::getline(file, line);

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

    file.close();

    //print all node
    tree.printInOrder();
    // Print the total count of nodes in the KDTree
    std::cout << "Total nodes in KDTree: " << tree.getCount() << std::endl;

    return 0;

}


