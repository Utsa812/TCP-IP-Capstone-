
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"

/* Create a new graph with the given topology name */
graph_t *create_new_graph(char *topology_name) {
    graph_t *graph = (graph_t *)calloc(1, sizeof(graph_t));
    if (!graph) {
        printf("Error: Memory allocation failed for graph\n");
        return NULL;
    }

    strncpy(graph->topology_name, topology_name, TOPOLOGY_NAME_SIZE);
    graph->topology_name[TOPOLOGY_NAME_SIZE - 1] = '\0';
    graph->node_list = NULL;

    printf("Created new graph: %s\n", topology_name);
    return graph;
}

/* Create a new node in the graph */
node_t *create_graph_node(graph_t *graph, char *node_name) {
    if (!graph || !node_name) {
        printf("Error: Invalid graph or node name\n");
        return NULL;
    }

    // Check if node with same name already exists
    node_t *current = graph->node_list;
    while (current) {
        if (strcmp(current->node_name, node_name) == 0) {
            printf("Error: Node with name '%s' already exists\n", node_name);
            return NULL;
        }
        current = current->next;
    }

    // Create new node
    node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
    if (!new_node) {
        printf("Error: Memory allocation failed for node\n");
        return NULL;
    }

    // Initialize node properties
    strncpy(new_node->node_name, node_name, NODE_NAME_SIZE);
    new_node->node_name[NODE_NAME_SIZE - 1] = '\0';
    new_node->intf_list = NULL;
    new_node->next = NULL;

    // Add to graph's node list
    if (!graph->node_list) {
        graph->node_list = new_node;
    } else {
        current = graph->node_list;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }

    printf("Created new node: %s\n", node_name);
    return new_node;
}

/* Insert link between two nodes with interface names and cost */
void insert_link_between_two_nodes(node_t *node1, node_t *node2,
                                  char *from_if_name, char *to_if_name,
                                  unsigned int cost) {
    if (!node1 || !node2) {
        printf("Error: Invalid nodes provided\n");
        return;
    }

    // Create interface for node1
    intf_t *intf1 = (intf_t *)calloc(1, sizeof(intf_t));
    if (!intf1) {
        printf("Error: Memory allocation failed for interface 1\n");
        return;
    }

    // Create interface for node2
    intf_t *intf2 = (intf_t *)calloc(1, sizeof(intf_t));
    if (!intf2) {
        printf("Error: Memory allocation failed for interface 2\n");
        free(intf1);
        return;
    }

    // Initialize interface 1
    strncpy(intf1->intf_name, from_if_name, INTF_NAME_SIZE);
    intf1->intf_name[INTF_NAME_SIZE - 1] = '\0';
    intf1->link_cost = cost;
    intf1->att_node = node1;
    intf1->link = intf2; // Point to other interface

    // Initialize interface 2
    strncpy(intf2->intf_name, to_if_name, INTF_NAME_SIZE);
    intf2->intf_name[INTF_NAME_SIZE - 1] = '\0';
    intf2->link_cost = cost;
    intf2->att_node = node2;
    intf2->link = intf1; // Point to other interface

    // Add interface 1 to node1's interface list
    if (!node1->intf_list) {
        node1->intf_list = intf1;
    } else {
        intf_t *temp = node1->intf_list;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = intf1;
    }

    // Add interface 2 to node2's interface list
    if (!node2->intf_list) {
        node2->intf_list = intf2;
    } else {
        intf_t *temp = node2->intf_list;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = intf2;
    }

    printf("Created link: %s[%s] <--(%u)--> %s[%s]\n",
           node1->node_name, from_if_name, cost, node2->node_name, to_if_name);
}

/* Display the entire graph topology */
void dump_graph(graph_t *graph) {
    if (!graph) {
        printf("Error: Invalid graph\n");
        return;
    }

    printf("\n=================== Graph Topology: %s ===================\n",
           graph->topology_name);

    node_t *node = graph->node_list;
    if (!node) {
        printf("Graph is empty!\n");
        return;
    }

    while (node) {
        printf("\nNode: %s\n", node->node_name);
        printf("  Interfaces:\n");

        intf_t *intf = node->intf_list;
        if (!intf) {
            printf("    No interfaces\n");
        } else {
            while (intf) {
                if (intf->link && intf->link->att_node) {
                    printf("    %s ---[cost:%u]--- %s\n",
                           intf->intf_name,
                           intf->link_cost,
                           intf->link->att_node->node_name);
                } else {
                    printf("    %s ---[cost:%u]--- <disconnected>\n",
                           intf->intf_name, intf->link_cost);
                }
                intf = intf->next;
            }
        }
        node = node->next;
    }
    printf("==========================================================\n\n");
}
