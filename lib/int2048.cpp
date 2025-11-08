#include "int2048.h"
#include <string>


namespace sjtu {
constexpr long long BASE = 1e9;
constexpr int base_len = 9;
constexpr long long NTT_P1 = 998244353;
constexpr long long NTT_P2 = 1004535809;
constexpr long long NTT_P3 = 469762049;
constexpr long long INV_P1_MOD_P2 = 669690699;
constexpr long long INV_P1P2_MOD_P3 = 354521948;
constexpr long long NTT_G = 3;
std::vector<int> swap_index;

void int2048::addData(const int2048 &other) {
    long long carry = 0;
    for(int i = 0; i < std::max(data_.size(), other.data_.size()); i++) {
        if(i == data_.size()) {
            data_.push_back(0);
        }
        long long other_digit = (i < other.data_.size() ? other.data_[i] : 0);
        data_[i] += other_digit + carry; 
        if(data_[i] >= BASE) {
            data_[i] -= BASE;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    if(carry) {
        data_.push_back(carry);
    }
}

void int2048::minusData(const int2048 &other) {
    int cmp = compareData(other);
    if(cmp == -1) {
        sign_ = -sign_;
    }
    long long carry = 0;
    for(int i = 0; i < std::max(data_.size(), other.data_.size()); i++) {
        if(i == data_.size()) {
            data_.push_back(0);
        }
        long long other_digit = (i < other.data_.size() ? other.data_[i] : 0);
        data_[i] = cmp != -1 ? data_[i] - other_digit + carry : other_digit - data_[i] + carry; 
        if(data_[i] < 0) {
            data_[i] += BASE;
            carry = -1;
        } else {
            carry = 0;
        }
    }
}



int2048::int2048():sign_(1) {
    data_.push_back(0);
}

int2048::int2048(long long num) {
    unsigned long long abs_num;
    if(num < 0) {
        abs_num = -num;
        sign_ = -1;
    } else {
        sign_ = 1;
        abs_num = num;
    }
    while(abs_num) {
        data_.push_back(abs_num % BASE);
        abs_num /= BASE;
    }
    normalize();
}
int2048::int2048(const std::string &num) {
    if(!num.empty() && num[0] == '-') {
        sign_ = -1;
    } else {
        sign_ = +1;
    }
    for(int i = num.length() - 1; i >= 0; i -= base_len) {
        long long digit = 0;
        for(int j = std::min(i, base_len - 1); j >= 0; j--) {
            if(num[i - j] >= '0' && num[i - j] <= '9') {
                digit = digit * 10 + num[i - j] - '0';
            } else {
                if(num[i - j] != '-' || num[i - j] == '-' && i - j != 0) {
                    throw "this is not a number";
                }
            }
        }
        data_.push_back(digit);
    }
    normalize();
}

int2048::int2048(const int2048 &other):sign_(other.sign_), data_(other.data_) {
    normalize();
}

double int2048::ToDouble() const{
  double num = 0;
  double base = 1;
  for(int i = 0; i < data_.size(); i++, base *= BASE) {
    num += data_[i] * base;
  }
  return num;
}

std::string int2048::ToString() const{
  std::string num;
  if(sign_ == -1) {
    num += "-";
  }
  for(int i = data_.size() - 1; i >= 0; i--) {
    num += std::to_string(data_[i]);
  }
  return num;
}

bool int2048::ToBool() const{
  if(data_.size() == 1 && data_[0] == 0) {
    return 0;
  }
  return 1;
}

void int2048::read(const std::string &num) {
    //std::cerr << "begin" << std::endl;
    if(!num.empty() && num[0] == '-') {
        sign_ = -1;
    } else {
        sign_ = +1;
    }
    data_.clear();
    for(int i = num.length() - 1; i >= 0; i -= base_len) {
        long long digit = 0;
        for(int j = std::min(i, base_len - 1); j >= 0; j--) {
            if(num[i - j] >= '0' && num[i - j] <= '9') {
                digit = digit * 10 + num[i - j] - '0';
            } else {
                if(num[i - j] != '-' || num[i - j] == '-' && i - j != 0) {
                    throw "this is not a number";
                }
            }
        }
        data_.push_back(digit);
    }
    normalize();
    //std::cerr << "end" << std::endl;
}

void int2048::normalize() {
    while(data_.size() > 1 && data_.back() == 0) {
        data_.pop_back();
    }
    if(data_.size() == 1 && data_.back() == 0) {
        sign_ = +1;
    }
    if(data_.empty()) {
        data_.push_back(0);
    }
}

void int2048::print() const{
    if(sign_ == -1) {
        printf("-");
    }
    for(int i = data_.size() - 1; i >= 0; i--) {
        if(i == data_.size() - 1) {
            printf("%lld", data_[i]);
        } else {
            printf("%0*lld", base_len, data_[i]);
        }
    }
}

int int2048::compareData(const int2048 &other) const{
    if(data_.size() != other.data_.size()) {
        return data_.size() > other.data_.size() ? 1 : -1;
    }
    for(int i = data_.size() - 1; i >= 0; i--) {
        if(data_[i] != other.data_[i]) {
            return data_[i] > other.data_[i] ? 1 : -1;
        }
    }
    return 0;
}

int2048 &int2048::add(const int2048 &other) {
    if(sign_ == other.sign_) {
        addData(other);
    } else {
        minusData(other);
    }
    normalize();
    return *this;
}

int2048 add(int2048 num1, const int2048 &num2) {
    return num1.add(num2);
}

int2048 &int2048::minus(const int2048 &other) {
    if(sign_ == other.sign_) {
        minusData(other);
    } else {
        addData(other);
    }
    normalize();
    return *this;
}

int2048 minus(int2048 num1, const int2048 &num2) {
    return num1.minus(num2);
}

int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 new_num(*this);
    new_num.sign_ = -new_num.sign_;
    new_num.normalize();
    return new_num;
}

int2048 &int2048::operator=(const int2048 &other) {
    sign_ = other.sign_;
    data_ = other.data_;
    return *this;
}

int2048 &int2048::operator+=(const int2048 &other) {
    return this->add(other);
}

int2048 operator+(int2048 num1, const int2048 &num2) {
    return num1 += num2;
}

int2048 &int2048::operator-=(const int2048 &other) {
    return this->minus(other);
}

int2048 operator-(int2048 num1, const int2048 &num2) {
    return num1 -= num2;
}

inline long long quickPower(long long BASE, long long exp, long long NTT_P) {
    long long ans = 1;
    while(exp) {
        if(exp % 2) {
            ans = (ans * BASE) % NTT_P;
        }
        BASE = (BASE * BASE) % NTT_P;
        exp >>= 1;
    }
    return ans;
}

inline long long modInverse(long long n, long long NTT_P) {
    return quickPower(n, NTT_P - 2, NTT_P);
}

void NTT2(std::vector<long long> &a, std::vector<long long> &b, long long NTT_P) {
    int n = a.size();
    for(int len = 2, j = 0; len <= n; len <<= 1, j++) {
        long long w_base = quickPower(NTT_G, (NTT_P - 1) / len, NTT_P);
        for(int i = 0; i < n ; i += len) {
            long long w = 1;
            long long half_len = len >> 1;
            for(int j = 0; j < half_len; j++) {
                long long u = a[i + j];
                long long v = a[i + j + half_len] * w % NTT_P;
                a[i + j] = (u + v) % NTT_P;
                a[i + j + half_len] = (u - v + NTT_P) % NTT_P;
                u = b[i + j];
                v = b[i + j + half_len] * w % NTT_P;
                b[i + j] = (u + v) % NTT_P;
                b[i + j + half_len] = (u - v + NTT_P) % NTT_P;
                w = w * w_base % NTT_P;
            }
        }
    }
}

void NTT1(std::vector<long long> &a, long long NTT_P) {
    int n = a.size();
    for(int len = 2, j = 0; len <= n; len <<= 1, j++) {
        long long w_base = quickPower(NTT_G, (NTT_P - 1) / len, NTT_P);
        w_base = modInverse(w_base, NTT_P);
        for(int i = 0; i < n ; i += len) {
            long long w = 1;
            long long half_len = len >> 1;
            for(int j = 0; j < half_len; j++) {
                long long u = a[i + j];
                long long v = a[i + j + half_len] * w % NTT_P;
                w = w * w_base % NTT_P;
                a[i + j] = (u + v) % NTT_P;
                a[i + j + half_len] = (u - v + NTT_P) % NTT_P;
            }
        }
    }
    long long inverse_n = modInverse(n, NTT_P);
    for(int i = 0; i < n; i++) {
        a[i] = (a[i] * inverse_n) % NTT_P;
    }
    
}

std::vector<long long> convolution(const std::vector<long long> &a1, const std::vector<long long> &a2, long long NTT_P) {
    int n = a1.size();
    std::vector<long long> r1 = a1;
    std::vector<long long> r2 = a2;
    NTT2(r1, r2, NTT_P); 
    for(int i = 0; i < n; i++) {
        r1[i] = r1[i] * r2[i] % NTT_P;
    }
    for(int i = 0; i < n; i++) {
        if(i < swap_index[i]) {
            std::swap(r1[i], r1[swap_index[i]]);
        }
    }
    NTT1(r1, NTT_P);
    return r1;
}

__int128 garner(long long r1, long long r2, long long r3) {
    __int128_t k = (r2 - r1 + NTT_P2) * INV_P1_MOD_P2 % NTT_P2;
    __int128_t res12 = r1 + k * NTT_P1;
    __int128 P12 = (__int128)NTT_P1 * NTT_P2;
    __int128 k2 = (r3 - res12 % NTT_P3 + NTT_P3) * INV_P1P2_MOD_P3 % NTT_P3;
    return res12 + k2 * P12;
}

int2048 &int2048::operator*=(const int2048 &other) {
    sign_ = sign_ * other.sign_;
    int final_size = data_.size() + other.data_.size();
    if(final_size <= 32) {
        int n1 = data_.size(), n2 = other.data_.size();
        std::vector<long long> data(n1 + n2, 0);
        for(int i = 0; i < n1; i++) {
            for(int j = 0; j < n2; j++) {
                data[i + j] += data_[i] * other.data_[j];
                if(data[i + j] > BASE * BASE) {
                    data[i + j + 1] += data[i + j] / BASE;
                    data[i + j] %= BASE;
                }
            }
        }
        for(int i = 0; i < data.size() - 1; i++) {
            data[i + 1] += data[i] / BASE;
            data[i] %= BASE;
        }
        data_ = data;
        normalize();
        return *this;
    }
    std::vector<long long> a1 = data_;
    std::vector<long long> a2 = other.data_;
    int n;
    
    for(n = 1; n < final_size; n <<= 1) ;
    a1.resize(n);
    a2.resize(n);
    swap_index.resize(n);
    for(int i = 0, j = 0; i < n; i++) {
        swap_index[i] = j;
        if(i < j) {
            std::swap(a1[i], a1[j]);
            std::swap(a2[i], a2[j]);
        }
        int bit;
        for(bit = n >> 1; bit & j; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;
    }
    std::vector<long long> r1 = convolution(a1, a2, NTT_P1);
    std::vector<long long> r2 = convolution(a1, a2, NTT_P2);
    std::vector<long long> r3 = convolution(a1, a2, NTT_P3);
    //std::cerr << "finish conv" << std::endl;
    data_.clear();
    __int128 carry = 0;
    for(int i = 0; i < n; i++) {
        __int128 digit = garner(r1[i], r2[i], r3[i]) + carry;
        data_.push_back(digit % BASE);
        carry = digit / BASE;
    }
    while(carry) {
        data_.push_back(carry % BASE);
        carry /= BASE;
    }
    normalize();
    return *this;
}

int2048 operator*(int2048 num1, const int2048 &num2) {
    return num1 *= num2;
}

void newtonInv(int2048 &ans, const int2048 &divisor) {
    int n = divisor.data_.size();
    if(n <= 15) {
        int2048 num1(1), num2(1), num3(divisor);
        num1 <<= (n << 1);
        std::vector<int2048> num2_list, num3_list;
        while(num3 <= num1) {
            //std::cerr << num3 << std::endl;
            num2_list.push_back(num2);
            num3_list.push_back(num3);
            num2 += num2;
            num3 += num3;
        }
        for(;!num2_list.empty();num2_list.pop_back(), num3_list.pop_back()) {
            num2 = num2_list.back();
            num3 = num3_list.back();
            if(num3 <= num1) {
                ans += num2;
                num1 -= num3;
            }
        }
        return ;
    }
    int new_n = (n + 3) >> 1;
    int2048 num1(divisor);
    num1 >>= (n - new_n);
    newtonInv(ans, num1);
    ans = ((ans + ans) << (n - new_n)) - (ans * ans * divisor >> (new_n << 1));
    return ;
}

int2048 &int2048::operator/=(const int2048 &other) {
    if(other.data_.size() == 1 && other.data_[0] == 0) {
        throw "divide a zero";
    }
    int2048 num1(*this), num2(other);
    num1.sign_ = 1;
    num2.sign_ = 1;
    int shift = num1.data_.size() - (num2.data_.size() << 1);
    if(shift > 0) {
        num1 <<= shift;
        num2 <<= shift;
    }
    int n = num1.data_.size(), m = num2.data_.size();
    int2048 num2_inv;
    newtonInv(num2_inv, num2);
    int2048 ans(num1 * num2_inv >> (m << 1));
    int2048 mul_ans(ans * num2);
    while(mul_ans > num1) {
        ans -= 1;
        mul_ans -= num2;
    }
    while(mul_ans + num2 <= num1) {
        ans += 1;
        mul_ans += num2;
    }
    ans.sign_ = sign_ * other.sign_;
    if(ans.sign_ == -1 && mul_ans != num1) {
        ans -= 1;
    }
    *this = ans;
    normalize();
    return *this;
}

int2048 operator/(int2048 num1, const int2048 &num2) {
    return num1 /= num2;
}

int2048 &int2048::operator%=(const int2048 &other) {
    *this -= (*this / other) * other;
    normalize();
    return *this;
}

int2048 operator%(int2048 num1, const int2048 &num2) {
    return num1 %= num2;
}

int2048 &int2048::operator<<=(int shift) {
    int n = data_.size();
    data_.resize(n + shift);
    for(int i = n - 1; i >= 0; i--) {
        data_[i + shift] = data_[i];
    }
    for(int i = 0; i < shift; i++) {
        data_[i] = 0;
    }
    normalize();
    return *this;
}

int2048 operator<<(int2048 num1, int shift) {
    return num1 <<= shift;
}

int2048 &int2048::operator>>=(int shift) {
    int n = data_.size() - shift;
    if(n <= 0) {
        data_.clear();
        data_.push_back(0);
        return *this;
    }
    for(int i = 0; i < n; i++) {
        data_[i] = data_[i + shift];
    }
    data_.resize(n);
    normalize();
    return *this;
}

int2048 operator>>(int2048 num1, int shift) {
    return num1 >>= shift;
}

std::istream &operator>>(std::istream &in, int2048 &num) {
    std::string string_num;
    in >> string_num;
    num.read(string_num);
    return in;
}

std::ostream &operator<<(std::ostream &out, const int2048 &num) {
    if(num.sign_ == -1) {
        out << "-";
    }
    for(int i = num.data_.size() - 1; i >= 0; i--) {
        if(i == num.data_.size() - 1) {
            out << num.data_[i];
        } else {
            if(num.data_[i] == 0) {
                for(int j = 0; j < base_len; j++) {
                    out << '0';
                }
            } else {
                for(int p = BASE / 10; p > num.data_[i]; p /= 10) {
                    out << '0';
                }
                out << num.data_[i];
            }
        }  
    }
    return out;
}

bool operator==(const int2048 &num1, const int2048 &num2) {
    return num1.sign_ == num2.sign_ && num1.compareData(num2) == 0;
}

bool operator!=(const int2048 &num1, const int2048 &num2) {
    return num1.sign_ != num2.sign_ || num1.compareData(num2) != 0;
}

bool operator<(const int2048 &num1, const int2048 &num2) {
    if(num1.sign_ != num2.sign_) {
        return num1.sign_ < num2.sign_;
    }
    if(num1.sign_ == 1) {
        return num1.compareData(num2) == -1;
    }
    return num1.compareData(num2) == 1; 
}

bool operator>(const int2048 &num1, const int2048 &num2) {
    if(num1.sign_ != num2.sign_) {
        return num1.sign_ > num2.sign_;
    }
    if(num1.sign_ == 1) {
        return num1.compareData(num2) == 1;
    }
    return num1.compareData(num2) == -1;
}

bool operator<=(const int2048 &num1, const int2048 &num2) {
    if(num1.sign_ != num2.sign_) {
        return num1.sign_ < num2.sign_;
    }
    if(num1.sign_ == 1) {
        return num1.compareData(num2) != 1;
    }
    return num1.compareData(num2) != -1; 
}

bool operator>=(const int2048 &num1, const int2048 &num2) {
    if(num1.sign_ != num2.sign_) {
        return num1.sign_ > num2.sign_;
    }
    if(num1.sign_ == 1) {
        return num1.compareData(num2) != -1;
    }
    return num1.compareData(num2) != 1; 
}

} // namespace sjtu