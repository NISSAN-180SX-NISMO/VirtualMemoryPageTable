//
// Created by User on 21.04.2024.
//

#ifndef OS_TASK4_NISSAN_180SX_NISMO_FIFO_H
#define OS_TASK4_NISSAN_180SX_NISMO_FIFO_H

#include <queue>
#include "PageFaultHandlerInterface.h"
#include "../PageTable.h"

namespace PageFaultHandle {

    typedef int PPN; // Physical Page Number
    typedef int VPN; // Virtual Page Number

    class FIFO : public PageFaultHandlerInterface {
    private:
        std::queue<std::pair<PPN, VPN>> queue;  // очередь страниц
        PageTable* page_table_class;
    public:
        explicit FIFO(PageTable&);

        int handle(std::bitset<1> operation, int VPN) override;

        int frame_request(std::bitset<1> operation, int VPN) override;
    };

    FIFO::FIFO(PageTable& page_table_class) : page_table_class(&page_table_class) {}

#define table this->page_table_class->page_table // std::vector<std::shared_ptr<PageItem>> page_table;

    int FIFO::handle(std::bitset<1> operation, int VPN) {
        // берем первую страницу из очереди
        PPN page_to_remove = this->queue.front().first;
        // удаляем страницу из очереди
        this->queue.pop();
        // удаляем страницу из таблицы
        table[page_to_remove] = nullptr;
        // добавляем новую страницу в таблицу на место удаленной
        int new_page = page_to_remove;
        table[new_page] = std::make_shared<PageTable::PageItem>(new_page, VPN, 0b1, operation.test(0) ? 0b1 : 0b0);
        // возращаем номер физической страницы
        return new_page;
    }


    int FIFO::frame_request(std::bitset<1> operation, int VPN) {
        for (int PPN = 0; PPN < table.size(); ++PPN) {
            if (table[PPN]) {
                if (table[PPN]->VPN == VPN) {
                    // если операция записи, то устанавливаем бит M (был модифицирован)
                    if (operation.test(0)) table[PPN]->M.set();
                    // устанавливаем бит R (был использован)
                    table[PPN]->R.set();
                    // возвращаем номер физической страницы
                    return PPN;
                } else continue;
            } else {
                // добавляем виртуальную страницу в очередь
                this->queue.push({PPN, VPN});
                // добавляем виритуальную страницу в таблицу
                table[PPN] = std::make_shared<PageTable::PageItem>(PPN, VPN, 0b1, operation.test(0) ? 0b1 : 0b0);
                // возвращаем номер физической страницы
                return PPN;
            }
        }
        // если таблица заполнена, то вызываем обработку page fault
        return this->handle(operation, VPN);
    }
}

#endif //OS_TASK4_NISSAN_180SX_NISMO_FIFO_H
