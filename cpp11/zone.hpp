//
// MessagePack for C++ memory pool
//
// Copyright (C) 2008-2013 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_ZONE_HPP
#define MSGPACK_ZONE_HPP

#include <cstdlib>
#include <memory>
#include <vector>

#include "cpp_config.hpp"

#ifndef MSGPACK_ZONE_CHUNK_SIZE
#define MSGPACK_ZONE_CHUNK_SIZE 8192
#endif

#ifndef MSGPACK_ZONE_ALIGN
#define MSGPACK_ZONE_ALIGN sizeof(int)
#endif

namespace msgpack {

class zone {
private:
	struct finalizer {
		finalizer(void (*func)(void*), void* data):func_(func), data_(data) {}
		void operator()() { func_(data_); }
		void (*func_)(void*);
		void* data_;
	};
	struct finalizer_array {
		finalizer_array():tail_(nullptr), end_(nullptr), array_(nullptr) {}
		void call() {
			finalizer* fin = tail_;
			for(; fin != array_; --fin) (*(fin-1))();
		}
		~finalizer_array() {
			call();
			::free(array_);
		}
		void clear() {
			call();
			tail_ = array_;
		}
		void push(void (*func)(void* data), void* data)
		{
			finalizer* fin = tail_;

			if(fin == end_) {
				push_expand(func, data);
				return;
			}

			fin->func_ = func;
			fin->data_ = data;

			++tail_;
		}
		void push_expand(void (*func)(void*), void* data) {
			const size_t nused = end_ - array_;
			size_t nnext;
			if(nused == 0) {
				nnext = (sizeof(finalizer) < 72/2) ?
					72 / sizeof(finalizer) : 8;
			} else {
				nnext = nused * 2;
			}
			finalizer* tmp =
				static_cast<finalizer*>(::realloc(array_, sizeof(finalizer) * nnext));
			if(!tmp) {
				throw std::bad_alloc();
			}
			array_	= tmp;
			end_	= tmp + nnext;
			tail_	= tmp + nused;
			new (tail_) finalizer(func, data);

			++tail_;
		}
		finalizer* tail_;
		finalizer* end_;
		finalizer* array_;
	};
	struct chunk {
		chunk* next_;
	};
	struct chunk_list {
		chunk_list(size_t chunk_size)
		{
			chunk* c = static_cast<chunk*>(::malloc(sizeof(chunk) + chunk_size));
			if(!c) {
				throw std::bad_alloc();
			}

			head_ = c;
			free_ = chunk_size;
			ptr_  = reinterpret_cast<char*>(c) + sizeof(chunk);
			c->next_ = nullptr;
		}
		~chunk_list()
		{
			chunk* c = head_;
			while(true) {
				chunk* n = c->next_;
				::free(c);
				if(n) {
					c = n;
				} else {
					break;
				}
			}
		}
		void clear(size_t chunk_size)
		{
			chunk* c = head_;
			while(true) {
				chunk* n = c->next_;
				if(n) {
					::free(c);
					c = n;
				} else {
					break;
				}
			}
			head_->next_ = nullptr;
			free_ = chunk_size;
			ptr_  = reinterpret_cast<char*>(head_) + sizeof(chunk);
		}
		size_t free_;
		char* ptr_;
		chunk* head_;
	};
	size_t chunk_size_;
	chunk_list chunk_list_;
	finalizer_array finalizer_array_;

public:
	zone(size_t chunk_size = MSGPACK_ZONE_CHUNK_SIZE);

public:
	static zone* create(size_t chunk_size);
	static void destroy(zone* zone);
	void* malloc(size_t size);
	void* malloc_no_align(size_t size);

	void push_finalizer(void (*func)(void*), void* data);

	template <typename T>
	void push_finalizer(msgpack::unique_ptr<T> obj);

	void clear();

	void swap(zone& o);


	template <typename T, typename... Args>
	T* allocate(Args... args);

private:
	void undo_malloc(size_t size);

	template <typename T>
	static void object_destructor(void* obj);

	void* malloc_expand(size_t size);
};

inline zone* zone::create(size_t chunk_size)
{
	zone* z = static_cast<zone*>(::malloc(sizeof(zone) + chunk_size));
	if (!z) {
		return nullptr;
	}
	try {
		new (z) zone(chunk_size);
	}
	catch (...) {
		::free(z);
		return nullptr;
	}
	return z;
}

inline void zone::destroy(zone* z)
{
	z->~zone();
	::free(z);
}

inline zone::zone(size_t chunk_size):chunk_size_(chunk_size), chunk_list_(chunk_size_)
{
}

inline void* zone::malloc(size_t size)
{
	return malloc_no_align(
		((size)+((MSGPACK_ZONE_ALIGN)-1)) & ~((MSGPACK_ZONE_ALIGN)-1));
}

inline void* zone::malloc_no_align(size_t size)
{
	if(chunk_list_.free_ < size) {
		return malloc_expand(size);
	}

	char* ptr = chunk_list_.ptr_;
	chunk_list_.free_ -= size;
	chunk_list_.ptr_  += size;

	return ptr;
}

inline void* zone::malloc_expand(size_t size)
{
	chunk_list* const cl = &chunk_list_;

	size_t sz = chunk_size_;

	while(sz < size) {
		sz *= 2;
	}

	chunk* c = static_cast<chunk*>(::malloc(sizeof(chunk) + sz));

	char* ptr = reinterpret_cast<char*>(c) + sizeof(chunk);

	c->next_  = cl->head_;
	cl->head_ = c;
	cl->free_ = sz - size;
	cl->ptr_  = ptr + size;

	return ptr;
}

inline void zone::push_finalizer(void (*func)(void*), void* data)
{
	finalizer_array_.push(func, data);
}

template <typename T>
inline void zone::push_finalizer(msgpack::unique_ptr<T> obj)
{
	finalizer_array_.push(&zone::object_destructor<T>, obj.get());
	obj.release();
}

inline void zone::clear()
{
	finalizer_array_.clear();
	chunk_list_.clear(chunk_size_);
}

inline void zone::swap(zone& o)
{
	std::swap(*this, o);
}

template <typename T>
void zone::object_destructor(void* obj)
{
	reinterpret_cast<T*>(obj)->~T();
}

inline void zone::undo_malloc(size_t size)
{
	chunk_list_.ptr_  -= size;
	chunk_list_.free_ += size;
}


template <typename T, typename... Args>
T* zone::allocate(Args... args)
{
	void* x = malloc(sizeof(T));
	try {
		finalizer_array_.push(&zone::object_destructor<T>, x);
	} catch (...) {
		undo_malloc(sizeof(T));
		throw;
	}
	try {
		return new (x) T(args...);
	} catch (...) {
		--finalizer_array_.tail_;
		undo_malloc(sizeof(T));
		throw;
	}
}

}  // namespace msgpack

#endif /* msgpack/zone.hpp */
