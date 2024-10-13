raluca.niculescu
tudor.brandibur

# TASK 1 - The Friend Network

## 1.1 Introduction
This file provides the implementation of various functionalities to manage and analyze a social
network graph. The graph is represented using an adjacency list, where each node represents a user
and each edge represents a friendship between two users. The following data structures and functions
are utilized to handle graph operations and provide insights about user connections:
* Graph Representation (list_graph_t):
    An adjacency list is used to represent the graph
    Each node has a linked list of its neighbors (friends)

* Linked List (linked_list_t):
    A generic linked list data structure to store lists of neighbors for each node in the graph
    Each node in the linked list (ll_node_t) contains data (representing a user) and a pointer 
    to the next node.

* Queue (queue_t):
    A queue data structure is used in the BFS algorithm to traverse the graph

## 1.2 Functionalities
### Add connection
The add command is used to create a new connection between two friends in the social network. 
It uses the `lg_add_edge` function to add an edge between the corresponding nodes in the friend
graph. This represents a new friendship between the two users.

### Remove connection
The remove command is used to remove an existing connection between two friends in the social
network. It uses the `lg_remove_edge` function to remove the edge between the corresponding
nodes. This represents the termination of the friendship between the two users.

### Distance between two friends
The `distance_two_friends` function calculates the shortest distance between two friends. This
function uses a breadth-first search algorithm to explore the graph starting from node1 to determine
the shortest path to node2. It initializes arrays `parents[]` and `dist[]` to store the parent of each
node and the distance from the source node, respectively. The BFS is implemented in the helper function
`bfs_list_graph`, which traverses the graph level by level, updating the parents and distance arrays as
it goes. Once BFS completes, the function returns the distance from node1 to node2 stored in `dist[node2]`.
If node2 is unreachable from node1 there is no path between the two friends.

### Suggestions
The `suggest_friends` function generates friend suggestions for a given node  It starts by retrieving
the direct friends (neighbors) of the specified user and iterates over each direct friend, retrieving
their friends (friends of friends) and checking if these friends of friends are not  already a direct
friend of the user. If these conditions are met, the friend of a friend is added to the suggestions
list. Finally, the function returns the list of suggested friends, providing new friend recommendations.

### Common friends
The `common_friends` function helps find friends that two users have in common within a social network
It starts by fetching the list of friends for each user. Then, it creates an empty list to store mutual
friends. The function then compares each friend of node1 with each friend of node2. If a friend is found
in both lists, it's considered a mutual friend and added to the common list. This process ensures that
all mutual friends between the two users are identified. Finally, the function returns the list of
mutual friends, providing insights into shared connections within the social network.

### Number of friends of an user
Pretty self-explanatory.

### Most popular friend
This function calculates the most popular friend of a given node in the friends graph. It does this
by first retrieving the neighbors and then iterating through each neighbor to find the one with the
most connections. For each neighbor, the function calculates the number of connections and it keeps
track of the neighbor with the most connections and the number of connections it has.


# TASK 2 - The Post and Reposts
## 2.1 Introduction
This task provides the implementation of a post array `post_arr_t`, in which every post is the root of
a repost tree, memorized using a parent_id array. The following data structures are utilised in the
immplementation:

*Array of posts (`post_arr_t`)

        typedef struct{
            int nr_post;    //number of original posts
            int UID;        //unique id; is incremented for every post/repost
            int dim;        //post_t array dimension; used for reallocating the array
            post_t *posts;  //array of posts
        } post_arr_t;

*Post / Repost (`post_t`)

        struct post_t {
            int id;             //unique id
            char title[280];    //only if it is the original post
            int user_id;        //id of the user that posted
            int parent_id;      //id of the original post; 0 if the post is the original one
            like_t *like;       //structures in which the likes are stored
            tree_t *events;     //array of reposts; only for original posts
        };

*Tree of reposts (`tree_t`)

        typedef struct {
            int repo_nr;        //number of reposts
            int dim;            //length of array; used for reallocation
            post_t **reposts;   //array of reposts
        } tree_t;

*Likes (`like_t`)

        typedef struct {
            int nl;         //number of likes
            int *l;         //array of likes; stores the id of the users that liked the post
            int ldim;       //length of array; used for reallocation
        } like_t;
## 2.2 Functionalities

### Create post
The function `create_post` receives as input the name of the user and the title of the post
and creates a new entry in the post array, incrementing its size and reallocating it if necessary.

### Create repost
The function `create_repost` works just as `create_post` function, only the repost is created and 
inserted in the `*events` field of the corresponding post. It allso reallocs the events array if
necessary.

### Common repost
This task was implemented using two functions: `get_common_repo` and `com_repo`. The first function
finds the parent post in the post array and calls `com_repo`. This is a recursive function that
compares the parent id of each repost and, if they are not equal, finds which of the to reposts is
on a lower level in the tree and moves it up (becoming its parent), then recalls itself.

### Like
This task uses two functions `like_post` and `add_likes`. Like_post is the main function, in which 
the input is extracted and the `add_likes` function is called. Here, the id of the user is inserted 
in the like array (*l) and the like count is incremented; if it allready exists, it is removed, all
the entries to its right are moved one position to the left and the like count drops by 1.

### Ratio
`ratio` compares the number of likes field (nl) from the parent post with all of its reposts and prints
the wanted result.

### Delete
The function `delete` has two parts: delete an entire post(just remove all its repost than delet it)
and delete a repost. This last part was a bit tricky because it consists in removing a repost and all
of its children from the repost array. Here is where the `delete_children` function comes into place =).
It recursively searches and removes all of the children of a repost and compresses the repost array as it
does it.

### Get Likes
The function `get_likes` returns the number of likes a post/repost has.

### Get Reposts
Using the functions `get_reposts` and `print_reposts` the reposts of any post/repost can be seen. `get_reposts`
splis the input and calls `print_reposts` which is the one that actually prints the reposts.


# TASK 3 - The Feed

### How does the feed look
The `feed` function displays the most recent posts for a user based on their direct connections.
The function retrieves the user's ID and iterates through the posts in reverse order to show the
latest posts. If the post belongs to the user or one of their direct friends (determined by the
` distance_two_friends` function), it prints the post title. The feed is limited to the specified
number of posts.

### Viewing a profile
The `view_profile` function displays all posts and reposts made by a specific user. The function iterates
through the posts to print those that the user has posted. Additionally, it checks for any posts that the
user has reposted and prints them as well. This provides a comprehensive view of the user's activity on
the platform.

### Seeing the reposts
The `friends_repost` function lists all friends of a user who have reposted a specific post. The function
retrieves the user's ID and the post ID, then iterates through the repost events of the specified post.
For each repost, it checks if the user who reposted is a direct friend of the input user by using the
distance function. If they are direct friends, it prints the name of the friend who reposted the post.

### The common group
We get the largest common group that contains the designated user by implementing the Bron–Kerbosch
algorithm recursively to find all maximal cliques. `bron_kerbosch` explores the potential maximal cliques
by recursively adding vertices to the current clique candidate (R), considering only vertices in the
potential candidate set (P) that are connected to all vertices in the current candidate and not adjacent
to any vertex in the exclusion set (X). It optimizes the search space by excluding vertices that cannot
be part of maximal cliques, thus improving the algorithm's efficiency.

The helper functions are `ll_copy` that deep copy of a linked list and `ll_intersection` that returns the
intersection of two sorted linkedlists that are the friends of different users.
 
