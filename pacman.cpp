//Trabalho 4 de Inf 110, Pacman ultizando allegro 5
// Professor Andre
// Integrantes: Erick, Marcelo, Savio, Yago

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>		
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdlib.h>
#include <ctime>
#include <cmath>
#include <iostream>

using namespace std;

const int atraso = 5;		// atraso para fechar o jogo automaticamente ao final da partida
const float FPS = 4;		// fps
const int SCREEN_W = 680; 	// dimensoes da tela do jogo
const int SCREEN_H = 460;

enum MYKEYS					// teclas que poderao ser usaddas para movimentacao do pacman 
{
	KEY_W,
	KEY_S,
	KEY_D,
	KEY_A,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
};

//matriz definindo mapa do jogo: 1 representa paredes, 2 piluras, 0 representa corredor
char MAPA[24][24] =
{
	"11111111111111111111111",
	"12222222222122222222221",
	"12111211112121111211121",
	"12111211112121111211121",
	"12222222222222222222221",
	"12111212111111121211121",
	"12222212222122221222221",
	"11111211112121111211111",
	"11111212222222221211111",
	"11111212111011121211111",
	"22223222100000122232222",
	"11111212100000121211111",
	"11111212111111121211111",
	"11111212222022221211111",
	"11111212111111121211111",
	"12222222222122222222221",
	"12111211112121111211121",
	"12221222222222222212221",
	"11121212111111121212111",
	"12222212222122221222221",
	"12111111112121111111121",
	"12222222222222222222221",
	"11111111111111111111111"};

ALLEGRO_DISPLAY *display = NULL;				//criacao de ponteiros para o display, a fila de eventos, timer
ALLEGRO_EVENT_QUEUE *event_queue = NULL;		// mapas, personagens, fontes e musicas
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_BITMAP *mapa = NULL;
ALLEGRO_BITMAP *pacman = NULL;
ALLEGRO_BITMAP *pacman_fechado = NULL;
ALLEGRO_BITMAP *aux = NULL;
ALLEGRO_BITMAP *ponto = NULL;
ALLEGRO_BITMAP *erick_fants = NULL;
ALLEGRO_BITMAP *marcelo_fants = NULL;
ALLEGRO_BITMAP *savio_fants = NULL;
ALLEGRO_BITMAP *yago_fants = NULL;
ALLEGRO_FONT *fonte_pacman = NULL;
ALLEGRO_FONT *fonte_nomes = NULL;
ALLEGRO_FONT *fonte_pacman_numero = NULL;
ALLEGRO_FONT *fonte_mensagem_final = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;
ALLEGRO_AUDIO_STREAM *musica_vitoria = NULL;
ALLEGRO_AUDIO_STREAM *musica_derrota = NULL;

bool estaAndando = false;		// ver se o pacman esta movimentando
int aberto = 0;					// ver se a boca esta aberta
int cont_savio = 0;
int cont_yago = 0;
int cont_erick = 0;
int cont_marcelo = 0;
int situacao_jogo = 0; 	//0 - normal, 1 - perdeu, 2 - ganhou

int i = 13, j = 11; 	//posição inicial do Pacman na matriz
int q = 20;				//tamanho de cada célula no mapa
int posy = i * q;		//posicao pacman 
int posx = j * q;
int cont_ponto = 0;		// acumulador de pontos

//Coordenadas dos fantasmas
int erick_px = 10 * q;
int erick_py = 10 * q;
int marcelo_px = 12 * q;
int marcelo_py = 10 * q;
int savio_px = 10 * q;
int savio_py = 11 * q;
int yago_px = 12 * q;
int yago_py = 11 * q;

bool key[4] = {false, false, false, false};
bool redraw = true;
bool sair = false;

//funcao para o fantasma inteligente calcular a menor distancia para o pacman
double distancia(int posPacManx, int posErickx, int posPacMany, int posEricky) 
{
	return sqrt(pow(posErickx - posPacManx, 2) + pow(posEricky - posPacMany, 2));
}

int inicializa() 	//inicializa as variaveis para o fucionamento do jogo
{					// como o teclado, audio, display...

	if (!al_init())
	{
		cout << "Falha ao carregar Allegro" << endl;
		return 0;
	}

	if (!al_install_keyboard())
	{
		cout << "Falha ao inicializar o teclado" << endl;
		return 0;
	}

	timer = al_create_timer(0.5 / FPS);
	if (!timer)
	{
		cout << "Falha ao inicializar o temporizador" << endl;
		return 0;
	}

	if (!al_install_audio())
	{
		cout << "Falha ao inicializar o Audio" << endl;
		return 0;
	}

	if (!al_init_acodec_addon())
	{
		cout << "Falha ao inicializar o codec de audio" << endl;
		return 0;
	}

	if (!al_reserve_samples(5))
	{
		cout << "Falha ao reservar amostrar de audio" << endl;
		return 0;
	}

	if (!al_init_image_addon())
	{
		cout << "Falha ao iniciar al_init_image_addon!" << endl;
		return 0;
	}

										// inicializa o display e os pesonagens, mapas, e carrega as imagens necessarias
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_DISPLAY_MODE disp_data;

	display = al_create_display(SCREEN_W, SCREEN_H);
	al_set_window_position(display, (SCREEN_W / 2.5), (SCREEN_H / 2));
	al_set_window_title(display, "Pacman - CCP 2K19");

	if (!display)
	{
		cout << "Falha ao inicializar a tela" << endl;
		al_destroy_timer(timer);
		return 0;
	}

	mapa = al_load_bitmap("tijolopac.png");
	if (!mapa)
	{
		cout << "Falha ao carregar o mapa!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(mapa, 0, 0, 0);

	pacman = al_load_bitmap("pacman_direita.tga");
	if (!pacman)
	{
		cout << "Falha ao carregar o pacman!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(pacman, posx, posy, 0);

	erick_fants = al_load_bitmap("erick_up.png");

	if (!erick_fants)
	{
		cout << "Falha ao carregar os fatasmas!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(erick_fants, erick_px, erick_py, 0);

	marcelo_fants = al_load_bitmap("marcelo-up.png");

	if (!marcelo_fants)
	{
		cout << "Falha ao carregar os fatasmas!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(marcelo_fants, marcelo_px, marcelo_py, 0);

	savio_fants = al_load_bitmap("savio-up.png");

	if (!savio_fants)
	{
		cout << "Falha ao carregar os fatasmas!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(savio_fants, savio_px, savio_py, 0);

	yago_fants = al_load_bitmap("yago-up.png");

	if (!yago_fants)
	{
		cout << "Falha ao carregar os fatasmas!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(yago_fants, yago_px, yago_py, 0);

	pacman_fechado = al_load_bitmap("pacman_fechado.tga");
	if (!pacman_fechado)
	{
		cout << "Falha ao carregar o pacman fechado!" << endl;
		al_destroy_display(display);
		return 0;
	}
	al_draw_bitmap(pacman_fechado, posx, posy, 0);

	ponto = al_load_bitmap("ponto.tga");
	if (!ponto)
	{
		cout << "Falha ao carregar os pontos no mapa!" << endl;
		al_destroy_display(display);
		return 0;
	}

	if (!al_init_font_addon())
	{
		cout << "Falha ao inicializar al_init_font_addon!" << endl;
	}

	if (!al_init_ttf_addon())
	{
		cout << "Falha ao inicializar al_init_ttf_addon!" << endl;
	}

								//fontes dos textos
	fonte_pacman = al_load_ttf_font("pac-font.ttf", 30, 0);
	fonte_pacman_numero = al_load_ttf_font("crack-font.TTF", 20, 0);
	fonte_nomes = al_load_ttf_font("pac-font.ttf", 15, 0);
	fonte_mensagem_final = al_load_ttf_font("8bits.TTF", 50, 0);
	if ((!fonte_mensagem_final) || (!fonte_nomes) || (!fonte_pacman_numero) || (!fonte_pacman))
	{
		cout << "Falha ao carregar a fonte do jogo!" << endl;
		al_destroy_display(display);
		return 0;
	}
	
								//inica o audio e carrega aletoriamente uma das duas musicas do jogo
	srand(time(NULL));					
	if(rand()%5 != 0){
		musica = al_load_audio_stream("oldtownroad.ogg", 4, 1024);
	}
	else{
		musica = al_load_audio_stream("pacman.ogg", 4, 1024);
	}
	if (!musica)
	{
		cout << "O audio nao pode ser iniciado" << endl;
	}
	al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
	al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);
	

	event_queue = al_create_event_queue(); 		//cria a fila de eventos
	if (!event_queue)
	{
		cout << "Falha ao criar a fila de eventos" << endl;
		al_destroy_display(display);
		al_destroy_timer(timer);
		return 0;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(timer);

	return 1;
}

int main(int argc, char **argv)
{												//variaveis para movimentacao dos personagens
	int estado = 0;
	int intencao = 0, intencaoAnterior = 4;		//inteçao de movimento do pacman
	double menor = 9999999;						// para o fantasma erik calcular a menor distancia para o pacman
	int qualDistMenor = 0;						// para calcular a menor distancia
	int intencao_marcelo = 1, intencao_erick = 1;  //intençao de movimento do marcelo e erick
	bool chegouParede = false, chegouParedeErick = false;	//verificador de paredes
	int para = 0, paraMarcelo = 0, cont_marcelo = 0, cont_direcao_marcelo = 0, cont_direcao_erick = 0;	//
	bool direcaomarceloCima = false, direcaomarceloBaixo = false, direcaomarceloEsquerda = false, direcaomarceloDireita = false;
	bool direcaoerickCima = false, direcaoerickBaixo = false, direcaoerickEsquerda = false, direcaoerickDireita = false;
	if (!inicializa())
		return -1;

	while (!sair)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if ((key[KEY_W] || key[KEY_UP]) && MAPA[i - 1][j] == '1') 		// muda a inetencao de movimento do pacman
		{																// dependendo da tecla presionada
			intencao = 1;
		}
		if ((key[KEY_S] || key[KEY_DOWN]) && MAPA[i - 1][j] == '1')
		{
			intencao = 2;
		}
		if ((key[KEY_A] || key[KEY_LEFT]) && MAPA[i][j - 1] == '1')
		{
			intencao = 3;
		}
		if ((key[KEY_D] || key[KEY_LEFT]) && MAPA[i][j + 1] == '1')
		{
			intencao = 4;
		}
		if (!estaAndando)
		{
			intencao = 0;
		}

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{

			if (situacao_jogo == 0)		// ver se jogo terminou
			{
				estaAndando = false;
										//rota para o savio e yago
				char andarFantSavio[200] = "31113113331133333311122222222244422111222222222444443333444444444222244334422443333333333333333333311221133112222111111111333311111222222222";
				char andarFantYago[200] = "00121121122211333333334422244333444442222222211122444444433333333334422244221122211333112221111111333";

				//movimentaçao do pacman
				if (((key[KEY_W] || estado == 1) || (intencao == 1)) && MAPA[i - 1][j] != '1')
				{													// verifica a intecao
					i--;											// manda para direcao desejada
					if (intencao == 1)								
						intencao = 0;								
					posy = i * q;									
					estado = 1;				
					estaAndando = true;								//faz o movimento ser continuo para a direcao
					pacman = al_load_bitmap("pacman_cima.tga");		// modifica a imagem para o personagem olhar para
					if (MAPA[i][j] == '2')							// direcao correta
					{
						cont_ponto += 25;							// se for pirula acrescenta 25 pontos
						MAPA[i][j] = '0';							// substituiu as pirulas para corredor
					}
				}

				if (((key[KEY_S] || estado == 2) || (intencao == 2)) && MAPA[i + 1][j] != '1')
				{
					i++;
					if (intencao == 2)
						intencao = 0;
					posy = i * q;
					estado = 2;
					estaAndando = true;
					pacman = al_load_bitmap("pacman_baixo.tga");
					if (MAPA[i][j] == '2')
					{
						cont_ponto += 25;
						MAPA[i][j] = '0';
					}
				}

				if (((key[KEY_A] || estado == 3) || (intencao == 3)) && MAPA[i][j - 1] != '1')
				{
					j--;
					if (intencao == 3)
						intencao = 0;
					if (j < 0)
						j = 22;

					posx = j * q;
					estado = 3;
					estaAndando = true;
					pacman = al_load_bitmap("pacman_esquerda.tga");
					if (MAPA[i][j] == '2')
					{
						cont_ponto += 25;
						MAPA[i][j] = '0';
					}
				}

				if (((key[KEY_D] || estado == 4) || (intencao == 4)) && MAPA[i][j + 1] != '1')
				{
					j++;
					if (intencao == 4)
						intencao = 0;
					if (j >= 23)
						j = 0;
					posx = j * q;
					estado = 4;
					estaAndando = true;
					pacman = al_load_bitmap("pacman_direita.tga");
					
					if (MAPA[i][j] == '2' || MAPA[i][j] == '3')
					{
						cont_ponto += 25;			//se for pirula adiciona a pontuacao
						if (MAPA[i][j] == '3')		//substitui os bloco de pirulas para corredor
						{
							MAPA[i][j] = '4';	
						}
						else
							MAPA[i][j] = '0';
					}
				}

				if ((posx == marcelo_px && posy == marcelo_py) || (posx == yago_px && posy == yago_py) || (posx == savio_px && posy == savio_py) || (posx == erick_px && posy == erick_py))
				{
					situacao_jogo = 1;
				}

				if (andarFantSavio[cont_savio] == '1') // movimentacao fantasma savio (fantasma por rota)
				{
					savio_py -= 20;									
					savio_fants = al_load_bitmap("savio-up.png"); 	// carrega a imagem correspondente para o lado
					redraw = true;									// correspondente que esta se movimentando 
				}
				else if (andarFantSavio[cont_savio] == '2')
				{
					savio_px -= 20;
					savio_fants = al_load_bitmap("savio-left.png");
					redraw = true;
				}
				else if (andarFantSavio[cont_savio] == '3')
				{
					savio_px += 20;
					savio_fants = al_load_bitmap("savio-right.png");
					redraw = true;
				}
				else if (andarFantSavio[cont_savio] == '4')
				{
					savio_py += 20;
					savio_fants = al_load_bitmap("savio-down.png");
					redraw = true;
				}

				cont_savio++;
				if (cont_savio == 139)
				{
					cont_savio = 29;
				}

				if (andarFantYago[cont_yago] == '1')	//movimentacao do fantasma Yago (fantasma por rota)
				{
					yago_py -= 20;								
					yago_fants = al_load_bitmap("yago-up.png");	// carrega a imagem correspondente a direçao do movimento
					redraw = true;
				}
				else if (andarFantYago[cont_yago] == '2')
				{
					yago_px -= 20;
					yago_fants = al_load_bitmap("yago-left.png");
					redraw = true;
				}
				else if (andarFantYago[cont_yago] == '3')
				{
					yago_px += 20;
					yago_fants = al_load_bitmap("yago-right.png");
					redraw = true;
				}
				else if (andarFantYago[cont_yago] == '4')
				{
					yago_py += 20;
					yago_fants = al_load_bitmap("yago-down.png");
					redraw = true;
				}

				cont_yago++;
				if (cont_yago == 101)
				{
					cont_yago = 7;
				}

				//movimentacao do fantasma marcelo (fantasma aleatorio)
				char andarFantMarcelo[20] = "02110";	// rota para sair da caixa inicial, apos as rotas sao aleatorias

				if (paraMarcelo == 0)
				{
					if (andarFantMarcelo[cont_marcelo] == '1')		
					{
						marcelo_py -= 20;
						marcelo_fants = al_load_bitmap("marcelo-up.png"); // carrega a imagem correspondente ao movimento
						redraw = true;
					}
					else if (andarFantMarcelo[cont_marcelo] == '2')
					{
						marcelo_px -= 20;
						marcelo_fants = al_load_bitmap("marcelo-left.png");
						redraw = true;
					}
					else if (andarFantMarcelo[cont_marcelo] == '3')
					{
						marcelo_px += 20;
						marcelo_fants = al_load_bitmap("marcelo-right.png");
						redraw = true;
					}
					else if (andarFantMarcelo[cont_marcelo] == '4')
					{
						marcelo_py += 20;
						marcelo_fants = al_load_bitmap("marcelo-down.png");
						redraw = true;
					}
				}
				if (cont_marcelo < 15)
				{
					cont_marcelo++;
				}
				else
				{
					paraMarcelo = 1;
				}

				if (paraMarcelo == 1) // intencao de movimento marcelo
				{

					if (intencao_marcelo == 1 && MAPA[marcelo_py / 20 - 1][marcelo_px / 20] == '1')
					{
						chegouParede = true;
					}
					if (intencao_marcelo == 2 && MAPA[marcelo_py / 20][marcelo_px / 20 - 1] == '1')
					{
						chegouParede = true;
					}
					if (intencao_marcelo == 3 && MAPA[marcelo_py / 20][marcelo_px / 20 + 1] == '1')
					{
						chegouParede = true;
					}
					if (intencao_marcelo == 4 && MAPA[marcelo_py / 20 + 1][marcelo_px / 20] == '1')
					{
						chegouParede = true;
					}

					if (chegouParede) // verifica se o caminho esta livre 
					{
						if (MAPA[marcelo_py / 20 + 1][marcelo_px / 20] == '0' || MAPA[marcelo_py / 20 + 1][marcelo_px / 20] == '2')
						{
							cont_direcao_marcelo++;
							direcaomarceloBaixo = true;
						}
						if (MAPA[marcelo_py / 20 - 1][marcelo_px / 20] == '0' || MAPA[marcelo_py / 20 - 1][marcelo_px / 20] == '2')
						{
							cont_direcao_marcelo++;
							direcaomarceloCima = true;
						}
						if (MAPA[marcelo_py / 20][marcelo_px / 20 + 1] == '0' || MAPA[marcelo_py / 20][marcelo_px / 20 + 1] == '2')
						{
							cont_direcao_marcelo++;
							direcaomarceloDireita = true;
						}
						if (MAPA[marcelo_py / 20][marcelo_px / 20 - 1] == '0' || MAPA[marcelo_py / 20][marcelo_px / 20 - 1] == '2')
						{
							cont_direcao_marcelo++;
							direcaomarceloEsquerda = true;
						}

						srand(time(NULL));		

						while (true)
						{
							intencao_marcelo = rand() % 5; // sorteia aletoriamente a direcao que ira ir
							if (intencao_marcelo == 1 && direcaomarceloCima)
							{
								break;
							}
							else if (intencao_marcelo == 2 && direcaomarceloEsquerda)
							{
								break;
							}
							else if (intencao_marcelo == 3 && direcaomarceloDireita)
							{
								break;
							}
							else if (intencao_marcelo == 4 && direcaomarceloBaixo)
							{
								break;
							}
							else
							{
								continue;
							}
						}
					}

					if (intencao_marcelo == 1)
					{
						marcelo_py -= 20;
						marcelo_fants = al_load_bitmap("marcelo-up.png");
						redraw = true;
					}
					else if (intencao_marcelo == 2)
					{
						marcelo_px -= 20;
						marcelo_fants = al_load_bitmap("marcelo-left.png");
						redraw = true;
					}
					else if (intencao_marcelo == 3)
					{
						marcelo_px += 20;
						marcelo_fants = al_load_bitmap("marcelo-right.png");
						redraw = true;
					}
					else if (intencao_marcelo == 4)
					{
						marcelo_py += 20;
						marcelo_fants = al_load_bitmap("marcelo-down.png");
						redraw = true;
					}

					chegouParede = false;  			// zera as condicionais e contadores
					direcaomarceloCima = false;
					direcaomarceloBaixo = false;
					direcaomarceloDireita = false;
					direcaomarceloEsquerda = false;
					cont_direcao_marcelo = 0;
				}

				char andarFantErick[100] = "0000000311000"; // rota para sair da caixa

				if (para == 0)
				{
					if (andarFantErick[cont_erick] == '1')
					{
						erick_py -= 20;
						erick_fants = al_load_bitmap("erick_up.png");
						redraw = true;
					}
					else if (andarFantErick[cont_erick] == '2')
					{
						erick_px -= 20;
						erick_fants = al_load_bitmap("erick_left.png");
						redraw = true;
					}
					else if (andarFantErick[cont_erick] == '3')
					{
						erick_px += 20;
						erick_fants = al_load_bitmap("erick_right.png");
						redraw = true;
					}
					else if (andarFantErick[cont_erick] == '4')
					{
						erick_py += 20;
						erick_fants = al_load_bitmap("erick_down.png");
						redraw = true;
					}
				}
				if (cont_erick < 12)
				{
					cont_erick++;
				}
				else
				{
					para = 1;
					MAPA[9][11] = '1';
				}

				if (para == 1)		//intecoes de movimentacao do fantasma erick
				{

					if (intencao_erick == 1 && MAPA[erick_py / 20 - 1][erick_px / 20] == '1')
					{
						chegouParedeErick = true;
					}
					if (intencao_erick == 2 && MAPA[erick_py / 20][erick_px / 20 - 1] == '1')
					{
						chegouParedeErick = true;
					}
					if (intencao_erick == 3 && MAPA[erick_py / 20][erick_px / 20 + 1] == '1')
					{
						chegouParedeErick = true;
					}
					if (intencao_erick == 4 && MAPA[erick_py / 20 + 1][erick_px / 20] == '1')
					{
						chegouParedeErick = true;
					}

					if (true)		// verifica se o caminho esta livre 
					{
						if (MAPA[erick_py / 20 + 1][erick_px / 20] == '0' || MAPA[erick_py / 20 + 1][erick_px / 20] == '2')
						{
							cont_direcao_erick++;
							direcaoerickBaixo = true;
						}
						if (MAPA[erick_py / 20 - 1][erick_px / 20] == '0' || MAPA[erick_py / 20 - 1][erick_px / 20] == '2')
						{
							cont_direcao_erick++;
							direcaoerickCima = true;
						}
						if (MAPA[erick_py / 20][erick_px / 20 + 1] == '0' || MAPA[erick_py / 20][erick_px / 20 + 1] == '2')
						{
							cont_direcao_erick++;
							direcaoerickDireita = true;
						}
						if (MAPA[erick_py / 20][erick_px / 20 - 1] == '0' || MAPA[erick_py / 20][erick_px / 20 - 1] == '2')
						{
							cont_direcao_erick++;
							direcaoerickEsquerda = true;
						}

						srand(time(NULL));
												//calcula a menor deistancia para o pacman
						if (direcaoerickCima)	
						{	
							double dist1 = distancia(posx / 20, erick_px / 20, posy / 20, erick_py / 20 - 1);
							if (dist1 < menor)		//verifica se e a menor distancia
							{							
								menor = dist1;			
								qualDistMenor = 1;	
							}
						}
						if (direcaoerickBaixo)
						{
							double dist2 = distancia(posx / 20, erick_px / 20, posy / 20, erick_py / 20 + 1);
							if (dist2 < menor)
							{
								menor = dist2;
								qualDistMenor = 2;
							}
						}
						if (direcaoerickEsquerda)
						{
							double dist3 = distancia(posx / 20, erick_px / 20 - 1, posy / 20, erick_py / 20);
							if (dist3 < menor)
							{
								menor = dist3;
								qualDistMenor = 3;
							}
						}
						if (direcaoerickDireita)
						{
							double dist4 = distancia(posx / 20, erick_px / 20 + 1, posy / 20, erick_py / 20);
							if (dist4 < menor)
							{
								menor = dist4;
								qualDistMenor = 4;
							}
						}

						intencaoAnterior = intencao_erick;	// guarda a intencao de movimento antiga
						// aletera a intecao de movimento de acordo com a menor rota para pacman
						if (qualDistMenor == 1 && intencaoAnterior != 4)//intencao anterior nao deixa ele volta para o ultimo movimento
						{												//assim ele nao buga nas esquinas
							intencao_erick = 1;
						}
						else if (qualDistMenor == 2 && intencaoAnterior != 1)
						{
							intencao_erick = 4;
						}
						else if (qualDistMenor == 3 && intencaoAnterior != 3)
						{
							intencao_erick = 2;
						}
						else if (qualDistMenor == 4 && intencaoAnterior != 2)
						{
							intencao_erick = 3;
						}
						else
						{
							if (direcaoerickCima && intencaoAnterior != 4)
							{
								intencao_erick = 1;
							}
							else if (direcaoerickBaixo && intencaoAnterior != 1)
							{
								intencao_erick = 4;
							}
							else if (direcaoerickDireita && intencaoAnterior != 2)
							{
								intencao_erick = 3;
							}
							else if (direcaoerickEsquerda && intencaoAnterior != 3)
							{
								intencao_erick = 2;
							}
						}
					}
											// substitui a imagem do erick de acordo com a direcao tomada por ele
					if (intencao_erick == 1)					
					{
						erick_py -= 20;
						erick_fants = al_load_bitmap("erick_up.png");
						redraw = true;
					}
					else if (intencao_erick == 2)
					{
						erick_px -= 20;
						erick_fants = al_load_bitmap("erick_left.png");
						redraw = true;
					}
					else if (intencao_erick == 3)
					{
						erick_px += 20;
						erick_fants = al_load_bitmap("erick_right.png");
						redraw = true;
					}
					else if (intencao_erick == 4)
					{
						erick_py += 20;
						erick_fants = al_load_bitmap("erick_down.png");
						redraw = true;
					}

					chegouParedeErick = false;
					direcaoerickCima = false;
					direcaoerickBaixo = false;
					direcaoerickDireita = false;
					direcaoerickEsquerda = false;
					cont_direcao_erick = 0;
					menor = 999999;
				}
				//aqui se ver a situcao do jogo
				// se o pacman estiver no mesmo espaco dos fantasmas ou o pacman comer todas as pirulas o jogo termina
				if ((posx == marcelo_px && posy == marcelo_py) || (posx == yago_px && posy == yago_py) || (posx == savio_px && posy == savio_py) || (posx == erick_px && posy == erick_py))
				{
					situacao_jogo = 1;
				}
				if (cont_ponto == 5700) // se chegar nessa pontucao o pacman ganha o jogo
				{
					situacao_jogo = 2;
				}
			}
			else if (situacao_jogo == 1) // se o pacman perder a tela de derrota aparece
			{

				cout << "DERROTA" << endl;
				al_draw_textf(fonte_mensagem_final, al_map_rgb(rand()%256,rand()%256,rand()%256), 60, 200, 0, "DERROTA!"); //escreve "derrota" na tela
				al_destroy_audio_stream(musica); 
				musica_derrota = al_load_audio_stream("audio-derrota.ogg", 4, 1024);	// musica de derrota
				al_attach_audio_stream_to_mixer(musica_derrota, al_get_default_mixer());
				al_set_audio_stream_playmode(musica_derrota, ALLEGRO_PLAYMODE_LOOP);
				al_flip_display();
				al_rest(atraso);			// atrasado antes de fechar o jogo
				al_destroy_audio_stream(musica_derrota);
				break;
			}
			else
			{								// se o pacman ganhar a tela de vitoria aparece
				cout << "VENCEU" << endl;
				al_draw_textf(fonte_mensagem_final, al_map_rgb(rand()%256,rand()%256,rand()%256), 60, 200, 0, "VITORIA!"); // escreve "vitoria" na tela
				al_destroy_audio_stream(musica);
				musica_vitoria = al_load_audio_stream("audio-vitoria.ogg", 4, 1024);	//musica da vitotia
				al_attach_audio_stream_to_mixer(musica_vitoria, al_get_default_mixer());
				al_set_audio_stream_playmode(musica_vitoria, ALLEGRO_PLAYMODE_LOOP);
				al_flip_display();
				al_rest(atraso);
				al_destroy_audio_stream(musica_vitoria);
				break;
			}
			redraw = true;
		}
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			break;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch (ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_W:
				key[KEY_W] = true;
				break;

				case ALLEGRO_KEY_S:
				key[KEY_S] = true;
				break;

				case ALLEGRO_KEY_A:
				key[KEY_A] = true;
				break;

				case ALLEGRO_KEY_D:
				key[KEY_D] = true;
				break;
				case ALLEGRO_KEY_UP:
				key[KEY_W] = true;
				break;
				case ALLEGRO_KEY_DOWN:
				key[KEY_S] = true;
				break;
				case ALLEGRO_KEY_LEFT:
				key[KEY_A] = true;
				break;
				case ALLEGRO_KEY_RIGHT:
				key[KEY_D] = true;
				break;
			}

		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch (ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_W:
				key[KEY_W] = false;
				break;

				case ALLEGRO_KEY_S:
				key[KEY_S] = false;
				break;

				case ALLEGRO_KEY_A:
				key[KEY_A] = false;
				break;

				case ALLEGRO_KEY_D:
				key[KEY_D] = false;
				break;

				case ALLEGRO_KEY_ESCAPE:
				al_destroy_audio_stream(musica);
				sair = true;
				break;
				case ALLEGRO_KEY_UP:
				key[KEY_W] = false;
				break;
				case ALLEGRO_KEY_DOWN:
				key[KEY_S] = false;
				break;
				case ALLEGRO_KEY_LEFT:
				key[KEY_A] = false;
				break;
				case ALLEGRO_KEY_RIGHT:
				key[KEY_D] = false;
				break;
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
															// desenha o mapa com os tijolos
			for (int i = 0; i < 24; i++)
			{
				for (int j = 0; j < 24; j++)
				{
					if (MAPA[i][j] == '2' || MAPA[i][j] == '3')
					{
						al_draw_bitmap(ponto, (j * q), (i * q), 0);
					}
					if (MAPA[i][j] == '1')
					{
						al_draw_bitmap(mapa, (j * q), (i * q), 0);
					}
				}
			}
			if(situacao_jogo == 0 || situacao_jogo == 2){

				if (aberto % 2 == 0 && estaAndando)				// movimento de abrir e fechar boca do pacman
				{
					al_draw_bitmap(pacman, posx, posy, 0);		// ha cada numero para fornecido pelo acumulador boca aberta
				}												// nos impares boca fechada
				else if (aberto % 2 == 1 && estaAndando)
				{
					al_draw_bitmap(pacman_fechado, posx, posy, 0);
				}
				else
				{
					al_draw_bitmap(pacman, posx, posy, 0);
				}
			}

			aberto += 1;	// inclemento do acumulador
			//desenhando todas figuras e textos
			al_draw_bitmap(erick_fants, erick_px, erick_py, 0);	
			al_draw_bitmap(marcelo_fants, marcelo_px, marcelo_py, 0);
			al_draw_bitmap(savio_fants, savio_px, savio_py, 0);
			al_draw_bitmap(yago_fants, yago_px, yago_py, 0);
			al_draw_textf(fonte_pacman, al_map_rgb(255, 255, 0), 490, 30, 0, "Pacman");
			al_draw_textf(fonte_pacman, al_map_rgb(255, 255, 0), 520, 100, 0, "1234923423");
			al_draw_textf(fonte_pacman_numero, al_map_rgb(255, 255, 255), 490, 170, 0, "- INF 110 -");
			al_draw_textf(fonte_nomes, al_map_rgb(255, 0, 0), 490, 200, 0, "Erick");
			al_draw_textf(fonte_nomes, al_map_rgb(0, 255, 255), 490, 230, 0, "Marcelo");
			al_draw_textf(fonte_nomes, al_map_rgb(255, 0, 127), 490, 260, 0, "Savio");
			al_draw_textf(fonte_nomes, al_map_rgb(255, 127, 0), 490, 290, 0, "Yago");
			al_draw_textf(fonte_pacman_numero, al_map_rgb(255,255,0), 490, 390, 0, "Pontos: %d", cont_ponto);
			if(situacao_jogo == 0){
				al_flip_display();
			}
		}
	}
	al_destroy_bitmap(mapa);				// destroi o display, a fila de eventos, bitmap's,... no final do jogo
	al_destroy_bitmap(pacman_fechado);
	al_destroy_bitmap(ponto);
	al_destroy_bitmap(pacman);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}