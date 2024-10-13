#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "feed.h"
#include "users.h"
#include "utils.h"
#include "posts.h"
#include "friends.h"

void feed(char *input, post_arr_t *gal, list_graph_t *graph)
{
	char *name = strtok(input, " ");
	char *feed_size = strtok(NULL, " ");
	int size = atoi(feed_size);
	int user = get_user_id(name);

	// Iterate through the posts in reverse order to print the newest first
	for (int i = gal->nr_post - 1; i >= 0 % size; --i) {
		// Check if the post was made by the user
		if (gal->posts[i].user_id == user) {
			printf("%s: %s\n", name, gal->posts[i].title);
			size--;
		} else {
			// Check if the post was reposted by a friend of the user
			int d = distance_two_friends(graph, user, gal->posts[i].user_id);
			if (d == 1) {
				printf("%s: %s\n", get_user_name(gal->posts[i].user_id),
					   gal->posts[i].title);
				size--;
			}
		}
	}
}

void view_profile(char *input, post_arr_t *gal)
{
	char *name = strtok(input, "\n ");
	int ID = get_user_id(name);

	// Iterate through the posts and check if the user
	// has posted or reposted them
	for (int i = 0; i < gal->nr_post; ++i) {
		if (gal->posts[i].user_id == ID)
			printf("Posted: %s\n", gal->posts[i].title);
	}

	for (int i = 0; i < gal->nr_post; ++i) {
		for (int j = 0; j < gal->posts[i].events->repo_nr; ++j)
			if (gal->posts[i].events->reposts[j]->user_id == ID)
				printf("Reposted: %s\n", gal->posts[i].title);
	}
}

void friends_repost(char *input, post_arr_t *gal, list_graph_t *graph)
{
	char *name = strtok(input, " ");
	char *i = strtok(NULL, " ");
	int id = atoi(i);

	int k = 0;
	while (gal->posts[k].id != id)
		k++;

	// Check if the user has reposted the post
	for (int j = 0; j < gal->posts[k].events->repo_nr; ++j) {
		int p = distance_two_friends(graph, get_user_id(name),
									 gal->posts[k].events->reposts[j]->user_id);
		if (p == 1) {
			char *p = get_user_name(gal->posts[k].events->reposts[j]->user_id);
			printf("%s\n", p);
		}
	}
}

int bron_kerbosch(linked_list_t *R, linked_list_t *P, linked_list_t *X,
				  list_graph_t *graph, linked_list_t **max_clique) {
	if (!P->head && !X->head) {
		// Check if the current clique is bigger than the previous max_clique
		if (ll_get_size(R) > ll_get_size(*max_clique)) {
			// Free the previous max_clique and copy the new one
			ll_free(max_clique);
			*max_clique = ll_copy(R);
		}
		return 1;
	}
	linked_list_t *P_temp = ll_copy(P);
	ll_node_t *crt_node = P_temp->head;
	while (crt_node) {
		int v = *(int *)crt_node->data;

		// Create new R, P, X
		linked_list_t *R_new = ll_copy(R);
		ll_add_node(R_new, &v);

		linked_list_t *N_v = lg_get_neighbours(graph, v);

		linked_list_t *P_new = ll_intersection(P_temp, N_v);
		linked_list_t *X_new = ll_intersection(X, N_v);

		bron_kerbosch(R_new, P_new, X_new, graph, max_clique);

		ll_free(&R_new);
		ll_free(&P_new);
		ll_free(&X_new);

		// Remove v from P
		unsigned int pos;
		assert(find_node(P_temp, v, &pos));
		ll_node_t *removed_node = ll_remove_nth_node(P_temp, pos);
		crt_node = P_temp->head;
		free(removed_node->data);
		free(removed_node);

		// Add v to X
		ll_add_node(X, &v);
	}
	ll_free(&P_temp);
	return 0;
}

void common_groups(char *input, list_graph_t *graph) {
	char *name = strtok(input, " ");
	int id = get_user_id(name);

	linked_list_t *P = lg_get_neighbours(graph, id);
	linked_list_t *R = ll_create(sizeof(int));
	linked_list_t *X = ll_create(sizeof(int));
	linked_list_t *max_clique = ll_create(sizeof(int));

	ll_add_node(R, &id);

	bron_kerbosch(R, P, X, graph, &max_clique);
	printf("The closest friend group of %s is:\n", name);
	ll_print_string(max_clique);
	ll_free(&max_clique);

	// Free the lists
	ll_free(&R);
	ll_free(&X);
}

void handle_input_feed(char *input, post_arr_t *gal, list_graph_t *graph)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");
	char *text = strtok(NULL, "\n");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed"))
		feed(text, gal, graph);

	else if (!strcmp(cmd, "view-profile"))
		view_profile(text, gal);

	else if (!strcmp(cmd, "friends-repost"))
		friends_repost(text, gal, graph);

	else if (!strcmp(cmd, "common-group"))
		common_groups(text, graph);

	free(commands);
}
