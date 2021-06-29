#ifndef PROFILING_H
#define PROFILING_H

#include <vector>
#include <chrono>
#include <string>
#include <memory>
#include <cinttypes>

namespace profiling {

struct benchmark {
	const std::chrono::high_resolution_clock::time_point start;
	std::chrono::high_resolution_clock::time_point end;
	bool finished;

	benchmark(): start(std::chrono::high_resolution_clock::now()), finished(false) { end = start; }

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

	std::shared_ptr<buffer_benchmark> start_buffer_read(uint32_t buffer_id, uint64_t size) {
		all_dumped = false;
		auto ptr = std::make_shared<buffer_benchmark>(buffer_id, size);
		buffer_read_benchmarks.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<buffer_benchmark> start_buffer_write(uint32_t buffer_id, uint64_t size) {
		all_dumped = false;
		auto ptr = std::make_shared<buffer_benchmark>(buffer_id, size);
		buffer_write_benchmarks.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<kernel_benchmark> start_kernel_execution(uint32_t kernel_id) {
		all_dumped = false;
		auto ptr = std::make_shared<kernel_benchmark>(kernel_id);
		kernel_benchmarks.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<resource_allocation_benchmark> start_resource_allocation(
		size_t kernel_amount, 
		size_t buffer_amount, 
		size_t event_amount
	) {
		all_dumped = false;
		auto ptr = std::make_shared<resource_allocation_benchmark>(kernel_amount, buffer_amount, event_amount);
		resource_allocation_benchmarks.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<resource_allocation_benchmark> start_resource_deallocation(
		size_t kernel_amount, 
		size_t buffer_amount, 
		size_t event_amount
	) {
		all_dumped = false;
		auto ptr = std::make_shared<resource_allocation_benchmark>(kernel_amount, buffer_amount, event_amount);
		resource_deallocation_benchmarks.push_back(ptr);
		return ptr;
	}

	std::shared_ptr<sample_benchmark> start_sample_benchmark(
		std::string name,
		std::vector<std::pair<std::string, std::string>> parameters
	) {
		all_dumped = false;
		sample_bench = std::make_shared<sample_benchmark>(name, parameters);
		return sample_bench;
	}

	void dump() {
		print();
		if (!sample_bench || sample_bench->finished) {
			save_to_file();
		}
		if (sample_bench) {
			sample_bench = nullptr;
		}
		buffer_read_benchmarks.clear();
		buffer_write_benchmarks.clear();
		resource_allocation_benchmarks.clear();
		resource_deallocation_benchmarks.clear();
		kernel_benchmarks.clear();
		all_dumped = true;
	}

private:
	std::vector<std::shared_ptr<buffer_benchmark>> buffer_read_benchmarks;
	std::vector<std::shared_ptr<buffer_benchmark>> buffer_write_benchmarks;
	std::vector<std::shared_ptr<kernel_benchmark>> kernel_benchmarks;
	std::vector<std::shared_ptr<resource_allocation_benchmark>> resource_allocation_benchmarks;
	std::vector<std::shared_ptr<resource_allocation_benchmark>> resource_deallocation_benchmarks;
	std::shared_ptr<sample_benchmark> sample_bench;
	bool all_dumped = false;

	Profiler() {}

	~Profiler() {
		if (!all_dumped) {
			dump();
		}
	}

	void print();
	void save_to_file();
};


} // namespace profiling

#endif //  PROFILING_H

