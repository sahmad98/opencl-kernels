#include <iostream>
#include <boost/compute.hpp>
#include <CL/cl.h>
#include <chrono>
#include <cstring>

namespace compute = ::boost::compute;
int main () {
    auto gpu = compute::system::default_device();
    std::cout << "Using Default Device: " << gpu.name() << "\n";
    auto ctx = compute::context(gpu);
    auto cmd_queue = compute::command_queue(ctx, gpu, CL_QUEUE_PROFILING_ENABLE);

    const auto size = 256;
    compute::buffer device_buffer(ctx, size * sizeof(cl_int));
    cl_int err = 0;
    int* dev_ptr = (int*)clEnqueueMapBuffer(cmd_queue.get(),
            device_buffer.get(),
            CL_TRUE,
            CL_MAP_READ | CL_MAP_WRITE,
            0,
            sizeof(cl_int) * size,
            0,
            NULL,
            NULL,
            &err);

    if (!dev_ptr) {
        std::cerr << "Error in Mapping" << "\n";
    }

    std::memset(dev_ptr, 0, sizeof(cl_int) * size);

    auto program = compute::program::create_with_source_file("increment.cl", ctx);
    try {
        program.build();
    } catch (compute::opencl_error) {
        std::cout << program.build_log() << "\n";
    }

    auto increment_kernel = compute::kernel(program, "increment");
    std::cout << "Running: " << increment_kernel.name() << "\n";


    increment_kernel.set_arg(0, device_buffer);

    auto finish_event = cmd_queue.enqueue_1d_range_kernel(increment_kernel, 0, size, 128);
    compute::wait_list wait(finish_event);
    cmd_queue.flush();
    cmd_queue.finish();

    for (int i=1; i<size+1; ++i) {
        std::cout << dev_ptr[i-1] << " ";
        if (i % 32 == 0) {
            std::cout << "\n";
        }
    }

    clEnqueueUnmapMemObject(cmd_queue.get(), device_buffer.get(), dev_ptr, 0, NULL, NULL);

    auto start = finish_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_START);
    auto end = finish_event.get_profiling_info<cl_long>(CL_PROFILING_COMMAND_END);
    std::cout << "\nFinsihed Duration: " << end - start << " ns\n";
}
