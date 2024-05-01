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
        PageTable *page_table_class;
        int global_counter = 0;
    public:
        explicit NFU(PageTable &);

        void interrupting();

        int handle(std::bitset<1> operation, int VPN) override;

        int frame_request(std::bitset<1> operation, int VPN) override;

        void print() override;
    };

#define table this->page_table_class->page_table

    NFU::NFU(PageTable &page_table_class) : page_table_class(&page_table_class) {
        this->nfu_table.resize(page_table_class.page_table.size());
        for (auto &i: this->nfu_table) {
            i.PPN = 0;
            i.VPN = 0;
            i.counter = 0; // 4 байта в 16-ричной системе
        }
    }

    void NFU::interrupting() {
        for (int i = 0; i < nfu_table.size(); ++i) {
            if (table[i]) {
                nfu_table[i].counter += table[i]->R.test(0) == 1 ? 1 : 0;
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
        table[selected_index] = std::make_shared<PageTable::PageItem>(selected_index, VPN, 0b1,
                                                                      operation.test(0) ? 0b1 : 0b0);
        nfu_table[selected_index].counter = 0x00;

        return nfu_table[selected_index].PPN;
    }

    int NFU::frame_request(std::bitset<1> operation, int VPN) {
        for (int PPN = 0; PPN < table.size(); ++PPN) {
            if (table[PPN]) {
                if (table[PPN]->VPN == VPN) {
                    // Если операция записи, то устанавливаем бит M (был модифицирован)
                    if (operation.test(0)) table[PPN]->M.set();
                    // Устанавливаем бит R (был использован)
                    table[PPN]->R.set();
                    // При 5-ом обращении к памяти, то вызываем прерывание
                    if (!(global_counter % 5)) this->interrupting();
                    ++global_counter;
                    // Возвращаем номер физической страницы
                    return PPN;
                } else {
                    // При 5-ом обращении к памяти, то вызываем прерывание
                    if (!(global_counter % 5)) this->interrupting();
                    continue;
                };
            } else {
                // Добавляем виртуальную страницу в таблицу
                table[PPN] = std::make_shared<PageTable::PageItem>(PPN, VPN, 0b1, operation.test(0) ? 0b1 : 0b0);
                // Обновляем счетчик
                nfu_table[PPN].VPN = VPN;
                nfu_table[PPN].counter = 0x00;
                // При 5-ом обращении к памяти, то вызываем прерывание
                if (!(global_counter % 5)) this->interrupting();
                ++global_counter;
                // Возвращаем номер физической страницы
                return PPN;
            }
        }
        ++global_counter;
        // Если таблица заполнена, то вызываем обработку page fault
        return this->handle(operation, VPN);
    }

    void NFU::print() {
        std::cout << std::setw(5) << "PPN"
                  << std::setw(5) << "VPN"
                  << std::setw(5) << "R"
                  << std::setw(5) << "M"
                  << std::setw(10) << "Counter" << std::endl;  // добавляем заголовок для счетчика

        for (int i = 0; i < table.size(); ++i) {
            if (table[i]) {
                std::cout << std::setw(5) << table[i]->PPN
                          << std::setw(5) << table[i]->VPN
                          << std::setw(5) << table[i]->R.to_ulong()
                          << std::setw(5) << table[i]->M.to_ulong()
                          << std::setw(10) << nfu_table[i].counter << std::endl;  // добавляем вывод счетчика
            } else {
                std::cout << std::setw(5) << "N/A"
                          << std::setw(5) << "N/A"
                          << std::setw(5) << "N/A"
                          << std::setw(5) << "N/A"
                          << std::setw(10) << "N/A" << std::endl;  // добавляем вывод для случая отсутствующей страницы
            }
        }
        std::cout << "Global Counter: " << global_counter << std::endl;  // добавляем вывод глобального счетчика
    }

}

#endif //OS_TASK4_NISSAN_180SX_NISMO_NFU_H
