#include <bits/stdc++.h>


class Obserable;


class Observer : public std::enable_shared_from_this<Observer>
{
friend class Obserable;

public:
    Observer() = default;
   ~Observer();

    virtual void update() = 0;

    void peek(const std::shared_ptr<Obserable>& peekObj);

    void unpeek();

protected:
    std::weak_ptr<Obserable> peekObject;
};


class Obserable
{
friend class Observer;

using ObserverList = std::vector<std::weak_ptr<Observer>>;

public:
    Obserable() : observers(std::make_shared<ObserverList>()) {}

    void notify() const
    {
        decltype(observers) current_observers;
        {
            std::lock_guard<std::mutex> lock(locker);
            current_observers = observers;
        }

        for (auto&& observer : *current_observers)
        {
            auto x = observer.lock();
            if (x != nullptr)
            {
                x->update();
            }
        }
    }

    void regist(const std::weak_ptr<Observer>& observer)
    {
        {
            std::lock_guard<std::mutex> lock(locker);
            if (!observers.unique())
            {
                observers = std::make_shared<ObserverList>(*observers);
            }
            observers->push_back(observer);
        }
    }

    void unregist()
    {
        std::lock_guard<std::mutex> lock(locker);
        if (!observers.unique())
        {
            observers = std::make_shared<ObserverList>(*observers);
        }
        for (auto it = observers->begin(); it != observers->end();)
        {
            if (it->expired())
            {
                it = observers->erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    std::shared_ptr<ObserverList> observers;
    mutable std::mutex locker;
};


Observer::~Observer()
{
    unpeek();
}

void Observer::peek(const std::shared_ptr<Obserable>& peekObj)
{
    unpeek();
    peekObject = peekObj;
    peekObj->regist(shared_from_this());
}

void Observer::unpeek()
{
    auto p = peekObject.lock();
    if (p != nullptr)
    {
        p->unregist();
    }
}

struct Data : public Obserable
{
    void push_back(int x)
    {
        data.push_back(x);
    }

    std::vector<int> data;
};


class Graph : public Observer
{
public:
    void update() override
    {
        auto data = peekObject.lock();
        if (data != nullptr) {
            auto ptr = (Data*)data.get();
            for (auto x : ptr->data)
            {
                std::cout << x << ' ';
            }
            std::cout << std::endl;
        }
    }
};


int main(int argc, const char* argv[])
{
    std::shared_ptr<Data> data = std::make_shared<Data>();
    std::shared_ptr<Graph> g1 = std::make_shared<Graph>();
    std::shared_ptr<Graph> g2 = std::make_shared<Graph>();

    g1->peek(data);
    g2->peek(data);

    int x;
    while (std::cin >> x)
    {
        if (x == 0) 
        {
            break;
        }

        data->push_back(x);
        data->notify();
    }
    std::unique_ptr<int> a;
    return 0;
}