#include <SharedData.hpp>

std::map<std::string, void *> SharedData::sharedDataMap;

//void example() {
//    SharedData::SetData("data1", 1);
//    SharedData::SetData<float>("data2", 2);
//    auto data2 = SharedData::GetData<float>("data2");
//    SharedData::AddCallback<void()>("callback", [&data2]{
//        data2 = 1;
//    });
//    SharedData::InvokeCallback<void()>("callback");
//    if(SharedData::HasData("data1")) SharedData::RemoveData<int>("data1");
//}