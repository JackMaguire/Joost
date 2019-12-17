#include <vector>
#include <list>
#include <chrono>
#include <iostream>
#include <string>

//clang-8 template_template_test.cc -std=c++17 -o test -Wall -Wextra -lstdc++ && ./test
//gcc template_template_test.cc -std=c++17 -o test -Wall -Wextra -lstdc++ && ./test

template< template< typename T > class ContainerType = std::vector >
class Foo {
public:
  double
  time( int const size ) const {
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    ContainerType< int > ints;
    for( int i = 0; i < size; ++i ){
      ints.push_back( i );
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration< double > time_span = duration_cast<duration<double>>(t2 - t1);
    return time_span.count();
  }

};

int main(){
  Foo< std::vector > fvec;
  Foo< std::list > flist;
  Foo< std::basic_string > fstr;
  for( int i = 1024; i < (1<<16); i = i << 1 ){
    std::cout << i << " " << fvec.time( i ) << " " << flist.time( i ) << " " << fstr.time( i ) << std::endl;
  }
}
