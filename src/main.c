#include <genesis.h>
#include <resources.h>
#include <string.h>

// Le menu
bool game_start = FALSE;
char version[22] = "MIC GAME V1.0.0";
char start_message[22] = "--- PRESS START ---";

// Import des sprites = ref pointeur vers le fichier resources
Sprite *player;
Sprite *bloc;
Sprite *coin;

// Deplacement Player
int player_x = 120;
fix16 player_y = FIX16(146); // Float fix en C = non a virgule constant = economie de resource + precis qu'un int
int player_speed = 3;
int player_direction = 0;
int prev_player_direction = 0; // Pour eviter de redemarrer l'animation
// Saut du player
fix16 player_y_velocity = FIX16(0); // Par defaut la velocité en Y est un float fix de 0
const int floor_height = 146;
fix16 player_gravity = FIX16(0.5); // vitesse de chute apres le saut = gravité
bool player_jump = FALSE;

// Player anlimation => 5
#define Animation_Player_IDLE 0
#define Animation_Player_RIGHT 1
#define Animation_Player_LEFT 2
#define Animation_Player_JUMP 3
#define Animation_Player_DIE 4

//------------------------------------------------------FUNCTIONS---------------------------------------//

// Menu = game_start = FALSE
void delete_start_message()
{
	// Position X et Y du texte + nombre de char[]
	VDP_clearText(12, 25, 22); // Texte de la version
	VDP_clearText(10, 27, 22); // Test PRESS START
}
//-----------------------------------------------------------CONTROLLERS-----------------------------------//
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
		// Le saut
		if (state & BUTTON_A || state & BUTTON_B || state & BUTTON_C)
		{
			if (player_jump == FALSE)
			{
				// Changer l'etat = la condition evite le saut infinis
				player_jump = TRUE;
				player_y_velocity = FIX16(-6);
				// Animation de saut au debut
				SPR_setAnim(player, Animation_Player_JUMP);
			}
		}

		// Bouton start lance le jeu
		if (state & BUTTON_START)
		{
			game_start = TRUE;
			delete_start_message();
		}
	}
}
//-----------------------------------------------PLAYER-------------------------------------//
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

	// Changer animation seulement si direction change et pas en saut
	if (player_direction != prev_player_direction && player_jump == FALSE)
	{
		if (player_direction == 1)
			SPR_setAnim(player, Animation_Player_LEFT);
		else if (player_direction == 2)
			SPR_setAnim(player, Animation_Player_RIGHT);
		else
			SPR_setAnim(player, Animation_Player_IDLE);
		prev_player_direction = player_direction;
	}

	// MAJ Sprite Player = parser le float a  int en Y
	SPR_setPosition(player, player_x, F16_toInt(player_y));
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

	VDP_drawText(version, 12, 25);
	VDP_drawText(start_message, 10, 27);

	while (1)
	{
		// MAJ des sprites
		SPR_update();
		// SI menu ou jeu
		if (game_start == FALSE)
		{
			SPR_setVisibility(player, HIDDEN);
		}
		else
		{
			SPR_setVisibility(player, VISIBLE);
			move_player();
			// Le saut
			if (player_jump == TRUE)
			{
				player_y += player_y_velocity;
				player_y_velocity += player_gravity;
			}
			// Apres un saut si le player touche le sol
			if (player_jump == TRUE && F16_toInt(player_y) >= floor_height)
			{
				player_jump = FALSE;
				// Reset de la velocité
				player_y_velocity = FIX16(0);
				// Remettre animation idle
				SPR_setAnim(player, Animation_Player_IDLE);
			}
		}

		VDP_waitVSync();

		SYS_doVBlankProcess();
	}
	return (0);
}
