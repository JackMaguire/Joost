#pragma once

#include <memory>
#include <cassert>

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
struct DDFRNode {
  NodeType node_type = NodeType::UNINITIALIZED;
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data;

  unsigned int depth_calculated;
};

template< unsigned int N_POSSIBLE_MOVES >
class DDFRCache {
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data_;

public:

  void
  register_move( unsigned int const move ){
    assert( data_[ move ] != nullptr );
    assert( data_[ move ].node_type != NodeType::UNINITIALIZED );
    data_ = std::move( data_[ move ].data );
  }

  void reset(){
    data_.fill( nullptr );
  }

};

} //namespace joost
