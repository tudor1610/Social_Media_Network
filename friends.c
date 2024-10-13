#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "friends.h"
#include "users.h"
#include "utils.h"

#define MAX_FRIENDS 520

void bfs_list_graph(list_graph_t *lg, int src, int parents[], int dist[])
{
	queue_t *queue = q_create(sizeof(int), lg->nodes);
	q_enqueue(queue, &src);
	parents[src] = -1; // Src is the root of the BFS tree
	dist[src] = 0; // Distance from src to src is 0

	while (!q_is_empty(queue)) {
		int current_node = *(int *)q_front(queue);
		q_dequeue(queue);

		// Get the neighbors of the current node and iterate through them
		linked_list_t *neighbors = lg_get_neighbours(lg, current_node);
		ll_node_t *current_neighbor = neighbors->head;
		while (current_neighbor) {
			int neighbor_node = *(int *)current_neighbor->data;
			if (parents[neighbor_node] == -1) {
				parents[neighbor_node] = current_node;
				dist[neighbor_node] = dist[current_node] + 1;
				q_enqueue(queue, &neighbor_node);
			}
			current_neighbor = current_neighbor->next;
		}
	}
	q_free(queue);
}

int distance_two_friends(list_graph_t *graph, int node1, int node2)
{
	int parents[MAX_FRIENDS];
	int dist[MAX_FRIENDS];

	// Initialize parents and dist arrays
	memset(parents, -1, sizeof(parents));
	memset(dist, 0, sizeof(dist));

	bfs_list_graph(graph, node1, parents, dist);
	return dist[node2];
}

linked_list_t *common_friends(list_graph_t *graph, int node1, int node2)
{
	linked_list_t *friends1 = lg_get_neighbours(graph, node1);
	linked_list_t *friends2 = lg_get_neighbours(graph, node2);
	linked_list_t *common = ll_create(sizeof(int));

	ll_node_t *crt1 = friends1->head;
	while (crt1) {
		ll_node_t *crt2 = friends2->head;
		while (crt2) {
			// If the two friends are common, add them to the list
			if (*(int *)crt1->data == *(int *)crt2->data)
				ll_add_node(common, &(*(int *)crt1->data));
			crt2 = crt2->next;
		}
		crt1 = crt1->next;
	}
	return common;
}

linked_list_t *suggest_friends(list_graph_t *graph, int node)
{
	linked_list_t *friends = lg_get_neighbours(graph, node);
	ll_node_t *crt1 = friends->head;
	linked_list_t *suggestions = ll_create(sizeof(int));

	while (crt1) {
		linked_list_t *friends_of_friend =
					  lg_get_neighbours(graph, *(int *)crt1->data);
		ll_node_t *crt2 = friends_of_friend->head;
		unsigned int not_used;

		while (crt2) {
			// If the friend of a friend is not a friend of the user
			// and is not the user itself, add it to the suggestions list
			if (*(int *)crt2->data != node &&
				!lg_has_edge(graph, node, *(int *)crt2->data) &&
				!find_node(suggestions, *(int *)crt2->data, &not_used)) {
				ll_add_node(suggestions, &(*(int *)crt2->data));
			}
			crt2 = crt2->next;
		}
		crt1 = crt1->next;
	}
	return suggestions;
}

char *most_popular_friend(list_graph_t *graph, int node)
{
	linked_list_t *friends = lg_get_neighbours(graph, node);
	ll_node_t *crt = friends->head;
	int number_of_friends = ll_get_size(friends);
	int max_connections = number_of_friends;
	int max_node = node;

	while (crt) {
		// Get the number of friends of the current friend
		int connections_of_friends = ll_get_size(lg_get_neighbours
												(graph, *(int *)crt->data));

		// If the current friend has more friends than the previous one
		// or the same number of friends but a smaller id, update the max
		if (connections_of_friends > max_connections ||
		    (connections_of_friends == max_connections &&
		   *(int *)crt->data < max_node)) {
			max_connections = connections_of_friends;
			max_node = *(int *)crt->data;
		}
		crt = crt->next;
	}

	if (number_of_friends == max_connections)
		return get_user_name(node);
	return get_user_name(max_node);
}

void handle_input_friends(list_graph_t *graph, char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");
	char *name1, *name2;

	if (!cmd) {
		free(commands);
		return;
	}

	if (!strcmp(cmd, "add")) {
		name1 = strtok(NULL, "\n ");
		name2 = strtok(NULL, "\n ");
		lg_add_edge(graph, get_user_id(name1), get_user_id(name2));
		printf("Added connection %s - %s\n", name1, name2);

	} else if (!strcmp(cmd, "remove")) {
		name1 = strtok(NULL, "\n ");
		name2 = strtok(NULL, "\n ");
		lg_remove_edge(graph, get_user_id(name1), get_user_id(name2));
		printf("Removed connection %s - %s\n", name1, name2);

	} else if (!strcmp(cmd, "suggestions")) {
		name1 = strtok(NULL, "\n ");
		linked_list_t *suggestions = suggest_friends(graph, get_user_id(name1));
		if (!suggestions->head) {
			printf("There are no suggestions for %s\n", name1);
		} else {
			printf("Suggestions for %s:\n", name1);
			ll_print_string(suggestions);
		}
		ll_free(&suggestions);

	} else if (!strcmp(cmd, "distance")) {
		name1 = strtok(NULL, "\n ");
		name2 = strtok(NULL, "\n ");
		int d = distance_two_friends(graph,
									get_user_id(name1), get_user_id(name2));
		if (!d)
			printf("There is no way to get from %s to %s\n", name1, name2);
		else
			printf("The distance between %s - %s is %d\n", name1, name2, d);

	} else if (!strcmp(cmd, "common")) {
		name1 = strtok(NULL, "\n ");
		name2 = strtok(NULL, "\n ");
		linked_list_t *common = common_friends(graph,
								get_user_id(name1), get_user_id(name2));
		if (!common->head && !common->size) {
			printf("No common friends for %s and %s\n", name1, name2);
		} else {
			printf("The common friends between %s and %s are:\n", name1, name2);
			ll_print_string(common);
		}
		ll_free(&common);

	} else if (!strcmp(cmd, "friends")) {
		name1 = strtok(NULL, "\n ");
		int friends = ll_get_size(lg_get_neighbours(graph, get_user_id(name1)));
		printf("%s has %d friends\n", name1, friends);

	} else if (!strcmp(cmd, "popular")) {
		name1 = strtok(NULL, "\n ");
		char *popular = most_popular_friend(graph, get_user_id(name1));
		if (strcmp(popular, name1) != 0)
			printf("%s is the most popular friend of %s\n", popular, name1);
		else
			printf("%s is the most popular\n", popular);
	}

	free(commands);
}
