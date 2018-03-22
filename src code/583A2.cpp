//
//  main.cpp
//  581A2
//
//  Created by Peixu Ren on 2018-02-06.
//  Copyright © 2018 Peixu Ren. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "graphics.h"
#include "Grids.hpp"
#include "Net.hpp"
#include "Placement_Net.hpp"
using namespace std;

#define delay_time 500

void drawscreen(void);
void act_on_button_press(float x, float y);
void draw_init(Grids **mygrids,int map_h,int map_w,int gridsize);
void draw_cells(PLC &plcnet,int gridsize);
void draw_netline(PLC &plcnet,int gridsize);
void delay();

bool read_file(char *,Grids **mygrids,PLC &plcnet);
void rand_plc(Grids **mygrids,PLC &plcnet);
void sim_annealing(Grids **mygrids,PLC &plcnet,int init_cost,int gridsize);

int main(int argc, const char * argv[])
{
    // insert code here...
    
    clock_t start;
    clock_t end;
    double duration;
    
    int i,j;
    Grids **mygrids = NULL;
    PLC plcnet;
    
    char path[100] = "/Users/peixu/Documents/XCode/583A2/A2_Benchmarks/";
    char filename[10];
    cout<<"Please input the bench mark name: "<<endl;
    cin>>filename;
    strcat(path,filename);
    strcat(path,".txt");
    if(!read_file(path,mygrids,plcnet))
    {
        cout<<"Please clarify the filename is correct, or please modify the path in cpp file!"<<endl;
        exit(1); // terminate with error
    }
    
    mygrids = new Grids *[plcnet.grid_row];
    for(i=0;i<plcnet.grid_row;i++)
    {
        mygrids[i] = new Grids [plcnet.grid_col];
    }
    for(i=0;i<plcnet.grid_row;i++)
    {
        for(j=0;j<plcnet.grid_col;j++)
        {
            mygrids[i][j].y = i;
            mygrids[i][j].x = j;
        }
    }

    /********** Start: draw all the grids and execute the algorithm **********/
    //init the graph
    int gridsize = 50;
    printf ("About to start graphics.\n\n");
    init_graphics("583 Demo", WHITE);
    init_world (0.,0.,plcnet.grid_col*gridsize,plcnet.grid_row*gridsize);
    event_loop(act_on_button_press,NULL,NULL,drawscreen);//act_on_button_press
    clearscreen();
    draw_init(mygrids,plcnet.grid_row,plcnet.grid_col,gridsize);
    //draw_cells(plcnet,gridsize);
    //draw_netline(plcnet,gridsize);
    //flushinput();
    //delay();
    event_loop(act_on_button_press,NULL,NULL,NULL);//act_on_button_press
    /********** End: draw all the grids and execute the algorithm **********/

    /********** Start: simulated annealing **********/
    
    /********** Initialize: place all the cells at random **********/
    rand_plc(mygrids,plcnet);
    int init_cost = plcnet.cal_total_cost();
    cout<<"Initial plcnet.cost="<<init_cost<<endl;
    
    start = clock();
    sim_annealing(mygrids,plcnet,init_cost,gridsize);
    end = clock();
    duration = (end-start)/(double)CLOCKS_PER_SEC;
    cout<<"dtime="<<duration<<endl<<endl;
    
    
    /********** End: simulated annealing **********/

    event_loop(act_on_button_press,NULL,NULL,NULL);//act_on_button_press
    close_graphics();
    printf ("Graphics closed down.\n");
    
//    sim_annealing(mygrids,plcnet,init_cost,gridsize);
    
    for(i=0;i<plcnet.grid_row;i++)
    {
        delete [] mygrids[i];
    }
    delete [] mygrids;
    //plcnet.~PLC();
    return 0;
}

bool read_file(char *filename,Grids **mygrids,PLC &plcnet)
{
    int i,j;
    ifstream inf;
    inf.open(filename);
    if(!inf)
    {
        cout<<"Unable to open file"<<endl;
        return false; // terminate with error
    }
    inf>>plcnet.cell_num>>plcnet.wire_num>>plcnet.grid_row>>plcnet.grid_col;
    
    if(plcnet.cell_num>plcnet.grid_row*plcnet.grid_col)
    {
        cout<<"Too many cells for this net!"<<endl;
        return false; // terminate with error
    }
    
    plcnet.cells = new Grids [plcnet.cell_num];
    for(i=0;i<plcnet.cell_num;i++)
    {
        plcnet.cells[i].cell_No = i;
    }
    plcnet.nets = new Net [plcnet.wire_num];
    for(i=0;i<plcnet.wire_num;i++)
    {
        plcnet.nets[i].net_No = i;
        inf>>plcnet.nets[i].cell_num;
        inf>>plcnet.nets[i].src_No;
        
        plcnet.nets[i].sink_No = new int [plcnet.nets[i].cell_num-1];
        for(j=0;j<plcnet.nets[i].cell_num-1;j++)
        {
            inf>>plcnet.nets[i].sink_No[j];
        }
    }
    inf.close();
    
    return true;
}

void rand_plc(Grids **mygrids,PLC &plcnet)
{
    int i;
    int randx,randy;
    srand((unsigned int)time(0));
    for(i=0;i<plcnet.cell_num;i++)
    {
        while(plcnet.cells[i].x==-1 || plcnet.cells[i].y==-1)
        {
            randx = rand()%plcnet.grid_col;
            randy = rand()%plcnet.grid_row;
            if(mygrids[randy][randx].cell_No==-1)
            {
                mygrids[randy][randx].cell_No = i;
                mygrids[randy][randx].x = randx;
                mygrids[randy][randx].y = randy;
                plcnet.cells[i].x = randx;
                plcnet.cells[i].y = randy;
            }
        }
    }
}

void sim_annealing(Grids **mygrids,PLC &plcnet,int init_cost,int gridsize)
{
    Point i,j;
    int k;
    
    double T = 0;
    int old_cost=0,new_cost=0;
    
    double tempT=0;
    int rand_cost[50];
    
    for(k=0;k<50;k++)
    {
        i.x = rand()%plcnet.grid_col;
        i.y = rand()%plcnet.grid_row;
        j.x = rand()%plcnet.grid_col;
        j.y = rand()%plcnet.grid_row;
        plcnet.swap_cells(mygrids,i,j);
        rand_cost[k] = plcnet.cal_total_cost();
        tempT+=rand_cost[k];
    }
    tempT=tempT/50;
    for(k=0;k<50;k++)
    {
        rand_cost[k]=(rand_cost[k]-tempT)*(rand_cost[k]-tempT);
        T+=rand_cost[k];
    }
    T=20*sqrt(T/49);
    //cout<<"Init T="<<T<<endl;
    
    double delta_cost;
    double random;
    int move_num = 10*pow((double)plcnet.cell_num,4/3);
    double R_acc = 0; //ratio of accepted move to total moves
    double alpha = 1; //reduction ratio of T
    int num_acc;
    Point D_limit;//D_limit is acutally not a point, but a dimention indicator.
    D_limit.x = plcnet.grid_col;
    D_limit.y = plcnet.grid_row;
    
    int count = 0;
    srand((unsigned int)time(0));
    while(1)
    {
        int void_step=0;
        num_acc = move_num;//as my move is a "rip-up" method, this number should be reduced from iter_num to accepted number
        for(k=0;k<move_num;k++)//O(wire_num*cell_num*iter_num)
        {
            //pick two nodes at rrandom
            i.x = rand()%plcnet.grid_col;
            i.y = rand()%plcnet.grid_row;
            j.x = rand()%plcnet.grid_col;
            j.y = rand()%plcnet.grid_row;
            while(abs(j.x-i.x)>D_limit.x)
                j.x = rand()%plcnet.grid_col;
            while(abs(j.y-i.y)>D_limit.y)
                j.y = rand()%plcnet.grid_row;
            
            if(i.x==j.x && i.y==j.y)
            {
                void_step++;
                continue;
            }
            
            if(mygrids[i.y][i.x].cell_No==-1 && mygrids[j.y][j.x].cell_No==-1)
            {
                void_step++;
                continue;
            }
            
            plcnet.find_inv(mygrids,i,j);
            old_cost = plcnet.cal_swap_cost();
            plcnet.swap_cells(mygrids,i,j);
            new_cost = plcnet.cal_swap_cost();
            
            if(new_cost>old_cost)
            {
                delta_cost = (double)(new_cost-old_cost);
                random = (double)rand()/RAND_MAX;
                //cout<<"exp(-delta_cost/T)="<<exp(-delta_cost/T)<<endl;
                if(random>exp(-delta_cost/T))
                {
                    plcnet.swap_cells(mygrids,i,j); //re-swap
                    num_acc--;
                }
            }
        }
        
        clearscreen();
        draw_init(mygrids,plcnet.grid_row,plcnet.grid_col,gridsize);
        draw_cells(plcnet,gridsize);
        draw_netline(plcnet,gridsize);
        flushinput();
        
        count++;
        new_cost = plcnet.cal_total_cost();
        R_acc = (double)(num_acc-void_step)/(double)(move_num-void_step);
        if(R_acc>0.96) {alpha = 0.5;}
        else if(R_acc>0.8) {alpha = 0.9;}
        else if(R_acc>0.15) {alpha = 0.95;}
        else {alpha = 0.8;}
        T = T*alpha;
        //cout<<"R_acc="<<R_acc<<" alpha="<<alpha<<" T="<<T<<endl<<endl;
        
        if(T<0.005*new_cost/plcnet.wire_num)
        {
            cout<<"count="<<count<<" cost="<<new_cost<<endl;
            break;
        }
        
        D_limit.x = D_limit.x*(1-0.44+R_acc);
        if(D_limit.x<1) D_limit.x=1;
        if(D_limit.x>plcnet.grid_col) D_limit.x=plcnet.grid_col;
        
        D_limit.y = D_limit.y*(1-0.44+R_acc);
        if(D_limit.y<1) D_limit.y=1;
        if(D_limit.y>plcnet.grid_row) D_limit.y=plcnet.grid_row;
        
        //cout<<"D_limit.x="<<D_limit.x<<" D_limit.y="<<D_limit.y<<endl<<endl;
    }
}

void drawscreen(void)
{
    clearscreen();
    setcolor(BLACK);
    drawtext(500,200,"Please click \"procceed\"button",2000.);
    //    setcolor(BLACK);
    //    drawrect(500,500,500+100,500+100);
}

void act_on_button_press(float x, float y) {
    
    /* Called whenever event_loop gets a button press in the graphics *
     * area.  Allows the user to do whatever he/she wants with button *
     * clicks.                                                        */
    printf("User clicked a button at coordinates (%f, %f)\n", x, y);
}

void draw_init(Grids **mygrids,int map_h,int map_w,int gridsize)
{
    //char nnnn[10];
    int gx,gy;
    int agap = gridsize/8;
    setcolor(BLACK);
    for(int i=0;i<map_h;i++)
    {
        for(int j=0;j<map_w;j++)
        {
            gx = mygrids[i][j].x * gridsize;
            gy = mygrids[i][j].y * gridsize;
            drawrect(gx+agap,gy+agap,gx+gridsize-agap,gy+gridsize-agap);
        }
    }
}

void draw_cells(PLC &plcnet,int gridsize)
{
    char nnnn[10];
    int gx,gy;
    int agap = gridsize/8;
    
    for(int i=0;i<plcnet.cell_num;i++)
    {
        gx = plcnet.cells[i].x * gridsize;
        gy = plcnet.cells[i].y * gridsize;
        setcolor(LIGHTGREY);
        fillrect(gx+agap,gy+agap,gx+gridsize-agap,gy+gridsize-agap);
        sprintf(nnnn,"%d",i);
        setcolor(WHITE);
        drawtext(gridsize/2+gx,gridsize/2+gy,nnnn,150.);
    }
}

void draw_netline(PLC &plcnet,int gridsize)
{
    int i,j;
    int gx1,gy1,gx2,gy2;
    setcolor(BLACK);
    for(i=0;i<plcnet.wire_num;i++)
    {
        gx1 = plcnet.cells[plcnet.nets[i].src_No].x * gridsize;
        gy1 = plcnet.cells[plcnet.nets[i].src_No].y * gridsize;
        for(j=0;j<plcnet.nets[i].cell_num-1;j++)
        {
            gx2 = plcnet.cells[plcnet.nets[i].sink_No[j]].x * gridsize;
            gy2 = plcnet.cells[plcnet.nets[i].sink_No[j]].y * gridsize;
            //cout<<"gx2="<<gx2<<"\tgy2="<<gy2<<endl;
            drawline(gridsize/2+gx1,gridsize/2+gy1,gridsize/2+gx2,gridsize/2+gy2);
        }
    }
}

void delay()
{
    int a=0;
    for(int i=0;i<delay_time;i++)
        for(int j=1;j<1000;j++)
            for(int k=0;k<1000;k++)
                a++;
}
