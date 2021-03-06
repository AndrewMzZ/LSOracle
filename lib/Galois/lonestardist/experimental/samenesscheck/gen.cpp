/*
 * This file belongs to the Galois project, a C++ library for exploiting parallelism.
 * The code is being released under the terms of the 3-Clause BSD License (a
 * copy is located in LICENSE.txt at the top-level directory).
 *
 * Copyright (C) 2018, The University of Texas at Austin. All rights reserved.
 * UNIVERSITY EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR ANY PARTICULAR PURPOSE, NON-INFRINGEMENT AND WARRANTIES OF
 * PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE FROM COURSE OF
 * DEALING OR USAGE OF TRADE.  NO WARRANTY IS EITHER EXPRESS OR IMPLIED WITH
 * RESPECT TO THE USE OF THE SOFTWARE OR DOCUMENTATION. Under no circumstances
 * shall University be liable for incidental, special, indirect, direct or
 * consequential damages or loss of profits, interruption of business, or
 * related expenses which may arise from use of Software or Documentation,
 * including but not limited to those resulting from defects in Software and/or
 * Documentation, or loss or inaccuracy of data of any kind.
 */

// NOTE: must be before DistBenchStart.h as that relies on some cuda
// calls

#include <iostream>
#include <limits>
#include "galois/DistGalois.h"
#include "galois/gstl.h"
#include "DistBenchStart.h"
#include "galois/DReducible.h"
#include "galois/runtime/Tracer.h"

struct NodeData {
  char blah;
};

typedef galois::graphs::DistGraph<NodeData, unsigned> Graph;
typedef galois::graphs::DistGraphEdgeCut<NodeData, unsigned> Graph_edgeCut;
typedef typename Graph::GraphNode GNode;

/******************************************************************************/
/* Main */
/******************************************************************************/

constexpr static const char* const name = "Check Sameness";
constexpr static const char* const desc = "Sameness check.";
constexpr static const char* const url  = 0;

int main(int argc, char** argv) {
  galois::DistMemSys G;
  DistBenchStart(argc, argv, name, desc, url);

  const auto& net = galois::runtime::getSystemNetworkInterface();
  std::vector<unsigned> dummyScale;

  partitionScheme = OEC;

  Graph* regular = new Graph_edgeCut(inputFile, partFolder, net.ID, net.Num,
                                     dummyScale, false);
  // pass in the second copy as transpose even though it isn't
  Graph* regular2 = new Graph_edgeCut(inputFileTranspose, partFolder, net.ID,
                                      net.Num, dummyScale, false);

  galois::gPrint("Graphs loaded: beginning checking of all edges.\n");

  galois::do_all(galois::iterate(regular->masterNodesRange()), [&](auto node) {
    for (auto edge : regular->edges(node)) {
      auto edgeDst  = regular->getEdgeDst(edge);
      auto edgeData = regular->getEdgeData(edge);
      bool found    = false;

      // check to see if this edge exists in the other graph
      for (auto edge2 : regular2->edges(node)) {
        auto edgeDst2  = regular2->getEdgeDst(edge2);
        auto edgeData2 = regular2->getEdgeData(edge2);

        if (edgeDst2 == edgeDst) {
          if (edgeData2 == edgeData) {
            found = true;
            break;
          }
        }
      }

      if (!found) {
        printf("Edge %lu to %lu with same weight not found\n",
               regular->getGID(node), regular->getGID(edgeDst));
        GALOIS_DIE("An edge was not found");
      }
    }
  });

  return 0;
}
