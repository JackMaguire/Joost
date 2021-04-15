#pragma once

#include <memory>
#include <cassert>

namespace joost {

// classes and enums

class DDFRNode;
class DDFRCache;

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
using DDFRNodePtr = std::unique_ptr< DDFRNode< class StateType, class OutcomeType, N_POSSIBLE_MOVES > >;

template< unsigned int N_POSSIBLE_MOVES >
struct RecursionOutcome {
  std::array< int, N_POSSIBLE_MOVES > moves;
  bool found_solution = false;

  RecursionOutcome(){
    moves.fill( -1 );
  }

};

template< unsigned int N_POSSIBLE_MOVES >
struct GrowingArray {
  std::array< int, N_POSSIBLE_MOVES > data;
  unsigned int next_index = 0;

  GrowingArray(){
    data.fill( -1 );
  }

  void push_back( unsigned int const value ){
    data[ next_index ] = value;
    ++next_index;
  }
};

enum class NodeType {
  UNINITIALIZED = 0,
  NONLEAF,
  LEAF_TERM, //leaf node that cannot furthur propogate (a true leaf)
  LEAF_UNFINISHED //this is only a leaf because we previously decided not to run more
};

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
class DDFRNode {
  using NodePtr_t = DDFRNodePtr< StateType, OutcomeType, N_POSSIBLE_MOVES >;

  friend class DDFRCache;

private:
  //DATA

  std::array< NodePtr_t, N_POSSIBLE_MOVES > data_;
  StateType outgoing_state_;
  OutcomeType outcome_;

  //   Node State
  NodeType node_type_ = NodeType::UNINITIALIZED;
  unsigned int depth_calculated_;


public:
  //METHODS
  template< class Forecaster >
  void
  initialize( StateType const incoming_state, unsigned int const move ){
    //assumes static void forecast( StateType const incoming_state, unsigned int const move, StateType &, OutcomeType & )
    Forecaster::forecast( incoming_state, move, outgoing_state_, outcome_ );
  }
};

/*namespace {
template< unsigned int N_POSSIBLE_MOVES, class Forecaster >
void
sample_to_depth(
  std::array< DDFRNodePtr, N_POSSIBLE_MOVES > & data
  unsigned int const depth
){
  for( unsigned int i = 0; i < data_.size(); ++i ){
    if( data_[ i ] == nullptr ){
      data_[ i ] = std::make_unique< DDFRNode >();
      data_[ i ]->initialize< Forecaster >( i );
    }
    
    //data_[ i ]->
  }
}
}*/

struct NeverStopEarly {

  template< class OutcomeType >
  static
  bool
  stop(){
    return false;
  }

};

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
class DDFRCache {
  using NodePtr_t = DDFRNodePtr< StateType, OutcomeType, N_POSSIBLE_MOVES >;

  std::array< NodePtr_t, N_POSSIBLE_MOVES > data_;
  StateType current_state_;

public:
  void
  set_state( StateType const & state ){
    current_state_ = state;
  }

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

  template< class Forecaster, class StopEarlyFailure = NeverStopEarly, class StopEarlySuccess = NeverStopEarly,  >
  RecursionOutcome< N_POSSIBLE_MOVES >
  sample_to_depth( unsigned int const depth ){

    using RecOutcome = RecursionOutcome< N_POSSIBLE_MOVES >;

    RecOutcome best_outcome;
    
    GrowingArray moves;

    for( unsigned int i = 0; i < data_.size(); ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->initialize< Forecaster >( i );
      }

      OutcomeType const & outcome = data_[ i ]->outcome_;

      if( StopEarlyFailure::stop( outcome ) ){
	continue;
      }

      if( StopEarlySuccess::stop( outcome ) ){
	moves.push_back( i );
	RecOutcome result;
	result.moves = moves.data;
	result.found_solution = true;
	return result;
      }

      GrowingArray moves_copy = moves;
      moves_copy.push_back( i );

      data_[ i ]->sample_to_depth( depth - 1 );
    }

    return outcome;
  }

};

} //namespace joost
