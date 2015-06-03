#warning deprecated

/* XBoost: Ada-Boost and Friends on Haar/ICF/HOG Features, Library and ToolBox
 *
 * Copyright (c) 2008-2014 Paolo Medici <medici@ce.unipr.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "abversion.h"
#include "Utils.h"
#include "IntegralImageDataSet.h"

#include "HaarClassifier.h"
#include "Trainer/AdaBoostTrainer.h"

#include "Trainer/DecisionTreeLearner.h"

#include "FeatureGenerator/HaarFeatureGenerators.h"

#include "Oracle/DecisionStumpOracle.h"
#include "Oracle/BayesianStumpOracle.h"
#include "Oracle/NaiveDecisionStumpOracle.h"

#include "Trainer/DiscreteAdaBoost.h"

#include <mpi.h>

int main(int argc, char *argv[]) {
  int numprocs, rank, namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  const char *datasetA, *datasetB;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name(processor_name, &namelen);

  printf("Process %d on %s out of %d\n", rank, processor_name, numprocs);

  int number;
  if (rank == 0) {
    //// SERVER
    
    datasetA = argv[1];
    datasetB = argv[2];
    
    IntegralImageDataSet< Pattern<IntegralImageData> > training_set;
    // load images
    training_set.LoadPatterns(datasetA,  1);
    training_set.LoadPatterns(datasetB, -1);
    std::cout << training_set.Size() << " patterns correctly loaded\n";
    
    number = 12;
    // SERIALIZE DATA and BROADCAST
    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
     
     
    // INITIALIZE STEP: SEND FEATURES
     
    // WAIT ALL CHILD COMPLETE PROCESSING
    
    // FIND BEST, UPDATE WEIGTS
    
    // CONTINUE
     
  } else {
    
    // RECEIVE PATTERN
    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // RECEIVE FEATURE RANGE
    
    // 
//     MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
//              MPI_STATUS_IGNORE);
    printf("Process 1 received number %d from process 0\n",
            number);
  }
  
  MPI_Finalize();
}