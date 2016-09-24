
#pragma once

#include <map>
#include <functional>

#include "JuceHeader.h"
#include "SynthParams.h"
#include "ListenerComponent.h"
#include "OutputLevelComponent.h"
#include "SourceNodeComponent.h"

/**
 * Parent class for all panel components that make use of the dirtyFlag of SynthParams (e.g. for automation).
 * The child components in these panels have to be registered with their corresponding parameter.
 */
class PanelBase : public Component, protected Timer
{
public:

    PanelBase(SynthParams &p) : params(p)
    {
        startTimerHz(60);
    }

    ~PanelBase()
    {
        stopTimer();
    }

protected:
    typedef std::function<void()> tHookFn;

    //==============================================================================

    /**
     * Register reference listener component with corresponding parameters that needs to be checked.
     * @param l listener component
     * @param posX x position of reference listener
     * @param posY y position of reference listener
     * @param ori reference listener orientation
     * @param sceneWidth width of the scene component in px
     * @param sceneHeight height of the scene component in px
     * @param hook callback function
     */
    void registerListener(ListenerComponent *l, Param *posX, Param *posY, Param *ori,
                          int sceneWidth, int sceneHeight, const tHookFn hook = tHookFn())
    {
        l->setSceneSize(sceneWidth, sceneHeight);
        l->updateBackgroundAngle(ori->getDefaultUI());

        listenerReg[l] = { posX, posY, ori };
        if (hook)
        {
            postUpdateHook[l] = hook;
            hook();
        }
    }

    /**
     * Check whether registered parameters of listener have been changed and synchronize its
     * position and orientation on UI.
     * Registered callback function is called whenever listener position or orientation is dirty.
     */
    void updateDirtyListener()
    {
        for (auto l2p : listenerReg)
        {
            auto itHook = postUpdateHook.find(l2p.first);

            // position
            for (int i = 0; i < 2; ++i)
            {
                if (l2p.second[i]->isUIDirty())
                {
                    l2p.first->relocate();

                    if (itHook != postUpdateHook.end())
                    {
                        itHook->second();
                    }
                }
            }

            // orientation
            if (l2p.second[2]->isUIDirty())
            {
                l2p.first->repaint();
                l2p.first->updateBackgroundAngle(l2p.second[2]->getUI());

                if (itHook != postUpdateHook.end())
                {
                    itHook->second();
                }
            }
        }
    }

    //==============================================================================

    /**
     * Register source component with corresponding parameters that needs to be checked.
     * This function also registers source's volume slider and volume parameter.
     * @param s source component
     * @param posX x position of source
     * @param posY y position of source
     * @param vol source input volume
     * @param level real time input level of the source
     * @param lock indicator solely for whether source is allowed to be moved
     * @param sceneWidth width of the scene component in px
     * @param sceneHeight height of the scene component in px
     * @param hook callback function
     */
    void registerSource(SourceNodeComponent *s, Param *posX, Param *posY, Param *vol, Param *level,
                        int sceneWidth, int sceneHeight, const tHookFn hook = tHookFn())
    {
        s->setSceneSize(sceneWidth, sceneHeight);

        registerSlider(s->getVolSlider(), vol);
        s->getVolSlider()->refreshVolLevel(level->getDefaultUI());

        sourceReg[s] = { posX, posY , vol, level };
        if (hook)
        {
            postUpdateHook[s] = hook;
            hook();
        }
    }

    /**
     * Check whether registered parameters of sources have been changed and synchronize its position on UI.
     * Registered callback function is called whenever source position is dirty.
     */
    void updateDirtySources()
    {
        for (auto s2p : sourceReg)
        {
            // position
            for (int i = 0; i < 2; ++i)
            {
                if (s2p.second[i]->isUIDirty())
                {
                    s2p.first->relocate();

                    auto itHook = postUpdateHook.find(s2p.first);
                    if (itHook != postUpdateHook.end())
                    {
                        itHook->second();
                    }
                }
            }
        }
    }

    /**
     * Update all source levels independently from audio buffer size.
     * Depends only on set timer frequency.
     */
    void updateSourceLevels()
    {
        // refresh all source levels
        for (auto s2p : sourceReg)
        {
            s2p.first->getVolSlider()->refreshVolLevel(s2p.second[3]->getUI());
        }
    }

    //==============================================================================

    /**
     * Register slider component with corresponding parameter that needs to be checked.
     * @param s slider to handle changes and synchronize UI if parameter is dirty
     * @param p parameter to register
     * @param hook callback function
     */
    void registerSlider(Slider *s, Param* p, const tHookFn hook = tHookFn())
    {
        s->setValue(p->getDefaultUI(), dontSendNotification);
        s->setTextValueSuffix(p->getUnit());

        sliderReg[s] = { p };
        if (hook)
        {
            postUpdateHook[s] = hook;
            hook();
        }
    }

    /**
     * Handle slider that was moved and set registered parameter.
     * @sliderThatWasMoved slider in the UI that was moved
     * @return true if slider is registered and could be handled
     */
    bool handleSlider(Slider* sliderThatWasMoved)
    {
        auto it = sliderReg.find(sliderThatWasMoved);
        if (it != sliderReg.end()) 
        {
            float val = static_cast<float>(sliderThatWasMoved->getValue());
            it->second->setUI(val);

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end())
            {
                itHook->second();
            }
            return true;
        }
        return false;
    }

    /**
     * Check whether registered parameters of sliders have changed and synchronize its set value on UI.
     */
    void updateDirtySliders()
    {
        for (auto s2p : sliderReg)
        {
            if (s2p.second->isUIDirty())
            {
                s2p.first->setValue(s2p.second->getUI(), dontSendNotification);

                auto itHook = postUpdateHook.find(s2p.first);
                if (itHook != postUpdateHook.end())
                {
                    itHook->second();
                }
            }
        }
    }

    //==============================================================================

    /**
     * Register button component with corresponding parameter that needs to be checked.
     * Use this to display and synchronize its button state on UI correctly.
     * @param b button to handle clicks and synchronize UI if parameter is dirty
     * @param state parameter to register
     * @param hook callback function
     */
    void registerButton(Button *b, ParamStepped<eOnOffState> *state, const tHookFn hook = tHookFn())
    {
        b->setWantsKeyboardFocus(false);
        b->setToggleState(state->getStep() == eOnOffState::eOn, dontSendNotification);

        buttonReg[b] = { state };
        if (hook)
        {
            postUpdateHook[b] = hook;
            hook();
        }
    }

    /**
     * Handle button that was clicked and set registered parameter.
     * @buttonThatWasClicked button in the UI that was clicked
     * @return true if button is registered and could be handled
     */
    bool handleButton(Button* buttonThatWasClicked)
    {
        auto it = buttonReg.find(buttonThatWasClicked);
        if (it != buttonReg.end())
        {
            bool isOn = buttonThatWasClicked->getToggleState();
            eOnOffState state;
            isOn ? state = eOnOffState::eOn : state = eOnOffState::eOff;
            it->second->setUI(static_cast<float>(state));

            auto itHook = postUpdateHook.find(it->first);
            if (itHook != postUpdateHook.end())
            {
                itHook->second();
            }
            return true;
        }
        return false;
    }

    /**
     * Check whether registered parameters of buttons have changed and synchronize its button state on UI.
     */
    void updateDirtyButtons()
    {
        for (auto b2p : buttonReg)
        {
            if (b2p.second->isUIDirty())
            {
                b2p.first->setToggleState(b2p.second->getStep() == eOnOffState::eOn, dontSendNotification);

                auto itHook = postUpdateHook.find(b2p.first);
                if (itHook != postUpdateHook.end())
                {
                    itHook->second();
                }
            }
        }
    }

    //==============================================================================

    virtual void timerCallback() override
    {
        updateDirtyListener();
        updateDirtySources();
        updateSourceLevels();

        updateDirtySliders();
        updateDirtyButtons();
    }

    SynthParams &params;
    std::map<Component*, tHookFn> postUpdateHook;
    std::map<ListenerComponent*, std::array<Param*, 3>> listenerReg;
    std::map<SourceNodeComponent*, std::array<Param*, 5>> sourceReg;
    std::map<Slider*, Param*> sliderReg;
    std::map<Button*, ParamStepped<eOnOffState>*> buttonReg;
};
