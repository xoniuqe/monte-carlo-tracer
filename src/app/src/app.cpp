#include<iostream>
#include "SDL.h"

int main(int argc, char * argv[]) {
	std::cout << "Hallo Welt" << std::endl;
	// SDL mit dem Grafiksystem initialisieren.
    	if(SDL_Init(SDL_INIT_VIDEO) == -1)
    	{
        	// Ups, das hat nicht funktioniert!
        	// Wir geben die Fehlermeldung aus.
        	std::cerr << "Konnte SDL nicht initialisieren! Fehler: " << SDL_GetError() << std::endl;
        	return 1;
    	}
	SDL_Window* window = SDL_CreateWindow("Monte Carlo Raytracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
	if(window == NULL) {
		//std::cerr << "window is null! << SDl_GetError() << std::endl;
		return 1;
	}
	
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	SDL_Event event;
	while(true) {
	     	while(SDL_PollEvent( &event ) != 0) {

        		//User requests quit
        		if( event.type == SDL_QUIT ){
				return 0;       
        		} 
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	        if (SDL_RenderClear(renderer)!=0) {
        	    std::cout << SDL_GetError() << std::endl;
        	}		
		SDL_RenderPresent(renderer);
	}
	return 0;
}
