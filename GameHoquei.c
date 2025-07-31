#include "Lib/raylib.h"
#include "Lib/raymath.h"
#include <string.h>
#include <stdio.h>
#include "Hoquei/gameHoquei.h"

void gameHoquei(){
    InitWindow(1280, 720, "Air Hockey");
    InitAudioDevice();
    SetTargetFPS(60);
    /*if(!IsWindowFullscreen()){
        ToggleFullscreen();
    }*/
    GameState game;
    iniciarJogo(&game);

    while(!WindowShouldClose() && game.rodando){
        BeginDrawing();

        atualizaCoresSom(&game);
        controlarJogadores(&game);
        atualizarContador(&game);
        verificarColisoes(&game);
        animacaoFinal(&game);
        desenharJogo(&game);
        

        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
}

void iniciarJogo(GameState *game){

    game->screen_width = GetScreenWidth();
    game->screen_height = GetScreenHeight();

    game->jogador1 = (Jogador) {
        .posicao = {175, game->screen_height/2},
        .velocidade = {0.0f, 0.0f},
        .atracao = {0.0f, 0.0f},
        .cor = WHITE,
        .raio = 55.0f,
        .coefgravidade = 0.0f,
        .gols = 0
    };

    game->jogador2 = (Jogador) {
        .posicao = {game->screen_width - 175, game->screen_height/2},
        .velocidade = {0.0f, 0.0f},
        .atracao = {0.0f, 0.0f},
        .cor = WHITE,
        .raio = 55.0f,
        .coefgravidade = 0.0f,
        .gols = 0
    };

    game->bola = (Bola) {
        .posicao = {game->screen_width/2, game->screen_height/2},
        .velocidade = {0.0f, 0.0f},
        .cor = WHITE,
        .som = LoadSound("Hoquei/src_sounds/colisaometalica.wav"),
        .raio = 22.0f
    };

    game->gol1 = (Gol)  {
        .inicio = {0, (game->screen_height/2)-100},
        .fim = {0, (game->screen_height/2)+100}
    };

    game->gol2 = (Gol)  {
        .inicio = {game->screen_width, (game->screen_height/2)-100},
        .fim = {game->screen_width, (game->screen_height/2)+100}
    };

    game->divisoria = (Divisoria) {
        .inicio = {game->screen_width/2, 0},
        .fim = {game->screen_width/2, game->screen_height}
    };

    game->circuloesq = (Circulo) {
        .posicao = {0, game->screen_height/2},
        .raio = 175.0f,
    };

    game->circulomeio = (Circulo) {
        .posicao = {game->screen_width/2, game->screen_height/2},
        .raio = 175.0f,
    };

    game->circulodir = (Circulo) {
        .posicao = {game->screen_width, game->screen_height/2},
        .raio = 175.0f,
    };
    game->fadecolor = (Color) {0.0f, 0.0f, 0.0f, 0.0f};
    game->aceleracao = 1.6f;
    game->hue = 0.0f;
    game->atrito = 0.94f;
    game->vel_max = 12.0f;
    game->vel_max_bola = 2.7f*game->vel_max;
    game->opacidade = 0.0f;
    game->sobreposicao = 0.0f;
    game->proj = 0.0f;
    game->G = 400.0f;
    game->flagmusica = 1;
    game->flagcontador = 1;
    game->rodando = 1;
    game->terminou = 0;
    game->contador = 6;
    game->grossuralinha = 12;
    game->vetorcolisao = (Vector2) {0.0f, 0.0f};
    game->opacidadefade = 0.0f;
    game->colorbackground = BLACK;
    game->colortext = RAYWHITE;
    game->musicadefundo = LoadMusicStream("Hoquei/src_sounds/musicadefundo.wav");
    game->somcontagem = LoadSound("Hoquei/src_sounds/contagem.wav");
    game->volumemusicadefundo = 1.0f;
}

void atualizaCoresSom(GameState *game){
    if(game->contador == 3 && game->flagcontador){ 
        PlaySound(game->somcontagem);
        game->flagcontador = 0;
    }    
    if(game->contador==0 && game->flagmusica){
        game->flagmusica = 0;
        PlayMusicStream(game->musicadefundo);
        SetMusicVolume(game->musicadefundo, game->volumemusicadefundo);
    }
    UpdateMusicStream(game->musicadefundo);
    if(game->terminou == 0){
        if(game->opacidade<1){
            SetWindowOpacity(game->opacidade);
            game->opacidade+=0.00833;
        }
        game->colortext = ColorFromHSV(fmod(game->hue + 180.0f, 360.0f), 1.0f, 1.0f);
        game->divisoria.cor = game->colortext;
        game->circuloesq.cor = game->colortext;
        game->circulomeio.cor = game->colortext;
        game->circulodir.cor = game->colortext;
        game->gol1.cor = game->colortext;
        game->gol2.cor = game->colortext;

    game->hue += 10.0f * GetFrameTime();
    if (game->hue > 360.0f) game->hue -= 360.0f;
    }

}



void atualizarContador(GameState *game){
    game->tempoatual = GetTime();
    game->fps = GetFPS();
    if(game->tempoatual >= 2 && game->tempoatual < 3){
        game->contador = 3;
    }
    if(game->tempoatual >= 3 && game->tempoatual < 4){
        game->contador = 2;
    }
    if(game->tempoatual >= 4 && game->tempoatual < 5){
        game->contador = 1;
    }
    if(game->tempoatual > 5){
        game->contador = 0;
    }

}

void controlarJogadores(GameState *game){

    game->jogador1.posicao = Vector2Add(game->jogador1.posicao, game->jogador1.velocidade);
    game->jogador2.posicao = Vector2Add(game->jogador2.posicao, game->jogador2.velocidade);
    game->bola.posicao = Vector2Add(game->bola.posicao, game->bola.velocidade);

    if(game->terminou == 0 && game->contador == 0){
        if (IsKeyDown(KEY_W)) {
            game->jogador1.velocidade.y -= game->aceleracao;
        }
        if (IsKeyDown(KEY_A)) {
            game->jogador1.velocidade.x -= game->aceleracao;
        }
        if (IsKeyDown(KEY_S)) {
            game->jogador1.velocidade.y += game->aceleracao;
        }
        if (IsKeyDown(KEY_D)) {
            game->jogador1.velocidade.x += game->aceleracao;
        }
        if (IsKeyDown(KEY_UP)) {
            game->jogador2.velocidade.y -= game->aceleracao;
        }
        if (IsKeyDown(KEY_LEFT)) {
            game->jogador2.velocidade.x -= game->aceleracao;
        }
        if (IsKeyDown(KEY_DOWN)) {
            game->jogador2.velocidade.y += game->aceleracao;
        }
        if (IsKeyDown(KEY_RIGHT)) {
            game->jogador2.velocidade.x += game->aceleracao;
        }

        if (game->jogador1.velocidade.x > game->vel_max) game->jogador1.velocidade.x = game->vel_max;
        if (game->jogador1.velocidade.x < game->vel_max*-1) game->jogador1.velocidade.x = game->vel_max*-1;
        if (game->jogador1.velocidade.y > game->vel_max) game->jogador1.velocidade.y = game->vel_max;
        if (game->jogador1.velocidade.y < game->vel_max*-1) game->jogador1.velocidade.y = game->vel_max*-1;
        if (game->jogador2.velocidade.x > game->vel_max) game->jogador2.velocidade.x = game->vel_max;
        if (game->jogador2.velocidade.x < game->vel_max*-1) game->jogador2.velocidade.x = game->vel_max*-1;
        if (game->jogador2.velocidade.y > game->vel_max) game->jogador2.velocidade.y = game->vel_max;
        if (game->jogador2.velocidade.y < game->vel_max*-1) game->jogador2.velocidade.y = game->vel_max*-1;
        if (game->bola.velocidade.x > game->vel_max_bola) game->bola.velocidade.x = game->vel_max_bola;
        if (game->bola.velocidade.x < game->vel_max_bola*-1) game->bola.velocidade.x = game->vel_max_bola*-1;
        if (game->bola.velocidade.y > game->vel_max_bola) game->bola.velocidade.y = game->vel_max_bola;
        if (game->bola.velocidade.y < game->vel_max_bola*-1) game->bola.velocidade.y = game->vel_max_bola*-1;
        
        
        if (!IsKeyDown(KEY_D) && !IsKeyDown(KEY_A)) {
            game->jogador1.velocidade.x *= game->atrito;
            if (fabs(game->jogador1.velocidade.x) < 1.0f) game->jogador1.velocidade.x = 0;
        }
        if (!IsKeyDown(KEY_S) && !IsKeyDown(KEY_W)) {
            game->jogador1.velocidade.y *= game->atrito;
            if (fabs(game->jogador1.velocidade.y) < 1.0f) game->jogador1.velocidade.y = 0;
        }
        if (!IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
            game->jogador2.velocidade.x *= game->atrito;
            if (fabs(game->jogador2.velocidade.x) < 1.0f) game->jogador2.velocidade.x = 0;
        }
        if (!IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP)) {
            game->jogador2.velocidade.y *= game->atrito;
            if (fabs(game->jogador2.velocidade.y) < 1.0f) game->jogador2.velocidade.y = 0;
        }
    }
}

void verificarColisoes (GameState *game){
    if(game->terminou==0){
        if (CheckCollisionCircles(game->jogador1.posicao, game->jogador1.raio, game->bola.posicao, game->bola.raio)) {
            if(Vector2Distance(game->jogador1.posicao, game->bola.posicao) > 0.0f){
                game->sobreposicao = (game->jogador1.raio+game->bola.raio) - Vector2Distance(game->jogador1.posicao, game->bola.posicao);
                game->vetorcolisao = Vector2Subtract(game->bola.posicao, game->jogador1.posicao);
                game->vetorcolisao = Vector2Normalize(game->vetorcolisao);
                game->vetorcolisao = Vector2Scale(game->vetorcolisao, game->sobreposicao);
                game->jogador1.posicao = Vector2Add(game->jogador1.posicao, Vector2Scale(game->vetorcolisao, -0.5f));
                game->bola.posicao = Vector2Add(game->bola.posicao, Vector2Scale(game->vetorcolisao, 0.5f));
            }

            
            game->n = Vector2Normalize(game->vetorcolisao);
            game->v_rel = Vector2Subtract(game->bola.velocidade, game->jogador1.velocidade);
            game->proj = Vector2DotProduct(game->v_rel, game->n);
            if (game->proj < 0) {
                game->variacao = Vector2Scale(game->n, -2.0f * game->proj);
                game->bola.velocidade = Vector2Add(game->bola.velocidade, game->variacao);
            }
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
        }

        if (CheckCollisionCircles(game->jogador2.posicao, game->jogador2.raio, game->bola.posicao, game->bola.raio)) {
            if(Vector2Distance(game->jogador2.posicao, game->bola.posicao) > 0.0f){
                game->sobreposicao = (game->jogador2.raio+game->bola.raio) - Vector2Distance(game->jogador2.posicao, game->bola.posicao);
                game->vetorcolisao = Vector2Subtract(game->bola.posicao, game->jogador2.posicao);
                game->vetorcolisao = Vector2Normalize(game->vetorcolisao);
                game->vetorcolisao = Vector2Scale(game->vetorcolisao, game->sobreposicao);
                game->jogador2.posicao = Vector2Add(game->jogador2.posicao, Vector2Scale(game->vetorcolisao, -0.5f));
                game->bola.posicao = Vector2Add(game->bola.posicao, Vector2Scale(game->vetorcolisao, 0.5f));
            }
            game->n = Vector2Normalize(game->vetorcolisao);
            game->v_rel = Vector2Subtract(game->bola.velocidade, game->jogador2.velocidade);
            game->proj = Vector2DotProduct(game->v_rel, game->n);
            if (game->proj < 0) {
                game->variacao = Vector2Scale(game->n, -2.0f * game->proj);
                game->bola.velocidade = Vector2Add(game->bola.velocidade, game->variacao);
            }
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
        }
        if(CheckCollisionCircleLine(game->bola.posicao, game->bola.raio, game->gol1.inicio, game->gol1.fim)){
            game->jogador2.gols++;
            game->jogador1.posicao = (Vector2) {game->jogador1.raio+20, game->screen_height/2};
            game->jogador2.posicao = (Vector2) {game->screen_width-(game->jogador2.raio+20), game->screen_height/2};
            game->bola.posicao = (Vector2) {1*game->screen_width/4, game->screen_height/2};
            game->jogador1.velocidade = (Vector2) {0.0f, 0.0f};
            game->jogador2.velocidade = (Vector2) {0.0f, 0.0f};
            game->bola.velocidade = (Vector2) {0.0f, 0.0f};
        }
        if(CheckCollisionCircleLine(game->bola.posicao, game->bola.raio, game->gol2.inicio, game->gol2.fim)){
            game->jogador1.gols++;
            game->jogador1.posicao = (Vector2) {game->jogador1.raio+20, game->screen_height/2};
            game->jogador2.posicao = (Vector2) {game->screen_width-(game->jogador2.raio+20), game->screen_height/2};
            game->bola.posicao = (Vector2) {3*game->screen_width/4, game->screen_height/2};
            game->jogador1.velocidade = (Vector2) {0.0f, 0.0f};
            game->jogador2.velocidade = (Vector2) {0.0f, 0.0f};
            game->bola.velocidade = (Vector2) {0.0f, 0.0f};
        }

        if (game->bola.posicao.x + game->bola.raio >= game->screen_width) {
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
            game->bola.posicao.x = game->screen_width - game->bola.raio;
            game->bola.velocidade.x *= -0.8f;
        }
        if (game->bola.posicao.x - game->bola.raio <= 0) {
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
            game->bola.posicao.x = game->bola.raio;
            game->bola.velocidade.x *= -0.8f;
        }
        if (game->bola.posicao.y + game->bola.raio >= game->screen_height) {
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
            game->bola.posicao.y = game->screen_height - game->bola.raio;
            game->bola.velocidade.y *= -0.8f;
        }
        if (game->bola.posicao.y - game->bola.raio <= 0) {
            SetSoundVolume(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f));
            SetSoundPitch(game->bola.som, Clamp(Vector2Length(game->bola.velocidade)/game->vel_max, 0.0f, 1.0f)+0.5f);
            PlaySound(game->bola.som);
            game->bola.posicao.y = game->bola.raio;
            game->bola.velocidade.y *= -0.8f;
        }
        if (game->jogador1.posicao.x + game->jogador1.raio >= game->divisoria.inicio.x) {
            game->jogador1.posicao.x = game->divisoria.inicio.x - game->jogador1.raio;
            game->jogador1.velocidade.x *= -1;
        }
        if (game->jogador1.posicao.x - game->jogador1.raio <= 0) {
            game->jogador1.posicao.x = game->jogador1.raio;
            game->jogador1.velocidade.x *= -1;
        }
        if (game->jogador1.posicao.y + game->jogador1.raio >= game->screen_height) {
            game->jogador1.posicao.y = game->screen_height - game->jogador1.raio;
            game->jogador1.velocidade.y *= -1;
        }
        if (game->jogador1.posicao.y - game->jogador1.raio <= 0) {
            game->jogador1.posicao.y = game->jogador1.raio;
            game->jogador1.velocidade.y *= -1;
        }
        if (game->jogador2.posicao.x + game->jogador2.raio >= game->screen_width) {
            game->jogador2.posicao.x = game->screen_width - game->jogador2.raio;
            game->jogador2.velocidade.x *= -1;
        }
        if (game->jogador2.posicao.x - game->jogador2.raio <= game->divisoria.inicio.x) {
            game->jogador2.posicao.x = game->divisoria.inicio.x + game->jogador2.raio;
            game->jogador2.velocidade.x *= -1;
        }
        if (game->jogador2.posicao.y + game->jogador2.raio >= game->screen_height) {
            game->jogador2.posicao.y = game->screen_height - game->jogador2.raio;
            game->jogador2.velocidade.y *= -1;
        }
        if (game->jogador2.posicao.y - game->jogador2.raio <= 0) {
            game->jogador2.posicao.y = game->jogador2.raio;
            game->jogador2.velocidade.y *= -1;
        }
    }


}

void desenharJogo(GameState *game){
    ClearBackground(game->colorbackground);

    DrawCircleV(game->circuloesq.posicao, game->circuloesq.raio, game->circuloesq.cor);
    DrawCircleV(game->circuloesq.posicao, game->circuloesq.raio - game->grossuralinha, game->colorbackground);

    DrawCircleV(game->circulomeio.posicao, game->circulomeio.raio, game->circulomeio.cor);
    DrawCircleV(game->circulomeio.posicao, game->circulomeio.raio-game->grossuralinha, game->colorbackground);

    DrawCircleV(game->circulodir.posicao, game->circulodir.raio, game->circulodir.cor);
    DrawCircleV(game->circulodir.posicao, game->circulodir.raio-game->grossuralinha, game->colorbackground);

    DrawLineEx(game->divisoria.inicio, game->divisoria.fim, game->grossuralinha, game->divisoria.cor);

    DrawCircleV(game->jogador1.posicao, game->jogador1.raio, game->jogador1.cor);
    DrawCircleV(game->jogador1.posicao, 0.6f*game->jogador1.raio, game->colorbackground);
    
    DrawCircleV(game->bola.posicao, game->bola.raio, game->bola.cor);
    
    DrawCircleV(game->jogador2.posicao, game->jogador2.raio, game->jogador2.cor);
    DrawCircleV(game->jogador2.posicao, 0.6f*game->jogador2.raio, game->colorbackground);
    
    DrawLineEx(game->gol1.inicio, game->gol1.fim, game->grossuralinha, game->gol1.cor);
    DrawLineEx(game->gol2.inicio, game->gol2.fim, game->grossuralinha, game->gol2.cor);

    sprintf(game->tempo, "TEMPO ATUAL: %.2f", game->tempoatual);
    DrawText(game->tempo, game->screen_width / 2 - MeasureText(game->tempo, 10) / 2, 
        game->screen_height - 20, 10, RAYWHITE);

    sprintf(game->placar, "%d %d", game->jogador1.gols, game->jogador2.gols);
    DrawText(game->placar, game->screen_width/2 - MeasureText(game->placar, 80) / 2, 
        20, 80, game->colortext);

    sprintf(game->textfps, "FPS: %d", game->fps);
    DrawText(game->textfps, game->screen_width - 100, 20, 20, game->colortext);

    if(game->contador>0 && game->contador<=3){
        sprintf(game->contregressiva, "%d", game->contador);
        DrawText(game->contregressiva, game->screen_width/2 - MeasureText(game->contregressiva, 150)/2, game->screen_height/2, 150, RAYWHITE);
    }
    if(game->jogador1.gols == 7){
        sprintf(game->vencedor, "JOGADOR 1 VENCEU!!!");
        DrawText(game->vencedor, game->screen_width/2  - MeasureText(game->vencedor, 80)/2, game->screen_height/2, 80, RAYWHITE);
    }
    if(game->jogador2.gols == 7){
        sprintf(game->vencedor, "JOGADOR 2 VENCEU!!!");
        DrawText(game->vencedor, game->screen_width/2 - MeasureText(game->vencedor, 80)/2, game->screen_height/2 , 80, RAYWHITE);    
    }

    DrawRectangle(0,0,game->screen_width, game->screen_height, game->fadecolor);
}
void animacaoFinal(GameState *game){
    if (game->terminou==0 && (game->jogador1.gols == 7 || game->jogador2.gols == 7)){
        game->jogador1.posicao = (Vector2) {game->screen_width/2 - 175, game->screen_height/2};
        game->jogador2.posicao = (Vector2) {game->screen_width/2 + 175, game->screen_height/2};
        game->bola.posicao = (Vector2) {game->screen_width/2, game->screen_height/2};
        game->colorbackground = BLACK;
        game->colortext = RAYWHITE;
        game->jogador1.cor = ORANGE;
        game->jogador2.cor = ORANGE;
        game->bola.cor = YELLOW;
        game->gol1.cor = RED;
        game->gol2.cor = RED;
        game->divisoria.cor = game->colorbackground;
        game->circuloesq.cor = game->colorbackground;
        game->circulomeio.cor = game->colorbackground;
        game->circulodir.cor = game->colorbackground; 
        game->terminou = 1;
        game->jogador1.velocidade = Vector2Rotate(Vector2Subtract(game->bola.posicao, game->jogador1.posicao), PI/2.0f);        
        game->jogador2.velocidade = Vector2Rotate(Vector2Subtract(game->bola.posicao, game->jogador2.posicao), PI/2.0f);
        game->jogador1.velocidade = Vector2Scale(Vector2Normalize(game->jogador1.velocidade), 2);
        game->jogador2.velocidade = Vector2Scale(Vector2Normalize(game->jogador2.velocidade), 2);
        
    }
    if(game->terminou){
        game->fadecolor = Fade(BLACK, game->opacidadefade);
        game->opacidadefade+=0.003f;
        game->volumemusicadefundo-=0.0032f;
        if(game->volumemusicadefundo<=0.0f) game->volumemusicadefundo = 0.0f;
        SetMusicVolume(game->musicadefundo, game->volumemusicadefundo);
        game->jogador1.coefgravidade = GetFrameTime() * game->G * 1/ Vector2Length(Vector2Subtract(game->bola.posicao, game->jogador1.posicao));
        game->jogador2.coefgravidade = GetFrameTime() * game->G * 1/ Vector2Length(Vector2Subtract(game->bola.posicao, game->jogador2.posicao));
        game->jogador1.atracao = Vector2Scale(Vector2Normalize(Vector2Subtract(game->bola.posicao, game->jogador1.posicao)), game->jogador1.coefgravidade);
        game->jogador2.atracao = Vector2Scale(Vector2Normalize(Vector2Subtract(game->bola.posicao, game->jogador2.posicao)), game->jogador2.coefgravidade);
        game->jogador1.velocidade = Vector2Add(game->jogador1.velocidade, game->jogador1.atracao);
        game->jogador2.velocidade = Vector2Add(game->jogador2.velocidade, game->jogador2.atracao);
        if(game->opacidadefade>1.5f){
            UnloadSound(game->bola.som);
            UnloadMusicStream(game->musicadefundo);
            game->rodando = 0;
        }
    }
}

