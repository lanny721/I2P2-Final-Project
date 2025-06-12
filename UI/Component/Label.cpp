#include <allegro5/allegro_font.h>
#include <memory>

#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Label.hpp"

namespace Engine {
    Label::Label(const std::string &text, const std::string &font, int fontSize, float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, float anchorX, float anchorY) : IObject(x, y, 0, 0, anchorX, anchorY), font(Resources::GetInstance().GetFont(font, fontSize)), Text(text), Color(al_map_rgba(r, g, b, a)) {
    }
    void Label::Draw() const {
        Engine::Point drawPos(Position.x - Anchor.x * GetTextWidth() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.x,
                              Position.y - Anchor.y * GetTextHeight() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.y);
        if (drawPos.x < -GetTextWidth() || drawPos.x > GameEngine::GetInstance().GetScreenWidth() + GetTextWidth() ||
            drawPos.y < -GetTextHeight() || drawPos.y > GameEngine::GetInstance().GetScreenHeight() + GetTextHeight()) {
            return; // Skip drawing if out of bounds.
        }
        al_draw_text(font.get(), Color, drawPos.x, drawPos.y, 0, Text.c_str());
    }
    int Label::GetTextWidth() const {
        return al_get_text_width(font.get(), Text.c_str());
    }
    int Label::GetTextHeight() const {
        return al_get_font_line_height(font.get());
    }
}
