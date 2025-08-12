#include <stdlib.h>
#include <time.h>
#include "Lib/raylib.h"
#include "Sorteio/sorteio.h"
#include <stdio.h>
#include <math.h>   

static const char* nome_do_jogo(int id){
    return (id == JOGO_GOLF) ? "GOLF" :
           (id == JOGO_HOQUEI) ? "HOQUEI" :
           (id == JOGO_MEMORIA) ? "MEMORIA" : "DESCONHECIDO";
}

void embaralhar_ints(int *v, size_t n){
    for(size_t i = n - 1; i > 0; --i){
        size_t j = (size_t)(rand() % (int)(i + 1));
        int tmp = v[i]; v[i] = v[j]; v[j] = tmp;
    }
}

void sorteia_ordem_ids(int ordem[3]){
    int base[3] = { JOGO_GOLF, JOGO_HOQUEI, JOGO_MEMORIA };
    static int seeded = 0;
    if(!seeded){ srand((unsigned)time(NULL)); seeded = 1; }
    embaralhar_ints(base, 3);
    ordem[0] = base[0]; ordem[1] = base[1]; ordem[2] = base[2];
}

void sorteia_ordem_funcoes(int (*ordem[3])(void)){
    int ids[3]; sorteia_ordem_ids(ids);
    for(int i = 0; i < 3; ++i){
        ordem[i] = (ids[i] == JOGO_GOLF)   ? gameGolf
                 : (ids[i] == JOGO_HOQUEI) ? gameHoquei
                 :                           gameMemoria;
    }
}

void tela_sorteio_preview(int ordem[3]){
    sorteia_ordem_ids(ordem);
    const int LARGURA = 960, ALTURA = 540;
    InitWindow(LARGURA, ALTURA, "Sorteio dos Minijogos");

    // se for usar imagem
    // 1) coloca os arquivos na pasta Sorteio/assets/:
    //    Sorteio/assets/sorteio_golf.jpg   (ou .png se for png)
    //    Sorteio/assets/sorteio_hoquei.png
    //    Sorteio/assets/sorteio_memoria.png
    // 2) as linhas abaixo ja carregam dessas paths
    Texture2D img_golf   = LoadTexture("Sorteio/assets/sorteio_golf.png");
    Texture2D img_hoquei = LoadTexture("Sorteio/assets/sorteio_hoquei.png");
    Texture2D img_mem    = LoadTexture("Sorteio/assets/sorteio_memoria.png");
    bool tem_texturas = img_golf.id && img_hoquei.id && img_mem.id;

    SetTargetFPS(60);
    float timer = 0.0f;
    const float passo = 1.0f; // 1s pra revelar cada posição
    int reveladas = 0;
    float extra = 0.0f;

    while (!WindowShouldClose()){
        timer += GetFrameTime();
        if (reveladas < 3 && timer >= passo){ reveladas++; timer = 0.0f; }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("ORDEM SORTEADA", 20, 20, 28, DARKGRAY);
        DrawText("coloca oz print no assets/ e descomenta no codigo.", 20, 54, 16, GRAY);

        int w = LARGURA - 80, h = ALTURA - 140;
        int slotW = w/3 - 10, x0 = 40, y0 = 100;

        for(int i = 0; i < 3; ++i){
            int x = x0 + i*(slotW + 15);
            Rectangle r = { (float)x, (float)y0, (float)slotW, (float)h };
            DrawRectangleRoundedLines(r, 0.08f, 8, GRAY);

            if(i < reveladas){
                int id = ordem[i];

                if(tem_texturas){
                    Texture2D *tex = (id == JOGO_GOLF) ? &img_golf
                                      : (id == JOGO_HOQUEI) ? &img_hoquei : &img_mem;
                    float scale = fminf(r.width/tex->width, r.height/tex->height);
                    int dw = (int)(tex->width*scale), dh = (int)(tex->height*scale);
                    DrawTexturePro(*tex,
                                   (Rectangle){0,0,(float)tex->width,(float)tex->height},
                                   (Rectangle){r.x + (r.width-dw)/2, r.y + (r.height-dh)/2, (float)dw, (float)dh},
                                   (Vector2){0,0}, 0.0f, WHITE);
                } else {
                    Color c = (id == JOGO_GOLF) ? (Color){  0,161, 89,255}
                             : (id == JOGO_HOQUEI)? (Color){ 31, 97,141,255}
                             :                       (Color){154, 72,193,255};
                    DrawRectangleRounded(r, 0.08f, 8, Fade(c, 0.35f));
                    const char* nome = nome_do_jogo(id);
                    int tw = MeasureText(nome, 32);
                    DrawText(nome, x + (slotW - tw)/2, y0 + h/2 - 16, 32, BLACK);
                }

                char pos[8]; snprintf(pos, sizeof(pos), "#%d", i+1);
                DrawText(pos, x + 6, y0 + 6, 24, DARKGRAY);
            } else {
                DrawRectangleRounded(r, 0.08f, 8, Fade(LIGHTGRAY, 0.5f));
                int tw = MeasureText("???", 28);
                DrawText("???", x + slotW/2 - tw/2, y0 + h/2 - 14, 28, DARKGRAY);
                char pos[8]; snprintf(pos, sizeof(pos), "#%d", i+1);
                DrawText(pos, x + 6, y0 + 6, 24, DARKGRAY);
            }
        }
        EndDrawing();

        if(reveladas >= 3){
            extra += GetFrameTime();
            if(extra > 1.2f) break;  // espera ~1.2s e fecha
        }
    }

    if(tem_texturas){ UnloadTexture(img_golf); UnloadTexture(img_hoquei); UnloadTexture(img_mem); }
    CloseWindow();
}
