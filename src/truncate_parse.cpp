// This file contains the methods for the parse.h class

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <cstdlib>

using namespace std;

#include <truncate_parse.h>

void parse::print_syntax( const char *error_string )
{
  cout << endl << "Error: " << error_string << endl;
  cout << endl << "Truncate" << endl
	   <<         "--------" << endl
	   << "S. Martin" << endl
	   << "Version " << VERSION << endl << endl
	   << "Converts a .sim formatted file into .ind, .full, and .int files for use" << endl
	   << "by recursive/parallel layout." << endl << endl
	   << "Usage: truncate [options] root_file" << endl << endl
	   << "root_file -- the root name of the files to be input and output." << endl << endl
	   << "INPUT" << endl
	   << "-----" << endl 
	   << "The root_file is the name of the .sim file without the .sim extension" << endl
	   << "and has the format\n\t id <tab> id <tab> sim," << endl
	   << "where ids are strings and sim is a float." << endl << endl
	   << "OUTPUTS" << endl
	   << "-------" << endl
	   << "The .ind file has two columns, the first giving the new integer ids" << endl
	   << "(starting at 0) and the second giving the original string ids." << endl << endl
	   << "The .full file contains the same information in the same format as the" << endl
	   << "input .sim file, except that the string ids are replaced by their integer" << endl
	   << "equivalents, and weights <=0 are screened out." << endl << endl
	   << "Finally, the .int file has the same format as the .full file, but contains" << endl
	   << "the truncated version (top n links) for use by layout." << endl << endl
	   << "OPTIONS" << endl
	   << "-------" << endl
	   << "\t-m : Number of times to scan .sim file >= 1." << endl
	   << "\t-n : Normalize output to .int file." << endl
	   << "\t-t top_n : Truncate .sim file using top n links before writing" << endl 
	   << "\t           to .int file >= 1 (default 10)." << endl
       << "\t-r : read a .coord file and create a .real file." << endl << endl;    
    exit(1);
}

parse::parse ( int argc, char** argv)
{
  map<string,string> m;

  // make sure there are at least two arguments and -l flag 
  if ( argc < 2)
	print_syntax ( "not enough arguments!" );
  
  // all input/output files use same root
  sim_file = ind_file = int_file = full_file = coord_file = real_file = argv[argc-1];
  
  // set defaults
  memory_use = 1;
  topn = 10;
  normalize = false;
  real_out = false;
  
  // now check for optional arguments
  string arg;
  for( int i = 1; i<argc-1; i++ )
  {
	arg = argv[i];

	// check for memory conservation
    if ( arg == "-m" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-m flag has no argument." );
		else
		{
			memory_use = atoi ( argv[i] );
			if ( memory_use < 1 )
				print_syntax ( "number of file scans must be >= 1." );
		}
	}
	
	// check for top link arguments
	else if ( arg == "-t" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-t flag has no argument." );
		else
		{
			topn = atoi ( argv[i] );
			if ( topn <= 0 )
				print_syntax ( "number of sim links must be >= 1." );
		}
	}
	
    // check for normalized output
    else if ( arg == "-n" )
	    normalize = true;
	else if ( arg == "-r" )
	    real_out = true;
    else
        print_syntax ( "unrecongized option!" );
  }
  
  // add approriate file suffixes
  sim_file = sim_file + ".sim";
  ind_file = ind_file + ".ind";
  int_file = int_file + ".int";
  full_file = full_file + ".full";
  coord_file = coord_file + ".coord";
  real_file = real_file + ".real";
  
  // echo sim_file and coord_file
  cout << "Using " << sim_file << " for .sim file, " << endl
       << "Will output " << ind_file << " for .ind file," << endl
	   << "            " << int_file << " for .int file, and" << endl
	   << "            " << full_file << " for .full file." << endl;
  
  if ( real_out )
     cout << "Using " << coord_file << " for .coord file, and" << endl
	      << "      " << real_file << " for .real file." << endl;
	 
  // echo arguments input or default
  cout << "Using memory = " << memory_use << endl
       << "      topn = " << topn << endl
	   << "      normalize = " << normalize << endl;

}
