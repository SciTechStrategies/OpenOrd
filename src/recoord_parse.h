// The parse class contains the methods necessary to parse
// the command line, print help, and do error checking

#ifndef PARSEH
#define PARSEH

#define VERSION "1.0 5/13/2006"

class parse {

public:

    // Methods
	
	parse ( int argc, char **argv );
	~parse () {}
	
	// user parameters
	string icoord_file;		// .icoord file (input)
	string ind_file;        // .ind file (input)
	string coord_file;	    // .coord file (output)
    string edges_file;		// .edges file (output)
	string iedges_file;		// .iedges file (input)

    bool edges_out;			// true to produce .iedges output
	
private:

	void print_syntax ( const char *error_string );

};

#endif
