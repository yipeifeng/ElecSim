#include <ElecDataStruct/Event.h>

void test1() {
    ElecData::Event* event = new ElecData::Event();

    event->getEventId();
}

int main() {

    test1();
}
