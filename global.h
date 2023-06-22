#ifndef GLOBAL_H
#define GLOBAL_H

/*

i = 1   j = 0   0
i = 1   j = 1   1
i = 0   j = 2   2
i = 3   j = 2   3
i = 0   j = 3   4
i = 2   j = 3   5
i = 3   j = 1   6
i = 0   j = 4   7
i = 4   j = 4   8
i = 7   j = 5   9
i = 1   j = 5   10
i = 2   j = 1   11


i =  1 j =  0
i =  1 j =  1
i =  0 j =  2
i =  3 j =  2
i =  0 j =  3
i =  2 j =  3
i =  3 j =  1
i =  0 j =  4
i =  4 j =  4
i =  7 j =  5
i =  5 j =  5
i =  1 j =  6

*/
//1   0 = 0
//0   0

//0   2 = 2
//2   2

//0   3 = 4
//3   3

//5   3 = 5
//2   3

//0   4
//4   4

namespace ArrayIndex {

    constexpr int indexes[9][9] = {  0, -1,  2,  4,  7, -1, -1, -1, -1,
                                     0,  1, -1, -1, -1, -1, 11, -1, -1,
                                     0,  11, 2,  5, -1, -1, -1, -1, -1,
                                    -1,  6,  3,  4, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1,  8, -1, -1, -1, -1,
                                    -1, -1, -1,  5, -1, 10, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1,
                                    -1, -1, -1, -1, -1,  9, -1, -1, -1,
                                    -1, -1, -1, -1, -1, -1, -1, -1, -1 };
}

#endif // GLOBAL_H
