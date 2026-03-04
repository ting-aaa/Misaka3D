#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace Misaka {

struct JsonValue;
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

struct JsonValue {
    using ValueType = std::variant<std::nullptr_t, bool, double, std::string, JsonArray, JsonObject>;

    ValueType value;

    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool v);
    JsonValue(double v);
    JsonValue(const std::string& v);
    JsonValue(std::string&& v);
    JsonValue(const JsonArray& v);
    JsonValue(JsonArray&& v);
    JsonValue(const JsonObject& v);
    JsonValue(JsonObject&& v);

    bool IsNull() const;
    bool IsBool() const;
    bool IsNumber() const;
    bool IsString() const;
    bool IsArray() const;
    bool IsObject() const;

    bool AsBool(bool defaultValue = false) const;
    double AsNumber(double defaultValue = 0.0) const;
    std::string AsString(const std::string& defaultValue = "") const;

    const JsonArray* AsArray() const;
    const JsonObject* AsObject() const;

    const JsonValue* Find(const std::string& key) const;
};

class SimpleJsonParser {
public:
    static bool Parse(const std::string& text, JsonValue& outValue, std::string* outError = nullptr);
};

} // namespace Misaka
