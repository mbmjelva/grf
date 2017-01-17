#include "ForestSerializer.h"
#include "utility.h"


void ForestSerializer::serialize(std::ostream& stream, Forest forest) {
  size_t num_trees = forest.get_trees().size();
  stream.write((char*) &num_trees, sizeof(num_trees));

  for (std::shared_ptr<Tree> tree : forest.get_trees()) {
    tree_serializer.serialize(stream, tree);
  }

  observation_serializer.serialize(stream, forest.get_observations());
}

Forest ForestSerializer::deserialize(std::istream& stream) {
  size_t num_trees;
  stream.read((char*) &num_trees, sizeof(num_trees));
  std::vector<std::shared_ptr<Tree>> trees;

  for (int i = 0; i < num_trees; i++) {
    std::shared_ptr<Tree> tree = tree_serializer.deserialize(stream);
    trees.push_back(tree);
  }

  Observations observations = observation_serializer.deserialize(stream);
  return Forest(trees, observations);
}
