// Copyright (c) 2012, Luoxizhi. All rights reserved.
//
// History:
// Date         Reference       Person          Descriptions
// ---------- 	-------------- 	--------------  ---------------------------
// 2012/03/01                   Luoxizhi        Initial Implementation
//
//-------------------------------------------------------------------------
#pragma once

#ifndef JCycleMap_H
#define JCycleMap_H

#define JCycleMap_Default  128

template<class _Key, class _Value>
class JCycleMap
{
public:
	JCycleMap(): size_capacity(JCycleMap_Default)
		, size_real(0)
		, map_header(NULL)
		, map_tail(NULL)
	{			
	}

	JCycleMap(long size): size_capacity(size)
		, size_real(0)
		, map_header(NULL)
		, map_tail(NULL)
	{
	}

	~JCycleMap(){
		clear();
	}

private:
	typedef struct _MapNode{
		_Key				key;
		_Value				val;
		struct _MapNode*	next;
		struct _MapNode*	prev;
	} MapNode;

	unsigned long	size_capacity;
	unsigned long	size_real;
	MapNode*		map_header;
	MapNode*		map_tail;

public:
	class iterator{
	public:
		iterator() : _ptr(NULL){}
		iterator(MapNode* ptr) : _ptr(ptr){}
		iterator& operator = (iterator& iter){
			_ptr = iter._ptr;
			return *this;
		}
		bool operator != (iterator& iter){
			return _ptr != iter._ptr;
		}
		bool operator == (iterator& iter){
			return _ptr == iter._ptr;
		}
		pair<_Key, _Value> operator *(){
			return make_pair(_ptr->key, _ptr->val);
		}
		iterator& operator ++ (){
			_ptr = _ptr->next;
			return *this;
		}
		iterator& operator -- (){
			_ptr = _ptr->prev;
			return *this;
		}
		MapNode*  _ptr;
	};

	iterator begin(){
		return iterator(map_header);
	}

	iterator end(){
		return iterator(map_tail->next);
	}

	iterator find(_Key key){
		MapNode* tmp = map_header;
		while(tmp){
			if( tmp->key == key ){
				return iterator(tmp);
			}
			tmp = tmp->next;
		}
		return iterator(tmp);
	}

	_Value& operator [] (_Key key){
		MapNode* tmp = map_header;
		while(tmp){
			if( tmp->key == key ){
				return tmp->val;
			}
			tmp = tmp->next;
		}
		tmp = add_new_one();
		tmp->key = key;
		return tmp->val;
	}

	void clear(){
		while(map_tail){
			map_tail = map_tail->prev;
			if( map_tail ){
				delete map_tail->next;
			}
		}
		map_header = NULL;
		map_tail = NULL;
	}

	void resize(unsigned long size){
		if( size > size_capacity ){
			size_capacity = size;
		}else{
			long count = size_capacity - size;
			while(count-- && map_tail){
				map_tail = map_tail->prev;
				if( map_tail ){
					delete map_tail->next;
				}
			}
			
		}
	}

private:
	MapNode* add_new_one(){
		if( map_tail ){
			if( size_real < size_capacity ){
				if( map_tail ){
					map_tail->next = new MapNode;
					map_tail->next->prev = map_tail;
					map_tail->next->next = NULL;
					map_tail = map_tail->next;
					size_real++;
				}			
			}else{
				map_header->prev = map_tail;
				map_tail->next = map_header;
				map_header = map_header->next;
				map_header->prev = NULL;
				map_tail = map_tail->next;
				map_tail->next = NULL;
			}
		}else{
			map_header = new MapNode;
			map_header->prev = NULL;
			map_header->next = NULL;
			map_tail = map_header;
			size_real++;
		}		
		return map_tail;
	}
};


#endif // JCycleList_H