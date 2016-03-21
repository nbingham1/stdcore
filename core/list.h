/*
 * list.h
 *
 *  Created on: Feb 4, 2014
 *      Author: nbingham
 */

#pragma once

#include "slice.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef NULL
#define NULL 0
#endif

namespace core
{

template <class value_type>
struct list
{
	struct node
	{
		node()
		{
			next = this;
			prev = this;
		}
	
		virtual ~node()
		{
		}
	
		node *next;
		node *prev;
	};

	struct item : node
	{
		item()
		{
		}

		item(const value_type &d)
		{
			data = d;
		}

		~item()
		{
		}

		value_type data;
	};

	list()
	{
		left.next = &right;
		right.prev = &left;
		count = 0;
	}

	template <class container>
	list(const container &c)
	{
		left.next = &right;
		right.prev = &left;
		count = 0;
		for (typename container::const_iterator i = c.begin(); i != c.end(); i++)
			end().push(*i);
	}

	list(const list<value_type> &c)
	{
		left.next = &right;
		right.prev = &left;
		count = 0;
		for (const_iterator i = c.begin(); i != c.end(); i++)
			end().push(*i);
	}

	list(int n, const value_type &t)
	{
		left.next = &right;
		right.prev = &left;
		count = 0;
		end().push(n, t);
	}

	~list()
	{
		clear();
	}

	node left;
	node right;
	int count;

	struct iterator;
	struct const_iterator;

	struct iterator
	{
	protected:
		friend class list<value_type>;
		friend class const_iterator;
		list<value_type> *lst;
		node *loc;
	public:
		typedef list<value_type> container;
		typedef value_type type;

		iterator()
		{
			lst = NULL;
			loc = NULL;
		}

		iterator(list<value_type> *l, node *n)
		{
			lst = l;
			loc = n;
		}

		iterator(list<value_type> *l, int n)
		{
			lst = l;
			if (n > 0)
			{
				loc = lst->left.next;
				while (n > 0)
				{
					loc = loc->next;
					n--;
				}
			}
			else
			{
				loc = &lst->right;
				while (n < 0)
				{
					loc = loc->prev;
					n++;
				}
			}
		}

		iterator(const iterator &i)
		{
			lst = i.lst;
			loc = i.loc;
		}

		~iterator() {}

		value_type &operator*()
		{
			return ((item*)loc)->data;
		}
		value_type *operator->()
		{
			return &((item*)loc)->data;
		}

		value_type *pointer()
		{
			return &((item*)loc)->data;
		}

		value_type &value()
		{
			return ((item*)loc)->data;
		}

		iterator &operator++(int)
		{
			loc = loc->next;
			return *this;
		}

		iterator &operator--(int)
		{
			loc = loc->prev;
			return *this;
		}

		iterator &operator++()
		{
			loc = loc->next;
			return *this;
		}

		iterator &operator--()
		{
			loc = loc->prev;
			return *this;
		}

		iterator &operator+=(int n)
		{
			for (int i = 0; i < n && loc != &lst->right; i++)
				loc = loc->next;

			return *this;
		}

		iterator &operator-=(int n)
		{
			for (int i = 0; i < n && loc != &lst->left; i++)
				loc = loc->prev;

			return *this;
		}

		iterator operator+(int n) const
		{
			iterator result(*this);
			result += n;
			return result;
		}

		iterator operator-(int n) const
		{
			iterator result(*this);
			result -= n;
			return result;
		}

		iterator &operator=(iterator i)
		{
			lst = i.lst;
			loc = i.loc;
			return *this;
		}

		bool operator==(iterator i) const
		{
			return loc == i.loc;
		}

		bool operator!=(iterator i) const
		{
			return loc != i.loc;
		}

		int operator-(iterator i) const
		{
			int count = 0;
			while (i.loc != loc && i.loc != &i.lst->right)
			{
				count++;
				i.loc = i.loc->next;
			}

			while (i.loc != loc && i.loc != &i.lst->left)
			{
				count--;
				i.loc = i.loc->prev;
			}

			return count;
		}

		bool operator==(const_iterator i) const
		{
			return loc == i.loc;
		}

		bool operator!=(const_iterator i) const
		{
			return loc != i.loc;
		}

		int operator-(const_iterator i) const
		{
			int count = 0;
			while (i.loc != loc && i.loc != &i.lst->right)
			{
				count++;
				i.loc = i.loc->next;
			}

			while (i.loc != loc && i.loc != &i.lst->left)
			{
				count--;
				i.loc = i.loc->prev;
			}

			return count;
		}

		template <class iterator_type>
		void swap(iterator_type i)
		{
			value_type temp;
			memcpy(&temp, &((item*)loc)->data, sizeof(value_type));
			memcpy(&((item*)loc)->data, &((item*)i.loc)->data, sizeof(value_type));
			memcpy(&((item*)i.loc)->data, &temp, sizeof(value_type));
		}

		list<value_type> pop(int n = 1)
		{
			list<value_type> result;
			node *start = loc;
			for (int i = 0; i < n && loc != &lst->right; i++)
				loc = loc->next;
			for (int i = 0; i > n && start != &lst->left && start != lst->left.next; i--)
				start = start->prev;

			if (start != loc)
			{
				result.left.next = start;
				result.right.prev = loc->prev;
				start->prev->next = loc;
				loc->prev = start->prev;
				result.left.next->prev = &result.left;
				result.right.prev->next = &result.right;
				if (n > 0)
					lst->count -= n;
				else
					lst->count += n;
			}

			return result;
		}

		void drop(int n = 1)
		{
			if (n > 0)
			{
				node* start = loc->prev;
				
				for (int i = 0; i < n && loc != &lst->right; i++)
				{
					node *temp = loc->next;
					delete loc;
					loc = temp;
				}
					
				start->next = loc;
				loc->prev = start;
				lst->count -= n;
			}
			else if (n < 0)
			{
				node *start = loc->prev;
				
				for (int i = 0; i > n && start != &lst->left; i--)
				{
					node *temp = start->prev;
					delete start;
					start = temp;
				}
				
				start->next = loc;
				loc->prev = start;
				lst->count += n;
			}
		}

		void push(value_type v)
		{
			node *start = loc->prev;
			start->next = new item(v);
			start->next->prev = start;
			start = start->next;
			start->next = loc;
			loc->prev = start;
			lst->count++;
		}

		void push(int n, value_type v)
		{
			node *start = loc->prev;
			for (int i = 0; i < n; i++)
			{
				start->next = new item(v);
				start->next->prev = start;
				start = start->next;
			}

			start->next = loc;
			loc->prev = start;
			lst->count += n;
		}

		template <class container>
		void push(const container &c)
		{
			node *start = loc->prev;
			for (typename container::const_iterator i = c.begin(); i != c.end(); i++)
			{
				start->next = new item(*i);
				start->next->prev = start;
				start = start->next;
			}

			start->next = loc;
			loc->prev = start;
			lst->count += c.size();
		}

		slice<iterator> sub(int n = -1)
		{
			iterator right;
			if (n < 0)
				right = lst->end()+n;
			else
				right = *this+n-1;
			return slice<iterator>(*this, right);
		}

		void replace(int n, int m, value_type v)
		{
			int j = 0;
			iterator i = *this;
			while (i != lst->end() && j < n && j < m)
			{
				*i = v;
				i++;
				j++;
			}

			if (i == lst->end() && j < m)
				i.push(m-j, v);
			else if (j < m)
				(i+1).push(m-j, v);
			else if (j < n)
				i.pop(n-j);
		}
		
		template <class container>
		void replace(int n, const container &c)
		{
			typename container::const_iterator j = c.begin();
			int k = 0;
			iterator i = *this;
			while (i != lst->end() && k < n && j != c.end())
			{
				*i = *j;
				i++;
				j++;
				k++;
			}

			if (i == lst->end() && j != c.end())
				i.push(j.sub());
			else if (j != c.end())
				(i+1).push(j.sub());	
			else if (k < n)
				i.pop(n-k);
		}

	};

	struct const_iterator
	{
	protected:
		friend class list<value_type>;
		friend class iterator;
		const list<value_type> *lst;
		const node *loc;
	public:
		typedef const list<value_type> container;
		typedef value_type type;

		const_iterator()
		{
			lst = NULL;
			loc = NULL;
		}

		const_iterator(const list *l)
		{
			lst = l;
			loc = &l->left;
		}

		const_iterator(const list *l, const node *n)
		{
			lst = l;
			loc = n;
		}

		const_iterator(list<value_type> *l, int n)
		{
			lst = l;
			if (n > 0)
			{
				loc = lst->left.next;
				while (n > 0)
				{
					loc = loc->next;
					n--;
				}
			}
			else
			{
				loc = &lst->right;
				while (n < 0)
				{
					loc = loc->prev;
					n++;
				}
			}
		}


		const_iterator(const const_iterator &i)
		{
			lst = i.lst;
			loc = i.loc;
		}

		~const_iterator() {}

		const value_type &operator*()
		{
			return ((const item*)loc)->data;
		}
		const value_type *operator->()
		{
			return &((const item*)loc)->data;
		}

		const_iterator &operator++(int)
		{
			loc = loc->next;
			return *this;
		}

		const_iterator &operator--(int)
		{
			loc = loc->prev;
			return *this;
		}

		const_iterator &operator++()
		{
			loc = loc->next;
			return *this;
		}

		const_iterator &operator--()
		{
			loc = loc->prev;
			return *this;
		}

		const_iterator &operator+=(int n)
		{
			for (int i = 0; i < n; i++)
				(*this)++;

			return *this;
		}

		const_iterator &operator-=(int n)
		{
			for (int i = 0; i < n; i++)
				(*this)--;

			return *this;
		}

		const_iterator operator+(int n) const
		{
			const_iterator result(*this);
			for (int i = 0; i < n; i++)
				result++;

			return result;
		}

		const_iterator operator-(int n) const
		{
			const_iterator result(*this);
			for (int i = 0; i < n; i++)
				result--;

			return result;
		}

		const_iterator &operator=(const_iterator i)
		{
			lst = i.lst;
			loc = i.loc;
			return *this;
		}

		bool operator==(const_iterator i) const
		{
			return lst == i.lst && loc == i.loc;
		}

		bool operator!=(const_iterator i) const
		{
			return lst != i.lst || loc != i.loc;
		}

		int operator-(const_iterator i) const
		{
			int count = 0;
			for (const_iterator j = i; j.loc != loc && j.loc != &j.lst->right; j++)
				count++;
			return count;
		}

		const_iterator &operator=(iterator i)
		{
			lst = i.lst;
			loc = i.loc;
			return *this;
		}

		bool operator==(iterator i) const
		{
			return lst == i.lst && loc == i.loc;
		}

		bool operator!=(iterator i) const
		{
			return lst != i.lst || loc != i.loc;
		}

		int operator-(iterator i) const
		{
			int count = 0;
			for (const_iterator j = i; j.loc != loc && j.loc != &j.lst->right; j++)
				count++;
			return count;
		}

		slice<const_iterator> sub(int n = -1)
		{
			const_iterator right;
			if (n < 0)
				right = lst->end()+n;
			else
				right = *this+n-1;
			return slice<const_iterator>(*this, right);
		}
	};

	int size() const
	{
		return count;
	}

	iterator at(int i)
	{
		return begin() + i;
	}

	const_iterator at(int i) const
	{
		return begin() + i;
	}

	value_type &get(int i) const
	{
		return ((item*)(begin() + i))->data;
	}

	value_type &operator[](int i) const
	{
		return ((item*)(begin() + i))->data;
	}

	value_type &front() const
	{
		return *begin();
	}

	value_type &back() const
	{
		return *rbegin();
	}

	iterator begin()
	{
		return iterator(this, left.next);
	}

	iterator end()
	{
		return iterator(this, &right);
	}

	iterator rbegin()
	{
		return iterator(this, right.prev);
	}

	iterator rend()
	{
		return iterator(this, &left);
	}

	const_iterator begin() const
	{
		return const_iterator(this, left.next);
	}

	const_iterator end() const
	{
		return const_iterator(this, &right);
	}

	const_iterator rbegin() const
	{
		return const_iterator(this, right.prev);
	}

	const_iterator rend() const
	{
		return const_iterator(this, &left);
	}

	slice<iterator> sub(int left, int right = -1)
	{
		return slice<iterator>(iterator(this, left), iterator(this, right));
	}

	slice<const_iterator> sub(int left, int right = -1) const
	{
		return slice<const_iterator>(const_iterator(this, left), const_iterator(this, right));
	}

	void push_back(const value_type &t)
	{
		end().push(t);
	}
	
	void push_front(const value_type &t)
	{
		begin().push(t);
	}

	void push_back(int n, const value_type &t)
	{
		end().push(n, t);
	}
	
	void push_front(int n, const value_type &t)
	{
		begin().push(n, t);
	}


	template <class container>
	void push_back(container &c)
	{
		end().push(c);
	}

	template <class container>
	void push_front(container &c)
	{
		begin().push(c);
	}

	list<value_type> pop_back(unsigned int n = 1)
	{
		return end().pop(-n);
	}

	list<value_type> pop_front(unsigned int n = 1)
	{
		return begin().pop(n);
	}

	void drop_front(unsigned int n = 1)
	{
		begin().drop(n);
	}

	void drop_back(unsigned int n = 1)
	{
		end().drop(-n);
	}

	void clear()
	{
		node *curr = left.next, *prev;
		while (curr != &right)
		{
			prev = curr;
			curr = curr->next;
			delete prev;
		}
		left.next = &right;
		right.prev = &left;
		count = 0;
	}

	template <class container>
	list<value_type> &operator=(const container &c)
	{
		clear();
		for (typename container::const_iterator i = c.begin(); i != c.end(); i++)
			push_back(*i);
		return *this;
	}

	list<value_type> &operator=(const list<value_type> &c)
	{
		clear();
		for (const_iterator i = c.begin(); i != c.end(); i++)
			push_back(*i);
		return *this;
	}
};

}

