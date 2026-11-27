#include "..\..\include\viewer_iface.h"
#include "..\..\include\util.h"
#include<iostream>
#include<iomanip>
#include<stdlib.h>

using namespace std;

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
void renderRoomDetails(int floor, int room,
                       const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                       int entityCount)
{
    cout<<"+---------"<<"FLOOR:"<<playerFloor<<"-ROOM:"<<playerRoom<<"---------+"<<endl;
    cout<<"|"<<setw(5)<<"ME"<<setw(28)<<"|"<<endl;
    cout<<"|"<<setw(5)<<" ";
    for(int i=0; i<MAX_ENTITIES_PER_ROOM; i++)
    {
        cout<<setw(3)<<" ";
        if(roomEntities[playerFloor-1][playerRoom-1][i]==ENT_NONE)
            cout<<" ";
        else
            cout<<roomEntities[playerFloor-1][playerRoom-1][i];
    }
    cout<<setw(6)<<"|"<<endl;
    cout<<"+"<<setfill('-')<<setw(33)<<"+"<<endl;
}
void renderAsciiBuilding(int playerFloor,int playerRoom)
{
    cout<<"<===========================BUILDING-----MAP===========================>"<<endl;
    int floor=0, room=0;
    for(int j=0; j<16; j++)
    {
        if(j==1 || j==2) floor=5;
        else if(j==4 || j==5) floor=4;
        else if(j==7 || j==8) floor=3;
        else if(j==10 || j==11) floor=2;
        else if(j==13 || j==14) floor=1;

        if(j%3==0)
        {
            if(j==3 || j==9 || j==12)
            {
                cout<<setfill(' ')<<setw(11)<<"|____";
            }
            else
            {
                cout<<setfill(' ')<<setw(11)<<" ";
            }
            for(int i=0; i<49; i++)
            {
                if(i%12==0)
                    cout<<"+";
                else
                    cout<<"-";
            }
            if(j==0)
            {
                cout<<"_____Lift 2";
            }
            else if(j==12)
            {
                cout<<"_____|";
            }
            else if(j==3 || j==6)
            {
                cout<<setw(6)<<"|";
            }
            else if(j==9)
            {
                cout<<setw(6)<<"_  |";
            }
        }
        else
        {
            if(j==4 || j==2 || j==10 || j==8 || j==14 || j==13)
            {
                cout<<setfill(' ')<<setw(11)<<"|____";
            }
            else
            {
                cout<<setfill(' ')<<setw(11)<<" ";
            }
            for(int i=0; i<49; i++)
            {
                if(i>=0 && i<12) room =1;
                else if(i>=12 && i<24) room=2;
                else if(i>=24 && i<36) room=3;
                else if(i>=36 && i<48) room=4;

                if(i%12==0)
                {
                    if(j%3==1)
                    {
                        if(i!=48)
                        {
                            if(playerFloor==floor && playerRoom==room)
                                cout<<"| F"<<floor<<"R"<<room<<"   ME ";
                            else
                                cout<<"| F"<<floor<<"R"<<room<<"      ";
                        }
                        else
                            cout<<"|";
                    }
                    else
                        cout<<"|";

                }
                else
                {
                    if(j%3!=1)
                        cout<<" ";
                }
            }
            if(j>0 && j<12)
            {
                if(j==11)
                    cout<<setw(6)<<" |_| |";
                else if(j==10)
                    cout<<setw(6)<<" | | |";
                else
                    cout<<setw(6)<<"|";
            }
        }
        cout<<endl;
    }
}

void renderObjectivesChecklist(const bool *objectives)
{
    cout<<"<===============OBJECTIVES===============>"<<endl;
    cout<<left<<setw(25)<<"Master Key: "<<(objectives[OBJ_MASTER_KEY] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Clue 4: "<<(objectives[OBJ_CLUE_4] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Clue 2: "<<(objectives[OBJ_CLUE_2] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Clue 7: "<<(objectives[OBJ_CLUE_7] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Cabinet Unlocked: "<<(objectives[OBJ_CABINET_UNLOCKED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Train Key: "<<(objectives[OBJ_TRAIN_KEY] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Fuse Collected: "<<(objectives[OBJ_FUSE_COLLECTED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Fuse Installed: "<<(objectives[OBJ_FUSE_INSTALLED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Lift Pass: "<<(objectives[OBJ_LIFT_PASS] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Lift 2 Active: "<<(objectives[OBJ_LIFT2_ACTIVE] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Passenger Rescued: "<<(objectives[OBJ_PASSENGER_RESCUED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Cat Carrier: "<<(objectives[OBJ_CAT_CARRIER] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Cat Secured: "<<(objectives[OBJ_CAT_SECURED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Train Prepared: "<<(objectives[OBJ_TRAIN_PREPARED] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Mission Won: "<<(objectives[OBJ_MISSION_WON] ? "YES":"NO")<<endl;
    cout<<left<<setw(25)<<"Mission Failed: "<<(objectives[OBJ_MISSION_FAILED] ? "YES":"NO")<<endl;
    cout<<right;
    cout<<"<========================================>"<<endl;
}