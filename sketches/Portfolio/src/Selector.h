#pragma once

#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

#include "common/xf/Font.h"

class Selector
{
public:
    class Handler
    {
    public:
        ~Handler() = default;
        virtual void selected(int index) = 0;
    };

    Selector() = default;
    Selector(int capacity, int selectedIndex);

    void setHandler(Handler *handler);
    void selectPrev();
    void selectNext();

    void resize(float left, float top, float height, float scale);
    bool update(float mouseX, float mouseY, bool mousePressed);
    void draw(chr::gl::IndexedVertexBatch<chr::gl::XYZ> &rectBatch, chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> &dotBatch, chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> &ringBatch);
    void draw(chr::gl::IndexedVertexBatch<chr::gl::XYZ> &rectBatch, chr::XFont &font, const std::vector<std::u16string> &text);

protected:
    int capacity;
    int selectedIndex;
    int indexOver = -1;

    float left;
    float top;
    float height;

    chr::Clock::Ref clock1 = chr::Clock::create();
    chr::Clock::Ref clock2 = chr::Clock::create();

    float trackWidth;
    float dotSize;
    float ringSize;
    float appendixSize;
    float boxSize;
    float boxPadding;
    float fontSize;

    Handler *handler = nullptr;

    glm::vec2 getCenter(int index) const;
    static void drawText(chr::XFont &font, const std::u16string &text, float x, float y, chr::XFont::Alignment alignment = chr::XFont::Alignment::ALIGN_BASELINE);
};
