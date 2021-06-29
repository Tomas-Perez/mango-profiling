#include "profiling.h"
#include "json.hpp"

#include <iostream>
#include <fstream>
#include <random>

namespace profiling {

    void Profiler::print() {
        using std::chrono::duration_cast;
		using std::chrono::duration;
		using std::chrono::nanoseconds;

		if(sample_bench) {
			std::cout << "Sample finished" << std::endl;
			std::cout << "Name: " << sample_bench->name << std::endl;
			std::cout << "Parameters: " << std::endl;
			for (auto &p : sample_bench->parameters) {
				std::cout << "\t" << p.first << ": " << p.second << std::endl;
			}
			auto result_str = sample_bench->success_set? (sample_bench->success? "SUCCESS" : "FAILURE") : "UNKNOWN";
			auto duration_ns = duration_cast<nanoseconds>(sample_bench->end - sample_bench->start);
			std::cout << "Result: " << result_str << " | Duration (ns): " << duration_ns.count() << std::endl;
		}
		std::cout << "Buffer reads:" << std::endl;
		for (auto &bb : buffer_read_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(bb->end - bb->start);
			std::cout << "Id: " << bb->buffer_id << " | Size (bytes): " << bb->size << " | Duration (ns): " << duration_ns.count() << std::endl;
		}
		std::cout << "Buffer writes:" << std::endl;
		for (auto &bb : buffer_write_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(bb->end - bb->start);
			std::cout << "Id: " << bb->buffer_id << " | Size (bytes): " << bb->size << " | Duration (ns): " << duration_ns.count() << std::endl;
		}
		std::cout << "Kernel executions:" << std::endl;
		for (auto &kb : kernel_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(kb->end - kb->start);
			std::cout << "Id: " << kb->kernel_id << " | Duration (ns): " << duration_ns.count() << std::endl;
		}
		std::cout << "Resource allocations:" << std::endl;
		for (auto &rb : resource_allocation_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(rb->end - rb->start);
			std::cout
				<< "# Kernels: " << rb->kernel_amount 
				<< " | # Buffers: " << rb->buffer_amount 
				<< " | # Events: " << rb->event_amount 
				<< " | Duration (ns): " << duration_ns.count() << std::endl;
		}
		std::cout << "Resource deallocations:" << std::endl;
		for (auto &rb : resource_deallocation_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(rb->end - rb->start);
			std::cout
				<< "# Kernels: " << rb->kernel_amount 
				<< " | # Buffers: " << rb->buffer_amount 
				<< " | # Events: " << rb->event_amount 
				<< " | Duration (ns): " << duration_ns.count() << std::endl;
		}
    }

	void Profiler::save_to_file() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 9999);
		int rand_num = dist(gen);
		auto rand_str = std::to_string(rand_num);
		auto rand_str_padded = std::string(4 - rand_str.length(), '0') + rand_str;

        using nlohmann::json;
        using std::chrono::duration_cast;
		using std::chrono::duration;
		using std::chrono::nanoseconds;
		using std::chrono::seconds;

        std::string filename("mango_profiling_");

        json data;

		if(sample_bench) {
            data["name"] = sample_bench->name;
            filename += sample_bench->name + "_";
            json params;
			for (auto &p : sample_bench->parameters) {
                params[p.first] = p.second;
			}
            data["params"] = params;
			auto result_str = sample_bench->success_set? (sample_bench->success? "SUCCESS" : "FAILURE") : "UNKNOWN";
            data["result"] = result_str;
			auto duration_ns = duration_cast<nanoseconds>(sample_bench->end - sample_bench->start);
            data["total_duration"] = duration_ns.count();
		}
        std::vector<json> buffer_reads;
		for (auto &bb : buffer_read_benchmarks) {
            auto duration_ns = duration_cast<nanoseconds>(bb->end - bb->start);
            json read = {
                {"buffer_id", bb->buffer_id},
                {"size", bb->size},
                {"duration", duration_ns.count()}
            };
            buffer_reads.emplace_back(read);
		}
		std::vector<json> buffer_writes;
		for (auto &bb : buffer_write_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(bb->end - bb->start);
            json write = {
                {"buffer_id", bb->buffer_id},
                {"size", bb->size},
                {"duration", duration_ns.count()}
            };
            buffer_writes.emplace_back(write);
		}
		std::vector<json> kernel_executions;
		for (auto &kb : kernel_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(kb->end - kb->start);
            json exec = {
                {"kernel_id", kb->kernel_id},
                {"duration", duration_ns.count()}
            };
            kernel_executions.emplace_back(exec);
		}
		std::vector<json> resource_allocations;
		for (auto &rb : resource_allocation_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(rb->end - rb->start);
            json alloc = {
                {"kernel_amount", rb->kernel_amount},
                {"buffer_amount", rb->buffer_amount},
                {"event_amount", rb->event_amount},
                {"duration", duration_ns.count()}
            };
            resource_allocations.emplace_back(alloc);
		}
		std::vector<json> resource_deallocations;
		for (auto &rb : resource_deallocation_benchmarks) {
			auto duration_ns = duration_cast<nanoseconds>(rb->end - rb->start);
            json dealloc = {
                {"kernel_amount", rb->kernel_amount},
                {"buffer_amount", rb->buffer_amount},
                {"event_amount", rb->event_amount},
                {"duration", duration_ns.count()}
            };
			resource_deallocations.emplace_back(dealloc);
		}
        data["buffer_reads"] = buffer_reads;
        data["buffer_writes"] = buffer_writes;
        data["kernel_executions"] = kernel_executions;
        data["resource_allocations"] = resource_allocations;
        data["resource_deallocations"] = resource_deallocations;

        const auto p1 = std::chrono::system_clock::now();
        auto timestamp = duration_cast<seconds>(p1.time_since_epoch()).count();

        filename += std::to_string(timestamp) + "_" + rand_str_padded + ".json";

        std::ofstream ofs(filename, std::ios::out);
        ofs << data.dump();
        ofs.close();

        std::cout << "Profiling file saved at: " << filename << std::endl;
    }
}