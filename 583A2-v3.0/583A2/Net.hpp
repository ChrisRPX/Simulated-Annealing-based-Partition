//
//  Net.hpp
//  583A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#ifndef Net_hpp
#define Net_hpp

#include <stdio.h>
#include <iostream>
#include "Grids.hpp"
using namespace std;

class Net
{
public:
    int net_No;
    int cell_num;
    int src_No;
    int *sink_No;
    bool involved;
    
    Net();
    ~Net();
};

#endif /* Net_hpp */
