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
                    objectives[OBJ_CABINET_UNLOCKED]=true;
                    score+=SCORE_CABINET_UNLOCK;
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

bool handleInspectCommand(int currentFloor, int currentRoom,
                          int& energy, int& time,
                          const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int entityCount,
                          bool objectives[NUM_OBJECTIVES], int& score)
{
    if(objectives[OBJ_CLUE_4] && objectives[OBJ_CLUE_2] && objectives[OBJ_CLUE_7])
        return false;

    for(int i=0; i<entityCount; i++)
    {
        if(roomEntities[currentFloor-1][currentRoom-1][i]==ENT_CLUE_2 && !(objectives[OBJ_CLUE_2]))
        {
            objectives[OBJ_CLUE_2]=true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
        else if(roomEntities[currentFloor-1][currentRoom-1][i]==ENT_CLUE_4 && !(objectives[OBJ_CLUE_4]))
        {
            objectives[OBJ_CLUE_4]=true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
        else if(roomEntities[currentFloor-1][currentRoom-1][i]==ENT_CLUE_7 && !(objectives[OBJ_CLUE_7]))
        {
            objectives[OBJ_CLUE_7]=true;
            applyScoreDelta(score, SCORE_CLUE_FOUND);
        }
    }

    applyResourceDelta(energy, time, COST_INSPECT_ENERGY, COST_INSPECT_TIME);
    return true;
}
bool handleCollectCommand(char itemSymbol, int currentFloor, int currentRoom,
                          int& energy, int& time, int& score,
                          char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                          int& entityCount,
                          char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                          bool objectives[NUM_OBJECTIVES])
{
    int itemFound=-1;
    for(int i=0; i<entityCount; i++)
    {
        if(roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol)
        {
            itemFound=i;
            break;
        }
    }
    if(itemFound==-1)
        return false;

    if(hasItem(inventory, inventoryCount, itemSymbol))
        return false;

    if (itemSymbol == ENT_CLUE_2 || itemSymbol == ENT_CLUE_4 || itemSymbol == ENT_CLUE_7)
        return false;
    
    if(itemSymbol!=ENT_ENERGY_PACK && itemSymbol!=ENT_EMERGENCY_TOOLKIT && itemSymbol!=ENT_OBSERVATION_TOKEN)
    {
        if(itemSymbol==ENT_MASTER_KEY)
        {
            objectives[OBJ_MASTER_KEY]=true;
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        }
        else if(itemSymbol==ENT_CABINET_TRAIN_KEY)
        {
            objectives[OBJ_TRAIN_KEY]=true;
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
            applyScoreDelta(score, SCORE_TRAIN_KEY);
        }
        else if(itemSymbol==ENT_FUSE)
        {
            objectives[OBJ_FUSE_COLLECTED]=true;
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        }
        else if(itemSymbol==ENT_MEDICAL_KIT)
        {
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        }
        else if(itemSymbol==ENT_LIFT_PASS)
        {
            objectives[OBJ_LIFT_PASS]=true;
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
            applyScoreDelta(score, SCORE_LIFT_PASS);
        }
        else if(itemSymbol==ENT_EMERGENCY_CARD)
        {
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        }
        else if(itemSymbol==ENT_CAT_CARRIER)
        {
            objectives[OBJ_CAT_CARRIER]=true;
            applyResourceDelta(energy, time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
        }
        if(itemSymbol==ENT_CAT)
        {
            if(objectives[OBJ_CAT_CARRIER])
                {
                    applyResourceDelta(energy,time, COST_ITEM_PICKUP_ENERGY, COST_ITEM_PICKUP_TIME);
                    applyScoreDelta(score, SCORE_SECURE_CAT);
                    objectives[OBJ_CAT_SECURED]=true;
                }
            else
                return false;
        }
        else
        {
            if(inventoryCount>=MAX_INVENTORY_CAPACITY)
                return false;
            
            inventory[inventoryCount]=itemSymbol;
            inventoryCount++;
        }
        for(int i=0; i<entityCount; i++)
        {
            if(roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol)
            {
                roomEntities[currentFloor-1][currentRoom-1][i]=ENT_NONE;

                for(int j=i; j<entityCount-1; j++)
                    roomEntities[currentFloor-1][currentRoom-1][j]=roomEntities[currentFloor-1][currentRoom-1][j+1];

                entityCount--;
                roomEntities[currentFloor-1][currentRoom-1][entityCount]=ENT_NONE;
                break;
            }
        }
        return true;
    }
    else
    {
        if(itemSymbol==ENT_ENERGY_PACK)
        {
            applyResourceDelta(energy, time, BONUS_ENERGY_PACK, 0);
        }
        else if(itemSymbol==ENT_EMERGENCY_TOOLKIT)
        {
            applyResourceDelta(energy, time, BONUS_EMERGENCY_TOOLKIT, 0);
        }
        else if(itemSymbol==ENT_OBSERVATION_TOKEN)
        {
            applyResourceDelta(energy, time, 0, BONUS_OBSERVATION_TOKEN);
        }
        for(int i=0; i<entityCount; i++)
        {
            if(roomEntities[currentFloor-1][currentRoom-1][i]==itemSymbol)
            {
                roomEntities[currentFloor-1][currentRoom-1][i]=ENT_NONE;

                for(int j=i; j<entityCount-1; j++)
                    roomEntities[currentFloor-1][currentRoom-1][j]=roomEntities[currentFloor-1][currentRoom-1][j+1];

                entityCount--;
                roomEntities[currentFloor-1][currentRoom-1][entityCount]=ENT_NONE;
                break;
            }
        }
        return true;
    }
    return false;
}
bool handleMoveCommand(int targetFloor, int targetRoom,
                       int& currentFloor, int& currentRoom,
                       int& energy, int& time,
                       const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                       const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                       int& emergencyTripsLeft, bool lift2Operational)
{
    char connType='#';
    if(!(isDirectlyConnected(currentFloor, currentRoom, targetFloor, targetRoom, horizontalConn, connType)))
        return false;

    if(!(canTraverse(targetFloor, targetRoom, connType, inventory, inventoryCount, emergencyTripsLeft, lift2Operational)))
        return false;

    if(connType==CONN_DOOR || connType==CONN_LOCKED_DOOR || connType=='S')
    {
        applyResourceDelta(energy, time, COST_MOVE_ORDINARY_ENERGY, COST_MOVE_ORDINARY_TIME);
    }
    else if(connType==CONN_WINDOW)
    {
        applyResourceDelta(energy, time, COST_MOVE_WINDOW_ENERGY, COST_MOVE_WINDOW_TIME);
    }
    else if(connType=='V')
    {
        applyResourceDelta(energy, time, COST_MOVE_LIFT_ENERGY, COST_MOVE_LIFT_TIME);
    }
    else if(connType=='X')
    {
        if(lift2Operational && hasItem(inventory, inventoryCount, ENT_LIFT_PASS))
        {
            applyResourceDelta(energy, time, COST_MOVE_LIFT_ENERGY, COST_MOVE_LIFT_TIME);
        }
        else if(hasItem(inventory, inventoryCount, ENT_EMERGENCY_CARD) && emergencyTripsLeft>0)
        {
            applyResourceDelta(energy, time, COST_MOVE_LIFT_ENERGY, COST_MOVE_LIFT_TIME);
            emergencyTripsLeft--;
        }
        else
            return false;
    }
    currentFloor=targetFloor;
    currentRoom=targetRoom;
    return true;

}
void runSimulation(int& playerFloor, int& playerRoom,
                   int& energy, int& time, int& score,
                   char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                   bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft,
                   char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                   char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM],
                   int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR])