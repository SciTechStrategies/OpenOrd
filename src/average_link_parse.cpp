// This file contains the methods for the parse.h class

#include <string>
#include <iostream>
#include <map>
#include <cstdlib>

using namespace std;

#include <average_link.h>
#include <average_link_parse.h>

void parse::print_syntax( const char *error_string )
{
  cout << endl << "Error: " << error_string << endl;
  cout << endl << "Average Link Clustering" << endl
	   <<     "----------------------" << endl
	   << "S. Martin" << endl
	   << "Version " << VERSION << endl << endl
	   << "This program provides average link clustering of the results" << endl
	   << "of layout." << endl << endl
	   << "Usage: average_link [options] root_file" << endl << endl
	   << "root_file -- the root name of the files to be input and output." << endl << endl
	   << "INPUT FILES:" << endl << endl
	   << "root_file.full is the full sim file (output by truncate)" << endl
       << "  with the form" << endl
	   << "\tnode_id <tab> node_id <tab> weight" << endl
	   << "  where node_id is a string, and weight is > 0." << endl << endl
	   << "root_file.icoord is the output file from layout of the form" << endl
	   << "\tnode_id <tab> x-coord <tab> y-coord" << endl << endl
       << "root_file.iedges can also be output by layout, and contains" << endl
       << "  the edges remaining at the end of the ordination.  This" << endl
       << "  file should have the same format as root_file.full." << endl 
       << "  (Except spaces can be substituted for tabs.)" << endl << endl
       << "OUTPUT FILES:" << endl << endl
       << "root_file.clust is output by this program and contains" << endl
       << "  the average link clustering, with the format" << endl
       << "\tnode_id <tab> cluster_num <tab> importance" << endl << endl
       << "root_file.mindist is output if the -d flag is present." << endl
       << "  This file contains the distances for computing the threshold" << endl
       << "  value, one per line." << endl << endl
       << "root_file.clustin is output if the -d flag is present." << endl
       << "  This file contains the input the the clustering algorithm" << endl
       << "  in the format (tab delimited)" << endl
       << "\tnode_id1  node_id2  distance  x1  y1  x2  y2" << endl << endl
	   << "OPTIONS:" << endl << endl
       << "\t-t {real>0} threshold value for distance curve" << endl
       << "\t            (default is automatic selection)" << endl
       << "\t-s {int>=0} number of shortest links to add (default is 1)" << endl 
       << "\t-d output .mindist & .clustin files (for debugging)" << endl
       << "\t-n {int>0} neighborhood radius for estimating derivatives" << endl
       << "\t           in automatic threshold selection (default 10)" << endl << endl;
           
    exit(1);
}

parse::parse ( int argc, char** argv)
{
  map<string,string> m;

  // make sure there are at least two arguments  
  if ( argc < 2)
	print_syntax ( "not enough arguments!" );
  
  coord_file = sim_file = edges_file = slc_file = dist_file = clustin_file = argv[argc-1];
  coord_file = coord_file + ".icoord";
  sim_file = sim_file + ".full";
  edges_file = edges_file + ".iedges";
  slc_file = slc_file + ".clust";
  dist_file = dist_file + ".mindist";
  clustin_file = clustin_file + ".clustin";
  
  // echo sim_file and coord_file
  cout << "Using " << sim_file << " for full file, " << endl
       << "      " << coord_file << " for icoord file, and " << endl
       << "      " << edges_file << " for edges file." << endl;
  
  // set defaults
  threshold = 0;
  num_short_links = 1;
  output_dist = false;
  neighborhood_size = 10;
  
  // now check for optional arguments
  string arg;
  for( int i = 1; i<argc-1; i++ )
  {
	arg = argv[i];

	// check for threshold
    if ( arg == "-t" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-t flag has no argument." );
		else
		{
			threshold = atof ( argv[i] );
			if ( threshold <= 0 )
				print_syntax ( "threshold must be > 0." );
		}
	}
	// check for short links
	else if ( arg == "-s" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-s flag has no argument." );
		else
		{
			num_short_links = atoi ( argv[i] );
			if ( num_short_links < 0 )
				print_syntax ( "number of short links must be >= 0." );
		}
	}
	// check for neighborhood size
	else if ( arg == "-n" )
	{
		i++;
		if ( i >= (argc-1) )
			print_syntax ( "-n flag has no argument." );
		else
		{
			neighborhood_size = atoi ( argv[i] );
			if ( neighborhood_size <= 0 )
				print_syntax ( "neighborhood size must be > 0." );
		}
	}
    // check for .dist option
    else if ( arg == "-d" )
	    output_dist = true;
    
    else
        print_syntax ( "unrecongized option!" );
  }
  
  // echo arguments input or default
  cout << "Using threshold = " << threshold << " (0 indicates automatic selection)" << endl
       << "      number of shortest links = " << num_short_links << endl
       << "      output debug files = " << output_dist << endl
       << "      neighborhood size = " << neighborhood_size << endl;

}
