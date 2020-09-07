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

	protected:
		virtual piranha::Node *generateInterface();
	};

} /* namespace toccata */

#endif /* TOCCATA_UI_COLOR_NODE_OUTPUT_H */
