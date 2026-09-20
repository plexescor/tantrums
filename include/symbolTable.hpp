#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>
#include "ast.hpp"

class SymbolTable
{
	public:
		SymbolTable();
		~SymbolTable();

		void pushScope();
		void popScope();
		void declare(std::string& name, std::string& type, bool isMutable);
		void declareFunction(std::string& name, std::string& type, std::vector<ParameterNode>& params);

		std::optional<std::pair<std::string, bool>> lookup(std::string& name);
		std::optional<std::pair<std::string, std::vector<ParameterNode>>> lookupFunction(std::string& name);
		bool existsInCurrentScope(std::string& name);

	private:
		// vector of: names -> { type, isMutable }
		std::vector<std::map<std::string, std::pair<std::string, bool>>> scopes;
		std::map<std::string, std::pair<std::string, std::vector<ParameterNode>>> functionDeclarations;
};