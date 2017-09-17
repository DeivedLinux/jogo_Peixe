#include "GameManager.h"


Board* create_board(SDL_Surface** tileSprite){
	Board* board;
	board = (Board*)malloc(sizeof(Board));

	board->width = BOARD_WIDTH;
	board->height = BOARD_HEIGHT;
	board->sprite = tileSprite;

	board->spaces = (int**)malloc(board->width * sizeof(int*));
	for(int i = 0; i < board->width; i++){
		board->spaces[i] = (int*)malloc(board->height * sizeof(int));
		for(int j = 0; j < board->height; j++){
			board->spaces[i][j] = 0;
		}		
	}

	return board;
}

Settings* create_settings(){
	Settings* settings;
	settings = (Settings*)malloc(sizeof(Settings));

	//Default settings
	//Shark
	settings->amountSharks = 10;
	settings->captureRange = 10;
	settings->sharkLife = 50;
	settings->sharkTimeLapseToReproduction = 40;
	//Fish
	settings->amountFishes = 10;
	settings->perceptionRange = 5;
	settings->fishLife = 50;
	settings->fishTimeLapseToReproduction = 30;
	settings->isTrailDrawable = 1;

	return settings;
}

void create_game(SDL_Surface** tileSprite){

	board = create_board(tileSprite);	
	settings = create_settings();

	printf("Insira a quantidade de tubaroes: ");
	scanf("%i", &settings->amountSharks);

	printf("Insira a quantidade de peixes: ");
	scanf("%i", &settings->amountFishes);


	int wantExtraSettings = 0;
	printf("(1)=sim / (0)=nao. Mostrar opcoes extras? ");
	scanf("%i", &wantExtraSettings);

	if(wantExtraSettings){

		printf("Insira a range de captura dos tubaroes: ");
		scanf("%i", &settings->captureRange);

		printf("Insira a range de percepcao dos peixes: ");
		scanf("%i", &settings->perceptionRange);

		printf("Insira o tempo de amadurecimento para a reprodução dos tubaroes: ");
		scanf("%i", &settings->sharkTimeLapseToReproduction);

		printf("Insira o tempo de amadurecimento para a reprodução dos peixes: ");
		scanf("%i", &settings->fishTimeLapseToReproduction);

		printf("(1)=sim / (0)=nao. Mostrar rastros?");
		scanf("%i", &settings->isTrailDrawable);
	}
}

