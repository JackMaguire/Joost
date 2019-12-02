#include "graph.hh"

#include <iostream>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

using namespace joost::graph;

template< typename E, typename N >
void
run(){
  constexpr int NODECOUNT = 10;
  Graph< N, E > g( NODECOUNT );

  std::cout << g.num_nodes() << " nodes and " << g.num_edges() << " edges." << std::endl;

  for( int i = 0; i < NODECOUNT; ++i ){
    for( int j = i + 1; j < NODECOUNT; j += i + 1 ){
      g.add_edge( i, j );
    }
  }

  std::cout << g.num_nodes() << " nodes and " << g.num_edges() << " edges." << std::endl;
}

int main(){

  run< Graph, EdgeBase, FlatNodeBase< std::vector > >();
  //run< Graph, EdgeBase, FlatNodeBase< std::list > >();
  //run< Graph, EdgeBase, MapNodeBase< std::map > >();
  //run< Graph, EdgeBase, MapNodeBase< std::unordered_map > >();

}
