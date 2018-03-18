#include <iostream>
#include <math.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

class BigNum {
public:
    vector<int> number;

    int size() const {
        return number.size();
    }

    void initialize(string);
    bool negative = false;

    BigNum();
    BigNum(string);
    BigNum(int);

    BigNum& operator=(BigNum);
    BigNum& operator=(const int&);
    BigNum& operator=(string);
    BigNum& operator=(vector<int>);
    friend ostream& operator<<(ostream&, const BigNum&);  
    friend istream& operator>>(istream&, const BigNum&);  

    BigNum sqrt();
};

BigNum::BigNum() {
    number = {};
}

BigNum::BigNum(string rhs) {
    initialize(rhs);
}

BigNum::BigNum(int rhs) {
    initialize(to_string(rhs));
}

void BigNum::initialize(string source) {
    number.resize(source.size());
    for (int i = 0; i < source.size(); i++) {
        number.at(i) = source.at(i) - '0';
    }
    return;
}

string to_string(BigNum rhs) {
    string result;
    for (int i : rhs.number) {
        result += to_string(i);
    }
    return result;
}

int to_int(BigNum rhs) {
    int result = stoi(to_string(rhs));
    return result;
}

BigNum& BigNum::operator=(BigNum rhs) {
  number = rhs.number;
  return *this;
}

BigNum& BigNum::operator=(const int& rhs) {
  initialize(to_string(rhs));
  return *this;
}

BigNum& BigNum::operator=(string rhs) {
  initialize(rhs);
  return *this;
}

BigNum& BigNum::operator=(vector<int> rhs) {
  number = rhs;
  return *this;
}

ostream& operator<<(ostream& os, const BigNum& rhs) {
    for (int i : rhs.number) {
        os << i;
    }
    return os;  
}  

istream& operator>>(istream& is, BigNum& rhs) {
    string input;
    is >> input;
    rhs = input;
    return is;  
}

bool operator==(BigNum lhs, const BigNum& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (int i = 0; i < lhs.size(); i++) {
    if (rhs.number.at(i) != lhs.number.at(i)) {
        return false;
    }
  }
  return true;
}

bool operator!=(BigNum lhs, const BigNum& rhs) {
    return !(lhs == rhs);
}

bool operator<(BigNum lhs, const BigNum& rhs) {
    if (lhs.size() == 0 && rhs.size() != 0) {
        return true;
    }

    if (lhs.size() != 0 && rhs.size() == 0) {
        return false;
    }

    if (lhs == BigNum(0) && rhs == BigNum(0)) {
        return false;
    }

    if (lhs.negative == true && rhs.negative == false) {
        return true;
    }

    if (lhs.negative == false && rhs.negative == true) {
        return false;
    }

    if (lhs.negative == false && lhs.size() > rhs.size()) {
        return false;
    }
    
    if (lhs.negative == false && lhs.size() < rhs.size()) {
        return true;
    }

    for (int i = 0; i < lhs.size(); i++) {
      if (lhs.number.at(i) > rhs.number.at(i)) {
        return false;
      }
      if (lhs.number.at(i) < rhs.number.at(i)) {
        return true;
      }
    }

    return false;
}

bool operator<(BigNum lhs, const int& rhs) {
    return lhs < BigNum(rhs);
}

bool operator>(BigNum lhs, const BigNum& rhs) {
    return !(lhs < rhs) && !(lhs == rhs);
}

bool operator>(BigNum lhs, const int& rhs) {
    return lhs > BigNum(rhs);
}

bool operator>=(BigNum lhs, const BigNum& rhs) {
    return !(lhs < rhs);
}

bool operator>=(BigNum lhs, const int& rhs) {
    return lhs >= BigNum(rhs);
}

bool operator<=(BigNum lhs, const BigNum& rhs) {
    return !(lhs > rhs);
}

bool operator<=(BigNum lhs, const int& rhs) {
    return lhs <= BigNum(rhs);
}

inline BigNum operator+(BigNum lhs, const BigNum& rhs) {
    BigNum top = rhs;

    int min = lhs.size();
    if (top.size() < min) { min = top.size(); }

    int max = lhs.size();
    if (top.size() > max) { max = top.size(); }

    std::reverse(lhs.number.begin(), lhs.number.end());
    std::reverse(top.number.begin(), top.number.end());

    if (top.size() < max) {
        while (top.size() < max) {
            top.number.push_back(0);
        }
    }
    else if (lhs.size() < max) {
        while (lhs.size() < max) {
            lhs.number.push_back(0);
        }
    }


    BigNum sum;
    sum.number = vector<int>(max+1, 0);
    for (int i = 0; i < max; i++) {
        sum.number.at(i) = top.number.at(i) + lhs.number.at(i);
    }

    for (int i = 0; i < max; i++) {
        if (sum.number.at(i) > 9) {
            sum.number.at(i) -= 10;
            sum.number.at(i+1) += 1;
        }
    }

    for (int i = max; i >= 0; i--) {
        if (sum.number.at(i) != 0) {
            break;
        }
        else {
            sum.number.pop_back();
        }
    }


    std::reverse(sum.number.begin(), sum.number.end());
    return sum;
}

inline BigNum operator+(BigNum lhs, int rhs) {
    return lhs + BigNum(rhs);
}

void operator++(BigNum& lhs, int rhs) {
    lhs = lhs + 1;
}

void operator+=(string& lhs, BigNum& rhs) {
    lhs = lhs + to_string(rhs);
}

inline BigNum operator-(BigNum lhs, const BigNum& rhs) {
    BigNum sum;
    int max = lhs.size();
    if (rhs.size() > max) { max = rhs.size(); }
    int min = lhs.size();
    if (rhs.size() < min) { min = rhs.size(); }
    
    for (int i = 0; i < min; i++) {
        sum.number.push_back(lhs.number.at(lhs.size()-1 - i) - rhs.number.at(rhs.size()-1 - i));
    }
    if (lhs.size() > min) {
        for (int i = min; i < lhs.size(); i++) {
            sum.number.push_back(lhs.number.at(lhs.size()-1 - i));
        }
    }
    else if (rhs.size() > min) {
        for (int i = min; i < rhs.size(); i++) {
            sum.number.push_back(lhs.number.at(rhs.size()-1 - i));
        }
    }
    std::reverse( sum.number.begin(), sum.number.end() );

    for (int i = lhs.size() - 1; i > 0; i--) {
        if (sum.number.at(i) < 0) {
            sum.number.at(i-1) -= 1;
            sum.number.at(i) += 10;
        }
    }

    vector<int> refreshed_sum;
    int i = 0;
    for (i; i < sum.number.size(); i++) {
        if (sum.number.at(i) != 0) {
            break;
        }
    }
    for (i; i < sum.number.size(); i++) {
        refreshed_sum.push_back(sum.number.at(i));
    }

    sum.number = refreshed_sum;

    if (sum.size() == 0) {
        sum.number = {0};
    }

    if (sum.number.at(0) < 0) {
        sum.number.at(0) *= -1;
        sum.negative = true;
        cout << "negated" << endl;
    }

    return sum;
}

inline BigNum operator-(BigNum lhs, int rhs) {
    return lhs - BigNum(rhs);
}

inline BigNum operator*(BigNum lhs, const BigNum& rhs) {
    vector<BigNum> mult_results;
    int rhs_carry_size = rhs.number.size() + lhs.number.size();
    vector<int> rhs_carry(rhs_carry_size, 0);

    for (int i = lhs.size()-1; i >= 0; i--) { // for each on lower bar

        for (int i = 0; i < rhs_carry.size(); i++) { // set carries to 0
            rhs_carry.at(i) = 0;
        }

        BigNum sub_result;
        for (int k = 0; k < lhs.size()-1 - i; k++) { // push back initial zeroes
            sub_result.number.push_back(0);
        }

        for (int j = rhs.number.size()-1; j >= 0; j--) { // for each on upper bar
            sub_result.number.push_back(lhs.number.at(i) * rhs.number.at(j) + rhs_carry.at(j)); // push back lower * upper + carry
            
            if (sub_result.number.at(sub_result.number.size()-1) > 9) { // if last number > 9
                int new_digit = sub_result.number.at(sub_result.number.size()-1) % 10; // digit is the remainder / 10
                if (j > 0) { // if upper bar not the last one
                    rhs_carry.at(j-1) += sub_result.number.at(sub_result.number.size()-1) / 10; // increase carry 
                    sub_result.number.at(sub_result.number.size()-1) = new_digit; // set digit
                }
                else { // if upper bar last one
                    sub_result.number.push_back(sub_result.number.at(sub_result.number.size()-1) / 10); // push back carry
                    sub_result.number.at(sub_result.number.size()-2) = new_digit; // set digit
                }
            }
        }   

        reverse(sub_result.number.begin(), sub_result.number.end());
        mult_results.push_back(sub_result);
    }

    BigNum result;
    result.initialize("0");
    for (BigNum i : mult_results) {
        result = i + result;
    }
    return result;
}

inline BigNum operator*(BigNum lhs, int rhs) {
    return lhs * BigNum(rhs);;
}

inline BigNum operator%(BigNum lhs, const BigNum& rhs) {
    while (lhs >= rhs) {
        lhs = lhs - rhs;
    }
    return lhs;
}

inline BigNum operator%(BigNum lhs, int rhs) {
    return lhs % BigNum(rhs);
}

inline BigNum operator/(BigNum lhs, const BigNum& rhs) {
    BigNum counter = 0;
    while (lhs >= rhs) {
        lhs = lhs - rhs;
        counter = counter + 1;
    }
    return counter;
}

inline BigNum operator/(BigNum lhs, int rhs) {
    lhs = lhs / BigNum(rhs);
    return lhs;
}

BigNum BigNum::sqrt() {
    BigNum root = *this;
    BigNum midpoint = root / 2;
    BigNum original = root;
    BigNum low = 0;
    BigNum high = original;
    BigNum square;
    BigNum old_midpoint;
    string trash;

    while (true) {
        old_midpoint = midpoint;
        
        square = midpoint * midpoint;
        if (square > original) {
            high = midpoint;
            midpoint = (midpoint + low) / 2;
        }
        else if (square < original) {
            low = midpoint;
            midpoint = (midpoint + high) / 2;
        }
        
        if (midpoint == old_midpoint) {
            break;
        }
    }
    return midpoint;
}


BigNum p;
BigNum q;
BigNum n;
BigNum t; 
bool flag;
BigNum e[100];
BigNum d[100];
BigNum j;
int temp[100];
int m[100], en[100];
char msg[100];

bool prime(BigNum);
BigNum cd(BigNum);
void ce();
void encrypt();
std::string decrypt();

bool prime(BigNum pr)
{
    BigNum i;
    j = pr.sqrt();
    for (i = 2; i <= j; i++)
    {
        if (pr % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main()
{
    BigNum a(100);
    BigNum b(13);
    cout << a % b << endl;
    //////////////////////
    int i;
    cout << "\nENTER FIRST PRIME NUMBER\n";
    cin >> p;
    flag = prime(p);
    if (flag != true)
    {
        cout << "\nWRONG INPUT\n";
        exit(1);
    }
    cout << "\nENTER ANOTHER PRIME NUMBER\n";
    cin >> q;
    flag = prime(q);
    if (flag != true || p == q)
    {
        cout << "\nWRONG INPUT\n";
        exit(1);
    }
    cout << "\nENTER MESSAGE\n";
    fflush(stdin);
    cin >> msg;
    for (i = 0; msg[i] != '\0'; i++)
        m[i] = msg[i];
    n = p * q;
    t = (p - 1) * (q - 1);
    ce();
    cout << "\nPOSSIBLE VALUES OF e AND d ARE\n";
    for (i = 0; i < j - 1; i++)
        cout << e[i] << "\t" << d[i] << "\n";
    encrypt();
    cout << decrypt();

    return 0;
}

void ce() {
    int k;
    BigNum i;
    BigNum new_flag;
    k = 0;
    for (i = 2; i < t; i++)
    {
        cout << "Calculating: " << i << " / " << t << '\r';
        if (t % i == 0) { 
            continue;
        }
        flag = prime(i);

        if (flag == true && i != p && i != q)
        {
            e[k] = i;
            new_flag = cd(BigNum(e[k]));
            if (new_flag > 0)
            {
                d[k] = new_flag;
                k++;
            }
            if (k == 99)
                break;
        }
    }
    cout << endl;
}

BigNum cd(BigNum x) {
    BigNum k = 1;
    string trash;
    while (1)
    {
        k = t + k;
        if (k % x == 0) {
            return (k / x);
        }
    }
}

void encrypt()
{
    int i;
    int pt;
    int ct;
    BigNum key = e[0];
    int len;
    BigNum k;
    i = 0;
    len = strlen(msg);
    cout << "len:" << len << endl;
    while (i != len)
    {
        pt = m[i];
        pt = pt - 96;
        k = 1;
        for (j = 0; j < key; j++)
        {
            k = k * pt;
            k = k % n;
        }
        temp[i] = to_int(k);
        ct = to_int(k + 96);
        en[i] = ct;
        i++;
    }
    en[i] = -1;
    cout << "\nTHE ENCRYPTED MESSAGE IS\n";
    for (i = 0; en[i] != -1; i++) {
        printf("%c", en[i]);
    }
}

std::string decrypt()
{
    int i;
    BigNum pt, ct, key = d[0], k;
    i = 0;
    while (en[i] != -1)
    {
        ct = temp[i];
        k = 1;
        for (j = 0; j < key; j++)
        {
            k = k * ct;
            k = k % n;
        }
        pt = k + 96;
        m[i] = to_int(pt);
        i++;
    }
    std::string result;
    m[i] = -1;
    for (i = 0; m[i] != -1; i++) {
        result += m[i];
    }
    return result;
}