#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "utils.h"
#include "users.h"

#define MAX_STRING_SIZE	500

// --- LINKED LIST SUPPORT START --- //

linked_list_t
*ll_create(unsigned int data_size)
{
	linked_list_t *ll;
	ll = malloc(sizeof(*ll));
	DIE(!ll, "Malloc failed linkedlist\n");
	ll->head = NULL;
	ll->data_size = data_size;
	ll->size = 0;

	return ll;
}

ll_node_t *get_nth_node(linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;

	unsigned int len = list->size - 1;
	unsigned int i;
	ll_node_t *node = list->head;

	if (len < n)
		n = len;

	for (i = 0; i < n; ++i)
		node = node->next;

	return node;
}

void
ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data)
{
	ll_node_t *prev, *curr;
	ll_node_t *new_node;

	if (!list)
		return;

	if (n > list->size)
		n = list->size;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	new_node = malloc(sizeof(*new_node));
	DIE(!new_node, "Malloc failed new_node\n");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "Malloc failed new_data\n");
	memcpy(new_node->data, new_data, list->data_size);

	new_node->next = curr;
	if (!prev)
		list->head = new_node;
	else
		prev->next = new_node;

	list->size++;
}

void ll_add_node(linked_list_t *list, const void *new_data)
{
	int i = 0;
	ll_node_t *current = list->head;
	while (current && *((int *)current->data) < *((int *)new_data)) {
		i++;
		current = current->next;
		if (current == list->head)
			break;
	}
	ll_add_nth_node(list, i, new_data);
}

ll_node_t
*ll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	ll_node_t *prev, *curr;

	if (!list || !list->head)
		return NULL;

	if (n > list->size - 1)
		n = list->size - 1;

	curr = list->head;
	prev = NULL;
	while (n > 0) {
		prev = curr;
		curr = curr->next;
		--n;
	}

	if (!prev)
		list->head = curr->next;
	else
		prev->next = curr->next;

	list->size--;
	return curr;
}

unsigned int
ll_get_size(linked_list_t *list)
{
	if (!list)
		return -1;
	return list->size;
}

void
ll_free(linked_list_t **pp_list)
{
	ll_node_t *current_node;

	if (!pp_list || !*pp_list)
		return;

	while (ll_get_size(*pp_list) > 0) {
		current_node = ll_remove_nth_node(*pp_list, 0);
		free(current_node->data);
		current_node->data = NULL;
		free(current_node);
		current_node = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}

void
ll_print_int(linked_list_t *list)
{
	ll_node_t *node = list->head;

	for (; node; node = node->next)
		printf("%d ", *(int *)node->data);
	printf("\n");
}

void
ll_print_string(linked_list_t *list)
{
	ll_node_t *node = list->head;

	for (; node; node = node->next)
		printf("%s\n", get_user_name(*(int *)node->data));
}

linked_list_t *ll_copy(linked_list_t *list) {
	linked_list_t *copy = ll_create(list->data_size);
	ll_node_t *current_node = list->head;
	while (current_node) {
		ll_add_node(copy, current_node->data);
		current_node = current_node->next;
	}
	return copy;
}

linked_list_t *ll_intersection(linked_list_t *list1, linked_list_t *list2) {
	linked_list_t *intersection = ll_create(sizeof(int));

	ll_node_t *current_node1 = list1->head;
	ll_node_t *current_node2 = list2->head;

	while (current_node1 && current_node2) {
		int data1 = *(int *)current_node1->data;
		int data2 = *(int *)current_node2->data;

		if (data1 == data2) {
			ll_add_node(intersection, &data1);
			current_node1 = current_node1->next;
			current_node2 = current_node2->next;
		} else if (data1 < data2) {
			current_node1 = current_node1->next;
		} else {
			current_node2 = current_node2->next;
		}
	}
	return intersection;
}

// --- GRAPH SUPPORT START --- //

list_graph_t
*lg_create(int nodes)
{
	list_graph_t *graph = (list_graph_t *)malloc(sizeof(list_graph_t));
	DIE(!graph, "Malloc failed graph\n");
	graph->neighbors = (linked_list_t **)
						malloc(nodes * sizeof(linked_list_t *));
	DIE(!graph->neighbors, "Malloc failed graph neighbors\n");
	for (int i = 0; i < nodes; i++)
		graph->neighbors[i] = ll_create(sizeof(int));

	graph->nodes = nodes;
	return graph;
}

void
lg_add_edge(list_graph_t *graph, int src, int dest)
{
	if (!graph || !graph->neighbors ||
		!is_node_in_graph(src, graph->nodes) ||
		!is_node_in_graph(dest, graph->nodes))
		return;

	ll_add_node(graph->neighbors[src], &dest);
	ll_add_node(graph->neighbors[dest], &src);
}

int
lg_has_edge(list_graph_t *graph, int src, int dest)
{
	ll_node_t *curr = graph->neighbors[src]->head;
	while (curr) {
		if (*((int *)curr->data) == dest)
			return 1;
		curr = curr->next;
	}
	return 0;
}

void
lg_remove_edge(list_graph_t *graph, int src, int dest)
{
	unsigned int pos1, pos2;
	ll_node_t *n1 = find_node(graph->neighbors[src], dest, &pos1);
	ll_node_t *n2 = find_node(graph->neighbors[dest], src, &pos2);

	assert(n1 == ll_remove_nth_node(graph->neighbors[src], pos1));
	assert(n2 == ll_remove_nth_node(graph->neighbors[dest], pos2));
	free(n1->data);
	free(n1);
	free(n2->data);
	free(n2);
}

void
lg_free(list_graph_t *graph)
{
	for (int i = 0; i < graph->nodes; i++)
		ll_free(&graph->neighbors[i]);

	free(graph->neighbors);
	free(graph);
}

void
lg_print_graph(list_graph_t *graph)
{
	for (int i = 0; i < graph->nodes; i++) {
		if (ll_get_size(graph->neighbors[i]) > 0) {
			char *name = get_user_name(i);
			printf("%s, %d: ", name, i);
			ll_print_int(graph->neighbors[i]);
		}
	}
}

ll_node_t *find_node(linked_list_t *ll, int data, unsigned int *pos)
{
	ll_node_t *crt = ll->head;
	unsigned int i;

	for (i = 0; i != ll->size; ++i) {
		if (data == *(int *)crt->data) {
			*pos = i;
			return crt;
		}
		crt = crt->next;
	}
	return NULL;
}

int is_node_in_graph(int n, int nodes)
{
	return n >= 0 && n < nodes;
}

linked_list_t
*lg_get_neighbours(list_graph_t *graph, int node)
{
	if (!graph || !graph->neighbors || !is_node_in_graph(node, graph->nodes))
		return NULL;

	return graph->neighbors[node];
}

// --- QUEUE SUPPORT START --- //

queue_t
*q_create(unsigned int data_size, unsigned int max_size)
{
	queue_t *q = calloc(1, sizeof(*q));
	DIE(!q, "Calloc queue failed");

	q->data_size = data_size;
	q->max_size = max_size;

	q->buff = malloc(max_size * sizeof(*q->buff));
	DIE(!q->buff, "Malloc buffer failed");

	return q;
}

unsigned int
q_get_size(queue_t *q)
{
	return !q ? 0 : q->size;
}

unsigned int
q_is_empty(queue_t *q)
{
	return !q ? 1 : !q->size;
}

void
*q_front(queue_t *q)
{
	if (!q || !q->size)
		return NULL;

	return q->buff[q->read_idx];
}

int q_dequeue(queue_t *q)
{
	if (!q || !q->size)
		return 0;

	free(q->buff[q->read_idx]);

	q->read_idx = (q->read_idx + 1) % q->max_size;
	--q->size;
	return 1;
}

int q_enqueue(queue_t *q, void *new_data)
{
	void *data;
	if (!q || q->size == q->max_size)
		return 0;

	data = malloc(q->data_size);
	DIE(!data, "Malloc data failed");
	memcpy(data, new_data, q->data_size);

	q->buff[q->write_idx] = data;
	q->write_idx = (q->write_idx + 1) % q->max_size;
	++q->size;

	return 1;
}

void q_clear(queue_t *q)
{
	unsigned int i;
	if (!q || !q->size)
		return;

	for (i = q->read_idx; i != q->write_idx; i = (i + 1) % q->max_size)
		free(q->buff[i]);

	q->read_idx = 0;
	q->write_idx = 0;
	q->size = 0;
}

void q_free(queue_t *q)
{
	if (!q)
		return;

	q_clear(q);
	free(q->buff);
	free(q);
}

