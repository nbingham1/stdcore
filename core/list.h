/*
 * list.h
 *
 *  Created on: Feb 4, 2014
 *      Author: nbingham
 */

#include "container.h"
#include "memory.h"

#ifndef list_h
#define list_h

namespace core
{

template <class value_type>
struct list
{
	list()
	{
		left.next = &right;
		right.prev = &left;
	}

	list(const value_type &t, int n = 1)
	{
		left.next = &right;
		right.prev = &left;
		for (int i = 0; i < n; i++)
			push_back(t);
	}

	struct node
	{
		node()  { next = this; prev = this; }
		virtual ~node() { }
		virtual bool has_data() { return false; }

		node *next;
		node *prev;
	};

	struct item : node
	{
		item()  { }
		item(const value_type &d) { data = d; }
		~item() { }
		bool has_data() { return true; }

		value_type data;
	};

	node left;
	node right;

	struct iterator;
	struct const_iterator;

	struct iterator
	{
		typedef value_type type;

		iterator()
		{
			lst = NULL;
			pos = NULL;
		}

		iterator(list *l)
		{
			lst = l;
			pos = &l->left;
		}

		iterator(list *l, node *n)
		{
			lst = l;
			pos = n;
		}

		iterator(const iterator &i)
		{
			lst = i.lst;
			pos = i.pos;
		}

		~iterator() {}

		list *lst;
		node *pos;

		value_type &operator*()
		{
			return ((item*)pos)->data;
		}
		value_type *operator->()
		{
			return &((item*)pos)->data;
		}

		iterator &operator++(int)
		{
			pos = pos->next;
			return *this;
		}

		iterator &operator--(int)
		{
			pos = pos->prev;
			return *this;
		}

		iterator &operator++()
		{
			pos = pos->next;
			return *this;
		}

		iterator &operator--()
		{
			pos = pos->prev;
			return *this;
		}

		iterator &operator+=(int n)
		{
			for (int i = 0; i < n; i++)
				(*this)++;

			return *this;
		}

		iterator &operator-=(int n)
		{
			for (int i = 0; i < n; i++)
				(*this)--;

			return *this;
		}

		iterator operator+(int n)
		{
			iterator result(*this);
			for (int i = 0; i < n; i++)
				result++;

			return result;
		}

		iterator operator-(int n)
		{
			iterator result(*this);
			for (int i = 0; i < n; i++)
				result--;

			return result;
		}

		iterator &operator=(iterator i)
		{
			lst = i.lst;
			pos = i.pos;
			return *this;
		}

		bool operator==(iterator i)
		{
			return lst == i.lst && pos == i.pos;
		}

		bool operator!=(iterator i)
		{
			return lst != i.lst || pos != i.pos;
		}

		int operator-(iterator i)
		{
			int count = 0;
			for (iterator j = i; j.pos != pos && j.pos != &j.lst->right; j++)
				count++;
			return count;
		}

		bool operator==(const_iterator i)
		{
			return lst == i.lst && pos == i.pos;
		}

		bool operator!=(const_iterator i)
		{
			return lst != i.lst || pos != i.pos;
		}

		int operator-(const_iterator i)
		{
			int count = 0;
			for (const_iterator j = i; j.pos != pos && j.pos != &j.lst->right; j++)
				count++;
			return count;
		}

		template <class iterator_type>
		void swap(iterator_type i)
		{
			value_type temp;
			memcpy(&temp, &((item*)pos)->data, sizeof(value_type));
			memcpy(&((item*)pos)->data, &*i, sizeof(value_type));
			memcpy(&*i, &temp, sizeof(value_type));
		}

		void push(value_type v)
		{
			node *result = new item(v);

			result->next = pos->next;
			result->prev = result->next->prev;
			result->prev->next = result;
			result->next->prev = result;
		}

		template <class container>
		void merge(container &c)
		{
			boundary<typename container::iterator> b = c.bound();
			for (typename container::iterator i = b.left; i != b.right+1; i++)
				push(*i);
		}

		void pop(int n = 1)
		{
			node *l = NULL;

			for (int i = 0; i < n && pos->has_data(); i++)
			{
				l = pos->next;

				pos->next->prev = pos->prev;
				pos->prev->next = pos->next;

				delete pos;
				pos = l;
			}
		}

		void pop(iterator last)
		{
			node *l = NULL;

			while (pos != last.pos && pos->has_data())
			{
				l = pos->next;

				pos->next->prev = pos->prev;
				pos->prev->next = pos->next;

				delete pos;
				pos = l;
			}
		}

		void rpush(value_type v)
		{
			node *result = new item(v);

			result->prev = pos->prev;
			result->next = result->prev->next;
			result->next->prev = result;
			result->prev->next = result;
		}

		template <class container>
		void rmerge(container &c)
		{
			boundary<typename container::iterator> b = c.bound();
			for (typename container::iterator i = b.left; i != b.right+1; i++)
				rpush(*i);
		}

		void rpop(int n = 1)
		{
			node *l = NULL;

			for (int i = 0; i < n && pos->has_data(); i++)
			{
				l = pos->prev;

				pos->next->prev = pos->prev;
				pos->prev->next = pos->next;

				delete pos;
				pos = l;
			}
		}

		void rpop(iterator last)
		{
			node *l = NULL;

			while (pos != last && pos->has_data())
			{
				l = pos->prev;

				pos->next->prev = pos->prev;
				pos->prev->next = pos->next;

				delete pos;
				pos = l;
			}
		}
	};

	struct const_iterator
	{
		typedef value_type type;

		const_iterator()
		{
			lst = NULL;
			pos = NULL;
		}

		const_iterator(const list *l)
		{
			lst = l;
			pos = &l->left;
		}

		const_iterator(const list *l, const node *n)
		{
			lst = l;
			pos = n;
		}

		const_iterator(const const_iterator &i)
		{
			lst = i.lst;
			pos = i.pos;
		}

		~const_iterator() {}

		const list *lst;
		const node *pos;

		const value_type &operator*()
		{
			return ((const item*)pos)->data;
		}
		const value_type *operator->()
		{
			return &((const item*)pos)->data;
		}

		const_iterator &operator++(int)
		{
			pos = pos->next;
			return *this;
		}

		const_iterator &operator--(int)
		{
			pos = pos->prev;
			return *this;
		}

		const_iterator &operator++()
		{
			pos = pos->next;
			return *this;
		}

		const_iterator &operator--()
		{
			pos = pos->prev;
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

		const_iterator operator+(int n)
		{
			const_iterator result(*this);
			for (int i = 0; i < n; i++)
				result++;

			return result;
		}

		const_iterator operator-(int n)
		{
			const_iterator result(*this);
			for (int i = 0; i < n; i++)
				result--;

			return result;
		}

		const_iterator &operator=(const_iterator i)
		{
			lst = i.lst;
			pos = i.pos;
			return *this;
		}

		bool operator==(const_iterator i)
		{
			return lst == i.lst && pos == i.pos;
		}

		bool operator!=(const_iterator i)
		{
			return lst != i.lst || pos != i.pos;
		}

		int operator-(const_iterator i)
		{
			int count = 0;
			for (const_iterator j = i; j.pos != pos && j.pos != &j.lst->right; j++)
				count++;
			return count;
		}

		const_iterator &operator=(iterator i)
		{
			lst = i.lst;
			pos = i.pos;
			return *this;
		}

		bool operator==(iterator i)
		{
			return lst == i.lst && pos == i.pos;
		}

		bool operator!=(iterator i)
		{
			return lst != i.lst || pos != i.pos;
		}

		int operator-(iterator i)
		{
			int count = 0;
			for (const_iterator j = i; j.pos != pos && j.pos != &j.lst->right; j++)
				count++;
			return count;
		}
	};

	int size()
	{
		return end() - begin();
	}

	value_type &at(int i)
	{
		return ((item*)(begin() + i))->data;
	}

	value_type &operator[](int i)
	{
		return ((item*)(begin() + i))->data;
	}

	value_type &front() const
	{
		return *left;
	}

	value_type &back() const
	{
		return *right;
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

	boundary<iterator> bound()
	{
		return boundary<iterator>(begin(), rbegin());
	}

	boundary<const_iterator> bound() const
	{
		return boundary<const_iterator>(begin(), rbegin());
	}

	void push_back(value_type t)
	{
		rbegin().push(t);
	}

	template <class container>
	void merge_back(container &c)
	{
		rbegin().merge(c);
	}

	void push_front(value_type t)
	{
		begin().rpush(t);
	}

	template <class container>
	void merge_front(container &c)
	{
		begin().rmerge(c);
	}

	void pop_back(int n = 1)
	{
		rbegin().rpop(n);
	}

	void pop_front(int n = 1)
	{
		begin().pop(n);
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
	}

	list<value_type> &operator=(list<value_type> a)
	{
		clear();
		push_back(a.bound());
		return *this;
	}
};

}

#endif