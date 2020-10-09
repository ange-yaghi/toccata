#ifndef TOCCATA_UI_SETTINGS_H
#define TOCCATA_UI_SETTINGS_H

#include "profile.h"

namespace toccata {

#define SETTING(name) FillSetting(&name, #name, profile, defaultProfile)
#define HEAT_MAP(name) FillHeatMap(&name, #name, profile, defaultProfile)

    struct Settings {
        int BarDisplay_MinimumChannelCount = 3;
        ysVector BarDisplay_BackgroundColor = ysMath::Constants::Zero;

        ysVector MidiDisplay_AccidentalChannelColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_NaturalChannelColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_ChannelLineColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_BarStartLineColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_BarEndLineColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_MissedNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_IgnoredNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_DefaultNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_UnmappedPlayedNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_MappedPlayedNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_DefaultPlayedNoteColor = ysMath::Constants::Zero;
        ysVector MidiDisplay_CursorColor = ysMath::Constants::Zero;

        HeatMap *MidiDisplay_VelocityHeatMap = nullptr;
        HeatMap *MidiDisplay_TimingHeatMap = nullptr;

        int PieceDisplay_MinimumChannelCount = 0;
        ysVector PieceDisplay_BackgroundColor = ysMath::Constants::Zero;

        ysVector Button_ClickedColor = ysMath::Constants::Zero;
        ysVector Button_HoldColor = ysMath::Constants::Zero;
        ysVector Button_HoverColor = ysMath::Constants::Zero;
        ysVector Button_DefaultColor = ysMath::Constants::Zero;

        ysVector Toggle_CheckedHoldColor = ysMath::Constants::Zero;
        ysVector Toggle_CheckedHoverColor = ysMath::Constants::Zero;
        ysVector Toggle_UncheckedHoldColor = ysMath::Constants::Zero;
        ysVector Toggle_UncheckedHoverColor = ysMath::Constants::Zero;
        ysVector Toggle_CheckedColor = ysMath::Constants::Zero;
        ysVector Toggle_UncheckedColor = ysMath::Constants::Zero;

        ysVector NumericInput_InnerColor = ysMath::Constants::Zero;
        ysVector NumericInput_EnabledOuterColor = ysMath::Constants::Zero;
        ysVector NumericInput_DisabledOuterColor = ysMath::Constants::Zero;

        template <typename T_Setting>
        void FillSetting(T_Setting *setting, const std::string &name, Profile *profile, Profile *defaultProfile) {
            if (profile == nullptr || !profile->GetSetting(name, setting)) {
                if (defaultProfile == nullptr) return;

                defaultProfile->GetSetting(name, setting);
            }
        }

        void FillHeatMap(HeatMap **target, const std::string &name, Profile *profile, Profile *defaultProfile) {
            HeatMap *heatMap = nullptr;
            if (profile != nullptr) heatMap = profile->GetHeatMap(name);
            if (heatMap == nullptr) {
                if (defaultProfile != nullptr) heatMap = defaultProfile->GetHeatMap(name);
            }

            *target = heatMap;
        }

        void Fill(Profile *profile, Profile *defaultProfile) {
            SETTING(BarDisplay_MinimumChannelCount);
            SETTING(BarDisplay_BackgroundColor);

            HEAT_MAP(MidiDisplay_VelocityHeatMap);
            HEAT_MAP(MidiDisplay_TimingHeatMap);

            SETTING(MidiDisplay_AccidentalChannelColor);
            SETTING(MidiDisplay_NaturalChannelColor);
            SETTING(MidiDisplay_ChannelLineColor);
            SETTING(MidiDisplay_BarStartLineColor);
            SETTING(MidiDisplay_BarEndLineColor);
            SETTING(MidiDisplay_MissedNoteColor);
            SETTING(MidiDisplay_IgnoredNoteColor);
            SETTING(MidiDisplay_DefaultNoteColor);
            SETTING(MidiDisplay_UnmappedPlayedNoteColor);
            SETTING(MidiDisplay_MappedPlayedNoteColor);
            SETTING(MidiDisplay_DefaultPlayedNoteColor);
            SETTING(MidiDisplay_CursorColor);

            SETTING(PieceDisplay_MinimumChannelCount);
            SETTING(PieceDisplay_BackgroundColor);

            SETTING(Button_ClickedColor);
            SETTING(Button_DefaultColor);
            SETTING(Button_HoldColor);
            SETTING(Button_HoverColor);

            SETTING(Toggle_CheckedHoldColor);
            SETTING(Toggle_CheckedHoverColor);
            SETTING(Toggle_UncheckedHoldColor);
            SETTING(Toggle_UncheckedHoverColor);
            SETTING(Toggle_CheckedColor);
            SETTING(Toggle_UncheckedColor);

            SETTING(NumericInput_InnerColor);
            SETTING(NumericInput_EnabledOuterColor);
            SETTING(NumericInput_DisabledOuterColor);
        }
    };

} /* namespace toccata */

#endif /* TOCCATA_UI_SETTINGS_H */
