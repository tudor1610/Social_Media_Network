#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "users.h"
#include "posts.h"
#include "utils.h"

void create_post(char *input, post_arr_t *gallery)
{
	char *name = strtok(input, " ");
	char *text = strtok(NULL, "\n");

	// Add new post
	int n = gallery->nr_post;
	gallery->posts[n].id = gallery->UID++;
	gallery->posts[n].user_id = get_user_id(name);
	gallery->posts[n].parent_id = 0;
	gallery->posts[n].like = malloc(sizeof(like_t));
	DIE(!gallery->posts[n].like, "Malloc failed posts likes\n");
	gallery->posts[n].like->nl = 0;
	gallery->posts[n].like->ldim = 1;
	gallery->posts[n].like->l = malloc(sizeof(int));
	DIE(!gallery->posts[n].like->l, "Malloc failed post likes length\n");
	strcpy(gallery->posts[n].title, text);

	// Init reposts tree
	gallery->posts[n].events = malloc(sizeof(tree_t));
	DIE(!gallery->posts[n].events, "Malloc failed post events\n");
	gallery->posts[n].events->repo_nr = 0;
	gallery->posts[n].events->reposts = malloc(1 * sizeof(post_t *));
	DIE(!gallery->posts[n].events->reposts, "Malloc failed reposts events\n");
	gallery->posts[n].events->dim = 1;
	gallery->nr_post++;

	printf("Created %s for %s\n", text, name);
	// Realloc gallery if necessary
	if (gallery->nr_post == gallery->dim) {
		gallery->dim *= 2;
		gallery->posts = realloc(gallery->posts, gallery->dim * sizeof(post_t));
		DIE(!gallery->posts, "Realloc failed gallery posts\n");
	}
}

void create_repost(char *input, post_arr_t *gal)
{
	char *name = strtok(input, " ");
	char *id1 = strtok(NULL, " ");
	char *id2 = strtok(NULL, " ");
	int i = 0;
	while (gal->posts[i].id != atoi(id1))
		i++;
	post_t *new_repost = malloc(sizeof(post_t));
	DIE(!new_repost, "Malloc failed new repost\n");
	new_repost->id = gal->UID++;
	new_repost->like = malloc(sizeof(like_t));
	DIE(!new_repost->like, "Malloc failed new repost likes\n");
	new_repost->like->nl = 0;
	new_repost->like->ldim = 1;
	new_repost->like->l = malloc(sizeof(int));
	DIE(!new_repost->like->l, "Malloc failed new repost likes l\n");
	new_repost->user_id = get_user_id(name);
	if (id2)
		new_repost->parent_id = atoi(id2);
	else
		new_repost->parent_id = atoi(id1);

	int n = gal->posts[i].events->repo_nr++;
	gal->posts[i].events->reposts[n] = new_repost;

	// Realloc reposts array if necessary
	if (gal->posts[i].events->repo_nr == gal->posts[i].events->dim) {
		gal->posts[i].events->dim *= 2;
		int d = gal->posts[i].events->dim;
		gal->posts[i].events->reposts = realloc(gal->posts[i].events->reposts,
												d * sizeof(post_t *));
		DIE(!gal->posts[i].events->reposts,
			"Realloc failed reposts from events\n");
	}
	printf("Created repost #%d for %s\n", gal->UID - 1, name);
}

void print_reposts(int id, post_t **repo, int n)
{
	for (int i = 0; i < n; ++i) {
		if (repo[i]->parent_id == id) {
			printf("Repost #%d by %s\n", repo[i]->id,
				   get_user_name(repo[i]->user_id));
			print_reposts(repo[i]->id, repo, n);
		}
	}
}

void get_reposts(char *input, post_arr_t *gal)
{
	char *id1 = strtok(input, " ");
	char *id2 = strtok(NULL, "\n ");
	int i = 0;
	while (gal->posts[i].id != atoi(id1) && i < gal->nr_post)
		i++;
	if (id2) {
		int j = 0;
		while (gal->posts[i].events->reposts[j]->id != atoi(id2))
			j++;
		printf("Repost #%d by %s\n", gal->posts[i].events->reposts[j]->id,
			   get_user_name(gal->posts[i].events->reposts[j]->user_id));
		print_reposts(atoi(id2), gal->posts[i].events->reposts,
					  gal->posts[i].events->repo_nr);
	} else {
		printf("%s - Post by %s\n", gal->posts[i].title,
			   get_user_name(gal->posts[i].user_id));
		print_reposts(atoi(id1), gal->posts[i].events->reposts,
					  gal->posts[i].events->repo_nr);
	}
}

void com_repo(post_t **repo, int id1, int id2, int n)
{
	int p1, p2;
	for (int i = 0; i < n; ++i) {
		if (repo[i]->id == id1)
			p1 = i;
		if (repo[i]->id == id2)
			p2 = i;
	}
	if (repo[p1]->parent_id == repo[p2]->parent_id) {
		printf("%d\n", repo[p1]->parent_id);
		return;
	}
	if (id1 == repo[p2]->parent_id) {
		printf("%d\n", id1);
		return;
	}
	if (repo[p1]->parent_id == id2) {
		printf("%d\n", id2);
		return;
	}
	if (repo[p1]->parent_id < repo[p2]->parent_id)
		com_repo(repo, id1, repo[p2]->parent_id, n);
	else
		com_repo(repo, repo[p1]->parent_id, id2, n);
}

void get_common_repo(char *input, post_arr_t *gal)
{
	char *p = strtok(input, " ");
	char *id1 = strtok(NULL, " ");
	char *id2 = strtok(NULL, " ");
	int i = 0;
	while (1) {
		if (gal->posts[i].id == atoi(p))
			break;
		i++;
	}
	printf("The first common repost of %s and %s is ", id1, id2);
	com_repo(gal->posts[i].events->reposts, atoi(id1),
			 atoi(id2), gal->posts[i].events->repo_nr);
}

void add_likes(like_t *a, int id) {
	for (int i = 0; i < a->nl; ++i) {
		if (a->l[i] == id) {
			// The person liked the photo; now they dislike it
			for (int j = i; j < a->nl - 1; ++j)
				a->l[j] = a->l[j + 1];
			a->nl--;
			return;
		}
	}
	// Got here? The person didn't like the photo yet
	int n = a->nl;
	a->l[n] = id;
	a->nl++;
	if (a->nl == a->ldim) {
		// Realloc
		a->ldim *= 2;
		a->l = realloc(a->l, a->ldim * sizeof(int));
		DIE(!a->l, "Realloc failed me\n");
	}
}

void like_post(char *input, post_arr_t *gal)
{
	char *name = strtok(input, " ");
	char *id1 = strtok(NULL, " ");
	char *id2 = strtok(NULL, " ");
	int i = 0;
	while (gal->posts[i].id != atoi(id1))
		i++;

	if (id2) {
		int j = 0;
		while (gal->posts[i].events->reposts[j]->id != atoi(id2))
			j++;
		int n = gal->posts[i].events->reposts[j]->like->nl;
		add_likes(gal->posts[i].events->reposts[j]->like, get_user_id(name));
		if (n < gal->posts[i].events->reposts[j]->like->nl)
			// They liked the repost
			printf("User %s liked repost %s\n", name, gal->posts[i].title);
		else
			printf("User %s unliked repost %s\n", name, gal->posts[i].title);
	} else {
		int n = gal->posts[i].like->nl;
		add_likes(gal->posts[i].like, get_user_id(name));
		if (n < gal->posts[i].like->nl)
			printf("User %s liked post %s\n", name, gal->posts[i].title);
		else
			printf("User %s unliked post %s\n", name, gal->posts[i].title);
	}
}

void get_likes(char *input, post_arr_t *gal)
{
	char *id1 = strtok(input, " ");
	char *id2 = strtok(NULL, " ");
	int i = 0;
	while (gal->posts[i].id != atoi(id1))
		i++;
	if (id2) {
		int j = 0;
		while (gal->posts[i].events->reposts[j]->id != atoi(id2))
			j++;
		printf("Repost #%s has %d likes\n", id2,
			   gal->posts[i].events->reposts[j]->like->nl);
	} else {
		printf("Post %s has %d likes\n", gal->posts[i].title,
			   gal->posts[i].like->nl);
	}
}

void ratio(int ID, post_arr_t *gal)
{
	int i = 0;
	while (gal->posts[i].id != ID)
		i++;
	int max_likes = gal->posts[i].like->nl;
	int r = -1;
	for (int j = 0; j < gal->posts[i].events->repo_nr; ++j) {
		if (gal->posts[i].events->reposts[j]->like->nl > max_likes) {
			max_likes = gal->posts[i].events->reposts[j]->like->nl;
			r = gal->posts[i].events->reposts[j]->id;
		}
	}
	if (r != -1)
		printf("Post %d got ratio'd by repost %d\n", ID, r);
	else
		printf("The original post is the highest rated\n");
}

void delete_children(int parent_id, post_t **list, int *nr_reposts)
{
	for (int i = 0; i < (*nr_reposts); ++i) {
		if (list[i]->parent_id == parent_id) {
			int id  = list[i]->id;
			free(list[i]->like->l);
			free(list[i]->like);
			free(list[i]);
			for (int j = i; j < (*nr_reposts) - 1; ++j)
				list[j] = list[j + 1];
			(*nr_reposts)--;
			delete_children(id, list, nr_reposts);
		}
	}
}

void shl(post_t **post, int k, int len)
{
	for (int i = k; i < len - 1; ++i)
		post[i] = post[i + 1];
}

void delete(char *input, post_arr_t *gal)
{
	char *id1 = strtok(input, " ");
	char *id2 = strtok(NULL, " ");
	int i = 0;
	while (gal->posts[i].id != atoi(id1))
		i++;
	if (id2) {
		int j = 0;
		while (gal->posts[i].events->reposts[j]->id != atoi(id2))
			j++;
		// If something goes wrong in task 3, the problem is most likely here
		// It was the case. I solved it
		free(gal->posts[i].events->reposts[j]->like->l);
		free(gal->posts[i].events->reposts[j]->like);
		free(gal->posts[i].events->reposts[j]);
		shl(gal->posts[i].events->reposts, j, gal->posts[i].events->repo_nr);
		gal->posts[i].events->repo_nr--;
		delete_children(atoi(id2), gal->posts[i].events->reposts,
						&gal->posts[i].events->repo_nr);
		printf("Deleted repost #%s of post %s\n", id2, gal->posts[i].title);
	} else {
		// Free post
		if (gal->posts[i].events->repo_nr) {
			for (int j = 0; j < gal->posts[i].events->repo_nr; ++j) {
				free(gal->posts[i].events->reposts[j]->like->l);
				free(gal->posts[i].events->reposts[j]->like);
				free(gal->posts[i].events->reposts[j]);
			}
		}
		free(gal->posts[i].events->reposts);
		free(gal->posts[i].events);
		free(gal->posts[i].like->l);
		free(gal->posts[i].like);
		// Move all posts left
		for (int k = i; k < gal->nr_post - 1; ++k)
			gal->posts[k] = gal->posts[k + 1];
		gal->nr_post--;
	}
}

void handle_input_posts(char *input, post_arr_t *gallery)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");
	char *text = strtok(NULL, "\n");
	if (!cmd) {
		free(commands);
		return;
	}
	if (!strcmp(cmd, "create"))
		create_post(text, gallery);

	else if (!strcmp(cmd, "repost"))
		create_repost(text, gallery);

	else if (!strcmp(cmd, "common-repost"))
		get_common_repo(text, gallery);

	else if (!strcmp(cmd, "like"))
		like_post(text, gallery);

	else if (!strcmp(cmd, "ratio"))
		ratio(atoi(text), gallery);

	else if (!strcmp(cmd, "delete"))
		delete(text, gallery);

	else if (!strcmp(cmd, "get-likes"))
		get_likes(text, gallery);

	else if (!strcmp(cmd, "get-reposts"))
		get_reposts(text, gallery);
	free(commands);
}

void free_gallery(post_arr_t *gal)
{
	for (int i = 0; i < gal->nr_post; ++i) {
		if (gal->posts[i].events->repo_nr) {
			for (int j = 0; j < gal->posts[i].events->repo_nr; ++j) {
				free(gal->posts[i].events->reposts[j]->like->l);
				free(gal->posts[i].events->reposts[j]->like);
				free(gal->posts[i].events->reposts[j]);
			}
		}
		free(gal->posts[i].events->reposts);
		free(gal->posts[i].events);
		free(gal->posts[i].like->l);
		free(gal->posts[i].like);
	}
	free(gal->posts);
	free(gal);
}
