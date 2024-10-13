#ifndef POSTS_H
#define POSTS_H

typedef struct post_t post_t;
typedef struct {
	int repo_nr;
	int dim;
	post_t **reposts;
} tree_t;

typedef struct {
	int nl;
	int *l;
	int ldim;
} like_t;

struct post_t {
	int id;
	char title[280];
	int user_id;
	int parent_id;
	like_t *like;
	tree_t *events;
};

typedef struct{
	int nr_post;
	int UID;
	int dim;
	post_t *posts;
} post_arr_t;

/**
 * Function that handles the calling of every command from task 2
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input, post_arr_t *gal);

void create_post(char *input, post_arr_t *gallery);

void create_repost(char *input, post_arr_t *gal);

void print_reposts(int id, post_t **repo, int n);

void get_reposts(char *input, post_arr_t *gal);

void com_repo(post_t **repo, int id1, int id2, int n);

void get_common_repo(char *input, post_arr_t *gal);

void add_likes(like_t *a, int id);

void like_post(char *input, post_arr_t *gal);

void get_likes(char *input, post_arr_t *gal);

void ratio(int ID, post_arr_t *gal);

void delete(char *input, post_arr_t *gal);

void free_gallery(post_arr_t *gal);

#endif // POSTS_H
