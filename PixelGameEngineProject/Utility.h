#pragma once

#include <cstdlib>
#include <iostream>

/**
 * \brief Print with cout
 * \param value Value to print
 */
#define LOG(VALUE) std::cout << (VALUE) << std::endl

 /* alive objects */
int g_objects{ 0 };

void* operator new(size_t size)
{
	void* ptr = malloc(size);

	++g_objects;

	LOG(g_objects);

	return ptr;
}

void operator delete(void* p)
{
	free(p);

	--g_objects;

	LOG(g_objects);	
}