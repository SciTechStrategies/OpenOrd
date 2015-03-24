// This file contains the declarations of functions and variables.

// The average_link class contains the ported awk code from Kevin.
// The code is unmodified except for variable declarations and
// various features that have been commented out.

#ifndef AVERAGE_LINKH
#define AVERAGE_LINKH

#include <vector>
#include <string>
#include <map>
#include <average_link.h>

using namespace std;

class average_link {

public:

	// Methods
	void next_line ( int pid1, int pid2, float dist,
                     float x1, float y1, float x2, float y2 );
	void output_clusters ( string filename, map <int, node> &node_info );

	// Con/Decon
	average_link( int set_max_paper_id, float set_threshold );
	~average_link( ) { }
	
private:

	// variables in original awk code
	int nPairs;
	int nClusters;
	int papercount;
	int nJoins;
	
	int max_paper_id;
	float THRESHOLD;

	vector<int> cluster;
    vector<int> newcluster;
    
    // for each clust:
    vector<float> sumdist;  // sum coord distances
    vector<float> sumX;     // sum X
    vector<float> sumY;     // sum Y
    vector<int> nPapers;    // number of papers
    vector<int> nCords;     // number of coords
    vector<int> joinable;  // flag as to whether it is joinable after the threshold

    // for each action: 
    //   action[] -- 1) new 2) add 3) add after thresh 4) join
    //   actionid1[]
    //   actionid2[] 
    
    // for each paper:
    // clusternumber
    vector<int> importance; // importance   
    
};

#endif
