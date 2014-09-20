// Angie Graci
// CSC 444
// Dr. Lea
// Fall 2014

// Exercise 1

/*------------------------------------------------------------------------------

PART 1: Transform grammar

  PART 1.1: Original Grammar

    S -> A EOL | EOL
    A -> ID EQ E
    Q -> ID QMARK
    E -> E AND E | E OR E | E XOR E | NOT E | LP E RP | ID | TRUE | FALSE 

    TERMINALS
      a-z   ID
      &     AND
      |     OR
      ^     XOR
      ~     NOT
      =     EQ
      ?     QMARK 
      0     FALSE 
      1     TRUE 
      (     LP 
      )     RP 
      \n    EOL

  PART 1.2: Add Order of Operations

    S -> L | EOL
    L -> A | Q
    A -> ID EQ E
    Q -> ID QMARK
    E -> E OR W | W
    W -> W XOR X | X
    X -> X AND Y | Y
    Y -> NOT Y | Z
    Z -> LP E RP | ID | TRUE | FALSE

  PART 1.3: Elimination of Left Recursion

    S  -> ID S' EOL
    S' -> A | Q
    A  -> EQ E
    Q  -> QMARK
    E  -> W E'
    E' -> OR W E' | ϵ
    W  -> X W'
    W' -> XOR X W' | ϵ
    X  -> Y X'
    X' -> AND Y X' | ϵ
    Y  -> Y' Z
    Y' -> NOT Y' | ϵ
    Z  -> LP E RP | ID | TRUE | FALSE

  PART 1.4: First Definitions

    f(S)  = { ID }
    f(S') = { QMARK, EQ }
    f(A)  = { EQ }
    f(Q)  = { QMARK }
    f(E)  = { NOT, LP, ID, TRUE, FALSE }
    f(E') = { OR, ϵ}
    f(W)  = { NOT, LP, ID, TRUE, FALSE }
    f(W') = { XOR, ϵ}
    f(X)  = { NOT, LP, ID, TRUE, FALSE }
    f(X') = { AND, ϵ }
    f(Y)  = { NOT, LP, ID, TRUE, FALSE }
    f(Y') = { NOT, ϵ }
    f(Z)  = { LP , ID, TRUE, FALSE }

  PART 1.5: Follows Definitions

    F(S)  = { }
    F(S') = { EOL }                   aka f(EOL)
    F(A)  = { EOL }                   aka F(S')
    F(Q)  = { EOL }                   aka F(S')
    F(E)  = { EOL }                   aka F(A)
    F(E') = { EOL }                   aka F(E)
    F(W)  = { OR, EOL }               aka f(E'), F(E)
    F(W') = { OR, EOL }               aka F(W)
    F(X)  = { XOR, OR, EOL }          aka f(W'), F(W)
    F(X') = { XOR, OR, EOL }          aka F(X)
    F(Y)  = { AND, XOR, OR, EOL }     aka f(X'), F(X)
    F(Y') = { LP, ID, TRUE, FALSE }   aka f(Z)
    F(Z)  = { AND, XOR, OR, EOL }     aka F(Y)

------------------------------------------------------------------------------*/

// PART 2: Implementation

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio_ext.h>

// TERMINALS

char EOL = '\n';
char EQ = '=';
char QMARK = '?';
char AND = '&';
char OR = '|';
char XOR = '^';
char NOT = '~';
char LP = '(';
char RP = ')';
char LIT = 'L';
char T = '1';
char F = '0';

char ID_START = 'a';
char ID_END = 'z';

// STATE

char IDS[26] = "00000000000000000000000000";
char yytext;
char token;
char ID;

// HELPER FUNCTIONS

// check if a token is a legal id
int is_lit(char x) {
  return ( (x >= ID_START) && (x <= ID_END) );
}

// get the value associated with an id
int get_id(char id) {
  return IDS[id - ID_START];
}

// set the value of an id
int set_id(char val) {
  IDS[ID - ID_START] = val;
  return 0;
}

// ERRORS

// error functions
int parse_error(char *expected) {
  // print error message
  printf("Parse Error. Found: %c. Expected: %s\n", yytext, expected);
  while (getchar() != EOL); // empty line
  return 0; // give empty response
}

// YYLEX

void next() {
  if ( token != EOL ) {
    yytext = getchar(); // next char from stdin
    if ((yytext == EOL) 
	|| (yytext == EQ)
	|| (yytext == QMARK)
	|| (yytext == AND)
	|| (yytext == OR)
	|| (yytext == XOR)
	|| (yytext == NOT)
	|| (yytext == LP)
	|| (yytext == RP)
	|| (yytext == T)
	|| (yytext == F)) {
      token = yytext;
    } else if (is_lit(yytext)) {
      token = LIT;
    } else if (yytext == ' ') {
      next();
    } else {
      printf("Unexpected character: %c\n", yytext);
      next();
    }
  }
}

// return lex val if matched else null char 
char check(char tok) {
  if ( tok == token ) return yytext;
  return 0;
}

// like check (consumes the token)
char match(char tok) {
  char lexval = check(tok);
  if ( lexval ) next();
  return lexval;
}

// OPEARTORS

char not (char x) {
  int a = x - '0';
  a = !a;
  return a + '0';
}

char and (char x, char y) {
  int a = x - '0';
  int b = y - '0';
  a = a && b;
  return a + '0';
}

char xor (char x, char y) {
  int a = x - '0';
  int b = y - '0';
  a = a != b;
  return a + '0';
}

char or (char x, char y) {
  int a = x - '0';
  int b = y - '0';
  a = a || b;
  return a + '0';
}

// EVALUATION

char E(); // forward declaration

char Z () {
  char r;
  if ( match(LP) ) {
    r = E();
    if ( !r || match(RP) ) return r; 
    else return parse_error( "\")\"" );
  } else {
    if ( check(LIT) ) return get_id( match(LIT) );
    else if ( check(T) ) return match( T );
    else if ( check(F) ) return match( F );
    else return parse_error("\"(\"ID, \"0\", \"1\"");
  }
}

char Y () {
  char r;
  int toggle = 0;
  // Y'
  while (match(NOT)) {
    toggle = !toggle;
  }
  // Z
  if ( check(LIT) || check(LP) || check(T) || check(F) ) { // f(Z)
    r = Z();
    if (r && toggle) r = not(r);
    return r;
  } else {
    return parse_error("\"(\", ID, \"0\", \"1\"");
  }
}

char X () {
  char r, c;
  // Y
  if ( check(NOT) || check(LIT) || check(LP)
       || check(T) || check(F) ) { // f(Y)
    r = Y();
    if (!r) return r;
  } else {
    return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
  }
  // X'
  while ( match(AND) ) { // f(X')
      if ( check(NOT) || check(LIT) || check(LP) 
	   || check(T) || check(F) ) { // f(Y)
	c = Y();
	if (!c) return c;
      } else {
	return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
      }
      r = and(r,c);
  }
  return r;
}

char W () {
  char r, c;
  // X
  if ( check(NOT) || check(LP) || check(LIT)
       || check(T) || check(F) ) { // f(X)
    r = X();
    if (!r) return r;
  } else {
    return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
  }
  // W'
  while( match(XOR) ) {
    if ( check(NOT) || check(LP) || check(LIT) 
	 || check(T) || check(F) ) { // f(X)
      c = X();
      if (!r) return r;
    } else {
      return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
    }
    r = xor(r,c);
  } 
  return r;
}

char E () {
  char r, c;
  // W
  if ( check(NOT) || check(LP) || check(LIT)
       || check(T) || check(F) ) { // f(W)
    r = W();
    if (!r) return r;
  } else {
    return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
  }
  // E'
  while( match(OR) ) {
    if ( check(NOT) || check(LP) || check(LIT) 
	 || check(T) || check(F) ) { // f(W)
      c = W();
      if (!r) return r;
    } else {
      return parse_error("\"~\", \"(\", ID, \"0\", \"1\"");
    }
    r = or(r,c);
  } 
  return r;
}

char A () {
  char r;
  if (match(EQ)) {
    r = E();
    if (r) {
      set_id(r);
    }
    return 0;
  } else {
    return parse_error("\"=\"");
  }
}

char Q () {
  char r;
  if (match(QMARK)) {
    r = get_id(ID);
    return r;
  } else {
    return parse_error("\"?\"");
  }
}

char S () {
  ID = yytext; // first
  char r;
  if (match(LIT)) {
    if (check(QMARK)) r = Q();
    else if (check(EQ)) r = A();
    else return parse_error("\"?\", \"=\"");
    if (match(EOL)) return r; // follows
    else return parse_error("\"\\n\"");
  } else {
    return parse_error("ID");
  }
}

// REPL
int main() {
  char output;
  for (;;) { // CTRL C to exit
    printf("# ");
    next(); // read first token
    output = S(); // eval
    if (output) printf("%c\n", output); // print
    token = 0; // clear token
  }
  return 0;
}
