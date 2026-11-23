#include "..\..\include\physics.h"
#include "..\..\include\util.h"
#include "..\..\include\config.h"
#include<cstdlib>

// Movement validation and resource adjustments without structures
bool isDirectlyConnected(int fromF, int fromR, int toF, int toR,
                         const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1],
                         char& outConnType)
{
    outConnType=' ';

    if(fromF<1 || fromF>NUM_FLOORS)
    {
        outConnType='#';
        return false;
    }
    if(toF<1 || toF>NUM_FLOORS)
    {
        outConnType='#';
        return false;
    }
    if(fromR<1 || fromR>ROOMS_PER_FLOOR)
    {
        outConnType='#';
        return false;
    }
    if(toR<1 || toR>ROOMS_PER_FLOOR)
    {
        outConnType='#';
        return false;
    }

    //Veritcal movement

    //1-Staircase

    if(fromR==1 && toR==1 && abs(fromF-toF)==1)
    {
        if((fromF==3 && toF==4)||(fromF==4 && toF==3))
        {
            outConnType='#';
            return false;
        }
        outConnType='S';
        return true;
    }

    //2- Lift 1
    if(((fromR==3 || fromR==2) && (toR==3 || toR==2))
        &&((fromF==1 && toF==2)||(fromF==2 && toF==1)))
    {
        outConnType='V';
        return true;
    }

    //3- Lift 2
    if(fromR==4 && toR==4)
    {
        if((fromF==2 && toF==4)||(fromF==4 && toF==2))
        {
            outConnType='X';
            return true;
        }
        else if((fromF==2 && toF==5)||(fromF==5 && toF==2))
        {
            outConnType='X';
            return true;
        }
        else if((fromF==4 && toF==5)||(fromF==5 && toF==4))
        {
            outConnType='X';
            return true;
        }
        outConnType='#';
        return false;
    }

    //Horizontal movement
    if(fromF==toF && abs(fromR-toR)==1)
    {
        int connectionIndex= ((fromR<toR ? fromR : toR)-1);
        int floor=fromF-1;

        if(horizontalConn[floor][connectionIndex]=='#')
        {
            outConnType='#';
            return false;
        }
        outConnType=horizontalConn[floor][connectionIndex];
        return true;
    }
    return false;
}

bool hasItem(const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, char itemSymbol)
{
    for(int i=0; i<inventoryCount; i++)
    {
        if(inventory[i]==itemSymbol)
        {
            return true;
        }
    }
    return false;
}

bool canTraverse(int targetFloor, int targetRoom, char connType,
                 const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount,
                 int emergencyTripsLeft, bool lift2Operational)
{
    (void)targetRoom; (void)targetFloor;

    if(connType=='#')
    {
        return false;
    }
    else if(connType=='L')
    {
        if(hasItem(inventory, inventoryCount, '<'))
        {
            return true;
        }
        return false;
    }
    else if(connType=='X')
    {
        if(lift2Operational && hasItem(inventory, inventoryCount, '('))
        {
            return true;
        }
        else if(hasItem(inventory,inventoryCount, '$') && emergencyTripsLeft>0)
        {
            return true;
        }
        return false;
    }
    return true;
}

bool addItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
    if(inventoryCount>=MAX_INVENTORY_CAPACITY)
        return false;

    inventory[inventoryCount]=itemSymbol;
    inventoryCount++;
    return true;
}
bool removeItem(char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, char itemSymbol)
{
    for(int i=0; i<inventoryCount; i++)
    {
        if(inventory[i]==itemSymbol)
        {
            inventory[i]=ENT_NONE;

            //shifting to the left
            for(int j=i; j<inventoryCount-1; j++)
            {
                inventory[j]=inventory[j+1];
            }
            inventory[inventoryCount-1]=ENT_NONE;
            inventoryCount--;
            return true;
        }
    }
    return false;
}
void applyResourceDelta(int& energy, int& time, int deltaEnergy, int deltaTime)
{
    energy+=deltaEnergy;

    if (energy>MAX_ENERGY)
        energy=MAX_ENERGY;
    else if(energy<0)
        energy=0;

    
    time+=deltaTime;

    if (time>MAX_TIME)
        time=MAX_TIME;
    else if(time<0)
        time=0;
}
void applyScoreDelta(int& score, int deltaScore)
{
    score+=deltaScore;

    if(score<0)
        score=0;
}