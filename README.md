# KmeansCluster
Implementation of the K-Means Clustering algorithm in C++ meant to be tested with various datasets.

This program can be run from the command-line by giving the following arguments:
[dataFilename] [K] [maxIterations] [threshold] [maxRuns]
   [initializationMethod] [normalizationMethod]
   
Options for initializationMethod:
  randSelection
  randPartition
  
Options for normalizationMethod:
  minmax
  zscore
  
The internal validation measures calculated are Calinski-Harabaz and Silhouette Width

@author Ross Adams
Summer 2020
