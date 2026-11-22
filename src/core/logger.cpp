#include "..\..\include\logger.h"
#include "..\..\include\util.h"
#include<iostream>
#include<string>
#include<fstream>

using namespace std;

// File persistence for savegame.txt and game_summary.txt using primitive variables
bool saveGameStateToFile(const char* filename,
                         int playerFloor, int playerRoom,
                         int energy, int time, int score,
                         const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                         const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft,
                         const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                         const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    ofstream outFile(filename);

    if(!(outFile.is_open()))
    {
        cout<<"Game could not be saved  properly."<<endl;
        return false;
    }

    outFile<<"START_ROOM: F"<<playerFloor<<"R"<<playerRoom<<"\n\n";
    outFile<<"ENERGY: "<<energy<<"\n";
    outFile<<"TIME: "<<time<<"\n";
    outFile<<"SCORE: "<<score<<"\n\n";
    outFile<<"INVENTORY: ";
    if(inventoryCount!=0)
    {
        for(int i=0; i<inventoryCount; i++)
        {
            outFile<<inventory[i]<<",";
        }
        outFile<<"\n\n";
    }
    else
        outFile<<"NONE\n\n";

    outFile<<"CLUES_COLLECTED: "<<
        (objectives[OBJ_CLUE_4] ? "4":"")<<
        (objectives[OBJ_CLUE_2] ? "2":"")<<
        (objectives[OBJ_CLUE_7] ? "7":"");
    
    if(objectives[OBJ_CLUE_4] || objectives[OBJ_CLUE_2] || objectives[OBJ_CLUE_7])
        outFile<<"\n";
    else
        outFile<<"NONE\n";
    outFile<<"MASTER_KEY: "<< (objectives[OBJ_MASTER_KEY] ? "YES\n" : "NO\n");
    outFile<<"CABINET_UNLOCKED: "<< (objectives[OBJ_CABINET_UNLOCKED] ? "YES\n" : "NO\n");
    outFile<<"TRAIN_KEY: "<< (objectives[OBJ_TRAIN_KEY] ? "YES\n" : "NO\n");
    outFile<<"FUSE_COLLECTED: "<< (objectives[OBJ_FUSE_COLLECTED] ? "YES\n" : "NO\n");
    outFile<<"FUSE_INSTALLED: "<< (objectives[OBJ_FUSE_INSTALLED] ? "YES\n" : "NO\n");
    outFile<<"LIFT_PASS: "<< (objectives[OBJ_LIFT_PASS] ? "YES\n" : "NO\n");
    outFile<<"LIFT2_ACTIVE: "<< (objectives[OBJ_LIFT2_ACTIVE] ? "YES\n" : "NO\n");
    outFile<<"PASSENGER_RESCUED: "<< (objectives[OBJ_PASSENGER_RESCUED] ? "YES\n" : "NO\n");
    outFile<<"CAT_CARRIER: "<< (objectives[OBJ_CAT_CARRIER] ? "YES\n" : "NO\n");
    outFile<<"CAT_RESCUED: "<< (objectives[OBJ_CAT_SECURED] ? "YES\n" : "NO\n");
    outFile<<"TRAIN_PREPARED: "<< (objectives[OBJ_TRAIN_PREPARED] ? "YES\n" : "NO\n");
    outFile<<"MISSION_WON: "<< (objectives[OBJ_MISSION_WON] ? "YES\n" : "NO\n");
    outFile<<"MISSION_FAILED: "<< (objectives[OBJ_MISSION_FAILED] ? "YES\n" : "NO\n");

    outFile<<"EMERGENCY_TRIPS: "<<emergencyTripsLeft<<"\n";

    for(int f=0; f<NUM_FLOORS; f++)
    {
        for(int r=0; r<ROOMS_PER_FLOOR; r++)
        {
            outFile<<"F"<<f+1<<"R"<<r+1<<": ";
            if(entityCounts[f][r]==0)
                outFile<<"NONE";
            else
            {
                for(int ent=0; ent<entityCounts[f][r]; ent++)
                {
                    outFile<<roomEntities[f][r][ent]<<",";
                }
            }
            outFile<<"\n";
        }
    }
    outFile.close();
    return true;
}
bool loadGameStateFromFile(const char* filename,
                           int& playerFloor, int& playerRoom,
                           int& energy, int& time, int& score,
                           char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                           bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                           char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                           int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])
{
    playerFloor=0;
    playerRoom=0;
    energy=0;
    time=0;
    score=0;
    inventoryCount=0;
    emergencyTripsLeft=0;

    for(int i=0; i<MAX_INVENTORY_CAPACITY; i++)
    {
        inventory[i]=ENT_NONE;
    }
    for(int i=0; i<NUM_OBJECTIVES; i++)
    {
        objectives[i]=false;
    }
    for(int f=0; f<NUM_FLOORS; f++)
    {
        for(int r=0; r<ROOMS_PER_FLOOR; r++)
        {
            entityCounts[f][r]=0;
            for(int ent=0; ent<MAX_ENTITIES_PER_ROOM; ent++)
            {
                roomEntities[f][r][ent]=ENT_NONE;
            }
        }
    }
    ifstream File(filename);

    if(!(File.is_open()))
    {
        cout<<"Save file could not be opened properly."<<endl;
        return false;
    }

    string label="";
    string value="";

    while(File>>label>>value)
    {
        if(label=="START_ROOM:")
        {
            playerFloor=value[1]-'0';
            playerRoom=value[3]-'0';
        }
        else if(label=="ENERGY:")
            energy=stoi(value);
        else if(label=="TIME:")
            time=stoi(value);
        else if(label=="SCORE:")
            score=stoi(value);

        else if(label=="INVENTORY:")
        {
            if(value!="NONE")
            {
                for(int i=0; i<(int)value.length() && inventoryCount<MAX_INVENTORY_CAPACITY; i++)
                    {
                        if(value[i]!=',')
                        {
                            inventory[inventoryCount]=value[i];
                            inventoryCount++;
                        }
                    }
            }
        }
        else if(label=="MASTER_KEY:" && value=="YES")
            objectives[OBJ_MASTER_KEY]=true;
        else if(label=="CLUES_COLLECTED:")
        {
            if(value!="NONE")
            {
                if((int)value.length()==3)
                {
                    objectives[OBJ_CLUE_4]=true;
                    objectives[OBJ_CLUE_2]=true;
                    objectives[OBJ_CLUE_7]=true;
                }
                else if((int)value.length()==2)
                {
                    if(value[0]=='4')
                    {
                        objectives[OBJ_CLUE_4]=true;
                        if(value[1]=='2')
                            objectives[OBJ_CLUE_2]=true;
                        else
                            objectives[OBJ_CLUE_7]=true;
                    }
                    else if(value[0]=='2')
                    {
                        objectives[OBJ_CLUE_2]=true;
                        objectives[OBJ_CLUE_7]=true;
                    }
                }
                else if((int)value.length()==1)
                {
                    if(value[0]=='4') objectives[OBJ_CLUE_4]=true;
                    else if(value[0]=='2') objectives[OBJ_CLUE_2]=true;
                    else if(value[0]=='7') objectives[OBJ_CLUE_7]=true;
                }
            }
        }
        else if(label=="CABINET_UNLOCKED:" && value=="YES")
            objectives[OBJ_CABINET_UNLOCKED]=true;
        else if(label=="TRAIN_KEY:" && value=="YES")
            objectives[OBJ_TRAIN_KEY]=true;
        else if(label=="FUSE_COLLECTED:" && value=="YES")
            objectives[OBJ_FUSE_COLLECTED]=true;
        else if(label=="FUSE_INSTALLED:" && value=="YES")
            objectives[OBJ_FUSE_INSTALLED]=true;
        else if(label=="LIFT_PASS:" && value=="YES")
            objectives[OBJ_LIFT_PASS]=true;
        else if(label=="LIFT2_ACTIVE:" && value=="YES")
            objectives[OBJ_LIFT2_ACTIVE]=true;
        else if(label=="PASSENGER_RESCUED:" && value=="YES")
            objectives[OBJ_PASSENGER_RESCUED]=true;
        else if(label=="CAT_CARRIER:" && value=="YES")
            objectives[OBJ_CAT_CARRIER]=true;
        else if(label=="CAT_RESCUED:" && value=="YES")
            objectives[OBJ_CAT_SECURED]=true;
        else if(label=="TRAIN_PREPARED:" && value=="YES")
            objectives[OBJ_TRAIN_PREPARED]=true;
        else if(label=="MISSION_WON:" && value=="YES")
            objectives[OBJ_MISSION_WON]=true;
        else if(label=="MISSION_FAILED:" && value=="YES")
            objectives[OBJ_MISSION_FAILED]=true;
        else if(label=="EMERGENCY_TRIPS:")
            emergencyTripsLeft=stoi(value);
        else
        {
            for(int f=0; f<NUM_FLOORS; f++)
            {
                
                for(int r=0; r<ROOMS_PER_FLOOR; r++)
                {
                    string new_label="F"+to_string(f+1)+"R"+to_string(r+1)+":";

                    if(label==new_label)
                    {
                        if(value!="NONE")
                        {
                            for(int k=0; k<(int)value.length() && entityCounts[f][r]<MAX_ENTITIES_PER_ROOM; k++)
                            {
                                if(value[k]!=',')
                                {
                                    roomEntities[f][r][entityCounts[f][r]]=value[k];
                                    entityCounts[f][r]++;
                                }
                            }   
                        }
                    }
                }
            }
        }
    }
    File.close();
    return true;
}