#ifndef MINIMAP_H
#define MINIMAP_H

#include <vector>

template <class T, class S> class MiniMap
{
public:
	void append(T t, S s);
	S *find(T t);
	void clear();
	int len();
	S *get(int i);
private:
	std::vector<T> tClass;
	std::vector<S> sClass;
};

template <class T, class S> int MiniMap<T,S>::len()
{
	return (int)tClass.size();
}

template <class T, class S> void MiniMap<T,S>::clear()
{
	this->tClass.clear();
	this->sClass.clear();
}

template <class T, class S> S *MiniMap<T,S>::get(int i)
{
	return &sClass[i];
}

template <class T, class S> void MiniMap<T,S>::append(T t, S s)
{
	size_t i = 0;
	for( i = 0; i < this->tClass.size(); i++)
	{
		if( tClass[i] == t )
		{
			break;
		}
	}
	if( i >= this->tClass.size())
	{
		tClass.push_back(t);
		sClass.push_back(s);
	}else{
		sClass[i] = s;
	}
}

template <class T, class S> S *MiniMap<T,S>::find(T t)
{
	S *s = nullptr;
	for( size_t i = 0; i < this->tClass.size(); i++)
	{
		if( tClass[i] == t )
		{
			s = &sClass[i];
			break;
		}
	}
	return s;
}

#endif