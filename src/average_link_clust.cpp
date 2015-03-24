// This file contains the member definitions of the average_link class

// The average_link class contains the ported awk code from Kevin Boyack.
// The code is unmodified except for variable declarations and
// various features that have been commented out.

// Original explanation of algorithm:
//
// The "importance" rules are as follows:
// 
// When forming a new cluster before the thresold, both elements get
// assigned importance=1.  New clusters that are formed after the
// threshold get assigned an importance of -1.
// 
// When adding an element to a cluster, the element that "pulled in" the
// added element has its importance incremented and the element added is
// assigned importance=0, if before threshold.  If after the threshold, the
// element added is assigned -1 (and the other element is unchanged).
// 
// When joining, no importances are effected in any case.
//
// Joinability rules:
// After the threshold, the following takes effect:
// - All clusters formed before the cluster are "unjoinable"
// - All new clusters of 2 elements are "joinable"
// - Every time a joinable cluster grows by one it stays joinable
// - When a pair of elements is considered, and they both already
// belong to clusters:
//      - If one or both clusters are joinable, join them,
//        EVEN IF Z IS NOT LESS THAN 0
//      - If they are both joinable, the new cluster is joinable,
//        otherwise, the new cluster is not joinable

// The function average_link contains the awk average link code.
// pid1, pid2 are "paper ids" which must be integers but do
// not necessarily have to be continuous, while dist, x1, y1, x2, y2
// are floats where dist = sqrt( (x1-x2)^2 + (y1-y2)^2 ).
// The code was originally called with each line of a file with
// these quantities sorted in ascending order by dist.

#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>

#include <average_link_clust.h>

// The constructor allocates the space necessary to run the
// average_link code and sets all variables to 0 (the default
// in awk).  max_paper_id is the maximum integer id for all the papers
// set_threshold is the value for THRESHOLD

average_link::average_link ( int set_max_paper_id, float set_threshold )
{
    nPairs = 0;
    nClusters = 0;
    papercount = 0;
    nJoins = 0;
    
    // save max_paper_id
    max_paper_id = set_max_paper_id;
    
    cluster.assign ( max_paper_id+1, 0 );
    newcluster.assign ( max_paper_id+1, 0 );
    sumdist.assign ( max_paper_id+1, (float)0.0 );
    sumX.assign ( max_paper_id+1, (float)0.0 );
    sumY.assign ( max_paper_id+1, (float)0.0 );
    nPapers.assign ( max_paper_id+1, 0 );
    nCords.assign ( max_paper_id+1, 0 );
    joinable.assign ( max_paper_id+1, 0 );
    importance.assign ( max_paper_id+1, 0 );
    
    THRESHOLD = set_threshold;
}

void average_link::next_line ( int pid1, int pid2, float dist,
                              float x1, float y1, float x2, float y2 )
{

  // for debugging (test file input):
  /*
  cout << pid1 << "\t" << pid2 << "\t" << dist << "\t"
       << x1 << "\t" << y1 << "\t" << x2 << "\t" << y2 << endl;
  */
  
  // local awk variables
  int cluster1, cluster2;//, temp; 
  int nPapers1, nPapers2, nCords1, nCords2;
  float dx1, dx2, dy1, dy2, distclusters;
  float avedist1, avedist2, distedge1, distedge2, dadd, expecteddist, Z;
   
  // nPairs keeps track of the number of lines we have done
  nPairs++;

  //  find cluster of pid1
  cluster1 = cluster[pid1];
  //temp = cluster1;
  while (newcluster[cluster1] != 0) cluster1 = newcluster[cluster1];
  cluster[pid1] = cluster1;  // cache/update, for future reference
  //if (temp != 0 && cluster1 == 0) cout << "somethingwronghere1" << endl;

  //  find cluster of pid2
  cluster2 = cluster[pid2];
  //temp = cluster2;
  while (newcluster[cluster2] != 0) cluster2 = newcluster[cluster2];
  cluster[pid2] = cluster2;  // cache/update, for future reference
  //if (temp != 0 && cluster2 == 0) cout << "somethingwronghere2" << endl;

  if (cluster1 == 0 && cluster2 == 0) {  // make a new cluster
    nClusters++;
    cluster[pid1] = nClusters;
    cluster[pid2] = nClusters;
    papercount += 2;
    sumdist[nClusters] = 2 * dist;
    sumX[nClusters] = x1 + x2;
    sumY[nClusters] = y1 + y2;
    nPapers[nClusters] = 2;
    nCords[nClusters] = 2;
    //cout << "1 , " << dist << endl; 
    //action[nActions] = 1;
    //actionid1[nActions] = pid1;
    //actionid2[nActions] = pid2;
    //nActions++;
    if (dist <= THRESHOLD) {
       importance[pid1] = 1;
       importance[pid2] = 1;
    }
    else {            // reminder: "used to" make new only if dist<=thresh
       joinable[nClusters] = 1;
       importance[pid1] = -1;
       importance[pid2] = -1;
    }
  }
  else if (cluster1 == 0) {  // add pid1 to pid2s cluster
    cluster[pid1] = cluster2;
    papercount++;
    //  update position/width:
    sumdist[cluster2] += dist;
    sumX[cluster2] += x1;
    sumY[cluster2] += y1;
    nPapers[cluster2]++;
    nCords[cluster2]++;
    if (dist <= THRESHOLD) {
	importance[pid2]++;
        //action[nActions] = 2;
    }
    else {
	importance[pid1]--;
        //action[nActions] = 3;
    }
    //cout << "0 , " << dist << endl;
    //actionid1[nActions] = pid1;
    //actionid2[nActions] = pid2;
    //nActions++;
  }
  else if (cluster2 == 0) {  // add pid2 to pid1s cluster
    cluster[pid2] = cluster1;
    papercount++;
    //  update position/width:
    sumdist[cluster1] += dist;
    sumX[cluster1] += x2;
    sumY[cluster1] += y2;
    nPapers[cluster1]++;
    nCords[cluster1]++;
    if (dist <= THRESHOLD) {
        importance[pid1]++;
        //action[nActions] = 2;
    }
    else {
	importance[pid2]--;
        //action[nActions] = 3;
    }
    //cout << "0 , " << dist << endl;
    //actionid1[nActions] = pid2;
    //actionid2[nActions] = pid1;
    //nActions++;
  }
  else if (cluster1 != cluster2)  {  // maybe combine clusters

   // this new conditional for logical fix
   if (dist <= THRESHOLD || joinable[cluster1] == 1 || joinable[cluster2] == 1) {

    nPapers1 = nPapers[cluster1];
    nPapers2 = nPapers[cluster2];
    nCords1 = nCords[cluster1];
    nCords2 = nCords[cluster2];
    dx1 = sumX[cluster1] / nPapers1;
    dx2 = sumX[cluster2] / nPapers2;
    dy1 = sumY[cluster1] / nPapers1;
    dy2 = sumY[cluster2] / nPapers2;

    distclusters = sqrt(((dx1 - dx2)*(dx1 - dx2)) + ((dy1 - dy2)*(dy1 - dy2)));
    avedist1 = sumdist[cluster1] / nCords1;
    avedist2 = sumdist[cluster2] / nCords2;
    distedge1 = .564 * avedist1 * sqrt( (float)nPapers1 );
    distedge2 = .564 * avedist2 * sqrt( (float)nPapers2 );
    dadd = ((dist - avedist1) / 2) + ((dist - avedist2) / 2);
    expecteddist = distedge1 + distedge2 + dadd;
    Z = (distclusters - expecteddist) / dist;

      if (Z < 0 || dist > THRESHOLD) {        // do the join
	    nJoins++;
        nClusters++;
        newcluster[cluster1] = nClusters;
        newcluster[cluster2] = nClusters;
        //  compute position/width of new cluster
        sumdist[nClusters] = sumdist[cluster1] + sumdist[cluster2] + dist;
        sumX[nClusters] = sumX[cluster1] + sumX[cluster2];
        sumY[nClusters] = sumY[cluster1] + sumY[cluster2];
        nPapers[nClusters] = nPapers[cluster1] + nPapers[cluster2];
        nCords[nClusters] = nCords[cluster1] + nCords[cluster2] + 1;
        //cout << "-1 , " << dist << endl;
        //action[nActions] = 4;
        //actionid1[nActions] = pid1;
        //actionid2[nActions] = pid2;
        //nActions++;
	    if (joinable[cluster1] == 1 && joinable[cluster2] == 1)
	      joinable[nClusters] == 1;
      }
    }
  }

  // else they are already in the same cluster

}

// This routine outputs the results of the average link clustering
// algorithm.

void average_link::output_clusters ( string filename, map < int, node> &node_info )
{

    // local awk variables
    int i, curcluster, nJoinable, nJoinableElements;
    curcluster = 0;
    nJoinable = 0;
    nJoinableElements = 0;
    
    int cluster1;
    
    vector<int> clusternumber;
    clusternumber.assign ( max_paper_id+1, 0 );
    
    // show the cluster ID for each paper

    //  renumbering scheme
    for (i=1; i <= nClusters; i++) {
        if (newcluster[i] == 0) {
            curcluster++;
	    clusternumber[i] = curcluster;
	    if (joinable[i] == 1) nJoinable++;
        }
    }

    //   for (i=0; i < papercount; i++) {
    //   for (i=0; i <= 7226; i++) {
    //   for (i=0; i <= 7300; i++) {
    //   for (i=0; i <= 4000000; i++) {

    ofstream clust_out ( filename.c_str() );
    if ( !clust_out )
    {
        cout << "Error: could not open " << filename << ".  Program stopped." << endl;
        exit(1);
    }
    
    for (i=0; i <= max_paper_id; i++) {
        cluster1 = cluster[i];
	if (cluster1 != 0) {
            while (newcluster[cluster1] != 0) cluster1 = newcluster[cluster1];
	    if (joinable[cluster1] == 1) nJoinableElements++;
	    cluster1 = clusternumber[cluster1];  // goes with renumbering scheme
	    clust_out << node_info[i].id << "\t" << cluster1 << "\t" << importance[i] << endl;
	    //printf("%d , %d , %d\n", i, cluster1, importance[i]);
        }
    }
    clust_out.close();

    //  PUT NO COMMAS IN THIS OUTPUT SO EACH LINE IS "ONE" FIELD OF STATS:
    //cout << "nClusters " << nClusters << "   nJoins " << nJoins << "  nJoinable " << nJoinable << endl;
    //cout << "papercount " << papercount << "     nClusters " << nClusters << endl;
    //cout << "nPairs " << nPairs << "   THRESHOLD: " << THRESHOLD << endl;
    //cout <<  "nJoinableElements " << nJoinableElements << endl;

    // print out the actions (file 4):
    //for (i=0; i < nActions; i++ ) {
	//print "Action," action[i] "," actionid1[i] "," actionid2[i];
    //}

}
