#pragma once

#include <memory>

namespace bintree {
    template <typename T>
    /* Используем базовый класс для корректного раздельного владения объектом по this.
     */
    struct TNode: std::enable_shared_from_this<TNode<T>> {
        using TNodePtr = std::shared_ptr<TNode<T>>;
        using TNodeConstPtr = std::shared_ptr<const TNode<T>>;
        using TNodeWeakPtr = std::weak_ptr<TNode<T>>;

        bool hasLeft() const {
            return bool(left);
        }

        bool hasRight() const {
            return bool(right);
        }

        bool hasParent() const {
            return bool(parent.use_count());
        }

        T& getValue() {
            return value;
        }

        const T& getValue() const {
            return value;
        }

        TNodePtr getLeft() {
            return left;
        }

        TNodeConstPtr getLeft() const {
            return left;
        }

        TNodePtr getRight() {
            return right;
        }

        TNodeConstPtr getRight() const {
            return right;
        }

        TNodePtr getParent() {
            if (parent.use_count() == 0)
                return nullptr;
            else
                return parent.lock();
        }

        TNodeConstPtr getParent() const {
            if (parent.use_count() == 0)
                return nullptr;
            else
                return parent.lock();
        }

        static TNodePtr createLeaf(T v) {
            /* У make_shared() нет доступа к приватному конструктору класса, поэтому
             * создаем shared_ptr конструктором, передав в него указатель, созданный
             * оператором new.
             */
            auto tmp_ptr = new TNode(v);
            return TNodePtr(tmp_ptr);
        }

        /* Изменим аргументы метода: вместо обычных указателей будем использовать
         * shared_ptr. Это поможет избежать появление двух shared_ptr с одинаковыми
         * указателями на память с объектом, но с разными блоками управления и
         * следовательно разными счетчиками использования.
         */
        static TNodePtr fork(T v, TNodePtr left, TNodePtr right) {
            /* У make_shared() нет доступа к приватному конструктору класса, поэтому
             * создаем shared_ptr конструктором, передав в него указатель, созданный
             * оператором new.
             */
            auto tmp_ptr = new TNode(v, left, right);
            TNodePtr ptr(tmp_ptr);
            setParent(ptr->getLeft(), ptr);
            setParent(ptr->getRight(), ptr);
            return ptr;
        }

        TNodePtr replaceLeft(TNodePtr l) {
            /* Используем shared_from_this() для корректного конструирования shared_ptr
             * из this, то есть решаем проблему, когда создается несколько shared_ptr из
             * this, имеющих разные управляющие блоки).
             */
            setParent(l, this->shared_from_this());
            setParent(left, nullptr);
            std::swap(l, left);
            return l;
        }

        TNodePtr replaceRight(TNodePtr r) {
            /* Аналогично методу выше.
             */
            setParent(r, this->shared_from_this());
            setParent(right, nullptr);
            std::swap(r, right);
            return r;
        }

        TNodePtr replaceRightWithLeaf(T v) {
            return replaceRight(createLeaf(v));
        }

        TNodePtr replaceLeftWithLeaf(T v) {
            return replaceLeft(createLeaf(v));
        }

        TNodePtr removeLeft() {
            return replaceLeft(nullptr);
        }
        TNodePtr removeRight() {
            return replaceRight(nullptr);
        }

    private:
        T value;
        TNodePtr left = nullptr;
        TNodePtr right = nullptr;
        /* Для предотвращения проблемы с перекрестными shared_ptr (объекты содержат
         * указатели друг на друга) используем weak_ptr для поля parent. Это привело к
         * некоторым изменениям в методах hasParent() и getParent().
         */
        TNodeWeakPtr parent;

        TNode(T v)
            : value(v)
        {
        }
        /* Аналогично методу fork() изменим параметры на shared_ptr.
         */
        TNode(T v, TNodePtr left, TNodePtr right)
            : value(v)
            , left(left)
            , right(right)
        {
        }

        static void setParent(TNodePtr node, TNodePtr parent) {
            if (node)
                node->parent = parent;
        }
    };
}
