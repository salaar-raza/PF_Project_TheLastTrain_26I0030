#include "..\..\include\simulator.h"
#include "..\..\include\physics.h"
#include "..\..\include\switches.h"
#include "..\..\include\logger.h"
#include "..\..\include\viewer_iface.h"
#include "..\..\include\util.h"

// Modular simulation loop using primitive variables
bool checkEndConditions(int energy, int time, const bool objectives[NUM_OBJECTIVES])
{
    if(energy<=0 || time<=0)
    {
        return true;
    }

    int objectiveCount=0;
    
    for(int i=0; i<NUM_OBJECTIVES; i++)
    {
        if(objectives[i]==true)
            objectiveCount++;
    }

    if(objectiveCount==NUM_OBJECTIVES)
        return true;

    return false;
}
bool handleUnlockCabinetCommand(const char* code, int currentFloor, int currentRoom,
                                bool objectives[NUM_OBJECTIVES], int& score,
                                char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                                int& entityCount)
{
    if(!(currentFloor==5 && currentRoom==1))
        return false;

    if(!(objectives[OBJ_CLUE_4] && objectives[OBJ_CLUE_2] && objectives[OBJ_CLUE_7]))
        return false;
    else
    {
        if(code==nullptr || (code[0]!='4' || code[1]!='2' || code[2]!='7' || code[3]!='\0'))
            return false;
        else
        {
            for(int i=0; i<entityCount; i++)
            {
                if(roomEntities[currentFloor-1][currentRoom-1][i]=='^')
                {
                    roomEntities[currentFloor-1][currentRoom-1][i]=ENT_NONE;

                //shifting to left
                    for(int j=i; j<entityCount-1; j++)
                        roomEntities[currentFloor-1][currentRoom-1][j]=roomEntities[currentFloor-1][currentRoom-1][j+1];

                    roomEntities[currentFloor - 1][currentRoom - 1][entityCount-1] = ENT_NONE;
                    entityCount--;
                    objectives[OBJ_TRAIN_KEY]=true;
                    score+=SCORE_TRAIN_KEY;
                    return true;
                }
            }
        }
    }
    return false;
}
bool handleUseCommand(char itemSymbol, int currentFloor, int currentRoom,
                      int& energy, int& time, int& score,
                      char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                      bool objectives[NUM_OBJECTIVES])
{
    if(itemSymbol==ENT_MASTER_KEY)
    {
        if(unlockFloor3Door(inventory, inventoryCount))
        {
            if(currentFloor==3 && (currentRoom==1 || currentRoom==2))
            {
                applyResourceDelta(energy, time,COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
                return true;
            }
        }
    }
    else if(itemSymbol==ENT_FUSE)
    {
        if(currentFloor==3 && currentRoom==4)
        {
            if(installFuseControlPanel(inventory, inventoryCount,objectives, score))
            {
                applyResourceDelta(energy, time,COST_ITEM_USE_ENERGY, COST_ITEM_USE_TIME);
                return true;
            }
        }
    }
    return false;
}