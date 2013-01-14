// Extensions/additions to FLTK, primarily to take advantage of C++11 features.
// 
// Major addition is lambda-compatible handler mechanisms for events and main widget callbacks.
// A pointer or reference to the widget and any other desired data can easily be bound to
// the lambda, so these callbacks do not take any parameters.
// Event handlers must return a non-zero integer to indicate that they've accepted an event.

#ifndef FLTK3UTILS_H
#define FLTK3UTILS_H

#include <fltk3/fltk3.h>

#include <functional>
#include <utility>
#include <map>
#include <list>

namespace flu {

void initialize();

// ****************************************************************************
// Interface for function pointer based callbacks
// ****************************************************************************
extern std::map<int, std::list<std::function<int()> > > globalEventHandlers;

void GenericCallback(fltk3::Widget * widget, void * data);
void CheckCallback(void * data);

// ****************************************************************************
// Lambda-compatible per-event callback interface mixin
// ****************************************************************************
template<typename BaseWidget>
class FLU: public BaseWidget {
  protected:
    std::map<int, std::function<int()> > eventHandlers;
    
  public:
    // Forward any arguments to constructors of base class
    template<typename... args_t>
    explicit FLU(args_t &&... args): BaseWidget(std::forward<args_t>(args)...) {}
    
    virtual int handle(int event) {
        auto handler = eventHandlers.find(event);
        if(handler != eventHandlers.end())
            return handler->second();
        return BaseWidget::handle(event);
    }
    
    // Callback handler takes no parameters and returns void (default return type if not specified)
    template<typename cb_t>
    void callback(const cb_t & cb) {
        auto wrapped = new std::function<void()>;
        *wrapped = cb;
        BaseWidget::callback(GenericCallback, wrapped);
    }
    
    
    // Event handlers must take a no parameters and return an int
    template<typename cb_t>
    void register_handler(int event, const cb_t & h) {eventHandlers[event] = h;}
    
    template<typename cb_t>
    void on_push(const cb_t & h) {register_handler(fltk3::PUSH, h);}
    
    template<typename cb_t>
    void on_release(const cb_t & h) {register_handler(fltk3::RELEASE, h);}
    
    template<typename cb_t>
    void on_enter(const cb_t & h) {register_handler(fltk3::ENTER, h);}
    
    template<typename cb_t>
    void on_leave(const cb_t & h) {register_handler(fltk3::LEAVE, h);}
    
    template<typename cb_t>
    void on_drag(const cb_t & h) {register_handler(fltk3::DRAG, h);}
    
    template<typename cb_t>
    void on_focus(const cb_t & h) {register_handler(fltk3::FOCUS, h);}
    
    template<typename cb_t>
    void on_unfocus(const cb_t & h) {register_handler(fltk3::UNFOCUS, h);}
    
    template<typename cb_t>
    void on_keydown(const cb_t & h) {register_handler(fltk3::KEYDOWN, h);}
    
    template<typename cb_t>
    void on_keyup(const cb_t & h) {register_handler(fltk3::KEYUP, h);}
    
    template<typename cb_t>
    void on_close(const cb_t & h) {register_handler(fltk3::CLOSE, h);}
    
    template<typename cb_t>
    void on_move(const cb_t & h) {register_handler(fltk3::MOVE, h);}
    
    template<typename cb_t>
    void on_shortcut(const cb_t & h) {register_handler(fltk3::SHORTCUT, h);}
    
    template<typename cb_t>
    void on_deactivate(const cb_t & h) {register_handler(fltk3::DEACTIVATE, h);}
    
    template<typename cb_t>
    void on_activate(const cb_t & h) {register_handler(fltk3::ACTIVATE, h);}
    
    template<typename cb_t>
    void on_hide(const cb_t & h) {register_handler(fltk3::HIDE, h);}
    
    template<typename cb_t>
    void on_show(const cb_t & h) {register_handler(fltk3::SHOW, h);}
    
    template<typename cb_t>
    void on_paste(const cb_t & h) {register_handler(fltk3::PASTE, h);}
    
    template<typename cb_t>
    void on_selectionclear(const cb_t & h) {register_handler(fltk3::SELECTIONCLEAR, h);}
    
    template<typename cb_t>
    void on_mousewheel(const cb_t & h) {register_handler(fltk3::MOUSEWHEEL, h);}
    
    template<typename cb_t>
    void on_dnd_enter(const cb_t & h) {register_handler(fltk3::DND_ENTER, h);}
    
    template<typename cb_t>
    void on_dnd_drag(const cb_t & h) {register_handler(fltk3::DND_DRAG, h);}
    
    template<typename cb_t>
    void on_dnd_leave(const cb_t & h) {register_handler(fltk3::DND_LEAVE, h);}
    
    template<typename cb_t>
    void on_dnd_release(const cb_t & h) {register_handler(fltk3::DND_RELEASE, h);}
    
    template<typename cb_t>
    void on_screen_configuration_changed(const cb_t & h) {register_handler(fltk3::SCREEN_CONFIGURATION_CHANGED, h);}
    
    template<typename cb_t>
    void on_fullscreen(const cb_t & h) {register_handler(fltk3::FULLSCREEN, h);}
};

// aliases for standard FLTK widgets
// TODO: complete these!
using Widget = FLU<fltk3::Widget>;
using Adjuster = FLU<fltk3::Adjuster>;
using Browser = FLU<fltk3::Browser>;
using Button = FLU<fltk3::Button>;
// using CairoWindow = FLU<fltk3::CairoWindow>;
using Chart = FLU<fltk3::Chart>;
using CheckBrowser = FLU<fltk3::CheckBrowser>;
using CheckButton = FLU<fltk3::CheckButton>;
using Choice = FLU<fltk3::Choice>;
using ClockOutput = FLU<fltk3::ClockOutput>;
using Clock = FLU<fltk3::Clock>;

using DoubleWindow = FLU<fltk3::DoubleWindow>;
using Group = FLU<fltk3::Group>;
using LightButton = FLU<fltk3::LightButton>;
using MultilineInput = FLU<fltk3::MultilineInput>;
using ReturnButton = FLU<fltk3::ReturnButton>;
using RoundButton = FLU<fltk3::RoundButton>;
using TextDisplay = FLU<fltk3::TextDisplay>;
using Window = FLU<fltk3::Window>;

// ****************************************************************************
// Lambda-compatible callback interface, works with any widget
// ****************************************************************************

// Use C++11 lambdas with any FLTK widget.
// 
// cb(button, [&fullscreen, window]() {
//     if(fullscreen)
//         window->fullscreen_off();
//     else
//         window->fullscreen();
//     fullscreen = !fullscreen;
// });

template<typename cb_t>
void cb(fltk3::Widget * widget, const cb_t & cb) {
    auto wrapped = new std::function<void()>;
    *wrapped = cb;
    widget->callback(GenericCallback, wrapped);
}

// Global callbacks and event handlers
template<typename cb_t>
void add_check(const cb_t & cb) {
    auto wrapped = new std::function<void()>;
    *wrapped = cb;
    fltk3::add_check(CheckCallback, wrapped);
}

template<typename cb_t>
void add_idle(const cb_t & cb) {
    auto wrapped = new std::function<void()>;
    *wrapped = cb;
    fltk3::add_idle(CheckCallback, wrapped);
}

template<typename cb_t>
void add_timeout(double t, const cb_t & cb) {
    auto wrapped = new std::function<void()>;
    *wrapped = cb;
    fltk3::add_timeout(t, CheckCallback, wrapped);
}

template<typename cb_t>
void repeat_timeout(double t, const cb_t & cb) {
    auto wrapped = new std::function<void()>;
    *wrapped = cb;
    fltk3::repeat_timeout(t, CheckCallback, wrapped);
}

// Handler must take no parameters and return int
template<typename cb_t>
void add_handler(int event, const cb_t & h) {globalEventHandlers[event].push_front(h);}

template<typename cb_t>
void on_push(const cb_t & h) {add_handler(fltk3::PUSH, h);}

template<typename cb_t>
void on_release(const cb_t & h) {add_handler(fltk3::RELEASE, h);}

template<typename cb_t>
void on_enter(const cb_t & h) {add_handler(fltk3::ENTER, h);}

template<typename cb_t>
void on_leave(const cb_t & h) {add_handler(fltk3::LEAVE, h);}

template<typename cb_t>
void on_drag(const cb_t & h) {add_handler(fltk3::DRAG, h);}

template<typename cb_t>
void on_focus(const cb_t & h) {add_handler(fltk3::FOCUS, h);}

template<typename cb_t>
void on_unfocus(const cb_t & h) {add_handler(fltk3::UNFOCUS, h);}

template<typename cb_t>
void on_keydown(const cb_t & h) {add_handler(fltk3::KEYDOWN, h);}

template<typename cb_t>
void on_keyup(const cb_t & h) {add_handler(fltk3::KEYUP, h);}

template<typename cb_t>
void on_close(const cb_t & h) {add_handler(fltk3::CLOSE, h);}

template<typename cb_t>
void on_move(const cb_t & h) {add_handler(fltk3::MOVE, h);}

template<typename cb_t>
void on_shortcut(const cb_t & h) {add_handler(fltk3::SHORTCUT, h);}

template<typename cb_t>
void on_deactivate(const cb_t & h) {add_handler(fltk3::DEACTIVATE, h);}

template<typename cb_t>
void on_activate(const cb_t & h) {add_handler(fltk3::ACTIVATE, h);}

template<typename cb_t>
void on_hide(const cb_t & h) {add_handler(fltk3::HIDE, h);}

template<typename cb_t>
void on_show(const cb_t & h) {add_handler(fltk3::SHOW, h);}

template<typename cb_t>
void on_paste(const cb_t & h) {add_handler(fltk3::PASTE, h);}

template<typename cb_t>
void on_selectionclear(const cb_t & h) {add_handler(fltk3::SELECTIONCLEAR, h);}

template<typename cb_t>
void on_mousewheel(const cb_t & h) {add_handler(fltk3::MOUSEWHEEL, h);}

template<typename cb_t>
void on_dnd_enter(const cb_t & h) {add_handler(fltk3::DND_ENTER, h);}

template<typename cb_t>
void on_dnd_drag(const cb_t & h) {add_handler(fltk3::DND_DRAG, h);}

template<typename cb_t>
void on_dnd_leave(const cb_t & h) {add_handler(fltk3::DND_LEAVE, h);}

template<typename cb_t>
void on_dnd_release(const cb_t & h) {add_handler(fltk3::DND_RELEASE, h);}

template<typename cb_t>
void on_screen_configuration_changed(const cb_t & h) {add_handler(fltk3::SCREEN_CONFIGURATION_CHANGED, h);}

template<typename cb_t>
void on_fullscreen(const cb_t & h) {add_handler(fltk3::FULLSCREEN, h);}

} // namespace flu

#endif // FLTK3UTILS_H
