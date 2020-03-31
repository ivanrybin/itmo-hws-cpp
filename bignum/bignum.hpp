/*
    Bignum addition and multiplication.
    Base = 2^32.
    Ivan Rybin 2019.
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <iterator>

struct bignum {
    bignum() : big_str("0"), 
               data   ({0}) {}

    bignum(uint32_t decimal) : big_str(""),                                 // bignum(uin32_t)
                               data   ({decimal}) {
        big_str = this->to_string();
    }

    bignum(const bignum& another) : big_str(another.big_str),               // bignum(bignum)
                                    data   (another.data) 	{}

    explicit bignum(const std::string& decimal_str) {                       // bignum(string)
	    big_str = decimal_str;
	    recursive_div(decimal_str);

	    zero_cleaner(data);
	}

    explicit operator bool    () const {                                    // bool
        return (data.size() > 1 || data[0] != 0) ? true : false;
    }

    explicit operator uint32_t() const {                                    // uint32_t()
        if (data.size() == 1) {
            return data[0];
        }

        uint64_t mod = data[0] + data[1] * bignum::u_base;

        return mod % (bignum::u_base + 1);
    }

    bignum& operator=(uint32_t rhs) {                                       // = uin32_t
        big_str = std::to_string(rhs);
        data.assign(1, rhs);

        return *this;
    }

    bignum& operator=(const bignum& rhs) {                                  // = bignum
        if (this == &rhs) {
            return *this;
        }

        this->data    = rhs.data;
        this->big_str = rhs.big_str;

        return *this;
    }

    bignum& operator+=(const bignum& rhs) {                                 // += bignum
       	if (rhs.data.size() == 1 && rhs.data[0] == 0) { 
            return *this; 
        }

        *this = *this + rhs;
        return *this;
    }

    bignum& operator*=(const bignum& rhs) {                                 // *= bignum
        if (rhs.data.size() == 1 && rhs.data[0] == 1) { 
            return *this; 
        }

        *this = *this * rhs;
        return *this;
    }

    std::string to_string() const {                                         // to_string()  
        std::stringstream ss;
        std::string       curr{};

        size_t d_size = data.size();

        std::vector<std::vector<uint32_t>> vec_numb(d_size, std::vector<uint32_t>{});

        for (size_t i = 0; i < d_size; ++i) {
            curr = std::to_string(data[i]);
            
            for (auto j = curr.rbegin();  j < curr.rend(); ++j) {
                vec_numb[i].push_back((*j - '0'));
            }

        }

        std::vector<uint32_t> res = *vec_numb.rbegin();
        std::vector<uint32_t> tmp{};

        for (auto numb = vec_numb.rbegin() + 1; numb < vec_numb.rend(); ++numb) {
            tmp.assign(res.size() + bignum::u_base_vec.size(), 0);            // Horner's method

            bignum_prod(tmp, res, bignum::u_base_vec, bignum::d_base);

            tmp.push_back(0);
            bignum_sum (tmp, *numb, tmp.size(), bignum::d_base);

            res = tmp;
        }

        for (auto i = res.rbegin(); i < res.rend(); ++i) {
            ss << *i;
        }

        return ss.str();
    }

    void bignum_sum(      std::vector<uint32_t>& result,                    // bignum_sum() 
                    const std::vector<uint32_t>& rhs, 
                                          size_t res_size, 
                                        uint64_t base) const
    {

        size_t   rhs_size = rhs.size();
        uint64_t sum      = 0;

        for (size_t i = 0; i < res_size; ++i) {     // O(n)
            if (i < rhs_size) {
                sum  += rhs[i];
            }
            sum      += result[i];

            result[i] = sum % base;  
            sum       = sum / base;
        }

        zero_cleaner(result);
    }

    void bignum_prod(      std::vector<uint32_t>& result,                   // bignum_prod()
                     const std::vector<uint32_t>& upper, 
                     const std::vector<uint32_t>& lower, 
                                         uint64_t base) const
    {
        
        size_t r_size = result.size();
        size_t u_size =  upper.size();
        size_t l_size =  lower.size();

        size_t      t = 0;
        size_t      k = 0;

        uint64_t  sum = 0;
        uint64_t prod = 1;

        for (size_t i = 0; i < l_size; ++i) {               // O(n^2)

            std::vector<uint64_t> buffer(u_size + 1, 0);

            for (size_t j = 0; j < u_size; ++j) {
                prod           = lower[i];                  
                prod          *= upper[j];                  

                sum            = prod % base;
                sum           += buffer[j];
                
                buffer[j]      = sum  % base;
                buffer[j + 1]  = sum  / base;
                buffer[j + 1] += prod / base;
            }

            t   = 0;
            sum = 0;

            for (size_t p = k; p < r_size; ++p) {

                if (t < u_size + 1) {
                    sum += buffer[t];
                    ++t;
                }
                sum      += result[p];

                result[p] = sum % base;
                sum       = sum / base;
            }
            ++k;
        }

        zero_cleaner(result);
    }

    // bases
    static const uint64_t       u_base     = 4294967295;
    static const uint64_t       d_base     = 10;
    const std::vector<uint32_t> u_base_vec = {5,9,2,7,6,9,4,9,2,4};

    // friends
    friend const bignum operator+(const bignum& lhs, const bignum& rhs);
    friend const bignum operator*(const bignum& lhs, const bignum& rhs);

    friend std::ostream& operator<<(std::ostream& os, const bignum& rhs);
    friend std::istream& operator>>(std::istream& is, const bignum& rhs);

    // deleted operators
    bignum&             operator--()                                     = delete;
    bignum&             operator-=(const bignum& rhs)                    = delete;
    bignum&             operator- (const bignum& rhs)                    = delete;
    friend const bignum operator- (const bignum& lhs, const bignum& rhs) = delete;

    private: // -----------------------------------------bignum private ----------------------------------------- //

    void recursive_div(const std::string& number) {                            // bignum_prod()
        if (number.length() < 10) {

            data.push_back(stoul(number));

        } else { 

            std::string res{};                                                // recursive division

            uint64_t l    = 0;
            uint64_t end  = number.length();   
            uint64_t div  = stoull(number.substr(l, 19)) / bignum::u_base;    // first 19 digits
            uint64_t mod  = stoull(number.substr(l, 19)) % bignum::u_base;

            res          += std::to_string(div);
            l = 19;
            
            uint64_t acc  = mod * 10 + (number[l] - '0');

            while (l < end) {
                
                if (acc < bignum::u_base && l + 1 != end) {
                    while (acc < bignum::u_base) {
                        res += "0";
                        acc *= 10;
                        ++l;
                        acc += number[l] - '0';
                    }
                }
                
                div  = acc / bignum::u_base;
                mod  = acc % bignum::u_base;

                res += std::to_string(div);
                ++l;

                acc = mod * 10 + (number[l] - '0');
            }
            data.push_back(mod);

            recursive_div(res);     // recursive division
        }
    } 

    void zero_cleaner(std::vector<uint32_t>& vec) const {                       // zero_cleaner()
        // deletes last zeros
        if (vec.size() > 1) {
            
            auto i = vec.rbegin();
            size_t cnt = 0;

            while (*i == 0) {
                ++cnt;
                ++i;
            }

            vec.resize(vec.size() - cnt);
        }
    }

    std::string           big_str = {};
    std::vector<uint32_t> data    = {};

}; // bignum

inline std::ostream& operator<<(std::ostream& os, const bignum& rhs) {
    os << rhs.big_str;
    return os;
}

inline std::istream& operator>>(std::istream& is, bignum& rhs) {
    std::string decimal;
    is >> decimal;
    
    rhs = bignum(decimal);
    return is;
}

inline const bignum operator+(const bignum& lhs, const bignum& rhs) {

    bignum res{};

    size_t l_size = lhs.data.size();
    size_t r_size = rhs.data.size();

    if (l_size == 1 && lhs.data[0] == 0) { return rhs; }
    if (r_size == 1 && rhs.data[0] == 0) { return lhs; }

    res.data = (l_size > r_size) ? lhs.data : rhs.data;
    res.data.push_back(0);

    size_t res_size = std::max(l_size, r_size) + 1;

    (l_size > r_size) ? res.bignum_sum(res.data, rhs.data, res_size, bignum::u_base) 
                      : res.bignum_sum(res.data, lhs.data, res_size, bignum::u_base);


    res.big_str = res.to_string();

    return res;
}

inline const bignum operator*(const bignum& lhs, const bignum& rhs) {

    bignum res{};

    size_t l_size = lhs.data.size();
    size_t r_size = rhs.data.size();

    if ((l_size == 1 && lhs.data[0] == 0) || (r_size == 1 && rhs.data[0] == 0)) { return res; }

    res.data.resize(l_size + r_size);

    (l_size > r_size) ? res.bignum_prod(res.data, lhs.data, rhs.data, bignum::u_base) 
                      : res.bignum_prod(res.data, rhs.data, lhs.data, bignum::u_base);

    res.big_str = res.to_string();

    return res;
}
