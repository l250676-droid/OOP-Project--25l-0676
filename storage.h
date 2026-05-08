#ifndef STORAGE_H
#define STORAGE_H

 template<typename T>
class Storage {
    T data[100];
    int count;
public:
    Storage() : count(0) {}
    bool add(const T& item);
    bool removeByID(int id);
    T* findByID(int id);
    T* getAll() { return data; }
    int size() const { return count; }
    void clear() { count = 0; }
};
 
template<typename T>
bool Storage<T>::add(const T& item) {
    if (count >= 100) return false;
    data[count++] = item;
    return true;
}
 
template<typename T>
bool Storage<T>::removeByID(int id) {
    for (int i = 0; i < count; i++) {
        if (data[i].getID() == id) {
            for (int j = i; j < count - 1; j++)
                data[j] = data[j+1];
            count--;
            return true;
        }
    }
    return false;
}
 
template<typename T>
T* Storage<T>::findByID(int id) {
    for (int i = 0; i < count; i++)
        if (data[i].getID() == id) return &data[i];
    return nullptr;
}
#endif
