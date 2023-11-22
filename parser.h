
#ifndef PARSER_H
#define PARSER_H

#include "token.h"

// Define your parse tree node structure
typedef struct node {
    char *name;
    Token token;
    struct node *child1;
    struct node *child2;
    struct node *child3;
    struct node *child4;
} Node;

// Function to start the parsing process
Node *parser();

// Function to free the allocated parse tree
void freeParseTree(Node *root);

#endif
