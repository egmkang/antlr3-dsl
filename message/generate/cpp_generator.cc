#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <iomanip>
#include "cpp_generator.h"

std::map<std::string, std::pair<std::string, std::string> > kTypeMap;

static inline void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}

CppFieldString GetCppFieldString(Node* node)
{
  CppFieldString ret;

  if(node->type() == kKeyWordEnum ||
      node->type() == kKeyWordEnableIf)
  {
    ret.type = node->type();
    ret.name = node->name();
    return ret;
  }

  if(node->type() == kKeyWordArray)
  {
    FieldArray *array = static_cast<FieldArray*>(node);
    if(node->is_integer())
    {
      const std::pair<std::string, std::string>& type_field_string = kTypeMap[array->type() + array->field_type()];
      ret.type = type_field_string.first;
      ret.name = type_field_string.second + array->Name();
    }
    else
    {
      ret.type = "st" + array->field_type();
      ret.name = "m_ast" + array->Name();
    }

    ret.array_size = array->length();

    ret.array_size_type = kTypeMap[kKeyWordInt16].first;
    ret.array_size_name = kTypeMap[kKeyWordInt16].second + array->Name() + "Count";

    return ret;
  }

  if(node->is_integer())
  {
    Field *field = static_cast<Field*>(node);
    const std::pair<std::string, std::string>& type_field_string = kTypeMap[node->type()];
    ret.type = type_field_string.first;
    ret.name = type_field_string.second + node->Name();
    ret.default_value = field->default_value();
    if(!ret.default_value.length())
    {
      ret.default_value = "0";
    }

    return ret;
  }

  if(node->type() == kKeyWordString)
  {
    FieldString *field = static_cast<FieldString*>(node);
    const std::pair<std::string, std::string>& type_field_string = kTypeMap[kKeyWordString];
    ret.type = type_field_string.first;
    ret.name = type_field_string.second + field->Name();
    ret.array_size = field->length();

    return ret;
  }

  Field *field = static_cast<Field*>(node);
  ret.type = "st" + node->type();
  ret.name = "m_st" + node->Name();
  ret.default_value = field->default_value();

  return ret;
}

CppGenerator::CppGenerator(const std::string& path, const std::string& file_name, const std::string& out_path):
  Generator(path, file_name, out_path)
{
}

void CppGenerator::InitGenerate()
{
  kTypeMap.clear();
  kTypeMap["int8"]  = std::make_pair("int8_t", "m_by");
  kTypeMap["int16"] = std::make_pair("int16_t", "m_n");
  kTypeMap["int32"] = std::make_pair("int32_t", "m_i");
  kTypeMap["int64"] = std::make_pair("int64_t", "m_l");
  kTypeMap["string"] = std::make_pair("char", "m_sz");
  kTypeMap["arrayint8"]  = std::make_pair("int8_t", "m_aby");
  kTypeMap["arrayint16"] = std::make_pair("int16_t", "m_an");
  kTypeMap["arrayint32"] = std::make_pair("int32_t", "m_ai");
  kTypeMap["arrayint64"] = std::make_pair("int64_t", "m_al");


  ::mkdir(this->out_path().c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

  std::string out_put_file = out_path() + "/" + file_name();
  this->hh_stream_.open((out_put_file + ".h").c_str(), std::ios_base::out);
  this->cc_stream_.open((out_put_file + ".cpp").c_str(), std::ios_base::out);

  this->cc_stream_ << "#include \"public/" << file_name() << ".h\"" << std::endl;
  this->hh_stream_ << "#pragma once" << std::endl;


  const std::string& package = this->package("cpp");
  namespaces_.clear();
  SplitString(package, namespaces_, ".");

  //includes
  for(std::vector<std::string>::const_iterator iter = this->includes().begin();
      iter != this->includes().end();
      ++iter)
  {
    this->hh_stream_ << "#include " << *iter << std::endl;
  }

  this->cc_stream_ << "#include \"Common/common_codeengine.h\"" << std::endl;

  for(std::vector<std::string>::const_iterator iter = this->namespaces_.begin();
      iter != this->namespaces_.end();
      ++iter)
  {
    this->hh_stream_ << "namespace " << *iter << " {";
    this->cc_stream_ << "namespace " << *iter << " {";
  }
  this->hh_stream_ << std::endl;
  this->cc_stream_ << std::endl;
}

void CppGenerator::CloseGenerate()
{
  this->hh_stream_ << std::endl;
  this->cc_stream_ << std::endl;

  for(std::vector<std::string>::const_iterator iter = this->namespaces_.begin();
      iter != this->namespaces_.end();
      ++iter)
  {
    this->hh_stream_ << "}" << std::endl;
    this->cc_stream_ << "}" << std::endl;
  }

  this->hh_stream_.close();
  this->cc_stream_.close();
}

void CppGenerator::GenerateEnum(TypeClass *type)
{
  this->hh_stream_ << std::endl << "enum";
  if(type->name().length())
    this->hh_stream_ << " "<< type->name();
  this->hh_stream_ << std::endl << "{";
  this->indent_up();

  const std::vector<Node*>& nodes = type->fields();
  for(size_t idx = 0; idx < nodes.size(); ++idx)
  {

    FieldEnum *field = static_cast<FieldEnum*>((nodes[idx]));
    this->hh_stream_ << std::endl <<  this->indent()
      << field->name() << " = " << field->default_value() << ",";
    if(field->comment().size())
    {
      this->hh_stream_ << field->comment();
    }
  }

  this->indent_down();
  this->hh_stream_ << std::endl << "};" << std::endl;
}

void CppGenerator::GenerateTypedef(Node *type)
{
  NodeTypeDef *type_rename = static_cast<NodeTypeDef*>(type);
  if(type_rename->comment().size())
    this->hh_stream_ << std::endl << this->indent() << type_rename->comment();
  this->hh_stream_ << std::endl << this->indent() << "typedef C" << type_rename->type_exist() << " C" << type_rename->type_def() << ";";
}

void CppGenerator::GenerateStruct(TypeClass *type)
{
  std::ostringstream init_stream;
  std::ostringstream encode_stream;
  std::ostringstream decode_stream;
  (void)encode_stream;
  (void)decode_stream;

  std::string class_name = "st" + type->Name();

  //header
  if(type->comment().size())
    this->hh_stream_ << std::endl << this->indent() << type->comment();
  this->hh_stream_ << std::endl << this->indent() << "struct " << class_name;
  this->hh_stream_ << std::endl << this->indent() << "{";

  //init
  init_stream << std::endl << this->indent() << "void " << class_name << "::clear(void)";
  init_stream << std::endl << this->indent() << "{";

  //encode
  encode_stream << std::endl << this->indent() << "int32_t " << class_name << "::encode(char** pOut)";
  encode_stream << std::endl << this->indent() << "{";

  //decode
  decode_stream << std::endl << this->indent() << "int32_t " << class_name << "::decode(char** pIn)";
  decode_stream << std::endl << this->indent() << "{";

  this->indent_up();

  this->hh_stream_ << std::endl << this->indent() << "void clear(void);";
  this->hh_stream_ << std::endl << this->indent() << "int32_t encode(char** pOut);";
  this->hh_stream_ << std::endl << this->indent() << "int32_t decode(char** pIn);";
  this->hh_stream_ << std::endl;

  encode_stream << std::endl << this->indent() << "if (pOut == NULL || *pOut == NULL)";
  encode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  encode_stream << std::endl << this->indent() << "return 0;";
  this->indent_down();
  encode_stream << std::endl << this->indent() << "}" << std::endl;
  encode_stream << std::endl << this->indent() << "int16_t node_size = 0;";
  encode_stream << std::endl << this->indent() << "char* ptmp = *pOut;" << std::endl;
  encode_stream << std::endl << this->indent() << "int32_t iOutLength = 0;";
  encode_stream << std::endl << this->indent() << "int32_t coded_length = 0;" << std::endl;
  encode_stream << std::endl << this->indent() << "*pOut += sizeof(int16_t);" << std::endl;

  decode_stream << std::endl << this->indent() << "if (pIn == NULL || *pIn == NULL)";
  decode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  decode_stream << std::endl << this->indent() << "return 0;";
  this->indent_down();
  decode_stream << std::endl << this->indent() << "}";
  decode_stream << std::endl;

  decode_stream << std::endl << this->indent() << "int16_t node_size = 0;";
  decode_stream << std::endl << this->indent() << "int32_t iOutLength = 0;";
  decode_stream << std::endl << this->indent() << "int32_t decode_length = 0;";
  decode_stream << std::endl << std::endl << this->indent()
    << "decode_length = CCodeEngine::decode_int16(pIn,(uint16_t*)&node_size);";
  decode_stream << std::endl << this->indent() << "iOutLength += decode_length;";

  const std::vector<Node*>& nodes = type->fields();
  for(size_t idx = 0; idx < nodes.size(); ++idx)
  {
    //header file
    const CppFieldString& field_string = GetCppFieldString(nodes[idx]);
    if(field_string.array_size.length() && field_string.type != "char")
    {
      this->hh_stream_ << std::endl << this->indent() << field_string.array_size_type << " " << field_string.array_size_name << ";";
    }
    this->hh_stream_ << std::endl << this->indent() << field_string.type << " " << field_string.name;
    if(field_string.array_size.length())
      this->hh_stream_ << "[" << field_string.array_size << "]";
    this->hh_stream_ << ";";
    if(nodes[idx]->comment().size())
      this->hh_stream_ << nodes[idx]->comment();

    //init
    if(field_string.type != "char" && !field_string.array_size.length())
    {
      init_stream << std::endl << this->indent() << "this->" << field_string.name << " = " << field_string.default_value << ";";
    }
    else if(field_string.type == "char")
    {
      init_stream << std::endl << this->indent() << "this->" << field_string.name << "[0] = " << "'\\0';";
    }
    else if(field_string.array_size.length())
    {
      init_stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;" ;
    }

    //encode
    if(field_string.type != "char" && !field_string.array_size.length())
    {
      if(nodes[idx]->is_integer())
      {
        encode_stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_" << nodes[idx]->type() << "(pOut, " << field_string.name << ");";
      }
      else
      {
        encode_stream << std::endl << this->indent() << "coded_length = this->" << field_string.name << ".encode(pOut);";
      }
      encode_stream << std::endl << this->indent() << "node_size += coded_length;" << std::endl;
    }
    else if(field_string.type == "char")
    {
      encode_stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_string(pOut, " << field_string.name << ", " << field_string.array_size << ");";
      encode_stream << std::endl << this->indent() << "node_size += coded_length;" << std::endl;
    }
    else if(field_string.array_size.length())
    {
      encode_stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " < 0)";
      encode_stream << std::endl << this->indent() << "{";
      this->indent_up();
      encode_stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;";
      this->indent_down();
      encode_stream << std::endl << this->indent() << "}";

      encode_stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " > " << field_string.array_size << ")";
      encode_stream << std::endl << this->indent() << "{";
      this->indent_up();
      encode_stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = " << field_string.array_size << ";";
      this->indent_down();
      encode_stream << std::endl << this->indent() << "}";

      encode_stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_int16(pOut, " << field_string.array_size_name << ");";
      encode_stream << std::endl << this->indent() << "node_size += coded_length;";
      encode_stream << std::endl;

      encode_stream << std::endl << this->indent() << "for(int16_t n = 0; n < this->" << field_string.array_size_name << "; ++n)";
      encode_stream << std::endl << this->indent() << "{";
      encode_stream << std::endl;

      this->indent_up();
      if(nodes[idx]->is_integer())
      {
        encode_stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_" << static_cast<FieldArray*>(nodes[idx])->field_type() << "(pOut, this->" << field_string.name << "[n]);";
        encode_stream << std::endl << this->indent() << "node_size += coded_length;";
      }
      else
      {
        encode_stream << std::endl << this->indent() << "coded_length = this->" << field_string.name << "[n].encode(pOut);";
        encode_stream << std::endl << this->indent() << "node_size += coded_length;";
      }
      this->indent_down();
      encode_stream << std::endl << this->indent() << "}";
      encode_stream << std::endl;
    }

    //decode
    decode_stream << std::endl;
    if(field_string.type != "char" && !field_string.array_size.length())
    {
      if(nodes[idx]->is_integer())
      {
        decode_stream << std::endl << this->indent() << "decode_length = CCodeEngine::decode_" << nodes[idx]->type() << "(pIn, (u" << nodes[idx]->type() << "_t*)&" << field_string.name << ");";
      }
      else
      {
        decode_stream << std::endl << this->indent() << "decode_length = this->" << field_string.name << ".decode(pIn);";
      }
      decode_stream << std::endl << this->indent() << "iOutLength += decode_length;";
      decode_stream << std::endl << this->indent() << "node_size -= decode_length;";
    }
    else if(field_string.type == "char")
    {
      decode_stream << std::endl << this->indent() << "decode_length = CCodeEngine::decode_string(pIn, this->" << field_string.name << ", sizeof(" << field_string.name << "));";
      decode_stream << std::endl << this->indent() << "iOutLength += decode_length;";
      decode_stream << std::endl << this->indent() << "node_size -= decode_length;";
    }
    else if(field_string.array_size.length())
    {
      decode_stream << std::endl << this->indent() << "decode_length = CCodeEngine::decode_int16(pIn, (uint16_t*)&this->" << field_string.array_size_name << ");";
      decode_stream << std::endl << this->indent() << "iOutLength += decode_length;";
      decode_stream << std::endl << this->indent() << "node_size -= decode_length;";
      decode_stream << std::endl;
      decode_stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " < 0)";
      decode_stream << std::endl << this->indent() << "{";
      this->indent_up();
      decode_stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;";
      this->indent_down();
      decode_stream << std::endl << this->indent() << "}";
      decode_stream << std::endl << this->indent() << "else if (this->" << field_string.array_size_name << " > " << field_string.array_size << ")";
      decode_stream << std::endl << this->indent() << "{";
      this->indent_up();
      decode_stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = " << field_string.array_size << ";";
      this->indent_down();
      decode_stream << std::endl << this->indent() << "}";

      decode_stream << std::endl << this->indent() << "for(int16_t n = 0; n < " << field_string.array_size_name << "; ++n)";
      decode_stream << std::endl << this->indent() << "{";
      this->indent_up();
      if(nodes[idx]->is_integer())
      {
        decode_stream << std::endl << this->indent() << "decode_length = CCodeEngine::decode_" << static_cast<FieldArray*>(nodes[idx])->field_type() << "(pIn, (u" << static_cast<FieldArray*>(nodes[idx])->field_type() << "_t*)&" << field_string.name << "[n]);";
      }
      else
      {
        decode_stream << std::endl << this->indent() << "decode_length = this->" << field_string.name << "[n].decode(pIn);";
      }
      decode_stream << std::endl << this->indent() << "iOutLength += decode_length;";
      decode_stream << std::endl << this->indent() << "node_size -= decode_length;";
      this->indent_down();
      decode_stream << std::endl << this->indent() << "}";
    }
  }

  encode_stream << std::endl << this->indent() << "*pOut -= (node_size + sizeof(int16_t));";
  encode_stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_int16(pOut, node_size);" << std::endl;
  encode_stream << std::endl << this->indent() << "iOutLength = node_size + coded_length;" << std::endl;
  encode_stream << std::endl << this->indent() << "*pOut = ptmp + iOutLength;" << std::endl;
  encode_stream << std::endl << this->indent() << "return iOutLength;";

  decode_stream << std::endl;
  decode_stream << std::endl << this->indent() << "if (node_size < 0)";
  decode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  decode_stream << std::endl << this->indent() << "return 0;";
  this->indent_down();
  decode_stream << std::endl << this->indent() << "}";

  decode_stream << std::endl << this->indent() << "else";

  decode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  decode_stream << std::endl << this->indent() << "skip_byte_of(pIn, node_size);";
  decode_stream << std::endl << this->indent() << "iOutLength += node_size;";
  this->indent_down();
  decode_stream << std::endl << this->indent() << "}";
  decode_stream << std::endl;
  decode_stream << std::endl << this->indent() << "return iOutLength;";

  this->indent_down();

  this->hh_stream_ << this->indent() << std::endl << "};" << std::endl;
  init_stream << this->indent() << std::endl << "}" << std::endl;
  encode_stream << this->indent() << std::endl << "}" << std::endl;
  decode_stream << this->indent() << std::endl << "}" << std::endl;

  this->cc_stream_ << init_stream.str();
  this->cc_stream_ << encode_stream.str();
  this->cc_stream_ << decode_stream.str();
}

void CppGenerator::GenerateMessage(TypeClass *type)
{
  std::ostringstream init_stream;
  std::ostringstream encode_stream;
  std::ostringstream decode_stream;
  (void)encode_stream;
  (void)decode_stream;

  std::string class_name = "C" + type->Name();

  //header
  if(type->comment().size())
    this->hh_stream_ << std::endl << this->indent() << type->comment();
  this->hh_stream_ << std::endl << this->indent() << "class " << class_name << " : public CMessageBody";
  this->hh_stream_ << std::endl << this->indent() << "{";
  this->hh_stream_ << std::endl << this->indent() << "public:";

  //init
  init_stream << std::endl << this->indent() << class_name << "::" << class_name << "()";
  init_stream << std::endl << this->indent() << "{";

  //encode
  encode_stream << std::endl << this->indent() << "int32_t " << class_name << "::encode(char* pszOut, int32_t& iOutLength)";
  encode_stream << std::endl << this->indent() << "{";

  //decode
  decode_stream << std::endl << this->indent() << "int32_t " << class_name << "::decode(const char* pszIn, const int32_t iInLength)";
  decode_stream << std::endl << this->indent() << "{";

  this->indent_up();

  this->hh_stream_ << std::endl << this->indent() << class_name << "();";
  this->hh_stream_ << std::endl << this->indent() << "virtual int32_t encode(char* pszOut, int32_t& iOutLength);";
  this->hh_stream_ << std::endl << this->indent() << "virtual int32_t decode(const char* pszIn, const int32_t iInLength);";
  this->hh_stream_ << std::endl << this->indent() << "virtual void dump(){};";
  this->hh_stream_ << std::endl;

  encode_stream << std::endl << this->indent() << "if (NULL == pszOut)";
  encode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  encode_stream << std::endl << this->indent() << "return fail;";
  this->indent_down();
  encode_stream << std::endl << this->indent() << "}" << std::endl;
  encode_stream << std::endl << this->indent() << "char* ptmp = pszOut;";
  encode_stream << std::endl << this->indent() << "int32_t coded_length = 0;";
  encode_stream << std::endl << this->indent() << "iOutLength = 0;";


  decode_stream << std::endl << this->indent() << "if (NULL == pszIn || iInLength <= 0)";
  decode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  decode_stream << std::endl << this->indent() << "return fail;";
  this->indent_down();
  decode_stream << std::endl << this->indent() << "}";
  decode_stream << std::endl;

  decode_stream << std::endl << this->indent() << "char* ptmp = const_cast<char*>(pszIn);";
  decode_stream << std::endl << this->indent() << "int32_t remained_length = iInLength;";
  decode_stream << std::endl << this->indent() << "int32_t decoded_length = 0;";

  const std::vector<Node*>& nodes = type->fields();
  for(size_t idx = 0; idx < nodes.size(); ++idx)
  {
    const CppFieldString& field_string = GetCppFieldString(nodes[idx]);
    GenerateMessageHeaderField(this->hh_stream_, nodes[idx], field_string);
    GenerateMessageInitField(init_stream, nodes[idx], field_string);
    GenerateMessageImplFieldEncode(encode_stream, nodes[idx], field_string);
    GenerateMessageImplFieldDecode(decode_stream, nodes[idx], field_string);
  }

  encode_stream << std::endl << std::endl << this->indent() << "return success;";

  decode_stream << std::endl << this->indent() << "if (remained_length < 0)";
  decode_stream << std::endl << this->indent() << "{";
  this->indent_up();
  decode_stream << std::endl << this->indent() << "return fail;";
  this->indent_down();
  decode_stream << std::endl << this->indent() << "}";
  decode_stream << std::endl << std::endl << this->indent() << "return success;";

  this->indent_down();

  this->hh_stream_ << this->indent() << std::endl << "};" << std::endl;
  init_stream << this->indent() << std::endl << "}" << std::endl;
  encode_stream << this->indent() << std::endl << "}" << std::endl;
  decode_stream << this->indent() << std::endl << "}" << std::endl;

  this->cc_stream_ << init_stream.str();
  this->cc_stream_ << encode_stream.str();
  this->cc_stream_ << decode_stream.str();
}

void CppGenerator::GenerateMessageHeaderField(std::ostream& stream, Node* node, const CppFieldString& field_string)
{
  if(field_string.type == kKeyWordEnableIf)
  {
    FiledsEnableIf *enable_if = static_cast<FiledsEnableIf*>(node);
    std::vector<Node*>& nodes = enable_if->fields();
    for(std::vector<Node*>::iterator iter = nodes.begin();
        iter != nodes.end();
        ++iter)
    {
      const CppFieldString& child_field_string = GetCppFieldString(*iter);
      this->GenerateMessageHeaderField(stream, *iter, child_field_string);
    }
    return;
  }

  if(field_string.array_size.length() && field_string.type != "char")
  {
    stream << std::endl << this->indent() << field_string.array_size_type << " " << field_string.array_size_name << ";";
  }
  stream << std::endl << this->indent() << field_string.type << " " << field_string.name;
  if(field_string.array_size.length())
    stream << "[" << field_string.array_size << "]";
  stream << ";";
  if(node->comment().size())
    stream << node->comment();
}

void CppGenerator::GenerateMessageInitField(std::ostream& stream, Node* node, const CppFieldString& field_string)
{
  if(field_string.type == kKeyWordEnableIf)
  {
    FiledsEnableIf *enable_if = static_cast<FiledsEnableIf*>(node);
    std::vector<Node*>& nodes = enable_if->fields();
    for(std::vector<Node*>::iterator iter = nodes.begin();
        iter != nodes.end();
        ++iter)
    {
      const CppFieldString& child_field_string = GetCppFieldString(*iter);
      this->GenerateMessageInitField(stream, *iter, child_field_string);
    }
    return;
  }
  if(field_string.type == "char")
  {
    stream << std::endl << this->indent() << "this->" << field_string.name << "[0] = " << "'\\0';";
  }
  else if(field_string.array_size.length())
  {
    stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;" ;
  }
  else if(!node->is_integer())
  {
    stream << std::endl << this->indent() << "this->" << field_string.name << ".clear();";
  }
  else if(field_string.type != "char" && !field_string.array_size.length())
  {
    stream << std::endl << this->indent() << "this->" << field_string.name << " = " << field_string.default_value << ";";
  }
}

void CppGenerator::GenerateMessageImplFieldEncode(std::ostream& stream, Node* node, const CppFieldString& field_string)
{
  if(field_string.type == kKeyWordEnableIf)
  {
    FiledsEnableIf *enable_if = static_cast<FiledsEnableIf*>(node);
    const CppFieldString& param1_string = GetCppFieldString(enable_if->param1_field());
    const CppFieldString& param2_string = GetCppFieldString(enable_if->param2_field());

    stream << std::endl << this->indent() << "if (" << param1_string.name << " " << enable_if->op() << " " << param2_string.name << ")"
          << std::endl << this->indent() << "{";
    this->indent_up();

    std::vector<Node*>& nodes = enable_if->fields();
    for(std::vector<Node*>::iterator iter = nodes.begin();
        iter != nodes.end();
        ++iter)
    {
      const CppFieldString& child_field_string = GetCppFieldString(*iter);
      this->GenerateMessageImplFieldEncode(stream, *iter, child_field_string);
    }

    this->indent_down();
    stream << std::endl << this->indent() << "}";
    return;
  }
  if(field_string.type != "char" && !field_string.array_size.length())
  {
    if(node->is_integer())
    {
      stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_" << node->type() << "(&ptmp, " << field_string.name << ");";
    }
    else
    {
      stream << std::endl << this->indent() << "coded_length = this->" << field_string.name << ".encode(&ptmp);";
    }
    stream << std::endl << this->indent() << "iOutLength += coded_length;" << std::endl;
  }
  else if(field_string.type == "char")
  {
    stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_string(&ptmp, " << field_string.name << ", " << field_string.array_size << ");";
    stream << std::endl << this->indent() << "iOutLength += coded_length;" << std::endl;
  }
  else if(field_string.array_size.length())
  {
    stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " < 0)";
    stream << std::endl << this->indent() << "{";
    this->indent_up();
    stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;";
    this->indent_down();
    stream << std::endl << this->indent() << "}";

    stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " > " << field_string.array_size << ")";
    stream << std::endl << this->indent() << "{";
    this->indent_up();
    stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = " << field_string.array_size << ";";
    this->indent_down();
    stream << std::endl << this->indent() << "}";

    stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_int16(&ptmp, " << field_string.array_size_name << ");";
    stream << std::endl << this->indent() << "iOutLength += coded_length;";
    stream << std::endl;

    stream << std::endl << this->indent() << "for(int16_t n = 0; n < this->" << field_string.array_size_name << "; ++n)";
    stream << std::endl << this->indent() << "{";
    stream << std::endl;

    this->indent_up();
    if(node->is_integer())
    {
      stream << std::endl << this->indent() << "coded_length = CCodeEngine::encode_" << static_cast<FieldArray*>(node)->field_type() << "(&ptmp, this->" << field_string.name << "[n]);";
      stream << std::endl << this->indent() << "iOutLength += coded_length;";
    }
    else
    {
      stream << std::endl << this->indent() << "coded_length = this->" << field_string.name << "[n].encode(&ptmp);";
      stream << std::endl << this->indent() << "iOutLength += coded_length;";
    }
    this->indent_down();
    stream << std::endl << this->indent() << "}";
    stream << std::endl;
  }
}

void CppGenerator::GenerateMessageImplFieldDecode(std::ostream& stream, Node* node, const CppFieldString& field_string)
{
  stream << std::endl;

  if(field_string.type == kKeyWordEnableIf)
  {
    FiledsEnableIf *enable_if = static_cast<FiledsEnableIf*>(node);
    const CppFieldString& param1_string = GetCppFieldString(enable_if->param1_field());
    const CppFieldString& param2_string = GetCppFieldString(enable_if->param2_field());

    stream << std::endl << this->indent() << "if (" << param1_string.name << " " << enable_if->op() << " " << param2_string.name << ")"
          << std::endl << this->indent() << "{";
    this->indent_up();

    std::vector<Node*>& nodes = enable_if->fields();
    for(std::vector<Node*>::iterator iter = nodes.begin();
        iter != nodes.end();
        ++iter)
    {
      const CppFieldString& child_field_string = GetCppFieldString(*iter);
      this->GenerateMessageImplFieldDecode(stream, *iter, child_field_string);
    }

    this->indent_down();
    stream << std::endl << this->indent() << "}";
    return;
  }

  if(field_string.type != "char" && !field_string.array_size.length())
  {
    if(node->is_integer())
    {
      stream << std::endl << this->indent() << "decoded_length = CCodeEngine::decode_" << node->type() << "(&ptmp, (u" << node->type() << "_t*)&" << field_string.name << ");";
    }
    else
    {
      stream << std::endl << this->indent() << "decoded_length = this->" << field_string.name << ".decode(&ptmp);";
    }
    stream << std::endl << this->indent() << "remained_length -= decoded_length;";
  }
  else if(field_string.type == "char")
  {
    stream << std::endl << this->indent() << "decoded_length = CCodeEngine::decode_string(&ptmp, this->" << field_string.name << ", sizeof(" << field_string.name << "));";
    stream << std::endl << this->indent() << "remained_length -= decoded_length;";
  }
  else if(field_string.array_size.length())
  {
    stream << std::endl << this->indent() << "decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&this->" << field_string.array_size_name << ");";
    stream << std::endl << this->indent() << "remained_length -= decoded_length;";
    stream << std::endl;
    stream << std::endl << this->indent() << "if (this->" << field_string.array_size_name << " < 0)";
    stream << std::endl << this->indent() << "{";
    this->indent_up();
    stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = 0;";
    this->indent_down();
    stream << std::endl << this->indent() << "}";
    stream << std::endl << this->indent() << "else if (this->" << field_string.array_size_name << " > " << field_string.array_size << ")";
    stream << std::endl << this->indent() << "{";
    this->indent_up();
    stream << std::endl << this->indent() << "this->" << field_string.array_size_name << " = " << field_string.array_size << ";";
    this->indent_down();
    stream << std::endl << this->indent() << "}";

    stream << std::endl << this->indent() << "for(int16_t n = 0; n < " << field_string.array_size_name << "; ++n)";
    stream << std::endl << this->indent() << "{";
    this->indent_up();
    if(node->is_integer())
    {
      stream << std::endl << this->indent() << "decoded_length = CCodeEngine::decode_" << static_cast<FieldArray*>(node)->field_type()  << "(&ptmp, (u" << static_cast<FieldArray*>(node)->field_type() << "_t*)&" << field_string.name << "[n]);";
    }
    else
    {
      stream << std::endl << this->indent() << "decoded_length = this->" << field_string.name << "[n].decode(&ptmp);";
    }
    stream << std::endl << this->indent() << "remained_length -= decoded_length;";
    this->indent_down();
    stream << std::endl << this->indent() << "}";
  }
}
