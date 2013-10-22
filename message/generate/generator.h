#ifndef __GENERATE_H__
#define __GENERATE_H__
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "type.h"

class Generator
{
  public:
    typedef std::vector<TypeClass*> TypeVector;
  public:
    Generator(const std::string& path, const std::string& file_name, const std::string& out_path);
    virtual ~Generator();

    void AddInclude(std::string include);
    void AddNamespace(std::string language, std::string space);
    void AddEnum(TypeClass *type);
    void AddStruct(TypeClass *type);
    void AddMessage(TypeClass *type);
    void AddForward(Node *type);
    void AddTypedef(Node *type);

    bool CheckError(TypeClass *type, Node *field);

    virtual void InitGenerate(){};
    virtual void CloseGenerate(){};

    void Generate();

    virtual void GenerateEnum(TypeClass *type) = 0;
    virtual void GenerateStruct(TypeClass *type) = 0;
    virtual void GenerateMessage(TypeClass *type) = 0;
    virtual void GenerateTypedef(Node *type) = 0;

    void indent_up() { indent_++;}
    void indent_down() { indent_--; }
    std::string indent() const
    {
      std::string s;
      for(int i = 0; i < indent_; ++i)
        s += "\t";
      return s;
    }

  public:
    const std::string& file_name() const { return file_name_; }
    const std::string& path() const { return path_; }
    const std::string& out_path() const { return out_path_; }

    void PrintKeyWordError(Node *field);
    void PrintTypeExistError(Node *field);
    void PrintTypeNotExistError(const std::string& type, int line);
    void PrintSymbolExistError(Node *field);
    void PrintSymbolNotExistError(const std::string& name, int line);

    bool IsKeyWord(const std::string& name);
    bool IsDefinedType(const std::string& name);
    bool IsDefinedSymbol(const std::string& name);

    const TypeVector& enums() const { return enum_; }
    const TypeVector& structs() const { return struct_; }
    const TypeVector& messages() const { return message_; }
    const std::vector<std::string>& includes() const { return cpp_includes_; }
    const std::string& package(const std::string& lang) { return namespace_[lang]; }

  private:
    std::string path_;
    std::string file_name_;
    std::string out_path_;

    std::vector<std::string> cpp_includes_;
    std::map<std::string, std::string> namespace_;

    std::set<std::string> key_words_;
    std::set<std::string> sys_types_;
    std::map<std::string, Node*> user_types_;
    std::map<std::string, Node*> symbols_;
    std::vector<Node*> message_typedef_;

    TypeVector enum_;
    TypeVector struct_;
    TypeVector message_;

    int indent_;
};

#endif
