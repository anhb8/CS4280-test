
#include <stdio.h>
#include "testTree.h"

// Helper function to print spaces for indentation
static void printSpaces(int level) {
    int i;
    for (i = 0; i < level; i++) {
        printf("  ");
    }
}

// Function to print the parse tree
void printTree(Node *root, int level) {
    if (root == NULL) return;

    printSpaces(level);
    printf("%s: %s\n", root->name, root->token.instance ? root->token.instance : "");


    printTree(root->child1, level + 1);
    printTree(root->child2, level + 1);
    printTree(root->child3, level + 1);
    printTree(root->child4, level + 1);
}
