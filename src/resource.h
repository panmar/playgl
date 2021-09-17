#pragma once

template <class T>
class LazyResource {
public:
    using ResourceDeleter = void (*)(T&);

    LazyResource(ResourceDeleter deleter) : deleter(deleter) {}

    LazyResource(LazyResource&& other)
        : _resource(other._resource),
          created(other.created),
          deleter(other.deleter) {
        other.created = false;
    }

    virtual ~LazyResource() { release(); }

    void release() {
        if (created) {
            created = false;
            deleter(_resource);
        }
    };

    const T& resource() const {
        if (!created) {
            _resource = create_resource();
            created = true;
        }
        return _resource;
    }

protected:
    virtual T create_resource() const = 0;

private:
    mutable T _resource;
    mutable bool created = false;
    ResourceDeleter deleter;
};