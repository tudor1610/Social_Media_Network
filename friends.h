#ifndef FRIENDS_H
#define FRIENDS_H

#include "utils.h"
#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

void bfs_list_graph(list_graph_t *lg, int src, int *parents, int *dist);
int distance_two_friends(list_graph_t *graph, int node1, int node2);
linked_list_t *common_friends(list_graph_t *graph, int node1, int node2);
linked_list_t *suggest_friends(list_graph_t *graph, int node);
char *most_popular_friend(list_graph_t *graph, int node);
void handle_input_friends(list_graph_t *graph, char *input);

#endif // FRIENDS_H
