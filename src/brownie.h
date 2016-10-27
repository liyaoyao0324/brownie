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

#ifndef BROWNIE_H
#define BROWNIE_H
#include "graph.h"
#include "settings.h"
#include "library.h"
#include <vector>

class Brownie {

private:
        Settings settings;              // settings object
        LibraryContainer libraries;     // read libraries

public:
        /**
         * Constructor
         * @param argc Argument count
         * @param args Argument strings
         */
        Brownie(int argc, char ** args);

        /**
         * Execute brownie
         */
        void run();

        /**
         * Run the assemble module
         */
        void assembleModule();

        /**
         * Run the compare
         */
        void compareModule();

        /**
         * Run the visualize module
         */
        void visualizeModule();

        /**
         * Execute stage one
         */
        void stageOne();

        /**
         * Execute stage two
         */
        void stageTwo();

        /**
         * Execute stage three
         */
        void stageThree();

        /**
         * Execute stage four
         */
        void stageFour();

        /**
         * Execute stage four
         */
        void stageFive();

        /**
         * Execute stage six
         */
        void stageSix();

        /**
         * Get the node filename
         * @return String containing the node filename
         */
        std::string getNodeFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("nodes.stage") + stageStr;
        }

        /**
         * Get the arc filename
         * @return String containing the arc filename
         */
        std::string getArcFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("arcs.stage") + stageStr;
        }

        /**
         * Get the metadata filename
         * @return String containing the metadata filename
         */
        std::string getMetaDataFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("metadata.stage") + stageStr;
        }

        /**
         * Get the binary node filename
         * @return String containing the binary node filename
         */
        std::string getBinNodeFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("nodes.bin.stage") + stageStr;
        }

        /**
         * Get the binary arc filename
         * @return String containing the binary arc filename
         */
        std::string getBinArcFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("arcs.bin.stage") + stageStr;
        }

        /**
         * Get the true multiplicity filename
         * @return String containing the true multiplicity filename
         */
        std::string getTrueMultFilename(int filestage) const {
                char stageStr[4];
                sprintf(stageStr, "%d", filestage);
                return settings.addTempDirectory("truemult.stage") + stageStr;
        }

        /**
         * Get the kmer filename
         * @return The kmer filename
         */
        std::string getKmerFilename() const {
                return settings.addTempDirectory("kmers.stage1");
        }

        /**
         * Get the spectrum filename
         * @return The spectrum filename
         */
        std::string getSpectrumFilename() const {
                return settings.addTempDirectory("spectrum.txt");
        }

        /**
         * Get the spectrum GNUplot filename
         * @return The spectrum GNUplot filename
         */
        std::string getSpectrumGNUPlotFilename() const {
                return settings.addTempDirectory("spectrum.gnu");
        }

        /**
         * Get the spectrum fit filename
         * @return The spectrum fit filename
         */
        std::string getSpectrumFitFilename() const {
                return settings.addTempDirectory("spectrum.fit");
        }

        /**
         * Check if it is necessary to perform stage one
         * @return True of false
         */
        bool stageOneNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 1;

                /*for (size_t i = 0; i < container.size(); i++) {
                        const ReadLibrary &input = container[i];
                        if (!ReadLibrary::fileExists(input.getMetadataFilename()))
                                return true;
                        if (!ReadLibrary::fileExists(input.getIsolatedFilename()))
                                return true;
                        if (!ReadLibrary::fileExists(input.getPairedFilename()))
                                return true;
                }*/

                return !Util::fileExists(getKmerFilename());
        }

        /**
         * Check if it is necessary to perform stage two
         * @return True of false
         */
        bool stageTwoNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 2;

                if (!Util::fileExists(getNodeFilename(2)))
                        return true;
                if (!Util::fileExists(getArcFilename(2)))
                        return true;
                return !Util::fileExists(getMetaDataFilename(2));
        }

        /**
         * Check if it is necessary to perform stage three
         * @return True or false
         */
        bool stageThreeNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 3;

                if (!Util::fileExists(getNodeFilename(3)))
                        return true;
                if (!Util::fileExists(getArcFilename(3)))
                        return true;
                return !Util::fileExists(getMetaDataFilename(3));
        }

        /**
         * Check if it is necessary to perform stage four
         * @return True or false
         */
        bool stageFourNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 4;

                if (!Util::fileExists(getNodeFilename(4)))
                        return true;
                if (!Util::fileExists(getArcFilename(4)))
                        return true;
                return !Util::fileExists(getMetaDataFilename(4));
        }

        /**
         * Check if it is necessary to perform stage five
         * @return True of false
         */
        bool stageFiveNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 5;

                for (size_t i = 0; i < libraries.getSize(); i++) {
                        const ReadLibrary &input = libraries.getInput(i);
                        if (!Util::fileExists(input.getOutputFileName()))
                                return true;
                        if (!Util::fileExists(input.getNodeChainFilename()))
                                return true;
                }

                return false;
        }

        /**
         * Check if it is necessary to perform stage six
         * @return True of false
         */
        bool stageSixNecessary() const {
                if (settings.getRunSpecificStage() != 0)
                        return settings.getRunSpecificStage() == 6;

                return true;
        }

        /**
         * In case Brownie skip  stage 4 or 5, it writes
         * the output contigs in fasta format.
         * By defult Brownie writes stage 3 output in binary format and read in fasta format in stage 5
         *
         */
        void writeGraphFasta();
};

#endif
