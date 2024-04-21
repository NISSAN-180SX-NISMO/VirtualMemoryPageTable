//
// Created by User on 21.04.2024.
//

#ifndef OS_TASK4_NISSAN_180SX_NISMO_NFU_H
#define OS_TASK4_NISSAN_180SX_NISMO_NFU_H

#include <queue>
#include "PageFaultHandlerInterface.h"
#include "../PageTable.h"
#include "../../lab4.h"

namespace PageFaultHandle {

    class NFU : public PageFaultHandlerInterface {
    private:
        struct NFUItem {
            NFUItem(int PPN, int VPN, uint32_t counter) :
            PPN(PPN), VPN(VPN), counter(counter) {}
            NFUItem() = default;
            int PPN; // Physical Page Number
            int VPN; // Virtual Page Number
            uint32_t counter; // счетчик использования
        };
        std::vector<NFUItem> nfu_table;  // таблица счетчиков использования
        PageTable* page_table_class;
    public:
        explicit NFU(PageTable&);

        void interrupting();

        int handle(std::bitset<1> operation, int VPN) override;

        int frame_request(std::bitset<1> operation, int VPN) override;
    };

#define table this->page_table_class->page_table
    NFU::NFU(PageTable& page_table_class) : page_table_class(&page_table_class) {
        this->nfu_table.resize(page_table_class.page_table.size());
        for (auto & i : this->nfu_table) {
            i.PPN = 0;
            i.VPN = 0;
            i.counter = 0x00; // 4 байта в 16-ричной системе
        }
    }

    void NFU::interrupting() {
        for (int i = 0; i < nfu_table.size(); ++i) {
            if (table[i]) {
                nfu_table[i].counter += table[i]->R.test(0) ? 1 : 0;
                table[i]->R.reset();
            }
        }
    }

    int NFU::handle(std::bitset<1> operation, int VPN) {
    // Находим страницы с минимальным значением счетчика
    std::vector<int> min_counter_indices;
    uint32_t min_counter_value = nfu_table[0].counter;
    for (int i = 0; i < nfu_table.size(); ++i) {
        if (nfu_table[i].counter < min_counter_value) {
            min_counter_indices.clear();
            min_counter_indices.push_back(i);
            min_counter_value = nfu_table[i].counter;
        } else if (nfu_table[i].counter == min_counter_value) {
            min_counter_indices.push_back(i);
        }
    }

    // Выбираем случайную страницу из найденных
    int random_index = uniform_rnd(0, min_counter_indices.size() - 1);
    int selected_index = min_counter_indices[random_index];

    // Заменяем выбранную страницу на новую
    table[selected_index] = std::make_shared<PageTable::PageItem>(selected_index, VPN, 0b1, operation.test(0) ? 0b1 : 0b0);
    nfu_table[selected_index].counter = 0x00;

    return nfu_table[selected_index].PPN;
}

    int NFU::frame_request(std::bitset<1> operation, int VPN) {
        for (int PPN = 0; PPN < table.size(); ++PPN) {
            if (!(PPN % 5)) this->interrupting();
            if (table[PPN]) {
                if (table[PPN]->VPN == VPN) {
                    // Если операция записи, то устанавливаем бит M (был модифицирован)
                    if (operation.test(0)) table[PPN]->M.set();
                    // Устанавливаем бит R (был использован)
                    table[PPN]->R.set();
                    // Увеличиваем счетчик
                    nfu_table[PPN].counter += 1;
                    // Возвращаем номер физической страницы
                    return PPN;
                } else continue;
            } else {
                // Добавляем виртуальную страницу в таблицу
                table[PPN] = std::make_shared<PageTable::PageItem>(PPN, VPN, 0b1, operation.test(0) ? 0b1 : 0b0);
                // Обновляем счетчик
                nfu_table[PPN].VPN = VPN;
                nfu_table[PPN].counter = 0x00;
                // Возвращаем номер физической страницы
                return PPN;
            }
        }
        // Если таблица заполнена, то вызываем обработку page fault
        return this->handle(operation, VPN);
    }

}

#endif //OS_TASK4_NISSAN_180SX_NISMO_NFU_H
