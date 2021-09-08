#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "store.h"

class Object {
public:
    Object(const std::string& id = "") : id(id) {}
    virtual ~Object() = default;

    StoreParam& operator[](const std::string& name) {
        if (id.empty()) {
            // NOTE(panmar): Anonymous objects use local attribute store
            // Their state will not be serialized
            // Their state cannot be modifiable by gui
            return local_store[name];
        }
        auto full_name = id + "." + name;
        return STORE[full_name];
    }

    const std::string id;

protected:
    Store local_store;

private:
    static std::string generate_id() {
        static int64_t id = 1;
        return "__id_" + std::to_string(id++) + "__";
    }
};

#define PROPERTY(type, name) type& name = (*this)[#name]