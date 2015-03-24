// Truncate.cpp --
//
// This program performs the initial truncate step for the recursive layout,
// generating .int, .ind, and .full files for use by the rest of the
// process.
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
#include <math.h>
#include <cstdlib>

using namespace std;

// parse command line
#include <truncate_parse.h>

// The following function scans the .sim file, creates the id catalog
// and outputs the .ind and .full files.  The .full files is the same
// as the .sim file but contains the integer ids from the .ind file.
// The order of the .ind file can be randomized (this has been commented out).

void create_ind_full( string sim_file, string ind_file, string full_file,
					  map <string, int> &id_catalog )
{

  cout << "Reading .sim file ... " << endl;
  
  // modification of Brian's original code
  char node1_buf[200], node2_buf[200], sim_buf[500]; 
  float edge_weight;
  FILE *fp;
  int ret;

  // Open (sim) File
  fp = fopen( sim_file.c_str(),"r" );
  if (fp == NULL)
  {
	cout << "Error: could not open " << sim_file << ".  Program terminated." << endl;
	exit (1);
  }	
  
  size_t pos1, pos2;					// variables for parsing
  string sim, id1, id2;
  
  // Read file, parse, and add into data structure
  int line_count = 0;
  while ((ret=fscanf(fp,"%[^\t]\t%[^\t]\t%f\n",node1_buf,node2_buf,&edge_weight)) && (ret!=EOF))
	{
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

	  // count line
	  line_count++;
	  
	  // ignore negative weights!
	  if ( edge_weight > 0 )
	  {
 	    // allocate new nodes if necessary
        if ( id_catalog.find (id1) == id_catalog.end () )
          id_catalog[id1] = 1;
        if ( id_catalog.find (id2) == id_catalog.end () )
          id_catalog[id2] = 1;
	  }
	}

  fclose(fp);

  if ( id_catalog.size() == 0 )
  {
    cout << "Error: " << sim_file << " is empty.  Program terminated." << endl;
	exit (1);
  }
  
  cout << "Read " << id_catalog.size() << " nodes and " << line_count << " lines." << endl;

  // node ids have been determined
  
  ofstream ind;
  ind.open( ind_file.c_str() );
  if ( !ind )
  {
	cout << "Error: could not open " << ind_file << ".  Program terminated." << endl;
	exit (1);
  }
   
  vector<string> to_write;
  for( map< string, int >::iterator i = id_catalog.begin(); 
	   i != id_catalog.end(); i++ ) {
    to_write.push_back(i->first);
  }

  // for random ordering of ind file:
  //srand ( 0 );
  //random_shuffle(to_write.begin(), to_write.end());
  
  cout << "Creating .ind file ..." << endl;
  
  // write and update id_catalog with index values
  map<string, int> assoc;
  for(unsigned int i=0; i < to_write.size(); i++ ) {
    assoc[to_write[i]] = i;
    ind << to_write[i] << "\t" << i << endl;
	id_catalog[to_write[i]] = i;
  }
  ind.close();

  // Now create the full file;

  ofstream num;
  num.open( full_file.c_str() );
  if ( !num )
  {
    cout << "Error: could not open " << full_file << ".  Program terminated." << endl;
	exit (1);
  }
  
  // Reset a buffer to the beginning
  // Open (sim) File
  fp = fopen( sim_file.c_str() ,"r");
  if (fp == NULL)
  {
	cout << "Error: could not open " << sim_file << " (for translation to .int).  Program terminated." << endl;
	exit (1);
  }

  cout << "Writing to .full file ..." << endl;
  
  // Read file, parse, and output to .int
  while ((ret=fscanf(fp,"%[^\t]\t%[^\t]\t%f\n",node1_buf,node2_buf,&edge_weight)) && (ret!=EOF))
	{
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

	  // ignore negative weights!
	  if ( edge_weight > 0 )
	  {
  
		// output to .int
		num << assoc[id1] << "\t"
			<< assoc[id2] << "\t"
			<< edge_weight << endl;
	  }
	}

  fclose(fp);
  num.close();

}

// This routine uses the .full file to compute the denominators for
// normalization

void get_denoms(string full_file, int memory_use, int num_nodes,
                vector <float> &denom_sims )
{
  cout << "Computing normalization denominators ..." << endl;
  // run multiple scans of .full file and create intermediate .int file
  ifstream in;
  int mem_step = num_nodes/memory_use;
  map <int, map<int, float> > sim_block;
  map<int, float>::iterator sim_iter;
  int i,j, id1, id2;
  double sim_val;

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

      while ( !in.eof() )
      {
        id1 = -1;
        in >> id1 >> id2 >> sim_val;
		id1 = id1;
		id2 = id2;
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
        for ( sim_iter = sim_block[j].begin(), denom_sims[j] = 0.0;
              sim_iter != sim_block[j].end();
              sim_iter++ )
          denom_sims[j] = denom_sims[j] + sim_iter->second;

      // erase block of similarities
      sim_block.clear();
  }
}

// Next we create a precursor .int file (normalized but not between 0 and 1)
// and save it to a .tmp file.
void create_int( string full_file, string int_file, int memory_use,
			    int topn, int num_nodes, vector <float> &denom_sims )
{
  cout << "Creating .int file ..." << endl;
  // run multiple scans of .full file and create .int file
  ifstream in;
  int mem_step = num_nodes/memory_use;
  map <int, map<int, float> > sim_block;
  map<int, float>::iterator sim_iter;
  multimap<float, int> sim_row;
  multimap<float, int>::iterator row_iter;
  int i,j,k, id1, id2;
  float sim_val;
  
  ofstream out ( int_file.c_str() );
  if ( !out )
  {
    cout << "Error: could not open .int file." << endl;
    exit(1);
  }
  
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
        exit (1);
      }
	  
      while ( !in.eof() )
      {
        id1 = -1;
        in >> id1 >> id2 >> sim_val;
		id1 = id1;
		id2 = id2;
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
	  
      // normalize similarities
      for ( j = mem_start; j < mem_stop; j++ )
        for ( sim_iter = sim_block[j].begin();
              sim_iter != sim_block[j].end();
              sim_iter++ )
          sim_iter->second = sim_iter->second /
                             sqrt ( denom_sims[j] * denom_sims[sim_iter->first] );
      
      // write out top n links and keep track of max weight
      for ( j = mem_start; j < mem_stop; j++ )
      {
        // sort row
        sim_row.clear();
        for ( sim_iter = sim_block[j].begin();
              sim_iter != sim_block[j].end();
              sim_iter++ )
          sim_row.insert (pair<float,int>(sim_iter->second,sim_iter->first));
          
        // output top n in this row
        for ( k = 0, row_iter = sim_row.end();
              (k < topn) && (row_iter != sim_row.begin());
              k++ )
        {
          row_iter--;
          out << j << "\t" << row_iter->second << "\t" << row_iter->first << endl;
        }
      }
      
      // erase block of similarities
      sim_block.clear();
  }
  
  out.close();
}

void create_real ( map < string, int > &id_catalog, string coord_file, string real_file )
{

  cout << "Reading .coord file ..." << endl;
  
  string id;
  float x_coord, y_coord;
  char id_buf[200];
  int ret;

  // code to read .coord file was taken from original vxord
  // Open File
  FILE *fp;
  fp = fopen(coord_file.c_str(),"r");
  if (fp == NULL) {
		printf("Couldn't open input file %s.\n",coord_file.c_str());
		exit(1);
  }
  
  cout << "Writing .real file ..." << endl;
  // open real file for output
  ofstream out_real ( real_file.c_str() );
  if ( !out_real )
  {
    cout << "Error: could not open .real file." << endl;
    exit(1);
  }
  
  // Read in all coordinates
  while ((ret=fscanf(fp,"%[^\t]\t%f\t%f\n",id_buf,&x_coord,&y_coord)) && (ret!=EOF)) {
		
		// Convert id to string
		id = id_buf;

		if ( id_catalog.find(id) == id_catalog.end() )
		{
			cout << "Error: found id in .coord file not present in .sim file." << endl;
			exit(1);
		}
		
		// write out to .real file
		out_real << id_catalog[id] << "\t" << x_coord << "\t" << y_coord << endl;
		
  }

  // Close files
  fclose(fp);
  out_real.close();

}
                
int main(int argc, char *argv[]) {
 
  // get user input
  parse command_line ( argc, argv );
  
  // This code proceeds in four steps:
  // 1. The .sim file is scanned to obtain an id catalog and to create a .ind file.  At 
  //    this stage the .full file is also created containing the sim file with integer ids.
  // 2. The .coord file is scanned to produce the .real file (optional).
  // 3. The .full file is scanned x times to obtain the normalization denominators, if required.
  // 4. The .full file is scanned again x times to create a .int file.
  //    This file has no header line, and has normalized but not divided by max weight
  //    entries.

  // normalization is performed by the formula wij/sqrt(sum(wij)*sum(wji)).  This normalization
  // is in agreement with Kevin's method for coarsening normalization.
  
  // STEP 1
  // ------
  map<string, int> id_catalog;
  create_ind_full ( command_line.sim_file, command_line.ind_file, 
			        command_line.full_file, id_catalog );
  int num_nodes = id_catalog.size();

  // STEP 2
  // ------
  if ( command_line.real_out )
    create_real ( id_catalog, command_line.coord_file, command_line.real_file );
  
  // STEP 3
  // ------
  vector <float> denom_sims ( num_nodes );
  if ( command_line.normalize )
    get_denoms ( command_line.full_file, command_line.memory_use, num_nodes,
                  denom_sims );
  else
    for ( int i = 0; i < num_nodes; i++ )
      denom_sims[i] = 1.0;

  // STEP 4
  // ------
  create_int ( command_line.full_file, command_line.int_file,
               command_line.memory_use, command_line.topn, num_nodes,
               denom_sims );
  
  cout << "Program finished successfully." << endl;

}
