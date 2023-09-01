#include "Overlay.hpp"

void Overlay::setEnabled(bool t_enabled){
    enabled = t_enabled;
    main_panel->setVisible(enabled);
    background_panel->setVisible(enabled);
    background_panel->moveToFront();
    main_panel->moveToFront();

}

void Overlay::toggle(){
    enabled = !enabled;
    setEnabled(enabled);
}
