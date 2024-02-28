#pragma once

#include <cmath>
#include <vector>
#include <map>
#include <iostream>
#include "src/misc/utilities.h"

/* #region IndexList */

template <typename T>
class IndexList
{
protected:
	std::map<int, T> map_;

protected:
	// 将记录值转为double。对于不可转换的值，处理为fallback。
	static double toDouble(T& val, double fallback);

public:
	using Component = typename std::pair<int, T>;
	using Iterator = typename std::map<int, T>::iterator;
	using ConstIterator = typename std::map<const int, T>::const_iterator;
	using IsIndexList = void;

public:
	inline IndexList();

	/// 插入单个元素
	inline void insert(int key, const T& item);
	/// 插入单个元素
	inline void insert(int key, T&& item);
	/// 插入单个元素
	inline void insert(const std::pair<const int, T>& item);
	/// 插入单个元素
	inline void insert(std::pair<const int, T>&& item);
	/// 插入一整个map
	inline void insert(const std::map<int, T>& map);
	/// 插入一整个IndexList
	inline void insert(const IndexList& index_list);

	/// 删除单个元素
	inline void erase(int key);
	/// 删除范围内的所有元素
	inline void erase(int start, int end);
	/// 清除所有内容
	inline void clear();

	/// 获取长度
	inline size_t size() const;
	/// 查询是否有某个key
	inline bool hasKey(int key) const;
	/// 查询值
	inline T& getVal(int key);
	inline T& operator[](int key);
	/// 改变某个元素的key, 同时让iter指向改变key的同一个元素
	inline void changeKey(Iterator& iter, int new_key);
	/// 改变某个元素的key
	inline void changeKey(const Iterator& iter, int new_key);

	/// 获取指定key的元素迭代器
	inline Iterator find(int key);
	/// 获取指定时间之前最近的元素（包括time）
	inline Iterator prevItem(int time);
	/// 获取指定时间之前最近的元素（包括time）
	inline ConstIterator prevItem(int time) const;
	/// 获取指定时间之后最近的元素（不包括time）
	inline Iterator nextItem(int time);
	/// 获取指定时间之后最近的元素（不包括time）
	inline ConstIterator nextItem(int time) const;

	/// 获取第一个元素
	inline std::pair<const int, T>& first();
	/// 获取第一个元素
	inline const std::pair<const int, T>& first() const;
	/// 获取最后一个元素
	inline std::pair<const int, T>& last();
	/// 获取最后一个元素
	inline const std::pair<const int, T>& last() const;

	inline Iterator begin();
	inline Iterator end();
	inline ConstIterator begin() const;
	inline ConstIterator end() const;

	/// 获取指定时间区间的子列表。会将指定时间之外最近的两个元素也包括在表中。
	IndexList<T> subList(int start, int end = -1) const;
	/// 整体偏移
	IndexList<T> offset(int offset);

	/// 线性插值获得当前位置的值。对于不可转为数字的关键点，视为fallback值。
	/// 默认的fallback值为NAN，也就意味着有关的插值结果均为NAN。
	/// 如果插值位置恰好是一个具有突变记录的点，返回的是突变记录突变后的值。
	double LinearInterpolate(int time, double fallback = NAN);

	/// 线性插值获得一组时间的值。这里的time_vec应为升序，并且时间间隔不要过长。
	/// 返回值遵循单点插值的逻辑。
	std::vector<double> LinearInterpolate(const std::vector<int>& time_vec, double fallback = NAN);

	/// 将列表内容输出到std::ostream
	friend std::ostream& operator<< (std::ostream& os, const IndexList<T>& lst) {
		for (typename IndexList<T>::ConstIterator iter = lst.map_.begin(); iter != lst.map_.end(); ++iter) {
			if (iter != lst.map_.begin()) {
				os << std::endl;
			}
			auto item = *iter;
			os << item.first << " : " << item.second;
		}

		return os;
	}

};

/* #endregion IndexList */

/* #region IndexList实现 */

template <typename T>
inline IndexList<T>::IndexList() {
	// 啥也不做
}

template <typename T>
inline void IndexList<T>::insert(int key, const T& item) {
	this->map_.insert_or_assign( key, item );
}

template <typename T>
inline void IndexList<T>::insert(int key, T&& item) {
	this->map_.insert_or_assign( key, std::forward<T>(item) );
}

template <typename T>
inline void IndexList<T>::insert(const std::pair<const int, T>& item) {
	this->insert(item.first, item.second);
}

template <typename T>
inline void IndexList<T>::insert(std::pair<const int, T>&& item) {
	this->insert(item.first, item.second);
}

template <typename T>
inline void IndexList<T>::insert(const std::map<int, T>& map) {
	this->map_.insert(map.begin(), map.end());
}

template <typename T>
inline void IndexList<T>::insert(const IndexList& index_list) {
	this->map_.insert(index_list.map_.begin(), index_list.map_.end());
}

template <typename T>
inline void IndexList<T>::erase(int key) {
	this->map_.erase(key);
}

template <typename T>
inline void IndexList<T>::erase(int start, int end) {
	this->map_.erase(this->map_.lower_bound(start), this->map_.lower_bound(end));
}

template <typename T>
inline void IndexList<T>::clear() {
	this->map_.clear();
}

template <typename T>
inline size_t IndexList<T>::size() const {
	return this->map_.size();
}

template <typename T>
inline bool IndexList<T>::hasKey(int key) const {
	return this->map_.find(key) != this->map_.end();
}

template <typename T>
inline T& IndexList<T>::operator[](int key) {
	return this->map_[key];
}

template <typename T>
inline T& IndexList<T>::getVal(int key) {
	return this->map_[key];
}

template <typename T>
inline void IndexList<T>::changeKey(IndexList::Iterator& iter, int new_key) {
	this->insert(new_key, iter->second);
	this->map_.erase(iter);
	iter = this->map_.find(new_key);
}

template <typename T>
inline void IndexList<T>::changeKey(const IndexList::Iterator& iter, int new_key) {
	this->insert(new_key, iter->second);
	this->map_.erase(iter);
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::find(int key) {
	return this->map_.find(key);
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::prevItem(int time) {
	auto before_iter = this->map_.upper_bound(time);
	if (before_iter == this->map_.begin()) {
		return this->map_.end();
	}
	else {
		--before_iter;
		return before_iter;
	}
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::prevItem(int time) const {
	auto before_iter = this->map_.upper_bound(time);
	if (before_iter == this->map_.begin()) {
		return this->map_.end();
	}
	else {
		--before_iter;
		return before_iter;
	}
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::nextItem(int time) {
	return this->map_.upper_bound(time);
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::nextItem(int time) const {
	return this->map_.upper_bound(time);
}

template <typename T>
inline std::pair<const int, T>& IndexList<T>::first() {
	return *(this->map_.begin());
}

template <typename T>
inline const std::pair<const int, T>& IndexList<T>::first() const {
	return *(this->map_.begin());
}

template <typename T>
inline std::pair<const int, T>& IndexList<T>::last() {
	return *(--this->map_.end());
}

template <typename T>
inline const std::pair<const int, T>& IndexList<T>::last() const {
	return *(--this->map_.end());
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::begin() {
	return this->map_.begin();
}

template <typename T>
inline typename IndexList<T>::Iterator IndexList<T>::end() {
	return this->map_.end();
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::begin() const {
	return this->map_.begin();
}

template <typename T>
inline typename IndexList<T>::ConstIterator IndexList<T>::end() const {
	return this->map_.end();
}


template <typename T>
IndexList<T> IndexList<T>::subList(int start, int end) const {
	if (end == -1) {
		end = this->last().first;
	}

	IndexList<T> output;

	IndexList<T>::ConstIterator start_iter = this->prevItem(start - 1);
	if (start_iter == this->map_.end()) {
		start_iter = this->nextItem(start - 1);
	}
	/*
	else {
		output.insert(start, start_iter->second);
		++start_iter;
	}
	*/

	IndexList<T>::ConstIterator end_iter = this->nextItem(end);
	for (IndexList<T>::ConstIterator iter = start_iter; iter != end_iter; ++iter) {
		output.insert(*iter);
	}
	if (end_iter != this->map_.end()) {
		output.insert(*end_iter);
	}

	return output;
}

template <typename T>
IndexList<T> IndexList<T>::offset(int offset) {
	IndexList<T> output;
	for (auto item : this->map_) {
		output.insert(item.first + offset, item.second);
	}

	return output;
}

template <typename T>
double IndexList<T>::toDouble(T& val, double fallback) {
	if constexpr (std::is_same_v<T, std::string>) {
		if (IsFloat(val)) {
			return stod(val);
		}
		else {
			return fallback;
		}
	}
	else if constexpr (std::is_arithmetic_v<T>) {
		return static_cast<double>(val);
	}
	else {
		return fallback;
	}

}

template <typename T>
double IndexList<T>::LinearInterpolate(int time, double fallback) {
	auto before_iter = this->map_.upper_bound(time);
	// 时间早于第一条记录的情况
	if (before_iter == this->map_.begin()) {
		auto before = *(before_iter);
		return toDouble(before.second);
	}
	// else
	auto before = *(--before_iter);

	auto after_iter = this->map_.upper_bound(time);
	// 时间晚于最后一条记录的情况
	if (after_iter == this->map_.end()) {
		auto after = *(--after_iter);
		return toDouble(after.second);
	}
	// else
	auto after = *(after_iter);

	// before.second 是 std::pair<T,T>
	double start = toDouble(before.second, fallback);
	double end = toDouble(after.second, fallback);

	if (std::isnan(start) || std::isnan(end)) {
		return NAN;
	}
	else {
		return LinearInterpolation(before.first, start, after.first, end, time);
	}
}

template <typename T>
std::vector<double> IndexList<T>::LinearInterpolate(const std::vector<int>& time_vec, double fallback) {
	// 空输入特殊处理：空输出
	if (time_vec.size() == 0) {
		return std::vector<double>();
	}
	// else

	int first_time_point = time_vec[0];
	// 是否在第一个记录之前？
	bool time_is_before_first_index = false;
	// 是否在最后一个记录之后？
	bool time_is_after_last_index = false;

	// 设置两个迭代器的初始位置，并更新两个flag
	auto before_iter = this->map_.upper_bound(first_time_point);
	if (before_iter == this->map_.begin()) {
		time_is_before_first_index = true;
		// 注意：由于map的迭代器不能先移到合法位置以外的部分，所以首次移动迭代器时before不要动。
	}
	else {
		--before_iter;
	}
	auto after_iter = this->map_.upper_bound(first_time_point);
	if (after_iter == this->map_.end()) {
		time_is_after_last_index = true;
	}

	// 创建返回值列表
	const int kSize = time_vec.size();
	std::vector<double> output(kSize);

	// 逐个插值
	for (int i = 0; i < kSize; ++i) {
		int time = time_vec[i];
		// 将两迭代器向前递进
		while (!time_is_after_last_index && after_iter->first <= time) {
			if (!time_is_before_first_index) {
				++before_iter;
			}
			else {
				time_is_before_first_index = false;
			}
			++after_iter;

			if (after_iter == this->map_.end()) {
				time_is_after_last_index = true;
			}
		}

		// 插值
		if (time_is_before_first_index) {
			output[i] = toDouble(after_iter->second, fallback);
		}
		else if (time_is_after_last_index) {
			output[i] = toDouble(before_iter->second, fallback);
		}
		else {
			double start = toDouble(before_iter->second, fallback);
			double end = toDouble(after_iter->second, fallback);

			if (std::isnan(start) || std::isnan(end)) {
				output[i] = NAN;
			}
			else {
				output[i] = LinearInterpolation(before_iter->first, start, after_iter->first, end, time);
			}
		}

	}

	return output;
}

/* #endregion IndexList实现 */

/* #region PairedIndexList */

template <typename T>
class PairedIndexList
{
protected:
	std::map<int, std::pair<T, T>> map_;

protected:
	static double toDouble(T& val, double fallback);

public:
	using Component = typename std::pair<int, std::pair<T, T>>;
	using Iterator = typename std::map<int, std::pair<T, T>>::iterator;
	using ConstIterator = typename std::map<const int, std::pair<T, T>>::const_iterator;
	using IsIndexList = void;
public:
	inline PairedIndexList();

	/// 插入单个元素
	inline void insert(int key, const T& item);
	/// 插入单个元素
	inline void insert(int key, T&& item);
	/// 插入具有突变的单个元素
	inline void insert(int key, const T& before, const T& after);
	/// 插入具有突变的单个元素
	inline void insert(int key, T&& before, T&& after);
	/// 插入单个元素
	inline void insert(const std::pair<const int, std::pair<T, T>>& item);
	/// 插入单个元素
	inline void insert(std::pair<const int, std::pair<T, T>>&& item);

	/// 插入一整个map
	inline void insert(const std::map<int, std::pair<T, T>>& map);
	/// 插入一整个IndexList
	inline void insert(const PairedIndexList& index_list);
	
	/// 删除单个元素
	inline void erase(int key);
	/// 删除范围内的元素
	inline void erase(int start, int end);
	/// 删除所有内容
	inline void clear();

	inline std::pair<T, T>& operator[](int key);

	/// 获取长度
	inline size_t size() const;
	/// 查询是否有某个key
	inline bool hasKey(int key) const;
	/// 获取起始值
	inline T& startVal(int key);
	/// 获取结束值
	inline T& endVal(int key);
	/// 查询某个记录的起始值是否等于结束值
	inline bool valIsUniform(int key);
	/// 改变某个元素的key, 同时让iter指向改变key的同一个元素
	inline void changeKey(Iterator& iter, int new_key);
	/// 改变某个元素的key
	inline void changeKey(const Iterator& iter, int new_key);

	/// 获取指定key的元素
	inline Iterator find(int key);
	/// 获取指定时间之前最近的元素（包括自身）
	inline Iterator prevItem(int time);
	/// 获取指定时间之前最近的元素（包括自身）
	inline ConstIterator prevItem(int time) const;
	/// 获取指定时间之后最近的元素（不包括自身）
	inline Iterator nextItem(int time);
	/// 获取指定时间之后最近的元素（不包括自身）
	inline ConstIterator nextItem(int time) const;

	/// 获取第一个元素
	inline std::pair<const int, std::pair<T, T>>& first();
	/// 获取第一个元素
	inline const std::pair<const int, std::pair<T, T>>& first() const;
	/// 获取最后一个元素
	inline std::pair<const int, std::pair<T, T>>& last();
	/// 获取最后一个元素
	inline const std::pair<const int, std::pair<T, T>>& last() const;

	inline Iterator begin();
	inline Iterator end();
	inline ConstIterator begin() const;
	inline ConstIterator end() const;

	/// 获取指定时间区间的子列表。会将指定时间之外最近的两个元素也包括在表中。
	PairedIndexList<T> subList(int start, int end = -1) const;
	/// 整体偏移
	PairedIndexList<T> offset(int offset);

	/// 线性插值获得当前位置的值。对于不可计算的类型，返回NAN
	/// 如果插值位置恰好是一个具有突变记录的点，返回的是突变记录突变后的值。
	double LinearInterpolate(int time, double fallback = NAN);

	/// 线性插值获得一组时间的值。这里的time_vec应为升序，并且时间间隔不要过长。
	/// 返回值遵循单点插值的逻辑。
	std::vector<double> LinearInterpolate(const std::vector<int>& time_vec, double fallback = NAN);

	/// 将列表内容输出到std::ostream
	friend std::ostream& operator<< (std::ostream& os, const PairedIndexList<T>& lst) {
		for (typename PairedIndexList<T>::ConstIterator iter = lst.map_.begin(); iter != lst.map_.end(); ++iter) {
			if (iter != lst.map_.begin()) {
				os << std::endl;
			}
			auto item = *iter;
			if (item.second.first == item.second.second) {
				os << item.first << " : " << item.second.first;
			}
			else {
				os << item.first << " : " << item.second.first << " , " << item.second.second;
			}
		}

		return os;
	}
};

/* #endregion */

/* #region PairedIndexList实现 */

template <typename T>
inline PairedIndexList<T>::PairedIndexList() {
	// 啥也不做
}

template <typename T>
inline void PairedIndexList<T>::insert(int key, const T& item) {
	this->map_.insert_or_assign(key, std::pair<T, T>(item, item));
}

template <typename T>
inline void PairedIndexList<T>::insert(int key, T&& item) {
	this->map_.insert_or_assign(key, std::pair<T, T>(item, std::forward<T>(item)));
}

template <typename T>
inline void PairedIndexList<T>::insert(int key, const T& before, const T& after) {
	this->map_.insert_or_assign(key, std::pair<T, T>(before, after));
}

template <typename T>
inline void PairedIndexList<T>::insert(int key, T&& before, T&& after) {
	this->map_.insert_or_assign(key, std::pair<T, T>(std::forward<T>(before), std::forward<T>(after)));
}

template <typename T>
inline void PairedIndexList<T>::insert(const std::pair<const int, std::pair<T, T>>& item) {
	this->insert(item.first, item.second.first, item.second.second);
}

template <typename T>
inline void PairedIndexList<T>::insert(std::pair<const int, std::pair<T, T>>&& item) {
	this->insert(item.first, item.second.first, item.second.second);
}

template <typename T>
inline void PairedIndexList<T>::insert(const std::map<int, std::pair<T,T>>& map) {
	this->map_.insert(map.begin(), map.end());
}

template <typename T>
inline void PairedIndexList<T>::insert(const PairedIndexList& index_list) {
	this->map_.insert(index_list.map_.begin(), index_list.map_.end());
}

template <typename T>
inline void PairedIndexList<T>::erase(int key) {
	this->map_.erase(key);
}

template <typename T>
inline void PairedIndexList<T>::erase(int start, int end) {
	this->map_.erase(this->map_.lower_bound(start), this->map_.lower_bound(end));
}

template <typename T>
inline void PairedIndexList<T>::clear() {
	this->map_.clear();
}

template <typename T>
inline std::pair<T, T>& PairedIndexList<T>::operator[](int key) {
	return this->map_[key];
}

template <typename T>
inline size_t PairedIndexList<T>::size() const {
	return this->map_.size();
}

template <typename T>
inline bool PairedIndexList<T>::hasKey(int key) const {
	return this->map_.find(key) != this->map_.end();
}

template <typename T>
inline T& PairedIndexList<T>::startVal(int key) {
	return this->map_[key].first;
}

template <typename T>
inline T& PairedIndexList<T>::endVal(int key) {
	return this->map_[key].second;
}

template <typename T>
inline bool PairedIndexList<T>::valIsUniform(int key) {
	return this->startVal(key) == this->endVal(key);
}

template <typename T>
inline void PairedIndexList<T>::changeKey(PairedIndexList::Iterator& iter, int new_key) {
	this->insert(new_key, iter->second.first, iter->second.second);
	this->map_.erase(iter);
	iter = this->map_.find(new_key);
}

template <typename T>
inline void PairedIndexList<T>::changeKey(const PairedIndexList::Iterator& iter, int new_key) {
	this->insert(new_key, iter->second.first, iter->second.second);
	this->map_.erase(iter);
}

template <typename T>
inline typename PairedIndexList<T>::Iterator PairedIndexList<T>::find(int key) {
	return this->map_.find(key);
}

template <typename T>
inline typename PairedIndexList<T>::Iterator PairedIndexList<T>::prevItem(int time) {
	auto before_iter = this->map_.upper_bound(time);
	if (before_iter == this->map_.begin()) {
		return this->map_.end();
	}
	else {
		--before_iter;
		return before_iter;
	}
}

template <typename T>
inline typename PairedIndexList<T>::ConstIterator PairedIndexList<T>::prevItem(int time) const {
	auto before_iter = this->map_.upper_bound(time);
	if (before_iter == this->map_.begin()) {
		return this->map_.end();
	}
	else {
		--before_iter;
		return before_iter;
	}
}

template <typename T>
inline typename PairedIndexList<T>::Iterator PairedIndexList<T>::nextItem(int time) {
	return this->map_.upper_bound(time);
}

template <typename T>
inline typename PairedIndexList<T>::ConstIterator PairedIndexList<T>::nextItem(int time) const {
	return this->map_.upper_bound(time);
}

template <typename T>
inline std::pair<const int, std::pair<T, T>>& PairedIndexList<T>::first() {
	return *(this->map_.begin());
}

template <typename T>
inline const std::pair<const int, std::pair<T, T>>& PairedIndexList<T>::first() const {
	return *(this->map_.begin());
}

template <typename T>
inline std::pair<const int, std::pair<T, T>>& PairedIndexList<T>::last() {
	return *(--this->map_.end());
}

template <typename T>
inline const std::pair<const int, std::pair<T, T>>& PairedIndexList<T>::last() const {
	return *(--this->map_.end());
}

template <typename T>
inline typename PairedIndexList<T>::Iterator PairedIndexList<T>::begin() {
	return this->map_.begin();
}

template <typename T>
inline typename PairedIndexList<T>::Iterator PairedIndexList<T>::end() {
	return this->map_.end();
}

template <typename T>
inline typename PairedIndexList<T>::ConstIterator PairedIndexList<T>::begin() const {
	return this->map_.begin();
}

template <typename T>
inline typename PairedIndexList<T>::ConstIterator PairedIndexList<T>::end() const {
	return this->map_.end();
}

template <typename T>
PairedIndexList<T> PairedIndexList<T>::subList(int start, int end) const {
	if (end == -1) {
		end = this->last().first;
	}

	PairedIndexList<T> output;

	PairedIndexList<T>::ConstIterator start_iter = this->prevItem(start - 1);
	if (start_iter == this->map_.end()) {
		start_iter = this->nextItem(start - 1);
	}
	/*
	else {
		output.insert(start, start_iter->second.first, start_iter->second.second);
		++start_iter;
	}
	*/

	PairedIndexList<T>::ConstIterator end_iter = this->nextItem(end);
	for (PairedIndexList<T>::ConstIterator iter = start_iter; iter != end_iter; ++iter) {
		output.insert(*iter);
	}
	if (end_iter != this->map_.end()) {
		output.insert(*end_iter);
	}

	return output;
}

template <typename T>
PairedIndexList<T> PairedIndexList<T>::offset(int offset) {
	PairedIndexList<T> output;
	for (auto& item : this->map_) {
		output.insert(item.first + offset, item.second.first, item.second.second);
	}

	return output;
}

template <typename T>
double PairedIndexList<T>::toDouble(T& val, double fallback) {
	if constexpr (std::is_same_v<T, std::string>) {
		if (IsFloat(val)) {
			return stod(val);
		}
		else {
			return fallback;
		}
	}
	else if constexpr (std::is_arithmetic_v<T>) {
		return static_cast<double>(val);
	}
	else {
		return fallback;
	}
		
}

template <typename T>
double PairedIndexList<T>::LinearInterpolate(int time, double fallback) {
	auto before_iter = this->map_.upper_bound(time);
	// 时间早于第一条记录的情况
	if (before_iter == this->map_.begin()) {
		auto before = *(before_iter);
		return toDouble(before.second.first);
	}
	// else
	auto before = *(--before_iter);

	auto after_iter = this->map_.upper_bound(time);
	// 时间晚于最后一条记录的情况
	if (after_iter == this->map_.end()) {
		auto after = *(--after_iter);
		return toDouble(after.second.second);
	}
	// else
	auto after = *(after_iter);

	// before.second 是 std::pair<T,T>
	double start = toDouble(before.second.second, fallback);
	double end = toDouble(after.second.first, fallback);
	
	if (std::isnan(start) || std::isnan(end)) {
		return NAN;
	}
	else {
		return LinearInterpolation(before.first, start, after.first, end, time);
	}	
}

template <typename T>
std::vector<double> PairedIndexList<T>::LinearInterpolate(const std::vector<int>& time_vec, double fallback) {
	// 空输入特殊处理：空输出
	if (time_vec.size() == 0) {
		return std::vector<double>();
	}
	// 自身为空，插值表返回全0
	if (this->size() == 0) {
		std::vector<double> zero(time_vec.size());
		std::fill(zero.begin(), zero.end(), 0.0);
		return zero;
	}
	// else

	int first_time_point = time_vec[0];
	// 是否在第一个记录之前？
	bool time_is_before_first_index = false;
	// 是否在最后一个记录之后？
	bool time_is_after_last_index = false;

	// 设置两个迭代器的初始位置，并更新两个flag
	auto before_iter = this->map_.upper_bound(first_time_point);
	if (before_iter == this->map_.begin()) {
		time_is_before_first_index = true;
		// 注意：由于map的迭代器不能先移到合法位置以外的部分，所以首次移动迭代器时before不要动。
	}
	else {
		--before_iter;
	}
	auto after_iter = this->map_.upper_bound(first_time_point);
	if (after_iter == this->map_.end()) {
		time_is_after_last_index = true;
	}

	// 创建返回值列表
	const int kSize = static_cast<int>(time_vec.size());
	std::vector<double> output(kSize);

	// 逐个插值
	for (int i = 0; i < kSize; ++i) {
		int time = time_vec[i];
		// 将两迭代器向前递进
		while (!time_is_after_last_index && after_iter->first <= time) {
			if (!time_is_before_first_index) {
				++before_iter;
			}
			else {
				time_is_before_first_index = false;
			}
			++after_iter;

			if (after_iter == this->map_.end()) {
				time_is_after_last_index = true;
			}
		}

		// 插值
		if (time_is_before_first_index) {
			output[i] = toDouble(after_iter->second.first, fallback);
		}
		else if (time_is_after_last_index) {
			output[i] = toDouble(before_iter->second.second, fallback);
		}
		else {
			double start = toDouble(before_iter->second.second, fallback);
			double end = toDouble(after_iter->second.first, fallback);

			if (std::isnan(start) || std::isnan(end)) {
				output[i] =  NAN;
			}
			else {
				output[i] =  LinearInterpolation(before_iter->first, start, after_iter->first, end, time);
			}	
		}
		
	}

	return output;
}

/* #endregion */
