#include "..\..\include\level.h"
#include "util.h"
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

// Modular implementation using primitive arrays for map parsing and verification
bool loadInitialState(const char* filename,
                      int& playerFloor, int& playerRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft)
    {
        ifstream File(filename);

        if(!File.is_open())
        {
            cout<<"Initial state file did not open properly."<<endl;
            return false;
        }
        string value;
        string label;

        while(File>>label>>value)
        {
            if(label=="START_ROOM:")
            {
                for(int i=0; i<4 ;i++)
                {
                    if(i==1)
                        playerFloor=value[i]-'0';
                    else if(i==3)
                        playerRoom=value[i]-'0';
                }
            }
            else if(label=="INITIAL_ENERGY:")
                energy=stoi(value);
            else if(label=="INITIAL_TIME:")
                time=stoi(value);
            else if(label=="INITIAL_SCORE:")
                score=stoi(value);
            else if(label=="INITIAL_INVENTORY:")
            {
                if(value=="NONE")
                    inventoryCount=0;
                else
                {
                    inventoryCount=0;
                    for(unsigned int i=0; i<value.length() && inventoryCount<MAX_INVENTORY_CAPACITY; i++)
                    {
                        if(value[i]!=',')
                        {
                            inventory[inventoryCount]=value[i];
                            inventoryCount++;
                        }
                    }

                }
            }
            else if(label=="PASSENGER_RESCUED:" && value=="YES")
                objectives[OBJ_PASSENGER_RESCUED]=true;
            else if(label=="CAT_RESCUED:" && value=="YES")
                objectives[OBJ_CAT_SECURED]=true;
            else if(label=="CABINET_UNLOCKED:" && value=="YES")
                objectives[OBJ_CABINET_UNLOCKED]=true;
            else if(label=="TRAIN_KEY_COLLECTED:" && value=="YES")
                objectives[OBJ_TRAIN_KEY]=true;
            else if(label=="FUSE_INSTALLED:" && value=="YES")
                objectives[OBJ_FUSE_INSTALLED]=true;
            else if(label=="LIFT_PASS_COLLECTED:" && value=="YES")
                objectives[OBJ_LIFT_PASS]=true;
            else if(label=="TRAIN_PREPARED:" && value=="YES")
                objectives[OBJ_TRAIN_PREPARED]=true;
            else if (label == "EMERGENCY_TRIPS:")
                emergencyTripsLeft = stoi(value);
        }
        File.close();
            return true;
    }

bool loadBuildingMap(const char* filename,
                     char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                     char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                     int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
    {
        ifstream File(filename);

        if(!(File.is_open()))
        {
            cout<<"The building file did not open properly."<<endl;
            return false;
        }
        //
        for(int i=0; i<NUM_FLOORS; i++)
            for(int j=0; j<ROOMS_PER_FLOOR; j++)
                entityCounts[i][j]=0;

        string line;
        while(getline(File, line))
        { 
            int floor=-1;
            if((int)line.length()>=57 && line[10]=='F')
            {
                int room=-1;
                int j=8;
                floor=(line[11]-'0')-1;
                for(int i=0; i<3; i++)
                {
                    if(line[(j+(i+1)*12)-1]=='L' && i==1)
                        horizontalConn[floor][i]='1';     //assigning '1' for lift 1
                    else
                        horizontalConn[floor][i]=line[j+(i+1)*12];
                }
                string ent_line;

                getline(File, ent_line);
                if((int)ent_line.length()>=56)
                    for(int i=9; i<56; i++)
                    {
                        if(i>8 && i<20)
                        {
                            room=1;
                        }
                        else if(i>20 && i<32)
                        {
                            room=2;
                        }
                        else if(i>32 && i<44)
                        {
                            room=3;
                        }
                        else if(i>44 && i<56)
                        {
                            room=4;
                        }
                        
                        if(ent_line[i]!=' ' && ent_line[i]!='|')
                        {
                            if(entityCounts[floor][room-1]<MAX_ENTITIES_PER_ROOM)
                            {
                                roomEntities[floor][room-1][entityCounts[floor][room-1]]=ent_line[i];
                                entityCounts[floor][room-1]++;
                            }
                        }
                    }
            }
        }
        File.close();
        return true;
    }

bool validateStartingRoom(int startFloor, int startRoom,
                          const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1])
    {
        if( (startFloor-1<0 || startFloor-1>NUM_FLOORS-1) || (startRoom-1<0 || startRoom-1>ROOMS_PER_FLOOR-1))
        {
            cout<<"Invalid initial state."<<endl;
            return false;
        }
        int floor=startFloor-1;
        int room=startRoom-1;
        bool leftBlock=false;
        bool rightBlock=false;

        if(room>0)
        {
            leftBlock = horizontalConn[floor][room-1]=='#' || horizontalConn[floor][room-1]=='L';
            rightBlock = (room==ROOMS_PER_FLOOR-1) || (horizontalConn[floor][room]=='#' || horizontalConn[floor][room]=='L');
        }
        if(leftBlock && rightBlock)
        {
            cout<<"Inescapable room."<<endl;
            return false;
        }
        return true;
    }

const char* getEntityName(char entitySymbol)
{
    
    switch (entitySymbol)
    {
        case '<': return "Master Key";
        case ')': return "Energy Pack";
        case '\\': return "Emergency Toolkit";
        case '4': return "Clue Digit 4";
        case '*': return "Fuse";
        case '~': return "Stranded Cat";
        case ';': return "Control Panel Terminal";
        case '(': return "Lift Pass";
        case '!': return "Stranded Passenger";
        case '%': return "Medical Kit";
        case '$': return "Emergency Access Card";
        case '2': return "Clue Digit 2";
        case '7': return "Clue Digit 7";
        case '^': return "Cabinet / Train Key";
        case '@': return "Cat Carrier";
        case '&': return "Observation Token";
        default:  return "Unknown Entity";
    }
}