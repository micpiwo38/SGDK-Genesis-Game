#include <genesis.h>
#include <resources.h>

// Import des sprites = ref pointeur vers le fichier resources
Sprite *player;
Sprite *bloc;
Sprite *coin;

// Deplacement
int player_x = 120;
int player_y = 146;
int player_speed = 3;
int player_direction = 0;

// Player anlimation => 5
#define Animation_Player_IDLE 0
#define Animation_Player_RIGHT 1
#define Animation_Player_LEFT 2
#define Animation_Player_JUMP 3
#define Animation_Player_DIE 4

// Les controles
void manette(u16 joy, u16 changed, u16 state)
{
	//
	if (joy == JOY_1 || joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
		{
			player_direction = 1;
		}
		else if (state & BUTTON_RIGHT)
		{
			player_direction = 2;
		}
		else
		{
			player_direction = 0;
		}
	}
}

// Deplacer player sprite
void move_player()
{
	if (player_direction == 1 && player_x >= 16) // bloc 16 a gauche = 0 + 16
	{
		player_x -= player_speed;
	}
	else if (player_direction == 2 && player_x <= 288) // 320 - 16
	{
		player_x += player_speed;
	}
	else
	{
	}
	// MAJ Sprite Player
	SPR_setPosition(player, player_x, player_y);
}

//------------------------------------------BOUCLE DE GAMEPLAY---------------------------------------//
int main()
{
	// Init de Video VDP et Sprite SPR
	VDP_setScreenWidth320();
	VDP_init();
	SPR_init();
	JOY_init();
	JOY_setEventHandler(&manette);

	// Charger et afficher une image de fond
	VDP_loadTileSet(level_one_sprite.tileset, TILE_USER_INDEX, DMA);
	// Decompresser l'image
	TileMap *map_level_one = unpackTileMap(level_one_sprite.tilemap, NULL);

	//
	VDP_setTileMapEx(BG_B, map_level_one, TILE_ATTR_FULL(PAL3, 0, FALSE, FALSE, TILE_USER_INDEX), 0, 0, 0, 0, 40, 24, CPU);

	// Le player sprite est un pointeur (adresse memeoire) -> SPR_addSprite a besoin de la reference de l'adresse memoire pour lier le fichier png de resources.res au fichier c
	//  Charger les sprites (adresse memoire du sprite, x, y, attribut de la tuile(la palette,?,?,?))
	player = SPR_addSprite(&player_sprite, player_x, player_y, TILE_ATTR(PAL0, 0, 0, 0));
	bloc = SPR_addSprite(&bloc_sprite, 30, 50, TILE_ATTR(PAL1, 0, 0, 0));
	coin = SPR_addSprite(&coin_sprite, 100, 100, TILE_ATTR(PAL2, 0, 0, 0));

	// Charger les palettes (4 palettes)
	PAL_setPalette(PAL0, player_sprite.palette->data, CPU);
	PAL_setPalette(PAL1, bloc_sprite.palette->data, CPU);
	PAL_setPalette(PAL2, coin_sprite.palette->data, CPU);
	PAL_setPalette(PAL3, level_one_sprite.palette->data, CPU);

	VDP_drawText("SUper jeu de Mic!", 10, 13);

	while (1)
	{
		SPR_update();
		move_player();
		VDP_waitVSync();

		SYS_doVBlankProcess();
	}
	return (0);
}
