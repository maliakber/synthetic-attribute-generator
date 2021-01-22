#include "generator.hpp"
#include "timing.hpp"

int main()
{
    char dist_type[] = {'i', 'c', 'a'}; // i = independent, c = correlated, a = anti-correlated
    auto num_tests  = 100u;
    auto input_size = 1u << 16;
    auto dim_size = 4u;
    auto dist_size = 3u;

    std::srand(std::time(0));
    
    std::vector<std::pair<int, char> > test_cases(num_tests);
    std::generate_n(test_cases.begin(), num_tests, [&dist_type, &dim_size, &dist_size](){
        auto dim = std::rand() % dim_size + 1; // adding 1, to exclude 0 dimension
        auto dist = std::rand() % dist_size;
        return std::make_pair(dim, dist_type[dist]);
    });

    auto const run_time = benchmark::benchmark([&input_size](std::pair<int, char> const& data){
        auto res = generate::generate_attr(input_size, data.first, data.second, std::time(0));
        return res.size();
    }, test_cases);

    std::cout << "Average time (us): " << run_time << std::endl;

    return 0;
}