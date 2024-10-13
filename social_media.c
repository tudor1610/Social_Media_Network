/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"
#include "utils.h"

#define MAX_FRIENDS 520

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	list_graph_t *graph;
	graph = lg_create(MAX_FRIENDS);

	post_arr_t *gallery = malloc(sizeof(post_arr_t));
	DIE(!gallery, "Malloc failed me\n");
	gallery->nr_post = 0;
	gallery->dim = 1;
	gallery->UID = 1;
	gallery->posts = malloc(1 * sizeof(post_t));
	DIE(!gallery->posts, "Malloc failed me\n");
	char input[MAX_COMMAND_LEN];
	while (1) {
		// If fgets returns null, we reached EOF
		if (!fgets(input, MAX_COMMAND_LEN, stdin))
			break;
		#ifdef TASK_1
		handle_input_friends(graph, input);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, gallery);
		#endif

		#ifdef TASK_3
		handle_input_feed(input, gallery, graph);
		#endif
	}
	lg_free(graph);
	free_gallery(gallery);
	free_users();

	return 0;
}
