// This file contains the methods for the parse.h class

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <cstdlib>

using namespace std;

#include <refine_parse.h>

// convert a number to a string!
string parse::itoa ( int i )
{
  ostringstream o;
  o << i;
  return o.str();
}
  
void parse::print_syntax( const char *error_string )
{
  cout << endl << "Error: " << error_string << endl;
  cout << endl << "Average Link Based Refining" << endl
	   <<     "---------------------------" << endl
	   << "S. Martin" << endl
	   << "Version " << VERSION << endl << endl
	   << "This program provides refining of a sim file based on" << endl
       << "the average link clustering of the results of layout." << endl << endl
	   << "Usage: refine -l {int>=2} [options] root_file" << endl << endl
	   << "root_file -- the root name of the files to be input and output." << endl << endl
	   << "INPUT FILES:" << endl << endl
	   << "root_file_l.icoord is the icoord file in the format produced" << endl
       << "  by the layout command." << endl
	   //<< "\t# nodes <tab> # edges" << endl
       << "\tint_id <tab> int_id <tab> weight" << endl
       //<< "\t..." << endl
	   << "  where int_id's are sequential integers, starting at 0," << endl
       << "  and weight is > 0." << endl << endl
	   << "root_file_(l-1).clust is the output file from the average link" << endl
       << "  clustering algorithm of the form" << endl
	   << "\tint_id <tab> clust_id <tab> importance" << endl << endl
       << "OUTPUT FILES:" << endl << endl
       << "root_file_(l-1).real is output by this program and contains" << endl
       << "  the new coord file for input to layout, with the same format" << endl
       << "  original .coord file." << endl << endl
	   << "OPTIONS:" << endl << endl
       << "\t-l {int>=2} current coarsening level (not optional)" << endl
       << "\t            if 2 then reads from root_file.icoord and .clust" << endl
	   << "\t-s {float} scale coordinates so they fit in a (2*float)^2 box" << endl
       << "\t-r convert _(l-1).coarse_int to _(l-1).refine_int" << endl << endl;
		   
    exit(1);
}

parse::parse ( int argc, char** argv)
{
  map<string,string> m;

  // make sure there are at least two arguments and -l flag 
  if ( argc < 4)
	print_syntax ( "not enough arguments!" );
  
  // all input/output files use same root
  blob_file = clust_file = real_file = coarse_file = refine_file = argv[argc-1];
  
  // set defaults
  level = -1;			// (check for level input -- mandatory)
  scale = 0;			// 0 means do not scale
  refine_int = false;	// do not ouput .refine_int
  
  // now check for optional arguments
  string arg;
  for( int i = 1; i<argc-1; i++ )
  {
	arg = argv[i];

	// check for coarsening level
    if ( arg == "-l" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-l flag has no argument." );
		else
		{
			level = atoi ( argv[i] );
			if ( level <= 1 )
				print_syntax ( "coarsen level must be > 1." );
		}
	}
	
	else if ( arg == "-s" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ("-s flag has no argument.");
		else
		{
			scale = atof ( argv[i] );
			if ( scale <= 0.0 )
				print_syntax ( "scale value must be positive." );
		}
	
	}
	
	else if ( arg == "-r" )
		refine_int = true;
	  
    else
        print_syntax ( "unrecongized option!" );
  }
  
  if ( level < 0 )
    print_syntax ( "coarsen level is not optional." );
    
  // add appropriate level extensions
  if ( level > 2 )
  {
    clust_file = clust_file + "_" + itoa ( level-1 );
    real_file = real_file + "_" + itoa ( level-1 );
	refine_file = refine_file + "_" + itoa ( level-1 );
	coarse_file = coarse_file + "_" + itoa ( level-1 );
  }
  blob_file = blob_file + "_" + itoa ( level );
  
  // add approriate file suffixes
  blob_file = blob_file + ".icoord";
  clust_file = clust_file + ".clust";
  real_file = real_file + ".real";
  coarse_file = coarse_file + ".coarse_int";
  refine_file = refine_file + ".refine_int";
  
  // echo sim_file and coord_file
  cout << "Using " << blob_file << " for .icoord file, " << endl
       << "      " << clust_file << " for .clust file. " << endl
       << "Will output " << real_file << " for refined .real file." << endl;
       
  if ( refine_int )
	 cout << "Using " << coarse_file << " for .coarse_int file, and" << endl
	      << "      " << refine_file << " for .refine_int file." << endl;
		  
  // echo arguments input or default
  cout << "Using level = " << level << endl
	   << "      scale = " << scale << " (0 means do not scale)" << endl;

}
