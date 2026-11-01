#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

#include "AAListOptionsPopup.h"
#include "../../Utilities/AAResizeUI.h"

class UISizeOptionsPanel : public AAListOptionsPopup
{
public:
    explicit UISizeOptionsPanel (AAResizeUI& resizeUIToUse)
        : AAListOptionsPopup ("UI SIZE"), resizeUI (resizeUIToUse)
    {
    }

    void updateItems()
    {
        juce::Array<AAListOptionsPopup::Item> sizeOptions;

        for (const auto& sizeOption : resizeUI.getSizeOptions())
        {
            const auto isCurrentSize = juce::approximatelyEqual (sizeOption.scale, resizeUI.getCurrentScale());

            sizeOptions.add ({sizeOption.name, [this, newScale = sizeOption.scale]
                              {
                                  if (juce::approximatelyEqual (newScale, resizeUI.getCurrentScale()))
                                      return;

                                  resizeUI.setCurrentScale (newScale);

                                  if (onSizeChanged != nullptr)
                                      onSizeChanged();
                              }, true, isCurrentSize});
        }

        setItems (sizeOptions);
    }

    std::function<void()> onSizeChanged;

private:
    AAResizeUI& resizeUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UISizeOptionsPanel)
};
