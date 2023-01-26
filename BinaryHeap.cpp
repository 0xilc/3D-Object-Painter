#include "BinaryHeap.h"

BinaryHeap::BinaryHeap()
{
    heapSize = 0;
    elements.resize(2);
}


bool BinaryHeap::Add(int uniqueId, double weight)
{
    // check if element exists
    if (contains(uniqueId))
        return false;
    
    
    HeapElement temp = {uniqueId,weight};
    // resize if needed
    if (heapSize + 1 == elements.size())
    {
        elements.resize(elements.size() * 2 + 1);
    }
    
    // Bubble Up
    heapSize++;
    bubbleUp(temp, heapSize);
    return true;
}

bool BinaryHeap::PopHeap(int& outUniqueId, double& outWeight)
{
    // check if heap is empty
    if (isEmpty()) return false;
    
    outUniqueId = elements[1].uniqueId;
    outWeight = elements[1].weight;
    elements[1] = elements[heapSize];
    heapSize--;
    bubbleDown(1);
    return true;
}

bool BinaryHeap::ChangePriority(int uniqueId, double newWeight)
{
    // check if element exists
    if (!contains(uniqueId)) return false;
    
    // change element weight
    int i = 1;
    for (; i < heapSize+1; i++)
    {
        if (elements[i].uniqueId == uniqueId)
        {
            elements[i].weight = newWeight;
            break;
        }
    }
    // parent = i/2; childs = 2*i , 2*i+1;
    // check need for bubble up
    if (elements[i/2].weight > elements[i].weight)
    {
        bubbleUp(elements[i], i);
    }
    else
    {
        bubbleDown(i);
    }
    return true;
}

int BinaryHeap::HeapSize() const
{
    return heapSize;
}


// akin defines:

bool BinaryHeap::contains(int uniqueId)
{
    for (int i = 1; i < heapSize + 1; i++)
    {
        if (elements[i].uniqueId == uniqueId)
            return true;
    }
    return false;
}

bool BinaryHeap::isEmpty()
{
    return (heapSize == 0);
}
            
void BinaryHeap::bubbleDown(int hole)
{
    int child;
    HeapElement temp = elements[hole];
    for (; hole * 2 <= heapSize; hole = child)
    {
        child = hole * 2;
        if ( child != heapSize && elements[child].weight > elements[child + 1].weight)
        {
            child ++;
        }
        if (elements[child].weight < temp.weight)
        {
            elements[hole] = elements[child];
        }
        else
        {
            break;
        }
    }
    elements[hole] = temp;
}

void BinaryHeap::bubbleUp(HeapElement element, int hole)
{
    // initialize sentinel
    elements[0] = element;
    // bubble up
    for ( ; element.weight < elements[hole/2].weight; hole/=2)
    {
        elements[hole] = elements[hole/2];
    }
    elements[hole] = element;
}
