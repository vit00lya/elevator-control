#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

using namespace std::literals;

class Node;
class Document;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>  {
public:

    using variant::variant;
    using Value = variant;
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(std::string value);
    Node(double value);
    Node(std::nullptr_t);
    Node(bool);

    Array& AsArray();
    Dict& AsMap() ;

    const Array& AsArray() const;
    const Dict& AsMap() const;
    int AsInt() const;
    double AsDouble() const;
    bool AsBool() const;
    const std::string& AsString() const;

    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    const Value& GetValue() const { return data_; }

    bool operator==(const Node& other) const {
        return (GetValue() == other.GetValue());
    }

    bool operator!=(const Node& other) const {
        return (GetValue() != other.GetValue());
    }

private:
    Value data_;

};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& other) const {
        return (GetRoot() == other.GetRoot());
    }
private:
    Node root_;
};

Document Load(std::istream& input);

void PrintNode(const Node& node, std::ostream& out);

void PrintValue(std::nullptr_t, std::ostream& out);
void PrintValue(std::string val, std::ostream& out);
void PrintValue(double val, std::ostream& out);
void PrintValue(int val, std::ostream& out);
void PrintValue(const Dict &val, std::ostream& out);
void PrintValue(bool val, std::ostream& out);
void PrintValue(const Array &val, std::ostream& out);

void Print(const Document& doc, std::ostream& output);

}  // namespace json
