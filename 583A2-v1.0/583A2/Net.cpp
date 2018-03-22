//
//  Net.cpp
//  583A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#include "Net.hpp"

Net::Net()
{
    net_No = -1;
    cell_num = 0;
    src_No = -1;
    sink_No = NULL;
}

Net::~Net()
{
    if(sink_No)
    {
        delete [] sink_No;
        //cout<<"The sink_No in net"<<this->net_No<<" has been deleted"<<endl;
    }
}

