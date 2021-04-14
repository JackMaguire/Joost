#pragma once

#include <memory>

namespace joost {

// classes and enums

class DDFRNode;

template< unsigned int N_POSSIBLE_MOVES >
using DDFRNodePtr = std::unique_ptr< DDFRNode< N_POSSIBLE_MOVES > >;

enum class NodeType {
  UNINITIALIZED = 0,
  NONLEAF,
  LEAF_TERM, //leaf node that cannot furthur propogate (a true leaf)
  LEAF_UNFINISHED //this is only a leaf because we previously decided not to run more
};

template< unsigned int N_POSSIBLE_MOVES >
class DDFRNode {
  NodeType node_type_ = NodeType::UNINITIALIZED;
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data_;


};

template< unsigned int N_POSSIBLE_MOVES >
class DDFRCache {
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data_;

public:
  
};

} //namespace joost
