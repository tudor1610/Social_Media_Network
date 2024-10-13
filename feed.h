#ifndef FEED_H
#define FEED_H

#include "utils.h"
#include "posts.h"
/**
 * Function that handles the calling of every command from task 3
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_feed(char *input, post_arr_t *gal, list_graph_t *graph);

void feed(char *input, post_arr_t *gal, list_graph_t *graph);

void view_profile(char *input, post_arr_t *gal);

void friends_repost(char *input, post_arr_t *gal, list_graph_t *graph);

linked_list_t *ll_copy(linked_list_t *list);

linked_list_t *ll_intersection(linked_list_t *list1, linked_list_t *list2);

int bron_kerbosch(linked_list_t *R, linked_list_t *P, linked_list_t *X,
				  list_graph_t *graph, linked_list_t **max_clique);

void common_groups(char *input, list_graph_t *graph);

#endif // FEED_H
