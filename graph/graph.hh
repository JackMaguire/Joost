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

class NodeBase;
class EdgeBase;
class GraphBase;

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

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
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
  MapType< node_id_int, EdgeBase * > edge_map_;
};

template< template< typename T > class ContainerType >
class FlatNodeBase : public NodeBase {
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

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
    return std::dynamic_pointer_cast< EdgeType >( get_edgebase( other_node_id ) );
  }

  void
  register_new_edge( node_id_int other_node_id, EdgeBase * ptr ){
    edges_.push_back( ptr );
  }

  void
  deregister_edge( EdgeBase const * ptr ){
    auto && pred = [=]( EdgeBase * edge ) -> bool {
      //preobably better check for equality here
      return edge->get_other_node_id( node_id() ) == ptr->get_other_node_id( node_id() );
    };
    auto iter = std::find_if( edges_.begin(), edges_.end(), pred );    
    edges_.erase( iter );
  }

private:
  ContainerType< EdgeBase * > edges_;
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

private:
  //How can we template this to be vector-indexed? Special SFINAE?
  std::map< node_id_int, NodePtr > nodes_;
  std::map< NodeIDPair, EdgePtr > edges_;
};

} //graph
} //joost
