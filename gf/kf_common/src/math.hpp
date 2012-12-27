/*
 * =====================================================================================
 *
 *       Filename:  math.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/24/2011 05:48:28 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef GF_MATH_HPP
#define GF_MATH_HPP
#include <vector>
#include <cassert>
#include <cstdlib>

template <typename Type>
void RandFrom(std::vector<Type> & in, int M) {
	assert(in.size()>= M);
	int n = in.size();
	for (int i = 0; i < M; i++) {
		Type tmp = in[i];
		int j = rand()%(n - i) + i;
		in[i] = in[j];
		in[j] = tmp;
	}
}

#endif


