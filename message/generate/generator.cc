#include <generate/generator.h>

template<class T, size_t N>
size_t ArraySize(const T (&array)[N])
{
  return N;
}

Generator::Generator(const std::string& path, const std::string& file_name, const std::string& out_path)
  :path_(path)
   , file_name_(file_name)
   , out_path_(out_path)
   , indent_(0)
{

  //TODO:egmkang,path

  std::string key_words[] =
  {
    kKeyWordInt8, kKeyWordInt16, kKeyWordInt32,
    kKeyWordInt64, kKeyWordArray, kKeyWordEnum,
    kKeyWordString, kKeyWordStruct, kKeyWordMessage,
    kKeyWordEnableIf
  };

  for(size_t index = 0; index < ArraySize(key_words); ++index)
  {
    this->key_words_.insert(key_words[index]);
  }

  std::string sys_types[] =
  {
    kKeyWordInt8, kKeyWordInt16, kKeyWordInt32, kKeyWordInt64, kKeyWordString
  };
  for(size_t index = 0; index < ArraySize(sys_types); ++index)
  {
    this->sys_types_.insert(sys_types[index]);
  }
}

Generator::~Generator()
{
}

bool Generator::IsKeyWord(const std::string& name)
{
  return this->key_words_.find(name) != this->key_words_.end();
}

bool Generator::IsDefinedType(const std::string& name)
{
  return this->sys_types_.find(name) != this->sys_types_.end() ||
    this->user_types_.find(name) != this->user_types_.end();
}

bool Generator::IsDefinedSymbol(const std::string& name)
{
  return this->symbols_.find(name) != this->symbols_.end();
}

void Generator::PrintKeyWordError(Node *field)
{
  std::cout << this->file_name_ << ":" << field->line_number() << " error: " << field->name() << " is keyword " << std::endl;
}

void Generator::PrintTypeExistError(Node *field)
{
  Node *redefined_type = this->user_types_[field->name()];
  std::cout << this->file_name_ << ":" << redefined_type->line_number() << " error: redefined " << redefined_type->name() << std::endl;
  std::cout << this->file_name_ << ":" << field->line_number() << " error: previous definition " << field->name() << std::endl;
}

void Generator::PrintSymbolNotExistError(const std::string& name, int line)
{
  std::cout << this->file_name_ << ":" << line << " error: symbol not found " << name << std::endl;
}

void Generator::PrintTypeNotExistError(const std::string& type, int line)
{
  std::cout << this->file_name_ << ":" << line << " error: " << type << " not defined" << std::endl;
}

void Generator::PrintSymbolExistError(Node *field)
{
  Node *redefined_symbol = this->symbols_[field->name()];
  std::cout << this->file_name_ << ":" << redefined_symbol->line_number() << " error: redefined " << redefined_symbol->name() << std::endl;
  std::cout << this->file_name_ << ":" << field->line_number() << " error: previous definition " << field->name() << std::endl;
}

void Generator::AddInclude(std::string include)
{
  cpp_includes_.push_back(include);
}

void Generator::AddNamespace(std::string language, std::string space)
{
  namespace_[language] = space;
}

void Generator::AddEnum(TypeClass *type)
{
  int errors = 0;
  errors += CheckError(type, type);

  const std::vector<Node*>& fields = type->fields();
  for(size_t idx = 0; idx < fields.size(); ++idx)
  {
    Node* field = fields[idx];
    errors += CheckError(type, field);
  }

  if(errors)
  {
    exit(0);
  }

  for(size_t index = 0; index < fields.size(); ++index)
  {
    this->symbols_[fields[index]->name()] = fields[index];
  }
  if(type->name().size())
  {
    this->symbols_[type->name()] = type;
  }
  this->enum_.push_back(type);

  if(type->name().size())
    this->user_types_.insert(std::make_pair(type->name(), type));
}

void Generator::AddStruct(TypeClass *type)
{
  int errors = 0;
  errors += CheckError(type, type);

  const std::vector<Node*>& fields = type->fields();
  for(size_t idx = 0; idx < fields.size(); ++idx)
  {
    Node* field = fields[idx];
    errors += CheckError(type, field);
  }

  if(errors)
  {
    exit(0);
  }

  this->struct_.push_back(type);
  this->user_types_.insert(std::make_pair(type->name(), type));
}

void Generator::AddMessage(TypeClass *type)
{
  int errors = 0;
  errors += CheckError(type, type);

  const std::vector<Node*>& fields = type->fields();
  for(size_t idx = 0; idx < fields.size(); ++idx)
  {
    Node* field = fields[idx];
    errors += CheckError(type, field);
  }

  if(errors)
  {
    exit(0);
  }

  this->message_.push_back(type);
  this->user_types_.insert(std::make_pair(type->name(), type));
}

void Generator::AddForward(Node *type)
{
  if(type->type() == kKeyWordEnum)
  {
    if(CheckError(static_cast<TypeClass*>(type), type))
    {
      exit(0);
    }
    this->symbols_[type->name()] = type;
  }
  else
  {
    AddStruct(static_cast<TypeClass*>(type));
  }
}

void Generator::AddTypedef(Node *type)
{
  if(CheckError(static_cast<TypeClass*>(type), type))
  {
    exit(0);
  }
  this->message_typedef_.push_back(type);
  this->user_types_[type->name()] = type;
}

bool Generator::CheckError(TypeClass *type, Node *field)
{
  if(field->type() == kKeyWordEnum)
  {
    if(field->name().length())
    {
      if(IsKeyWord(field->name()))
      {
        PrintKeyWordError(field);
        return true;
      }
      if(IsDefinedType(field->name()))
      {
        PrintTypeExistError(field);
        return true;
      }
    }

    if(field->is_field())
    {
      if(IsDefinedSymbol(field->name()))
      {
        PrintSymbolExistError(field);
        return true;
      }
    }
    return false;
  }
  else if(field->type() == kKeyWordStruct || field->type() == kKeyWordMessage || field->type() == kKeyWordTypedef)
  {
    if(IsKeyWord(field->name()))
    {
      PrintKeyWordError(field);
      return true;
    }
    if(IsDefinedType(field->name()))
    {
      PrintTypeExistError(field);
      return true;
    }
    return false;
  }
  else if(field->type() == kKeyWordEnableIf)
  {
    int error = 0;
    FiledsEnableIf *enable_if = static_cast<FiledsEnableIf*>(field);
    std::vector<Node*>& nodes = static_cast<FiledsEnableIf*>(field)->fields();
    for(size_t i = 0; i < nodes.size(); ++i)
    {
      error += CheckError(type, nodes[i]);
    }

    Node *param1_field = NULL;
    Node *param2_field = NULL;

    for(std::vector<Node*>::const_iterator iter = type->fields().begin();
        iter != type->fields().end() && *iter != field;
        ++iter)
    {
      if((*iter)->name() == enable_if->param1())
        param1_field = *iter;
      else if((*iter)->name() == enable_if->param2())
        param2_field = *iter;
      if(param1_field && param2_field)
        break;
    }
    if(!param1_field)
      param1_field = symbols_[enable_if->param1()];
    if(!param2_field)
      param2_field = symbols_[enable_if->param2()];

    if(!param1_field)
    {
      PrintSymbolNotExistError(enable_if->param1(), enable_if->line_number());
      error += 1;
    }
    if(!param2_field)
    {
      PrintSymbolNotExistError(enable_if->param2(), enable_if->line_number());
      error += 1;
    }
    enable_if->param1_field(param1_field);
    enable_if->param2_field(param2_field);

    return error;
  }
  else if(field->type() == kKeyWordString)
  {
    if(IsKeyWord(field->name()))
    {
      PrintKeyWordError(field);
      return true;
    }
  }
  else if(field->type() == kKeyWordArray)
  {
    FieldArray *field_array = static_cast<FieldArray*>(field);
    if(!IsDefinedType(field_array->field_type()))
    {
      PrintTypeNotExistError(field_array->field_type(), field_array->line_number());
      return true;
    }
  }
  else
  {
    if(!IsDefinedType(field->type()))
    {
      PrintTypeNotExistError(field->type(), field->line_number());
      return true;
    }
  }

  return false;
}

void Generator::Generate()
{
  this->InitGenerate();

  for(size_t idx = 0; idx < this->enum_.size(); ++idx)
  {
    if(this->enum_[idx]->is_skip())
      continue;
    this->GenerateEnum(this->enum_[idx]);
  }

  for(size_t idx = 0; idx < this->struct_.size(); ++idx)
  {
    if(this->struct_[idx]->is_skip())
      continue;
    this->GenerateStruct(this->struct_[idx]);
  }

  for(size_t idx = 0; idx < this->message_.size(); ++idx)
  {
    if(this->message_[idx]->is_skip())
      continue;
    this->GenerateMessage(this->message_[idx]);
  }

  for(size_t idx = 0; idx < this->message_typedef_.size(); ++idx)
  {
    this->GenerateTypedef(this->message_typedef_[idx]);
  }

  this->CloseGenerate();
}

