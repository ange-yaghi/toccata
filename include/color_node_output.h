#ifndef TOCCATA_UI_COLOR_NODE_OUTPUT_H
#define TOCCATA_UI_COLOR_NODE_OUTPUT_H

#include "delta.h"

#include <piranha.h>

namespace toccata {

	class ColorNodeOutput : public piranha::NodeOutput {
	public:
		static const piranha::ChannelType ColorType;

	public:
		ColorNodeOutput();
		ColorNodeOutput(const piranha::ChannelType *channelType);
		virtual ~ColorNodeOutput();

		virtual void fullCompute(void *target) const;

		void SetValue(const ysVector &value) { m_value = value; }
		ysVector GetValue() const { return m_value; }

	protected:
		virtual piranha::Node *generateInterface();

		ysVector m_value;
	};

} /* namespace toccata */

#endif /* TOCCATA_UI_COLOR_NODE_OUTPUT_H */
