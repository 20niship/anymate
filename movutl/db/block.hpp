#pragma once 
#include <string>

#ifndef ENTITY_NAME_MAX_LENGTH
#define ENTITY_NAME_MAX_LENGTH 30
#endif

namespace mu::db{

class Block{
protected:
  char m_name[ENTITY_NAME_MAX_LENGTH];
public:
  const char *name()const {return m_name;}
  void name(const std::string_view name);
};

} //namespace mu::db
