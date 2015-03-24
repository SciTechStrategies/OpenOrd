// "Smart" Refining for similarity graph based on results of average
// link clustering and layout layout.
//
// This program performs the refine step for the recursive layout,
// opposite the coarsen.cpp program
//
// The structure of the inputs and outputs of this code will be displayed
// if the program is called without parameters, or if an erroneous
// parameter is passed to the program.
//
// S. Martin
// 5/3/2006

// C++ library routines
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <deque>
#include <vector>
#include <cstdlib>

using namespace std;

// layout routines and constants
#include <refine_parse.h>

// The following routine reads in the .clust file and records the
// cluster membership and size information for future use.

void read_clust ( string clust_file, map <int, set<int> > &cluster_membership,
                  map <int, int> &cluster_sizes, int &min_clust, int &max_clust,
                  int &num_nodes, int &num_clusts )
{
  cout << "Reading .clust file ..." << endl;
  
  ifstream clust_in ( clust_file.c_str() );
  if ( !clust_in )
  {
    cout << "Error: could not open " << clust_file << ".  Program terminated." << endl;
    exit(1);
  }

  num_clusts = num_nodes = -1;
  int int_id, clust_id, importance;
  int lines_read = 0;
  while ( !clust_in.eof() )
  {
    int_id = -1;
    clust_in >> int_id >> clust_id >> importance;
    if ( int_id != -1 )    // check that line is not empty
    {
      lines_read++;
      cluster_membership[clust_id-1].insert(int_id);
      if ( cluster_sizes.find (clust_id-1) == cluster_sizes.end() )
        cluster_sizes[clust_id-1] = 0;
      cluster_sizes[clust_id-1] = cluster_sizes[clust_id-1] + 1;
      if ( clust_id > num_clusts ) num_clusts = clust_id;
      if ( int_id > num_nodes ) num_nodes = int_id;
    }  
  }
    
  clust_in.close ();
  num_nodes; 
  
  // check that cluster ids go from 0 to #clusters-1 with no gaps (for layout)
  for ( int i = 0; i < num_clusts; i ++ )
    if ( cluster_sizes.find ( i ) == cluster_sizes.end() )
    {
        cout << "Error: cluster ids are not sequential at " << i <<"." << endl;
        exit(1);
    }
  
  cout << "Read " << lines_read << " lines, with " << num_nodes << " nodes and " << num_clusts
       << " clusters." << endl;
       
  // compute max and min cluster sizes
  min_clust = max_clust = cluster_sizes[0];
  for ( int i = 0; i < num_clusts; i++ )
  {
    if ( min_clust > cluster_sizes[i] ) min_clust = cluster_sizes[i];
    if ( max_clust < cluster_sizes[i] ) max_clust = cluster_sizes[i];
  }
  
  cout << "Maximum cluster size " << max_clust << ", minimum cluster size "
       << min_clust << "." << endl;
}

// The next function transforms the blob file to a real file
void create_real ( string blob_file, string real_file,
				   map < int, set<int> > clusters,
				   set < int > &id_catalog,
				   float scale, float x_scale, float y_scale )
{
  cout << "Reading .icoord file ..." << endl;
  
  ifstream blob_in ( blob_file.c_str() );
  if ( !blob_in )
  {
    cout << "Error: could not open " << blob_file << ".  Program terminated." << endl;
    exit(1);
  }
  
  ofstream real_out ( real_file.c_str() );
  if ( !real_out )
  {
	cout << "Error: could not open " << real_file << ".  Program terminated." << endl;
	exit(1);
  }

  cout << "Writing .real file ..." << endl;
  int int_id;
  float x_coord, y_coord;
  set<int>::iterator clust_iter;
  while ( !blob_in.eof() )
  {
    int_id = -1;
    blob_in >> int_id >> x_coord >> y_coord;
    if ( int_id != -1 )    // check that line is not empty
	    for ( clust_iter = clusters[int_id].begin();
			  clust_iter != clusters[int_id].end();
			  clust_iter++ )
		{
			id_catalog.insert ( *clust_iter );
			if ( scale > 0.0 )
				real_out << *clust_iter << "\t" << x_coord*scale/x_scale
				         << "\t" << y_coord*scale/y_scale << endl;
			else
				real_out << *clust_iter << "\t" << x_coord
				         << "\t" << y_coord << endl;
        }  
  }

  blob_in.close();
  real_out.close();
  
}

// This function scans through the .icoord file to find the largest magnitude
// (in absolute value) x and y values, so that the output file may be scaled
// up or down according to the scale parameter

void get_scales ( string coord_file, float &x_scale, float &y_scale )
{
  cout << "Finding max and min x and y values for scaling ..." << endl;
  
  ifstream blob_in ( coord_file.c_str() );
  if ( !blob_in )
  {
    cout << "Error: could not open " << coord_file << ".  Program terminated." << endl;
    exit(1);
  }

  float max_x, max_y, min_x, min_y;
  max_x = max_y = -1.0;
  min_x = min_y = 1.0;
  float x_coord, y_coord;
  int int_id;
  
  while ( !blob_in.eof() )
  {
    int_id = -1;
    blob_in >> int_id >> x_coord >> y_coord;
    if ( int_id != -1 )    // check that line is not empty
    {
		if ( x_coord > max_x ) max_x = x_coord;
		if ( x_coord < min_x ) min_x = x_coord;
		if ( y_coord > max_y ) max_y = y_coord;
		if ( y_coord < min_y ) min_y = y_coord;
    }  
  }
  
  blob_in.close();

  if ( max_x > -min_x ) x_scale = max_x; else x_scale = -min_x;
  if ( max_y > -min_y ) y_scale = max_y; else y_scale = -min_y;
  
  //cout << "Found " << x_scale << " for x-coordinate scale, and " << y_scale
  //     << " for y-coordinate scale." << endl;  
	   
  // preserve x/y aspect ratio
  if ( x_scale > y_scale ) y_scale = x_scale; else x_scale = y_scale;

  cout << "Using " << x_scale << " as scale factor." << endl;
}

void create_int ( string coarse_file, string refine_file,
				  set < int > &id_catalog )
{
  cout << "Converting .coarse_int to .refine_int ..." << endl;

  ifstream coarse_in ( coarse_file.c_str() );
  if ( !coarse_in )
  {
    cout << "Error: could not open " << coarse_file << ".  Program terminated." << endl;
    exit(1);
  }
  
  ofstream refine_out ( refine_file.c_str() );
  if ( !refine_out )
  {
	cout << "Error: could not open " << refine_file << ".  Program stopped." << endl;
	exit(1);
  }
  
  int id1, id2;
  float edge_weight;
  while ( !coarse_in.eof () )
  {
	 id1 = -1;
	 coarse_in >> id1 >> id2 >> edge_weight;
	 if ( id1 >= 0 )
	 {
		if ( (id_catalog.find(id1) != id_catalog.end()) &&
			 (id_catalog.find(id2) != id_catalog.end()) )
			 refine_out << id1 << "\t" << id2 << "\t" << edge_weight << endl;
	 }
  }
  
  coarse_in.close();
  refine_out.close();
  
}

int main(int argc, char **argv)
{	
    // get user input
    parse command_line ( argc, argv );
	
    // read .clust file
    map <int, set<int> > cluster_membership;
    map <int, int> cluster_sizes;
    int min_clust, max_clust;
    int num_nodes, num_clusts; 
    read_clust ( command_line.clust_file, cluster_membership, cluster_sizes,
                 min_clust, max_clust, num_nodes, num_clusts );

	/*
    // output clusters (for debugging)
    map <int, set<int> >::iterator cm_iter;
	set<int>::iterator clust_iter;
    for ( cm_iter = cluster_membership.begin();
          cm_iter != cluster_membership.end();
          cm_iter++ )
		  {
		    cout << cm_iter->first << ": ";
			for ( clust_iter = (cm_iter->second).begin();
				  clust_iter != (cm_iter->second).end();
				  clust_iter++ )
				cout << *clust_iter << " ";
			cout << endl;
		  }
	*/	  
	
	// check if user wants us to scale the data
	float x_scale, y_scale;
	if ( command_line.scale > 0 )
	  get_scales ( command_line.blob_file, x_scale, y_scale );
	   
	// next we read the .blob file and output the .real file
	set < int > id_catalog;
	create_real ( command_line.blob_file, command_line.real_file,
				  cluster_membership, id_catalog,
				  command_line.scale, x_scale, y_scale );
	
	/*
	// print out id_catalog (for debugging)
	set <int>::iterator cat_iter;
	for ( cat_iter = id_catalog.begin(); cat_iter != id_catalog.end(); cat_iter++ )
	  cout << *cat_iter << endl;
	*/
	
	// do we need to convert a .coarse_int file?
	if ( command_line.refine_int )
	  create_int ( command_line.coarse_file, command_line.refine_file,
				   id_catalog );
				   
    cout << "Program finished successfully." << endl;
	
}


