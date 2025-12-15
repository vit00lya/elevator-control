#include "json.h"

using namespace std;
using namespace literals;

namespace json {

namespace {

Node LoadNode(istream& input);

Node LoadArray(istream& input) {
    Array result;
    char c;
    for (; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (c != ']') {
        throw ParsingError("Нет закрывающей скобки в массиве"s);
    }

    return Node(move(result));
}

Node LoadInt(istream& input) {
    using namespace std::literals;

      std::string parsed_num;

      // Считывает в parsed_num очередной символ из input
      auto read_char = [&parsed_num, &input] {
          parsed_num += static_cast<char>(input.get());
          if (!input) {
              throw ParsingError("Failed to read number from stream"s);
          }
      };

      // Считывает одну или более цифр в parsed_num из input
      auto read_digits = [&input, read_char] {
          if (!std::isdigit(input.peek())) {
              throw ParsingError("A digit is expected"s);
          }
          while (std::isdigit(input.peek())) {
              read_char();
          }
      };

      if (input.peek() == '-') {
          read_char();
      }
      // Парсим целую часть числа
      if (input.peek() == '0') {
          read_char();
          // После 0 в JSON не могут идти другие цифры
      } else {
          read_digits();
      }

      bool is_int = true;
      // Парсим дробную часть числа
      if (input.peek() == '.') {
          read_char();
          read_digits();
          is_int = false;
      }

      // Парсим экспоненциальную часть числа
      if (int ch = input.peek(); ch == 'e' || ch == 'E') {
          read_char();
          if (ch = input.peek(); ch == '+' || ch == '-') {
              read_char();
          }
          read_digits();
          is_int = false;
      }

      try {
          if (is_int) {
              // Сначала пробуем преобразовать строку в int
              try {
                  return Node(std::stoi(parsed_num));
              } catch (...) {
                  // В случае неудачи, например, при переполнении,
                  // код ниже попробует преобразовать строку в double
              }
          }
          return Node(std::stod(parsed_num));
      } catch (...) {
          throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
      }
}

Node LoadNone(istream& input, char c){
    std::string s;
    s.push_back(c);
    auto it = std::istreambuf_iterator<char>(input);
      auto end = std::istreambuf_iterator<char>();
      while (it != end) {
          const char ch = *it;
          if(ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t'){
             if (ch == ',' || ch == ']' || ch == '}') break;
             s.push_back(ch);
          }
          ++it;
      }
      if(s != "null"sv )
          throw ParsingError("Не верное значение null"s);

    return Node(nullptr);
}

Node LoadBool(istream& input, char c){
    std::string s;
    s.push_back(c);
    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    while (it != end) {
        const char ch = *it;
        if(ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t'){
           if(ch == ',' || ch == '}' || ch == ']') break;
           s.push_back(ch);
        }
        ++it;
    }

    bool b;
    if(s == "true"sv )
        b = true;
    else if(s == "false"sv )
        b = false;
    else
        throw ParsingError("Не верно значение булевой переменной"s);

    return Node(b);

}

Node LoadString(istream& input) {
    using namespace std::literals;

    auto it = std::istreambuf_iterator<char>(input);
    auto end = std::istreambuf_iterator<char>();
    std::string s;
    while (true) {
        if (it == end) {
            // Поток закончился до того, как встретили закрывающую кавычку?
            throw ParsingError("String parsing error");
        }
        const char ch = *it;
        if (ch == '"') {
            // Встретили закрывающую кавычку
            ++it;
            break;
        } else if (ch == '\\') {
            // Встретили начало escape-последовательности
            ++it;
            if (it == end) {
                // Поток завершился сразу после символа обратной косой черты
                throw ParsingError("String parsing error");
            }
            const char escaped_char = *(it);
            // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
            switch (escaped_char) {
                case 'n':
                    s.push_back('\n');
                    break;
                case 't':
                    s.push_back('\t');
                    break;
                case 'r':
                    s.push_back('\r');
                    break;
                case '"':
                    s.push_back('"');
                    break;
                case '\\':
                    s.push_back('\\');
                    break;
                default:
                    // Встретили неизвестную escape-последовательность
                    throw ParsingError("Unrecognized escape sequence \\"s + escaped_char  + " Строка:" + s);
            }
        } else if (ch == '\n' || ch == '\r') {
            // Строковый литерал внутри- JSON не может прерываться символами \r или \n
            throw ParsingError("Unexpected end of line"s);
        } else {
            // Просто считываем очередной символ и помещаем его в результирующую строку
            s.push_back(ch);
        }
        ++it;
    }

    return Node(move(s));
}

Node LoadDict(istream& input) {
    Dict result;
    char c;
    for (; input >> c && c != '}';) {
        if (c == ',') {
            input >> c;
        }

        string key = LoadString(input).AsString();
        input >> c;
        result.insert({move(key), LoadNode(input)});
    }
    if (c != '}') {
           throw ParsingError("Нет закрывающей скобки в словаре"s);
    }

    return Node(move(result));
}

Node LoadNode(istream& input) {
    char c,c1;

    while(input >> c ){
        c1 = input.peek();
        if (c == '[') {
            return LoadArray(input);
        } else if (c == '{') {
            return LoadDict(input);
        } else if (c == '"') {
            return LoadString(input);
        } else if (c == 'n' && c1 == 'u'){
            return LoadNone(input, c);
        } else if ((c == 't' && c1 == 'r') || (c == 'f' && c1 == 'a')){
            return LoadBool(input, c);
        } else {
	   input.putback(c);
           return LoadInt(input);
        }
    }
    return Node(nullptr);
}


}  // namespace

Node::Node(Array array)
    : data_(move(array)) {}

Node::Node(Dict map)
    : data_(move(map)) {}

Node::Node(int value)
    : data_(value) {}

Node::Node(long value)
    : data_(value) {}

Node::Node(string value)
    : data_(move(value)) {}

Node::Node(double value)
    : data_(move(value)) {}

Node::Node(std::nullptr_t value)
    : data_(move(value)) {}

Node::Node(bool value)
    : data_(move(value)) {}

Array& Node::AsArray() {
    if (Array* pval = std::get_if<Array>(&data_)){
        return *pval;
    }
    else{
        throw std::logic_error("Ошибка, элемент JSON не является массивом"s);
    }
}

Dict& Node::AsMap() {
    if ( Dict* pval = std::get_if<Dict>(&data_)){
        return *pval;
    }
    else{
        throw std::logic_error("Ошибка, элемент JSON не является словарем"s);
    }
}

const Array& Node::AsArray() const {
    if (const Array* pval = std::get_if<Array>(&data_)){
        return *pval;
    }
    else{
        throw std::logic_error("Ошибка, элемент JSON не является массивом"s);
    }
}

const Dict& Node::AsMap() const {
    if (const Dict* pval = std::get_if<Dict>(&data_)){
        return *pval;
    }
    else{
        throw std::logic_error("Ошибка, элемент JSON не является словарем"s);
    }
}

int Node::AsInt() const {
    if (const int* pval = std::get_if<int>(&data_)){
         return *pval;
     }
     else{
         throw std::logic_error("Ошибка, элемент JSON не является int"s);
     }
}

const string& Node::AsString() const {
    if (const string* pval = std::get_if<string>(&data_)){
         return *pval;
     }
     else{
         throw std::logic_error("Ошибка, элемент JSON не является string"s);
     }
}

  long Node::AsLong() const {
    if (const long* pval = std::get_if<long>(&data_)){ 
         return *pval;
     }
     else{
         throw std::logic_error("Ошибка, элемент JSON не является long"s);
     }
}

double Node::AsDouble() const {
    if (const double* pval = std::get_if<double>(&data_)){
         return *pval;
     }
     else{
         if (const int* pval = std::get_if<int>(&data_)){
                 return *pval;
             }
         else{
             throw std::logic_error("Ошибка, элемент JSON не является double или int"s);
         }
     }
}

bool Node::AsBool() const {
    if (const bool* pval = std::get_if<bool>(&data_)){
         return *pval;
     }
     else{
         throw std::logic_error("Ошибка, элемент JSON не является bool"s);
     }
}

bool Node::IsInt() const{

    return (std::get_if<int>(&data_));
}

bool Node::IsLong() const{

    return (std::get_if<long>(&data_));
}

bool Node::IsDouble() const{
    return ((std::get_if<int>(&data_)) || (std::get_if<double>(&data_)));
}
bool Node::IsPureDouble() const{
    return  std::get_if<double>(&data_);
}
bool Node::IsBool() const{
    return  std::get_if<bool>(&data_);
}
bool Node::IsString() const{
    return  std::get_if<string>(&data_);
}
bool Node::IsNull() const{
    return  std::get_if<nullptr_t>(&data_);
}
bool Node::IsArray() const{
    return  std::get_if<Array>(&data_);
}
bool Node::IsMap() const{
    return  std::get_if<Dict>(&data_);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

void PrintNode(const Node& node, std::ostream& out) {
    std::visit(
        [&out](const auto& value){ PrintValue(value, out); },
        node.GetValue());
}

void PrintValue(std::nullptr_t, std::ostream& out) {
    out << "null"sv;
}

void PrintValue(std::string val, std::ostream& out) {
    out << '"';
    for (unsigned int i = 0; i < val.length(); i++) {
            switch(val[i]){
                case (34):  out << "\\\""sv; break;
                case ('\n'):  out << "\\n"sv; break;
                case ('\t'):  out << "\\t"sv; break;
                case ('\r'):  out << "\\r"sv; break;
                case (92):  out << "\\\\"sv; break;
                default: out << val[i];
            }
      }
    out << '"';
}

void PrintValue(double val, std::ostream& out) {
    out << val;
}

 void PrintValue(long val, std::ostream& out) {
    out << val;
}

void PrintValue(int val, std::ostream& out) {
    out << val ;
}

void PrintValue(const Dict &val, std::ostream& out) {
    size_t size = val.size();
    out << '{';
    size_t i = 0;
    for (const auto& [key, value]: val) {
        out << " \""sv <<  key << "\": "sv;
        PrintNode(value, out);
        if (i < size - 1) {
            out << ",";
        }
        ++i;
    }
    out << " }"sv;
}

void PrintValue(bool val, std::ostream& out) {
    out << std::boolalpha << val;
}

void PrintValue(const Array &val, std::ostream& out) {
    size_t size = val.size();
    out << '[';
    for(size_t i=0; i < size; ++i){
        PrintNode(val[i], out);
        if (i < size - 1) {
            out << ",";
        }
    }
    out << ']';
}


void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), output);

}

}  // namespace json
