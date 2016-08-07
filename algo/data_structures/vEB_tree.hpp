#ifndef ALGO_DATA_STRUCTURES_VEB_TREE_HPP_
#define ALGO_DATA_STRUCTURES_VEB_TREE_HPP_

#include <array>
#include <memory>
#include <algorithm>

namespace detail
{
	template <typename T>
	constexpr T max(T a, T b) {
		return a > b ? a : b;
	}

	template <typename T>
	constexpr T cluster_size(T t)
	{
		return 2 << ((2 << max(0, t - 2)) - 1);
	}

	template <typename T>
	constexpr T digits(T t)
	{
		return 2u << (t - 1);
	}
}

struct Basic_vEB_tree
{
	typedef int num_type;
	static constexpr num_type none = - 1;
	static constexpr num_type high(num_type n, num_type digits)
	{
		return n >> (digits / 2);
	}
	static constexpr num_type low(num_type n, num_type digits)
	{
		num_type mask = (1 << digits / 2) - 1;
		return n & mask;
	}
	num_type max() const { return max_; }
	void max(num_type max) { max_ = max; }
	num_type min() const { return min_; }
	void min(num_type min) { min_ = min; }
private:
	num_type max_ = none;
	num_type min_ = none;
};

template <Basic_vEB_tree::num_type K> class vEB_tree;

template <Basic_vEB_tree::num_type K>
class vEB_tree : public Basic_vEB_tree
{
	static constexpr num_type cluster_size_ = detail::cluster_size(K);
	static constexpr num_type digits = detail::digits(K);
public:
	vEB_tree()
	{
		for (auto& i : clusters_)
		{
			i = std::make_unique<Cluster>();
		}
	}
	static constexpr num_type size = 2u << ((2u << (K - 1)) - 1);
	typedef vEB_tree<K - 1> Cluster;
	void insert(num_type value);
	num_type successor(num_type value) const;
	void remove(num_type value);
	bool contains(num_type value) const;
private:
	std::array<std::unique_ptr<Cluster>, cluster_size_> clusters_;
	std::unique_ptr<Cluster> summary_ = std::make_unique<Cluster>();
};

template <>
class vEB_tree<0> : public Basic_vEB_tree
{
	static constexpr num_type digits = 2;
public:
	static constexpr num_type size = 2;
	void insert(num_type value)
	{
		if (min() == none)
		{
			min(value);
			max(value);
		}
		else
		{
			if (value < min() )
			{
				min(value);
			}
			if (value > max())
			{
				max(value);
			}
		}
	}
	num_type successor(num_type value) const
	{
		if (value < min())
		{
			return min();
		}
		return max();
	}
	void remove(num_type value)
	{
		if (value == min())
		{
			min(none);
		}
		if (value == max())
		{
			max(none);
		}
	}
	bool contains(num_type value) const
	{
		return (min() == value || max() == value);
	}
};

template <Basic_vEB_tree::num_type K>
void vEB_tree<K>::insert(num_type value)
{
	if (min() == none)
	{
		min(value);
		max(value);
		return;
	}
	if (value < min())
	{	auto tmp = min();
		min(value);
		value = tmp;
	}
	if (value > max())
	{
		max(value);
	}
	const auto i = high(value, digits);
	if (clusters_[i]->min() == none)
	{
		summary_->insert(i);
	}
	clusters_[i]->insert(low(value, digits));
}

template <Basic_vEB_tree::num_type K>
typename vEB_tree<K>::num_type vEB_tree<K>::successor(num_type value) const
{
	if (value < min())
	{
		return min();
	}
	auto i = high(value, digits);
	num_type j;
	if (low(value, digits) < clusters_[i]->max())
	{
		j = clusters_[i]->successor(low(value, digits));
	}
	else
	{
		i = summary_->successor(i);
		j = clusters_[i]->min();
	}
	return i * cluster_size_ + j;
}

template <Basic_vEB_tree::num_type K>
void vEB_tree<K>::remove(num_type value)
{
	if (value == min())
	{
		const auto j = summary_->min();
		if (j == none)
		{
			min(none);
			max(none);
			return;
		}
		value = j * cluster_size_ + clusters_[j]->min();
		min(value);
	}
	const auto i = high(value, digits);
	clusters_[i]->remove(low(value, digits));
	if (clusters_[i]->min() == none)
	{
		summary_->remove(i);
	}
	if (value == max())
	{
		if (summary_->max() == none)
		{
			max(min());
		}
		else
		{
			const auto k = summary_->max();
			max(k * cluster_size_ + clusters_[k]->max());
		}
	}
}

template <Basic_vEB_tree::num_type K>
bool vEB_tree<K>::contains(num_type value) const
{
	if (min() == value || max() == value)
	{
		return true;
	}
	return clusters_[high(value, digits)]->contains(low(value, digits));
}

#endif
