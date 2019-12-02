#include <memory>
#include <cassert>
#include <map>

//Types//
using node_id_int = int;
////////

class NodeBase;
class EdgeBase;
class GraphBase;

//template< typename NodeType >
class EdgeBase {
public:
  node_id_int
  get_other_node_id( node_id_int i ) const {
    if( first_node->node_id() == i ){
      return second_node->node_id();
    } else {
      assert( second_node->node_id() == i );
      return first_node->node_id();
    }
  }

private:
  NodeBase * first_node;
  NodeBase * second_node;
};

class NodeBase {
public:
  NodeBase( node_id_int id ) :
    node_id_ = id
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

//TODO map-based concept
template< typename MapType >
class MapNodeBase : public NodeBase {
public:
  MapNodeBase( node_id_int id ) :
    NodeBase( id )
  {}

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
    //EdgeBase * const ptr = 
    auto iter = edge_map_.find( other_node_id );
    if( iter == edge_map_.end() ){
      return nullptr;
    }
    EdgeBase * const ptr = * iter;
    EdgeType * const ptr2 = std::dynamic_pointer_cast< EdgeType >( ptr );
    return ptr2;
  }

private:
  MapType< node_id_int, EdgeBase * > edge_map_;
}

template< typename ContainerType >
class FlatNodeBase : public NodeBase {
public:
  FlatNodeBase( node_id_int id ) :
    NodeBase( id )
  {}

  template< typename EdgeType >
  EdgeType *
  get_edge( node_id_int other_node_id ){
    auto && pred = [=]( EdgeBase * edge ) -> bool {
      return edge->get_other_node_id( node_id() ) == other_node_id;
    };
    auto iter = std::find_if( edges_.begin(), edges_.end(), pred );
    EdgeBase * const ptr = * iter;
    EdgeType * const ptr2 = std::dynamic_pointer_cast< EdgeType >( ptr );
    return ptr2;
  }

private:
  ContainerType< EdgeBase * > edges_;
}

//The graph owns all nodes and edges
template<
  typename NodeType,
  typename EdgeType,
  typename MapType = std::map
>
class GraphBase {
using NodePtr = std::unique_ptr< NodeType >;
using EdgePtr = std::unique_ptr< EdgeType >;
using NodeIDPair = std::pair< node_id_int, node_id_int >;

public:
  GraphBase(
    int num_nodes,
    node_id_int first_node_index = 0
  ){
    for( int i = 0; i < num_nodes; ++i ){
      node_id_int const index = first_node_index + i;
      nodes_[ index ] = std::make_unique( index );
    }
  }

private:
  MapType< node_id_int, NodePtr > nodes_;
  MapType< NodeIDPair, EdgePtr > edges_;
};
