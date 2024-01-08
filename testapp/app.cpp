/**
 * @file testapp/app.cpp
 * @author Caleb Burke
 * @date Jan 6, 2024
 */

#include <iostream>

#include <core/test.hpp>

int main(int argc, const char** argv) {
	(void)argc;
	(void)argv;
	std::cout << __FILE__ << "::" << __LINE__ << '\n';
    
	bool b = testprint(123);
	if(b){
		std::cout << "yes\n";
	} else {
		std::cout << "no\n";
	}

	return 0;
}

