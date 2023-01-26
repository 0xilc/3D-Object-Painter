#pragma once

#include <vector>

struct HeapElement
{
    int     uniqueId;
    double  weight;
};

class BinaryHeap
{
    private:
        std::vector<HeapElement> elements;

        // akin defines private:
        void bubbleDown(int hole);
        void bubbleUp(HeapElement element, int hole);
        bool isEmpty();
        bool contains(int uniqueId);
        int heapSize;
    
    // Do not remove this the tester will utilize this
    // to access the private parts.
    friend class HW3Tester;

    protected:
    public:
        // Constructors & Destructor
                            BinaryHeap();
        //
        bool                Add(int uniqueId, double weight);
        bool                PopHeap(int& outUniqueId, double& outWeight);
        bool                ChangePriority(int uniqueId, double newWeight);
        int                 HeapSize() const;
    
};
