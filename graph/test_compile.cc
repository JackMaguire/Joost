#include "graph.hh"

#include <iostream>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>

using namespace joost::graph;

template< template< typename N, typename E > class G, typename N, typename E = EdgeBase >
void
run( N ){
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

template< template< typename N, typename E > class G, typename N, typename E = EdgeBase >
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

//this works with gcc version 7.4.0
//gcc test_compile.cc -std=c++1z -o test -lstdc++

int main(){

  {
    FlatNodeBase< EdgeBase, std::vector > n( node_id_int( 0 ) );
    run< MapGraph >( n );

    //run< MapGraph, FlatNodeBase< EdgeBase, std::vector > >( n );
  }

  //run< MapGraph, FlatNodeBase >();
  run< MapGraph, FlatNodeBase< EdgeBase > >();
  run< MapGraph, FlatNodeBase< EdgeBase, std::vector > >();

  run< MapGraph, FlatNodeBase< EdgeBase, std::list > >();

  run< MapGraph, MapNodeBase< EdgeBase, std::map > >();
  run< MapGraph, MapNodeBase< EdgeBase, std::unordered_map > >();

}
