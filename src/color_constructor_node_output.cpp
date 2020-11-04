#include "../include/color_constructor_node_output.h"

toccata::ColorConstructorNodeOutput::ColorConstructorNodeOutput() {
    m_r = m_g = m_b = m_a = nullptr;
}

toccata::ColorConstructorNodeOutput::~ColorConstructorNodeOutput() {
    /* void */
}

void toccata::ColorConstructorNodeOutput::fullCompute(void *target) const {
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
    *output = ysMath::LoadVector((float)r_v, (float)g_v, (float)b_v, (float)a_v);
}

void toccata::ColorConstructorNodeOutput::registerInputs() {
    registerInput(&m_r);
    registerInput(&m_g);
    registerInput(&m_b);
    registerInput(&m_a);
}
