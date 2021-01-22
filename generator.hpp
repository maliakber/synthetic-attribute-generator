#ifndef GENERATE_HPP
#define GENERATE_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>

using attr_t = double;

namespace generate
{

/*
 * random_equal
 *
 *  returns a random value x \in [min,max]
 */
template<typename T>
T random_equal(T const& min, T const& max)
{
    T x = (T) rand() / RAND_MAX;
    return x * (max - min) + min;
}

/*
 * random_peak
 *
 *  Returns a random value x \in [min,max) as sum of dim equally
 *  distributed random values.
 */
template<typename T>
T random_peak(T const& min, T const& max, int const& dim)
{
    T sum = 0;
    for (auto d = 0; d < dim; d++)
        sum += random_equal(min, max);
    sum /= dim;
    return sum * (max - min) + min;
}

/*
 * random_normal
 *
 *  Returns a normally distributed random value x \in (med-var,med+var)
 *  with E[x] = med.
 *
 *  NOTE: This implementation works well if the random values returned by
 *  the underlaying random_equal are sufficiently independent.
 */
template<typename T>
T random_normal(T const& med, T const& var, int const& dim)
{
    return random_peak(med - var, med + var, dim);
}

/*
 * is_vector_ok
 *
 *  returns 1 iif all x_i \in [0,1]
 */
template<typename T>
bool is_vector_ok(T const& min, T const& max, std::vector<T> const& x)
{
    for(auto val : x)
    {
        if (val < min || val > max)
            return false;
    }
    return true;
}

/*
 * generate_indep
 *
 *  Generate count vectors x[0..dim-1] with x_i \in [0,1] independently
 *  and equally distributed and outputs them as a std::vector.
 */
std::vector<std::vector<attr_t> > generate_indep(int count, int dim)
{
    std::vector<std::vector<attr_t> > attributes(count);
    std::vector<attr_t> cur_attr(dim);
    attr_t min = 0, max = 1;
    std::generate_n(attributes.begin(), count, [&dim, &cur_attr, &min, &max](){
        std::generate_n(cur_attr.begin(), dim, [&min, &max](){
            return random_equal(min, max);
        });
        return cur_attr;
    });
    return attributes;
}

/*
 * generate_corr
 *
 *  Generates count vectors x[0..dim-1] with x_i \in [0,1].
 *  The x_i are correlated, i.e. if x is high in one dimension
 *  it is likely that x is high in another.
 */
std::vector<std::vector<attr_t> > generate_corr(int count, int dim)
{
    std::vector<std::vector<attr_t> > attributes(count);
    std::vector<attr_t> cur_attr(dim);
    attr_t min = 0, max = 1;
    std::generate_n(attributes.begin(), count, [&dim, &cur_attr, &min, &max](){
        do
        {
            attr_t v = random_peak(min, max, dim);
            attr_t l = v <= 0.5 ? v : 1.0 - v;
            std::fill(cur_attr.begin(), cur_attr.end(), v);
            for (auto d = 0; d < dim; d++)
            {
                attr_t h = random_normal(min, l, dim);
                cur_attr[d] += h;
                cur_attr[(d + 1) % dim] -= h;
            }
        } while (!is_vector_ok(min, max, cur_attr));
        
        return cur_attr;
    });
    
    return attributes;
}

/*
 * generate_anti
 *
 *  Generates count vectors x[0..dim-1] with x_i in [0,1], such that
 *  if x is high in one dimension it is likely that x is low in another.
 */
std::vector<std::vector<attr_t> > generate_anti(int count, int dim)
{
    std::vector<std::vector<attr_t> > attributes(count);
    std::vector<attr_t> cur_attr(dim);
    attr_t min = 0, max = 1;
    
    std::generate_n(attributes.begin(), count, [&dim, &cur_attr, &min, &max](){
        do
        {
            attr_t  v = random_normal(0.5, 0.25, dim);
            attr_t  l = v <= 0.5 ? v : 1.0 - v;
            std::fill(cur_attr.begin(), cur_attr.end(), v);
            for (auto d = 0; d < dim; d++)
            {
                attr_t h = random_equal(-l, l);
                cur_attr[d] += h;
                cur_attr[(d + 1) % dim] -= h;
            }
        } while (!is_vector_ok(min, max, cur_attr));
        
        return cur_attr;
    });
    
    return attributes;
}

// sample: auto res = generate::generate_attr(5, 2, 'a', 31);
std::vector<std::vector<attr_t> > generate_attr(int count, int dim, char dist, int seed)
{
    std::srand(seed); // updating the initial seed
    if(dist == 'i')
        return generate_indep(count, dim);
    else if(dist == 'c')
        return generate_corr(count, dim);
    else if(dist == 'a')
        return generate_anti(count, dim);
    else
        return std::vector<std::vector<attr_t>>(count, std::vector<attr_t>(dim, 0));
}
};

#endif // GENERATE_HPP
