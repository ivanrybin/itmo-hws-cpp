/*
    Polynomial.
    Ivan Rybin 2019.
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <iterator>


struct polynomial {
    explicit polynomial(const std::string& poly_str) {                          // polynomial(string)
        poly_parse(poly_str);
    }

    template<typename T> 
    const T operator()(const T& base) const {                                   // operator()

        T sum = 0;
        sum   = *coefficients.rbegin();

        for (auto coef = coefficients.rbegin() + 1; coef < coefficients.rend(); ++coef) {
            sum *= base;
            sum += *coef;
        }
        return sum;
    }

    uint32_t& at(size_t pos) {                                                  // at()
        if (pos >= coefficients.size()) {
            coefficients.resize(pos + 1, 0);
        }
        return coefficients[pos];
    }

    const uint32_t& at(size_t pos) const {                                      // at() const
        return coefficients[pos];
    }

private:

    void poly_parse(const std::string& poly) {                                  // poly_parse(string)
        size_t begin = poly.find_first_of('^');
        size_t end   = poly.find_first_of('+');

        size_t p_deg = std::stoull(poly.substr(begin + 1, end - begin - 1));

        coefficients.resize(p_deg + 1, 0);

        std::string part {};

        size_t  coef = 0;
        size_t  deg  = 0;                       // x^0              - ok
        size_t  star = 0;                       // 42*x^0           - ok
        size_t  mid  = 0;                       // x^3 + x^2 + x^0  - ok

        begin = 0;
        end   = 0;

        while(end != std::string::npos) {
            end   = poly.find_first_of('+', begin);
            part  = poly.substr(begin, end - begin);
            begin = end + 1;

            star  = part.find_first_of('*');
            mid   = part.find_first_of('^');

            (star == std::string::npos) ? coef = 1 
                                        : coef = stoul(part.substr(0, mid + 1));

            deg   = stoul(part.substr(mid + 1, std::string::npos));

            coefficients[deg] = coef; 
        }
    }

    std::vector<uint32_t> coefficients = {};

}; // polynomial
