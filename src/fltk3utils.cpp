
#include "fltk3utils.h"

namespace flu {

std::map<int, std::list<std::function<int()> > > globalEventHandlers;

// ****************************************************************************
// Functions for interfacing with function pointer based callbacks
// ****************************************************************************

void GenericCallback(fltk3::Widget * widget, void * data) {
    auto cb = static_cast<std::function<void()> *>(data);
    (*cb)();
}

void CheckCallback(void * data) {
    auto cb = static_cast<std::function<void()> *>(data);
    (*cb)();
}

// This handler's a bit different, as FLTK doesn't allow user data to be passed along.
// So we make our own handler list, and make a separate one for each event type while we're at it.
int HandlerCallback(int event)
{
    int retval = 0;
    for(auto h : globalEventHandlers[event]) {
        retval = h();
        if(retval)
            break;
    }
    return retval;
}

void initialize()
{
    fltk3::add_handler(HandlerCallback);
}

} // namespace flu
