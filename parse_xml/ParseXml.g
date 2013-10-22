grammar ParseXml;

options
{
    output = AST;
    language = C;
    ASTLabelType=pANTLR3_BASE_TREE;
}


prog	:
	(enum_decl | struct_decl)+
	;
	
enum_decl
	:  T_ENUM '{' enum_field_decl+ '}' -> ^(T_ENUM enum_field_decl+)
	;

enum_field_decl
	:  T_ID '=' T_INT ',' T_COMMENT? -> ^(T_ENUM_FIELD T_ID T_INT T_COMMENT?)
	;

struct_decl
	:
	T_STRUCT T_ID '{' struct_field_decl+ '}'
	;

struct_field_decl
	:	
	field_property_decl? field_decl ',' -> ^(field_decl field_property_decl?)
	;

field_decl 
	:
	(common_field_decl | array_field_decl)
	;

common_field_decl
	:
	T_ID T_ID
	;

array_field_decl
	:	
	T_ARRAY T_ID ',' T_ID '>' T_ID
	;

field_property_decl
	:
	T_PROPERPTY_BEGIN  property_item_decl property_item_decl1* T_PROPERPTY_END -> ^(T_PROPERPTY_BEGIN property_item_decl property_item_decl1*)
	;

property_item_decl
	:
	T_ID '=' prop_value_decl -> ^(T_ID prop_value_decl)
	;
	
property_item_decl1
	:	
	',' T_ID '=' prop_value_decl -> ^(T_ID prop_value_decl)
	;

prop_value_decl
	:
	(T_PROP_STRING | T_INT | T_ID)
	;

T_ENUM	:
	'enum'	
	;

T_ENUM_FIELD
	:
	'enum_field'	
	;
	
T_STRUCT:
	'struct'
	;

T_CLASS	:
	'class'
	;

T_ARRAY	:
	'CArray<'
	;

T_ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

T_INT :	'0'..'9'+
    ;
    
T_PROP_STRING
	: 
	'"' ~('"')* '"'
	;
	
T_PROPERPTY_BEGIN
	:
	'['	
	;
T_PROPERPTY_END
	:
	']'	
	;

T_COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

T_WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
        ) {$channel=HIDDEN;}
    ;
