#***************************************************************************
#                               Makefile
#                          -------------------
#
#  _________________________________________________________________________
#    Build for DrL
#  _________________________________________________________________________
#
#    begin                : Sat Oct 29 2005
#    copyright            : (C) 2005 by W. Michael Brown
#    email                : wmbrown@sandia.gov
# ***************************************************************************/

include ./Configuration.mk
OBJ_DIR = $(HOBJ_DIR)

VX_O     = $(OBJ_DIR)/layout.o $(OBJ_DIR)/parse.o \
           $(OBJ_DIR)/DensityGrid.o $(OBJ_DIR)/graph.o

VX_E     = $(BIN_DIR)/layout

REC_O 	 = $(OBJ_DIR)/average_link.o $(OBJ_DIR)/average_link_clust.o $(OBJ_DIR)/average_link_parse.o \
	   $(OBJ_DIR)/recoord.o $(OBJ_DIR)/recoord_parse.o \
	   $(OBJ_DIR)/coarsen.o $(OBJ_DIR)/coarsen_parse.o $(OBJ_DIR)/refine.o \
	   $(OBJ_DIR)/refine_parse.o $(OBJ_DIR)/truncate.o $(OBJ_DIR)/truncate_parse.o

REC_E	 = $(BIN_DIR)/truncate $(BIN_DIR)/average_link $(BIN_DIR)/coarsen $(BIN_DIR)/refine $(BIN_DIR)/recoord

all: $(VX_O) $(VX_E) $(REC_O) $(REC_E)

$(OBJ_DIR)/truncate.o: truncate.cpp
	$(CPP) $(CFLAGS) -o $@ truncate.cpp

$(OBJ_DIR)/truncate_parse.o: truncate_parse.cpp
	$(CPP) $(CFLAGS) -o $@ truncate_parse.cpp

$(OBJ_DIR)/recoord.o: recoord.cpp
	$(CPP) $(CFLAGS) -o $@ recoord.cpp

$(OBJ_DIR)/recoord_parse.o: recoord_parse.cpp
	$(CPP) $(CFLAGS) -o $@ recoord_parse.cpp

$(OBJ_DIR)/average_link.o: average_link.cpp
	$(CPP) $(CFLAGS) -o $@ average_link.cpp

$(OBJ_DIR)/average_link_clust.o: average_link_clust.cpp
	$(CPP) $(CFLAGS) -o $@ average_link_clust.cpp

$(OBJ_DIR)/average_link_parse.o: average_link_parse.cpp
	$(CPP) $(CFLAGS) -o $@ average_link_parse.cpp

$(OBJ_DIR)/coarsen.o: coarsen.cpp
	$(CPP) $(CFLAGS) -o $@ coarsen.cpp

$(OBJ_DIR)/coarsen_parse.o: coarsen_parse.cpp
	$(CPP) $(CFLAGS) -o $@ coarsen_parse.cpp

$(OBJ_DIR)/refine.o: refine.cpp
	$(CPP) $(CFLAGS) -o $@ refine.cpp

$(OBJ_DIR)/refine_parse.o: refine_parse.cpp
	$(CPP) $(CFLAGS) -o $@ refine_parse.cpp

$(OBJ_DIR)/layout.o: layout.cpp
	$(CPP) $(CFLAGS) -o $@ layout.cpp

$(OBJ_DIR)/parse.o: parse.cpp
	$(CPP) $(CFLAGS) -o $@ parse.cpp

$(OBJ_DIR)/DensityGrid.o: DensityGrid.cpp
	$(CPP) $(CFLAGS) -o $@ DensityGrid.cpp

$(OBJ_DIR)/graph.o: graph.cpp
	$(CPP) $(CFLAGS) -o $@ graph.cpp

$(BIN_DIR)/truncate: $(OBJ_DIR)/truncate.o
	$(CPP) $(LFLAGS) -o $@ $(OBJ_DIR)/truncate.o $(OBJ_DIR)/truncate_parse.o

$(BIN_DIR)/recoord: $(OBJ_DIR)/recoord.o
	$(CPP) $(LFLAGS) -o $@ $(OBJ_DIR)/recoord.o $(OBJ_DIR)/recoord_parse.o

$(BIN_DIR)/average_link: $(OBJ_DIR)/average_link.o
	$(CPP) $(LFLAGS) -o $@ $(OBJ_DIR)/average_link.o $(OBJ_DIR)/average_link_clust.o $(OBJ_DIR)/average_link_parse.o
	
$(BIN_DIR)/coarsen: $(OBJ_DIR)/coarsen.o
	$(CPP) $(LFLAGS) -o $@ $(OBJ_DIR)/coarsen.o $(OBJ_DIR)/coarsen_parse.o

$(BIN_DIR)/refine: $(OBJ_DIR)/refine.o
	$(CPP) $(LFLAGS) -o $@ $(OBJ_DIR)/refine.o $(OBJ_DIR)/refine_parse.o

$(BIN_DIR)/layout: $(VX_O)
	$(CPP) $(LFLAGS) -o $@ $(VX_O)


#
#  Remove objects, cores, etc.
#

clean:
	rm -rf *.o $(VX_O) $(VX_E) $(REC_O) $(REC_E) \
	  a.out core test *.exe Makefile.win Makefile.am*

veryclean: clean
	rm -rf *~ ./api
