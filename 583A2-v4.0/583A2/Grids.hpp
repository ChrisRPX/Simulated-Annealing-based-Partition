//
//  Grids.hpp
//  583A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#ifndef Grids_hpp
#define Grids_hpp

#include <stdio.h>

struct Point
{
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x1, int y1) : x(x1), y(y1) {}
};

class Grids
{
public:
    int cell_No;
    /*this may indicate
     1. the number of the cell which is used in cells[] in the whole placement, and the number will never been modified;
     2. in the case of grids, the occupation cell. This may change quite often. Actually, grids have no cell number.
     */
    int x;
    int y;
    //Point blo_loc;
    
    Grids();
    ~Grids();
};

#endif /* Grids_hpp */
