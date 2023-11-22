#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "scanner.h"

// Forward declarations of all the functions for nonterminal symbols
static Node *program();
static Node *vars();
static Node *varList();
static Node *expr();
static Node *M();
static Node *N();
static Node *R();
static Node *stats();
static Node *mStat();
static Node *stat();
static Node *block();
static Node *in();
static Node *out();
static Node *if_stat();
static Node *loop();
static Node *assign();
static Node *RO();

// Utility functions
static Node *createNode(const char *name);
static void syntaxError(const char *message);
static void match(TokenType expectedType, Node *currentNode);

// Current token in the parser
static Token currentToken;

// Function to start the parsing process
Node *parser() {
    currentToken = scanner(); // Initialize the first token
    Node *root = program();   // Start parsing from the root

    // Check for end of file token
    if (currentToken.type != EOFTk) {
        syntaxError("Code ends before file");
    }
    return root;
}

// Implementations of functions for each nonterminal
//static Node *program() {
//    Node *node = createNode("<program>");
//    node->child1 = vars();
//    match(KeywordTk, node); // Expecting "main"
//    node->child2 = stats();
//    match(KeywordTk, node); // Expecting "end"
//    return node;
//}


static Node *program() {
    Node *node = createNode("<program>");
    node->child1 = vars();

    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "main") == 0) {
        match(KeywordTk, node); // Matching "main"
    } else {
        syntaxError("Expected 'main'");
    }

    node->child2 = stats();

    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "end") == 0) {
        match(KeywordTk, node); // Matching "end"
    } else {
        syntaxError("Expected 'end'");
    }

    return node;
}



// <vars> -> empty | let <varList> .
//static Node *vars() {
//    Node *node = createNode("<vars>");
//    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "let") == 0) {
//        match(KeywordTk, node); // Matching "let"
//        node->child1 = varList();
//        match(OperatorTk, node); // Matching "."
//    }
//    // If the token is not "let", it's the empty production

//    return node;
//}

static Node *vars() {
    Node *node = createNode("<vars>");

    // Check for "let" keyword
    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "let") == 0) {
        match(KeywordTk, node); // Matching "let"
        node->child1 = varList();

        // Check for "." operator
        if (currentToken.type == OperatorTk && strcmp(currentToken.instance, ".") == 0) {
            match(OperatorTk, node); // Matching "."
        } else {
            syntaxError("Expected '.'");
        }
    }
    // If the token is not "let", it's the empty production

    return node;
}




// <varList> -> id = integer | id = integer <varList>
//static Node *varList() {
//    Node *node = createNode("<varList>");
//    match(IdentifierTk, node); // Expecting an identifier
//    match(OperatorTk, node); // Expecting "="
//    match(IntLiteralTk, node); // Expecting an integer
//    if (currentToken.type == IdentifierTk) {
//        node->child1 = varList(); // Recursive call for multiple variables
//    }
//    return node;
//}

static Node *varList() {
    Node *node = createNode("<varList>");
    if (currentToken.type == IdentifierTk) {
        match(IdentifierTk, node); // Expecting an identifier
        match(OperatorTk, node); // Expecting "="
        match(IntLiteralTk, node); // Expecting an integer
        if (currentToken.type == IdentifierTk) {
            node->child1 = varList(); // Recursive call for multiple variables
        }
    } else {
        syntaxError("Expected an identifier");
    }
    return node;
}



// <exp> -> <M> + <exp> | <M> - <exp> | <M>
static Node *expr() {
    Node *node = createNode("<expr>");
    node->child1 = M();
    if (currentToken.type == OperatorTk &&
        (strcmp(currentToken.instance, "+") == 0 || strcmp(currentToken.instance, "-") == 0)) {
        match(OperatorTk, node); // Matching "+" or "-"
        node->child2 = expr();
    }
    return node;
}


// <M> -> <N> * <M> | <N>
static Node *M() {
    Node *node = createNode("<M>");
    node->child1 = N();
    if (currentToken.type == OperatorTk && strcmp(currentToken.instance, "*") == 0) {
        match(OperatorTk, node); // Matching "*"
        node->child2 = M();
    }
    return node;
}





// <N> -> <R> / <N> | - <N> | <R>
static Node *N() {
    Node *node = createNode("<N>");
    if (currentToken.type == OperatorTk && strcmp(currentToken.instance, "-") == 0) {
        match(OperatorTk, node); // Matching "-"
        node->child1 = N();
    } else {
        node->child1 = R();
        if (currentToken.type == OperatorTk && strcmp(currentToken.instance, "/") == 0) {
            match(OperatorTk, node); // Matching "/"
            node->child2 = N();
        }
    }
    return node;
}

// <R> -> [ <exp> ] | id | integer
static Node *R() {
    Node *node = createNode("<R>");
    if (currentToken.type == OperatorTk && strcmp(currentToken.instance, "[") == 0) {
        match(OperatorTk, node); // Matching "["
        node->child1 = expr();
        match(OperatorTk, node); // Matching "]"
    } else if (currentToken.type == IdentifierTk) {
        match(IdentifierTk, node); // Matching an identifier
    } else if (currentToken.type == IntLiteralTk) {
        match(IntLiteralTk, node); // Matching an integer
    } else {
        syntaxError("Invalid token in <R>");
    }
    return node;
}

// <stats> -> <stat> <mStat>
//static Node *stats() {
//    Node *node = createNode("<stats>");
//    node->child1 = stat();
//    node->child2 = mStat();
//    return node;
//}

static Node *stats() {
    Node *node = createNode("<stats>");
    node->child1 = stat();
    if (currentToken.type != KeywordTk) {
        syntaxError("Expected a valid statement");
    }
    node->child2 = mStat();
    return node;
}


// <mStat> -> empty | <stat> <mStat>
static Node *mStat() {
    Node *node = createNode("<mStat>");
// Debug print
    printf("mStat: Current Token - Type: %d, Instance: %s\n", currentToken.type, currentToken.instance);
    // Check for the start of a <stat>; if none, return empty production
    if (currentToken.type == KeywordTk || strcmp(currentToken.instance, "start") == 0) {
        node->child1 = stat();
        node->child2 = mStat();
    }
    return node;
}


// <stat> -> <in> | <out> | <block> | <if> | <loop> | <assign>
static Node *stat() {
    Node *node = createNode("<stat>");
    // Check the current token to decide which production to use
    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "scan") == 0) {
        node->child1 = in();
    } else if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "print") == 0) {
        node->child1 = out();
    } else if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "start") == 0) {
        node->child1 = block();
    } else if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "cond") == 0) {
        node->child1 = if_stat();
    } else if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "loop") == 0) {
        node->child1 = loop();
    } else if (currentToken.type == IdentifierTk) {
        node->child1 = assign();
    } else {
        syntaxError("Invalid start of <stat>");
    }
    return node;
}

// <block> -> start <vars> <stats> stop
static Node *block() {
    Node *node = createNode("<block>");
    match(KeywordTk, node); // Matching "start"
    node->child1 = vars();
    node->child2 = stats();
    match(KeywordTk, node); // Matching "stop"
    return node;
}

// <in> -> scan identifier .
static Node *in() {
    Node *node = createNode("<in>");
    match(KeywordTk, node); // Matching "scan"
    match(IdentifierTk, node); // Matching an identifier
    match(OperatorTk, node); // Matching "."
    return node;
}

// <out> -> print <exp> .
//static Node *out() {
//    Node *node = createNode("<out>");
//    match(KeywordTk, node); // Matching "print"
//    node->child1 = expr();
//    match(OperatorTk, node); // Matching "."
//    return node;
//}

static Node *out() {
    Node *node = createNode("<out>");

    // Check for "print" keyword
    if (currentToken.type == KeywordTk && strcmp(currentToken.instance, "print") == 0) {
        match(KeywordTk, node); // Matching "print"
        node->child1 = expr();

        // Check for "." operator
        if (currentToken.type == OperatorTk && strcmp(currentToken.instance, ".") == 0) {
            match(OperatorTk, node); // Matching "."
        } else {
            syntaxError("Expected '.'");
        }
    } else {
        syntaxError("Expected 'print'");
    }

    return node;
}



// <if> -> cond ( <exp> <RO> <exp> ) <stat>
static Node *if_stat() {
    Node *node = createNode("<if>");
    match(KeywordTk, node); // Matching "cond"
    match(OperatorTk, node); // Matching "("
    node->child1 = expr();
    node->child2 = RO();
    node->child3 = expr();
    match(OperatorTk, node); // Matching ")"
    node->child4 = stat();
    return node;
}

// <loop> -> loop ( <exp> <RO> <exp> ) <stat>
static Node *loop() {
    Node *node = createNode("<loop>");
    match(KeywordTk, node); // Matching "loop"
    match(OperatorTk, node); // Matching "("
    node->child1 = expr();
    node->child2 = RO();
    node->child3 = expr();
    match(OperatorTk, node); // Matching ")"
    node->child4 = stat();
    return node;
}

// <assign> -> id ~ <exp> .
static Node *assign() {
    Node *node = createNode("<assign>");
    match(IdentifierTk, node); // Matching an identifier
    match(OperatorTk, node); // Matching "~"
    node->child1 = expr();
    match(OperatorTk, node); // Matching "."
    return node;
}

// <RO> -> <= | >= | < | > | = | ~
static Node *RO() {
    Node *node = createNode("<RO>");
    if (currentToken.type == OperatorTk && (
            strcmp(currentToken.instance, "<=") == 0 || strcmp(currentToken.instance, ">=") == 0 ||
            strcmp(currentToken.instance, "<") == 0 || strcmp(currentToken.instance, ">") == 0 ||
            strcmp(currentToken.instance, "=") == 0 || strcmp(currentToken.instance, "~") == 0)) {
        match(OperatorTk, node); // Matching a relational operator
    } else {
        syntaxError("Expected a relational operator");
    }
    return node;
}


static Node *createNode(const char *name) {
    Node *node = malloc(sizeof(Node));
    node->name = strdup(name);
    node->child1 = node->child2 = node->child3 = node->child4 = NULL;
    return node;
}



static void syntaxError(const char *message) {
    fprintf(stderr, "Syntax Error: %s\n", message);
    exit(1);
}

static void match(TokenType expectedType, Node *currentNode) {
    if (currentToken.type == expectedType) {
        // Store the current token in the node
        currentNode->token = currentToken;

        // Consume the current token
        currentToken = scanner();
    } else {
        syntaxError("Unexpected token");
    }
}

void freeParseTree(Node *root) {
    if (root == NULL) return;
    freeParseTree(root->child1);
    freeParseTree(root->child2);
    freeParseTree(root->child3);
    freeParseTree(root->child4);
    free(root->name);
    free(root);
}