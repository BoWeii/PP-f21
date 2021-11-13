#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>
#include <vector>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

void bottom_up_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances,
    bool *done)
{
    int dis = distances[frontier->vertices[0]] + 1;
#pragma omp parallel
    {
        std::vector<int> temp;
#pragma omp for
        for (int i = 0; i < g->num_nodes; i++)
        {
            int start_edge = g->incoming_starts[i];
            int end_edge = (i == g->num_nodes - 1)
                               ? g->num_edges
                               : g->incoming_starts[i + 1];
            if (!done[i]) //if parents[v] = -1 then
            {
                for (int neighbor = start_edge; neighbor < end_edge; neighbor++) //for n ∈ neighbors[v] do
                {
                    int incoming = g->incoming_edges[neighbor];
                    if (done[incoming])
                    {
                        distances[i] = dis;
                        temp.push_back(i);
                        break;
                    }
                }
            }
        }
        int now = __sync_fetch_and_add(&new_frontier->count, temp.size());
        for (int i = 0; i < temp.size(); i++)
        {
            new_frontier->vertices[now + i] = temp[i];
        }
    }
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances)
{
#pragma omp parallel for
    for (int i = 0; i < frontier->count; i++) // for v ∈ frontier do
    {

        int node = frontier->vertices[i];

        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes - 1)
                           ? g->num_edges
                           : g->outgoing_starts[node + 1];
        std::vector<int> temp;
        // attempt to add all neighbors to the new frontier
        for (int neighbor = start_edge; neighbor < end_edge; neighbor++) // for n ∈ neighbors[v] do
        {
            int outgoing = g->outgoing_edges[neighbor];
            if (distances[outgoing] == NOT_VISITED_MARKER)
            {
                distances[outgoing] = distances[node] + 1;
                temp.push_back(outgoing);
            }
        }

        int now = __sync_fetch_and_add(&new_frontier->count, temp.size());
        for (int i = 0; i < temp.size(); i++)
        {
            new_frontier->vertices[now + i] = temp[i];
        }
    }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

#pragma omp parallel for
    // initialize all nodes to NOT_VISITED , // parents ← [-1,-1,. . . -1]
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0) // frontier != {} do
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier); //next ← {}
        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers // frontier ← -> next
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}

void bfs_bottom_up(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;
    bool *done = (bool *)malloc(sizeof(bool) * (graph->num_nodes));

#pragma omp parallel for
    // initialize all nodes to NOT_VISITED , // parents ← [-1,-1,. . . -1]
    for (int i = 0; i < graph->num_nodes; i++)
    {
        done[i] = false;
        sol->distances[i] = NOT_VISITED_MARKER;
    }

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    done[ROOT_NODE_ID] = true;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0) // frontier != {} do
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier); //next ← {}
        bottom_up_step(graph, frontier, new_frontier, sol->distances, done);
        #pragma omp parallel for
        for (int i = 0; i < new_frontier->count; i++)
        {
            done[new_frontier->vertices[i]] = true;
        }

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers // frontier ← -> next
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
    free(done);
}

void bfs_hybrid(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;
    bool *done = (bool *)malloc(sizeof(bool) * (graph->num_nodes));

#pragma omp parallel for
    // initialize all nodes to NOT_VISITED , // parents ← [-1,-1,. . . -1]
    for (int i = 0; i < graph->num_nodes; i++)
    {
        done[i] = false;
        sol->distances[i] = NOT_VISITED_MARKER;
    }

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    done[ROOT_NODE_ID] = true;
    sol->distances[ROOT_NODE_ID] = 0;

    bool is_top_down;
    while (frontier->count != 0) // frontier != {} do
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif
        vertex_set_clear(new_frontier); //next ← {}
        is_top_down=frontier->count < graph->num_nodes * 0.05 ? true : false;
        if(is_top_down){
            top_down_step(graph, frontier, new_frontier, sol->distances);
        }
        else{
            bottom_up_step(graph, frontier, new_frontier, sol->distances, done);
        }
        #pragma omp parallel for
        for (int i = 0; i < new_frontier->count; i++)
        {
            done[new_frontier->vertices[i]] = true;
        }

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif
        // swap pointers // frontier ← -> next
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
    free(done);
}