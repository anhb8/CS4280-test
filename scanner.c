#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"

static FILE *inputFile;
static int lineNumber = 1;
static char currentChar;

static char getNextChar();

// Function to initialize the scanner with an input file
void initializeScanner(FILE *input) {
    inputFile = input;
    currentChar = getNextChar();
}

// Function to read the next character from the input file
static char getNextChar() {
    char c = fgetc(inputFile);
    if (c == '\n') {
        lineNumber++;
    }
    return c;
}

// Function to handle lexical errors
static void lexicalError(const char *message) {
    fprintf(stderr, "LEXICAL ERROR at line %d: %s\n", lineNumber, message);
    exit(1);
}

// Function to recognize and return the next token
Token scanner() {
    Token token;

    // Skip whitespace and comments
    while (true) {
        if (currentChar == '#') {
            // Skip comments
            while ((currentChar = getNextChar()) != '#' && currentChar != EOF) {
                // Skip comment content
            }
            if (currentChar == '#') {
                // End of comment
            } else {
                lexicalError("Unclosed comment");
            }
        } else if (!isspace(currentChar)) {
            break; // Found a non-comment, non-whitespace character
        } else {
            currentChar = getNextChar();
        }
    }

    // Check for EOF
    if (currentChar == EOF) {
        token.type = EOFTk;
        token.instance = "";
        token.line = lineNumber;
        return token;
    }

    // Check for operators and delimiters
    char buffer[3] = {currentChar, 0}; // Buffer to hold operators
    if (strchr("=<>~:+-*/%.(),{};[]", currentChar) != NULL) {
        if (strchr("=<>~:", currentChar) != NULL) {
            // Check for double-character operators
            char nextChar = getNextChar();
            if (nextChar == '=') {
                buffer[1] = nextChar;
            } else {
                ungetc(nextChar, inputFile);
            }
        }
        token.type = OperatorTk;
        token.instance = strdup(buffer);
        token.line = lineNumber;
        currentChar = getNextChar();
        return token;
    }

    // Check for integers
    if (isdigit(currentChar)) {
        char intBuffer[9] = {currentChar, 0};
        int bufferIndex = 1;
        while (isdigit(currentChar = getNextChar()) && bufferIndex < 8) {
            intBuffer[bufferIndex++] = currentChar;
        }
        ungetc(currentChar, inputFile);
        token.type = IntLiteralTk;
        token.instance = strdup(intBuffer);
        token.line = lineNumber;
        currentChar = getNextChar();
        return token;
    }

    // Check for keywords or identifiers
    if (isalpha(currentChar)) {
        char idBuffer[9] = {currentChar, 0};
        int bufferIndex = 1;
        while (isalnum(currentChar = getNextChar()) && bufferIndex < 8) {
            idBuffer[bufferIndex++] = currentChar;
        }
        ungetc(currentChar, inputFile);

        // Check if the token is a keyword
        if (strcmp(idBuffer, "start") == 0 || strcmp(idBuffer, "stop") == 0 ||
            strcmp(idBuffer, "loop") == 0 || strcmp(idBuffer, "void") == 0 ||
            strcmp(idBuffer, "var") == 0 || strcmp(idBuffer, "end") == 0 ||
            strcmp(idBuffer, "scan") == 0 || strcmp(idBuffer, "print") == 0 ||
            strcmp(idBuffer, "main") == 0 || strcmp(idBuffer, "cond") == 0 ||
            strcmp(idBuffer, "then") == 0 || strcmp(idBuffer, "let") == 0 ||
            strcmp(idBuffer, "func") == 0) {
            token.type = KeywordTk;
        } else {
            token.type = IdentifierTk;
        }
        token.instance = strdup(idBuffer);
        token.line = lineNumber;
        currentChar = getNextChar();
        return token;
    }

    // If none of the above, it's an error
    char errorMessage[2] = {currentChar, 0};
    lexicalError(errorMessage);

    // Return an error token (not reached in practice)
    token.type = ErrorTk;
    token.instance = "";
    token.line = lineNumber;
    return token;
}
