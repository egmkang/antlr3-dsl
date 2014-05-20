#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <iomanip>
#include "as_generator.h"

inline void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
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

AsGenerator::AsGenerator(const std::string& path, const std::string& file_name, const std::string& out_path):
  Generator(path, file_name, out_path)
{
}

void AsGenerator::InitGenerate()
{
  ::mkdir(this->out_path().c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
  const std::string& package = this->package("as");
  namespaces_.clear();
  SplitString(package, namespaces_, ".");
}

void AsGenerator::CloseGenerate()
{
}

void AsGenerator::GenerateEnum(TypeClass *type)
{
}

void AsGenerator::GenerateStruct(TypeClass *type)
{
}

void AsGenerator::GenerateMessage(TypeClass *type)
{
}

void AsGenerator::GenerateTypedef(Node *type)
{
}
