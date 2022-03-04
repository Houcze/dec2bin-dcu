#include <hip/hip_runtime.h>
#include <iostream>
#include <string>

__global__ void hip_bin(int *input, int *output, int n)
{
	int id = blockIdx.x * blockDim.x + threadIdx.x;
	if(id < n)
	{
		output[id] = (input[id] / (1 << id)) % 2;
	}
}


std::string bin(int num)
{
	int *h_a;
	int *h_b;
	int *d_a;
	int *d_b;
	int bin_number_bit{0};
	while((1 << bin_number_bit) < num)
	{
		bin_number_bit += 1;
	}
	bin_number_bit += 1;
	size_t bytes = bin_number_bit * sizeof(int);
	h_a = (int*)malloc(bytes);
	h_b = (int*)malloc(bytes);

	for(int i=0; i < bin_number_bit; i++)
	{
		h_a[i] = num;
		h_b[i] = 0;
	}

	hipMalloc(&d_a, bytes);
	hipMalloc(&d_b, bytes);

	hipMemcpy(d_a, h_a, bytes, hipMemcpyHostToDevice);
	hipMemcpy(d_b, h_b, bytes, hipMemcpyHostToDevice);

	int blockSize, gridSize;
	blockSize = bin_number_bit; 	// blockSize is the number of threads in a block
	gridSize = 1; 					// gridSize is the number of block
	hipLaunchKernelGGL(hip_bin, dim3(gridSize), dim3(blockSize), 0, 0, d_a, d_b, bin_number_bit);
	hipDeviceSynchronize();
	hipMemcpy(h_b, d_b, bytes, hipMemcpyDeviceToHost);

	
	char *result;
	result = (char*)malloc(bin_number_bit * sizeof(char));
	for(int i=bin_number_bit; i>0; i--)
	{
		result[bin_number_bit-i] = h_b[i-1] + '0';
	}
	std::string s = result;
	std::cout << s << std::endl;
	return s;
}


