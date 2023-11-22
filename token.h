//
// Created by saeed on 11/16/2023.
//

#ifndef UNTITLED16_TOKEN_H
#define UNTITLED16_TOKEN_H
// token.h

#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    EOFTk,
    KeywordTk,
    IdentifierTk,
    OperatorTk,
    IntLiteralTk,
    ErrorTk
} TokenType;

typedef struct {
    TokenType type;
    const char *instance;
    int line;
} Token;

#endif



#endif //UNTITLED16_TOKEN_H
