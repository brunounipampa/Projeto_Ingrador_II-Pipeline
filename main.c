#include <string.h>
#include <stdlib.h>
#include "multiciclo.h"

int main() {
 BancoRegistradores banco_registradores;
    descritor desc;
    MemoriaUnica memoria_unica;
    PC pc; 

    inicializarBancoRegistradores(&banco_registradores);
    inicializePilha(&desc);
    carregarMemoriaUnica(&memoria_unica);

    int opcao = 1;
    while (opcao != 0) {
        opcao = menu();
        switch (opcao) {
            case 1:
                carregarMemoriaUnica();
                break;
            case 2:
                imprimirMemoriaUnica(&memoria_unica);
                break;
            case 3:
                imprimirRegistradores(&banco_registradores);
                break;
            case 4:
                imprimirMemoriaUnica(&memoria_unica);
                imprimirRegistradores(&banco_registradores);
                break;
            case 5:                
                codificarInstrucao((char *)memoria_unica.instrucoes[pc.endereco_atual]);
                break;
            case 6:
                salvar_asm();
                break;
            case 7:
                salvar_data();
                break;
            case 8:
                while (pc.endereco_atual < TAM_MEMORIA) {
                                if (pc.endereco_atual == 0 || banco_registradores.registradores[0] == NULL || memoria_unica.instrucoes[0] == NULL == NULL) {
                        printf("Erro: Ponteiro nulo\n");
                        break;
                    }
                    executarCicloInstrucao(&pc, &banco_registradores, &memoria_unica);
                }
                imprimirMemoriaUnica(&memoria_unica);
                imprimirRegistradores(&banco_registradores);
                printf("programa finalizado\n");
                break;
            case 9:
                save_backup(&pc, &desc, &banco_registradores, &memoria_unica);
                executarCicloInstrucao(&pc, &banco_registradores, &memoria_unica);
                break;
            case 10:
                undo(&pc, &desc, &banco_registradores, &memoria_unica);
                break;
            case 0:
                printf("\nFinalizando o programa...\n\n");
                break;
            default:
                printf("Opcao invalida Escolha uma opcao valida.\n");
        }
    }

    return 0;
}
