/***************************************************************************
 *   Copyright (C) 2014 - 2016 Jan Fostier (jan.fostier@intec.ugent.be)    *
 *   This file is part of Brownie                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef DBGRAPH_H
#define DBGRAPH_H

#include "global.h"
#include "ssnode.h"
#include "dsnode.h"
#include "kmernpp.h"
#include "kmercounttable.h"
#include "nodechain.h"

#include <vector>
#include <google/sparse_hash_map>

// ============================================================================
// CLASS PROTOTYPES
// ============================================================================

class Settings;
class ReadLibrary;
class DSNode;
class Arc;
class Settings;
class NodePosPair;
class NodeEndTable;
class NodeEndRef;
class LibraryContainer;

// ============================================================================
// TYPEDEFS
// ============================================================================

typedef google::sparse_hash_map<Kmer, NodePosPair, KmerHash> KmerNodeTable;

// ============================================================================
// SORT DECLARATIONS
// ============================================================================

bool sortNodeByLength(const NodeID& left, const NodeID& right);

// ============================================================================
// DIJKSTRA AUXILIARY CLASSES
// ============================================================================

class PathDFS {
public:
        NodeID nodeID;
        size_t length;

        /**
         * Default constructor
         */
        PathDFS(NodeID nodeID, size_t length) :
                nodeID(nodeID), length(length) {};
};

struct PathDFSComp {

        /**
         * Compare two paths (by length)
         */
        bool operator()(const PathDFS& f, const PathDFS& s) {
                return f.length >= s.length;
        }
};

// ============================================================================
// GRAPH STATISTICS
// ============================================================================

class GraphStats
{
private:
        size_t numNodes;        // number of (valid) nodes in the graph
        size_t numArcs;         // number of (valid) arcs in the graph
        size_t N50;             // N50 of the nodes
        size_t totMargLength;   // total marginal length of all nodes

public:
        /**
         * Default constructor
         */
        GraphStats() : numNodes(0), numArcs(0), N50(0), totMargLength(0) {}

        /**
         * Set the graph metrics
         * @param numNodes Number of (valid) nodes in the graph
         * @param numArcs Number of (valid) arcs in the graph
         * @param N50 N50 of the nodes
         * @param totMargLength Total marginal length of all nodes
         */
        void setMetrics(size_t numNodes_, size_t numArcs_,
                        size_t N50_, size_t totMargLength_) {
                numNodes = numNodes_;
                numArcs = numArcs_;
                N50 = N50_;
                totMargLength = totMargLength_;
        }

        /**
         * Write metrics to the stout
         */
        friend std::ostream &operator<<(std::ostream &out,
                                        const GraphStats &stats);
};

// ============================================================================
// GRAPH CLASS
// ============================================================================

class DBGraph {

private:
        // ====================================================================
        // VARIABLES
        // ====================================================================

        const Settings &settings;       // settings object

        DSNode *nodes;                  // graph nodes
        Arc *arcs;                      // graph arcs

        NodeID numNodes;                // number of nodes
        NodeID numArcs;                 // number of arcs

        KmerNodeTable kmerNPPTable;     // kmer node table
        KmerSpectrum kmerSpectrum;      // kmer spectrum
        NodeChainContainer ncc;         // node chain container

#ifdef DEBUG
        std::vector<size_t> trueMult;
#endif

        // ====================================================================
        // OTHER STUFF
        // ====================================================================

        /**
         * Get the unique node extending a given node to the left
         * @param node Node to be extended (input)
         * @param leftNode Node that left-overlaps with the given node (output)
         * @return True if a unique left-overlapping node is found
         */
        bool getLeftUniqueSSNode(const SSNode &node, SSNode &leftNode) const;

        /**
         * Get the unique node extending a given node to the right
         * @param node Node to be extended (input)
         * @param rightNode Node that right-overlaps with the given node (output)
         * @return True if a unique right-overlapping node is found
         */
        bool getRightUniqueSSNode(const SSNode &node, SSNode &rightNode) const;

        /**
         * Increment the coverage of the arc that span two given kmers
         * @param left Left kmer reference
         * @param right Right kmer reference
         */
        void increaseCoverage(const NodeEndRef &left, const NodeEndRef &right);

        /**
         * Convert a vector of overlapping nodes to a string
         * @param nodeSeq A deque of overlapping nodes
         * @param output An stl string (output)
         */
        void convertNodesToString(const std::vector<NodeID> &nodeSeq,
                                  std::string &output);

        void markPairedArcs(const std::vector<NodeID>& seq);

public:
        static const DBGraph* graph;

        /**
         * Get the graph statistics
         * @return Graph statistics
         */
        GraphStats getGraphStats();

        /**
         * Get the first valid node
         * @param seed Node identifier to start from
         * @return Node identifier for first valid node, zero otherwise
         */
        NodeID getFirstValidNode(NodeID seed = 1);

        /**
         * Default constructor
         */
        DBGraph(const Settings& settings);

        /**
         * Destructor
         */
        ~DBGraph();

        /**
         * Get a const-reference to settings
         * @return A const-reference to settings
         */
        const Settings& getSettings() const {
                return settings;
        }

        // ====================================================================
        // SOLUTIONCOMP.CPP PUBLIC
        // ====================================================================

        // ====================================================================
        // GRAPH CONSTRUCTION
        // ====================================================================

        /**
         * Get the number of nodes
         * @return The number of nodes
         */
        NodeID getNumNodes() const {
                return numNodes;
        }

        /**
         * Get the number of arcs
         * @return The number of arcs
         */
        NodeID getNumArcs() const {
                return numArcs;
        }

        /**
         * Clear all nodes and arcs in this graph
         */
        void clear() {
                delete [] nodes;
                delete [] arcs;
                nodes = NULL;
                arcs = NULL;
                numNodes = numArcs = 0;
        }

        /**
         * Get a reference to a double stranded node, given the nodeID
         * @param nodeID Identifier for the node
         * @return Reference to the node
         */
        DSNode& getDSNode(NodeID nodeID) const {
                assert(nodeID > 0 && nodeID <= numNodes);
                return nodes[nodeID];
        }

        /**
         * Get a single stranded node, given the nodeID
         * @param nodeID Identifier for the node
         * @return A Single Stranded node
         */
        const SSNode getSSNode(NodeID nodeID) const {
                NodeID uNodeID = abs(nodeID);
                assert(uNodeID != 0 && uNodeID <= numNodes);
                return SSNode(nodes + uNodeID, nodeID);
        }

        /**
         * Get a single stranded node, given the nodeID
         * @param nodeID Identifier for the node
         * @return A Single Stranded node
         */
        SSNode getSSNode(NodeID nodeID) {
                NodeID uNodeID = abs(nodeID);
                assert(uNodeID != 0 && uNodeID <= numNodes);
                return SSNode(nodes + uNodeID, nodeID);
        }


        void sanityCheck();

        /**
         * Write a cytoscape graph of the current graph
         * @param filename Filename of the cytoscape graph
         * @param seedNodeID Seed node identifier
         * @param maxDepth Maximum depth (in terms of number of nodes)
         */
        void writeCytoscapeGraph(const std::string& filename,
                                 NodeID seedNodeID = 0, size_t maxDepth = 0) const;

        /**
         * Create a graph from file
         * @param nodeFilename Filename for the nodes
         * @param arcFilename Filename for the arcs
         * @param metaDataFilename Filename for the metadata
         */
        void createFromFile(const std::string& nodeFilename,
                            const std::string& arcFilename,
                            const std::string& metaDataFilename);

        /**
         * Simply write graph to file
         */
        void writeGraph(const std::string& nodeFilename,
                        const std::string& arcFilename,
                        const std::string& metaDataFilename) const;

        /**
         * Load graph to file (binary version)
         * @param nodeFilename node filename
         * @param arcFilename arc filename
         * @param metaDataFilename Metadata filename
         */
        void loadGraphBin(const std::string& nodeFilename,
                          const std::string& arcFilename,
                          const std::string& metaDataFilename);

        /**
         * Write graph to file (binary version)
         * @param nodeFilename node filename
         * @param arcFilename arc filename
         * @param metaDataFilename Metadata filename
         */
        void writeGraphBin(const std::string& nodeFilename,
                           const std::string& arcFilename,
                           const std::string& metaDataFilename) const;

        /**
         * FIXME: USE A FILENAME
         */
        void writeGraphFasta() const;

        // ====================================================================
        // CORRECTGRAPH.CPP (STAGE 4 ROUTINES)
        // ====================================================================
private:
        /**
         * Remove a node from the graph
         * @param nodeID node identifier
         **/
        void removeNode(NodeID nodeID);

        /**
         * Detach two nodes
         * @param leftID Identifier of the left node
         * @param rightID Identifier of the right node
         */
        void detachNode(NodeID leftID, NodeID rightID);

        /**
         * Given a node and a list of previous nodes, extract the path
         * @param dstID Identifier of the last node
         * @param prevNode Vector containing previous nodes
         * @return A vector with nodeIDs from srcID to dstID
         */
        std::vector<NodeID> getPath(NodeID dstID,
                                    const std::vector<NodeID>& prevNode) const;

        /**
         * Get the first and the last node of a path that can be deleted
         * @param path Input path sequence
         * @param first First node that can be deleted (output)
         * @param last Last node that can be deleted (output)
         */
        void getUniquePath(const std::vector<NodeID>& path,
                           size_t& first, size_t& last);

        /**
         * Get the average kmer coverage for a given path
         * @param path The path under consideration
         * @return The average kmer coverage
         */
        double getPathAvgKmerCov(const std::vector<NodeID>& path);

        /**
         * Remove the nodes in a path
         * @param path Vector containing nodeIDs to be removed
         */
        void removePath(const std::vector<NodeID>& path);

        /**
         * Given two parallel paths, check if a bubble can be popped
         * @param pathA Reference to the first path
         * @param pathB Reference to the second path
         * @return True if at least one node was deleted
         */
        bool handleParallelPaths(const std::vector<NodeID>& pathA,
                                 const std::vector<NodeID>& pathB,
                                 double covCutoff);

        /**
         * Given a seed node, find parallel paths that originate from this node
         * @param srcID Node identifier for the seed node
         * @param visited Empty vector to use as temporary storage
         * @param prevNode Pre-allocated vector to use as temporary storage
         * @param nodeColor Pre-allocated vector to use as temporary storage
         * @param covCutoff Coverage cuttoff
         * @param maxMargLength Maximum marginal search depth
         * @param maxNodesVisited Maximum number of visited nodes
         * @return True if at least one node was deleted
         */
        bool bubbleDetection(NodeID srcID, std::vector<NodeID>& visited,
                             std::vector<NodeID>& prevNode,
                             std::vector<NodeID>& nodeColor,
                             double covCutoff,
                             size_t maxMargLength, size_t maxNodesVisited);

        /**
         * Generic bubble detection (parallel paths of arbitrary length)
         * @param nodeID Node identifier
         * @param covCutoff Maximum coverage of a node to delete
         * @param maxLength Maximum marginal length of a parallel path
         * @return True if at least one node was removed
         */
        bool bubbleDetection(NodeID nodeID, double covCutoff,
                             size_t maxMargLength);

public: // FIXME !!! (private)
        /**
         * Graph correctoin based on flow conservation
         * @param nodeID Identifier for the source node
         * @param covCutoff Coverage cutoff
         */
        bool flowCorrection(NodeID nodeID, double covCutoff);

public:
        /**
         * Invalidate tips from a graph
         * @param covCutoff Maximum coverage of a node to delete
         * @param maxLength Maximum marginal length of a node to delete
         * @return True if at least one node was removed
         */
        bool clipTips(double covCutoff, size_t maxMargLength);

        /**
         * Concatentate linear paths
         * @return True if at least one node was merged
         **/
        bool concatenateNodes();

        /**
         * Generic bubble detection (parallel paths of arbitrary length)
         * @param covCutoff Maximum coverage of a node to delete
         * @param maxLength Maximum marginal length of a parallel path
         * @return True if at least one node was removed
         */
        bool bubbleDetection(double covCutoff, size_t maxMargLength);

        /**
         * Graph correction based on flow conservation
         */
        bool flowCorrection();

#ifdef DEBUG
        /**
         * Set the true node multiplicity
         * @param trueMult_ The true node multiplicity vector
         */
        void setTrueNodeMultiplicity(const std::vector<size_t> trueMult_) {
                trueMult = trueMult_;
        }
#endif

        // ====================================================================
        // COVERAGE.CPP (STAGE 3 ROUTINES)
        // ====================================================================
private:
        /**
         * Parse a buffer of reads and store kmers in temporary buffers per thread
         * @param thisThread Identifier for the current thread
         * @param readBuffer Input read bufferp
         * @param table Table to store the
         */
        void parseReads(size_t thisThread,
                        const std::vector<std::string>& readBuffer,
                        KmerCountTable& table);

        /**
         * Entry routine for worker thread for counting the kmers
         * @param myID Unique threadID
         * @param input LibraryContainer with the input libraries
         * @param table KmerCountTable to store the kmer counts
         */
        void kmerCountWorkerThread(size_t threadID, LibraryContainer* inputs,
                                   KmerCountTable* table);

        /**
         * Compute the kmer coverage of all nodes that have a probability
         * less than p of belonging to a Poisson error model
         * @param errLambda Error model lambda (Poisson model)
         * @param p Desired probability
         * @return Average k-mer coverage estimate
         */
        double getInitialKmerCovEstimate(double errLambda = 2,
                                         double p = 0.01) const;

public:
        /**
         * Count the node (kmer and nodestart) and arc multiplicity
         * @param tempdir Path to which to write temporary files
         * @param inputs Container with input libraries
         */
        void generateKmerSpectrum(const std::string& tempdir,
                                  LibraryContainer &inputs);

        /**
         * Load the kmer spectrum fit from disk
         * @param Filename File name
         */
        void loadKmerSpectrumFit(const std::string& filename) {
                kmerSpectrum.loadSpectrumFit(filename);
                std::cout << kmerSpectrum << std::endl;
        }

        /**
         * Get coverage cutoff
         * @return The coverage cutoff
         */
        double getCovCutoff() const {
                return kmerSpectrum.getCovCutoff();
        }

        /**
         * Given an observed kmer coverage, return the expected multiplicity
         * @return The expected multiplicity
         */
        int getExpMult(double obsKmerCov) const;

        /**
         * Get the probability of observing a certain number of kmers
         * @param obsKmerCov The observed number of kmers
         * @param mult The multiplicity of the node
         */
        double getObsProb(unsigned int obsCov, unsigned int mult) const;

        /**
         * Get the probability of observing a certain number of kmers
         * @param obsKmerCov The observed number of kmers
         * @param ML The marginal length of the node
         * @param mult The multiplicity of the node
         */
        double getObsProbLog(double obsKmerCov, int ML, unsigned int mult) const;

        /**
         * Get the average k-mer coverage
         * @return The average k-mer coverage
         */
        double getAvgKmerCov() const {
                return kmerSpectrum.getAvgKmerCov();
        }

        /**
         * Write a node file containing length and kmer coverage
         * @param filename Output filename
         */
        void writeNodeFile(const std::string& filename) const;

        // ====================================================================
        // KMERNPP.CPP (KMER - NODE-POSITON PAIR TABLE)
        // ====================================================================
public:
        /**
         * Build a table to relate Kmers and NodePositionPairs
         */
        void buildKmerNPPTable();

        /**
         * Destroy the kmer-NPP table
         */
        void destroyKmerNPPTable();

        /**
         * Insert a < kmer , npp > tuple in the KmerNPP table
         */
        bool insertNPP(const Kmer& kmer, NodePosPair npp);

        /**
         * Find Kmer in the Kmernodetable
         */
        NodePosPair findNPP(Kmer const &kmer) const;

        /**
         * Reverse-complement a NodePosPair element
         * @param npp NodePosPair element to reverse-complement
         **/
        void revCompNPP(NodePosPair& npp) const;

        /**
         * Populate the KmerCountTable
         * @param kmerCountTable Table to populate
         */
        void buildKmerCountTable(KmerCountTable& kmerCountTable);

        /**
         * Check whether two NodePosPairs are consecutive in the graph
         * @param left Left NodePosPair
         * @param right Right NodePosPair
         * @return True if the NodePosPairs are consecutive
         */
        bool consecutiveNPP(NodePosPair& left, NodePosPair& right) const;

        // ====================================================================
        // REDUCTION.CPP (STAGE 6)
        // ====================================================================

        /**
         * Load a node chain container from disk
         * @param filename File name
         */
        void loadNodeChainContainer(const std::string& filename,
                                    std::vector<NodeChain>& nodeChain);

        void performReduction(const NodeChain& reduction);
        void performReductionTypeA(const NodeChain& reduction);
        void performReductionTypeB(const NodeChain& reduction);
};

#endif
