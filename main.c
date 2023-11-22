#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "testTree.h"

int main(int argc, char *argv[]) {

    FILE *inputFile;

    // Check command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        perror("Error opening file");
        return 1;
    }
    // Initialize the scanner
    initializeScanner(inputFile);

    // Start the parsing process
    Node *parseTree = parser();

    // Print the parse tree
     printf("Parse Tree:\n");
     printTree(parseTree, 0);

    // Clean up
    freeParseTree(parseTree);
    fclose(inputFile);

    return 0;
}


