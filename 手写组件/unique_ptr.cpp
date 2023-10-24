#include <atomic>

namespace smt_ptr {

template <class T>
struct Deleter
{
    void operator()(T *p) { delete p; }
};


template <class T>
struct Deleter<T[]>
{
    void operator()(T *p) { delete[] p; }
};


template <class T, class Del = Deleter<T>>
class unique_ptr
{
public:
    unique_ptr() : m_impl(nullptr) {}

    unique_ptr(std::nullptr_t) : m_impl(nullptr) {}

    explicit unique_ptr(T *ptr) : m_impl(ptr) {}

    unique_ptr(unique_ptr &&u) : m_impl(u.m_impl.m_ptr) { u.m_impl.m_ptr = nullptr; }

    ~unique_ptr()
    {
        m_impl(m_impl.m_ptr);
        m_impl.m_ptr = nullptr;
    }

    T *get() const { return m_impl.m_ptr; }

    Del &get_deleter() { return m_impl; }

    const Del &get_deleter() const { return m_impl; }

    explicit operator bool() const { return m_impl.m_ptr != nullptr; }

    T &operator*() const { return *(m_impl.m_ptr); }

    T *operator->() const { return m_impl.m_ptr; }

    unique_ptr &operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }

    unique_ptr &operator=(unique_ptr &&__u)
    {
        if (this != &__u)
        {
            m_impl(m_impl.m_ptr);
            m_impl.m_ptr = __u.m_impl;
            __u.m_impl = nullptr;
        }
        return *this;
    }

    T *release()
    {
        T *res = m_impl.m_ptr;
        m_impl.m_ptr = nullptr;
        return res;
    }

    void reset(T *p = nullptr)
    {
        m_impl(m_impl.m_ptr);
        m_impl.m_ptr = p;
    }

    void swap(unique_ptr &u)
    {
        T *tmp = m_impl.m_ptr;
        m_impl.m_ptr = u.m_impl.m_ptr;
        u.m_impl.m_ptr = tmp;
    }

  private:

    struct Impl : Del 
    {
        T* m_ptr;
        Impl(T* p) : m_ptr(p), Del() {}
    };

    Impl m_impl;
};



template <class T, class Del>
class unique_ptr<T[], Del>
{
public:
    unique_ptr() : m_impl(nullptr) {}

    unique_ptr(std::nullptr_t) : m_impl(nullptr) {}

    explicit unique_ptr(T *ptr) : m_impl(ptr) {}

    unique_ptr(unique_ptr &&u) : m_impl(u.m_impl.m_ptr) { u.m_impl.m_ptr = nullptr; }

    ~unique_ptr()
    {
        m_impl(m_impl.m_ptr);
        m_impl.m_ptr = nullptr;
    }

    T *get() const { return m_impl.m_ptr; }

    Del &get_deleter() { return m_impl; }

    const Del &get_deleter() const { return m_impl; }

    explicit operator bool() const { return m_impl.m_ptr != nullptr; }

    T& operator[](std::size_t i) const { return m_impl.m_ptr[i]; }

    unique_ptr &operator=(std::nullptr_t)
    {
        reset();
        return *this;
    }

    unique_ptr &operator=(unique_ptr &&__u)
    {
        if (this != &__u)
        {
            m_impl(m_impl.m_ptr);
            m_impl.m_ptr = __u.m_impl;
            __u.m_impl = nullptr;
        }
        return *this;
    }

    T *release()
    {
        T *res = m_impl.m_ptr;
        m_impl.m_ptr = nullptr;
        return res;
    }

    void reset(T *p = nullptr)
    {
        m_impl(m_impl.m_ptr);
        m_impl.m_ptr = p;
    }

    void swap(unique_ptr &u)
    {
        T *tmp = m_impl.m_ptr;
        m_impl.m_ptr = u.m_impl.m_ptr;
        u.m_impl.m_ptr = tmp;
    }

  private:

    struct Impl : Del 
    {
        T* m_ptr;
        Impl(T* p) : m_ptr(p), Del() {}
    };

    Impl m_impl;
};

}

using namespace smt_ptr;
#include <memory>

struct Test
{
    ~Test() { printf("dtor\n"); }
};

int main(int argc, const char* argv[]) 
{
    unique_ptr<Test[]> p(new Test[3]);
    return 0;
}