#include "utility.h"
#include "InstrumentalRelabelingStrategy.h"

InstrumentalRelabelingStrategy::InstrumentalRelabelingStrategy():
  split_regularization(0) {}

InstrumentalRelabelingStrategy::InstrumentalRelabelingStrategy(double split_regularization):
  split_regularization(split_regularization) {}

std::unordered_map<size_t, double> InstrumentalRelabelingStrategy::relabel_outcomes(
     const Observations& observations,
    std::vector<size_t> &node_sampleIDs) {

  // Prepare the relevant averages.
  size_t num_samples = node_sampleIDs.size();

  double total_outcome = 0.0;
  double total_treatment = 0.0;
  double total_instrument = 0.0;

  for (size_t sampleID : node_sampleIDs) {
    total_outcome += observations.get(Observations::OUTCOME)[sampleID];
    total_treatment += observations.get(Observations::TREATMENT)[sampleID];
    total_instrument += observations.get(Observations::INSTRUMENT)[sampleID];
  }

  double average_outcome = total_outcome / num_samples;
  double average_treatment = total_treatment / num_samples;
  double average_instrument = total_instrument / num_samples;
  double average_regularized_instrument = (1 - split_regularization) * average_instrument
    + split_regularization * average_treatment;

  // Calculate the treatment effect.
  double numerator = 0.0;
  double denominator = 0.0;

  for (size_t sampleID : node_sampleIDs) {
    double outcome = observations.get(Observations::OUTCOME)[sampleID];
    double treatment = observations.get(Observations::TREATMENT)[sampleID];
    double instrument = observations.get(Observations::INSTRUMENT)[sampleID];
    double regularized_instrument = (1 - split_regularization) * instrument + split_regularization * treatment;

    numerator += (regularized_instrument - average_regularized_instrument) * (outcome - average_outcome);
    denominator += (regularized_instrument - average_regularized_instrument) * (treatment - average_treatment);
  }

  if (equalDoubles(denominator, 0.0, 1.0e-10)) {
    return std::unordered_map<size_t, double>(); // Signals that we should not perform a split.
  }

  double local_average_treatment_effect = numerator / denominator;

  // Create the new outcomes.
  std::unordered_map<size_t, double> relabeled_observations;

  for (size_t sampleID : node_sampleIDs) {
    double response = observations.get(Observations::OUTCOME)[sampleID];
    double treatment = observations.get(Observations::TREATMENT)[sampleID];
    double instrument = observations.get(Observations::INSTRUMENT)[sampleID];
    double regularized_instrument = (1 - split_regularization) * instrument + split_regularization * treatment;

    double residual = (response - average_outcome) - local_average_treatment_effect * (treatment - average_treatment);
    relabeled_observations[sampleID] = (regularized_instrument - average_regularized_instrument) * residual;
  }
  return relabeled_observations;
}