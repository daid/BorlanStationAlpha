#include "view.h"

static std::vector<View*> views;

View::View()
{
    views.push_back(this);
}

View::~View()
{
    views.erase(std::remove_if(views.begin(), views.end(), [this](View* v) { return v == this; }), views.end());
}

void View::draw_views(Frontend& frontend)
{
    frontend.begin_drawing();
    for(auto v : views)
        v->draw(frontend);
    frontend.present();
}
