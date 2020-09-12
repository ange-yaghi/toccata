#include "../include/theme_script_language_rules.h"

#include "../include/color_node_output.h"
#include "../include/color_constructor_node.h"
#include "../include/object_channel_types.h"
#include "../include/create_setting_node.h"
#include "../include/create_profile_node.h"
#include "../include/color_split_node.h"
#include "../include/int_to_color_node.h"
#include "../include/srgbi_node.h"

toccata::LanguageRules::LanguageRules() {
    /* void */
}

toccata::LanguageRules::~LanguageRules() {
    /* void */
}

void toccata::LanguageRules::registerBuiltinNodeTypes() {
    // ====================================================
    // Builtin types
    // ====================================================

    // Channels
    registerBuiltinType<piranha::ChannelNode>(
        "int_channel", &piranha::FundamentalType::IntType);
    registerBuiltinType<piranha::ChannelNode>(
        "string_channel", &piranha::FundamentalType::StringType);
    registerBuiltinType<ColorSplitNode>(
        "color_channel", &ColorNodeOutput::ColorType);
    registerBuiltinType<piranha::ChannelNode>(
        "float_channel", &piranha::FundamentalType::FloatType);
    registerBuiltinType<piranha::ChannelNode>(
        "bool_channel", &piranha::FundamentalType::BoolType);
    registerBuiltinType<piranha::ChannelNode>(
        "setting_channel", &ObjectChannel::SettingChannel);
    registerBuiltinType<piranha::ChannelNode>(
        "profile_channel", &ObjectChannel::ProfileChannel);

    // Constructors
    registerBuiltinType<SrgbiNode>("toccata_srgb");
    registerBuiltinType<ColorConstructorNode>("toccata_color_constructor");

    registerBuiltinType<CreateProfileNode>("toccata_profile");

    registerBuiltinType<CreateSettingNode<piranha::native_bool>>("toccata_bool_setting");
    registerBuiltinType<CreateSettingNode<piranha::native_float>>("toccata_float_setting");
    registerBuiltinType<CreateSettingNode<piranha::native_int>>("toccata_int_setting");
    registerBuiltinType<CreateSettingNode<piranha::native_string>>("toccata_string_setting");
    registerBuiltinType<CreateSettingNode<ysVector>>("toccata_color_setting");

    // Literal types
    registerBuiltinType<piranha::DefaultLiteralIntNode>("literal_int");
    registerBuiltinType<piranha::DefaultLiteralStringNode>("literal_string");
    registerBuiltinType<piranha::DefaultLiteralBoolNode>("literal_bool");
    registerBuiltinType<piranha::DefaultLiteralFloatNode>("literal_float");

    // Operator types
    /* void */

    // Conversion types
    registerBuiltinType<piranha::IntToFloatConversionNode>("toccata_int_to_float");
    registerBuiltinType<IntToColorNode>("toccata_int_to_color");

    // Literals ===========================================
    registerLiteralType(piranha::LITERAL_INT, "literal_int");
    registerLiteralType(piranha::LITERAL_STRING, "literal_string");
    registerLiteralType(piranha::LITERAL_BOOL, "literal_bool");
    registerLiteralType(piranha::LITERAL_FLOAT, "literal_float");

    // Operations =========================================
    /* void */

    // Conversions ========================================
    registerConversion(
        { &piranha::FundamentalType::IntType, &piranha::FundamentalType::FloatType },
        "toccata_int_to_float"
    );

    registerConversion(
        { &piranha::FundamentalType::IntType, &ColorNodeOutput::ColorType },
        "toccata_int_to_color"
    );
}
