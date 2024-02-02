#ifndef TRABALHOPRATICO_PROJETO_H
#define TRABALHOPRATICO_PROJETO_H

#define MAXNOME 64

struct data{
    int dia;
    int mes;
    int ano;
};
struct socio{
    char nome[MAXNOME];
    struct data nascimento;
    int nif;
    int telefone;
    struct data clube;
    double quotas;
    struct socio *next;
    struct socio *previous; //new
};

struct nomes{
    char nome[MAXNOME];
    struct nomes *next;
};

struct quotas{
    char nome[MAXNOME];
    double sobras;
    struct quotas *next;
};


#endif //TRABALHOPRATICO_PROJETO_H