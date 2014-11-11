SimpleInterpreter
=================

A simple interpreter for a toy grammar.

### Grammar

S -> A EOL | EOL

A -> ID EQ E

Q -> ID QMARK

E -> E AND E | E OR E | E XOR E | NOT E | LP E RP | ID | TRUE | FALSE

### Identifiers

a-z

Language
--------
* C

Purpose
-------
* Educational
