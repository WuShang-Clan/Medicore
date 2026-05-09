#pragma once

//  Storage<T> — Generic Template Container
//  Template class needs to be defined and declared at the same time so we wont have .cpp for it

template <typename T>
class Storage {
private:
    T   data[100];
    int count;

public:
    Storage() : count(0) {}

    bool add(const T& item) {
        if (count >= 100) return false;
        data[count++] = item;
        return true;
    }

    // Remove by ID — shifts array left, returns false if not found
    bool removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                for (int j = i; j < count - 1; j++)
                    data[j] = data[j + 1];
                count--;
                return true;
            }
        }
        return false;
    }

    // Find by ID — returns pointer or nullptr
    T* findById(int id) {
        for (int i = 0; i < count; i++)
            if (data[i].getId() == id) return &data[i];
        return nullptr;
    }

    const T* findById(int id) const {
        for (int i = 0; i < count; i++)
            if (data[i].getId() == id) return &data[i];
        return nullptr;
    }

    // Raw array access for iteration
    T* getAll() { return data; }
    const T* getAll() const { return data; }

    int  size()    const { return count; }
    bool isEmpty() const { return count == 0; }
    void clear() { count = 0; }

    // Index access
    T& operator[](int i) { return data[i]; }
    const T& operator[](int i) const { return data[i]; }

    // Max ID currently stored — used for auto-increment
    int getMaxId() const {
        int maxId = 0;
        for (int i = 0; i < count; i++)
            if (data[i].getId() > maxId) maxId = data[i].getId();
        return maxId;
    }
};