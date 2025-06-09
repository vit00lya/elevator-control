#include "json_builder.h"
using namespace std::literals;
namespace json {

    Node GetNodeFromValue(Node::Value &value)
    {
       if (std::holds_alternative<double>(value))
       {
           return
           {   std::get<double>(value)};
       }
       else if (std::holds_alternative<int>(value))
       {
           return
           {   std::get<int>(value)};
       }
       else if (std::holds_alternative<bool>(value))
       {
           return
           {   std::get<bool>(value)};
       }
       else if (std::holds_alternative<Dict>(value))
       {
           return
           {   std::get<Dict>(value)};
       }
       else if (std::holds_alternative<Array>(value))
       {
           return
           {   std::get<Array>(value)};
       }
       else if (std::holds_alternative<std::string>(value))
       {
           return
           {   std::get<std::string>(value)};
       }
       else if (std::holds_alternative<std::nullptr_t>(value))
       {
           return
           {   std::get<std::nullptr_t>(value)};
       }
       return
       {};
    }

    Builder::DictValueContext Builder::Key(std::string key) {
        if (root_ != nullptr) throw std::logic_error("Корневой узел пуст, нельзя для него вызвать метод `Ключ`"s);
        if (nodes_stack_.back()->IsMap()) {
            Node::Value str{std::move(key)}; // @suppress("Invalid arguments")
            nodes_.emplace_back(GetNodeFromValue(str));
            nodes_stack_.push_back(&nodes_.back());
        } else {
            throw std::logic_error("Вызов метода `Ключ` не для словаря не поддерживается."s);
        }
        return {*this};
    }

    Builder& Builder::Value(Node::Value value) {
        if (root_ != nullptr) throw std::logic_error("Корневой узел пуст, нельзя для него вызвать метод `Значение`"s);
        if (root_ == nullptr && nodes_stack_.empty()) {
            root_ = GetNodeFromValue(value);
        } else if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.back()->AsArray().emplace_back(GetNodeFromValue(value));
        } else if (nodes_stack_.back()->IsString()) {
            Node& node_ref =*nodes_stack_.back();
            nodes_stack_.pop_back();
            nodes_stack_.back()->AsMap().insert({node_ref.AsString(), GetNodeFromValue(value)});
        } else {
            throw std::logic_error("Вызвов метода `Значение` не в том месте."s);
        }
        return *this;
    }

    Builder::DictContext Builder::StartDict() {
        StartData(Dict{});
        return {*this};
    }

    Builder::ArrayContext Builder::StartArray() {
        StartData(Array{});
        return {*this};
    }

    Builder::ItemContext Builder::EndDict() {
        if (nodes_stack_.empty()) throw std::logic_error("Корневой узел пуст, нельзя для него вызвать метод `Конец словаря`."s);
        if (nodes_stack_.back()->IsMap()) {
            EndData();
            return *this;
        } else {
            throw std::logic_error("Вызов ключевого метода не для словаря не поддерживается."s);
        }
    }

    Builder::ItemContext Builder::EndArray() {
        if (nodes_stack_.empty()) throw std::logic_error("Корневой узел пуст, нельзя вызвать для него вызвать метод `Конец массива`"s);
        if (nodes_stack_.back()->IsArray()) {
            EndData();
            return *this;
        } else {
            throw std::logic_error("Вызов метода `Конец массива` не для массива не поддерживается."s);
        }
    }

    Node Builder::Build() {
        if (nodes_stack_.empty() && root_ != nullptr) {
            return root_;
        } else {
            throw std::logic_error("Корневой узел пуст, нельзя для него вызвать метод `Сформировать`."s);
        }
    }

    void Builder::EndData() {
        Node& node_ref = *nodes_stack_.back();
        nodes_stack_.pop_back();
        if (nodes_stack_.empty()) {
            root_ = std::move(node_ref);
        } else if (nodes_stack_.back()->IsArray()) {
            nodes_stack_.back()->AsArray().emplace_back(std::move(node_ref));
        } else if (nodes_stack_.back()->IsString()) {
            Node& str_node_ref = *nodes_stack_.back();
            nodes_stack_.pop_back();
            nodes_stack_.back()->AsMap().insert({str_node_ref.AsString(), std::move(node_ref)});
        }
    }

    Builder::ItemContext Builder::ItemContext::EndDict() {
        builder_.EndDict();
        return builder_;
    }

    Builder::ItemContext Builder::ItemContext::EndArray() {
        builder_.EndArray();
        return builder_;
    }

    Builder::DictContext Builder::ItemContext::StartDict() {
        builder_.StartDict();
        return {*this};
    }

    Builder::ArrayContext Builder::ItemContext::StartArray() {
        builder_.StartArray();
        return {*this};
    }

    Builder::ItemContext Builder::ItemContext::Value(Node::Value value) {
        builder_.Value(std::move(value));
        return {*this};
    }

    Builder::DictValueContext Builder::ItemContext::Key(std::string key) {
        builder_.Key(std::move(key));
        return {*this};
    }

    Builder::ArrayContext Builder::ArrayContext::Value(Node::Value value){
        Builder::ItemContext::Value(std::move(value));
        return {*this};
    }

    Builder::DictContext Builder::DictValueContext::Value(Node::Value value){
        Builder::ItemContext::Value(std::move(value));
        return {*this};
    }

    Node Builder::ItemContext::Build(){
        return builder_.Build();
    }



}
