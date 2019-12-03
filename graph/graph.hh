#include <memory>
#include <cassert>
#include <map>

//Saving me refactoring time later
#define debug_assert assert

namespace joost {
namespace graph {

//Types//
using node_id_int = int;
////////

//class NodeBase;
//class EdgeBase;
//class GraphBase;

class NodeBase {
public:
  NodeBase( node_id_int id ) :
    node_id_( id )
  {}

  void set_node_id( node_id_int setting ){
    node_id_ = setting;
  }

  node_id_int node_id() const {
    return node_id_;
  }
private:
  node_id_int node_id_;
};

//template< typename NodeType >
class EdgeBase {
public:
  node_id_int
  get_other_node_id( node_id_int i ) const {
    if( first_node_->node_id() == i ){
      return second_node_->node_id();
    } else {
      debug_assert( second_node_->node_id() == i );
      return first_node_->node_id();
    }
  }

  node_id_int first_node_id() const {
    return first_node_->node_id();
  }

  NodeBase * first_node() {
    return first_node_;
  }

  NodeBase const * first_node() const {
    return first_node_;
  }

  void
  set_first_node( NodeBase * node ){
    first_node_ = node;
  }

  node_id_int second_node_id() const {
    return second_node_->node_id();
  }

  NodeBase * second_node() {
    return second_node_;
  }

  NodeBase const * second_node() const {
    return second_node_;
  }

  void
  set_second_node( NodeBase * node ){
    second_node_ = node;
  }

private:
  NodeBase * first_node_;
  NodeBase * second_node_;
};

//TODO map-based concept
template< template< typename Key, typename Value > class MapType = std::map >
class MapNodeBase : public NodeBase {

using NodeType = MapNodeBase< MapType >;

public:
  MapNodeBase( node_id_int id ) :
    NodeBase( id )
  {}

  EdgeBase *
  get_edgebase( node_id_int other_node_id ){
    auto iter = edge_map_.find( other_node_id );
    if( iter == edge_map_.end() ){
      return nullptr;
    }
    EdgeBase * const ptr = * iter;
    return ptr;
  }

  EdgeBase const *
  get_edgebase( node_id_int other_node_id ) const {
    auto iter = edge_map_.find( other_node_id );
    if( iter == edge_map_.end() ){
      return nullptr;
    }
    EdgeBase * const ptr = * iter;
    return ptr;
  }

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
    return std::dynamic_pointer_cast< EdgeType >( get_edgebase( other_node_id ) );
  }

  template< typename EdgeType >
  EdgeType const *
  get_edge( node_id_int other_node_id ) const {
    return std::dynamic_pointer_cast< EdgeType >( get_edgebase( other_node_id ) );
  }

  void
  register_new_edge( node_id_int other_node_id, EdgeBase * ptr ){
    edge_map_[ other_node_id ] = ptr;
  }

  void
  deregister_edge( EdgeBase const * ptr ){
    node_id_int const other_node_id = ptr->get_other_node_id( node_id() );
    edge_map_.erase( other_node_id );
  }

private:
  using EdgeMapType = MapType< node_id_int, EdgeBase * >;
  EdgeMapType edge_map_;

public: //Edge iteration
  class UpperEdgeIterator {
  public:
    UpperEdgeIterator( NodeType * const node ):
      node_( node )
    {
      iter_ = node_->edge_map_.begin();
      advance_as_needed();
    }

    UpperEdgeIterator & operator ++(){
      ++iter_;
      advance_as_needed();
      return *this;
    }

    EdgeType & operator * () {
      return * iter_->second;
    }

    EdgeType const & operator * () const {
      return * iter_->second;
    }

    static UpperEdgeIterator end() {
      UpperEdgeIterator iter( true );
      return iter;
    }

    bool operator == ( UpperEdgeIterator const & other ){
      //1. check for end
      if( other.node_ == nullptr ){
	return node_ == nullptr;
      }

      //2. check for equality
      return node_ == other.node_ && iter_ == other.iter_;
    }

  private:
    void
    advance_as_needed(){
      while( iter_ != node_->edge_map_.end() && 
	iter_.second->get_other_node_id( node_->node_id() ) < node_->node_id()
      ){
	++iter_;
      }

      if( iter_ == node_->edge_map_.end() ){
	node_ = nullptr;
	iter_ = 0;
      }
    }

    NodeType * node_;
    EdgeMapType::iterator iter_;

    //This ctor is only used to contruct a dummy end()
    //Using bool so I don't break the rule of zero
    UpperEdgeIterator( bool ) :
      node_( nullptr ),
      iter_( 0 )
    {}
  };
  friend class UpperEdgeIterator;
};

template< template< typename T > class ContainerType >
class FlatNodeBase : public NodeBase {

using NodeType = FlatNodeBase< ContainerType >;

public:
  FlatNodeBase( node_id_int id ) :
    NodeBase( id )
  {}

  EdgeBase *
  get_edgebase( node_id_int other_node_id ){
    auto && pred = [=]( EdgeBase * edge ) -> bool {
      return edge->get_other_node_id( node_id() ) == other_node_id;
    };
    auto iter = std::find_if( edges_.begin(), edges_.end(), pred );
    EdgeBase * const ptr = * iter;
    return ptr;
  }

  EdgeBase const *
  get_edgebase( node_id_int other_node_id ) const {
    auto && pred = [=]( EdgeBase * edge ) -> bool {
      return edge->get_other_node_id( node_id() ) == other_node_id;
    };
    auto iter = std::find_if( edges_.begin(), edges_.end(), pred );
    EdgeBase * const ptr = * iter;
    return ptr;
  }

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
    return std::dynamic_pointer_cast< EdgeType >( get_edgebase( other_node_id ) );
  }

  template< typename EdgeType >
  EdgeType const *
  get_edge( node_id_int other_node_id ) const {
    return std::dynamic_pointer_cast< EdgeType >( get_edgebase( other_node_id ) );
  }

  void
  register_new_edge( node_id_int other_node_id, EdgeBase * ptr ){
    edges_.push_back( ptr );
  }

  void
  deregister_edge( EdgeBase const * ptr ){
    auto && pred = [=]( EdgeBase * edge ) -> bool {
      //there's probably a better check for pointer equality here
      return edge->get_other_node_id( node_id() ) == ptr->get_other_node_id( node_id() );
    };
    auto iter = std::find_if( edges_.begin(), edges_.end(), pred );    
    edges_.erase( iter );
  }

private:
  using EdgeContainerType = ContainerType< EdgeBase * >;
  EdgeContainerType edges_;

public: //Edge iteration
  class UpperEdgeIterator {
  public:
    UpperEdgeIterator( NodeType * const node ):
      node_( node )
    {
      iter_ = node_->edges_.begin();
      advance_as_needed();
    }

    UpperEdgeIterator & operator ++(){
      ++iter_;
      advance_as_needed();
      return *this;
    }

    EdgeType & operator * () {
      return * iter_;
    }

    EdgeType const & operator * () const {
      return * iter_;
    }

  private:
    void
    advance_as_needed(){
      while( iter_ != node_->edges_.end() && 
	iter_->get_other_node_id( node_->node_id() ) < node_->node_id()
      ){
	++iter_;
      }
    }

    NodeType * node_;
    EdgeContainerType::iterator iter_;
  };
  friend class UpperEdgeIterator;

};


//The graph owns all nodes and edges
template<
  typename NodeType,
  typename EdgeType
  //template< typename K, typename V > class MapType
>
class Graph {
using NodePtr = std::unique_ptr< NodeType >;
using EdgePtr = std::unique_ptr< EdgeType >;
using NodeIDPair = std::pair< node_id_int, node_id_int >;

using GraphType = Graph< NodeType, EdgeType >;

public:
  Graph(
    int num_nodes,
    node_id_int first_node_index = 0
  ){
    for( int i = 0; i < num_nodes; ++i ){
      node_id_int const index = first_node_index + i;
      nodes_[ index ] = std::make_unique< NodeType >( index );
    }
  }

  EdgeType *
  add_edge( node_id_int const node1, node_id_int const node2 ){
    NodeIDPair pair;
    if( node1 < node2 ){
      pair.first = node1;
      pair.second = node2;
    } else {
      pair.first = node2;
      pair.second = node1;
    }
    
    auto iter = edges_[ pair ];
    EdgePtr ptr = * iter;
    ptr = std::make_unique< EdgeType >();

    {
      auto first_node_iter = nodes_[ pair.first ];
      debug_assert( first_node_iter != nodes_.end() );
      NodeType * first_node = * first_node_iter;
      ptr->set_first_node( first_node );
      first_node->register_new_edge( pair.second, ptr );
    }

    {
      auto second_node_iter = nodes_[ pair.second ];
      debug_assert( second_node_iter != nodes_.end() );
      NodeType * second_node = * second_node_iter;
      ptr->set_second_node( second_node );
      second_node->register_new_edge( pair.first, ptr );
    }
  }

  void
  remove_edge( EdgeType * edge ){
    debug_assert( edge != nullptr );
    nodes_[ edge->first_node_id() ]->deregister_edge( edge );
    nodes_[ edge->second_node_id() ]->deregister_edge( edge );

    auto iter = edges_.find( std::make_pair( edge->first_node_id(), edge->second_node_id() ) );
    debug_assert( iter != edges_.end() );
    edges_.erase( iter );
  }

  void add_node( node_id_int node_id ){
    debug_assert( nodes_.at( node_id ) == nodes_.end() );
    nodes_[ index ] = std::make_unique< NodeType >( node_id );
  }

  unsigned int num_nodes() const {
    return nodes_.size();
  }

  unsigned int num_edges() const {
    return edges_.size();
  }

  NodeType * get_node( node_id_int node_id ) {
    return nodes_.at( node_id );
  }

  NodeType const * get_node( node_id_int node_id ) const {
    return nodes_.at( node_id );
  }

  EdgeType *
  find_edge( node_id_int node_id1, node_id_int node_id2 ){
    if( node_id1 > node_id2 ) return find_edge( node_id2, node_id1 );

    auto iter = edges_.find( std::make_pair( node_id1, node_id2 ) );
    if( iter == edges_.end() ){
      return nullptr;
    } else {
      return iter;
    }
  }

  EdgeType const *
  find_edge( node_id_int node_id1, node_id_int node_id2 ) const {
    if( node_id1 > node_id2 ) return find_edge( node_id2, node_id1 );

    auto iter = edges_.find( std::make_pair( node_id1, node_id2 ) );
    if( iter == edges_.end() ){
      return nullptr;
    } else {
      return iter;
    }
  }

private:
  using NodeMapType = std::map< node_id_int, NodePtr >;
  NodeMapType nodes_;

  using EdgeMapType = std::map< NodeIDPair, EdgePtr >;
  EdgeMapType edges_;

//Edge iteration
public:
  class EdgeIterator {
  public:
    EdgeIterator( GraphType * const graph ) :
      graph_( graph )
    {
      current_node_ = graph_->nodes_.begin();
      if( ! graph_->nodes_.empty() ){
	current_edge_ = current_node_->second->upper_edge_begin();
      } else {
	current_edge_ = NodeType::UpperEdgeIterator::end();
      }
    }

    //double-check this is prefix
    EdgeIterator &
    operator ++(){
      if( current_edge_ == NodeType::UpperEdgeIterator::end() ){
	//then we are already at the end, do nothing
      }

      ++current_edge_;

      if( current_edge_ == NodeType::UpperEdgeIterator::end() ){
	//then we need to move on to the next node
	++current_node_;
	if( current_node_ != graph_->nodes_.end() ){
	  current_edge_ = current_node_->second->upper_edge_begin();
	}//else we are done
      }

      return *this;
    }

    EdgeType & operator * (){
      return * current_edge_;
    }

    EdgeType & operator * () const {
      return * current_edge_;
    }

    static EdgeIterator end() {
      EdgeIterator iter( true );
      return iter;
    }

    bool operator == ( EdgeIterator const & other ) const {
      //1. check for end
      if( other.current_edge_ == NodeType::UpperEdgeIterator::end() ){
	return current_edge_ == NodeType::UpperEdgeIterator::end();
      }

      //2. check for equality
      return graph_ == other.graph_ &&
	current_node_ = other.current_node_ &&
	current_edge_ = other.current_edge_;
    }

  private:
    GraphType * graph_;
    NodeMapType::iterator current_node_;
    NodeType::UpperEdgeIterator current_edge_;
    
    //This ctor is only used to contruct a dummy end()
    //Using bool so I don't break the rule of zero
    EdgeIterator( bool ) :
      current_node_ = NodeType::UpperEdgeIterator::end();
    {}

  };
  friend class EdgeIterator;

  EdgeIterator edge_list_begin() {
    return EdgeIterator( *this );
  }

  EdgeIterator edge_list_end() {
    return EdgeIterator::end();
  }
};

} //graph
} //joost
