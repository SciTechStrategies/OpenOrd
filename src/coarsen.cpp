// "Smart" Coarsening for similarity graph based on results of average
// link clustering and layout.
//
// This program performs coarsining on a similarity graph based on
// the results of a average link clustering using layout.
// The algorithm computes derived similarities for groups of nodes
// by adding and possibly normalizing the the number of nodes in each group.
//
// The structure of the inputs and outputs of this code will be displayed
// if the program is called without parameters, or if an erroneous
// parameter is passed to the program.
//
// S. Martin
// 1/27/2005

// C++ library routines
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <deque>
#include <vector>
#include <math.h>
#include <cstdlib>

using namespace std;

// layout routines and constants
#include <coarsen_parse.h>

// The following routine reads in the .clust file and records the
// cluster membership and size information for future use.

void read_clust ( string clust_file, map <int, int> &cluster_membership,
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
      cluster_membership[int_id] = clust_id-1;
      if ( cluster_sizes.find (clust_id-1) == cluster_sizes.end() )
        cluster_sizes[clust_id-1] = 0;
      cluster_sizes[clust_id-1] = cluster_sizes[clust_id-1] + 1;
      if ( clust_id > num_clusts ) num_clusts = clust_id;
      if ( int_id > num_nodes ) num_nodes = int_id;
    }  
  }
    
  clust_in.close ();
  num_nodes++; 
  
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

// This routine scans the .full file to get the normalization denominators
void get_denoms(string full_file, int memory_use, int num_nodes, int num_clusts,
                map <int, int> &cluster_membership, vector <float> &denom_sims )
{
  cout << "Computing normalization denominators ..." << endl;
  
  // initialize denominators to zero
  int i;
  for ( i = 0; i < num_clusts; i++ )
    denom_sims[i] = 0.0;
    
  // run multiple scans of .full file and record denominators
  ifstream in;
  int mem_step = num_nodes/memory_use;
  map <int, map<int, float> > sim_block;
  map<int, float>::iterator sim_iter;
  double sim_val;
  int j, id1, id2;
  for ( i = 0; i < memory_use; i++ )
  {
      int mem_start = mem_step*i;
      int mem_stop = mem_step*(i+1);
      if ( i+1 == memory_use )
        mem_stop = num_nodes;

      // scan in the similarities for a block of nodes
      cout << "Scan " << i+1 << " of .full file ..." << endl;     
      in.open (full_file.c_str());
      if ( !in )
      {
        cout << "Error: could not open .full file." << endl;
        exit(1);
      }
	  //int dummy_num_nodes, dummy_num_edges;
	  //in >> dummy_num_nodes >> dummy_num_edges;
      
      while ( !in.eof() )
      {
        id1 = -1;
        in >> id1 >> id2 >> sim_val;
        if ( id1 >= 0 )
        {
          // is id1 or id2 within our block of interest?
          if ( (mem_start <= id1) & (id1 < mem_stop) )
            sim_block[id1][id2] = sim_val;
          if ( (mem_start <= id2) & (id2 < mem_stop) )
            sim_block[id2][id1] = sim_val;
        }
      }
      in.close();
      in.clear();

      // compute denominator sums
      for ( j = mem_start; j < mem_stop; j++ )
        for ( sim_iter = sim_block[j].begin();
              sim_iter != sim_block[j].end();
              sim_iter++ )
          denom_sims[cluster_membership[j]] = denom_sims[cluster_membership[j]] + sim_iter->second;

      // erase block of similarities
      sim_block.clear();
  }
}

// Now we do the actual coarsening
void coarsen_full ( string full_file, string full_out_file, string int_out_file,
                    int memory_use, int num_clusts, map <int, int> &cluster_sizes,
                    int min_clust, int max_clust, int *topn_links,
                    map <int, int> &cluster_membership, vector <float> &denom_sims )
{
  cout << "Coarsening graph ..." << endl;
  
  ofstream out_full ( full_out_file.c_str() );
  if ( !out_full )
  {
    cout << "Error: could not open " << full_out_file << "." << endl;
    exit(1);
  }
  //out_full << num_clusts << "\t" << 0 << endl;
  
  ofstream out_int ( int_out_file.c_str() );
  if ( !out_int )
  {
    cout << "Error: could not open " << int_out_file << "." << endl;
    exit(1);
  }
  //out_int << num_clusts << "\t" << 0 << endl;
  
  // run multiple scans of .full file
  ifstream in;
  int mem_step = num_clusts/memory_use;
  map <int, map<int, float> > sim_block;
  map <int, map<int, float> > coarse_sim;
  map<int, map<int,float> >::iterator row_iter;
  map<int,float>::iterator col_iter;
  multimap<float, int> sim_row;
  multimap<float, int>::iterator sim_row_iter;
  double sim_val;
  int i, j, k, id1, id2;
  int topn;
  for ( i = 0; i < memory_use; i++ )
  {
      int mem_start = mem_step*i;
      int mem_stop = mem_step*(i+1);
      if ( i+1 == memory_use )
        mem_stop = num_clusts;

      // scan in the similarities for a block of nodes
      cout << "Scan " << i+1 << " of .full file ..." << endl;     
      in.open (full_file.c_str());
      if ( !in )
      {
        cout << "Error: could not open .full file." << endl;
        exit(1);
      }
	  //int dummy_num_nodes, dummy_num_edges;
	  //in >> dummy_num_nodes >> dummy_num_edges;
      
      while ( !in.eof() )
      {
        id1 = -1;
        in >> id1 >> id2 >> sim_val;
        if ( id1 >= 0 )
        {
          // is id1 or id2 within our block of interest?
          if ( (mem_start <= cluster_membership[id1]) &
               (cluster_membership[id1] < mem_stop) )
            sim_block[id1][id2] = sim_val;
          if ( (mem_start <= cluster_membership[id2]) & 
               (cluster_membership[id2] < mem_stop) )
            sim_block[id2][id1] = sim_val;
        }
      }
      in.close();
      in.clear();

      cout << "Computing similarities ..." << endl;
      // compute cluster similarities
      for ( row_iter = sim_block.begin();
            row_iter != sim_block.end(); row_iter++ )
        for ( col_iter = row_iter->second.begin();
              col_iter != row_iter->second.end(); col_iter++ )
        {
           j = cluster_membership[row_iter->first];
           k = cluster_membership[col_iter->first];
           if ( coarse_sim.find (j) == coarse_sim.end() )
             coarse_sim[j][k] = 0.0;
           if ( coarse_sim[j].find(k) == coarse_sim[j].end() )
             coarse_sim[j][k] = 0.0;
           coarse_sim[j][k] = coarse_sim[j][k] + col_iter->second;
        }
      
      cout << "Writing out to .full file ..." << endl;
      
      // write out to .full file & normalize similarities
      for ( row_iter = coarse_sim.begin();
            row_iter != coarse_sim.end(); row_iter++ )
        for ( col_iter = row_iter->second.begin();
              col_iter != row_iter->second.end(); col_iter++ )
        {
           j = row_iter->first;
           k = col_iter->first;
		   // output self links only if there are no other links
           if ( (j != k) ) // || (row_iter->second.size() == 1) )
             out_full << j << "\t" << k << "\t" << coarse_sim[j][k] << endl;
		   // normalize for .int output
           coarse_sim[j][k] = coarse_sim[j][k]/sqrt(denom_sims[j]*denom_sims[k]);
        }     

      cout << "Writing out to .int file ..." << endl;
            
      // write out top n links
      for ( row_iter = coarse_sim.begin();
            row_iter != coarse_sim.end(); row_iter++ )
      {
        // sort row
        j = row_iter->first;
        sim_row.clear();
        for ( col_iter = coarse_sim[j].begin();
              col_iter != coarse_sim[j].end();
              col_iter++ )
            if ( (j != col_iter->first) ) // || (coarse_sim[j].size() == 1) )
               sim_row.insert (pair<float,int>(col_iter->second,col_iter->first));
        
        // output top n for this cluster
        if ( min_clust == max_clust )        // if all clusters are the same use min links
          topn = topn_links[0];
        else                                 // variable number of clusters
          topn = (int)((float)topn_links[0] + (float)(topn_links[1]-topn_links[0]) * 
                ((log((float)cluster_sizes[j]) - log((float)min_clust))/
				(log((float)max_clust)-log((float)min_clust))));
        
		
		for ( k = 0, sim_row_iter = sim_row.end();
              (k < topn) && (sim_row_iter != sim_row.begin());
              k++ )
        {
          sim_row_iter--;
          out_int << j << "\t" << sim_row_iter->second << "\t" << sim_row_iter->first << endl;
        }
      }
                        
                   
      // erase blocks of similarities
      sim_block.clear();
      coarse_sim.clear();
      
  }
  out_full.close();
  out_int.close();
  
}

int main(int argc, char **argv)
{	
    // get user input
    parse command_line ( argc, argv );
    
    // read .clust file
    map <int, int> cluster_membership;
    map <int, int> cluster_sizes;
    int min_clust, max_clust;
    int num_nodes, num_clusts; 
    read_clust ( command_line.clust_file, cluster_membership, cluster_sizes,
                 min_clust, max_clust, num_nodes, num_clusts );
    
    /*
    // output cluster membership (for debugging)
    map <int, int>::iterator cm_iter;
    for ( cm_iter = cluster_membership.begin();
          cm_iter != cluster_membership.end();
          cm_iter++ )
          cout << cm_iter->first << " " << cm_iter->second << endl;
    */
    
    // next we compute denominators for normalization
    vector <float> denom_sims ( num_clusts );
    if ( command_line.normalized_output )
      get_denoms ( command_line.full_file, command_line.memory_use, num_nodes, num_clusts,
                   cluster_membership, denom_sims );
    else
      for ( int i = 0; i < num_clusts; i++ )
        denom_sims[i] = 1.0;
      
    // create new .full file
    coarsen_full ( command_line.full_file, command_line.full_out_file,
                   command_line.int_out_file, command_line.memory_use,
                   num_clusts, cluster_sizes, min_clust, max_clust,
                   command_line.top_n_links, cluster_membership, denom_sims );
       
    cout << "Program finished successfully." << endl;
}


