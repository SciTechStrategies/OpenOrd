// The parse class contains the methods necessary to parse
// the command line, print help, and do error checking

#ifndef PARSEH
#define PARSEH

class parse {

public:

    // Methods
	
	parse ( int argc, char **argv );
	~parse () {}
	
	// user parameters
	string sim_file;		// .sim file (input)
	string coord_file;		// .coord file (input)
	string edges_file;      // .edges file (input)
	string slc_file;        // output file .clust
	string dist_file;       // .mindist
	string clustin_file;    // input to clustering algorithm
	
	float threshold;        // threshold value real >= 0
	int num_short_links;	// number of shortest links to add > 0
	bool output_dist;       // true to output .dist file
    int neighborhood_size;  // radius of neighborhood
    
private:

	void print_syntax ( const char *error_string );
	
};

#endif
