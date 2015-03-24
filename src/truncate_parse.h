// The parse class contains the methods necessary to parse
// the command line, print help, and do error checking

#ifndef PARSEH
#define PARSEH

#define VERSION "1.0 5/3/2006"

class parse {

public:

    // Methods
	
	parse ( int argc, char **argv );
	~parse () {}
	
	// user parameters
	string sim_file;		// .sim file (input)
	string ind_file;        // .int file (output)
	string int_file;        // output for layout
	string full_file;	    // .full file (output)
    string coord_file;		// .coord file (output)
	string real_file;		// .real file (output)
	
	int memory_use;         // scan files this many times
	int topn;	            // number of shortest links to keep
	bool normalize;		    // true to produce normalized output
    bool real_out;			// true to produce .real output
	
private:

	void print_syntax ( const char *error_string );

};

#endif
