#include "../include/srgbi_node_output.h"

toccata::SrgbiNodeOutput::SrgbiNodeOutput() {
    m_r = m_g = m_b = m_a = nullptr;
}

toccata::SrgbiNodeOutput::~SrgbiNodeOutput() {
    /* void */
}

void toccata::SrgbiNodeOutput::fullCompute(void *target) const {
    piranha::FloatValueOutput *r = static_cast<piranha::FloatValueOutput *>(m_r);
    piranha::FloatValueOutput *g = static_cast<piranha::FloatValueOutput *>(m_g);
    piranha::FloatValueOutput *b = static_cast<piranha::FloatValueOutput *>(m_b);
    piranha::FloatValueOutput *a = static_cast<piranha::FloatValueOutput *>(m_a);

    piranha::native_float r_v, g_v, b_v, a_v;
    r->fullCompute(&r_v);
    g->fullCompute(&g_v);
    b->fullCompute(&b_v);
    a->fullCompute(&a_v);

    ysVector *output = reinterpret_cast<ysVector *>(target);
    *output = ysColor::srgbToLinear(
    (float)(r_v / (piranha::native_float)255.0),
        (float)(g_v / (piranha::native_float)255.0),
        (float)(b_v / (piranha::native_float)255.0),
        (float)(a_v / (piranha::native_float)255.0));
}

void toccata::SrgbiNodeOutput::registerInputs() {
    registerInput(&m_r);
    registerInput(&m_g);
    registerInput(&m_b);
    registerInput(&m_a);
}

