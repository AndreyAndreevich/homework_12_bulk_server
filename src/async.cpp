#include "async.h"
#include "EventLoop.h"
#include "Storage.h"
#include "Writers.h"
#include <algorithm>

namespace async {

handle_t connect(std::size_t bulk) {
    handle_t handle;
    auto lmd = [&handle,&bulk](){
        auto handler = std::make_shared<Handler>(bulk);
        std::vector<std::shared_ptr<Observer>> writers = {
            std::make_shared<ConsoleWriter>(),
            std::make_shared<FileWriter>() 
        };
        writers[0]->subscribe(handler);
        writers[1]->subscribe(handler);
        int *id = new int;
        *id = Storage::Instance().addHandler(handler,writers);
        handle = static_cast<void*>(id);
    };
    EventLoop::Instance().send(EventCode::NOW,lmd);
    return handle;
}

void receive(handle_t handle, const char *data, std::size_t size) {
    if (!handle) throw (std::runtime_error("handle is nullptr"));
    std::string str = data;
    if (str.size() != size) throw (std::runtime_error("incorrect data size"));
    auto lmd = [handle,data,str,size]() mutable {
        auto& element = Storage::Instance().getHandler(*static_cast<int*>(handle));
        auto& buffer = std::get<2>(element);
        auto& handler = std::get<0>(element);
        buffer += data;
        std::stringstream stream(buffer);
        std::vector<std::string> commands;
        while (std::getline(stream, str)) {
            commands.push_back(str);
        }
        if (data[size-1] == '\n') {
            buffer.clear();
        } else {
            buffer = commands.back();
            commands.pop_back();
        }
        std::for_each(commands.begin(),commands.end(),[handler](const auto& command) {
            handler->addCommand(command);
        });
    };
    EventLoop::Instance().send(EventCode::ASYNC,lmd);
}

void disconnect(handle_t handle) {
    if (!handle) throw (std::runtime_error("handle is nullptr"));
    auto lmd = [handle](){
        auto id = static_cast<int*>(handle);
        auto handler = std::get<0>(Storage::Instance().getHandler(*id));
        handler->stop();
        Storage::Instance().removeHandler(*id);
        delete id;
    };
    EventLoop::Instance().send(EventCode::ASYNC,lmd);
}

}
