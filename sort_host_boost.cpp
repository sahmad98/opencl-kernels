#include <iostream>
#include <boost/compute.hpp>
#include <CL/cl.h>
#include <chrono>

namespace compute = ::boost::compute;
int main () {
    auto gpu = compute::system::default_device();
    std::cout << "Using Default Device: " << gpu.name() << "\n";
    auto ctx = compute::context(gpu);
    auto cmd_queue = compute::command_queue(ctx, gpu, CL_QUEUE_PROFILING_ENABLE);

    const auto size = 2 * 4096;
    int* host_ptr = (int*)malloc(sizeof(int) * size);

    for (int i=0; i<size; ++i) {
        host_ptr[i] = rand() % size;
    }

    auto program = compute::program::create_with_source_file("sort.cl", ctx);
    try {
        program.build();
    } catch (compute::opencl_error) {
        std::cout << program.build_log() << "\n";
    }
    auto sort_kernel = compute::kernel(program, "bubble_sort");
    std::cout << "Running: " << sort_kernel.name() << "\n";

    compute::buffer device_buffer(ctx, size * sizeof(cl_int), CL_MEM_READ_WRITE);
    auto copy_event = cmd_queue.enqueue_write_buffer_async(device_buffer, 0, sizeof(cl_int) * size, host_ptr);
    compute::wait_list copy_wait(copy_event);

    sort_kernel.set_arg(0, device_buffer);
    sort_kernel.set_arg(1, size);

    auto sort_finish_event = cmd_queue.enqueue_task(sort_kernel, copy_event);
    compute::wait_list wait(sort_finish_event);
    cmd_queue.enqueue_read_buffer(device_buffer, 0, sizeof(cl_int) * size, host_ptr, wait);
    cmd_queue.flush();
    cmd_queue.finish();

    for (int i=0; i<size; ++i) {
        std::cout << host_ptr[i] << " ";
        if (i !=0 and i % 16 == 0) {
            std::cout << "\n";
        }
    }

    auto start = sort_finish_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_START);
    auto end = sort_finish_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_END);
    auto start_data = copy_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_START);
    auto end_data = copy_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_END);
    std::cout << "\nFinsihed Duration: " << end - start << " ns\n";
    std::cout << "Copy Duration: " << end_data - start_data << " ns\n";

    auto rate = (1.0f * sizeof(int) * size) / (end_data - start_data);
    std::cout << "Copy Rate: " << rate << " GB/s\n"; 
    free(host_ptr);
}