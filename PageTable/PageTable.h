#ifndef OS_TASK4_NISSAN_180SX_NISMO_PAGETABLE_H
#define OS_TASK4_NISSAN_180SX_NISMO_PAGETABLE_H

#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include <iomanip>
#include "PageFaultHandle/PageFaultHandlerInterface.h"

namespace PageFaultHandle {
    class FIFO;
    class NFU;
}

class PageTable {
    friend class PageFaultHandle::FIFO;
    friend class PageFaultHandle::NFU;
public:
    enum class print: bool {
        debug,
        release
    };
private:
    struct PageItem {
        PageItem(int PPN, int VPN, std::bitset<1> R, std::bitset<1> M);

        int PPN = 0; // Physical Page Number
        int VPN = 0; // Virtual Page Number
        std::bitset<1> R = 0b0; // Usage bit
        std::bitset<1> M = 0b0; // Modification bit
    };

    std::vector<std::shared_ptr<PageItem>> page_table;

    std::shared_ptr<PageFaultHandlerInterface> page_fault_handler;

    void print_debug();

    void print_release();

public:
    explicit PageTable(int frame_count);

    void set_page_fault_handler(std::shared_ptr<PageFaultHandlerInterface> page_fault_handler);

    int frame_request(std::bitset<1> operation, int VPN);

    void print(print mode = print::debug);
};

PageTable::PageItem::PageItem(int PPN, int VPN, std::bitset<1> R, std::bitset<1> M) {
    this->PPN = PPN;
    this->VPN = VPN;
    this->R = R;
    this->M = M;
}

PageTable::PageTable(int frame_count) {
    this->page_fault_handler = nullptr;
    this->page_table.resize(frame_count);
    for (int i = 0; i < frame_count; i++) {
        this->page_table[i] = nullptr;
    }
}

void PageTable::set_page_fault_handler(std::shared_ptr<PageFaultHandlerInterface> page_fault_handler) {
    this->page_fault_handler = std::move(page_fault_handler);
}

int PageTable::frame_request(std::bitset<1> operation, int VPN) {
    if (!this->page_fault_handler) {
        throw std::runtime_error("Page fault handler is not set");
    }
    return this->page_fault_handler->frame_request(operation, VPN);
}

void PageTable::print_debug() {
    this->page_fault_handler->print();
}

void PageTable::print_release() {
    for (size_t i = 0; i < page_table.size(); ++i) {
        std::cout << (page_table[i] ? std::to_string(page_table[i]->VPN) : "#");
        if (i != page_table.size() - 1) {
            std::cout << ' ';
        }
    }
    std::cout << std::flush;
}

void PageTable::print(enum PageTable::print mode) {
    switch (mode) {
        case print::debug:
            print_debug();
            break;
        case print::release:
            print_release();
            break;
    };
}

#endif //OS_TASK4_NISSAN_180SX_NISMO_PAGETABLE_H
