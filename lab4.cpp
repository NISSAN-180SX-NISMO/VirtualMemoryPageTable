#include <iostream>
#include <algorithm>
#include "lab4.h"
#include "PageTable/PageTable.h"
#include "PageTable/PageFaultHandle/FIFO.h"
#include "PageTable/PageFaultHandle/NFU.h"

#define PAGE_TABLE_CAPACITY 5

int main(int argc, char *argv[]) {
//    if (argc != 2) {
//        std::cerr << "Usage: " << argv[0] << " <algorithm number>" << std::endl;
//        return 1;
//    }

//    int algorithm_number = std::stol(std::string(argv[1]));


    PageTable page_table(PAGE_TABLE_CAPACITY);
    int algorithm_number = 2;

    if (algorithm_number == 1) {
        PageFaultHandle::FIFO fifo(page_table);
        page_table.set_page_fault_handler(std::make_shared<PageFaultHandle::FIFO>(fifo));
    } else if (algorithm_number == 2) {
        PageFaultHandle::NFU nfu(page_table);
        page_table.set_page_fault_handler(std::make_shared<PageFaultHandle::NFU>(nfu));
    } else {
        std::cerr << "Invalid algorithm number. Must be 1 or 2." << std::endl;
        return 1;
    }
//    int operation_type, VPN;
//    while (std::cin >> operation_type >> VPN) {
//        std::bitset<1> operation_bit(operation_type);
//        page_table.frame_request(operation_bit, VPN);
//        page_table.print(PageTable::print::release);
//        std::cout << std::endl << std::flush;
//    }

    std::vector<std::pair<int, int>> input
            {
                    {0, 0},
                    {0, 1},
                    {0, 2},
                    {0, 3},
                    {0, 4},
                    {0, 5},
                    {0, 0},
                    {1, 0},
                    {0, 5},
                    {1, 0}
            };

    std::for_each(input.begin(), input.end(), [&](const std::pair<int, int> &pair) {
        std::bitset<1> operation_bit(pair.first);
        int VPN = pair.second;
        page_table.frame_request(operation_bit, VPN);
        page_table.print(PageTable::print::debug);
        std::cout << std::endl << std::flush;
    });


    return 0;
}