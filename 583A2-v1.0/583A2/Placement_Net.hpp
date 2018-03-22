//
//  Placement_Net.hpp
//  581A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#ifndef Placement_Net_hpp
#define Placement_Net_hpp

#include <stdio.h>
#include <iostream>
#include "Net.hpp"
#include "Grids.hpp"
using namespace std;

class PLC
{
public:
    int cell_num;
    int wire_num;
    int grid_col;
    int grid_row;
    
    int cost;
    
    Net *nets;
    Grids *cells;
    
    PLC();
    ~PLC();
    void swap_cells(Grids **mygrids,Point i,Point j);//After a swap, cost should be re-calculated "locally"
    //void cal_cost(int i, int j);//when swap happens, it only needs to calculate a "local" cost instead of global cost.
    /*
     I thought this should make it faster when I only calculate the cost related to the swapped nodes, because those nets does not contain these
     changed nodes do not need to re-calculate cost. But actually, to find the changed nets is a big work,
     which does not make the iteration or time complicity fewer. And the number of changed nets can be as many as net_num.
     */
    int cal_cost();
};

#endif /* Placement_Net_hpp */
