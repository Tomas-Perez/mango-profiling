#ifndef PROFILING_H
#define PROFILING_H

#include <vector>
#include <chrono>
#include <string>
#include <cinttypes>

namespace profiling {

struct benchmark {
	const std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	bool finished;

	benchmark(): start(std::chrono::high_resolution_clock::now()) {}

	void finish() {
		finished = true;
		end = std::chrono::high_resolution_clock::now();
	}
};

struct buffer_benchmark : benchmark {
	const uint32_t buffer_id;
	const uint64_t size;

	buffer_benchmark(uint32_t id, uint64_t size): 
		benchmark(),
		buffer_id(id), 
		size(size) {}	
};

struct kernel_benchmark : benchmark {
	const uint32_t kernel_id;

	kernel_benchmark(uint32_t id): benchmark(), kernel_id(id) {}	
};

struct resource_allocation_benchmark : benchmark {
	const size_t kernel_amount;
	const size_t buffer_amount;
	const size_t event_amount;

	resource_allocation_benchmark(
		size_t kernel_amount, 
		size_t buffer_amount, 
		size_t event_amount
	): benchmark(), kernel_amount(kernel_amount), buffer_amount(buffer_amount), event_amount(event_amount) {}	
};

struct sample_benchmark : benchmark {
	const std::string name;
	const std::vector<std::pair<std::string, std::string>> parameters;
	bool success;
	bool success_set = false;

	sample_benchmark(std::string name, std::vector<std::pair<std::string, std::string>> parameters):
		name(name), parameters(parameters) {}

	void fail() {
		success = false;
		success_set = true;
	}

	void correct() {
		success = true;
		success_set = true;
	}
};

/*! \class Profiler
 *  \brief 
 */
class Profiler {
	
public:

	static Profiler &get_instance() noexcept {
		static Profiler instance;
		return instance;
	}

	buffer_benchmark *start_buffer_read(uint32_t buffer_id, uint64_t size) {
		buffer_read_benchmarks.emplace_back(buffer_id, size);
		return &buffer_read_benchmarks.back();
	}

	buffer_benchmark *start_buffer_write(uint32_t buffer_id, uint64_t size) {
		buffer_write_benchmarks.emplace_back(buffer_id, size);
		return &buffer_write_benchmarks.back();
	}

	kernel_benchmark *start_kernel_execution(uint32_t kernel_id) {
		kernel_benchmarks.emplace_back(kernel_id);
		return &kernel_benchmarks.back();
	}

	resource_allocation_benchmark *start_resource_allocation(
		size_t kernel_amount, 
		size_t buffer_amount, 
		size_t event_amount
	) {
		resource_allocation_benchmarks.emplace_back(kernel_amount, buffer_amount, event_amount);
		return &resource_allocation_benchmarks.back();
	}

	resource_allocation_benchmark *start_resource_deallocation(
		size_t kernel_amount, 
		size_t buffer_amount, 
		size_t event_amount
	) {
		resource_deallocation_benchmarks.emplace_back(kernel_amount, buffer_amount, event_amount);
		return &resource_deallocation_benchmarks.back();
	}

	sample_benchmark *start_sample_benchmark(
		std::string name,
		std::vector<std::pair<std::string, std::string>> parameters
	) {
		sample_bench = new sample_benchmark(name, parameters);
		return sample_bench;
	}

private:
	std::vector<buffer_benchmark> buffer_read_benchmarks;
	std::vector<buffer_benchmark> buffer_write_benchmarks;
	std::vector<kernel_benchmark> kernel_benchmarks;
	std::vector<resource_allocation_benchmark> resource_allocation_benchmarks;
	std::vector<resource_allocation_benchmark> resource_deallocation_benchmarks;
	sample_benchmark *sample_bench;

	Profiler() {}

	~Profiler() {
		print();
		if (sample_bench) {
			if (sample_bench->finished) {
				save_to_file();
			}
			delete sample_bench;
		}
	}

	void print();
	void save_to_file();
};


} // namespace profiling

#endif //  PROFILING_H

