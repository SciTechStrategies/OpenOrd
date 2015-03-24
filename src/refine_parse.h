// The parse class contains the methods necessary to parse
// the command line, print help, and do error checking

#ifndef PARSEH
#define PARSEH

#define VERSION "1.0 5/9/2006"

class parse {

public:

    // Methods
	
	parse ( int argc, char **argv );
	~parse () {}
	
	// user parameters
	string blob_file;		// .full file (input)
	string clust_file;      // .clust file (input)
	string real_file;       // output for layout
	string refine_file;		// refined int file
	string coarse_file;		// coarse int file
	
	int level;            // threshold value real >= 0
	float scale;		  // scale coords to fix in box
    bool refine_int;	  // refine .int file as well
	
private:

	void print_syntax ( const char *error_string );
	string itoa ( int i );
};

#endif
