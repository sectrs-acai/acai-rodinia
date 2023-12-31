/*
 * Implementing Breadth first search on CUDA using algorithm given in HiPC'07
 * paper "Accelerating Large Graph Algorithms on the GPU using CUDA"
 *
 * Copyright (c) 2008 
 * International Institute of Information Technology - Hyderabad. 
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its 
 * documentation for educational purpose is hereby granted without fee, 
 * provided that the above copyright notice and this permission notice 
 * appear in all copies of this software and that you do not sell the software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,EXPRESS, 
 * IMPLIED OR OTHERWISE.
 *
 * Created by Pawan Harish.
 *
 * Modified by Shinpei Kato.
 */

#define NUM_ITR 1000000

__global__ void
Kernel(struct Node *g_graph_nodes, int *g_graph_edges, int *g_graph_mask, int *g_updating_graph_mask, int *g_graph_visited, int *g_cost, int no_of_nodes) 
{
for (int itr = 0; itr < NUM_ITR; ++itr) {

	int tid = blockIdx.x * MAX_THREADS_PER_BLOCK + threadIdx.x;
	if (tid < no_of_nodes) {
		if (g_graph_mask[tid]) {
			g_graph_mask[tid] = false;
			for(int i = g_graph_nodes[tid].starting; i < (g_graph_nodes[tid].no_of_edges + g_graph_nodes[tid].starting); i++) {
				int id = g_graph_edges[i];
				if (!g_graph_visited[id]) {
					g_cost[id] = g_cost[tid] + 1;
					g_updating_graph_mask[id] = true;
				}
			}
		}
	}

}
}
