#include <time.h>
#include "raylib.h" //Pesquisando achei essa biblioteca que ja usa funções do prórpio windows
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    Vector2 pos;  // Esse tipo vem da biblioteca "raylib" / A struct que define Vector tem float para armazenar "x" e "y"
    Vector2 veloci;
    float raio;
    Color cor;  //Esse tipo vem da biblioteca "raylib" / A struct que define Color define as cores com base nos valores de char para "r","g","b","a"
    float massa;
} Bola;

float ProdutoEscalar(Vector2 vetor1, Vector2 vetor2)
{
    return vetor1.x*vetor2.x+vetor1.y*vetor2.y;
}

int ResolveColisaoBolas(Bola *bola_1, Bola *bola_2)
{
    float dist_centro_x, dist_centro_y, distancia_centros, sobrepos, normal_x, normal_y, peTan1, peTan2, peNorm2,peNorm1, massa_1, massa_2, momento_1, momento_2;

    dist_centro_x = bola_2->pos.x - bola_1->pos.x;
    dist_centro_y = bola_2->pos.y - bola_1->pos.y;
    distancia_centros = sqrtf(dist_centro_x*dist_centro_x + dist_centro_y*dist_centro_y);

    //Verifica sobreposição
    if (distancia_centros<(bola_1->raio+bola_2->raio))
    {

        //Em pesquisas encontrei que é importante fazer essa verificação para as bolas não ficarem sobrepostas
        sobrepos = (bola_1->raio+bola_2->raio)-distancia_centros;
        normal_x = dist_centro_x/distancia_centros;
        normal_y = dist_centro_y/distancia_centros;

        bola_1->pos.x -= normal_x*sobrepos*0.5f;
        bola_1->pos.y -= normal_y*sobrepos*0.5f;
        bola_2->pos.x += normal_x*sobrepos*0.5f;
        bola_2->pos.y += normal_y*sobrepos*0.5f;

        Vector2 normal = {normal_x, normal_y};
        Vector2 tangente = {-normal_y, normal_x};

        peTan1 = ProdutoEscalar(bola_1->veloci, tangente);
        peTan2 = ProdutoEscalar(bola_2->veloci, tangente);

        peNorm1 = ProdutoEscalar(bola_1->veloci, normal);
        peNorm2 = ProdutoEscalar(bola_2->veloci, normal);

        massa_1 = bola_1->massa;
        massa_2 = bola_2->massa;

        momento_1 = (peNorm1*(massa_1-massa_2)+2.0f*massa_2*peNorm2)/(massa_1+massa_2);
        momento_2 = (peNorm2*(massa_2-massa_1)+2.0f*massa_1*peNorm1)/(massa_1+massa_2);

        //Atualiza velocidades
        bola_1->veloci.x = tangente.x*peTan1+normal.x*momento_1;
        bola_1->veloci.y = tangente.y*peTan1+normal.y*momento_1;
        bola_2->veloci.x = tangente.x*peTan2+normal.x*momento_2;
        bola_2->veloci.y = tangente.y*peTan2+normal.y*momento_2;

        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{

    int num_bolas, altura_janela, largura_janela, temp_exc, cont_coli_parede=0, cont_coli_bolas=0, i, j;
    float raio_bola, vel_max; //Essas váriaveis são Float pois irei associar à struct "bola" (Vector e Raio são Float)
    float temp_corrido = 0.0, temp_corrido_frame;

    printf("Escolha o numero de bolas:\n");
    scanf("%d", &num_bolas);
    printf("Escolha a altura da janela:\n");
    scanf("%d", &altura_janela);
    printf("Escolha a largura da janela:\n");
    scanf("%d", &largura_janela);
    printf("Escolha o raio das bolas:\n");
    scanf("%f", &raio_bola);
    printf("Escolha a velocidade maxima das bolas:\n");
    scanf("%f", &vel_max);
    printf("Escolha o tempo de execussao:\n");
    scanf("%d", &temp_exc);

    //Necessário para a inicialização da Janela quando se usa "Raylib"
    InitWindow(largura_janela, altura_janela, "Simulacao de Colisoes - Bilhar");
    SetTargetFPS(60); //Preseta a janela para rodar a 60 frames

    Bola *bolas = (Bola *)malloc(num_bolas * sizeof(Bola));
    srand(time(NULL));

    for (i=0; i< num_bolas; i++)
    {
        bolas[i].raio = raio_bola;
        bolas[i].massa = raio_bola;

        //Gera as bolas em posição aleatória e garante que não vá nascer dentro da parede
        bolas[i].pos.x = GetRandomValue((int)bolas[i].raio, largura_janela - (int)bolas[i].raio);
        bolas[i].pos.y = GetRandomValue((int)bolas[i].raio, altura_janela - (int)bolas[i].raio);

        //Inicializa as bolas com velocidade aleatória
        bolas[i].veloci.x = (float)(GetRandomValue(-vel_max, vel_max));
        bolas[i].veloci.y = (float)(GetRandomValue(-vel_max, vel_max));

        bolas[i].cor = (Color){ GetRandomValue(50, 255), GetRandomValue(50, 255), GetRandomValue(50, 255), 255 };//Gera cores aleatórias para as bolas
    }

    while (temp_corrido <= temp_exc)
    {
        temp_corrido += GetFrameTime(); //Tempo desde o último frame
        temp_corrido_frame = GetFrameTime();

        for (i=0; i<num_bolas; i++)
        {
            bolas[i].pos.x += bolas[i].veloci.x * temp_corrido_frame;
            bolas[i].pos.y += bolas[i].veloci.y * temp_corrido_frame;

            if ((bolas[i].pos.x - bolas[i].raio)<0)
            {
                bolas[i].pos.x = bolas[i].raio;
                bolas[i].veloci.x *= -1;
                cont_coli_parede++;
            }
            else if ((bolas[i].pos.x + bolas[i].raio)>largura_janela)
            {
                bolas[i].pos.x = largura_janela - bolas[i].raio;
                bolas[i].veloci.x *= -1;
                cont_coli_parede++;
            }

            if ((bolas[i].pos.y - bolas[i].raio)<0)
            {
                bolas[i].pos.y = bolas[i].raio;
                bolas[i].veloci.y *= -1;
                cont_coli_parede++;

            }
            else if ((bolas[i].pos.y + bolas[i].raio)>altura_janela)
            {
                bolas[i].pos.y = altura_janela - bolas[i].raio;
                bolas[i].veloci.y *= -1;
                cont_coli_parede++;
            }
        }

        for (i=0; i<num_bolas; i++)
        {
            for (j=i+1; j<num_bolas; j++)
            {
                if(ResolveColisaoBolas(&bolas[i], &bolas[j]))
                    cont_coli_bolas++;
            }
        }

        //Desenha
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (i=0; i<num_bolas; i++)
        {
            DrawCircleV(bolas[i].pos, bolas[i].raio, bolas[i].cor);
            DrawCircleLines((int)bolas[i].pos.x, (int)bolas[i].pos.y, bolas[i].raio, BLACK);
        }

        EndDrawing();
    }

    free(bolas);
    CloseWindow();

    printf("\nNumero de vezes que houve colisao com as paredes: %d", cont_coli_parede);
    printf("\nNumero de vezes que houve colisao entre as bolas: %d", cont_coli_bolas);

    return 0;
}
