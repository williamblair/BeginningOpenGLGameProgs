#ifndef GAME_TIMER_H_INCLUDED
#define GAME_TIMER_H_INCLUDED

#include <cstdint>

#include <SDL2/SDL.h>

class GameTimer
{
public:

    GameTimer() :
        lastTicks(0),
        thisTicks(0),
        ticksCounter(0),
        intervalTicks(1000*1),
        framesCount(0),
        deltaMilli(0),
        FPS(0.0f)
    {
        lastTicks = SDL_GetTicks();
    }
    
    inline float GetDeltaTimeMilli() const
    {
        return float(deltaMilli);
    }
    
    inline float GetDeltaTimeSecs() const
    {
        return GetDeltaTimeMilli()/1000.0f;
    }
    
    inline float GetFPS() const
    {
        return FPS;
    }

    void Update(int framesPassed)
    {
        framesCount += framesPassed;
        thisTicks = SDL_GetTicks();
        ticksCounter += thisTicks - lastTicks;
        deltaMilli = thisTicks - lastTicks;
        if (ticksCounter >= intervalTicks)
        {
            float secondsPassed = float(ticksCounter) / 1000.0f;
            FPS = float(framesCount) / secondsPassed;
            framesCount = 0;
            ticksCounter -= intervalTicks;
        }
        lastTicks = thisTicks;
    }

private:
    uint32_t lastTicks;
    uint32_t thisTicks;
    uint32_t ticksCounter;
    uint32_t intervalTicks;
    uint32_t framesCount;
    uint32_t deltaMilli;
    float FPS;
};

#endif // GAME_TIMER_H_INCLUDED

