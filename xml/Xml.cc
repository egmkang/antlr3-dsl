#include <assert.h>
#include <iostream>
#include <vector>
#include "ParseXmlLexer.h"
#include "ParseXmlParser.h"
#include "XmlDocInfo.h"

std::vector<XmlEnum*> g_enum;
std::vector<XmlClass*> g_struct;
std::vector<XmlClass*> g_class;

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i)
{
  assert(i < tree->getChildCount(tree));
  return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree)
{
  return (const char*) tree->getText(tree)->chars;
}

const char* getComment(pANTLR3_BASE_TREE tree, unsigned i)
{
  if(tree->getChildCount(tree) > i)
  {
    pANTLR3_BASE_TREE child = getChild(tree, i);
    pANTLR3_COMMON_TOKEN tok = child->getToken(child);
    if(tok && tok->type == T_COMMENT)
      return getText((pANTLR3_BASE_TREE)tree->getChild(tree, i));
  }
  return "";
}

std::string removeQuotation(const std::string& str)
{
  if((*str.begin() == '\'' && *--str.end() == '\'' ) ||
      (*str.begin() == '\"' && *--str.end() == '\"'))
  {
    return str.substr(1, str.size() - 2);
  }
  return str;
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

void ParseXmlClass(pANTLR3_BASE_TREE tree);
void ParseXmlEnum(pANTLR3_BASE_TREE tree);

void Analysis(pANTLR3_BASE_TREE tree)
{
  pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
  if(tok)
  {
    switch(tok->type)
    {
      case T_ENUM:
        {
          ParseXmlEnum(tree);
        }
        break;
      case T_STRUCT:
      case T_CLASS:
        ParseXmlClass(tree);
        break;
    }
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

void ParseXmlEnum(pANTLR3_BASE_TREE tree)
{
  int child_count = tree->getChildCount(tree);
  assert(tree && child_count >= 1);
  (void)child_count;
  XmlEnum *xml_enum = NULL;
  for(int index = 0; index < child_count; ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);
    pANTLR3_COMMON_TOKEN tok = child->getToken(child);
    unsigned tok_type = tok->type;
    const std::string& tok_text = getText(child);

    switch(tok_type)
    {
      case T_ID:
        {
          xml_enum = new XmlEnum(tok_text);
        }
        break;
      case T_ENUM_FIELD:
        {
          if(!xml_enum)
            xml_enum = new XmlEnum("");

          pANTLR3_BASE_TREE child_name = getChild(child, 0);
          pANTLR3_BASE_TREE child_value = getChild(child, 1);

          EnumField enum_field;
          enum_field.name() = getText(child_name);
          enum_field.value() = getText(child_value);
          enum_field.comment() = getComment(tree, 2);

          xml_enum->fields().push_back(enum_field);
        }
        break;
    }
  }
  g_enum.push_back(xml_enum);
}

void ParseProperty(pANTLR3_BASE_TREE tree, FieldProperty& property)
{
  int child_count = tree->getChildCount(tree);
  for(int index = 0; index < child_count; ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);

    pANTLR3_BASE_TREE property_name = getChild(child, 0);
    const std::string& property_name_str = getText(property_name);

    pANTLR3_BASE_TREE property_value = getChild(child, 1);
    const std::string& property_value_str = removeQuotation(getText(property_value));

    if(property_name_str == "attribute")
    {
      property.attr() = property_value_str;
    }
    else if(property_name_str == "default")
    {
      property.default_value() = property_value_str;
    }
    else if(property_name_str == "optional")
    {
      property.optional() = property_value_str;
    }
    else if(property_name_str == "path")
    {
      property.path() = property_value_str;
    }
    else
    {
      property[property_name_str] = property_value_str;
    }
  }
}

XmlField ParseField(pANTLR3_BASE_TREE tree)
{
  const std::string& type = getText(getChild(tree, 0));
  const std::string& name = getText(getChild(tree, 1));
  const std::string& array_length = tree->getChildCount(tree) == 3 ? getText(getChild(tree, 2)) : "";

  XmlField field(type, name);
  field.array_length() = array_length;
  return field;
}

void ParseXmlClass(pANTLR3_BASE_TREE tree)
{
  int child_count = tree->getChildCount(tree);
  assert(tree && child_count >= 1);
  (void)child_count;
  const std::string& name = getText(getChild(tree, 0));
  XmlClass *type = new XmlClass(name);

  FieldProperty property;

  for(unsigned index = 1;
      index < tree->getChildCount(tree);
      ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);
    pANTLR3_COMMON_TOKEN tok = child->getToken(child);
    unsigned tok_type = tok->type;
    const std::string& tok_text = getText(child);
    (void)tok_text;

    switch(tok_type)
    {
      case T_PROPERPTY_BEGIN:
        {
          property.clear();
          ParseProperty(child, property);
        }
        break;
      case T_FIELD:
      case T_ARRAY:
        {
          XmlField field = ParseField(child);
          field.property() = property;
          type->fields().push_back(field);
        }
        break;
    }
  }

  if(type->functions().empty())
  {
    g_struct.push_back(type);
  }
  else
  {
    g_class.push_back(type);
  }
}
