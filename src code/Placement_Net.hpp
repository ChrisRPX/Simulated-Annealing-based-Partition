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
    
    void reset_inv(); //nets[i].involved should be reset before next iteration
    void find_inv(Grids **mygrids,Point i,Point j); //to find which nets are involved
    int cal_swap_cost(); //when swap happens, it only needs to calculate a "local" cost instead of global cost.
    int cal_total_cost();
};

#endif /* Placement_Net_hpp */
