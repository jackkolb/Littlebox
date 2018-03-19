#include<iostream>
#include<math.h>
#include<string.h>
#include <string>
#include<stdlib.h>
#include <vector>

using namespace std;

class BigNum {
public:
    vector<int> number;
    int size = 0;

    void initialize(string);
    void copy(BigNum);

    void add(BigNum);
    void subtract(BigNum);
    void multiply(BigNum);
    void divide(BigNum);
    BigNum sqrt();

    bool is_less_than(BigNum);
    bool is_less_than_equal_to(BigNum);
    bool is_greater_than(BigNum);
    bool is_greater_than_equal_to(BigNum);
    bool is_equal_to(BigNum);

    void display();
}

void BigNum::initialize(string source) {
    number.resize(source.size());
    for (int i = 0; i < source.size(); i++) {
        number.at(i) = source.at(i);
    }
    size = source.size();
    return;
}

void BigNum::display() {
    for (int i : number) {
        cout << i;
    }
}
 
BigNum BigNum::add(BigNum additional) {
    int max = size;
    if (additional.size > max) { max = additional; }

    BigNum sum;

    for (int i = 0; i < size; i++) {
        sum.number.push_back(number.at(i) + additional.at(i));
    }

    for (int i = size-1; i > 0; i++) {
        if (sum.number.at(i) > 9) {
            sum.number.at(i-1) += sum.number.at(i) - 10;
            sum.number.at(i) -= 10;
        }
    }

    return sum;
}

long int p, q, n, t, flag, e[100], d[100], temp[100], j, m[100], en[100], i;
char msg[100];

int prime(long int);
void ce();
long int cd(long int);
void encrypt();
std::string decrypt();

int prime(long int pr)
{
    int i;
    j = sqrt(pr);
    for (i = 2; i <= j; i++)
    {
        if (pr % i == 0)
            return 0;
    }
    return 1;
}

int main()
{
/*    cout << "\nENTER FIRST PRIME NUMBER\n";
    cin >> p;
    flag = prime(p);
    if (flag == 0)
    {
        cout << "\nWRONG INPUT\n";
        exit(1);
    }
    cout << "\nENTER ANOTHER PRIME NUMBER\n";
    cin >> q;
    flag = prime(q);
    if (flag == 0 || p == q)
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
    decrypt();*/

    BigNum a;
    a.number = {1, 2, 3, 4, 5, 6 ,7 ,8 ,9};
    BigNum b;
    b.number = {3, 2, 1, 4, 3, 2, 1, 0, 9};

    BigNum c = a.add(b);

    c.display();

    return 0;
}

void ce()
{
    int k = 0;
    for (i = 2; i < t; i++)
    {
        if (t % i == 0) { continue; }
        flag = prime(i);
        if (flag == 1 && i != p && i != q)
        {
            e[k] = i;
            flag = cd(e[k]);
            if (flag > 0) { d[k] = flag; k++; }
            if (k == 99) { break; }
        }
    }
}

long int cd(long int x) {
    long int k = 1;
    while (1)
    {
        k = k + t;
        if (k % x == 0) { return (k / x); }
    }
}

void encrypt()
{
    long int pt, ct, key = e[0], k, len;
    i = 0;
    len = strlen(msg);
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
        temp[i] = k;
        ct = k + 96;
        en[i] = ct;
        i++;
    }
    en[i] = -1;
    cout << "\nTHE ENCRYPTED MESSAGE IS\n";
    for (i = 0; en[i] != -1; i++)
        printf("%c", en[i]);
}

std::string decrypt()
{
    long int pt, ct, key = d[0], k;
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
        m[i] = pt;
        i++;
    }
    std::string result;
    m[i] = -1;
    for (i = 0; m[i] != -1; i++)
        result += m[i];
    return result;
}