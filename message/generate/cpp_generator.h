#ifndef __CPP_GENERATOR_H__
#define __CPP_GENERATOR_H__
#include <generate/generator.h>
#include <iostream>
#include <fstream>

struct CppFieldString
{
  std::string type;
  std::string name;
  std::string array_size;
  std::string default_value;

  std::string array_size_type;
  std::string array_size_name;
};

class CppGenerator : public Generator
{
  public:
    CppGenerator(const std::string& path, const std::string& file_name, const std::string& out_path);

    virtual void InitGenerate();
    virtual void CloseGenerate();

    virtual void GenerateEnum(TypeClass *type);
    virtual void GenerateStruct(TypeClass *type);
    virtual void GenerateMessage(TypeClass *type);
    virtual void GenerateTypedef(Node *type);

  private:
    void GenerateMessageHeaderField(std::ostream& stream, Node* node, const CppFieldString& field_string);
    void GenerateMessageInitField(std::ostream& stream, Node* node, const CppFieldString& field_string);
    void GenerateMessageImplFieldEncode(std::ostream& stream, Node* node, const CppFieldString& field_string);
    void GenerateMessageImplFieldDecode(std::ostream& stream, Node* node, const CppFieldString& field_string);
  private:
    std::ofstream hh_stream_;
    std::ofstream cc_stream_;
    std::vector<std::string> namespaces_;
};

#endif
