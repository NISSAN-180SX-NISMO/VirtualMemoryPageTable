//
// Created by User on 21.04.2024.
//

#ifndef OS_TASK4_NISSAN_180SX_NISMO_PAGEFAULTHANDLERINTERFACE_H
#define OS_TASK4_NISSAN_180SX_NISMO_PAGEFAULTHANDLERINTERFACE_H

#include <memory>
#include <bitset>

class PageFaultHandlerInterface {
public:
    virtual ~PageFaultHandlerInterface() = default;
    virtual int handle(std::bitset<1> operation, int VPN) = 0;
    virtual int frame_request(std::bitset<1> operation, int VPN) = 0;  // метод для запроса реальной страницы
};

#endif //OS_TASK4_NISSAN_180SX_NISMO_PAGEFAULTHANDLERINTERFACE_H
