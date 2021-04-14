#pragma once

#include <memory>
#include <cassert>

namespace joost {

// classes and enums

class DDFRNode;

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
using DDFRNodePtr = std::unique_ptr< DDFRNode< class StateType, class OutcomeType, N_POSSIBLE_MOVES > >;

enum class NodeType {
  UNINITIALIZED = 0,
  NONLEAF,
  LEAF_TERM, //leaf node that cannot furthur propogate (a true leaf)
  LEAF_UNFINISHED //this is only a leaf because we previously decided not to run more
};

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
class DDFRNode {
private:
  //DATA

  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data_;
  StateType outgoing_state_;
  OutcomeType outcome_;

  //   Node State
  NodeType node_type_ = NodeType::UNINITIALIZED;
  unsigned int depth_calculated_;


public:
  //METHODS
  template< class Forecaster >
  void
  run_forecast( StateType const incoming_state, unsigned int const move ){
    Forecaster::forecast( incoming_state, move, outgoing_state_, outcome_ );
  }
};

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
class DDFRCache {
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > data_;
  StateType current_state_;

public:

  void
  register_move( unsigned int const move ){
    assert( data_[ move ] != nullptr );
    assert( data_[ move ].node_type != NodeType::UNINITIALIZED );
    data_ = std::move( data_[ move ].data );
  }

  void
  reset( StateType const & state ){
    data_.fill( nullptr );
    current_state_ = state;
  }

  template< class Forecaster >
  void
  sample_to_depth( unsigned int const depth ){
    for( unsigned int i = 0; i < data_.size(); ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->run_forecast< Forecaster >( i );
      }

      data_[ i ]->
    }
  }

};

} //namespace joost
