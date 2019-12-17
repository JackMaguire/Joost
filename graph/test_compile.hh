#include "graph.hh"

#include <iostream>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

using namespace joost::graph;

template< template< typename N, typename E > class G, typename N, typename E = EdgeBase >
//template< template< typename N, typename E > class G, template<class EdgeType, template<class T> class ContaierType> class N, typename E = EdgeBase >
void
run(){
  constexpr int NODECOUNT = 10;
  G< N, E > g( NODECOUNT );

  std::cout << g.num_nodes() << " nodes and " << g.num_edges() << " edges." << std::endl;

  for( int i = 0; i < NODECOUNT; ++i ){
    for( int j = i + 1; j < NODECOUNT; j += i + 1 ){
      g.add_edge( i, j );
    }
  }

  std::cout << g.num_nodes() << " nodes and " << g.num_edges() << " edges." << std::endl;
}

int main(){

  //run< MapGraph, FlatNodeBase >();
  run< MapGraph, FlatNodeBase< EdgeBase, std::vector > >();
  //run< Graph, FlatNodeBase< std::list > >();
  //run< Graph, MapNodeBase< std::map > >();
  //run< Graph, MapNodeBase< std::unordered_map > >();

}
