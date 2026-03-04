#include "SimpleJson.h"

#include <cctype>
#include <cstdlib>

namespace Misaka {

JsonValue::JsonValue() : value(nullptr) {}
JsonValue::JsonValue(std::nullptr_t) : value(nullptr) {}
JsonValue::JsonValue(bool v) : value(v) {}
JsonValue::JsonValue(double v) : value(v) {}
JsonValue::JsonValue(const std::string& v) : value(v) {}
JsonValue::JsonValue(std::string&& v) : value(std::move(v)) {}
JsonValue::JsonValue(const JsonArray& v) : value(v) {}
JsonValue::JsonValue(JsonArray&& v) : value(std::move(v)) {}
JsonValue::JsonValue(const JsonObject& v) : value(v) {}
JsonValue::JsonValue(JsonObject&& v) : value(std::move(v)) {}

bool JsonValue::IsNull() const { return std::holds_alternative<std::nullptr_t>(value); }
bool JsonValue::IsBool() const { return std::holds_alternative<bool>(value); }
bool JsonValue::IsNumber() const { return std::holds_alternative<double>(value); }
bool JsonValue::IsString() const { return std::holds_alternative<std::string>(value); }
bool JsonValue::IsArray() const { return std::holds_alternative<JsonArray>(value); }
bool JsonValue::IsObject() const { return std::holds_alternative<JsonObject>(value); }

bool JsonValue::AsBool(bool defaultValue) const {
    if (auto p = std::get_if<bool>(&value)) return *p;
    return defaultValue;
}

double JsonValue::AsNumber(double defaultValue) const {
    if (auto p = std::get_if<double>(&value)) return *p;
    return defaultValue;
}

std::string JsonValue::AsString(const std::string& defaultValue) const {
    if (auto p = std::get_if<std::string>(&value)) return *p;
    return defaultValue;
}

const JsonArray* JsonValue::AsArray() const {
    return std::get_if<JsonArray>(&value);
}

const JsonObject* JsonValue::AsObject() const {
    return std::get_if<JsonObject>(&value);
}

const JsonValue* JsonValue::Find(const std::string& key) const {
    const JsonObject* obj = AsObject();
    if (!obj) return nullptr;
    auto it = obj->find(key);
    if (it == obj->end()) return nullptr;
    return &it->second;
}

namespace {

class Parser {
public:
    explicit Parser(const std::string& text) : _text(text) {}

    bool Parse(JsonValue& outValue, std::string* outError) {
        SkipWhitespace();
        if (!ParseValue(outValue)) {
            if (outError) *outError = _error;
            return false;
        }

        SkipWhitespace();
        if (!IsEnd()) {
            SetError("Unexpected trailing characters");
            if (outError) *outError = _error;
            return false;
        }

        return true;
    }

private:
    const std::string& _text;
    size_t _pos = 0;
    std::string _error;

    bool IsEnd() const {
        return _pos >= _text.size();
    }

    char Peek() const {
        return IsEnd() ? '\0' : _text[_pos];
    }

    char Advance() {
        return IsEnd() ? '\0' : _text[_pos++];
    }

    void SkipWhitespace() {
        while (!IsEnd() && std::isspace(static_cast<unsigned char>(Peek()))) {
            ++_pos;
        }
    }

    void SetError(const std::string& message) {
        if (_error.empty()) {
            _error = message + " at position " + std::to_string(_pos);
        }
    }

    bool Match(const char* keyword) {
        size_t start = _pos;
        while (*keyword) {
            if (IsEnd() || Advance() != *keyword) {
                _pos = start;
                return false;
            }
            ++keyword;
        }
        return true;
    }

    bool ParseValue(JsonValue& outValue) {
        SkipWhitespace();
        const char c = Peek();

        if (c == '{') return ParseObject(outValue);
        if (c == '[') return ParseArray(outValue);
        if (c == '"') return ParseString(outValue);
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return ParseNumber(outValue);
        if (Match("true")) {
            outValue = JsonValue(true);
            return true;
        }
        if (Match("false")) {
            outValue = JsonValue(false);
            return true;
        }
        if (Match("null")) {
            outValue = JsonValue(nullptr);
            return true;
        }

        SetError("Invalid value");
        return false;
    }

    bool ParseObject(JsonValue& outValue) {
        if (Advance() != '{') {
            SetError("Expected '{'");
            return false;
        }

        JsonObject object;
        SkipWhitespace();

        if (Peek() == '}') {
            Advance();
            outValue = JsonValue(std::move(object));
            return true;
        }

        while (!IsEnd()) {
            JsonValue keyValue;
            if (!ParseString(keyValue)) return false;
            std::string key = keyValue.AsString();

            SkipWhitespace();
            if (Advance() != ':') {
                SetError("Expected ':'");
                return false;
            }

            JsonValue value;
            if (!ParseValue(value)) return false;
            object.emplace(std::move(key), std::move(value));

            SkipWhitespace();
            char c = Advance();
            if (c == '}') {
                outValue = JsonValue(std::move(object));
                return true;
            }
            if (c != ',') {
                SetError("Expected ',' or '}'");
                return false;
            }

            SkipWhitespace();
        }

        SetError("Unterminated object");
        return false;
    }

    bool ParseArray(JsonValue& outValue) {
        if (Advance() != '[') {
            SetError("Expected '['");
            return false;
        }

        JsonArray array;
        SkipWhitespace();
        if (Peek() == ']') {
            Advance();
            outValue = JsonValue(std::move(array));
            return true;
        }

        while (!IsEnd()) {
            JsonValue element;
            if (!ParseValue(element)) return false;
            array.push_back(std::move(element));

            SkipWhitespace();
            char c = Advance();
            if (c == ']') {
                outValue = JsonValue(std::move(array));
                return true;
            }
            if (c != ',') {
                SetError("Expected ',' or ']'");
                return false;
            }

            SkipWhitespace();
        }

        SetError("Unterminated array");
        return false;
    }

    bool ParseString(JsonValue& outValue) {
        if (Advance() != '"') {
            SetError("Expected '\"'");
            return false;
        }

        std::string result;
        while (!IsEnd()) {
            char c = Advance();
            if (c == '"') {
                outValue = JsonValue(std::move(result));
                return true;
            }
            if (c == '\\') {
                if (IsEnd()) {
                    SetError("Invalid escape sequence");
                    return false;
                }
                char esc = Advance();
                switch (esc) {
                    case '"': result.push_back('"'); break;
                    case '\\': result.push_back('\\'); break;
                    case '/': result.push_back('/'); break;
                    case 'b': result.push_back('\b'); break;
                    case 'f': result.push_back('\f'); break;
                    case 'n': result.push_back('\n'); break;
                    case 'r': result.push_back('\r'); break;
                    case 't': result.push_back('\t'); break;
                    default:
                        SetError("Unsupported escape sequence");
                        return false;
                }
            } else {
                result.push_back(c);
            }
        }

        SetError("Unterminated string");
        return false;
    }

    bool ParseNumber(JsonValue& outValue) {
        const size_t start = _pos;

        if (Peek() == '-') Advance();

        if (!std::isdigit(static_cast<unsigned char>(Peek()))) {
            SetError("Invalid number");
            return false;
        }

        while (std::isdigit(static_cast<unsigned char>(Peek()))) Advance();

        if (Peek() == '.') {
            Advance();
            if (!std::isdigit(static_cast<unsigned char>(Peek()))) {
                SetError("Invalid number fractional part");
                return false;
            }
            while (std::isdigit(static_cast<unsigned char>(Peek()))) Advance();
        }

        if (Peek() == 'e' || Peek() == 'E') {
            Advance();
            if (Peek() == '+' || Peek() == '-') Advance();
            if (!std::isdigit(static_cast<unsigned char>(Peek()))) {
                SetError("Invalid number exponent");
                return false;
            }
            while (std::isdigit(static_cast<unsigned char>(Peek()))) Advance();
        }

        const std::string token = _text.substr(start, _pos - start);
        char* endPtr = nullptr;
        const double value = std::strtod(token.c_str(), &endPtr);
        if (!endPtr || *endPtr != '\0') {
            SetError("Failed to parse number");
            return false;
        }

        outValue = JsonValue(value);
        return true;
    }
};

} // namespace

bool SimpleJsonParser::Parse(const std::string& text, JsonValue& outValue, std::string* outError) {
    Parser parser(text);
    return parser.Parse(outValue, outError);
}

} // namespace Misaka
