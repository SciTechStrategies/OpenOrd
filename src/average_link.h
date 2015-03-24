// This file contains compile time parameters and structure
// definition for the main average_link program.

#ifndef MAINH
#define MAINH

#include <string>

using namespace std;

#define VERSION "1.0 10/27/2005"

#define MAX_FILE_NAME 250   // max length of filename

// The int_node structure is an intermediate format 
// for storing node information read from a .coord file
struct int_node {
    int id;
    float x;
    float y;
};

// The node structure is the final format for storing node information 
struct node {
    string id;
    float x;
    float y;
};

// The id_pair structure is used to store indices of an edge pair
// in the final table (sorted by distance)
struct id_pair {
    int row;
    int col;
};

#endif
