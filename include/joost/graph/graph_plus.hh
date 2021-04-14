#include "graph.hh"

namespace joost {
namespace graph {

template< typename Core, typename DataType >
class DataAdder : public Core {
public:
  template< typename ...Args >
  DataAdder( Args ...args ) :
    Core( args )
  {}

  DataType & data() {
    return data_;
  }

  DataType const & data() const {
    return data_;
  }
private:
  DataType data_;
};

template< typename NodeType, typename DataType >
class NodeWithData : DataAdder< NodeType, DataType > {
  NodeWithData( node_id_int id ) :
    DataAdder( id )
  {}

};


} //graph
} //joost
