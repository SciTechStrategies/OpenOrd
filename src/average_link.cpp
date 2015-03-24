// Average Link Clustering of Layout output
//
// This program performs average link clustering on the results of layout.
// The algorithm is based on using both the underlying graph structure as
// well as the coordinates provided by layout.
//
// The structure of the inputs and outputs of this code will be displayed
// if the program is called without parameters, or if an erroneous
// parameter is passed to the program.
//
// S. Martin
// 10/27/2005

// C++ library routines
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

// layout routines and constants
#include <average_link.h>
#include <average_link_parse.h>
#include <average_link_clust.h>


// The following subroutine read and stores the information
// for the .coord file in a format for easy
// lookup of string (id_catalog)

void read_coord  ( string coord_file, map <string, int_node> &id_catalog )
{

  cout << "Reading .icoord file ..." << endl;
  
  ifstream coord_in ( coord_file.c_str() );
  if ( !coord_in )
  {
    cout << "Error: could not open " << coord_file << ".  Program terminated." << endl;
    exit(1);
  }
  
  string id;
  float coord_x, coord_y;
  while ( !coord_in.eof() )
  {
    id = "";
    coord_in >> id >> coord_x >> coord_y;
    if ( id != "" )    // check that line is not empty
      if ( id_catalog.find ( id ) == id_catalog.end() )
      {
        id_catalog[id].x = coord_x;
        id_catalog[id].y = coord_y;
      }
      else
      {
        cout << "Error: duplicate coordinate entries found!  Program terminated." << endl;
        exit(1);
      }
  }
  
  coord_in.close ();
  
  // go through and number ids alphabetically
  map <string, int_node>::iterator cat_iter;
  int int_id;
  for ( cat_iter = id_catalog.begin(), int_id = 0;
        cat_iter != id_catalog.end(); cat_iter++, int_id++ )
    cat_iter->second.id = int_id;
  
  cout << "Read " << id_catalog.size() << " nodes." << endl; 
}

// The next subroutine reads the .edges file to create an
// upper triangular adjacency matrix.  It then reads .sim
// and merges the top similarities (as passed in num_short_links)
// into the adjacency matrix.

void read_edges_sim ( string edges_file, string sim_file, int num_short_links,
                      map <string, int_node> &id_catalog,
                      map <int, map<int, float> > &adj_mat,
                      vector <float> &min_sim )
{

  // first we read the .edges file into the adjacency matrix
  // -------------------------------------------------------
  
  cout << "Reading edges file ... " << endl;
  
  // modification of Brian's original code
  //char node1_buf[200], node2_buf[200], sim_buf[500]; 
  float edge_weight, dist;
  //FILE *fp;
  //int ret;

  // Open (edges) File
  ifstream edges_in ( edges_file.c_str() );
  if ( !edges_in )
  {
	cout << "Error: could not open " << edges_file << ".  Program terminated." << endl;
	exit(1);
  }	
  // read in dummy #nodes, #edges
  //int dummy_num_nodes, dummy_num_edges;
  //edges_in >> dummy_num_nodes >> dummy_num_edges;
  
  size_t pos1, pos2;					// variables for parsing
  string sim, id1, id2;
 
  // Read file, parse, and add into data structure
  int line_count = 0;
  //while ((ret=fscanf(fp,"%[^\t]\t%[^\t]\t%f\n",node1_buf,node2_buf,&edge_weight)) && (ret!=EOF))
  while ( !edges_in.eof() )
	{
	  edge_weight = -1.0;
	  edges_in >> id1 >> id2 >> edge_weight;
	  
	  /* old code won't read spaces:
      sprintf(sim_buf,"%s\t%s\t%f\t",node1_buf,node2_buf,edge_weight);
	  sim = sim_buf;
      
      // Parse objects and sim
      pos1 = 0;
      pos2 = sim.find('\t',pos1);
      id1 = sim.substr(pos1,pos2-pos1);
      pos1 = pos2+1;
      pos2 = sim.find('\t',pos1);
      id2 =  sim.substr(pos1,pos2-pos1);
      pos1 = pos2+1;
      pos2 = sim.find('\t',pos1);
      edge_weight = atof(sim.substr(pos1,pos2-pos1).c_str());
      */
      

	  
	  // ignore negative weights!
	  if ( edge_weight > 0 )
	  {	  
         // count line
	     line_count++;
	     
        // populate upper triangular portion of adj. matrix
        if ( (id_catalog.find(id1) != id_catalog.end ()) &&
             (id_catalog.find(id2) != id_catalog.end ()) )
        {
            // compute distance between id1 and id2
            dist = sqrt ( pow((id_catalog[id1].x - id_catalog[id2].x),2) +
                          pow((id_catalog[id1].y - id_catalog[id2].y),2) );
            if ( id_catalog[id1].id < id_catalog[id2].id )
              adj_mat[id_catalog[id1].id][id_catalog[id2].id] = dist;
            else
              adj_mat[id_catalog[id2].id][id_catalog[id1].id] = dist;                 
        }
        else 
        {
           cout << "Error: found identifiers not present in .coord file.  Program Stopped." << endl;
           exit(1);
        }
	  }
	}

  edges_in.close();
  
  // count number of edges so far
  map <int, map<int, float> >::iterator row_iter;
  map <int, float>::iterator col_iter;
  int num_edges;
  for ( row_iter = adj_mat.begin(), num_edges = 0;
        row_iter != adj_mat.end(); row_iter++ )
    for ( col_iter = row_iter->second.begin();
          col_iter != row_iter->second.end();
          col_iter++, num_edges++ );
  cout << "Read " << line_count << " lines and " << num_edges << " edges." << endl;

  // next we read & store the top num_short_links edges from the .sim file
  // ---------------------------------------------------------------------
  
  map <int, map<int, float> > sim_adj;
  int int_id1, int_id2;
  
  cout << "Reading .full file ..." << endl;
  
  // Open (sim) File
  ifstream full_file ( sim_file.c_str() );
  if ( !full_file )
  {
	cout << "Error: could not open " << sim_file << ".  Program terminated." << endl;
	exit(1);
  }	
  // skip first line
  //full_file >> dummy_num_nodes >> dummy_num_edges;
    
  // init min_sim structure for keeping track of the minimum distance similarity
  for (int min_sim_i = 0; min_sim_i < id_catalog.size(); min_sim_i++ )
    min_sim[min_sim_i] = 0.0;
    
  // Read file, parse, and add into data structure
  line_count = 0;
  while ( !full_file.eof() )
  //while ((ret=fscanf(fp,"%[^\t]\t%[^\t]\t%f\n",node1_buf,node2_buf,&edge_weight)) && (ret!=EOF))
	{
      edge_weight = -1.0;
	  full_file >> id1 >> id2 >> edge_weight;

	  // count line
	  line_count++;
	  
	  // ignore negative weights!
	  if ( edge_weight > 0 )
	  {
        // populate upper triangular portion of adj. matrix
        if ( (id_catalog.find(id1) != id_catalog.end ()) &&
             (id_catalog.find(id2) != id_catalog.end ()) )
        {
            // compute distance between id1 and id2
            dist = sqrt ( pow((id_catalog[id1].x - id_catalog[id2].x),2) +
                          pow((id_catalog[id1].y - id_catalog[id2].y),2) );
                          
            // save integer versions of id1,id2 for quick reference
            int_id1 = id_catalog[id1].id;
            int_id2 = id_catalog[id2].id;
            
            // keep track of minimum dist sim for each node
            if ( (min_sim[int_id1] == 0.0) || (min_sim[int_id1] > dist) )
                min_sim[int_id1] = dist;

            if ( (min_sim[int_id2] == 0.0) || (min_sim[int_id2] > dist) )
                min_sim[int_id2] = dist;

            // compute number of ids in sim matrix row and maximum distance
            int num_in_row, max_ind;
            float max_dist;
            for ( col_iter = sim_adj[int_id1].begin(), num_in_row = 0, max_dist = -1.0;
                  col_iter != sim_adj[int_id1].end(); num_in_row++, col_iter++ )
              if ( col_iter->second > max_dist )
              {
                 max_dist = col_iter->second;
                 max_ind = col_iter->first;
              }
              
            // add or reject new edge based on distance
            // and number of entries present
            if ( num_in_row < num_short_links )
              sim_adj[int_id1][int_id2] = dist;
            else if ( dist < max_dist )
            {
              // replace old shortest distance by new shortest distance
              sim_adj[int_id1][int_id2] = dist;
              sim_adj[int_id1].erase( max_ind );
            }  
            
            // now do same thing for id2
            for ( col_iter = sim_adj[int_id2].begin(), num_in_row = 0, max_dist = -1.0;
                  col_iter != sim_adj[int_id2].end(); num_in_row++, col_iter++ )
              if ( col_iter->second > max_dist )
              {
                 max_dist = col_iter->second;
                 max_ind = col_iter->first;
              }
              
            // add or reject new edge based on distance
            // and number of entries present
            if ( num_in_row < num_short_links )
              sim_adj[int_id2][int_id1] = dist;
            else if ( dist < max_dist )
            {
              // replace old shortest distance by new shortest distance
              sim_adj[int_id2][int_id1] = dist;
              sim_adj[int_id2].erase( max_ind );
            }           
        }
        else 
        {
           cout << "Error: found identifiers not present in .coord file.  Program Stopped." << endl;
           exit(1);
        }
	  }
	}

  full_file.close();

  // sort minimum sim distances
  sort ( min_sim.begin(), min_sim.end() );
    
  // count number of edges in final graph for user
  for ( row_iter = sim_adj.begin(), num_edges = 0;
        row_iter != sim_adj.end(); row_iter++ )
    for ( col_iter = row_iter->second.begin();
          col_iter != row_iter->second.end();
          col_iter++, num_edges++ );
          
  cout << "Read " << line_count << " lines, using " << num_edges << " edges." << endl;
  
  /*
  // output .sim information (for debugging)
  map <int, map<int, float> >::iterator row_iter;
  map <int, float>::iterator col_iter;
  for ( row_iter = sim_adj.begin(); row_iter != sim_adj.end(); row_iter++ )
    for ( col_iter = row_iter->second.begin();
          col_iter != row_iter->second.end();
          col_iter++ )
          cout << row_iter->first << " "
               << col_iter->first << " "
               << col_iter->second << endl;
  */
  
  cout << "Merging minimal .full edges into .iedges graph ..." << endl;
  
  // Finally, we merge the .sim information into the .edges matrix
  // -------------------------------------------------------------
  for ( row_iter = sim_adj.begin(); row_iter != sim_adj.end(); row_iter++ )
    for ( col_iter = row_iter->second.begin();
          col_iter != row_iter->second.end();
          col_iter++ )
    {
        int_id1 = row_iter->first;
        int_id2 = col_iter->first;
        dist = col_iter->second;
        if ( int_id1 < int_id2 )
          adj_mat[int_id1][int_id2] = dist;
        else
          adj_mat[int_id2][int_id1] = dist;    
    }
  
  // count number of edges in final graph for user
  for ( row_iter = adj_mat.begin(), num_edges = 0;
        row_iter != adj_mat.end(); row_iter++ )
    for ( col_iter = row_iter->second.begin();
          col_iter != row_iter->second.end();
          col_iter++, num_edges++ );
          
  cout << "Total of " << num_edges << " edges in graph." << endl;
  
}

// The select_threshold routine automatically selects a distance
// threshold for use by the average link clustering algorithm.  This
// threshold is found by going backwards from the largest distance
// and locating the first incidence of slope = 45 degrees on
// and normalized rank vs. normalized distance plot.
// num_neighbors gives the number of neighbors to use when smoothing
// the curve.

float select_threshold ( vector <float> &min_sim,
                         int num_neighbors )
{
    cout << "Automatically selecting threshold ..." << endl;
    
    // first we find the maximum distance and rank
    // for use in normalization
    float max_dist;

    int min_sim_i;
        
    for ( min_sim_i = 0, max_dist = 0.0; min_sim_i < min_sim.size(); min_sim_i++ )
          if ( min_sim[min_sim_i] > max_dist )
               max_dist = min_sim[min_sim_i];
          
    int num_dist;
    num_dist = min_sim.size();
    
    // go backwards to first node in the middle of the neighborhood
    int nhood;
    min_sim_i = min_sim.size();
    min_sim_i--;    // last actual distance
    for ( nhood = 0; (nhood < num_neighbors) && (min_sim_i > 0); nhood++ )
        min_sim_i--;
    
    // go backwards down the normalized curve computing slopes
    // to select the threshold
    float uphill, downhill, slope, threshold;
    bool found_threshold = false;
    while ( min_sim_i > 0 )
    {
        // go uphill to get highest value
        for ( nhood = 0; (nhood < num_neighbors) && (min_sim_i < min_sim.size()); nhood++ )
            min_sim_i++;
            
        // test to see if we hit the top
        if ( min_sim_i < min_sim.size() )
           uphill = min_sim[min_sim_i];
        else
        {
           cout << "Error: automatic threshold identification failed.  Program stopped." << endl;
           exit(1);
        }
        
        // go downhill to get lowest value
        for ( nhood = 0; (nhood < 2*num_neighbors) && (min_sim_i > 0); nhood++ )
            min_sim_i--;
            
        // test to see if we hit the bottom
        if ( min_sim_i > 0 )
           downhill = min_sim[min_sim_i];
        else
        {
           cout << "Error: automatic threshold identification failed.  Program stopped." << endl;
           exit(1);
        }
        
        // compute slope using these values
        slope = ( (float)num_dist/(max_dist*(float)(2*nhood)) )*
                ( uphill - downhill ); 

        // go back to initial position (this should be safe)
        for ( nhood =0; nhood < num_neighbors; nhood++ )
            min_sim_i++;
        
        // stop when slope ~ .5
        if ( slope >= .5 )
           threshold = min_sim[min_sim_i];
        else
        {
           found_threshold = true;
           break;
        }
           
        // decrement for next pass
        min_sim_i--;
    }
    
    if ( !found_threshold )
    {
       cout << "Error: automatic threshold identification failed.  Program stopped." << endl;
       exit(1);
    }
    
    cout << "Found threshold of " << threshold << "." << endl;
    
    return threshold;
}

int main(int argc, char **argv)
{	
    // get user input
    parse command_line ( argc, argv );
    
    // read in .coord file
    map <string, int_node> id_catalog;  // map for fast lookup of strings
    read_coord ( command_line.coord_file, id_catalog );
    
    /*
    // ouptut id_catalog for debugging:
    map <string, int_node>::iterator id_iter;
    for ( id_iter = id_catalog.begin(); id_iter != id_catalog.end(); id_iter++ )
        cout << id_iter->first << " " << id_iter->second.id << " " << id_iter->second.x
                 << " " << id_iter->second.y << endl;
    */
    
    // next populate graph using .sim and .edges file
    map <int, map<int, float> > adj_mat;    // sparse adj. matrix for graph
                                            // indexed by integer .id in id_catalog
    vector <float> min_sim ( id_catalog.size() );  // minimum distance in sim file
                                                   // (for threshold selection)
    read_edges_sim ( command_line.edges_file, command_line.sim_file, 
                     command_line.num_short_links, id_catalog, adj_mat, min_sim );
    
    
    /*
    // output graph (for debugging):
    map <int, map<int, float> >::iterator out_row_iter;
    map <int, float>::iterator out_col_iter;
    for ( out_row_iter = adj_mat.begin(); out_row_iter != adj_mat.end(); out_row_iter++ )
      for ( out_col_iter = out_row_iter->second.begin();
            out_col_iter != out_row_iter->second.end();
            out_col_iter++ )
            cout << out_row_iter->first << " "
                 << out_col_iter->first << " "
                 << out_col_iter->second << endl;
    */
    
    // re-index by integer node id
    map <int, node> node_info;
    map <string, int_node>::iterator id_cat_iter, temp_cat_iter;
    for ( id_cat_iter = id_catalog.begin(); id_cat_iter != id_catalog.end(); )
    {
      node_info[id_cat_iter->second.id].id = id_cat_iter->first;
      node_info[id_cat_iter->second.id].x = id_cat_iter->second.x;
      node_info[id_cat_iter->second.id].y = id_cat_iter->second.y;
      temp_cat_iter = id_cat_iter;
      id_cat_iter++;
      id_catalog.erase ( temp_cat_iter );
    }
    
    /*
    // ouptut re-indexed info for debugging:
    map <int, node>::iterator info_iter;
    for ( info_iter = node_info.begin(); info_iter != node_info.end(); info_iter++ )
        cout << info_iter->first << " " << info_iter->second.id << " " << info_iter->second.x
                 << " " << info_iter->second.y << endl;
    */
    
    // sort graph by ascending value of distance
    cout << "Sorting graph by distance ..." << endl;
    id_pair row_col;
    multimap < float, id_pair > sorted_adj;
    map <int, map<int, float> >::iterator row_iter, temp_row_iter;
    map <int, float>::iterator col_iter, temp_col_iter;
    for ( row_iter = adj_mat.begin(); row_iter != adj_mat.end(); )
    {
      for ( col_iter = row_iter->second.begin();
            col_iter != row_iter->second.end(); )
      {
        row_col.row = row_iter->first;
        row_col.col = col_iter->first;
        sorted_adj.insert ( pair<float, id_pair> (col_iter->second, row_col) );
        temp_col_iter = col_iter;
        col_iter++;
        row_iter->second.erase ( temp_col_iter );
      }
      temp_row_iter = row_iter;
      row_iter++;
      adj_mat.erase ( temp_row_iter );
    }
      
    // should we output the debugging files?
    if ( command_line.output_dist )
    {
          
      cout << "Writing out the .clustin file ..." << endl;
      ofstream dist_out ( command_line.clustin_file.c_str() );
      if ( !dist_out )
      {
         cout << "Error: could not open .clustin file.  Program stopped." << endl;
         exit(1);
      }
      
      int int_id1, int_id2;
      multimap <float, id_pair>::iterator dist_out_iter;
      for ( dist_out_iter = sorted_adj.begin();
            dist_out_iter != sorted_adj.end();
            dist_out_iter++ )
      {
        int_id1 = dist_out_iter->second.row;
        int_id2 = dist_out_iter->second.col;
         
        dist_out << node_info[int_id1].id << "\t"
                 << node_info[int_id2].id << "\t"
                 << dist_out_iter->first << "\t"
                 << node_info[int_id1].x << "\t"
                 << node_info[int_id1].y << "\t"
                 << node_info[int_id2].x << "\t"
                 << node_info[int_id2].y << endl;
      }
      dist_out.close();
      
      cout << "Writing out the .mindist file ..." << endl;
      ofstream min_out ( command_line.dist_file.c_str() );
      if ( !min_out )
      {
         cout << "Error: could not open .mindist file.  Program stopped." << endl;
         exit(1);
      }

      // output minimum sim distances
      for ( int min_sim_i = 0; min_sim_i < min_sim.size(); min_sim_i++ )
        min_out << min_sim[min_sim_i] << endl;
      
      min_out.close();
      
    }
      
    /*
    // output sorted table for debugging:
    multimap < float, id_pair >::iterator sort_adj_iter;
    for ( sort_adj_iter = sorted_adj.begin();
          sort_adj_iter != sorted_adj.end();
          sort_adj_iter++ )
      cout << sort_adj_iter->second.row << " "
           << sort_adj_iter->second.col << " "
           << sort_adj_iter->first << endl;
    */
             
    // choose automatic value for distance, if necessary
    if ( command_line.threshold == 0 )
        command_line.threshold = select_threshold ( min_sim, command_line.neighborhood_size );
    
    // call average link algorithm
    cout << "Computing average link clustering ..." << endl;

    int line_count;
    int pid1, pid2;
    float dist, x1, y1, x2, y2;
    average_link cluster ( node_info.size()-1, command_line.threshold );
    multimap < float, id_pair >::iterator sort_mat_iter;
    for ( sort_mat_iter = sorted_adj.begin();
          sort_mat_iter != sorted_adj.end();
          sort_mat_iter++ )
    {
        pid1 = sort_mat_iter->second.row;
        pid2 = sort_mat_iter->second.col;
        dist = sort_mat_iter->first;
        x1 = node_info[pid1].x;
        y1 = node_info[pid1].y;
        x2 = node_info[pid2].x;
        y2 = node_info[pid2].y;
        
        /*
        // output table for debugging
        cout << pid1 << " " << pid2 << " "
             << dist << " " << x1 << " " << y1 << " "
             << x2 << " " << y2 << " " << endl;
        */
        
        cluster.next_line ( pid1, pid2, dist, x1, y1, x2, y2 );
    }
    
       
    // output average link clustering 
    cout << "Writing output file ..." << endl;
    
    cluster.output_clusters ( command_line.slc_file, node_info );
    
    cout << "Program finished successfully." << endl;
}


