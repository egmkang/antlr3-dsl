#include <assert.h>
#include <iostream>
#include <vector>
#include "ParseXmlLexer.h"
#include "ParseXmlParser.h"
#include "XmlDocInfo.h"

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i)
{
  assert(i < tree->getChildCount(tree));
  return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree)
{
  return (const char*) tree->getText(tree)->chars;
}

void print_ast(pANTLR3_BASE_TREE tree, int depth)
{
  if (tree == NULL)
  {
    return;
  }
  for(int i = 0; i < depth; ++i)
    std::cout << "  ";
  pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
  std::cout << getText(tree);
  if(tok)
    std::cout << "  =>  " << tok->type;
  std::cout << std::endl;

  unsigned count = tree->getChildCount(tree);
  for(unsigned i = 0; i < count; ++i)
  {
    print_ast(getChild(tree, i), depth + 1);
  }
}

void Analysis(pANTLR3_BASE_TREE tree);

int main(int argc, const char* argv[])
{
  pANTLR3_INPUT_STREAM stream = antlr3FileStreamNew((pANTLR3_UINT8)"./xml.txt", ANTLR3_ENC_8BIT);
  if(!stream)
  {
    std::cerr << "need one file" << std::endl;
    return 0;
  }
  assert(stream);

  pParseXmlLexer lexer = ParseXmlLexerNew(stream);
  assert(lexer);

  pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
  assert(tstream);

  pParseXmlParser parser = ParseXmlParserNew(tstream);
  assert(parser);

  ParseXmlParser_prog_return r = parser->prog(parser);

  pANTLR3_BASE_TREE tree = r.tree;

  print_ast(tree, 0);

  Analysis(tree);

  return 0;
}

void Analysis(pANTLR3_BASE_TREE tree)
{
  pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
  if(tok)
  {

  }
  else
  {
    unsigned count = tree->getChildCount(tree);
    for(unsigned i = 0; i < count; ++i)
    {
      Analysis(getChild(tree, i));
    }
  }
}
