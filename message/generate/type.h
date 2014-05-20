#ifndef __TYPE_H__
#define __TYPE_H__
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <sstream>

const std::string kKeyWordInt8("int8");
const std::string kKeyWordInt16("int16");
const std::string kKeyWordInt32("int32");
const std::string kKeyWordInt64("int64");
const std::string kKeyWordString("string");
const std::string kKeyWordArray("array");
const std::string kKeyWordEnum("enum");
const std::string kKeyWordStruct("struct");
const std::string kKeyWordMessage("message");
const std::string kKeyWordEnableIf("enable_if");
const std::string kKeyWordTypedef("typedef");

class Node
{
  public:
    Node(const std::string& type, const std::string& name)
      : type_(type), name_(name), line_number_(-1), integer_(false), skip_(false)
    {
      if(kKeyWordInt8  == type ||
          kKeyWordInt16 == type ||
          kKeyWordInt32 == type ||
          kKeyWordInt64 == type)
      {
        set_integer();
      }
    }
    virtual ~Node(){}
    std::string type() const { return type_; }
    std::string name() const { return name_; }
    std::string Name() const
    {
      std::string s = name_;
      s[0] = std::towupper(s[0]);
      return s;
    }
    int line_number() const { return line_number_; }
    Node* line_number(int number) { line_number_ = number;  return this; }
    virtual bool is_field() const { return true; }
    virtual std::string debug() const { return ""; }
    bool is_integer() const { return integer_; }
    void set_integer() { integer_ = true; }
    bool is_skip() const { return skip_; }
    void set_skip() { skip_ = true; }
    std::string comment() const { return comment_; }
    Node* comment(std::string comment)
    {
      comment_ = comment;
      return this;
    }
  private:
    std::string type_;
    std::string name_;
    int line_number_;
    bool integer_;
    bool skip_;
    std::string comment_;
};

class NodeTypeDef : public Node
{
  public:
    NodeTypeDef(const std::string& type1, const std::string& type2)
      :Node(kKeyWordTypedef, type2)
       ,type_exist_(type1)
       ,type_def_(type2)
    {
    }
    std::string type_exist() const { return type_exist_;}
    std::string type_def() const { return type_def_; }
  private:
    std::string type_exist_;
    std::string type_def_;
};

class Field : public Node
{
  public:
    Field(const std::string& type, const std::string& name, const std::string& default_value)
      : Node(type, name), default_value_(default_value)
    {
    }
    virtual std::string default_value() const { return default_value_; }
    std::string debug() const
    {
      std::string debug_string = type() + " " + name();
      if(default_value_.length())
      {
        debug_string += " = " + default_value_;
      }
      debug_string += ";";
      return debug_string;
    }
  private:
    std::string default_value_;
};

class FieldEnum : public Field
{
  public:
    FieldEnum(const std::string& name, const std::string& value)
      : Field(kKeyWordEnum, name, value)
    {
    }
    std::string debug() const
    {
      return name() + " = " + default_value() + ";";
    }
};

class FieldArray : public Field
{
  public:
    FieldArray(const std::string& field_type, const std::string& name, const std::string& length)
      : Field(kKeyWordArray, name, ""), field_type_(field_type), length_(length)
    {
      if(kKeyWordInt8  ==  field_type ||
          kKeyWordInt16 == field_type ||
          kKeyWordInt32 == field_type ||
          kKeyWordInt64 == field_type)
      {
        set_integer();
      }
    }
    std::string field_type() const { return field_type_; }
    virtual std::string default_value() const { return ""; }
    std::string length() const { return length_; }
    std::string debug() const
    {
      return "CArray<" + field_type() + "," + length() + "> " + name() + ";";
    }
  private:
    std::string field_type_;
    std::string length_;
};

class FieldString : public Node
{
  public:
    FieldString(const std::string& name, const std::string& length)
      :Node(kKeyWordString, name), length_(length)
    {
    }
    std::string length() const { return length_; }
    std::string debug() const
    {
      return "char " + name() + "[" + length() + "];";
    }
  private:
    std::string length_;
};

class FiledsEnableIf: public Node
{
  public:
    FiledsEnableIf(const std::string& param1, const std::string& op, const std::string& param2)
      :Node(kKeyWordEnableIf, "")
       ,param1_(param1)
       ,op_(op)
       ,param2_(param2)
       ,param1_field_(NULL)
       ,param2_field_(NULL)
    {
    }

    ~FiledsEnableIf()
    {
      for(size_t idx = 0; idx < this->fields_.size(); ++idx)
      {
        delete this->fields_[idx];
      }
    }

    void AddField(Node *field)
    {
      this->fields_.push_back(field);
    }

    std::vector<Node*>& fields() { return this->fields_; }
    const std::string& param1() const { return param1_; }
    const std::string& param2() const { return param2_; }
    const std::string& op() const { return op_; }
    Node* param1_field() const { return param1_field_; }
    Node* param2_field() const { return param2_field_; }
    void param1_field(Node* field) { param1_field_ = field; }
    void param2_field(Node* field) { param2_field_ = field; }

  private:
    std::string param1_;
    std::string op_;
    std::string param2_;
    std::vector<Node*> fields_;
    Node* param1_field_;
    Node* param2_field_;
};

class TypeClass : public Node
{
  public:
    TypeClass(const std::string& type, const std::string& name)
      : Node(type, name)
    {
    }

    ~TypeClass()
    {
      for(std::vector<Node*>::iterator iter = fields_.begin();
          iter != fields_.end();
          ++iter)
      {
        delete *iter;
      }
    }

    void AddNode(Node *node)
    {
      assert(node);
      fields_.push_back(node);
    }

    const std::vector<Node*>& fields() const { return fields_; }
    virtual bool is_field() const { return false; }

    std::string debug() const
    {
      std::ostringstream stream;
      stream << type() << " " << name() << ":" << std::endl;
      for(std::vector<Node*>::const_iterator iter = fields_.begin();
          iter != fields_.end();
          ++iter)
      {
        stream << "\t" << (*iter)->debug() << std::endl;
      }
      return stream.str();
    }
  private:
    std::vector<Node*>  fields_;
};

#endif
