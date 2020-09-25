#include <CL/cl.h>

#include <iostream>

int main() {
	cl_uint num_platforms = 0;
	clGetPlatformIDs(0, NULL, &num_platforms);
	std::cout << "Platforms: " << num_platforms << std::endl;
}
