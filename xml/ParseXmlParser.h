/** \file
 *  This C header file was generated by $ANTLR version 3.3 Nov 30, 2010 12:45:30
 *
 *     -  From the grammar source file : D:\\message\\ParseXml.g
 *     -                            On : 2013-10-24 15:01:37
 *     -                for the parser : ParseXmlParserParser *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The parser ParseXmlParser has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef pParseXmlParser, which is returned from a call to ParseXmlParserNew().
 *
 * The methods in pParseXmlParser are  as follows:
 *
 *  - ParseXmlParser_prog_return      pParseXmlParser->prog(pParseXmlParser)
 *  - ParseXmlParser_enum_decl_return      pParseXmlParser->enum_decl(pParseXmlParser)
 *  - ParseXmlParser_enum_field_decl_return      pParseXmlParser->enum_field_decl(pParseXmlParser)
 *  - ParseXmlParser_struct_decl_return      pParseXmlParser->struct_decl(pParseXmlParser)
 *  - ParseXmlParser_class_decl_return      pParseXmlParser->class_decl(pParseXmlParser)
 *  - ParseXmlParser_struct_field_decl_return      pParseXmlParser->struct_field_decl(pParseXmlParser)
 *  - ParseXmlParser_field_decl_return      pParseXmlParser->field_decl(pParseXmlParser)
 *  - ParseXmlParser_array_field_decl_return      pParseXmlParser->array_field_decl(pParseXmlParser)
 *  - ParseXmlParser_common_field_decl_return      pParseXmlParser->common_field_decl(pParseXmlParser)
 *  - ParseXmlParser_field_property_decl_return      pParseXmlParser->field_property_decl(pParseXmlParser)
 *  - ParseXmlParser_property_item_decl_return      pParseXmlParser->property_item_decl(pParseXmlParser)
 *  - ParseXmlParser_property_item_decl1_return      pParseXmlParser->property_item_decl1(pParseXmlParser)
 *  - ParseXmlParser_prop_value_decl_return      pParseXmlParser->prop_value_decl(pParseXmlParser)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_ParseXmlParser_H
#define _ParseXmlParser_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct ParseXmlParser_Ctx_struct ParseXmlParser, * pParseXmlParser;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif
typedef struct ParseXmlParser_prog_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_prog_return;

typedef struct ParseXmlParser_enum_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_enum_decl_return;

typedef struct ParseXmlParser_enum_field_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_enum_field_decl_return;

typedef struct ParseXmlParser_struct_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_struct_decl_return;

typedef struct ParseXmlParser_class_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_class_decl_return;

typedef struct ParseXmlParser_struct_field_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_struct_field_decl_return;

typedef struct ParseXmlParser_field_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_field_decl_return;

typedef struct ParseXmlParser_array_field_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_array_field_decl_return;

typedef struct ParseXmlParser_common_field_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_common_field_decl_return;

typedef struct ParseXmlParser_field_property_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_field_property_decl_return;

typedef struct ParseXmlParser_property_item_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_property_item_decl_return;

typedef struct ParseXmlParser_property_item_decl1_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_property_item_decl1_return;

typedef struct ParseXmlParser_prop_value_decl_return_struct
{
    /** Generic return elements for ANTLR3 rules that are not in tree parsers or returning trees
     */
    pANTLR3_COMMON_TOKEN    start;
    pANTLR3_COMMON_TOKEN    stop;
    pANTLR3_BASE_TREE	tree;
   
}
    ParseXmlParser_prop_value_decl_return;



/** Context tracking structure for ParseXmlParser
 */
struct ParseXmlParser_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_PARSER   pParser;


     ParseXmlParser_prog_return (*prog)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_enum_decl_return (*enum_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_enum_field_decl_return (*enum_field_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_struct_decl_return (*struct_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_class_decl_return (*class_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_struct_field_decl_return (*struct_field_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_field_decl_return (*field_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_array_field_decl_return (*array_field_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_common_field_decl_return (*common_field_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_field_property_decl_return (*field_property_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_property_item_decl_return (*property_item_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_property_item_decl1_return (*property_item_decl1)	(struct ParseXmlParser_Ctx_struct * ctx);
     ParseXmlParser_prop_value_decl_return (*prop_value_decl)	(struct ParseXmlParser_Ctx_struct * ctx);
    // Delegated rules

    const char * (*getGrammarFileName)();
    void            (*reset)  (struct ParseXmlParser_Ctx_struct * ctx);
    void	    (*free)   (struct ParseXmlParser_Ctx_struct * ctx);
    /* @headerFile.members() */
    pANTLR3_BASE_TREE_ADAPTOR	adaptor;
    pANTLR3_VECTOR_FACTORY		vectors;
    /* End @headerFile.members() */
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API pParseXmlParser ParseXmlParserNew         (pANTLR3_COMMON_TOKEN_STREAM instream);
ANTLR3_API pParseXmlParser ParseXmlParserNewSSD      (pANTLR3_COMMON_TOKEN_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the parser will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define EOF      -1
#define T__18      18
#define T__19      19
#define T__20      20
#define T__21      21
#define T_ENUM      4
#define T_ID      5
#define T_INT      6
#define T_COMMENT      7
#define T_STRUCT      8
#define T_CLASS      9
#define T_PROPERPTY_BEGIN      10
#define T_PROPERPTY_END      11
#define T_PROP_STRING      12
#define T_ENUM_FIELD      13
#define T_ARRAY      14
#define T_FIELD      15
#define T_PROPERTY      16
#define T_WS      17
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for ParseXmlParser
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
