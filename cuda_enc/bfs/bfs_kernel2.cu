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
Kernel2(int *g_graph_mask, int *g_updating_graph_mask, int *g_graph_visited, int *g_over, int no_of_nodes)
{
for (int itr = 0; itr < NUM_ITR; ++itr) {

	int tid = blockIdx.x * MAX_THREADS_PER_BLOCK + threadIdx.x;
	if (tid < no_of_nodes) {
		if (g_updating_graph_mask[tid]) {
			g_graph_mask[tid] = true;
			g_graph_visited[tid] = true;
			*g_over = true;
			g_updating_graph_mask[tid] = false;
		}
	}

}
}
