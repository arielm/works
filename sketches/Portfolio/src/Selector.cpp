#include "Selector.h"

#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Sprite.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float DOT_RADIUS_PIXELS = 56; // SPECIFIC TO "dot_112"
static constexpr float TRACK_WIDTH = 4;
static constexpr float DOT_SIZE = 16;
static constexpr float RING_SIZE = 32;
static constexpr float APPENDIX_SIZE = 24;
static constexpr float BOX_SIZE = 32;
static constexpr float BOX_PADDING = 8;
static constexpr float FONT_SIZE = 16;
static constexpr float SELECTION_DELAY = 0.125f;

Selector::Selector(int capacity, int selectedIndex)
:
capacity(capacity),
selectedIndex(selectedIndex)
{
    clock1->start();
}

void Selector::setHandler(Handler *handler)
{
    this->handler = handler;
}

void Selector::selectPrev()
{
    if (clock2->getState() == Clock::STARTED && clock2->getTime() < SELECTION_DELAY)
    {
        return;
    }
    else
    {
        clock2->restart();
    }

    selectedIndex--;
    if (selectedIndex < 0)
    {
        selectedIndex = capacity - 1;
    }

    clock1->restart();

    if (handler)
    {
        handler->selected(selectedIndex);
    }
}

void Selector::selectNext()
{
    if (clock2->getState() == Clock::STARTED && clock2->getTime() < SELECTION_DELAY)
    {
        return;
    }
    else
    {
        clock2->restart();
    }

    selectedIndex++;
    if (selectedIndex >= capacity)
    {
        selectedIndex = 0;
    }

    clock1->restart();

    if (handler)
    {
        handler->selected(selectedIndex);
    }
}

void Selector::resize(float left, float top, float height, float scale)
{
    this->left = left;
    this->top = top;
    this->height = height;

    trackWidth = scale * TRACK_WIDTH;
    dotSize = scale * DOT_SIZE;
    ringSize = scale * RING_SIZE;
    appendixSize = scale * APPENDIX_SIZE;
    boxSize = scale * BOX_SIZE;
    boxPadding = scale * BOX_PADDING;
    fontSize = scale * FONT_SIZE;
}

bool Selector::update(float mouseX, float mouseY, bool mousePressed)
{
    bool over = math::Rectf(left + (dotSize - ringSize) / 2, top + (dotSize - ringSize) / 2, ringSize, height + (ringSize - dotSize)).contains(mouseX, mouseY);

    indexOver = -1;
    for (int i = 0; i < capacity; i++)
    {
        auto center = getCenter(i);
        auto rectangle = math::Rectf(center.x - ringSize / 2, center.y - ringSize / 2, ringSize, ringSize);

        if (rectangle.contains(mouseX, mouseY))
        {
            indexOver = i;
            break;
        }
    }

    if (indexOver != -1)
    {
        if (mousePressed)
        {
            selectedIndex = indexOver;
            clock1->restart();

            if (handler)
            {
                handler->selected(selectedIndex);
            }
        }
    }

    return over;
}

void Selector::draw(IndexedVertexBatch<XYZ> &rectBatch, IndexedVertexBatch<XYZ.UV> &dotBatch, IndexedVertexBatch<XYZ.UV> &ringBatch)
{
    draw::Rect()
        .setBounds(left + (dotSize - trackWidth) / 2, top, trackWidth, height)
        .append(rectBatch);

    for (int i = 0; i < capacity; i++)
    {
        draw::Sprite()
            .setAnchor(0.5f, 0.5f)
            .append(dotBatch, Matrix().translate(getCenter(i)).scale(0.5f * dotSize / DOT_RADIUS_PIXELS));
    }

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(ringBatch, Matrix().translate(getCenter(selectedIndex)).scale(0.5f * ringSize / DOT_RADIUS_PIXELS));
}

void Selector::draw(IndexedVertexBatch<XYZ> &rectBatch, XFont &font, const vector<u16string> &text)
{
    int index = (clock1->getState() == Clock::STARTED && clock1->getTime() < 2.0f) ? selectedIndex : indexOver;
    if (index != -1)
    {
        font.setSize(fontSize);
        font.setColor(0, 0, 0, 1);

        auto center = getCenter(index);

        draw::Rect()
            .setBounds(center.x, center.y - trackWidth / 2, appendixSize, trackWidth)
            .append(rectBatch);

        float boxWidth = font.getStringAdvance(text[index]) + boxPadding * 2;

        draw::Rect()
            .setBounds(center.x + appendixSize, center.y - boxSize / 2, boxWidth, boxSize)
            .append(rectBatch);

        drawText(font, text[index], center.x + appendixSize + boxPadding, center.y, XFont::ALIGN_MIDDLE);
    }
}

glm::vec2 Selector::getCenter(int index) const
{
    float u = index / (float)(capacity - 1);
    return {left + dotSize / 2, top + u * (height - dotSize) + dotSize / 2};
}

void Selector::drawText(chr::XFont &font, const u16string &text, float x, float y, XFont::Alignment alignment)
{
    float offset = font.getOffsetY(alignment);

    for (auto c : text)
    {
        int glyphIndex = font.getGlyphIndex(c);
        float w = font.getGlyphAdvance(glyphIndex);

        if (glyphIndex >= 0)
        {
            font.addGlyph(glyphIndex, x, y + offset);
        }

        x += w;
    }
}
