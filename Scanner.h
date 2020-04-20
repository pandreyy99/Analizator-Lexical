//
// Created by Andrei on 20-Apr-20.
//

#ifndef LEXICAL_ANALYZER_SCANNER_H
#define LEXICAL_ANALYZER_SCANNER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <iostream>
#include <ostream>

#define lambda '~'
#define ANYTHING " `~1234567890-=!?@#$%^&*()_+[]{}<>;:,.'/|\"\\\tqwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"
#define HEXCHRLWR "abcdef"
#define HEXCHRUPP "ABCDEF"
#define BINCHR "01"
#define PLUSMINUS "-+"
#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJLKLMNOPQRSTUVWXYZ"
#define NUMBERS "0123456789"
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

using namespace std;

typedef struct {
    int start_state;
    char symbol;
    int end_state;
} transition;

typedef struct {
    int start_state;
    string symbols;
    int end_state;
} transitions;

enum states {
    initState,

    opPlusIntermState,
    opMinusIntermState,
    opOrIntermState,
    opEqualIntermState,
    opSlashIntermState,
    opAndIntermState,

    powIntState,
    powFloatState,
    intExponentialState,
    floatExponentialState,
    floatLoopState,
    floatPointState,

    zeroNrState,
    binIdState,
    binLoopState,
    hexIdState,
    hexLoopState,

    lineCommLoopState,
    multipleLineCommState,
    multipleStarCommState,

    chrStartState,
    chrEndState,
    chrBackState,

    strEndState,
    strLoopState,
    strSlashState,

    forbiddenState,
    errorState,

    whereFinalStatesBegin,

    identifierState,
    literalIntState,
    literalFloatState,
    literalChrState,
    literalStringState,

    operatorState,
    separatorState,
    commState,

    delimState,
    spaceState,
    tabState,
    newLineState,

    whereStatesEnd
};

class Scanner {
private:
    FILE *in;
    char *filename;

    int previousState;
    int currentState;

    int lposs;
    int rposs;

    string currentContent;
    string previousContent;

    transition lambdaTrans[14] = {
            // Number transitions
            {intExponentialState,   lambda, literalIntState},
            {floatLoopState,        lambda, literalFloatState},
            {floatExponentialState, lambda, literalFloatState},
            {binLoopState,          lambda, literalIntState},
            {hexLoopState,          lambda, literalIntState},
            {zeroNrState,           lambda, literalIntState},

            // Operators transitions
            {opPlusIntermState,     lambda, operatorState},
            {opMinusIntermState,    lambda, operatorState},
            {opSlashIntermState,    lambda, operatorState},
            {opEqualIntermState,    lambda, operatorState},
            {opAndIntermState,      lambda, operatorState},
            {opOrIntermState,       lambda, operatorState},

            // Others
            {lineCommLoopState,     lambda, commState}
    };

    transition simpleTrans[47] = {
            // Delimiters related
            {initState,             ';',  delimState},
            {initState,             ' ',  spaceState},
            {initState,             '\n', newLineState},
            {initState,             '0',  zeroNrState},
            {initState,             '\r', tabState},

            // Numbers related
            {literalIntState,       '.',  floatPointState},
            {zeroNrState,           '.',  floatPointState},
            {zeroNrState,           'b',  binIdState},
            {zeroNrState,           'x',  hexIdState},

            // Exponential numbers
            {literalIntState,       'e',  powIntState},
            {powIntState,           '+',  intExponentialState},
            {literalFloatState,     'e',  powFloatState},
            {powIntState,           '-',  floatExponentialState},
            {floatLoopState,        'e',  powFloatState},
            {intExponentialState,   '.',  forbiddenState},
            {floatExponentialState, '.',  forbiddenState},

            // Comment
            {opSlashIntermState,    '/',  lineCommLoopState},
            {opSlashIntermState,    '*',  multipleLineCommState},
            {multipleLineCommState, '*',  multipleStarCommState},
            {multipleStarCommState, '*',  multipleStarCommState},
            {multipleStarCommState, '/',  commState},
            {multipleStarCommState, '\n', multipleLineCommState},
            {multipleLineCommState, '\n', multipleLineCommState},

            // Strings and characters
            {initState,             '\'', chrStartState},
            {chrEndState,           '\'', literalChrState},
            {chrStartState,         '\\', chrBackState},
            {initState,             '"',  strLoopState},
            {strLoopState,          '\\', strSlashState},
            {strLoopState,          '"',  literalStringState},
            {strSlashState,         '"',  strLoopState},
            {strLoopState,          '\n', strLoopState},

            // Intermediary operators
            {initState,             '-',  opMinusIntermState},
            {initState,             '+',  opPlusIntermState},
            {initState,             '/',  opSlashIntermState},
            {initState,             '&',  opAndIntermState},
            {initState,             '|',  opOrIntermState},

            // Composed operators
            {opAndIntermState,      '&',  operatorState},
            {opAndIntermState,      '=',  operatorState},
            {opOrIntermState,       '|',  operatorState},
            {opOrIntermState,       '=',  operatorState},
            {opMinusIntermState,    '-',  operatorState},
            {opMinusIntermState,    '=',  operatorState},
            {opPlusIntermState,     '+',  operatorState},
            {opPlusIntermState,     '=',  operatorState},
            {opSlashIntermState,    '=',  operatorState},
            {opEqualIntermState,    '=',  operatorState}
    };

    transitions multipleTrans[37] = {
            // Identfiers
            {initState,             ALPHABET,  identifierState},
            {identifierState,       ALPHABET,  identifierState},
            {identifierState,       NUMBERS,   identifierState},

            // Literals
            {initState,             NUMBERS,   literalIntState},
            {literalIntState,       NUMBERS,   literalIntState},
            {literalFloatState,     NUMBERS,   literalFloatState},
            {literalIntState,       ALPHABET,  forbiddenState},
            {literalFloatState,     ALPHABET,  forbiddenState},

            // Weird literals
            {floatPointState,       NUMBERS,   floatLoopState},

            // Hex and Bin
            {binIdState,            BINCHR,    binLoopState},
            {binLoopState,          BINCHR,    binLoopState},
            {binLoopState,          NUMBERS,   forbiddenState},
            {binLoopState,          ALPHABET,  forbiddenState},
            {hexIdState,            NUMBERS,   hexLoopState},
            {hexIdState,            HEXCHRLWR, hexLoopState},
            {hexIdState,            HEXCHRUPP, hexLoopState},
            {hexLoopState,          NUMBERS,   hexLoopState},
            {hexLoopState,          HEXCHRLWR, hexLoopState},
            {hexLoopState,          HEXCHRUPP, hexLoopState},
            {hexLoopState,          ALPHABET,  forbiddenState},

            // Exponential literals
            {powIntState,           NUMBERS,   intExponentialState},
            {intExponentialState,   NUMBERS,   intExponentialState},

            {powFloatState,         PLUSMINUS, floatExponentialState},
            {powFloatState,         NUMBERS,   floatExponentialState},
            {floatExponentialState, NUMBERS,   floatExponentialState},
            {floatExponentialState, ALPHABET,  forbiddenState},

            // Comments
            {lineCommLoopState,     ANYTHING,  lineCommLoopState},
            {multipleLineCommState, ANYTHING,  multipleLineCommState},
            {multipleStarCommState, ANYTHING,  multipleLineCommState},

            // Strings and charactes
            {chrStartState,         ANYTHING,  chrEndState},
            {chrBackState, "'nt0",             chrEndState},
            {strLoopState,          ANYTHING,  strLoopState},
            {strSlashState,         ANYTHING,  strLoopState},

            // Operators and separators
            {initState,    "-*=%^><!",         opEqualIntermState},
            {initState,    "#,.:?",            operatorState},

            {initState,    "[]{}()",           separatorState}
    };
public:
    class Token {
        int type;
        std::string content;

    public:
        Token();

        Token(int type, std::string content);

        int getType() const;

        void setType(int type);

        const std::string &getContent() const;

        void setContent(const std::string &content);

        bool operator==(const Token &rhs) const;

        bool operator!=(const Token &rhs) const;

        friend std::ostream &operator<<(std::ostream &os, const Token &token);
    };

    Scanner(char *filename);

    bool isKeyword(string str);

    int lambdaTranz(int state);

    bool canDoSimplTrans(int index, int state, char symbol);

    bool canDoMultTrans(int index, int state, char symbol);

    int searchState(int state, char symbol);

    bool isFinal(int state);

    Token getToken();

    virtual ~Scanner();
};


#endif //LEXICAL_ANALYZER_SCANNER_H
