// Recoord.cpp --
//
// This program reverses the effect of truncate to revert to original labels
//
// The structure of the inputs and outputs of this code will be displayed
// if the program is called without parameters, or if an erroneous
// parameter is passed to the program.
//
// S. Martin
// 5/13/2006

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
#include <recoord_parse.h>

// create .edges file from .iedges file
void create_edges ( map < int, string > &id_catalog, string iedges_file,
				    string edges_file )
{

  ifstream in_edges ( iedges_file.c_str() );
  if ( !in_edges )
  {
    cout << "Error: could not open " << iedges_file << "." << endl;
    exit(1);
  }
  
  cout << "Reading .iedges file ..." << endl;
  
  ofstream out_edges ( edges_file.c_str() );
  if ( !out_edges )
  {
    cout << "Error: could not open " << edges_file << "." << endl;
    exit(1);
  }
  
  cout << "Creating .edges file ..." << endl;
  
  int int_id1, int_id2;
  float weight;
  while ( !in_edges.eof() )
  {
  
     int_id1 = -1;
	 in_edges >> int_id1 >> int_id2 >> weight;
	 
	 if ( int_id1 >= 0 )	// not at end of file
	 {
	   if ( (id_catalog.find ( int_id1 ) == id_catalog.end()) ||
	        (id_catalog.find ( int_id2 ) == id_catalog.end()) )
	   {
		 cout << "Error: found unknown integer ids." << endl;
		 exit (1);
	   }
	   else
	     out_edges << id_catalog[int_id1] << "\t" << id_catalog[int_id2] << "\t"
			       << weight << endl;
	 }
	 
  }
  
  in_edges.close();
  out_edges.close();
  
}

// create .coord file from .icoord file
void create_coord ( map < int, string > &id_catalog, string icoord_file,
				    string coord_file )
{

  ifstream in_coord ( icoord_file.c_str() );
  if ( !in_coord )
  {
    cout << "Error: could not open " << icoord_file << "." << endl;
    exit(1);
  }
  
  cout << "Reading .icoord file ..." << endl;
  
  ofstream out_coord ( coord_file.c_str() );
  if ( !out_coord )
  {
    cout << "Error: could not open " << coord_file << "." << endl;
    exit(1);
  }
  
  cout << "Creating .coord file ..." << endl;
  
  int int_id;
  float x_coord, y_coord;
  while ( !in_coord.eof() )
  {
  
     int_id = -1;
	 in_coord >> int_id >> x_coord >> y_coord;
	 
	 if ( int_id >= 0 )	// not at end of file
	 {
	   if ( id_catalog.find(int_id) != id_catalog.end() )
	     out_coord << id_catalog[int_id] << "\t" << x_coord << "\t" << y_coord << endl;
	   else
	   {
	     cout << "Error: found unknown integer id." << endl;
		 exit (1);
	   }
	 }
  }
  
  in_coord.close();
  out_coord.close();
  
}

// read .ind file into id_catalog variable
void read_ind ( string ind_file, map < int, string > &id_catalog )
{

  cout << "Reading .ind file ..." << endl;
  
  int int_id;
  string string_id;
  char id_buf[200];
  int ret;

  // code to read .ind file was taken from original vxord
  // Open File
  FILE *fp;
  fp = fopen(ind_file.c_str(),"r");
  if (fp == NULL) {
		printf("Couldn't open input file %s.\n",ind_file.c_str());
		exit(1);
  }
  
  // Read in all coordinates
  while ((ret=fscanf(fp,"%[^\t]\t%d\n",id_buf,&int_id)) && (ret!=EOF)) {
		
		// Convert id to string
		string_id = id_buf;	
		
		id_catalog[int_id] = string_id;
		
  }

  // Close file
  fclose(fp);

  /*
  // print out id catalog (for debugging)
  map < int, string >::iterator cat_iter;
  for ( cat_iter = id_catalog.begin(); cat_iter != id_catalog.end(); cat_iter++ )
    cout << cat_iter->first << "\t" << cat_iter->second << endl;
  */
  	
}
                
int main(int argc, char *argv[]) {
 
  // get user input
  parse command_line ( argc, argv );
  
  // read .ind file
  map < int, string > id_catalog;
  read_ind ( command_line.ind_file, id_catalog );

  // translate .icoord to .coord
  create_coord ( id_catalog, command_line.icoord_file, command_line.coord_file );

  // translate .iedges to .edges, if desired
  if ( command_line.edges_out )
    create_edges ( id_catalog, command_line.iedges_file, command_line.edges_file );

  cout << "Program finished successfully." << endl;
}
