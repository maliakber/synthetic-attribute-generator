/**
 * Timing library to benchmark and comparatively analyse different implementation approaches
 */

#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <algorithm> // std::for_each()
#include <chrono>	 // timing libraries
#include <iostream>  // std::cout

namespace benchmark {

using duration = double;

template < typename Callable, typename Container >
	duration benchmark( Callable f, Container test_instances )
	{
		using output_type = decltype( f( test_instances.front() ) );
		output_type output = 0;

		auto const start_time = std::chrono::steady_clock::now();
		std::for_each( std::cbegin( test_instances )
					 , std::cend  ( test_instances )
					 , [&output, f]( auto const& v ){ output = output + f(v); } );
		auto const end_time = std::chrono::steady_clock::now();

		// do something arbitrary with output. In this case, we print it out.
		std::cout << output << std::endl;

		// return average time
		// the syntax for this library is a bit cumbersome...
		return std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count()
			/ static_cast< duration >( test_instances.size() );
	}
} // namespace benchmark

#endif // BENCHMARK_HPP