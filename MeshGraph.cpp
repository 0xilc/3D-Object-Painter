#include "MeshGraph.h"
#include "BinaryHeap.h"

// For printing
#include <fstream>
#include <iostream>
#include <sstream>


MeshGraph::MeshGraph(const std::vector<Double3>& vertexPositions,
                     const std::vector<IdPair>& edges)
{
    // initialize adjList size
    adjList.resize(vertexPositions.size());
    for (int i = 0; i < vertexPositions.size(); i++)
    {
        Vertex temp = {i,vertexPositions[i]};
        vertices.push_back(temp);
    }
    for (int i = 0; i < edges.size(); i++)
    {
        IdPair temp = edges[i];
        adjList[temp.vertexId0].push_back(&vertices[temp.vertexId1]);
        adjList[temp.vertexId1].push_back(&vertices[temp.vertexId0]);
    }
}

double MeshGraph::AverageDistanceBetweenVertices() const
{
    double sum = 0;
    for (int i = 0; i < adjList.size(); i++)
    {
        Vertex current = vertices[i];
        std::list<Vertex*>::const_iterator it;
        for (it = adjList[i].begin(); it != adjList[i].end(); it++)
        {
            sum += Double3::Distance(current.position3D, (*it)->position3D);
        }
    }
    return sum / ((double)TotalEdgeCount()* 2);
}

double MeshGraph::AverageEdgePerVertex() const
{
    return (double)TotalEdgeCount() / (double)TotalVertexCount();
}

int MeshGraph::TotalVertexCount() const
{
    return (int)vertices.size();
}

int MeshGraph::TotalEdgeCount() const
{
    int count = 0;
    for (int i = 0; i < TotalVertexCount(); i++)
    {
        count += (int)adjList[i].size();
    }
    return count/2;
}

int MeshGraph::VertexEdgeCount(int vertexId) const
{
    if (vertexId > adjList.size() || vertexId < 0)
        return -1;
    else
        return (int)adjList[vertexId].size();
}

void MeshGraph::ImmediateNeighbours(std::vector<int>& outVertexIds,
                                    int vertexId) const
{
    outVertexIds.resize(0);
    // check such vertex exists
    if ((vertexId >= 0 && vertexId < vertices.size()))
    {
        std::list<Vertex*>::const_iterator it;
        for (it = adjList[vertexId].begin(); it != adjList[vertexId].end(); it++)
        {
            outVertexIds.push_back((*it)->id);
        }
    }
}

void MeshGraph::PaintInBetweenVertex(std::vector<Color>& outputColorAllVertex,
                                     int vertexIdFrom, int vertexIdTo,
                                     const Color& color) const
{
    BinaryHeap pq;
    std::vector<int> previous(vertices.size(),-1);
    std::vector<double> distance(vertices.size(),INFINITY);
    distance[vertexIdFrom] = 0;
    for (int i = 0; i < vertices.size(); i++)
    {
        pq.Add(i, distance[i]);
    }
    // Djikstra's algorithm
    while (pq.HeapSize() != 0)
    {
        HeapElement v;
        pq.PopHeap(v.uniqueId, v.weight);
        
        std::list<Vertex*>::const_iterator it;
        for (it = adjList[v.uniqueId].begin(); it != adjList[v.uniqueId].end(); it++)
        {
            Double3 prevVertexPos = vertices[v.uniqueId].position3D;
            Double3 nextVertexPos = (*it)->position3D;
            double newDist = distance[v.uniqueId] + Double3::Distance(prevVertexPos, nextVertexPos);
            if (newDist < distance[(*it)->id])
            {
                distance[(*it)->id] = newDist;
                previous[(*it)->id] = v.uniqueId;
                pq.ChangePriority((*it)->id, newDist);
            }
        }
    }
    
    // paint all vertices to black:
    Color black = {0,0,0};
    outputColorAllVertex.resize(vertices.size(), black);
    
    // paint the shortest path vertices with color
    int pathVertexId = vertexIdTo;
    while (pathVertexId != -1
    {
        outputColorAllVertex[pathVertexId] = color;
        pathVertexId = previous[pathVertexId];
    }
}

void MeshGraph::PaintInRangeGeodesic(std::vector<Color>& outputColorAllVertex,
                                    int vertexId, const Color& color,
                                    int maxDepth, FilterType type,
                                    double alpha) const
{
    // check such vertex exists in the graph
    if (!contains(vertexId)) return;
    
    // initialize outputColorAllVertex array
    Color black = {0,0,0};
    outputColorAllVertex.resize(vertices.size(),black);
    std::vector<double> distances(vertices.size(),INFINITY);
    std::vector<double> depth(vertices.size(),INFINITY);
    BinaryHeap que;
    int ctr = 0;
    que.Add(vertexId, ctr++);
    distances[vertexId] = 0;
    depth[vertexId] = 0;
    

    while (que.HeapSize() != 0)
    {
        // paint vertex, pop and enqueue its childs
        HeapElement currentVertex;
        que.PopHeap(currentVertex.uniqueId, currentVertex.weight);
        double filter = gaussianFilter(distances[currentVertex.uniqueId], type, alpha);
        Color colorInstance = color;
        colorInstance.r *= filter;
        colorInstance.g *= filter;
        colorInstance.b *= filter;
        outputColorAllVertex[currentVertex.uniqueId] = colorInstance;
        
        if (depth[currentVertex.uniqueId] < maxDepth)
        {
            // add adjacent vertices into the heap
            BinaryHeap pq;
            std::list<Vertex*>::const_iterator it;
            for (it = adjList[currentVertex.uniqueId].begin();
                 it != adjList[currentVertex.uniqueId].end(); it++)
            {
                // if node not visited
                if (distances[(*it)->id] == INFINITY)
                    pq.Add((*it)->id, (*it)->id);
            }
            
            while (pq.HeapSize() != 0)
            {
                HeapElement childHeapElement;
                pq.PopHeap(childHeapElement.uniqueId, childHeapElement.weight);
                Vertex child = vertices[childHeapElement.uniqueId];
                Vertex parent = vertices[currentVertex.uniqueId];
                que.Add(child.id, ctr++);
                depth[child.id] = depth[parent.id] + 1;
                distances[child.id] = distances[parent.id] +
                                      Double3::Distance(child.position3D, parent.position3D);
            }
        }
    }
}

void MeshGraph::PaintInRangeEuclidian(std::vector<Color>& outputColorAllVertex,
                                      int vertexId, const Color& color,
                                      int maxDepth, FilterType type,
                                      double alpha) const
{
    // check such vertex exists in the graph
    if (!contains(vertexId)) return;
    
    // initialize outputColorAllVertex array
    Color black = {0,0,0};
    outputColorAllVertex.resize(vertices.size(),black);
    
    std::vector<double> distances(vertices.size(),INFINITY);
    BinaryHeap que;
    que.Add(vertexId, 0);
    
    while (que.HeapSize() != 0)
    {
        // paint vertex, pop and enqueue its childs
        HeapElement currentVertex;
        que.PopHeap(currentVertex.uniqueId, currentVertex.weight);
        double distance = Double3::Distance(vertices[currentVertex.uniqueId].position3D,  vertices[vertexId].position3D);
        double filter = gaussianFilter(distance, type, alpha);
        Color colorInstance = color;
        colorInstance.r *= filter;
        colorInstance.g *= filter;
        colorInstance.b *= filter;
        outputColorAllVertex[currentVertex.uniqueId] = colorInstance;
        
        if (currentVertex.weight < maxDepth)
        {
            // add adjacent vertices in vertexId ascending order to the que
            BinaryHeap pq;
            std::list<Vertex*>::const_iterator it;
            for (it = adjList[currentVertex.uniqueId].begin();
                 it != adjList[currentVertex.uniqueId].end(); it++)
            {
                // if node not visited
                if (distances[(*it)->id] == INFINITY)
                    pq.Add((*it)->id, (*it)->id);
            }
            
            while (pq.HeapSize() != 0)
            {
                HeapElement childHeapElement;
                pq.PopHeap(childHeapElement.uniqueId, childHeapElement.weight);
                Vertex child = vertices[childHeapElement.uniqueId];
                Vertex parent = vertices[currentVertex.uniqueId];
                que.Add(child.id, currentVertex.weight + 1);
            }
        }
    }
}

void MeshGraph::WriteColorToFile(const std::vector<Color>& colors,
                                 const std::string& fileName)
{
    // IMPLEMENTED
    std::stringstream s;
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        int r = static_cast<int>(colors[i].r);
        int g = static_cast<int>(colors[i].g);
        int b = static_cast<int>(colors[i].b);

        s << r << ", " << g << ", " << b << "\n";
    }
    std::ofstream f(fileName.c_str());
    f << s.str();
}

void MeshGraph::PrintColorToStdOut(const std::vector<Color>& colors)
{
    // IMPLEMENTED
    for(int i = 0; i < static_cast<int>(colors.size()); i++)
    {
        std::cout << static_cast<int>(colors[i].r) << ", "
                  << static_cast<int>(colors[i].g) << ", "
                  << static_cast<int>(colors[i].b) << "\n";
    }
}


// akin defines:
bool MeshGraph::contains(int vertexId) const
{
    int verticesSize = (int)vertices.size();
    for (int i = 0; i < verticesSize; i++)
    {
        if (vertices[i].id == vertexId)
            return true;
    }
    return false;
}


double MeshGraph::gaussianFilter(double x, FilterType type, double alpha) const
{
    if (type == FILTER_BOX)
    {
        if (x >= -1 * alpha && x <= alpha)
            return 1.0;
        else
            return 0;
    }
    else if (type == FILTER_GAUSSIAN)
    {
        return exp ( - (x * x) / (alpha * alpha));
    }
    else
    {
        return -1.0;
    }
}

