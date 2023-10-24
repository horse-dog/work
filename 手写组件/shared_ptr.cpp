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

template <class T, class Del>
struct ctrl_blk : Del
{
    ctrl_blk(T* pointer) : ptr(pointer), ref_count(1), weak_count(1) {}

    ~ctrl_blk() {}

    void add_ref_copy() { ++ref_count; }

    bool add_ref_lock()
    {
        auto count = ref_count.load();
        do
        {
            if (count == 0)
            {
                return false;
            }
        } while (!ref_count.compare_exchange_weak(count, count + 1));
        return true;
    }

    void release()
    {
        if (std::atomic_fetch_sub_explicit(&ref_count, 1, std::memory_order_acq_rel) == 1)
        {
            (*this)(ptr);
            ptr = nullptr;
            if (std::atomic_fetch_sub_explicit(&weak_count, 1, std::memory_order_acq_rel) == 1)
            {
                delete this;
            }
        }
    }

    void add_weak_count() { ++weak_count; }

    void release_weak()
    {
        if (std::atomic_fetch_sub_explicit(&weak_count, 1, std::memory_order_acq_rel) == 1)
        {
            delete this;
        }
    }

    T* ptr;
    std::atomic<int> ref_count;
    std::atomic<int> weak_count;
};

template <class T, class Del = Deleter<T>>
class weak_ptr;

template <class T, class Del = Deleter<T>>
class shared_ptr
{
public:
    constexpr shared_ptr() : m_blk(nullptr) {}

    constexpr shared_ptr(std::nullptr_t) : m_blk(nullptr) {}

    explicit shared_ptr(T *ptr) { m_blk = new ctrl_blk<T, Del>(ptr); }

    shared_ptr(const shared_ptr &s) : m_blk(s.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_ref_copy();
        }
    }

    shared_ptr(shared_ptr &&s) : m_blk(s.m_blk) 
    {
        s.m_blk = nullptr;
    }

    explicit shared_ptr(const weak_ptr<T, Del> &w);

    ~shared_ptr()
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
            m_blk = nullptr;
        }
    }

    T *get() const { return m_blk->ptr; }

    explicit operator bool() const { return m_blk != nullptr && m_blk.ptr != nullptr; }

    T &operator*() const { return *(m_blk->ptr); }

    T *operator->() const { return m_blk ? m_blk->ptr : nullptr; }

    shared_ptr &operator=(const shared_ptr &s)
    {
        if (this != &s)
        {
            if (m_blk != nullptr)
            {
                m_blk->release();
            }
            m_blk = s.m_blk;
            if (m_blk != nullptr)
            {
                m_blk->add_ref_copy();
            }
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&s)
    {
        if (this != &s)
        {
            this->m_blk = s.m_blk;
            s.m_blk = nullptr;
        }
        return *this;
    }

    shared_ptr &operator=(std::nullptr_t)
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
        }
        m_blk = nullptr;
        return *this;
    }

    void reset()
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
        }
        m_blk = nullptr;
    }

    void reset(T *p)
    {
        if (m_blk != nullptr)
        {
            if (m_blk->ptr == p)
            {
                return;
            }
            m_blk->release();
        }
        m_blk = new ctrl_blk<T, Del>(p);
    }

    void swap(shared_ptr &s)
    {
        auto tmp = this->m_blk;
        this->m_blk = s.m_blk;
        s.m_blk = tmp;
    }

    bool unique() const { return m_blk ? m_blk->ref_count == 1 : false; }

    long use_count() { return m_blk ? m_blk->ref_count.load() : 0; }

private:
    ctrl_blk<T, Del>* m_blk;

friend class weak_ptr<T, Del>;
};


template <class T, class Del>
class shared_ptr<T[], Del>
{
public:
    constexpr shared_ptr() : m_blk(nullptr) {}

    constexpr shared_ptr(std::nullptr_t) : m_blk(nullptr) {}

    explicit shared_ptr(T *ptr) { m_blk = new ctrl_blk<T, Del>(ptr); }

    shared_ptr(const shared_ptr &s) : m_blk(s.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_ref_copy();
        }
    }

    explicit shared_ptr(const weak_ptr<T[], Del> &w);

    shared_ptr(shared_ptr &&s) : m_blk(s.m_blk) 
    {
        s.m_blk = nullptr;
    }

    ~shared_ptr()
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
            m_blk = nullptr;
        }
    }

    T *get() const { return m_blk->ptr; }

    explicit operator bool() const { return m_blk != nullptr && m_blk.ptr != nullptr; }

    T &operator[](std::size_t i) const { return m_blk->ptr[i]; }

    shared_ptr &operator=(const shared_ptr &s)
    {
        if (this != &s)
        {
            if (m_blk != nullptr)
            {
                m_blk->release();
            }
            m_blk = s.m_blk;
            if (m_blk != nullptr)
            {
                m_blk->add_ref_copy();
            }
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&s)
    {
        if (this != &s)
        {
            if (m_blk != nullptr)
            {
                m_blk->release();
            }
            m_blk = s.m_blk;
            s.m_blk = nullptr;
        }
        return *this;
    }

    shared_ptr &operator=(std::nullptr_t)
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
        }
        m_blk = nullptr;
        return *this;
    }

    void reset()
    {
        if (m_blk != nullptr)
        {
            m_blk->release();
        }
        m_blk = nullptr;
    }

    void reset(T *p)
    {
        if (m_blk != nullptr)
        {
            if (m_blk->ptr == p)
            {
                return;
            }
            m_blk->release();
        }
        m_blk = new ctrl_blk<T, Del>(p);
    }

    void swap(shared_ptr &s)
    {
        auto tmp = this->m_blk;
        this->m_blk = s.m_blk;
        s.m_blk = tmp;
    }

    bool unique() const { return m_blk ? m_blk->ref_count == 1 : false; }

    long use_count() { return m_blk ? m_blk->ref_count.load() : 0; }

private:
    ctrl_blk<T, Del>* m_blk;

friend class weak_ptr<T[], Del>;
};


template <class T, class Del>
class weak_ptr
{
public:
    constexpr weak_ptr() : m_blk(nullptr) {}

    weak_ptr(const shared_ptr<T, Del>& s) : m_blk(s.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
    }

    weak_ptr(const weak_ptr& w) : m_blk(w.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
    }

    weak_ptr(weak_ptr&& w) : m_blk(w.m_blk)
    {
        w.m_blk = nullptr;
    }

    ~weak_ptr()
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
    }

    bool expired() const { return m_blk ? m_blk->ref_count == 0 : true; }

    shared_ptr<T, Del> lock() const
    {
        return shared_ptr<T, Del>(*this);
    }

    weak_ptr& operator=(const weak_ptr& __r)
    {
        if (this != &__r)
        {
            if (m_blk != nullptr)
            {
                m_blk->release_weak();
            }
            m_blk = __r.m_blk;
            if (m_blk != nullptr)
            {
                m_blk->add_weak_count();
            }
        }
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& __r)
    {
        if (this != &__r)
        {
            if (m_blk != nullptr)
            {
                m_blk->release_weak();
            }
            m_blk = __r.m_blk;
            __r.m_blk = nullptr;
        }
        return *this;
    }

    weak_ptr& operator=(const shared_ptr<T, Del>& __r)
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
        m_blk = __r.m_blk;
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
        return *this;
    }

    void reset()
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
        m_blk = nullptr;
    }

    void swap(weak_ptr& __s)
    {
        auto tmp = m_blk;
        m_blk = __s.m_blk;
        __s.m_blk = tmp;
    }

    long use_count() const { return m_blk ? m_blk->ref_count.load() : 0; }

private:
    ctrl_blk<T, Del>* m_blk;

friend class shared_ptr<T, Del>;
};


template <class T, class Del>
class weak_ptr<T[], Del>
{
public:
    constexpr weak_ptr() : m_blk(nullptr) {}

    weak_ptr(const shared_ptr<T[], Del>& s) : m_blk(s.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
    }

    weak_ptr(const weak_ptr& w) : m_blk(w.m_blk)
    {
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
    }

    weak_ptr(weak_ptr&& w) : m_blk(w.m_blk)
    {
        w.m_blk = nullptr;
    }

    ~weak_ptr()
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
    }

    bool expired() const { return m_blk ? m_blk->ref_count == 0 : true; }

    shared_ptr<T[], Del> lock() const
    {
        return shared_ptr<T[], Del>(*this);
    }

    weak_ptr& operator=(const weak_ptr& __r)
    {
        if (this != &__r)
        {
            if (m_blk != nullptr)
            {
                m_blk->release_weak();
            }
            m_blk = __r.m_blk;
            if (m_blk != nullptr)
            {
                m_blk->add_weak_count();
            }
        }
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& __r)
    {
        if (this != &__r)
        {
            if (m_blk != nullptr)
            {
                m_blk->release_weak();
            }
            m_blk = __r.m_blk;
            __r.m_blk = nullptr;
        }
        return *this;
    }

    weak_ptr& operator=(const shared_ptr<T[], Del>& __r)
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
        m_blk = __r.m_blk;
        if (m_blk != nullptr)
        {
            m_blk->add_weak_count();
        }
        return *this;
    }

    void reset()
    {
        if (m_blk != nullptr)
        {
            m_blk->release_weak();
        }
        m_blk = nullptr;
    }

    void swap(weak_ptr& __s)
    {
        auto tmp = m_blk;
        m_blk = __s.m_blk;
        __s.m_blk = tmp;
    }

    long use_count() const { return m_blk ? m_blk->ref_count.load() : 0; }

private:
    ctrl_blk<T, Del>* m_blk;

friend class shared_ptr<T[], Del>;
};


template <class T, class Del>
shared_ptr<T, Del>::shared_ptr(const weak_ptr<T, Del> &w)
{
    ctrl_blk<T, Del>* tmp = w.m_blk;
    if (tmp != nullptr)
    {
        if (tmp->add_ref_lock())
        {
            m_blk = tmp;
            return;
        }
    }
    m_blk = nullptr;
}

template <class T, class Del>
shared_ptr<T[], Del>::shared_ptr(const weak_ptr<T[], Del> &w)
{
    ctrl_blk<T, Del>* tmp = w.m_blk;
    if (tmp != nullptr)
    {
        if (tmp->add_ref_lock())
        {
            m_blk = tmp;
            return;
        }
    }
    m_blk = nullptr;
}

}

#include <memory>
#include <iostream>
using namespace smt_ptr;

class BB;

class AA
{
public:
    weak_ptr<BB> bptr;
    AA() : bptr(nullptr) {}
    ~AA() { std::cout << "~AA()" << std::endl; }
};

class BB
{
public:
    weak_ptr<AA> aptr;
    BB() : aptr(nullptr) {}
    ~BB() { std::cout << "~BB()" << std::endl; }
};
 
int main( ) {
    shared_ptr<AA> aa(new AA());
    shared_ptr<BB> bb(new BB());
    aa->bptr = bb;
    bb->aptr = aa;
    std::cout << aa.use_count() << std::endl;
    std::cout << bb.use_count() << std::endl;
    return 0;
}
