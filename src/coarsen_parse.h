// The parse class contains the methods necessary to parse
// the command line, print help, and do error checking

#ifndef PARSEH
#define PARSEH

#define VERSION "1.0 1/27/2005"

class parse {

public:

    // Methods
	
	parse ( int argc, char **argv );
	~parse () {}
	
	// user parameters
	string full_file;		// .full file (input)
	string clust_file;      // .clust file (input)
	string full_out_file;   // output .full to here
	string int_out_file;    // truncated input to clustering algorithm
	
	int level;            // threshold value real >= 0
	int top_n_links[2];	    // number of shortest links to add > 0
	bool normalized_output; // true to produce normalized output
	int memory_use;         // number of times to scan file 
    
private:

	void print_syntax ( const char *error_string );
	string itoa ( int i );
};

#endif
