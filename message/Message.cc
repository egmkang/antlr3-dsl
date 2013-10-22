#include <assert.h>
#include <iostream>
#include <generate/generator.h>
#include <generate/cpp_generator.h>
#include "MessageLexer.h"
#include "MessageParser.h"

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
    if(tok && tok->type == COMMENT)
      return getText((pANTLR3_BASE_TREE)tree->getChild(tree, i));
  }
  return "";
}

void Analysis(pANTLR3_BASE_TREE tree, Generator *generator);

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

void help()
{
  std::cerr << "message [option]" << std::endl
            << "Options:" << std::endl
            << "  -cpp_out=dir    set C++ output dicectory" << std::endl
            << "  -Idir           set input directory" << std::endl
            << "  --help          show me all options" << std::endl;
}

int main(int argc, const char* argv[])
{
  if(argc < 2)
  {
    help();
    return 0;
  }

  std::string input_dir = "./";
  std::string cpp_out = "./cpp/";
  std::string file_name = "";

  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if(arg.find("--help") != std::string::npos)
    {
      help();
      exit(0);
    }
    else if(arg.find("-cpp_out") != std::string::npos)
    {
      cpp_out = argv[i];
      cpp_out = cpp_out.substr(cpp_out.find("-cpp_out")+8+1);
    }
    else if(arg.find("-I") != std::string::npos)
    {
      input_dir = argv[i];
      input_dir = input_dir.substr(input_dir.find("-I")+2);
    }
    else
    {
      file_name = argv[i];
    }
  }

  std::string input_file_name = input_dir + "/" + file_name;

  pANTLR3_INPUT_STREAM stream = antlr3FileStreamNew((pANTLR3_UINT8)input_file_name.c_str(), ANTLR3_ENC_8BIT);
  if(!stream || file_name.empty())
  {
    std::cerr << "need one file" << std::endl;
    std::cerr << "use --help to see all options" << std::endl;
    return 0;
  }
  assert(stream);

  pMessageLexer lexer = MessageLexerNew(stream);
  assert(lexer);

  pANTLR3_COMMON_TOKEN_STREAM tstream = antlr3CommonTokenStreamSourceNew(
      ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
  assert(tstream);

  pMessageParser parser = MessageParserNew(tstream);
  assert(parser);

  MessageParser_prog_return r = parser->prog(parser);

  pANTLR3_BASE_TREE tree = r.tree;

#ifdef _DEBUG_
  print_ast(tree, 0);
#endif

  Generator *generator = new CppGenerator(input_dir,
      file_name.find(".") != std::string::npos ? file_name.substr(0, file_name.find('.')) : file_name,
      cpp_out);
  Analysis(tree, generator);

#ifdef _DEBUG_
  const Generator::TypeVector& enums = generator->enums();
  for(Generator::TypeVector::const_iterator iter = enums.begin();
      iter != enums.end();
      ++iter)
  {
    std::cout << (*iter)->debug();
  }

  const Generator::TypeVector& structs = generator->structs();
  for(Generator::TypeVector::const_iterator iter = structs.begin();
      iter != structs.end();
      ++iter)
  {
    std::cout << (*iter)->debug();
  }

  const Generator::TypeVector& messages = generator->messages();
  for(Generator::TypeVector::const_iterator iter = messages.begin();
      iter != messages.end();
      ++iter)
  {
    std::cout << (*iter)->debug();
  }
#endif

  generator->Generate();

  return 0;
}

TypeClass* ParseEnum(pANTLR3_BASE_TREE tree, Generator *generator);
TypeClass* ParseStruct(pANTLR3_BASE_TREE tree, Generator *generator);
TypeClass* ParseMessage(pANTLR3_BASE_TREE tree, Generator *generator);
Node* ParseField(pANTLR3_BASE_TREE tree, Generator *generator);

void Analysis(pANTLR3_BASE_TREE tree, Generator *generator)
{
  pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
  int line = tok ? tok->line : -1;
  if(tok)
  {
    switch(tok->type)
    {
      case ENUM:
        {
          TypeClass *type_enum = ParseEnum(tree, generator);
          type_enum->line_number(line);
          generator->AddEnum(type_enum);
        }
        break;
      case STRUCT:
        {
          TypeClass *type_struct = ParseStruct(tree, generator);
          type_struct->line_number(line);
          generator->AddStruct(type_struct);
        }
        break;
      case MESSAGE:
        {
          TypeClass *type_message = ParseMessage(tree, generator);
          type_message->line_number(line);
          generator->AddMessage(type_message);
        }
        break;
      case PACKAGES:
        {
          const std::string& package_name = getText(getChild(tree, 0));
          generator->AddNamespace("cpp", package_name);
        }
        break;
      case LBRACKET:
        {
          std::string include_file = getText(getChild(tree, 0));
          include_file = "<" + include_file + ">";
          generator->AddInclude(include_file);
        }
        break;
      case LQUO:
        {
          std::string include_file = getText(getChild(tree, 0));
          include_file = "\"" + include_file + "\"";
          generator->AddInclude(include_file);
        }
        break;
      case FORWARD:
        {
          pANTLR3_BASE_TREE child_type = getChild(tree, 0);
          pANTLR3_BASE_TREE child_name = getChild(tree, 1);
          pANTLR3_COMMON_TOKEN tok = tree->getToken(child_type);
          Node *forward = NULL;
          if(tok->type == ENUM)
          {
            forward = new FieldEnum(getText(child_name), "");
          }
          else
          {
            forward = new TypeClass(getText(child_type), getText(child_name));
          }
          forward->set_skip();
          forward->line_number(line);
          generator->AddForward(forward);
        }
        break;
      case TYPEDEF:
        {
          pANTLR3_BASE_TREE type1 = getChild(tree, 0);
          pANTLR3_BASE_TREE type2 = getChild(tree, 1);
          const char *comment = getComment(tree, 2);
          NodeTypeDef *node = new NodeTypeDef(getText(type1), getText(type2));
          node->line_number(line)->comment(comment);
          generator->AddTypedef(node);
        }
        break;
      default:
        std::cout << "Unknown token type:" << tok->type;
        if(getText(tree))
          std::cout << " name:" << getText(tree);
        std::cout << std::endl;
    }
  }
  else
  {
    unsigned count = tree->getChildCount(tree);
    for(unsigned i = 0; i < count; ++i)
    {
      Analysis(getChild(tree, i), generator);
    }
  }
}

TypeClass* ParseEnum(pANTLR3_BASE_TREE tree, Generator *generator)
{
  int child_count = tree->getChildCount(tree);
  assert(tree && child_count >= 1);
  (void)child_count;
  TypeClass *type_enum = NULL;
  for(unsigned index = 0;
      index < tree->getChildCount(tree);
      ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);
    pANTLR3_COMMON_TOKEN tok = child->getToken(child);
    unsigned tok_type = tok->type;
    const std::string& tok_text = getText(child);

    switch(tok_type)
    {
      case ID:
          type_enum = new TypeClass(kKeyWordEnum, tok_text);
        break;
      case FIELD_ENUM:
        {
          if(!type_enum)
            type_enum = new TypeClass(kKeyWordEnum, "");
          Node* node = ParseField(child, generator);
          type_enum->AddNode(node);
        }
        break;
    }
  }
  return type_enum;
}

TypeClass* ParseStruct(pANTLR3_BASE_TREE tree, Generator *generator)
{
  assert(tree);
  const std::string& struct_name = getText(getChild(tree, 0));
  const std::string& comment = getComment(tree, 1);
  TypeClass *type_struct = new TypeClass(kKeyWordStruct, struct_name);
  type_struct->comment(comment);
  for(unsigned index = comment.size() ? 2 : 1;
      index < tree->getChildCount(tree);
      ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);
    Node *node = ParseField(child, generator);
    type_struct->AddNode(node);
  }
  return type_struct;
}

TypeClass* ParseMessage(pANTLR3_BASE_TREE tree, Generator *generator)
{
  assert(tree);
  const std::string& message_name = getText(getChild(tree, 0));
  const std::string& comment = getComment(tree, 1);
  TypeClass *type_message = new TypeClass(kKeyWordMessage, message_name);
  type_message->comment(comment);
  for(unsigned index = comment.size() ? 2 : 1;
      index < tree->getChildCount(tree);
      ++index)
  {
    pANTLR3_BASE_TREE child = getChild(tree, index);
    Node *node = ParseField(child, generator);
    if(node)
      type_message->AddNode(node);
  }
  return type_message;
}

Node* ParseField(pANTLR3_BASE_TREE tree, Generator *generator)
{
  pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
  int line = tok->line;
  switch(tok->type)
  {
    case FIELD_ENUM:
      {
        pANTLR3_BASE_TREE child_name = getChild(tree, 0);
        pANTLR3_BASE_TREE child_value = getChild(tree, 1);
        const char* comment = getComment(tree, 2);
        return (new FieldEnum(getText(child_name), getText(child_value)))
                ->line_number(line)
                ->comment(comment);
      }
      break;
    case ARRAY:
      {
        pANTLR3_BASE_TREE child_type = getChild(tree, 0);
        pANTLR3_BASE_TREE child_name = getChild(tree, 1);
        pANTLR3_BASE_TREE child_length = getChild(tree, 2);
        const char* comment = getComment(tree, 3);
        if(getText(child_type) == kKeyWordString)
        {
          return (new FieldString(getText(child_name), getText(child_length)))
                  ->line_number(line)
                  ->comment(comment);
        }
        return (new FieldArray(getText(child_type), getText(child_name), getText(child_length)))
                ->line_number(line)
                ->comment(comment);
      }
      break;
    case FIELD:
      {
        pANTLR3_BASE_TREE child_type = getChild(tree, 0);
        pANTLR3_BASE_TREE child_name = getChild(tree, 1);
        const char* comment = getComment(tree, 2);
        return (new Field(getText(child_type),
            getText(child_name),
            ""))->line_number(line)
                ->comment(comment);
      }
      break;
    case FIELD_DEFAULT:
      {
        pANTLR3_BASE_TREE child_type = getChild(tree, 0);
        pANTLR3_BASE_TREE child_name = getChild(tree, 1);
        pANTLR3_BASE_TREE child_default_value = getChild(tree, 2);
        const char* comment = getComment(tree, 3);
        return (new Field(getText(child_type),
            getText(child_name),
            getText(child_default_value)))->line_number(line)
                ->comment(comment);
      }
    case ENABLEIF:
      {
        pANTLR3_BASE_TREE param1 = getChild(tree, 0);
        pANTLR3_BASE_TREE op     = getChild(tree, 1);
        pANTLR3_BASE_TREE param2 = getChild(tree, 2);
        FiledsEnableIf *enable_if = new FiledsEnableIf(getText(param1), getText(op), getText(param2));
        for(unsigned i = 3; i < tree->getChildCount(tree); ++i)
        {
          Node *node = ParseField(getChild(tree, i), generator);
          enable_if->AddField(node);
        }
        return enable_if->line_number(line);
      }
      break;
    default:
        std::cout << "Unknown token type:" << tok->type;
        if(getText(tree))
          std::cout << " name:" << getText(tree);
        std::cout << std::endl;
      break;
  }
  return NULL;
}

