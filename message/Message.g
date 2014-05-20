grammar Message;

options
{
    output = AST;
    language = C;
    ASTLabelType=pANTLR3_BASE_TREE;
}


prog	:
	(forward_decl | typedef_decl | packages | include | enum_decl | struct_decl | message_decl)+
	;

forward_decl 
	: FORWARD ENUM ID INT? -> ^(FORWARD ENUM ID INT?)
	| FORWARD STRUCT ID -> ^(FORWARD STRUCT ID)
	;

typedef_decl
	: COMMENT? TYPEDEF ID ID -> ^(TYPEDEF ID ID COMMENT?)
	;

packages 
	:PACKAGES PATH LANG? -> ^(PACKAGES PATH LANG?)
	;

include :
	INCLUDE '"' PATH '"' -> ^(LQUO PATH)
	|INCLUDE '<' PATH '>' -> ^(LBRACKET PATH)
	;

enum_decl :
	COMMENT? ENUM ID? '{' enum_field_decl+ '}' -> ^(ENUM ID? COMMENT? enum_field_decl+)
	;
	
struct_decl 	:
	COMMENT? STRUCT ID '{' field_decl* '}' -> ^(STRUCT ID COMMENT? field_decl*)
	;
	
message_decl :
	COMMENT? MESSAGE ID '{' message_field '}' -> ^(MESSAGE ID COMMENT? message_field )
	;

message_field
	:	(field_decl | enable_if)+
	;

enable_if 
	: ENABLEIF ID OP ID'{' field_decl+ '}' -> ^(ENABLEIF ID OP ID field_decl+)
	;
	
field_decl:
	(default_decl |array_decl | default_expr_decl)
	;

enum_field_decl	:
	ID '='  INT  ',' COMMENT? -> ^(FIELD_ENUM ID INT COMMENT?)
	;
	
default_decl
	:	ID ID ',' COMMENT? -> ^(FIELD ID ID COMMENT?)
	;

array_decl
	:
	ID ID '[' INT ']' ',' COMMENT?  -> ^(ARRAY ID ID INT COMMENT?)
	| ID ID '[' ID ']' ',' COMMENT? -> ^(ARRAY ID ID ID COMMENT?)
	;

default_expr_decl:
	ID ID '=' ID ',' COMMENT? -> ^(FIELD_DEFAULT ID ID ID COMMENT?)
	;

ENUM	:
	'enum'	
	;
FIELD	:
	'FIELD'
	;
FIELD_ENUM
	:
	'ENUM'
	;
FIELD_DEFAULT	:
	'FIELD_DEFAULT'
	;
ARRAY	:
	'ARRAY'
	;
STRUCT	:
	'struct'	
	;
MESSAGE	:
	'message'	
	;
INCLUDE :
	'include'
	;
PACKAGES
	: 'package'
	;
LANG	: ('cpp' | 'as')
	;
LQUO	: 'lquo'
	;
LBRACKET: 'lbracket'
	;

ENABLEIF: 'enable_if'
	;

FORWARD	: 'forward'
	;

TYPEDEF	: 'typedef'
	;

OP	: ('==' | '>' | '<' | '!=' | '>=' | '<=')
	;

ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

PATH	:
	ID (('/' | '.') ID)*
	;
    
INT :	'0'..'9'+
    ;

COMMENT
    :   '//' ~('\n'|'\r')*
    ;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;