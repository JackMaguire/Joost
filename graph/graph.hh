#include <memory>
#include <cassert>

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
class GraphBase {

};
