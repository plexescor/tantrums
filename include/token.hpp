#pragma once

#include <string>
#include <string_view>
#include <cstdint>
enum class TokenType
{
	TOKEN_THROWS,
	TOKEN_INT8,
	TOKEN_INT16,
	TOKEN_INT32,
	TOKEN_INT64,
	TOKEN_INT128,
	TOKEN_UINT8,
	TOKEN_UINT16,
	TOKEN_UINT32,
	TOKEN_UINT64,
	TOKEN_UINT128,
	TOKEN_FLOAT32,
	TOKEN_FLOAT64,
	TOKEN_STRING,
	TOKEN_BOOL,
	TOKEN_MUT,
	TOKEN_ERROR,
	TOKEN_TRY,
	TOKEN_CATCH,
	TOKEN_VOID,
	TOKEN_TRUE,
	TOKEN_FALSE,
	TOKEN_RETURN,
	TOKEN_PURE,
	TOKEN_IO,
	TOKEN_HEAP,
	TOKEN_USE,
	TOKEN_IDENTIFIER,
	TOKEN_INTEGER_LITERAL,
	TOKEN_FLOAT_LITERAL,
	TOKEN_BOOL_LITERAL,
	TOKEN_STRING_LITERAL,
	TOKEN_SINGLE_QUOTE,
	TOKEN_DOUBLE_QUOTE,
	TOKEN_LEFT_PARENTHESIS,
	TOKEN_RIGHT_PARENTHESIS,
	TOKEN_LEFT_BRACE,
	TOKEN_COMMA,
	TOKEN_RIGHT_BRACE,
	TOKEN_LEFT_BRACKET,
	TOKEN_EQUALITY_OPERATOR,
	TOKEN_NOT_EQUALITY_OPERATOR,
	TOKEN_NOT_OPERATOR,
	TOKEN_RIGHT_BRACKET,
	TOKEN_SEMICOLON,
	TOKEN_LAZY_RETURN_OPERATOR,
	TOKEN_MODULE_NAMESPACE_CROSS_OPERATOR,
	TOKEN_METHOD_MEMBER_ACCESS_OPERATOR,
	TOKEN_CHAIN_OPERATOR,
	TOKEN_ASSIGNMENT_OPERATOR,
	TOKEN_PLUS_OPERATOR,
	TOKEN_MINUS_OPERATOR,
	TOKEN_STAR_OPERATOR,
	TOKEN_DIVISION_OPERATOR,
	TOKEN_GREATER_THAN_OPERATOR,
	TOKEN_LESS_THAN_OPERATOR,
	TOKEN_GREATER_THAN_OR_EQUAL_OPERATOR,
	TOKEN_LESS_THAN_OR_EQUAL_OPERATOR,
	TOKEN_NULLABLE_OPERATOR,
	TOKEN_NULL_COALESCING_OPERATOR,
	TOKEN_COMPOUND_ADD_OPERATOR,
	TOKEN_COMPOUND_SUBTRACT_OPERATOR,
	TOKEN_COMPOUND_MULTIPLY_OPERATOR,
	TOKEN_COMPOUND_DIVIDE_OPERATOR,
	TOKEN_INCREMENT_OPERATOR,
	TOKEN_DECREMENT_OPERATOR,
	TOKEN_IF,
	TOKEN_ELSE,
	TOKEN_FOR,
	TOKEN_WHILE,
	TOKEN_BREAK,
	TOKEN_CONTINUE,
	TOKEN_AUTO,
	TOKEN_AMPERSAND_OPERATOR,
	TOKEN_END_OF_FILE
};

constexpr std::string_view tokenTypeToString(TokenType type)
{
	switch (type)
	{
		// Types & Keywords
		case TokenType::TOKEN_AUTO: return "auto";
		case TokenType::TOKEN_THROWS: return "throws";
		case TokenType::TOKEN_INT8: return "int8";
		case TokenType::TOKEN_INT16: return "int16";
		case TokenType::TOKEN_INT32: return "int32";
		case TokenType::TOKEN_INT64: return "int64";
		case TokenType::TOKEN_INT128: return "int128";
		case TokenType::TOKEN_UINT8: return "uint8";
		case TokenType::TOKEN_UINT16: return "uint16";
		case TokenType::TOKEN_UINT32: return "uint32";
		case TokenType::TOKEN_UINT64: return "uint64";
		case TokenType::TOKEN_UINT128: return "uint128";
		case TokenType::TOKEN_FLOAT32: return "float32";
		case TokenType::TOKEN_FLOAT64: return "float64";
		case TokenType::TOKEN_STRING: return "string";
		case TokenType::TOKEN_BOOL: return "bool";
		case TokenType::TOKEN_ERROR: return "error";
		case TokenType::TOKEN_TRY: return "try";
		case TokenType::TOKEN_CATCH: return "catch";
		case TokenType::TOKEN_VOID: return "void";
		case TokenType::TOKEN_TRUE: return "true";
		case TokenType::TOKEN_FALSE: return "false";
		case TokenType::TOKEN_RETURN: return "return";
		case TokenType::TOKEN_PURE: return "pure";
		case TokenType::TOKEN_IO: return "io";
		case TokenType::TOKEN_HEAP: return "heap";
		case TokenType::TOKEN_USE: return "use";
		case TokenType::TOKEN_IF: return "if";
		case TokenType::TOKEN_ELSE: return "else";
		case TokenType::TOKEN_FOR: return "for";
		case TokenType::TOKEN_MUT: return "mut";
		case TokenType::TOKEN_WHILE: return "while";
		case TokenType::TOKEN_BREAK: return "break";
		case TokenType::TOKEN_CONTINUE: return "continue";
		case TokenType::TOKEN_IDENTIFIER: return "<identifier>";
		case TokenType::TOKEN_INTEGER_LITERAL: return "<integer literal>";
		case TokenType::TOKEN_FLOAT_LITERAL: return "<float literal>";
		case TokenType::TOKEN_BOOL_LITERAL: return "<bool literal>";
		case TokenType::TOKEN_STRING_LITERAL: return "<string literal>";
		case TokenType::TOKEN_SINGLE_QUOTE: return "'";
		case TokenType::TOKEN_DOUBLE_QUOTE: return "\"";
		case TokenType::TOKEN_LEFT_PARENTHESIS: return "(";
		case TokenType::TOKEN_RIGHT_PARENTHESIS: return ")";
		case TokenType::TOKEN_LEFT_BRACE: return "{";
		case TokenType::TOKEN_RIGHT_BRACE: return "}";
		case TokenType::TOKEN_LEFT_BRACKET: return "[";
		case TokenType::TOKEN_RIGHT_BRACKET: return "]";
		case TokenType::TOKEN_COMMA: return ",";
		case TokenType::TOKEN_SEMICOLON: return ";";
		case TokenType::TOKEN_ASSIGNMENT_OPERATOR: return "=";
		case TokenType::TOKEN_EQUALITY_OPERATOR: return "==";
		case TokenType::TOKEN_NOT_EQUALITY_OPERATOR: return "!=";
		case TokenType::TOKEN_NOT_OPERATOR: return "!";
		case TokenType::TOKEN_PLUS_OPERATOR: return "+";
		case TokenType::TOKEN_MINUS_OPERATOR: return "-";
		case TokenType::TOKEN_STAR_OPERATOR: return "*";
		case TokenType::TOKEN_DIVISION_OPERATOR: return "/";
		case TokenType::TOKEN_GREATER_THAN_OPERATOR: return ">";
		case TokenType::TOKEN_LESS_THAN_OPERATOR: return "<";
		case TokenType::TOKEN_GREATER_THAN_OR_EQUAL_OPERATOR: return ">=";
		case TokenType::TOKEN_LESS_THAN_OR_EQUAL_OPERATOR: return "<=";
		case TokenType::TOKEN_AMPERSAND_OPERATOR: return "&";
		case TokenType::TOKEN_NULLABLE_OPERATOR: return "?";
		case TokenType::TOKEN_NULL_COALESCING_OPERATOR: return "??";
		case TokenType::TOKEN_COMPOUND_ADD_OPERATOR: return "+=";
		case TokenType::TOKEN_COMPOUND_SUBTRACT_OPERATOR: return "-=";
		case TokenType::TOKEN_COMPOUND_MULTIPLY_OPERATOR: return "*=";
		case TokenType::TOKEN_COMPOUND_DIVIDE_OPERATOR: return "/=";
		case TokenType::TOKEN_INCREMENT_OPERATOR: return "++";
		case TokenType::TOKEN_DECREMENT_OPERATOR: return "--";
		case TokenType::TOKEN_LAZY_RETURN_OPERATOR: return "<~~";
		case TokenType::TOKEN_MODULE_NAMESPACE_CROSS_OPERATOR: return "!->";
		case TokenType::TOKEN_METHOD_MEMBER_ACCESS_OPERATOR: return "->";
		case TokenType::TOKEN_CHAIN_OPERATOR: return "<-->";
		case TokenType::TOKEN_END_OF_FILE: return "<EOF>";
		default: return "<unknown>";
	}
}

constexpr std::string_view tokenTypeToEnumName(TokenType type)
{
	switch (type)
	{
		case TokenType::TOKEN_THROWS: return "TOKEN_THROWS";
		case TokenType::TOKEN_AUTO: return "TOKEN_AUTO";
		case TokenType::TOKEN_MUT: return "TOKEN_MUT";
		case TokenType::TOKEN_INT8: return "TOKEN_INT8";
		case TokenType::TOKEN_INT16: return "TOKEN_INT16";
		case TokenType::TOKEN_INT32: return "TOKEN_INT32";
		case TokenType::TOKEN_INT64: return "TOKEN_INT64";
		case TokenType::TOKEN_INT128: return "TOKEN_INT128";
		case TokenType::TOKEN_UINT8: return "TOKEN_UINT8";
		case TokenType::TOKEN_UINT16: return "TOKEN_UINT16";
		case TokenType::TOKEN_UINT32: return "TOKEN_UINT32";
		case TokenType::TOKEN_UINT64: return "TOKEN_UINT64";
		case TokenType::TOKEN_UINT128: return "TOKEN_UINT128";
		case TokenType::TOKEN_FLOAT32: return "TOKEN_FLOAT32";
		case TokenType::TOKEN_FLOAT64: return "TOKEN_FLOAT64";
		case TokenType::TOKEN_STRING: return "TOKEN_STRING";
		case TokenType::TOKEN_BOOL: return "TOKEN_BOOL";
		case TokenType::TOKEN_ERROR: return "TOKEN_ERROR";
		case TokenType::TOKEN_TRY: return "TOKEN_TRY";
		case TokenType::TOKEN_CATCH: return "TOKEN_CATCH";
		case TokenType::TOKEN_VOID: return "TOKEN_VOID";
		case TokenType::TOKEN_TRUE: return "TOKEN_TRUE";
		case TokenType::TOKEN_FALSE: return "TOKEN_FALSE";
		case TokenType::TOKEN_RETURN: return "TOKEN_RETURN";
		case TokenType::TOKEN_PURE: return "TOKEN_PURE";
		case TokenType::TOKEN_IO: return "TOKEN_IO";
		case TokenType::TOKEN_HEAP: return "TOKEN_HEAP";
		case TokenType::TOKEN_USE: return "TOKEN_USE";
		case TokenType::TOKEN_IDENTIFIER: return "TOKEN_IDENTIFIER";
		case TokenType::TOKEN_INTEGER_LITERAL: return "TOKEN_INTEGER_LITERAL";
		case TokenType::TOKEN_FLOAT_LITERAL: return "TOKEN_FLOAT_LITERAL";
		case TokenType::TOKEN_BOOL_LITERAL: return "TOKEN_BOOL_LITERAL";
		case TokenType::TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";
		case TokenType::TOKEN_SINGLE_QUOTE: return "TOKEN_SINGLE_QUOTE";
		case TokenType::TOKEN_DOUBLE_QUOTE: return "TOKEN_DOUBLE_QUOTE";
		case TokenType::TOKEN_LEFT_PARENTHESIS: return "TOKEN_LEFT_PARENTHESIS";
		case TokenType::TOKEN_RIGHT_PARENTHESIS: return "TOKEN_RIGHT_PARENTHESIS";
		case TokenType::TOKEN_LEFT_BRACE: return "TOKEN_LEFT_BRACE";
		case TokenType::TOKEN_COMMA: return "TOKEN_COMMA";
		case TokenType::TOKEN_RIGHT_BRACE: return "TOKEN_RIGHT_BRACE";
		case TokenType::TOKEN_LEFT_BRACKET: return "TOKEN_LEFT_BRACKET";
		case TokenType::TOKEN_EQUALITY_OPERATOR: return "TOKEN_EQUALITY_OPERATOR";
		case TokenType::TOKEN_NOT_EQUALITY_OPERATOR: return "TOKEN_NOT_EQUALITY_OPERATOR";
		case TokenType::TOKEN_NOT_OPERATOR: return "TOKEN_NOT_OPERATOR";
		case TokenType::TOKEN_RIGHT_BRACKET: return "TOKEN_RIGHT_BRACKET";
		case TokenType::TOKEN_SEMICOLON: return "TOKEN_SEMICOLON";
		case TokenType::TOKEN_LAZY_RETURN_OPERATOR: return "TOKEN_LAZY_RETURN_OPERATOR";
		case TokenType::TOKEN_MODULE_NAMESPACE_CROSS_OPERATOR: return "TOKEN_MODULE_NAMESPACE_CROSS_OPERATOR";
		case TokenType::TOKEN_METHOD_MEMBER_ACCESS_OPERATOR: return "TOKEN_METHOD_MEMBER_ACCESS_OPERATOR";
		case TokenType::TOKEN_CHAIN_OPERATOR: return "TOKEN_CHAIN_OPERATOR";
		case TokenType::TOKEN_ASSIGNMENT_OPERATOR: return "TOKEN_ASSIGNMENT_OPERATOR";
		case TokenType::TOKEN_PLUS_OPERATOR: return "TOKEN_PLUS_OPERATOR";
		case TokenType::TOKEN_MINUS_OPERATOR: return "TOKEN_MINUS_OPERATOR";
		case TokenType::TOKEN_STAR_OPERATOR: return "TOKEN_STAR_OPERATOR";
		case TokenType::TOKEN_DIVISION_OPERATOR: return "TOKEN_DIVISION_OPERATOR";
		case TokenType::TOKEN_GREATER_THAN_OPERATOR: return "TOKEN_GREATER_THAN_OPERATOR";
		case TokenType::TOKEN_LESS_THAN_OPERATOR: return "TOKEN_LESS_THAN_OPERATOR";
		case TokenType::TOKEN_GREATER_THAN_OR_EQUAL_OPERATOR: return "TOKEN_GREATER_THAN_OR_EQUAL_OPERATOR";
		case TokenType::TOKEN_LESS_THAN_OR_EQUAL_OPERATOR: return "TOKEN_LESS_THAN_OR_EQUAL_OPERATOR";
		case TokenType::TOKEN_NULLABLE_OPERATOR: return "TOKEN_NULLABLE_OPERATOR";
		case TokenType::TOKEN_NULL_COALESCING_OPERATOR: return "TOKEN_NULL_COALESCING_OPERATOR";
		case TokenType::TOKEN_COMPOUND_ADD_OPERATOR: return "TOKEN_COMPOUND_ADD_OPERATOR";
		case TokenType::TOKEN_COMPOUND_SUBTRACT_OPERATOR: return "TOKEN_COMPOUND_SUBTRACT_OPERATOR";
		case TokenType::TOKEN_COMPOUND_MULTIPLY_OPERATOR: return "TOKEN_COMPOUND_MULTIPLY_OPERATOR";
		case TokenType::TOKEN_COMPOUND_DIVIDE_OPERATOR: return "TOKEN_COMPOUND_DIVIDE_OPERATOR";
		case TokenType::TOKEN_INCREMENT_OPERATOR: return "TOKEN_INCREMENT_OPERATOR";
		case TokenType::TOKEN_DECREMENT_OPERATOR: return "TOKEN_DECREMENT_OPERATOR";
		case TokenType::TOKEN_IF: return "TOKEN_IF";
		case TokenType::TOKEN_ELSE: return "TOKEN_ELSE";
		case TokenType::TOKEN_FOR: return "TOKEN_FOR";
		case TokenType::TOKEN_WHILE: return "TOKEN_WHILE";
		case TokenType::TOKEN_BREAK: return "TOKEN_BREAK";
		case TokenType::TOKEN_CONTINUE: return "TOKEN_CONTINUE";
		case TokenType::TOKEN_AMPERSAND_OPERATOR: return "TOKEN_AMPERSAND_OPERATOR";
		case TokenType::TOKEN_END_OF_FILE: return "TOKEN_END_OF_FILE";
		default: return "UNKNOWN";
	}
}

struct Token
{
	TokenType type;
	std::string value;
	uint64_t line;
	uint64_t column;
};