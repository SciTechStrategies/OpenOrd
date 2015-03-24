How To Run OpenOrd
------------------

S. Martin
11/13/2007

There are a few different ways to run OpenOrd, described in the 
following cases.  The example used here is derived from a yeast 
microarray dataset.  This dataset is available online at
http://cellcycle-www.stanford.edu/ and is the subject of
Spellman et al., (1998).  Comprehensive Identification of Cell 
Cycle-regulated Genes of the Yeast Saccharomyces cerevisiae by 
Microarray Hybridization.  Molecular Biology of the Cell 9, 3273-3297.

Previous clustering work related to OpenOrd using this dataset has appeared 
in G.S. Davidson, B.N. Wylie, and K.W. Boyack. Cluster Stability and 
the Use of Noise in Interpretation of Clustering. Proceedings of the 
IEEE Symposium on Information Visualization 2001 (INFOVIS'01), 23-30.

Serial
------
First, you can run it as a OpenOrd serial program without recursion.  In 
this case it performs as a force-directed graph layout tool.  The main 
input in this case is just a .sim file, which is just an adjacency 
matrix for a weighted graph.  An example of OpenOrd in this case is given in 
the serial directory.  To run serial non-recursive OpenOrd go to the serial 
directory and type

  > ../../bin/truncate yeast

The truncate command truncates and converts a .sim file into .int files, 
.ind files and .full files, for potential use by other commands in the 
toolbox.  One of the main outputs of this routine is a conversion from 
strings to integer ids.

  > ../../bin/layout yeast

The layout program is the central engine and does the force-directed 
graph drawing using the .int file.

  > ../../bin/recoord yeast

The recoord command re-converts the .icoord file to a standard .coord 
file.  In other words recoord converts the integer ids back to the 
original string ids.

The coordinates that you get from this procedure should look like the 
image shown in serial.pdf (edges were drawn from data produced using the 
-e option for layout and recoord).
yeast_gs

Tips for Using the Serial Layout Tool
-------------------------------------
1. The results of the layout program can be altered dramatically by 
changing the various parameters.  For casual use, the easiest way to 
change the results is by using the -c parameter for edge cutting.  
Edge cutting is done in the late stages of the algorithm in order to 
achieve less dense layouts.  Edges are cut if there is a lot of 
stress on them (a large value in the objective function sum).  The 
edge cutting parameter is a value between 0 and 1 with 0 representing no 
edge cutting and 1 representing maximal edge cutting.

2. You can also alter each of the values in the simulated annealing 
schedule using the .parms file with the -p option.  This is more 
complicated.  Examples of different effects that can be achieved with 
the .parms file can be seen by looking into the recursive_layout.sh 
script.

Recursive
---------
An example showing how to run OpenOrd using the recursive option is found in 
the recursive directory.  To run the recursive example, follow these 
directions.

1. Copy the recursive_layout.sh script to the recursive directory and 
rename it:

   > cp recursive_layout.sh recursive/yeast.sh

2. Copy the parameter files into the directory.

   > cp *.parms recursive

3. Enter the directory.  At this point you would also edit the
   shell script and parms file for your particular project.

   > cd recursive

4. In the directory run the shell script.  In Max/Linux/Unix type

   > ./yeast.sh

   If you are using windows and have installed MSYS you need to specify 
   the MSYS path first, e.g.

   > path ..\..\MSYS\1.0\bin
   > sh yeast.sh

5. To look at the results you can examine the following files:

   Coarsening
   ----------
   .coarse_icoords -- the coordinate files generated during coarsening
   .coarse_iedges -- the edges files generated during coarsening
   .coarse_int -- the .int files generated during coarsening
   .clust -- the average link cluster files generated during coarsening

   Refining
   --------
   .real -- before refining
   .refine_icoord -- the refined files
   .refine_iedges -- refined edge cuts
   .refine_int -- the .int files used during refining

   Other Files
   -----------
   .ind -- conversion between .sim ids and integer ids at top level
   .full -- full integer id version of .int at different levels

6. To clean a project you can use the clean_project.sh command in this
   directory.

   > ./clean_project.sh recursive

   Or in windows:

   > path ..\MSYS\1.0\bin
   > sh clean_project.sh recursive

The full set of coordinates that you get from this procedure should look 
like the images you see in recursive.pdf.

Tips for Using the Recursive Layout Shell
-----------------------------------------
There are a lot of potential parameter adjustments in the recursive 
version of OpenOrd.  Here are a few tips on adjusting the parameters to get 
desired effects.

1. The biggest changes can be had by changing the number of levels in 
the coarsening.  This is done by altering the recursive_layout.sh script 
(usually in the project directory) MAX_LEVEL variable.  Another handy 
feature is the ability to restart, which is done by altering the 
START_LEVEL variable.

2. The next biggest changes are achieved by altering the edge cutting 
parameters in the layout program.  In the recursive version of OpenOrd, 
these parameters can be adjusted in the recursive_layout.sh script by 
changing in particular the LAST_CUT, REFINE_CUT, and FINAL_CUT 
parameters.  LAST_CUT is the edge cutting parameter for the coarsest 
graph, REFINE_CUT is the edge cutting parameter to use during 
refinement, and FINAL_CUT is the edge cutting paramter to use during the 
very last layout.

Parallel
--------
The parallel version of OpenOrd can be run either with or without the 
recursive option.  In any case you need to re-compile the code using the 
configuration.gnu-parallel makefile.  To do this just go to the src 
directory and type (you will need a computer with MPI)

  > make clean
  > cp Configuration.gnu_parallel Configuration.mk
  > make

Now go to the examples/parallel directory.  Depending on the parallel 
computer, you can type something like

  > ../../bin/truncate yeast
  > mpirun -n 4 ../../bin/layout yeast
  > ../../bin/recoord yeast

The result using four processors is shown in parallel.pdf.  Note that 
this process is somewhat random so you may get a slightly different 
picture.

You can also run OpenOrd in the full Distributed recursive Layout mode.  
This is done by using the MPI version of layout along with the 
recursive_layout.sh script, with variables appropriately modified for 
your computer.

Tips for Using the Parallel Version of OpenOrd
----------------------------------------------
1. The parallel version of OpenOrd is used only for extremely large graphs.  
If your computer has a lot of memory it is better to use the serial 
recursive version.  In any case, the only truly parallel code in the OpenOrd 
library is the layout tool.  The other codes are serial.  However, they 
all have adjustments to deal with large files.  The adjustments are 
described in the documentation for each program.  In the truncate tool, 
for example, you can change the -m flag to request multiple scans 
through very large files.  This prevents the entire .sim file from being 
read into memory at once so that larger files can be processed.  The 
other codes have similar options, and the recursive_layout.sh script 
also has variables that can be changed to affect these options.

Real-Time Option
----------------
The last feature that might be of interest in OpenOrd is the real-time 
clustering option, which allows you to add new points to an existing 
layout.  For an example of the real-time option, go to the realtime 
directory.  There you will find yeast.sim, and yeast_90.sim, which 
contains the first 90% of yeast.sim.  

First you have to create a layout using the yeast_90.sim file:

  > ../../bin/truncate yeast_90
  > ../../bin/layout yeast_90
  > ../../bin/recoord yeast_90

Now you have to save the results of the yeast_90 layout for use as 
a starting position for the entire dataset.  This is done by 
creating a .real file as follows:

  > cp yeast_90.coord yeast.coord
  > ../../bin/truncate -r yeast

Finally you can use the base layout with the entire yeast.sim file.  The 
-r flag in layout specifies the length of time to leave the coordinates 
from the .real file fixed (below we leave them permanently fixed).

  > ../../bin/layout -r 1 yeast
  > ../../bin/recoord yeast

The results of these commands are shown in realtime.pdf.
