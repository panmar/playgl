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
        // TODO(panmar): Uncomment me, when store injection working
        // return STORE[full_name];
        return local_store[name];
    }

    const std::string id;

protected:
    Store local_store;

private:
    static std::string generate_id() {
        static int64_t id = 1;
        return fmt::format("__id:{}__", id++);
    }
};

#define PROPERTY(type, name) type& name = (*this)[#name]
