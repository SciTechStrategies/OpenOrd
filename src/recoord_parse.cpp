// This file contains the methods for the parse.h class

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <cstdlib>

using namespace std;

#include <recoord_parse.h>

void parse::print_syntax( const char *error_string )
{
  cout << endl << "Error: " << error_string << endl;
  cout << endl << "Recoord" << endl
	   <<         "-------" << endl
	   << "S. Martin" << endl
	   << "Version " << VERSION << endl << endl
	   << "Converts a .icoord formatted file into a .coord file." << endl << endl
	   << "Usage: recoord [options] root_file" << endl << endl
	   << "root_file -- the root name of the files to be input and output." << endl << endl
	   << "INPUT" << endl
	   << "-----" << endl 
	   << "The root_file is the name of the .icoord file without the .icoord extension." << endl
	   << "A root_file.ind file must also be present, as produced by truncate." << endl << endl
	   << "OUTPUTS" << endl
	   << "-------" << endl
	   << "The .coord file has the coordinates produced by layout using the original ids." << endl
	   << "A .edges file is optionally produced using a .iedges file from layout." << endl << endl
	   << "OPTIONS" << endl
	   << "-------" << endl
       << "\t-e : read a .iedges file and create a .edges file." << endl << endl;    
    exit(1);
}

parse::parse ( int argc, char** argv)
{

  // make sure there are at least two arguments 
  if ( argc < 2)
	print_syntax ( "not enough arguments!" );
  
  // all input/output files use same root
  coord_file = ind_file = edges_file = icoord_file = iedges_file = argv[argc-1];
  
  // set defaults
  edges_out = false;
  
  // now check for optional arguments
  string arg;
  for( int i = 1; i<argc-1; i++ )
  {
	arg = argv[i];
	
	// check for edges file
	if ( arg == "-e" )
	    edges_out = true;
    else
        print_syntax ( "unrecongized option!" );
  }
  
  // add approriate file suffixes
  coord_file = coord_file + ".coord";
  ind_file = ind_file + ".ind";
  edges_file = edges_file + ".edges";
  iedges_file = iedges_file + ".iedges";
  icoord_file = icoord_file + ".icoord";
  
  // echo sim_file and coord_file
  cout << "Using " << icoord_file << " for .icoord file, and " << endl
       << "       " << ind_file << " for .ind file." << endl
	   << "Will output " << coord_file << " for .coord file." << endl;
  
  if ( edges_out )
    cout << "Using " << iedges_file << " for .iedges file, and" << endl
	     << "      " << edges_file << " for .edges file." << endl;
		 
}
