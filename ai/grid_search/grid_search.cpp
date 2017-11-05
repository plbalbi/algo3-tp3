#include "grid_search.hpp"
#include <cstdlib>
#include <cmath>

bool avanzar(vector<float> &params, int inf_limit, int sup_limit, float step, uint param_init, uint param_fin){
    for (size_t i = param_init; i < param_fin; i++) {
      if (params[i] >= sup_limit) {
        params[i] = inf_limit;
      }else{
        params[i] += step;
        return true;
      }
    }
    return false;
}

void randomize(pesos& params, int inf_limit, int sup_limit, uint idx_desde, uint idx_hasta) {
    for (uint i= idx_desde; i< idx_hasta; i++){
        float randnum = inf_limit + (sup_limit-inf_limit) * (static_cast <float> (rand()))/( static_cast <float> (RAND_MAX));
        params[i] = randnum;
    }
}

void grid_search::thorough_train(int inf_limit, int sup_limit, float step){
    //Precondiciones, pueden cambiar
    // assert(inf_limit < 0);
    // assert(sup_limit > 0);
    // assert(sup_limit + inf_limit == 0);
    // assert(step > 0);
    float optimal_fitness = 0;

    pesos params((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);
    pesos optimal_weights = params;
    do{
        float result = this->fitness(params);
        if (optimal_fitness <= result){
            optimal_fitness = result;
            optimal_weights = params;
            cout << "NUEVO MAX " << result << endl;
            for (uint i=0; i<optimal_weights.size();i++){
                cout << optimal_weights[i] << "\t ";
            }
            cout << endl << endl << endl;
        }

    } while (avanzar(params, inf_limit, sup_limit, step, 1, (uint)params.size()));
}

void grid_search::minecraft_train(int inf_limit, int sup_limit, float step){
    pesos abs_optimos((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);
    float abs_opt_fitness = 0;
    pesos current_params((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);

    // LA PRIMERA PARTE
    // param_init=0                     param_fin=params.size() - this->c

    pesos parciales1_optimos((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);
    float parcial1_fitness = 0;
    uint param_init=1;
    uint param_fin=current_params.size() - this->c;

    //Tomo rarndom los ultimos valores
    randomize(current_params, inf_limit, sup_limit, param_fin, current_params.size());


    do{
        float result = this->fitness(current_params);
        if (result >= parcial1_fitness){
            parcial1_fitness = result;
            parciales1_optimos = current_params;

            if (result >= abs_opt_fitness) {
                abs_opt_fitness = result;
                abs_optimos = current_params;
                cout << endl << endl << endl;
                cout << "--------------------------------------------------------NUEVO MAX ABSOLUTO" << result << "--------------------------------------------------------" << endl;
                cout << endl << endl << endl;
            }

            cout << endl << endl << endl;
            cout << "NUEVO MAX PARCIAL1" << result << endl;
            cout << endl << endl << endl;
        }
    } while (avanzar(current_params, inf_limit, sup_limit, step, param_init, param_fin));



    // LA SEGUNDA PARTE

    current_params = pesos((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);

    // PARA LOS PRIMEROS fijo los optimos parciales

    for (uint i = param_init; i < param_fin; i++ ){
        current_params[i] = parciales1_optimos[i];
    }


    // param_init=params.size() - this->c      param_fin=params.size()
    pesos parciales2_optimos((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), inf_limit);
    float parcial2_fitness = 0;
    param_init=current_params.size() - this->c;
    param_fin=current_params.size();

    do{
        float result = this->fitness(current_params);
        if (result >= parcial2_fitness){
            parcial2_fitness = result;
            parciales2_optimos = current_params;

            if (result >= abs_opt_fitness) {
                abs_opt_fitness = result;
                abs_optimos = current_params;
                cout << endl << endl << endl;
                cout << "--------------------------------------------------------NUEVO MAX ABSOLUTO" << result << "--------------------------------------------------------" << endl;
                cout << endl << endl << endl;
            }

            cout << endl << endl << endl;
            cout << "NUEVO MAX PARCIAL1" << result << endl;
            cout << endl << endl << endl;
        }
    } while (avanzar(current_params, inf_limit, sup_limit, step, param_init, param_fin));

    cout << "---------------------------------------------------------" << endl;
    cout << "                         RESUMEN                         " << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Optimo Parcial 1 con fitness: " << parcial1_fitness << endl;
    for (uint i=0; i<parciales1_optimos.size(); i++){
        cout << parciales1_optimos[i] << "\t";
    }
    cout << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Optimo Parcial 2 con fitness: " << parcial2_fitness << endl;
    for (uint i=0; i<parciales2_optimos.size(); i++){
        cout << parciales2_optimos[i] << "\t";
    }
    cout << endl;
    cout << "---------------------------------------------------------" << endl;
    cout << "Optimo Absoluto con fitness: " << abs_opt_fitness << endl;
    for (uint i=0; i<abs_optimos.size(); i++){
        cout << abs_optimos[i] << "\t";
    }
    cout << endl;
}


bool grid_search::fitness(pesos& params){
    return regular_fitness(this->columnas, this->filas, this->c, this->p, params);
}

// bool grid_search::fitness(pesos& params, pesos& maximo){
//     return regular_fitness(this->columnas, this->filas, this->c, this->p, params, maximo);
// }




/************************************** EMPIEZO CON EL GRASP, LO DE ARRIBA ESTA MEDIO DESORDENADO **********************************************/



pesos grid_search::get_random_params() {

    pesos params((int) golosa::cuantos_parametros(this->columnas,this->filas,this->c), this->inf_limit);

    //Elijo el 1er elem de esta lista
    vector<int> posibles_inicios;
    for (int i=-1; i<(int)this->columnas; i++){
        posibles_inicios.push_back(i);
    }

    params[0] = posibles_inicios[rand() % posibles_inicios.size()];

    //Elijo todo el resto de aca
    vector<float> posibles_pesos;
    for (float i = this->inf_limit; i <= this->sup_limit; i += this->step ){
        posibles_pesos.push_back(i);
    }

    for (uint i= 1; i< params.size(); i++){
        params[i] = posibles_pesos[rand() % posibles_pesos.size()];
    }

    return params;
}


vector<golosa> grid_search::get_neighbors_golosos(const pesos& params){
    // Retorna los vecinos golosos y a si mismo

    vector<pesos> neighbors;
    if (params[0]> -1) {
        pesos copy = params;
        copy[0] = params[0] -1;
        neighbors.push_back(copy);
    }
    if (params[0]<(int)this->columnas) {
        pesos copy = params;
        copy[0] = params[0] +1;
        neighbors.push_back(copy);
    }

    for(uint i=1; i<params.size(); i++) {
        if (params[i] - this->step >= this->inf_limit) {
            pesos copy = params;
            copy[i] = params[i] - this->step;
            neighbors.push_back(copy);
        }
        if (params[i] + this->step <= this->sup_limit) {
            pesos copy = params;
            copy[i] = params[i] +this->step;
            neighbors.push_back(copy);
        }
    }

    // paso el vector de pesos a vector de golosa

    vector<golosa> golosos;
    for(uint i=0;i<neighbors.size();i++){
        golosos.push_back(golosa(neighbors[i], this->columnas, this->filas, this->c));
    }

    golosos.push_back(golosa(params, this->columnas, this->filas, this->c));
    return golosos;
}
