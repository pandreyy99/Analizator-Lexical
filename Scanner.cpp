//
// Created by Andrei on 20-Apr-20.
//

#include "Scanner.h"

char nameOfStates[][27] = {
        "Initial State",

        "PlusInterm",
        "MinusInterm",
        "SlashInterm",
        "EqualInterm",
        "OrInterm",
        "AndInterm",

        "PossibleExponentialInteger",
        "PossibleExponentialFloat",
        "ExponentialInteger",
        "ExponentialFloat",
        "FloatNumberLoop",
        "FloatPoin",

        "ZeroNumber",
        "BinaryIdentificator",
        "BinaryLoop",
        "HexIdentificator",
        "HexLoop",

        "CommentLineLoop",
        "CommentStarLoop",
        "CommentLoop",

        "CharStart",
        "CharEnd",
        "CharBack",

        "StringEnd",
        "StringLoop",
        "StringSlash",

        "ForbiddenState",
        "ErrorState",

        "WhereFinalStatesBegin",

        "Identificator",
        "LiteralInt",
        "LiteralFloat",
        "LiteralChar",
        "LiteralString",

        "Operator",
        "Separator",
        "Comment",

        "Delimiter",
        "Space",
        "TabSpace",
        "NewLine",

        "WhereStatesEnd",
        "Keyword"

};

Scanner::Token::Token() {}

Scanner::Token::Token(int type, std::string content) : type(type), content(content) {}

int Scanner::Token::getType() const {
    return type;
}

void Scanner::Token::setType(int type) {
    Token::type = type;
}

bool Scanner::Token::operator==(const Token &rhs) const {
    return type == rhs.type &&
           content == rhs.content;
}

bool Scanner::Token::operator!=(const Token &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const Scanner::Token &token) {
    if (token.type != -1) {
        os << "<type: " << nameOfStates[token.type] << ", content: " << token.content << ">\n";
    } else {
        os << "<type: No type yet, content: " << token.content << ">\n";
    }

    return os;
}

const std::string &Scanner::Token::getContent() const {
    return content;
}

void Scanner::Token::setContent(const std::string &content) {
    Token::content = content;
}

Scanner::Scanner(char *filename) : filename(filename) {
    in = fopen(filename, "r");

    previousState = currentState = initState;
    lposs = rposs = 0;
    currentContent = "";

    if (in == NULL)
        //perror ("Error opening file");
        std::cout << "Nu exista!";
}

bool Scanner::isKeyword(string str) {
    if (!str.compare("if") || !str.compare("else") ||
        !str.compare("while") || !str.compare("do") ||
        !str.compare("break") ||
        !str.compare("continue") || !str.compare("int")
        || !str.compare("double") || !str.compare("float")
        || !str.compare("return") || !str.compare("char")
        || !str.compare("case") || !str.compare("char")
        || !str.compare("sizeof") || !str.compare("long")
        || !str.compare("short") || !str.compare("typedef")
        || !str.compare("switch") || !str.compare("unsigned")
        || !str.compare("void") || !str.compare("static")
        || !str.compare("struct") || !str.compare("goto"))
        return (true);
    return (false);
}

bool Scanner::canDoSimplTrans(int index, int currState, char symbol) {
    return simpleTrans[index].start_state == currState &&
           simpleTrans[index].symbol == symbol;
}

bool Scanner::canDoMultTrans(int index, int currState, char symbol) {
    bool can = true;
    string symbols = multipleTrans[index].symbols;

    if (currState != multipleTrans[index].start_state)
        return !can;

    for (int i = 0; i < symbols.length(); i++) {
        if (symbol == symbols[i]) {
            return can;
        }
    }

    return !can;
}

int Scanner::searchState(int currState, char symbol) {
    if (symbol == '\0') return currState;

    for (int i = 0; i < ARRAY_SIZE(simpleTrans); i++) {
        if (canDoSimplTrans(i, currState, symbol)) return simpleTrans[i].end_state;
    }
    for (int i = 0; i < ARRAY_SIZE(multipleTrans); i++) {
        if (canDoMultTrans(i, currState, symbol)) return multipleTrans[i].end_state;
    }

    return errorState;
}

bool Scanner::isFinal(int state) {
    return (state > whereFinalStatesBegin
            && state < whereStatesEnd);
}

int Scanner::lambdaTranz(int currentState) {
    for (int index = 0; index < ARRAY_SIZE(lambdaTrans); index++) {
        if (currentState == lambdaTrans[index].start_state)
            return lambdaTrans[index].end_state;
    }

    return currentState;
}

Scanner::Token Scanner::getToken() {
    Token token = Token();
    char currentSymbol;

    while (currentSymbol = fgetc(this->in)) {
        currentState = searchState(previousState, currentSymbol);
        currentContent = currentContent + currentSymbol;

        /**printf("%s - %s -| %s ^ %c\n",
               nameOfStates[previousState], nameOfStates[currentState],
               currentContent.c_str(), currentSymbol );*/
        token.setContent(previousContent);

        switch (currentState) {
            case errorState: {


                if (isFinal(previousState)) {
                    // ltokAdd(&lt, prevState, word);

                    if (!isKeyword(previousContent)) {
                        token.setType(previousState);
                    } else {
                        token.setType(ARRAY_SIZE(nameOfStates) - 1); // index for keywords
                    }

                    currentContent = currentSymbol;
                    previousContent = currentSymbol;
                    previousState = initState;

                    return token;
                }

                int lambdaState = lambdaTranz(previousState);

                if (isFinal(lambdaState)) {
                    // ltokAdd(&lt, lmbState, word);
                    if (!isKeyword(previousContent)) {
                        token.setType(lambdaState);
                    } else {
                        token.setType(ARRAY_SIZE(nameOfStates) - 1);
                    }
                    currentContent = currentSymbol;
                    previousContent = currentSymbol;
                    previousState = initState;

                    return token;
                    // goto restart; -> // return something to know you can still search
                }

                string errorMsg = "\nErr : <";
                errorMsg.append(nameOfStates[currentState]);
                errorMsg.append(" : ");
                errorMsg.append(currentContent);

                token.setContent(errorMsg);
                token.setType(errorState);

                return token;
                // ltokAdd(&lt, errorState, errMsg);
                // goto end;
            }

            case forbiddenState: {
                string errorMsg = "\nErr : <";

                errorMsg.append(nameOfStates[currentState]);
                errorMsg.append(" : ");
                errorMsg.append(currentContent);

                token.setType(errorState);
                token.setContent(errorMsg);
                return token;

                //ltokAdd(&lt, errorState, errMsg);
                //free(errMsg);
                // goto end;
            }

            default: {
                previousState = currentState;
                previousContent = currentContent;

                token.setType(-1);
                token.setContent(currentContent);
                return token;
            }

        }
    }
}

Scanner::~Scanner() {
    fclose(this->in);
}




