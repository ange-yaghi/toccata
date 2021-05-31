#include "../include/component.h"

toccata::Component::Component() {
    m_engine = nullptr;
    m_shaders = nullptr;
    m_textRenderer = nullptr;

    m_settings = nullptr;

    m_parent = nullptr;
    m_activeElement = nullptr;
    m_enabled = true;
    m_visible = true;
}

toccata::Component::~Component() {
    /* void */
}

void toccata::Component::Construct() {
    /* void */
}

void toccata::Component::Initialize(
    dbasic::DeltaEngine *engine,
    dbasic::DefaultShaders *shaders,
    dbasic::TextRenderer *renderer,
    Settings *settings)
{
    Construct();
    
    m_engine = engine;
    m_shaders = shaders;
    m_textRenderer = renderer;
    m_settings = settings;

    for (Component *component : m_children) {
        component->Initialize(engine, shaders, renderer, settings);
    }
}

void toccata::Component::RenderAll() {
    if (!IsVisible()) return;

    Render();

    for (Component *component : m_children) {
        component->RenderAll();
    }
}

void toccata::Component::ProcessAll() {
    for (Component *component : m_children) {
        component->ProcessAll();
    }

    Update();
    if (InputEnabled()) ProcessInput();
}

float toccata::Component::CalculateFontSize(const std::string &text, float maxFontSize, float minFontSize, const BoundingBox &box) {
    const float width = m_textRenderer->CalculateWidth(text, maxFontSize);
    const float idealFontSize = (box.Width() / width) * maxFontSize;
    const float fontHeightCeiling = box.Height();

    if (idealFontSize > maxFontSize) return maxFontSize;
    else if (idealFontSize > fontHeightCeiling) return fontHeightCeiling;
    else if (idealFontSize < minFontSize) return minFontSize;
    else return idealFontSize;
}

void toccata::Component::RenderText(const std::string &text, const BoundingBox &box, float maxFontSize, float minFontSize) {
    const float fontSize = CalculateFontSize(text, maxFontSize, minFontSize, box);
    RenderText(text, { box.Left(), box.Bottom() }, fontSize);
}

void toccata::Component::AddChild(Component *component) {
    component->SetParent(this);
    m_children.push_back(component);
}

void toccata::Component::RequestControl() {
    m_activeElement = this;

    Component *parent = m_parent;
    while (parent != nullptr) {
        parent->m_activeElement = this;
        parent = parent->m_parent;
    }
}

void toccata::Component::ReleaseControl() {
    if (!HasControl()) return;

    m_activeElement = nullptr;

    Component *parent = m_parent;
    while (parent != nullptr) {
        parent->m_activeElement = nullptr;
        parent = parent->m_parent;
    }
}

bool toccata::Component::HasControl() const {
    const Component *parent = m_parent;
    const Component *root = this;
    while (parent != nullptr) {
        root = parent;
        parent = parent->m_parent;
    }

    return root->m_activeElement == this;
}

bool toccata::Component::InputEnabled() const {
    if (m_parent != nullptr) return m_enabled && m_parent->InputEnabled();
    else return m_enabled;
}

bool toccata::Component::IsVisible() const {
    if (m_parent != nullptr) return m_visible && m_parent->IsVisible();
    else return m_visible;
}

void toccata::Component::DrawBox(const BoundingBox &box, const ysVector &color) {
    const int sw = m_engine->GetScreenWidth();
    const int sh = m_engine->GetScreenHeight();

    m_shaders->SetBaseColor(color);
    m_shaders->SetLit(false);
    m_shaders->SetColorReplace(true);
    m_shaders->SetDiffuseTexture(nullptr);
    m_shaders->SetObjectTransform(
        ysMath::TranslationTransform(
            ysMath::LoadVector(
                box.CenterX() - sw / 2.0f,
                box.CenterY() - sh / 2.0f)));
    m_shaders->SetScale(box.Width() / 2.0f, box.Height() / 2.0f);
    m_engine->DrawBox(m_shaders->GetRegularFlags());
}

void toccata::Component::RenderText(const std::string &text, const ysVector2 &position, float textHeight) {
    const int sw = m_engine->GetScreenWidth();
    const int sh = m_engine->GetScreenHeight();

    m_textRenderer->RenderText(
        text,
        position.x - sw / 2.0f,
        position.y - sh / 2.0f,
        (float)textHeight);
}

void toccata::Component::Render() {
    /* void */
}

void toccata::Component::Update() {
    /* void */
}

void toccata::Component::ProcessInput() {
    /* void */
}
