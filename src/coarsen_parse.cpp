// This file contains the methods for the parse.h class

#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <cstdlib>

using namespace std;

#include <coarsen_parse.h>

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
  cout << endl << "Average Link Based Coarsening" << endl
	   <<     "-----------------------------" << endl
	   << "S. Martin" << endl
	   << "Version " << VERSION << endl << endl
	   << "This program provides coarsening of a sim file based on" << endl
       << "the average link clustering of the results of layout." << endl << endl
	   << "Usage: coarsen -l {int>=2} [options] root_file" << endl << endl
	   << "root_file -- the root name of the files to be input and output." << endl << endl
	   << "INPUT FILES:" << endl << endl
	   << "root_file_(l-1).full is the full file in the format produced" << endl
       << "  by the truncate command." << endl
	   //<< "\t# nodes <tab> # edges" << endl
       << "\tint_id <tab> int_id <tab> weight" << endl
       //<< "\t..." << endl
	   << "  where int_id's are sequential integers, starting at 0," << endl
       << "  and weight is > 0." << endl << endl
	   << "root_file_(l-1).clust is the output file from the average link" << endl
       << "  clustering algorithm of the form" << endl
	   << "\tint_id <tab> clust_id <tab> importance" << endl << endl
       << "OUTPUT FILES:" << endl << endl
       << "root_file_l.full is output by this program and contains" << endl
       << "  the new similarity matrix, with the same format as the" << endl
       << "  original .full file." << endl << endl
       << "root_file_l.int is contains the truncated version of" << endl
       << "  the .full file for use by layout." << endl << endl
	   << "OPTIONS:" << endl << endl
       << "\t-l {int>=2} current coarsening level (not optional)" << endl
       << "\t            if 2 then reads from root_file.full and .clust" << endl
       << "\t-t {int>=1} {int>=1} use top t1 to t2 links in .int file," << endl
       << "\t                     default 5 to 15, can use n to n for" << endl
       << "\t                     top n links." << endl
       << "\t-m {int>=1} scans the file m times for memory conservation" << endl
       << "\t            (default 1)" << endl 
       << "\t-n produces normalized similarities in .int" << endl << endl;
           
    exit(1);
}

parse::parse ( int argc, char** argv)
{
  map<string,string> m;

  // make sure there are at least two arguments and -l flag 
  if ( argc < 4)
	print_syntax ( "not enough arguments!" );
  
  // all input/output files use same root
  full_file = clust_file = full_out_file = int_out_file = argv[argc-1];
  
  // set defaults
  level = -1;    // (check for level input -- mandatory)
  top_n_links[0] = 5;
  top_n_links[1] = 15;
  normalized_output = false;
  memory_use = 1;
  
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
	
	// check for memory conservation
    else if ( arg == "-m" )
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
		if ( i >= (argc-2) )
			print_syntax ( "-t flag needs two arguments." );
		else
		{
			top_n_links[0] = atoi ( argv[i] );
			i++;
			top_n_links[1] = atoi ( argv[i] );
			if ( top_n_links[0] <= 0 )
				print_syntax ( "number of sim links must be >= 1." );
			if ( top_n_links[0] > top_n_links[1] )
			    print_syntax ( "number of sim links must increase." );
		}
	}
	
    // check for normalized output
    else if ( arg == "-n" )
	    normalized_output = true;
    else
        print_syntax ( "unrecongized option!" );
  }
  
  if ( level < 0 )
    print_syntax ( "coarsen level is not optional." );
    
  // add appropriate level extensions
  if ( level > 2 )
  {
    full_file = full_file + "_" + itoa ( level-1 );
    clust_file = clust_file + "_"+ itoa ( level-1 );
  }
  full_out_file = full_out_file + "_" + itoa ( level );
  int_out_file = int_out_file + "_" + itoa ( level );
  
  // add approriate file suffixes
  full_file = full_file + ".full";
  clust_file = clust_file + ".clust";
  full_out_file = full_out_file + ".full";
  int_out_file = int_out_file + ".int";
  
  // echo sim_file and coord_file
  cout << "Using " << full_file << " for .full file, " << endl
       << "      " << clust_file << " for .clust file. " << endl
       << "Will output " << full_out_file << " for coarsened .full file," << endl
       << "            " << int_out_file << " for .int file." << endl;
       
  // echo arguments input or default
  cout << "Using level = " << level << endl
       << "      scan file times = " << memory_use << endl
       << "      number of sim links to output = " << top_n_links[0]
       << " to " << top_n_links[1] << endl
       << "      normalized output = " << normalized_output << endl;

}
