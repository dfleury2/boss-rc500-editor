#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>

#include <iostream>
#include <tuple>

using namespace std;

/*
template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};
 
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
    template <size_t i>
    using arg_t = std::tuple_element_t<i, std::tuple<Args...>>;
};

template <typename F>
auto getFreeCallback(F f)
{
    return [](function_traits<F>::arg_t<0> data) {
        auto& f = *static_cast<F*>(data->UserData);
        return f(data);
    };
}
*/


class MyWindow : public Fl_Double_Window
{
public:
    MyWindow(int w, int h, const char* title)
        : Fl_Double_Window(w, h, "Simple Button")
    {
        auto bt = new Fl_Button(5, 5, 125, 25, "Simple Button");
    
        auto f = [](Fl_Widget* data) {
            cout << "button press" << endl; 
            //data->color(128);
        };
    
        bt->callback(f);
    }

};


const int red_color = 128;

int main()
{
    MyWindow win(400, 300, "Simple Button");
    
    win.show();

    return Fl::run();
}
