#include "index_list.h"

/* #region 前置声明 */

template <typename T, unsigned int window_size>
class IndexListWindow;

template <typename T, unsigned int window_size>
void RemoveAndIncrement(IndexList<T>& lst, IndexListWindow<T, window_size>& window, int window_index_to_remove);

/* #endregion 前置声明 */

/* #region IndexListWindow */

template <typename T, unsigned int window_size>
class IndexListWindow
{
private:
    using IterVector = std::array<typename IndexList<T>::Iterator, window_size>;
    IterVector iters_;
    typename IndexList<T>::Iterator begin_iter, end_iter_;
    size_t head_;

private:
    /// 接口索引值 -> 内部容器索引值
    size_t VecIndex(size_t index);
    size_t VecIndex(int index);

    /// 只从某一个点开始向后自增(含index)
    /// 配合删除操作一起使用。
    void MoveForwardAfter(int index);

public:
    /// 构造一个索引表窗口，起始位置在所给lst的开头
    IndexListWindow(IndexList<T>& lst);
    /// 构造一个索引表窗口，起始位置在iter的位置
    IndexListWindow(IndexList<T>& lst, const typename IndexList<T>::Iterator& iter);
    ~IndexListWindow() = default;

    /// 拷贝构造
    IndexListWindow(const IndexListWindow& other);
    /// 拷贝赋值
    IndexListWindow& operator=(const IndexListWindow& other);
    /// 移动构造
    IndexListWindow(IndexListWindow&& other);
    /// 移动赋值
    IndexListWindow& operator=(IndexListWindow&& other);

    /// 向前移动
    void MoveForward();
    /// 向前移动
    IndexListWindow& operator++();
    /// 向后移动
    void MoveBackward();
    /// 向后移动
    IndexListWindow& operator--();

    /// 获取特定下标的迭代器
    typename IndexList<T>::Iterator IteratorAt(size_t index);
    /// 获取特定下标的内容
    typename IndexList<T>::Component& At(size_t index);
    /// 获取特定下标的内容
    typename IndexList<T>::Component& operator[](size_t index);
    /// 是否左越界
    bool IsLeftOutOfBound();
    /// 是否右越界
    bool IsRightOutOfBound();
    /// 是否越界
    bool IsOutOfBound();

public:
    friend void RemoveAndIncrement<>(IndexList<T>& lst, IndexListWindow<T, window_size>& window, int window_index_to_remove);
};

/* #endregion IndexListWindow */

/* #region IndexListWindow实现 */

/* #region 构造 */

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>::IndexListWindow(IndexList<T>& lst)
{
    this->begin_iter = lst.begin();
    this->end_iter_ = lst.end();
    this->iters_ = IndexListWindow<T, window_size>::IterVector();
    typename IndexList<T>::Iterator iter0 = begin_iter;
    for (int i = 0; i < window_size; ++i)
    {
        iters_[i] = iter0;
        if (iter0 != end_iter_)
        {
            ++iter0;
        }
    }
    this->head_ = 0;
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>::IndexListWindow(
    IndexList<T>& lst,
    const typename IndexList<T>::Iterator& iter)
{
    this->begin_iter = lst.begin();
    this->end_iter_ = lst.end();
    this->iters_ = IndexListWindow<T, window_size>::IterVector();
    typename IndexList<T>::Iterator iter0 = iter;
    for (int i = 0; i < window_size; ++i)
    {
        iters_[i] = iter0;
        if (iter0 != end_iter_)
        {
            ++iter0;
        }
    }
    this->head_ = 0;
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>::IndexListWindow(const IndexListWindow& other)
{
    this->iters_ = other.iters_;
    this->begin_iter_ = other.begin_iter_;
    this->end_iter_ = other.end_iter_;
    this->head_ = other.head_;
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>& IndexListWindow<T, window_size>::operator=(const IndexListWindow& other)
{
    this->iters_ = other.iters_;
    this->begin_iter_ = other.begin_iter_;
    this->end_iter_ = other.end_iter_;
    this->head_ = other.head_;
    return *this;
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>::IndexListWindow(IndexListWindow&& other)
{
    this->iters_ = std::move(other.iters_);
    this->begin_iter_ = std::move(other.begin_iter_);
    this->end_iter_ = std::move(other.end_iter_);
    this->head_ = other.head_;
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>& IndexListWindow<T, window_size>::operator=(IndexListWindow&& other)
{
    this->iters_ = std::move(other.iters_);
    this->begin_iter_ = std::move(other.begin_iter_);
    this->end_iter_ = std::move(other.end_iter_);
    this->head_ = other.head_;
}

/* #endregion 构造 */

/* #region 私有方法 */

template <typename T, unsigned int window_size>
size_t IndexListWindow<T, window_size>::VecIndex(size_t index)
{
    return (this->head_ + index) % window_size;
}

template <typename T, unsigned int window_size>
size_t IndexListWindow<T, window_size>::VecIndex(int index)
{
    return (this->head_ + index + window_size) % window_size;
}

template <typename T, unsigned int window_size>
void IndexListWindow<T, window_size>::MoveForwardAfter(int index)
{
    constexpr int last = window_size - 1;
    for (int i = index; i < last; ++i)
    {
        this->iters_[this->VecIndex(index)] = this->iters_[this->VecIndex(index + 1)];
    }
    ++this->iters_[this->VecIndex(last)];
}

/* #endregion 私有方法 */

/* #region 接口 */

template <typename T, unsigned int window_size>
void IndexListWindow<T, window_size>::MoveForward()
{
    int a = this->VecIndex(-1);
    this->iters_[this->head_] = this->iters_[this->VecIndex(-1)];
    if (this->iters_[this->head_] != this->end_iter_)
    {
        ++this->iters_[this->head_];
    }
    this->head_ = this->VecIndex(1);
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>& IndexListWindow<T, window_size>::operator++()
{
    this->MoveForward();
    return *this;
}

template <typename T, unsigned int window_size>
void IndexListWindow<T, window_size>::MoveBackward()
{
    this->head_ = this->VecIndex(-1);
    this->iters_[this->head_] = this->iters_[this->VecIndex(1)];
    if (this->iters_[this->head] != this->begin_iter_)
    {
        --this->iters_[this->head_];
    }
}

template <typename T, unsigned int window_size>
IndexListWindow<T, window_size>& IndexListWindow<T, window_size>::operator--()
{
    this->MoveBackward();
    return *this;
}

template <typename T, unsigned int window_size>
typename IndexList<T>::Iterator IndexListWindow<T, window_size>::IteratorAt(size_t index)
{
    return this->iters_[this->VecIndex(index)];
}

template <typename T, unsigned int window_size>
typename IndexList<T>::Component& IndexListWindow<T, window_size>::At(size_t index)
{
    return *this->iters_[this->VecIndex(index)];
}

template <typename T, unsigned int window_size>
typename IndexList<T>::Component& IndexListWindow<T, window_size>::operator[](size_t index)
{
    return *this->iters_[this->VecIndex(index)];
}

template <typename T, unsigned int window_size>
bool IndexListWindow<T, window_size>::IsLeftOutOfBound()
{
    return this->iters_[this->VecIndex(1)] == this->begin_iter_;
}

template <typename T, unsigned int window_size>
bool IndexListWindow<T, window_size>::IsRightOutOfBound()
{
    return this->iters_[this->VecIndex(-1)] == this->end_iter_;
}

template <typename T, unsigned int window_size>
bool IndexListWindow<T, window_size>::IsOutOfBound()
{
    return this->IsLeftOutOfBound() || this->IsRightOutOfBound();
}

/* #endregion 接口 */

/* #endregion IndexListWindow实现 */

/* #region 联动函数 */

/// 将window中的指定元素从lst当中删除，同时window向前移动
template <typename T, unsigned int window_size>
void RemoveAndIncrement(IndexList<T>& lst, IndexListWindow<T, window_size>& window, int window_index_to_remove)
{
    auto iter = window.IteratorAt(window_index_to_remove);
    window.MoveForwardAfter(window_index_to_remove);
    lst.erase(iter);
}

/* #endregion */