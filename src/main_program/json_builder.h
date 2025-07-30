#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include "json.h"

namespace json {

    Node GetNodeFromValue(Node::Value &value);

    class Builder {
    private:
        class DictContext;
        class ArrayContext;
        class ItemContext;
        class DictValueContext;


        template<typename T>
        void StartData(T obj);
        void EndData();
        Node root_ = nullptr;
        std::vector<Node*> nodes_stack_;
        std::deque<Node> nodes_;

    public:
        DictValueContext Key(std::string key);
        Builder& Value(Node::Value value);
        DictContext StartDict();
        ArrayContext StartArray();
        ItemContext EndDict();
        ItemContext EndArray();
        Node Build();

    };

    class Builder::ItemContext{
    public:
        ItemContext(Builder& builder) : builder_(builder) {}
        Node Build();
        DictValueContext Key(std::string key);
        ItemContext Value(Node::Value value);
        DictContext StartDict();
        ArrayContext StartArray();
        ItemContext EndDict();
        ItemContext EndArray();
    private:
        Builder& builder_;
    };

    class Builder::DictValueContext : public ItemContext {
    public:
        DictContext Value(Node::Value value);
        Node Build() = delete;
        DictValueContext Key(std::string key) = delete;
        ItemContext EndDict() = delete;
        ItemContext EndArray() = delete;
    };

    class Builder::DictContext : public ItemContext {
    public:
        Node Build() = delete;
        ItemContext Value(Node::Value value) = delete;
        ItemContext EndArray() = delete;
        DictContext StartDict() = delete;
        ArrayContext StartArray() = delete;
    };

    class Builder::ArrayContext : public ItemContext {
    public:
        ArrayContext Value(Node::Value value);
        Node Build() = delete;
        DictValueContext Key(std::string key) = delete;
        ItemContext EndDict() = delete;
    };

    template<typename T>
    void Builder::StartData(T obj) {
        using namespace std::literals;
        std::string str;
        if constexpr (std::is_same<T, Array>::value) {
            str = "Array";
        } else {
            str = "Dict";
        }
        if (root_ != nullptr) throw std::logic_error("Корневой узел пуст, нельзя для него вызвать метод"s + str);
        if (nodes_stack_.empty() || nodes_stack_.back()->IsArray() || nodes_stack_.back()->IsString()) {
            nodes_.emplace_back(obj);
            nodes_stack_.push_back(&nodes_.back());
        } else {
            throw std::logic_error("Запуск "s + str + "- метода в данном месте не поддерживается."s);
        }
    }
}
