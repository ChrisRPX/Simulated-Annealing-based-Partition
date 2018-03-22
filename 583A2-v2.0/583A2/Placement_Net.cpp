//
//  Placement_Net.cpp
//  581A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#include "Placement_Net.hpp"

PLC::PLC()
{
    cell_num = 0;
    wire_num = 0;
    
    grid_col = 0;
    grid_row = 0;
    
    cost = 0;
    
    nets = NULL;
    cells = NULL;
}

PLC::~PLC()
{
    if(cells)
    {
        delete [] cells;
        cout<<"The cells in PLC has been deleted"<<endl;
    }
    if(nets)
    {
        delete [] nets;
        cout<<"The nets in PLC has been deleted"<<endl;
    }
}

void PLC::swap_cells(Grids **mygrids,Point i,Point j)
{
    //the only thing could be swapped is their locations
    int ax = i.x;
    int ay = i.y;
    int bx = j.x;
    int by = j.y;
    int temp;
    if(mygrids[ay][ax].cell_No==-1 && mygrids[by][bx].cell_No==-1)
        return;
    if(ax==bx && ay==by)
        return;
    
    temp = mygrids[ay][ax].cell_No;
    mygrids[ay][ax].cell_No = mygrids[by][bx].cell_No;
    mygrids[by][bx].cell_No = temp;
    
    if(mygrids[ay][ax].cell_No==-1 && mygrids[by][bx].cell_No!=-1)
    {
        cells[mygrids[by][bx].cell_No].x = bx;
        cells[mygrids[by][bx].cell_No].y = by;
        return;
    }
    
    if(mygrids[ay][ax].cell_No!=-1 && mygrids[by][bx].cell_No==-1)
    {
        cells[mygrids[ay][ax].cell_No].x = ax;
        cells[mygrids[ay][ax].cell_No].y = ay;
        return;
    }
    
    cells[mygrids[by][bx].cell_No].x = bx;
    cells[mygrids[by][bx].cell_No].y = by;
    cells[mygrids[ay][ax].cell_No].x = ax;
    cells[mygrids[ay][ax].cell_No].y = ay;
    
    /*
    if(i<0 || i>=cell_num || j<0 || j>=cell_num)
    {
        cout<<"Warning: Wrong swap! Cells do not exsit"<<endl;
        return;
    }
    */
}

int PLC::cal_cost()
{
    cost = 0;
    int i,j;
    int max_x,max_y,min_x,min_y;
    int dx=0,dy=0;
    for(i=0;i<wire_num;i++)
    {
        max_x = -1;
        max_y = -1;
        min_x = grid_col;
        min_y = grid_row;
        if(cells[nets[i].src_No].x<min_x) min_x = cells[nets[i].src_No].x;
        if(cells[nets[i].src_No].y<min_y) min_y = cells[nets[i].src_No].y;
        if(cells[nets[i].src_No].x>max_x) max_x = cells[nets[i].src_No].x;
        if(cells[nets[i].src_No].y>max_y) max_y = cells[nets[i].src_No].y;
        for(j=0;j<this->nets[i].cell_num-1;j++)
        {
            if(cells[nets[i].sink_No[j]].x<min_x) min_x = cells[nets[i].sink_No[j]].x;
            if(cells[nets[i].sink_No[j]].y<min_y) min_y = cells[nets[i].sink_No[j]].y;
            if(cells[nets[i].sink_No[j]].x>max_x) max_x = cells[nets[i].sink_No[j]].x;
            if(cells[nets[i].sink_No[j]].y>max_y) max_y = cells[nets[i].sink_No[j]].y;
        }
        dx = max_x - min_x;
        dy = max_y - min_y;
        cost += dx+dy;
    }
    return cost;
}
