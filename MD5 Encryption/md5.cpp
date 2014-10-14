#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

// four link variables
int A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0x10325476;

/**
 * constant parameter array: Ti
 * T[i] denote the i-th element of the table, which is equal to the integer part of 4294967296 times abs(sin(i))
 * (i is in radians)
 */
int T[65] = {
    0x00000000,

    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 
    
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x2441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

// four different nonlinear function
int F(int X, int Y,int Z) {
    return (X&Y)|((~X)&Z);
}
int G(int X, int Y, int Z) {
    return (X&Z)|(Y&(~Z));
}
int H(int X, int Y, int Z) {
    return X^Y^Z;
}
int I(int X, int Y, int Z) {
    return Y^(X|(~Z));
}

int rotateLeft(int num, int s) {
    return (num << s) | ((num & 0xFFFFFFFF) >> (32 - s));
}

// use one of following function set one of the link variable
int FF(int a, int b, int c, int d, int Mj, int s, int ti) {
    return (b + rotateLeft((a + F(b,c,d) + Mj + ti), s));
}
int GG(int a, int b, int c, int d, int Mj, int s, int ti) {
    return (b + rotateLeft((a + G(b,c,d) + Mj + ti), s));
}
int HH(int a, int b, int c, int d, int Mj, int s, int ti) {
    return (b + rotateLeft((a + H(b,c,d) + Mj + ti), s));
}
int II(int a, int b, int c, int d, int Mj, int s, int ti) {
    return (b + rotateLeft((a + I(b,c,d) + Mj + ti), s));
}

// translate a int type number between big-end and little-end order
int bigend(int num) {

    int c4 = ((num << 24) & 0xFF000000);
    int c3 = ((num <<  8) & 0x00FF0000);
    int c2 = ((num >>  8) & 0x0000FF00);
    int c1 = ((num >> 24) & 0x000000FF);

    return (c1 | c2 | c3 | c4);
}

/** 
 * 1.fill data to 32*n bits
 * 2.fill data to (512*n + 448) bits first and then (n = 0, 1, 2, ...)
 * 3.fill the extra 64 bits with the number of bit
 * Finally get 512*N (N = 1, 2, 3, ...)
 */
vector<int> fillBits(string data)
{
    int size, numOfInt, need, start;
    vector<int> result;

    size = data.size();
    numOfInt = size / 4;
    // need is number of needed char will 
    // be left after the left be transformed into int
    need = size % 4;  

    // turn 4 char into an int data
    for (int i = 0; i < numOfInt; i++) {
        int tmp = 0;
        for (int j = 0; j < 4; j++)
            tmp |= data[4 * i + j] << (8 * (4 - j - 1));

        result.push_back(tmp);
    }

    // maybe 0, 1, 2, or 3 char member will be left
    bool push_1_in_char = false;  // variable to record whether bit '1' was push to a char

    if (need != 0) {
        int tmp = 0, start = (size / 4) * 4;

        for (int i = 0; i < need; i++) 
            tmp |= data[start + i] << (8 * (3 - i));

        tmp |= 0x80 << ((3 - need) * 8);

        result.push_back(tmp);
        push_1_in_char = true;
    }

    if (!push_1_in_char)
        // create a new int start with 1
        result.push_back(0x80000000);

    need = 16 - (result.size() + 2) % 16;

    if (need != 16) {
        for (int i = 0; i < need; i++)
            result.push_back(0x00);
    }

    size = size << 3;  // number of bit = num of int * 8

    // It's very strange here, 
    // [0000 0000 0000 0000 | 0000 0000 0000 0001] must be add in order: 
    // [0000 0000 0000 0001 | 0000 0000 0000 0000] and in big-end order
    // I have to save the last 64 bit in big-end in advance, another choice is:
    // to push back them after grouping
    result.push_back(bigend((int)(size & 0x00000000FFFFFFFF)));
    result.push_back(bigend((int)((long)size >> 32)));

    return result;
}

// grouping data into different groups every 512 bit 
vector< vector<int> > grouping(vector<int> allbits)
{
    int size = allbits.size(), count = 1;
    vector< vector<int> > groups;  // structure: {{16 int}, {16 int}, ..., {<=16 int}}
    vector<int> tmp;


    int currentInt, turn;
    for (int i = 0; i < size; i++, count++) {
        currentInt = allbits[i];
        turn = bigend(currentInt);

        tmp.push_back(turn);
        if (tmp.size() == 16) {
            groups.push_back(tmp);
            tmp.clear();
            count = 1;
        }
    }

    return groups;
}


// loop to calculate new A, B, C, D
void loop(int *A, int *B, int *C, int *D, vector<int> group)
{
        /* Round 1 */
        *A = FF(*A, *B, *C, *D, group[ 0],  7, T[ 1]);
        *D = FF(*D, *A, *B, *C, group[ 1], 12, T[ 2]);
        *C = FF(*C, *D, *A, *B, group[ 2], 17, T[ 3]);
        *B = FF(*B, *C, *D, *A, group[ 3], 22, T[ 4]);

        *A = FF(*A, *B, *C, *D, group[ 4],  7, T[ 5]);
        *D = FF(*D, *A, *B, *C, group[ 5], 12, T[ 6]);
        *C = FF(*C, *D, *A, *B, group[ 6], 17, T[ 7]);
        *B = FF(*B, *C, *D, *A, group[ 7], 22, T[ 8]);

        *A = FF(*A, *B, *C, *D, group[ 8],  7, T[ 9]);
        *D = FF(*D, *A, *B, *C, group[ 9], 12, T[10]);
        *C = FF(*C, *D, *A, *B, group[10], 17, T[11]);
        *B = FF(*B, *C, *D, *A, group[11], 22, T[12]);

        *A = FF(*A, *B, *C, *D, group[12],  7, T[13]);
        *D = FF(*D, *A, *B, *C, group[13], 12, T[14]);
        *C = FF(*C, *D, *A, *B, group[14], 17, T[15]);
        *B = FF(*B, *C, *D, *A, group[15], 22, T[16]);

        /* Round* 2 **/
        *A = GG(*A, *B, *C, *D, group[ 1],  5, T[17]);
        *D = GG(*D, *A, *B, *C, group[ 6],  9, T[18]);
        *C = GG(*C, *D, *A, *B, group[11], 14, T[19]);
        *B = GG(*B, *C, *D, *A, group[ 0], 20, T[20]);

        *A = GG(*A, *B, *C, *D, group[ 5],  5, T[21]);
        *D = GG(*D, *A, *B, *C, group[10],  9, T[22]);
        *C = GG(*C, *D, *A, *B, group[15], 14, T[23]);
        *B = GG(*B, *C, *D, *A, group[ 4], 20, T[24]);

        *A = GG(*A, *B, *C, *D, group[ 9],  5, T[25]);
        *D = GG(*D, *A, *B, *C, group[14],  9, T[26]);
        *C = GG(*C, *D, *A, *B, group[ 3], 14, T[27]);
        *B = GG(*B, *C, *D, *A, group[ 8], 20, T[28]);

        *A = GG(*A, *B, *C, *D, group[13],  5, T[29]);
        *D = GG(*D, *A, *B, *C, group[ 2],  9, T[30]);
        *C = GG(*C, *D, *A, *B, group[ 7], 14, T[31]);
        *B = GG(*B, *C, *D, *A, group[12], 20, T[32]);

        /* Round* 3 **/
        *A = HH(*A, *B, *C, *D, group[ 5],  4, T[33]);
        *D = HH(*D, *A, *B, *C, group[ 8], 11, T[34]);
        *C = HH(*C, *D, *A, *B, group[11], 16, T[35]);
        *B = HH(*B, *C, *D, *A, group[14], 23, T[36]);

        *A = HH(*A, *B, *C, *D, group[ 1],  4, T[37]);
        *D = HH(*D, *A, *B, *C, group[ 4], 11, T[38]);
        *C = HH(*C, *D, *A, *B, group[ 7], 16, T[39]);
        *B = HH(*B, *C, *D, *A, group[10], 23, T[40]);

        *A = HH(*A, *B, *C, *D, group[13],  4, T[41]);
        *D = HH(*D, *A, *B, *C, group[ 0], 11, T[42]);
        *C = HH(*C, *D, *A, *B, group[ 3], 16, T[43]);
        *B = HH(*B, *C, *D, *A, group[ 6], 23, T[44]);

        *A = HH(*A, *B, *C, *D, group[ 9],  4, T[45]);
        *D = HH(*D, *A, *B, *C, group[12], 11, T[46]);
        *C = HH(*C, *D, *A, *B, group[15], 16, T[47]);
        *B = HH(*B, *C, *D, *A, group[ 2], 23, T[48]);

        /* Round* 4 **/
        *A = II(*A, *B, *C, *D, group[ 0],  6, T[49]);
        *D = II(*D, *A, *B, *C, group[ 7], 10, T[50]);
        *C = II(*C, *D, *A, *B, group[14], 15, T[51]);
        *B = II(*B, *C, *D, *A, group[ 5], 21, T[52]);

        *A = II(*A, *B, *C, *D, group[12],  6, T[53]);
        *D = II(*D, *A, *B, *C, group[ 3], 10, T[54]);
        *C = II(*C, *D, *A, *B, group[10], 15, T[55]);
        *B = II(*B, *C, *D, *A, group[ 1], 21, T[56]);

        *A = II(*A, *B, *C, *D, group[ 8], 6, T[57]);
        *D = II(*D, *A, *B, *C, group[15], 10, T[58]);
        *C = II(*C, *D, *A, *B, group[ 6], 15, T[59]);
        *B = II(*B, *C, *D, *A, group[13], 21, T[60]);

        *A = II(*A, *B, *C, *D, group[ 4], 6, T[61]);
        *D = II(*D, *A, *B, *C, group[11], 10, T[62]);
        *C = II(*C, *D, *A, *B, group[ 2], 15, T[63]);
        *B = II(*B, *C, *D, *A, group[ 9], 21, T[64]);
}

void test(string data)
{

    // initialize link variables
    int A = 0x67452301, B = 0xEFCDAB89, C = 0x98BADCFE, D = 0x10325476;
    vector<int> result = fillBits(data);
    int size = result.size();

    vector< vector<int> > groups = grouping(result);

    cout << " Input data:\n";
    cout << "+---------------------\n ";
    cout << data << endl << endl;
    cout << " grouping...\n size: " << dec << groups.size() << endl;
    cout << "+---------------------\n";

    for (int i = 0; i < groups.size(); i++) {
        cout << " " << dec << i + 1 << "\t";
        for (int j = 0; j < groups[i].size(); j++) {
            cout << hex << groups[i][j] << ' ';
        }
        cout << endl;
    }

    for (int i = 0; i < groups.size(); i++) {

        vector<int> group = groups[i];  // a group has 16 int
        int AA = A, BB = B, CC = C, DD = D;

        loop(&A, &B, &C, &D, group);

        A += AA; B += BB;
        C += CC; D += DD;
    }

    cout << "\n A B C D after calulate:\n";
    cout << "+--------------------------------\n";
    cout << " " << hex << A << ' ' << B << ' ' << C << ' ' << D << endl;

    cout << "\n MD5 value:\n";
    cout << "+--------------------------------\n";
    cout << " " << setw(8) << setfill('0') << bigend(A) << ' ' << bigend(B) << ' ' << bigend(C) << ' ' << bigend(D) << endl;

}


