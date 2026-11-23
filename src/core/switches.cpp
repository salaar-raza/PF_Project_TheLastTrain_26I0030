#include "..\..\include\switches.h"
#include"..\..\include\physics.h"
#include "util.h"

// Door lock mechanics, lifts, and cabinet verification without structures
bool unlockFloor3Door(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount)
{
    return hasItem(inventory, inventoryCount, '<');
}
bool installFuseControlPanel(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount,
                             bool objectives[NUM_OBJECTIVES], int& score)
{
    if(!(hasItem(inventory, inventoryCount, '*')))
        return false;
    else
    {
        removeItem(inventory, inventoryCount, '*');
        objectives[OBJ_FUSE_INSTALLED]=true;
        applyScoreDelta(score, 20);
        return true;
    }
}
