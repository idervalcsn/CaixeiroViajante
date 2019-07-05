#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>


using namespace std;

struct CustoInsercao{
    int noInserido;
    int arestaRemovida;
    double custo;
};

double ** matrizAdj; // matriz de adjacencia
int dimension; // quantidade total de vertices

bool    comparaCusto(CustoInsercao,CustoInsercao);
int getCusto(vector<int>);
void remover(vector<int> &v, int);
void printData();
vector<int> constructSolution();
vector<CustoInsercao> criaRestrita(double, vector<CustoInsercao>);
vector<CustoInsercao> listaDeCustos(vector<int>,vector<int>);


int main(int argc, char** argv) {



    readData(argc, argv, &dimension, &matrizAdj);
    printData();
    vector<int> solution = constructSolution();

    for(int i = 0; i <= dimension;i++){
        cout << solution[i] << endl;

    }
    cout << getCusto(solution) << endl;

    return 0;

    

}

bool    comparaCusto(CustoInsercao x, CustoInsercao y){
    return x.custo < y.custo;
}


vector<int> constructSolution(){

    srand(time(NULL))
    vector<int> solucao = {1,1};  //Solucao comeca na cidade 1
    vector<int> candidatos;
    int tamanhoSubtour = 4;
    //double custoTotal = matrizAdj[1][1];

    for(int i = 2; i <= dimension; i++ ){ //Populando a lista de candidatos
        int atual = i;
        candidatos.push_back(atual);
    }

    for(int i = 0; i < tamanhoSubtour; i++ ){ //Gerando subtour inicial
        ;
        int j =  rand() % candidatos.size() ;
        solucao.insert(solucao.begin() + 1, candidatos[j] );
        candidatos.erase(candidatos.begin() + j);
    }

    while(!candidatos.empty()){
        vector<CustoInsercao> custos = listaDeCustos(solucao, candidatos);
        sort(custos.begin(), custos.end(),comparaCusto);    //sorta baseado no custo de cada insercao
        vector<CustoInsercao> listaRestrita = criaRestrita(0.5, custos);
        int indiceAleatorio  =  rand() % listaRestrita.size();
        CustoInsercao escolhido = listaRestrita[indiceAleatorio];   //cidade escolhida, onde, e o custo

        solucao.insert(solucao.begin() + (escolhido.arestaRemovida + 1),escolhido.noInserido);
        remover(candidatos,escolhido.noInserido);

    }

    return solucao;


}

vector<CustoInsercao> listaDeCustos(vector<int> lista, vector<int> candidatos){

    vector<CustoInsercao> custo((lista.size() - 1) * candidatos.size());

    for(int i = 0, j = 1, l = 0; i < lista.size() - 1; i++,j++){
        for(auto k: candidatos){
            custo[l].custo = matrizAdj[ lista[i] ][ k ] + matrizAdj[ lista[j] ][k] - matrizAdj[ lista[i] ][ lista[j ]];
            custo[l].noInserido = k;
            custo[l].arestaRemovida = i;
            l++;
        }
    }

    return custo;
}

vector<CustoInsercao> criaRestrita(double alpha, vector<CustoInsercao> lista){
    int size = lista.size();
    double quantos = size * alpha; //quantidade de membros vai estar na lista restrita, baseado na pcentagem
    vector<CustoInsercao> restrita;

    if(quantos < 1){    //quando quantos < 1, nao existe lista restrita a ser criada.
        return lista;
    }

    for(int i = 0; i < quantos; i++){
        restrita.push_back(lista[i]);
    }

    return restrita;

}

void remover(vector<int> &v, int item){   //remove um item de um vetor por valor
    v.erase(remove(v.begin(),v.end(),item),v.end());
}

int getCusto(vector<int> solucao){
    double custo = 0.0;
    for(int i = 0, j = 1; i < solucao.size() - 1; i++,j++){
        custo += matrizAdj[solucao[i]][solucao[j]];
    }
    return custo;
}

void printData() {
  cout << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}
