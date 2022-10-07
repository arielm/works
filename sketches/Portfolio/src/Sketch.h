#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/Camera.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"
#include "common/TextRing.h"

#include "Selector.h"

class Sketch : public chr::CrossSketch, public Selector::Handler
{
public:
    Sketch();

    void setup() final;
    void resize() final;
    void update() final;
    void draw() final;

    void mouseMoved(float x, float y) final;
    void mouseDragged(int button, float x, float y) final;
    void mousePressed(int button, float x, float y) final;
    void mouseReleased(int button, float x, float y) final;

    void keyDown(int keyCode, int modifiers) final;
    void wheelUpdated(float offset) final;

    void selected(int index) final;

protected:
    chr::gl::Camera camera;

    chr::gl::ShaderProgram fogTextureShader;
    chr::gl::shaders::ColorShader colorShader;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;

    chr::gl::State decalState;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> decalBatch;
    std::vector<chr::gl::Texture> decalTextures;
    chr::TextRing ring;

    chr::gl::IndexedVertexBatch<chr::gl::XYZ> rectBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> dotBatch, ringBatch;
    chr::gl::Texture dotTexture, ringTexture;

    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> arrowBatch;
    chr::gl::Texture arrowTextures[2];

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::xf::Font> font1, font2;
    chr::xf::FontSequence sequence1, sequence2;

    float scale;

    int seed = 628371;
    int currentWorkIndex = 8;

    Selector selector;
    std::vector<std::u16string> workNames;
    std::vector<std::string> urls;

    chr::math::Rectf navigationRect;
    chr::math::Rectf arrowRect;
    bool navigationOver = false;
    bool arrowOver = false;

    glm::vec2 mousePosition;
    bool mouseIsPressed = false;

    void generate();
    void switchWork(int index);
    void openURL(const std::string &url);

    void drawTitle();
    void drawNavigation();
    void drawArrow();
    void loadTextures();

    static void drawText(chr::XFont &font, const std::u16string &text, float x, float y, chr::XFont::Alignment alignment = chr::XFont::Alignment::ALIGN_BASELINE);
};
