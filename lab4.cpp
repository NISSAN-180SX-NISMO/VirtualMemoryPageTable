#include <iostream>
#include "lab4.h"
#include "PageTable/PageTable.h"
#include "PageTable/PageFaultHandle/FIFO.h"
#include "PageTable/PageFaultHandle/NFU.h"
#include <vector>

int main(int argc, char *argv[])
{
    PageTable page_table(5);
    PageFaultHandle::NFU fifo(page_table);
    page_table.set_page_fault_handler(std::make_shared<PageFaultHandle::NFU>(fifo));

    // TODO сделать принт для NFU и посмотреть счетчики


    std::vector<std::pair<int, int>> operations = {
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

    for (auto &operation : operations)
    {
        int VPN = operation.second;
        std::bitset<1> operation_bit(operation.first);
        page_table.frame_request(operation_bit, VPN);
        page_table.print();
        std::cout << "-----------------------" << std::endl;
    }
    std::cin >> argc;
}