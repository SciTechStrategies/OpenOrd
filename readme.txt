OpenOrd: An Open-Source Toolbox for Large Graph Layout
------------------------------------------------------

S. Martin
smartin@sandia.gov
11/13/2007

Citation
--------
If you use this code in your research, please cite the following 
article:

S. Martin, W. M. Brown, R. Klavans, K. Boyack (2011) "OpenOrd: An Open-Source
Toolbox for Large Graph Layout," Visualization and Data Analysis (VDA):7868-06.
 

To Install for Linux/Unix/Mac
-----------------------------

First, copy the correct compiler information in the src directory 
to the file Configuration.mk.  Examples are included in the src
directory:

Configuration.gnu  # Default GNU (not parallel)
Configuration.gnu_parallel # GNU with LAM/MPI
Configuration.intel # Highly optimized serial intel

For example to compile using parallel GNU:

cd src
cp Configuration.gnu_parallel Configuration.mk
make
ls ../bin

To Install for Windows
----------------------

You need to have gcc and bash.  I have used MSYS and MinGW successfully
for that purpose in the past.  If you want to do this, go ahead and
install MSYS and MinGW to the directories MSYS/1.0 and MinGW/MinGW.
Then compile the codes from the src directory by typing

> copy Configuration.win Configuration.mk
> path ..\MSYS\1.0\bin;..\MinGW\MinGW\bin
> make

NOTE: If you do not want to set the path variables each time then you 
can specify the path permanently using Windows.  Also, you can install 
MSYS to your computer permanently.

Documentation -- Important Note
-------------------------------

Important NOTE: all programs in the OpenOrd library have documentation 
available, including descriptions of input and output formats, by typing 
the name of the program with no input.  For example, in the bin 
directory you can type 

  > layout

to get a full description of the inputs to the force directed layout 
program.

Examples
--------

Examples can be found in the examples directory.  If you are using 
windows you must specify the MSYS directory, as described in the 
readme.txt file in the examples directory.  

Modifications
-------------

The code is based on the following ideas, so any modifications might 
adhere to the below "standards:"

1.  A project has a rootname along with different extensions.  The
    yeast_gs directory in the examples directory is a project with
    the rootname "yeast_gs."  The initial file is "yeast_gs.sim" and
    numerous other files are produced along the way, but nearly all
    the commands in the bin directory only need the "yeast_gs" rootname
    as input.  If you want to add a new program, it should also input
    only a rootname, then read and write files with appropriate
    extensions.

2.  Each program has the format

    > command_name [options] root_name

    where the [options] are flags such as "-e," sometimes with an 
    argument.  If you want to change a command just add a flag.  
    If you want to change the behavior of an existing flag, make sure
    the preivous behavior is still available.  If you want to add a new
    command you can also use this format.

3.  Every command puts out instructions on it's purpose and how it can be
    used if you type

    > command_name

    with no options.  New commands might also follow this convention.

How the Codes Communicate
-------------------------

Below is how the recursive code communicates via the files with different
extensions.

The initial graph is given by root_name.sim, which is a sparse adjacency
matrix in the format

  <id1> <tab> <id2> <tab> <weight>

where id1 and id2 are strings and weight is a float.  id1 and id2 should
not be the same.

truncate: takes root_name.sim and creates root_name.int, root_name.ind,
        and root_name.full.

layout: takes root_name.int and creates root_name.icoord and (optional)
        root_name.iedges.

single_link: takes root_name.full root_name.icoord and root_name.iedges
        and creates root_name.clust

coarsen: takes root_name_(l-1).full and root_name_(l-1).clust (_(l-1) is
        automatically created by coarsen) and creates root_name_l.full
        and root_name_l.int

refine: takes root_name_l.icoord and root_name_(l-1).clust and creates
        root_name_(l-1).real

recoord: takes root_name.icoord and root_name.ind and creates corresponding
        root_name.coord (optionally .edges can also be created)

Bugs and Future Fixes
---------------------

1.  The coarsen program throws out unconnected components of the graph, 
    keeping only the largest "giant" component.

2.  Edge-cutting can be used in the parallel version of layout, but cut 
    edges are not communicated to the different processors.  This could
    potentially cause problems but really hasn't so far.
