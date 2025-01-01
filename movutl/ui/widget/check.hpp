#pragma once

#include <movutl/ui/ui.hpp>
#include <movutl/ui/widget.hpp>
namespace mu::ui {

class uiCheckbox : public uiWidget{
private:
	bool *value;
	bool last_value;
    std::string text;
public:
	uiCheckbox() = delete;
	uiCheckbox(const uiCheckbox&) = delete;
	uiCheckbox& operator=(const uiCheckbox&) = delete;
    uiCheckbox(std::string text_, bool *value_);
	void render() override;
	bool CallbackFunc(uiCallbackFlags flag, Vec2d vec2_1, int num_1, int num_2, const char **strings) override;
};

inline uiWidget* check(const std::string text_, bool* v) { return (new uiCheckbox(text_, v)); }

} // namespace mu::ui
