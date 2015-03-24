// Layout
//
// This program implements a parallel force directed graph drawing
// algorithm.  The algorithm used is based upon a random decomposition
// of the graph and simulated shared memory of node position and density.
// In this version, the simulated shared memory is spread among all processors
//
// The structure of the inputs and outputs of this code will be displayed
// if the program is called without parameters, or if an erroneous
// parameter is passed to the program.
//
// S. Martin
// 5/6/2005

// C++ library routines
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <deque>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;

// layout routines and constants
#include <layout.h>
#include <parse.h>
#include <graph.h>

// MPI
#ifdef MUSE_MPI
  #include <mpi.h>
#endif

int main(int argc, char **argv) {
  
  
  // initialize MPI
  int myid, num_procs;
  
  #ifdef MUSE_MPI
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &num_procs );
    MPI_Comm_rank ( MPI_COMM_WORLD, &myid );
  #else
    myid = 0;
	num_procs = 1;
  #endif
  
  // parameters that must be broadcast to all processors
  int rand_seed;
  float edge_cut;
  
  char int_file[MAX_FILE_NAME];
  char coord_file[MAX_FILE_NAME];
  char real_file[MAX_FILE_NAME];
  char parms_file[MAX_FILE_NAME];
  
  int int_out = 0;
  int edges_out = 0;
  int parms_in = 0;
  float real_in = -1.0;
  
  // user interaction is handled by processor 0
  if ( myid == 0 )
  {
    if ( num_procs > MAX_PROCS )
	{
		cout << "Error: Maximum number of processors is " << MAX_PROCS << "." << endl;
		cout << "Adjust compile time parameter." << endl;
		#ifdef MUSE_MPI
		  MPI_Abort ( MPI_COMM_WORLD, 1 );
		#else
		  exit (1);
		#endif
	}
	
	// get user input
    parse command_line ( argc, argv );
	rand_seed = command_line.rand_seed;
	edge_cut = command_line.edge_cut;
	int_out = command_line.int_out;
	edges_out = command_line.edges_out;
	parms_in = command_line.parms_in;
	real_in = command_line.real_in;
	strcpy ( coord_file, command_line.coord_file.c_str() );
	strcpy ( int_file, command_line.sim_file.c_str() );
	strcpy ( real_file, command_line.real_file.c_str() );
	strcpy ( parms_file, command_line.parms_file.c_str() );
	
  }
  
  // now we initialize all processors by reading .int file
  #ifdef MUSE_MPI
    MPI_Bcast ( &int_file, MAX_FILE_NAME, MPI_CHAR, 0, MPI_COMM_WORLD );
  #endif
  graph neighbors ( myid, num_procs, int_file );
  
  // check for user supplied parameters
  #ifdef MUSE_MPI
    MPI_Bcast ( &parms_in, 1, MPI_INT, 0, MPI_COMM_WORLD );
  #endif
  if ( parms_in )
  {
    #ifdef MUSE_MPI
	  MPI_Bcast ( &parms_file, MAX_FILE_NAME, MPI_CHAR, 0, MPI_COMM_WORLD );
	#endif
	neighbors.read_parms ( parms_file );
  }

  // set random seed, edge cutting, and real iterations parameters
  #ifdef MUSE_MPI
    MPI_Bcast ( &rand_seed, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast ( &edge_cut, 1, MPI_FLOAT, 0, MPI_COMM_WORLD );
	MPI_Bcast ( &real_in, 1, MPI_INT, 0, MPI_COMM_WORLD );
  #endif
  neighbors.init_parms ( rand_seed, edge_cut, real_in );

  // check for .real file with existing coordinates
  if ( real_in >= 0 )
  {
    #ifdef MUSE_MPI
	  MPI_Bcast ( &real_file, MAX_FILE_NAME, MPI_CHAR, 0, MPI_COMM_WORLD );
	#endif
	neighbors.read_real ( real_file );
  }
  
  neighbors.draw_graph ( int_out, coord_file );

  // do we have to write out the edges?
  #ifdef MUSE_MPI
    MPI_Bcast ( &edges_out, 1, MPI_INT, 0, MPI_COMM_WORLD );
  #endif
  if ( edges_out )
    {
	  #ifdef MUSE_MPI
        MPI_Bcast ( &coord_file, MAX_FILE_NAME, MPI_CHAR, 0, MPI_COMM_WORLD );
	  #endif
      for ( int i = 0; i < num_procs; i++ )
	  {
	    if ( myid == i )
	      neighbors.write_sim ( coord_file );
	    #ifdef MUSE_MPI
  	      MPI_Barrier ( MPI_COMM_WORLD );
	    #endif
	  }
    }
  
  // finally we output file and quit
  float tot_energy;
  tot_energy = neighbors.get_tot_energy ();
  if ( myid == 0 )
  {
	neighbors.write_coord ( coord_file );
	cout << "Total Energy: " << tot_energy << "." << endl
	     << "Program terminated successfully." << endl;
  }

  // MPI finalize
  #ifdef MUSE_MPI
    MPI_Finalize ();
  #endif

}
