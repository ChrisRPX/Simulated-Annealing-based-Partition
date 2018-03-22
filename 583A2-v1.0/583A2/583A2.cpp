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
    
    int i,j;
    Grids **mygrids = NULL;
    PLC plcnet;
    
    char filename[] = "/Users/peixu/Documents/XCode/583A2/A2_Benchmarks/c880.txt";
    if(!read_file(filename,mygrids,plcnet))
    {
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
    
    /********** Initialize: place all the cells at random **********/
    rand_plc(mygrids,plcnet);
    int init_cost = plcnet.cal_cost();
    cout<<"Initial plcnet.cost="<<init_cost<<endl;
    cout<<endl;
    /**********  **********/
    
    /********** Start: draw all the grids and execute the algorithm **********/
    //init the graph
    int gridsize = 50;
    printf ("About to start graphics.\n");
    init_graphics("583 Demo", WHITE);
    init_world (0.,0.,plcnet.grid_col*gridsize,plcnet.grid_row*gridsize);
    event_loop(act_on_button_press,NULL,NULL,drawscreen);//act_on_button_press
    clearscreen();
    draw_init(mygrids,plcnet.grid_row,plcnet.grid_col,gridsize);
    draw_cells(plcnet,gridsize);
    draw_netline(plcnet,gridsize);
    //flushinput();
    //delay();
    event_loop(act_on_button_press,NULL,NULL,NULL);//act_on_button_press
    /********** End: draw all the grids and execute the algorithm **********/

    /********** Start: simulated annealing **********/
    sim_annealing(mygrids,plcnet,init_cost,gridsize);
    /********** End: simulated annealing **********/

    event_loop(act_on_button_press,NULL,NULL,NULL);//act_on_button_press
    close_graphics();
    printf ("Graphics closed down.\n");
    
//    sim_annealing(mygrids,plcnet,init_cost,gridsize);
    
    /*
    srand((unsigned int)time(0));
    cout<<time(0)<<endl;
    for(i=0;i<100;i++)
        cout<<(double)rand()/RAND_MAX<<endl;
    cout<<time(0)<<endl;
    */
    for(i=0;i<plcnet.grid_row;i++)
    {
        delete [] mygrids[i];
    }
    delete [] mygrids;
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
    
    /*
    for(i=0;i<plcnet.wire_num;i++)
    {
        cout<<"plcnet.nets[i].src_No="<<plcnet.nets[i].src_No<<endl;
        for(j=0;j<plcnet.nets[i].cell_num-1;j++)
        {
            cout<<"plcnet.nets[i].sink_No[j]="<<plcnet.nets[i].sink_No[j]<<"\t";
        }
        cout<<endl;
    }
    */
    
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
    double T=10000;
    Point i,j;
    int k;
    int old_cost,new_cost;
    double delta_cost;
    double random;
    int iter_num = 10*pow((double)plcnet.cell_num,4/3);
    int count = 0;
    cout<<"iter_num="<<iter_num<<endl;
    
    srand((unsigned int)time(0));
    while(1)
    {
        for(k=0;k<iter_num;k++)//O(wire_num*cell_num*iter_num)
        {
            old_cost = plcnet.cal_cost();//O(wire_num*cell_num)
            
            i.x = rand()%plcnet.grid_col;
            i.y = rand()%plcnet.grid_row;
            j.x = rand()%plcnet.grid_col;
            j.y = rand()%plcnet.grid_row;
            //cout<<"k="<<k<<"\ti="<<i<<"\tj="<<j<<endl;
            plcnet.swap_cells(mygrids,i,j);
            new_cost = plcnet.cal_cost();//O(wire_num*cell_num)
            if(new_cost>old_cost)
            {
                delta_cost = (double)(new_cost-old_cost);
                random = (double)rand()/RAND_MAX;
                //cout<<"random="<<random<<endl;
                //cout<<"exp(-delta_cost/T)="<<exp(-delta_cost/T)<<endl;
                if(random>exp(-delta_cost/T))
                    plcnet.swap_cells(mygrids,i,j);
            }
            if(k==0)
                cout<<"new_cost="<<new_cost<<endl;
        }
        
        clearscreen();
        draw_init(mygrids,plcnet.grid_row,plcnet.grid_col,gridsize);
        draw_cells(plcnet,gridsize);
        draw_netline(plcnet,gridsize);
        flushinput();
        
        new_cost = plcnet.cal_cost();
        if(new_cost<init_cost*0.1)
            break;
        T = T*0.99;
        count++;
        
        cout<<"count="<<count<<endl<<endl;
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
