#pragma once

#include <memory>
#include <cassert>

namespace joost {

// classes and enums

class DDFRNode;
class DDFRCache;

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
using DDFRNodePtr = std::unique_ptr< DDFRNode< class StateType, class OutcomeType, N_POSSIBLE_MOVES > >;

template< class OutcomeType, unsigned int N_POSSIBLE_MOVES >
struct RecursionSolution {
  std::array< int, N_POSSIBLE_MOVES > moves;
  OutcomeType * outcome; //NO OWNERSHIP!!!

  RecursionSolution(){
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

struct NeverStopEarly {

  template< class OutcomeType >
  static
  bool
  stop(){
    return false;
  }

};

/*enum class NodeType {
  UNINITIALIZED = 0,
  NONLEAF,
  LEAF_TERM, //leaf node that cannot furthur propogate (a true leaf)
  LEAF_UNFINISHED //this is only a leaf because we previously decided not to run more
};*/

template< class StateType, class OutcomeType, unsigned int N_POSSIBLE_MOVES >
class DDFRNode {
  using NodePtr_t = DDFRNodePtr< StateType, OutcomeType, N_POSSIBLE_MOVES >;

  friend class DDFRCache;

private:
  //DATA

  std::array< NodePtr_t, N_POSSIBLE_MOVES > data_ = {};
  StateType outgoing_state_;
  OutcomeType outcome_;
  bool has_been_initialized_ = false;

  //   Node State
  //NodeType node_type_ = NodeType::UNINITIALIZED;
  //unsigned int depth_calculated_;


public:
  //METHODS
  template< class Forecaster >
  void
  initialize( StateType const incoming_state, unsigned int const move ){
    //assumes static void forecast( StateType const & incoming_state, unsigned int const move, StateType &, OutcomeType & )
    Forecaster::forecast( incoming_state, move, outgoing_state_, outcome_ );
    has_been_initialized_ = true;
  }

  template< class Forecaster,
	    class OutcomeRanker,
	    class StopEarlyFailure = NeverStopEarly,
	    class StopEarlySuccess = NeverStopEarly >
  RecursionSolution< N_POSSIBLE_MOVES >
  sample_to_depth_static(
    unsigned int const depth,
    GrowingArray< N_POSSIBLE_MOVES > const & moves
  ){
    if( depth == 0 ) return;

    using RecSolution = RecursionSolution< N_POSSIBLE_MOVES >;
    
    RecSolution best_solution;

    for( unsigned int i = 0; i < N_POSSIBLE_MOVES; ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->initialize<Forecaster>(outgoing_state_, i);
      } else {
	assert( data_[ i ]->has_been_initialized_ );
      }

      OutcomeType const & outcome = data_[ i ]->outcome_;

      if( StopEarlyFailure::stop( outcome ) ){
	continue;
      }

      GrowingArray< N_POSSIBLE_MOVES > moves_copy = moves;
      moves_copy.push_back( i );

      if( StopEarlySuccess::stop( outcome ) ){
	RecSolution result;
	result.moves = moves_copy;
	result.outcome = & data_[ i ]->outcome_;
	return result;
      }
     
      RecSolution const rec_solution =
	data_[ i ]->sample_to_depth_static<
	  Forecaster,
	OutcomeRanker,
	StopEarlyFailure,
	StopEarlySuccess >( depth - 1, moves_copy );

      if( OutcomeRanker::first_is_better( rec_solution, best_solution ) ){
	//relatively affordable copy
	best_solution = rec_solution;
      }

    }
    
    return best_solution;
  }  

  template< class Forecaster,
	    class OutcomeRanker,
	    class StopEarlyFailure = NeverStopEarly,
	    class StopEarlySuccess = NeverStopEarly >
  RecursionSolution< N_POSSIBLE_MOVES >
  sample_to_depth_dynamic(
    unsigned int const depth,
    StopEarlyFailure const & sef,
    StopEarlySuccess const & ses,
    GrowingArray< N_POSSIBLE_MOVES > const & moves
  ){
    if( depth == 0 ) return;

    using RecSolution = RecursionSolution< N_POSSIBLE_MOVES >;
    
    RecSolution best_solution;

    for( unsigned int i = 0; i < N_POSSIBLE_MOVES; ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->initialize<Forecaster>(outgoing_state_, i);
      } else {
	assert( data_[ i ]->has_been_initialized_ );
      }

      OutcomeType const & outcome = data_[ i ]->outcome_;

      if( StopEarlyFailure::stop( outcome ) ){
	continue;
      }

      GrowingArray< N_POSSIBLE_MOVES > moves_copy = moves;
      moves_copy.push_back( i );

      if( StopEarlySuccess::stop( outcome ) ){
	RecSolution result;
	result.moves = moves_copy;
	result.outcome = & data_[ i ]->outcome_;
	return result;
      }
     
      RecSolution const rec_solution =
	data_[ i ]->sample_to_depth_dynamic<
	  Forecaster,
	OutcomeRanker,
	StopEarlyFailure,
	StopEarlySuccess >( depth - 1, sef, ses, moves_copy );

      if( OutcomeRanker::first_is_better( rec_solution, best_solution ) ){
	//relatively affordable copy
	best_solution = rec_solution;
      }

    }
    
    return best_solution;
  }  

};

template< class StateType, class OutcomeType,
	  unsigned int N_POSSIBLE_MOVES >
class DDFRCache {
  using NodePtr_t = DDFRNodePtr< StateType, OutcomeType, N_POSSIBLE_MOVES >;

  std::array< NodePtr_t, N_POSSIBLE_MOVES > data_ = {};
  StateType current_state_;

  bool initial_state_has_been_set_ = false;

public:
  DDFRCache( StateType const & state ):
    current_state_( state )
  {}

  void
  set_state( StateType const & state ){
    current_state_ = state;
    initial_state_has_been_set_ = true;
  }

  void
  clear(){
    data_.fill( nullptr );
    initial_state_has_been_set_ = false;
  }

  void
  reset( StateType const & state ){
    data_.fill( nullptr );
    current_state_ = state;
  }

  void
  register_move( unsigned int const move ){
    assert( data_[ move ] != nullptr );
    data_ = std::move( data_[ move ]->data );
    current_state_ = data_[ move ]->outgoing_state_;
  }

  template< class Forecaster,
	    class OutcomeRanker,
	    class StopEarlyFailure = NeverStopEarly,
	    class StopEarlySuccess = NeverStopEarly >
  RecursionSolution< N_POSSIBLE_MOVES >
  sample_to_depth_static( unsigned int const depth ){

    assert( initial_state_has_been_set_ );

    using RecSolution = RecursionSolution< N_POSSIBLE_MOVES >;

    RecSolution best_solution;

    GrowingArray< N_POSSIBLE_MOVES > moves;

    for( unsigned int i = 0; i < N_POSSIBLE_MOVES; ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->initialize< Forecaster >(current_state_, i);
      } else {
	assert( data_[ i ]->has_been_initialized_ );
      }

      OutcomeType const & outcome = data_[ i ]->outcome_;

      if( StopEarlyFailure::stop( outcome ) ){
	continue;
      }

      if( StopEarlySuccess::stop( outcome ) ){
	moves.push_back( i );
	RecSolution result;
	result.moves = moves.data;
	result.outcome = & data_[ i ]->outcome_;
	return result;
      }

      GrowingArray< N_POSSIBLE_MOVES > moves_copy = moves;
      moves_copy.push_back( i );

      RecSolution const rec_solution =
	data_[ i ]->sample_to_depth_static<
	  Forecaster,
	OutcomeRanker,
	StopEarlyFailure,
	StopEarlySuccess >( depth - 1 );

      if( OutcomeRanker::first_is_better( rec_solution, best_solution ) ){
	//relatively affordable copy
	best_solution = rec_solution;
      }

    }

    return best_solution;
  }

  template< class Forecaster,
	    class OutcomeRanker,
	    class StopEarlyFailure,
	    class StopEarlySuccess >
  RecursionSolution< N_POSSIBLE_MOVES >
  sample_to_depth_dynamic(
    unsigned int const depth,
    StopEarlyFailure const & sef,
    StopEarlySuccess const & ses
  ){

    assert( initial_state_has_been_set_ );

    using RecSolution = RecursionSolution< N_POSSIBLE_MOVES >;

    RecSolution best_solution;

    GrowingArray< N_POSSIBLE_MOVES > moves;

    for( unsigned int i = 0; i < N_POSSIBLE_MOVES; ++i ){
      if( data_[ i ] == nullptr ){
	data_[ i ] = std::make_unique< DDFRNode >();
	data_[ i ]->initialize< Forecaster >(current_state_, i);
      } else {
	assert( data_[ i ]->has_been_initialized_ );
      }

      OutcomeType const & outcome = data_[ i ]->outcome_;

      if( sef.stop( outcome ) ){
	continue;
      }

      if( ses.stop( outcome ) ){
	moves.push_back( i );
	RecSolution result;
	result.moves = moves.data;
	result.outcome = & data_[ i ]->outcome_;
	return result;
      }

      GrowingArray< N_POSSIBLE_MOVES > moves_copy = moves;
      moves_copy.push_back( i );

      RecSolution const rec_solution =
	data_[ i ]->sample_to_depth_dynamic<
	  Forecaster,
	OutcomeRanker,
	StopEarlyFailure,
	StopEarlySuccess >( depth - 1, sef, ses, moves_copy );

      if( OutcomeRanker::first_is_better( rec_solution, best_solution ) ){
	//relatively affordable copy
	best_solution = rec_solution;
      }

    }

    return best_solution;
  }


};

} //namespace joost
