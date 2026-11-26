#include "..\..\include\viewer_iface.h"
#include "..\..\include\util.h"
#include<iostream>
#include<iomanip>

// Terminal UI and dashboard rendering using primitive parameters
void renderDashboard(int playerFloor, int playerRoom, int energy, int time, int score,
                     const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                     int emergencyTripsLeft)
{
    cout<<"<=+=+=+=+=+=+=+=+=+=PLAYER-----STATS=+=+=+=+=+=+=+=+=+=>"<<endl;
    
    cout<<"Floor:"<<setw(6)<<playerFloor<<setw(3)<<"|"<<"  Room:"<<setw(5)<<playerRoom<<endl;
    cout<<"Energy:"<<setw(5)<<energy<<setw(3)<<"|"<<"  Time:"<<setw(5)<<time<<endl;
    cout<<"Score:"<<setw(6)<<score<<endl;
    cout<<"Inventory ("<<inventoryCount<<"/"<<MAX_INVENTORY_CAPACITY<<"): ";
    if(inventoryCount==0)
    {
        cout<<"Empty"<<endl;
    }
    else
    {
        for(int i=0; i<inventoryCount; i++)
        {
            cout<<getEntityName(inventory[i])<<" ("<<inventory[i]<<")";

            if(i<inventoryCount-1)
                cout<<", ";
            if(i%2==1)
                cout<<endl<<setfill(' ')<<setw(17)<<" ";
        }
    }
    if(hasItem(inventory, inventoryCount, ENT_EMERGENCY_CARD))
        cout<<endl<<"Emergency Trips Left:"<<setw(3)<<emergencyTripsLeft<<endl;
    else
    {
        (void)emergencyTripsLeft;
        cout<<endl;
    }
    cout<<"<=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+==+=+=+=+=+=+=+=+=>"<<endl;
}