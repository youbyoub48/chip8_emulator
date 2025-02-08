#include <SDL2/SDL.h>   

template<typename T>
constexpr T WIDTHSCREEN{ 64*8 };

template<typename T>
constexpr T HEIGHTSCREEN{ 32*8 };

template<typename T>
constexpr T TOTAL_POINTS{ 5000 };


int main(int argc, char* argv[])
{
     if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        return EXIT_FAILURE;
    }

     SDL_Window* pWindow{ nullptr };
    SDL_Renderer* pRenderer{ nullptr };

     if (SDL_CreateWindowAndRenderer(WIDTHSCREEN<unsigned int>, HEIGHTSCREEN<unsigned int>, SDL_WINDOW_SHOWN, &pWindow, &pRenderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }


    SDL_Event events;
    bool isOpen{ true };

    // Définition du rectangle 
    SDL_Rect rectangle{ WIDTHSCREEN<int> / 2, HEIGHTSCREEN<int> / 2, 8, 8 };


     while (isOpen)
    {
         while (SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                isOpen = false;
                break;
            }
        }


        SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255); 
        SDL_RenderClear(pRenderer);  

        // Dessin du rectangle
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        SDL_RenderFillRect(pRenderer, &rectangle);

        SDL_RenderPresent(pRenderer);  
    }



    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}