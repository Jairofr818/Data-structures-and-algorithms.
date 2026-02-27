//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Jairo Figueroa Rojas
// Version     : 1.0
// Description : CS 300 Project Two - Advising Assistance Software (BST)
//============================================================================

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

//============================================================================
// Course structure
//============================================================================
struct Course {
    string courseNumber;
    string title;
    vector<string> prerequisites;

    Course() = default;
};

// Internal structure for tree node
struct Node {
    Course course;
    Node* left;
    Node* right;

    // default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // initialize with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================
class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course);
    void inOrder(Node* node) const;
    void destroyTree(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();

    void Insert(Course course);
    Course Search(string courseNumber) const;
    void InOrder() const;
    void Clear();
    bool IsEmpty() const;
};

// Default constructor
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

// Destructor
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
    root = nullptr;
}

// Insert a course
void BinarySearchTree::Insert(Course course) {
    // If there is nothing in the tree, the new node turns into the root.
    if (root == nullptr) {
        root = new Node(course);
    }
    else {
        addNode(root, course);
    }
}

// Add a course recursively to a node 
void BinarySearchTree::addNode(Node* node, Course course) {
    // To determine where to insert, compare courseNumber 
    if (course.courseNumber < node->course.courseNumber) {
        // Go left
        if (node->left == nullptr) {
            node->left = new Node(course);
        }
        else {
            addNode(node->left, course);
        }
    }
    else if (course.courseNumber > node->course.courseNumber) {
        // Go right
        if (node->right == nullptr) {
            node->right = new Node(course);
        }
        else {
            addNode(node->right, course);
        }
    }
    else {
        // If there is a Duplicate key replace
        node->course = course;
    }
}

// Traverse the tree sequentially (sorted output)
void BinarySearchTree::InOrder() const {
    inOrder(root);
}

// Recursive in-order traversal helper
void BinarySearchTree::inOrder(Node* node) const {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.title << endl;
        inOrder(node->right);
    }
}

// To look for a course
Course BinarySearchTree::Search(string courseNumber) const {
    // Starting from the root go down the tree
    Node* current = root;

    while (current != nullptr) {
        if (current->course.courseNumber == courseNumber) {
            return current->course;
        }

        // Traverse to the left or right according to the courseNumber order.
        if (courseNumber < current->course.courseNumber) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    // If is not found then return empty Course meaning that courseNumber will be empty
    Course empty;
    return empty;
}

// Remove every node (post-order).
void BinarySearchTree::destroyTree(Node* node) {
    if (node == nullptr) return;

    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

// Clear the BST
void BinarySearchTree::Clear() {
    destroyTree(root);
    root = nullptr;
}

// Verify that BST is empty.
bool BinarySearchTree::IsEmpty() const {
    return root == nullptr;
}

// Trim whitespace(to not get errors)
static string Trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;

    return s.substr(start, end - start);
}

// Convert string to uppercase(to not get errors)
static string ToUpper(string s) {
    for (char& ch : s) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return s;
}

// Divide CSV lines using commas(to not get errors)
static vector<string> SplitByComma(const string& line) {
    vector<string> tokens;
    string token;
    stringstream ss(line);

    while (getline(ss, token, ',')) {
        tokens.push_back(Trim(token));
    }

    return tokens;
}

//============================================================================
// Load courses from CSV file into BST
//============================================================================
static bool LoadCoursesFromFile(const string& filename, BinarySearchTree& bst) {

    ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    vector<vector<string>> coursesParsed;
    unordered_set<string> courseNumbersSeen;

    string line;

    // Read the file line by line
    while (getline(file, line)) {

        line = Trim(line);
        if (line.empty()) continue;

        vector<string> tokens = SplitByComma(line);

        // It requires course number and title
        if (tokens.size() < 2) {
            cout << "File format error." << endl;
            file.close();
            return false;
        }

        tokens[0] = ToUpper(tokens[0]); // Make the course number standard 
        courseNumbersSeen.insert(tokens[0]);
        coursesParsed.push_back(tokens);
    }

    file.close();

    // Verify the prerequisites after reading the full file.
    for (const auto& entry : coursesParsed) {

        for (size_t i = 2; i < entry.size(); i++) {
            string prereq = ToUpper(Trim(entry[i]));

            // Disregard empty values
            if (prereq.empty()) continue;

            // Notify if a prerequisite cannot be found
            if (courseNumbersSeen.find(prereq) == courseNumbersSeen.end()) {
                cout << "Warning: prerequisite " << prereq << " not found in file." << endl;
            }
        }
    }

    // Clear and add to BST 
    bst.Clear();

    for (const auto& entry : coursesParsed) {

        Course c;
        c.courseNumber = ToUpper(entry[0]);
        c.title = entry[1];

        for (size_t i = 2; i < entry.size(); i++) {
            string prereq = ToUpper(Trim(entry[i]));
            if (!prereq.empty()) c.prerequisites.push_back(prereq);
        }

        bst.Insert(c);
    }

    return true;
}

// Menu
static void PrintMenu() {
    cout << "1. Load Data Structure." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit" << endl;
}

// Get input choice from user
static int GetChoice() {
    cout << "What would you like to do? ";
    string input;
    getline(cin, input);
    input = Trim(input);

    try {
        size_t idx = 0;
        int choice = stoi(input, &idx);
        if (idx != input.size()) return -1;
        return choice;
    }
    catch (...) {
        return -1;
    }
}

// Display one course(details)
static void PrintCourse(BinarySearchTree& bst) {
    cout << "What course do you want to know about? ";
    string courseNum;
    getline(cin, courseNum);

    courseNum = ToUpper(Trim(courseNum));
    Course c = bst.Search(courseNum);

    if (c.courseNumber.empty()) {
        cout << "Course not found." << endl;
        return;
    }

    cout << c.courseNumber << ", " << c.title << endl;

    if (c.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < c.prerequisites.size(); i++) {
            cout << c.prerequisites[i];
            if (i + 1 < c.prerequisites.size()) cout << ", ";
        }
        cout << endl;
    }
}

//============================================================================
// Main
//============================================================================
int main() {

    BinarySearchTree bst;
    bool loaded = false;

    cout << "Welcome to the course planner." << endl;

    int choice = 0;

    while (choice != 9) {

        PrintMenu();
        choice = GetChoice();

        if (choice == 1) {
            cout << "Enter the file name: ";
            string filename;
            getline(cin, filename);

            if (!LoadCoursesFromFile(Trim(filename), bst)) {
                cout << "Error: Unable to open file." << endl;
                loaded = false;
            }
            else {
                loaded = true;
            }
        }
        else if (choice == 2) {
            if (!loaded) {
                cout << "Error: Please load the data structure first (Option 1)." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                bst.InOrder();
            }
        }
        else if (choice == 3) {
            if (!loaded) {
                cout << "Error: Please load the data structure first (Option 1)." << endl;
            }
            else {
                PrintCourse(bst);
            }
        }
        else if (choice == 9) {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            if (choice == -1) {
                cout << "That is not a valid option." << endl;
            }
            else {
                cout << choice << " is not a valid option." << endl;
            }
        }
    }

    return 0;
}