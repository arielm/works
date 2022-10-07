#include "Sketch.h"

#include "chr/Random.h"
#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Sprite.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float SIZE = 900;

static constexpr float MARGIN = 20;
static constexpr float BUTTON_PADDING = 10;
static constexpr float ARROW_SIZE = 120;

static const string TEXTURES[10] =
{
    "WB.jpg",
    "SEED.jpg",
    "HEBREUX.jpg",
    "TWITTER.jpg",
    "WAR.jpg",
    "MAPPING.jpg",
    "SCRIPTORIUM.jpg",
    "BEHIND.jpg",
    "SKETCHBOOK.jpg",
    "BABEL.jpg"
};

Sketch::Sketch()
:
fogTextureShader(InputSource::resource("FogTextureShader.vert"), InputSource::resource("FogTextureShader.frag"))
{
    workNames =
    {
        u"A day on the West Bank",
        u"The seed",
        u"Lui les Hébreux moi Pharaon",
        u"Twitter maze",
        u"The war of the words",
        u"Mapping, augmented",
        u"Javascriptorium",
        u"Javascriptorium, behind the scenes",
        u"Sketchbook on the book",
        u"Babel tower"
    };

    urls =
    {
        "WB.html",
        "SEED.html",
        "HEBREUX.html",
        "TWITTER.html",
        "WAR.html",
        "MAPPING.html",
        "SCRIPTORIUM.html",
        "BEHIND.html",
        "SKETCHBOOK.html",
        "BABEL.html"
    };

    selector = Selector(workNames.size(), currentWorkIndex);
    selector.setHandler(this);
}

void Sketch::setup()
{
    #if defined(CHR_PLATFORM_EMSCRIPTEN)
        EM_ASM(
        {
            document.getElementById('canvas').addEventListener('mouseup', function(evt)
            {
                if (Module.url != null)
                {
                    window.open(Module.url, '_self');
                    Module.url = null;
                }
            }, false);
        });
    #endif

    // ---

    for (auto path : TEXTURES)
    {
        decalTextures.emplace_back(Texture::ImageRequest(path)
                                       .setMipmap(true)
                                       .setAnisotropy(true)
                                       .setFlags(image::FLAGS_POT));
    }

    DecalProperties decalProperties;
    decalProperties.shader = fogTextureShader;
    decalProperties.color = { 1, 1, 1, 1 };
    decalProperties.texture = decalTextures[currentWorkIndex];

    ring.setDecalProperties(decalProperties);
    decalProperties.assign(decalState, decalBatch);

    // ---

    loadTextures();

    rectBatch
        .setShader(colorShader)
        .setShaderColor(1, 1, 1, 1);

    dotBatch
        .setShader(textureAlphaShader)
        .setShaderColor(1, 1, 1, 1)
        .setTexture(dotTexture);

    ringBatch
        .setShader(textureAlphaShader)
        .setShaderColor(1, 1, 1, 1)
        .setTexture(ringTexture);

    arrowBatch
        .setShader(textureAlphaShader)
        .setShaderColor(1, 1, 1, 1);

    // ---

    font1 = fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties2d());
    font1->setShader(textureAlphaShader);

    font2 = fontManager.getFont(InputSource::resource("Helvetica_Bold_64.fnt"), XFont::Properties2d());
    font2->setShader(textureAlphaShader);

    // ---

    generate();

    // ---

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    scale = windowInfo.height / SIZE;

    camera
        .setFov(45)
        .setClip(1, 1000)
        .setWindowSize(windowInfo.size);
}

void Sketch::update()
{
    selector.resize(MARGIN * scale, 80 * scale, windowInfo.height - (80 + MARGIN) * scale, scale);

    bool selectorOver = selector.update(mousePosition.x, mousePosition.y, mouseIsPressed);
    navigationOver = navigationRect.contains(mousePosition);
    arrowOver = arrowRect.contains(mousePosition);

    if (mouseIsPressed && !selectorOver)
    {
        if (navigationOver)
        {
            openURL("about.html");
        }
        else if (arrowOver)
        {
            openURL(urls[currentWorkIndex]);
        }
    }

    mouseIsPressed = false;
}

void Sketch::draw()
{
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---

    float fogDensity = fmax(0.0025f, 0.025f - clock()->getTime() * 0.5f * (0.025f - 0.0025f));

    camera.getViewMatrix()
        .setIdentity()
        .scale(1, -1, 1)
        .translate(0, 0, -250)
        .rotateX(15 * D2R)
        .rotateY(clock()->getTime() * 0.4f);

    decalState
        .setShaderMatrix(camera.getViewProjectionMatrix())
        .setShaderUniform("u_fogDensity", fogDensity)
        .setShaderUniform("u_fogColor", glm::vec3(0, 0, 0))
        .apply();
    decalBatch.flush();

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(0, windowInfo.height)
        .scale(1, -1);

    gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    //

    rectBatch.clear();
    dotBatch.clear();
    ringBatch.clear();
    font1->beginSequence(sequence1);

    selector.draw(rectBatch, dotBatch, ringBatch);
    selector.draw(rectBatch, *font1, workNames);

    rectBatch.flush();
    dotBatch.flush();
    ringBatch.flush();
    font1->endSequence();
    font1->replaySequence(sequence1);

    //

    drawTitle();
    drawNavigation();
    drawArrow();
}

void Sketch::mouseMoved(float x, float y)
{
    mousePosition = {x, y};
}

void Sketch::mouseDragged(int button, float x, float y)
{
    mouseMoved(x, y);
}

void Sketch::mousePressed(int button, float x, float y)
{
    mouseMoved(x, y);
    mouseIsPressed = true;
}

void Sketch::mouseReleased(int button, float x, float y)
{
    mouseIsPressed = false;
}

void Sketch::keyDown(int keyCode, int modifiers)
{
    switch (keyCode)
    {
        case chr::KEY_UP:
            selector.selectPrev();
            break;

        case chr::KEY_DOWN:
            selector.selectNext();
            break;
    }
}

void Sketch::wheelUpdated(float offset)
{
    if (offset > 0)
    {
        selector.selectNext();
    }
    else if (offset < 0)
    {
        selector.selectPrev();
    }
}

void Sketch::generate()
{
    float width = ring.decalProperties.texture.innerWidth;
    float height = ring.decalProperties.texture.innerHeight;

    decalBatch.clear();

    Random rnd1(seed);
    Random rnd2(seed);
    Random rnd3(seed);
    Random rnd4(seed);

    float r = 15;

    for (int i = 0; i < 30; i++)
    {
        float y = rnd1.nextFloat(-75, + 50);
        float h = 20 + rnd2.nextFloat(50);
        r += rnd3.nextFloat(7.5f);
        float a = (width / height) * h / r;
        float zoom = h / height;

        ring.decalProperties.zoom = zoom;
        ring.drawDecal(decalBatch, Matrix().rotateY(rnd4.nextFloat(TWO_PI)), 0, y, h, r, a, 5, 0, -y);
    }
}

void Sketch::switchWork(int index)
{
    currentWorkIndex = index;

    ring.decalProperties.texture = decalTextures[currentWorkIndex];
    decalBatch.setTexture(ring.decalProperties.texture);

    generate();
}

void Sketch::selected(int index)
{
    switchWork(index);
}

void Sketch::openURL(const string &url)
{
    #if defined(CHR_PLATFORM_EMSCRIPTEN)
        EM_ASM_ARGS({
            Module.url = UTF8ToString($0);
        }, url.c_str());
    #endif
}

void Sketch::drawTitle()
{
    font1->beginSequence(sequence1);

    font1->setSize(24 * scale);
    font1->setColor(1, 1, 1, 1);
    drawText(*font1, u"ariel malka / designer + programmer / webgl portfolio", MARGIN * scale, MARGIN * scale, XFont::ALIGN_TOP);

    font1->endSequence();
    font1->replaySequence(sequence1);
}

void Sketch::drawNavigation()
{
    rectBatch.clear();
    font2->beginSequence(sequence2);

    font2->setSize(20 * scale);
    font2->setColor(navigationOver ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

    auto text = u"about";
    float advance = font2->getStringAdvance(text);

    float textX = windowInfo.width - advance - (MARGIN + BUTTON_PADDING) * scale;
    float textY = (BUTTON_PADDING + MARGIN / 2) * scale;
    navigationRect = math::Rectf(textX - BUTTON_PADDING * scale, 0, advance + (BUTTON_PADDING * 2) * scale, (MARGIN + BUTTON_PADDING * 2) * scale);

    if (!navigationOver)
    {
        draw::Rect()
            .setBounds(navigationRect)
            .append(rectBatch);
    }

    drawText(*font2, text, textX, textY, XFont::ALIGN_MIDDLE);

    rectBatch.flush();
    font2->endSequence();
    font2->replaySequence(sequence2);
}

void Sketch::drawArrow()
{
    arrowRect = math::Rectf(windowInfo.width - (MARGIN + ARROW_SIZE) * scale, (windowInfo.height - ARROW_SIZE * scale) / 2, ARROW_SIZE * scale, ARROW_SIZE * scale);

    arrowBatch
        .setTexture(arrowOver ? arrowTextures[1] : arrowTextures[0])
        .clear();

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(arrowBatch, Matrix().translate(arrowRect.center()).scale(scale));

    arrowBatch.flush();
}

void Sketch::loadTextures()
{
    dotTexture = Texture(
        Texture::ImageRequest("dot_112.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));

    ringTexture = Texture(
        Texture::ImageRequest("ring_112.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));

    arrowTextures[0] = Texture(
        Texture::ImageRequest("arrow_up.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));

    arrowTextures[1] = Texture(
        Texture::ImageRequest("arrow_down.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));
}

void Sketch::drawText(chr::XFont &font, const u16string &text, float x, float y, XFont::Alignment alignment)
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