#include <iostream>
#include <vector>

template <typename T>
class Array {
private:
    T* data;
    size_t size;

public:
    Array(size_t n)
        : size(n)
    {
        data = new T[n];
    }

    /* Для класса необходимо определить конструктор копирования и operator=, так как
     * генерируемые компилятором конструктор и operator= просто копируют указатель data. В
     * итоге может получиться два объекта с одинаковыми указателями data и при удалении
     * одного из них в деструкторе освободится память по этому указателю, и во втором
     * объекте указатель станет недействительным.
     * Нужно выделить новую область в памяти и скопировать туда содержимое из старой
     * области памяти.
     */
    Array(const Array<T>& a) {
        size = a.size;
        data = new T[size];
        for (size_t i = 0; i != size; ++i) {
            data[i] = a.data[i];
        }
    }

    Array& operator=(const Array<T>& a) {
        if (&a == this)
            return *this;

        delete[] data;
        size = a.size;
        data = new T[size];
        for (size_t i = 0; i != size; ++i) {
            data[i] = a.data[i];
        }
        return *this;
    }

    /* Также для оптимизации можно дополнить класс конструктором перемещения и
     * перемещающим operator=. Это не обязательно для того, чтобы код в main() работал.
     */
    Array(Array<T>&& a) {
        size = a.size;
        data = a.data;

        a.size = 0;
        a.data = nullptr;
    }

    Array& operator=(Array<T>&& a) {
        if (&a == this)
            return *this;

        delete[] data;
        size = a.size;
        data = a.data;

        a.size = 0;
        a.data = nullptr;

        return *this;
    }

    T& operator[](size_t i) {
        return data[i];
    }

    const T& operator[](size_t i) const {
        return data[i];
    }

    const size_t GetSize() const {
        return size;
    }

    ~Array() {
        delete[] data;
    }
};

template <typename T>
Array<T> GetArray(size_t n, T x) {
    Array<T> A(n);
    for (size_t i = 0; i != n; ++i) {
        A[i] = x;
    }
    return A;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const Array<T>& A) {
    for (size_t i = 0; i != A.GetSize(); ++i) {
        out << A[i] << " ";
    }
    out << "\n";
    return out;
}

int main() {
    size_t n;
    std::cin >> n;
    Array<int> A(n);
    A = GetArray<int>(n, 0);
    // print the array
    std::cout << A << "\n";

    std::vector<Array<int>> vec;
    Array<int> B(n);
    // fill vector of 100 arrays 
    for (size_t i = 0; i < 100; ++i) {
        B = GetArray<int>(n, i);
        vec.push_back(B);
    }
}
