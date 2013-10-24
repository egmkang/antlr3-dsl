#ifndef __XML_DOC_INFO_H__
#define __XML_DOC_INFO_H__
#include <string>
#include <map>
#include <vector>

class EnumField
{
  public:
    std::string& name() { return name_; }
    std::string& value() { return value_; }
    std::string& comment() { return comment_; }
  private:
    std::string name_;
    std::string value_;
    std::string comment_;
};

class XmlEnum
{
  public:
    XmlEnum(const std::string& name) : name_(name){}
    std::string& name() { return name_; }
    std::vector<EnumField>& fields() { return fields_; }
  private:
    std::string name_;
    std::vector<EnumField> fields_;
};

class FieldProperty
{
  public:
    std::string& attr() { return attr_; }
    std::string& default_value() { return default_value_; }
    std::string& optional() { return optional_; }
    std::string& path() { return path_; }
    std::string& operator[] (std::string& property_name)
    {
      return other_property_[property_name];
    }
  private:
    std::string attr_;
    std::string default_value_;
    std::string optional_;
    std::string path_;
    std::map<std::string, std::string> other_property_;
};

class XmlField
{
  public:
    XmlField(const std::string& name, FieldProperty *property)
      : field_name_(name), property_(property)
    {
    }
    ~XmlField()
    {
      delete property_;
    }

    std::string& field_name() { return field_name_; }
    FieldProperty* property() { return property_; }
  private:
    std::string field_name_;
    FieldProperty *property_;
};

class XmlFunction
{
};

class XmlType
{
  public:
    XmlType(const std::string& name)
      : type_name_(name)
    {
    }
    ~XmlType()
    {
      this->fields_.clear();
    }

    std::vector<XmlField>& fields() { return fields_; }
    std::vector<XmlFunction>& functions() { return functions_; }
  private:
    std::string type_name_;
    std::vector<XmlField> fields_;
    std::vector<XmlFunction> functions_;
};

#endif
