#ifndef GRADIENTFOREST_PREDICTIONSTRATEGY_H
#define GRADIENTFOREST_PREDICTIONSTRATEGY_H

#include <unordered_map>
#include <vector>

#include "globals.h"
#include "Observations.h"
#include "Prediction.h"
#include "PredictionValues.h"

class PredictionStrategy {
public:
  virtual size_t prediction_length() = 0;
  virtual Prediction predict(const std::map<std::string, double>& average_prediction_values,
                             const std::unordered_map<size_t, double>& weights_by_sampleID,
                             const Observations& observations) = 0;

  virtual Prediction predict_with_variance(const std::vector<std::vector<size_t>>& leaf_sampleIDs,
                                           const Observations& observations,
                                           uint ci_group_size) = 0;

  virtual bool requires_leaf_sampleIDs() = 0;
  virtual PredictionValues precompute_prediction_values(
      const std::vector<std::vector<size_t>>& leaf_sampleIDs,
      const Observations& observations) = 0;
};


#endif //GRADIENTFOREST_PREDICTIONSTRATEGY_H