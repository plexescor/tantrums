#include <vector>
#include <map>
#include <optional>

#include "symbolTable.hpp"

SymbolTable::SymbolTable()
{

}

SymbolTable::~SymbolTable()
{

}

void SymbolTable::pushScope()
{
    scopes.push_back({});
}

void SymbolTable::popScope()
{
    scopes.pop_back();
}

void SymbolTable::declare(std::string& name, std::string& type, bool isMutable)
{
    scopes.back()[name] = std::make_pair(type, isMutable);
}

std::optional<std::pair<std::string, bool>> SymbolTable::lookup(std::string& name)
{
    // Loop thorugh the back
    // cz then inner scope could see outer
    for (int i = scopes.size() - 1; i >= 0; --i) 
    {
        if (scopes[i].contains(name)) return scopes[i][name];
    }
    return std::nullopt;
}

bool SymbolTable::existsInCurrentScope(std::string& name)
{
    return scopes.back().contains(name);
}