#include "graph.hh"

#include <iostream>

#include <vector>
#include <map>
#include <unordered_map>

using namespace joost::graph;

template< typename E, typename N >
void
run(){
  Graph< N, E > g( 5 );
  std::cout << g.num_nodes() << " nodes and " << g.num_edges() << " edges." << std::endl;
}

int main(){

  //run< Graph, EdgeBase, MapNodeBase< std::map > >();
  //run< Graph, EdgeBase, MapNodeBase< std::unordered_map > >();

  run< Graph, EdgeBase, FlatNodeBase< std::vector > >();

}
