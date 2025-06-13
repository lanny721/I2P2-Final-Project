#include <allegro5/allegro.h>
#include <memory>
#include "Engine/GameEngine.hpp"
#include "Engine/IObject.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/IScene.hpp"
#include "Image.hpp"

namespace Engine {
    Image::Image(std::string img, float x, float y, float w, float h, float anchorX, float anchorY) : IObject(x, y, w, h, anchorX, anchorY) {
        if (Size.x == 0 && Size.y == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.x = GetBitmapWidth();
            Size.y = GetBitmapHeight();
        } else if (Size.x == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.x = GetBitmapWidth() * Size.y / GetBitmapHeight();
        } else if (Size.y == 0) {
            bmp = Resources::GetInstance().GetBitmap(img);
            Size.y = GetBitmapHeight() * Size.x / GetBitmapWidth();
        } else /* Size.x != 0 && Size.y != 0 */ {
            bmp = Resources::GetInstance().GetBitmap(img, Size.x, Size.y);
        }
    }
    void Image::Draw() const {
        if (!Visible || !bmp) return; // Check if the image is visible and loaded
        if(isArrow){
            float cx = al_get_bitmap_width(bmp.get()) / 2.0;
            float cy = al_get_bitmap_height(bmp.get()) / 2.0;
            al_draw_scaled_rotated_bitmap(
                bmp.get(),
                cx,cy,//center point of image
                Position.x - Anchor.x * GetBitmapWidth() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.x,
                Position.y - Anchor.y * GetBitmapHeight() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.y,
                2.0f, 2.0f,//scale
                angle,
                0
            );
            return;
        }
        al_draw_tinted_scaled_bitmap(bmp.get(), color, 0, 0, GetBitmapWidth(), GetBitmapHeight(),
                              Position.x - Anchor.x * GetBitmapWidth() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.x,
                              Position.y - Anchor.y * GetBitmapHeight() - followCamera * GameEngine::GetInstance().GetActiveScene()->camera.y,
                              Size.x, Size.y, 0);
    }
    int Image::GetBitmapWidth() const {
        return al_get_bitmap_width(bmp.get());
    }
    int Image::GetBitmapHeight() const {
        return al_get_bitmap_height(bmp.get());
    }
}
