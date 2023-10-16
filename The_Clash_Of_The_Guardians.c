#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Estructura para representar a un guardian
typedef struct {
    char nombre[50];
    int puntosVida;
    int puntosAtaque;
    int puntosDefensa;
} Guardian;

// Estructura para representar a un jugador
typedef struct {
    char nombre[50];
    int puntosVida;
    Guardian mano[3];
    Guardian campoBatalla[12];
    int numCartasEnMano;
    int numCartasEnCampo;
} Jugador;

// Estructura para presentar a un jugador
typedef struct {
    int numeroTurno;
    char evento[100];
    int contadorHistorial;
} RegistroHistorial;


// Función para cargar los guardianes desde un archivo .txt
void cargarGuardianesDesdeArchivo(char* nombreArchivo, Guardian* guardianes, int* cantidadGuardianes) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s.\n", nombreArchivo);
        exit(1);
    }

    int i = 0;
    char nombreGuardian[100];
    char tipoGuardian[100];
    int pv, pa, pd;

    while (fscanf(archivo, "%99[^,],%99[^,],%d,%d,%d", nombreGuardian, tipoGuardian, &pv, &pa, &pd) == 5) {
    	
        // Copiamos los valores a la estructura Guardian
        strncpy(guardianes[i].nombre, nombreGuardian, sizeof(guardianes[i].nombre));
        guardianes[i].nombre[sizeof(guardianes[i].nombre) - 1] = '\0';
        guardianes[i].puntosVida = pv;
        guardianes[i].puntosAtaque = pa;
        guardianes[i].puntosDefensa = pd;

        i++;
    }

    *cantidadGuardianes = i;
    fclose(archivo);
}

// Función para crear una nueva carta de guardian
Guardian crearGuardian() {
    Guardian nuevoGuardian;
    printf("Ingrese el nombre del nuevo guardian: ");
    scanf("%s", nuevoGuardian.nombre);

    // Leer los valores de PV, PA y PD desde el usuario
    printf("Ingrese puntos de vida (PV): ");
    scanf("%d", &nuevoGuardian.puntosVida);
    printf("Ingrese puntos de ataque (PA): ");
    scanf("%d", &nuevoGuardian.puntosAtaque);
    printf("Ingrese puntos de defensa (PD): ");
    scanf("%d", &nuevoGuardian.puntosDefensa);

    // Validar los valores según los requisitos
    if (nuevoGuardian.puntosVida < 20 || nuevoGuardian.puntosVida > 150) {
        printf("Los puntos de vida deben estar entre 20 y 150.\n");
        return nuevoGuardian;
    }
    if (nuevoGuardian.puntosAtaque < 20 || nuevoGuardian.puntosAtaque > 100) {
        printf("Los puntos de ataque deben estar entre 20 y 100.\n");
        return nuevoGuardian;
    }
    if (nuevoGuardian.puntosDefensa < 10 || nuevoGuardian.puntosDefensa > 75) {
        printf("Los puntos de defensa deben estar entre 10 y 75.\n");
        return nuevoGuardian;
    }

    return nuevoGuardian;
}

// Función para mostrar cartas en el campo de batalla
void mostrarCampoDeBatalla(Jugador* jugador1, Jugador* jugador2) {
    printf("\nCampo de batalla de %s:\n", jugador1->nombre);
    for (int i = 0; i < jugador1->numCartasEnCampo; i++) {
        Guardian* carta = &jugador1->campoBatalla[i];
        printf("%d. %s (PV: %d, PA: %d, PD: %d)\n", i + 1, carta->nombre, carta->puntosVida, carta->puntosAtaque, carta->puntosDefensa);
    }

    printf("\nCampo de batalla de %s:\n", jugador2->nombre);
    for (int i = 0; i < jugador2->numCartasEnCampo; i++) {
        Guardian* carta = &jugador2->campoBatalla[i];
        printf("%d. %s (PV: %d, PA: %d, PD: %d)\n", i + 1, carta->nombre, carta->puntosVida, carta->puntosAtaque, carta->puntosDefensa);
    }
}

void batalla(Guardian* atacante, Guardian* defensor, RegistroHistorial* historial, int* contadorHistorial, int numeroTurno) {
    int danio = atacante->puntosAtaque - defensor->puntosDefensa;
    if (danio <= 0) {
        danio = 1; // Al menos 1 punto de daño
    }

    defensor->puntosVida -= danio;
    printf("%s ataca a %s con %d de daño. %s ahora tiene %d puntos de vida.\n", atacante->nombre, defensor->nombre, danio, defensor->nombre, defensor->puntosVida);
    
    // Registra el evento en el historial
    sprintf(historial[*contadorHistorial].evento, "%s ataca a %s con %d de daño.", atacante->nombre, defensor->nombre, danio);
    historial[*contadorHistorial].numeroTurno = numeroTurno;
    (*contadorHistorial)++;
}

// Función para realizar una acción del jugador
void realizarAccionDelJugador(Jugador* jugador, Jugador* oponente, Guardian* guardianes, int cantidadGuardianes, RegistroHistorial* historial, int* contadorHistorial, int* numeroTurno) {
    printf("\nTurno de %s\n", jugador->nombre);

    // Mostrar las cartas en la mano del jugador
    printf("\nCartas en tu mano:\n");
    for (int i = 0; i < jugador->numCartasEnMano; i++) {
        printf("%d. %s (PV: %d, PA: %d, PD: %d)\n", i + 1, jugador->mano[i].nombre, jugador->mano[i].puntosVida, jugador->mano[i].puntosAtaque, jugador->mano[i].puntosDefensa);
    }

    int opcion;
    printf("\nSelecciona una opcion:\n");
    printf("1. Agregar una carta al campo de batalla\n");
    printf("2. Atacar a un guardian enemigo\n");
    printf("Elije una opcion: ");
    scanf("%d", &opcion);
    switch (opcion) {
    	case 1:
            if (jugador->numCartasEnCampo < 12 && jugador->numCartasEnMano > 0) {
                int indiceCarta = -1;
                printf("Elije el numero de la carta que deseas agregar: ");
                scanf("%d", &indiceCarta);
                if (indiceCarta >= 1 && indiceCarta <= jugador->numCartasEnMano) {
                    // Agregar la carta seleccionada al campo de batalla
                    jugador->campoBatalla[jugador->numCartasEnCampo] = jugador->mano[indiceCarta - 1];
                    jugador->numCartasEnCampo++;
                    // Eliminar la carta de la mano
                    for (int i = indiceCarta - 1; i < jugador->numCartasEnMano - 1; i++) {
                        jugador->mano[i] = jugador->mano[i + 1];
                    }
                    jugador->numCartasEnMano--;
                    printf("Has agregado una carta al campo de batalla.\n");
                } else {
                    printf("Seleccion no valida. Inténtalo de nuevo.\n");
                }
            } else {
                printf("No puedes agregar mas cartas al campo de batalla o no tienes cartas en la mano.\n");
            }
            break;
     
        case 2:
            // Atacar a un guardián enemigo
            if (jugador->numCartasEnCampo > 0 && oponente->numCartasEnCampo > 0) {
                int indiceAtacante, indiceDefensor;
                printf("Elije el numero de la carta que atacara: ");
                scanf("%d", &indiceAtacante);
                printf("Elije el numero de la carta enemiga a la que atacaras: ");
                scanf("%d", &indiceDefensor);

                if (indiceAtacante >= 1 && indiceAtacante <= jugador->numCartasEnCampo && indiceDefensor >= 1 && indiceDefensor <= oponente->numCartasEnCampo) {
                    Guardian* atacante = &jugador->campoBatalla[indiceAtacante - 1];
                    Guardian* defensor = &oponente->campoBatalla[indiceDefensor - 1];

                    // Realizar la batalla entre el atacante y el defensor
                    batalla(atacante, defensor, historial, contadorHistorial, *numeroTurno);

                    // Si el defensor ha sido derrotado, eliminarlo del campo de batalla
                    if (defensor->puntosVida <= 0) {
                        for (int i = indiceDefensor - 1; i < oponente->numCartasEnCampo - 1; i++) {
                            oponente->campoBatalla[i] = oponente->campoBatalla[i + 1];
                        }
                        oponente->numCartasEnCampo--;
                    }
                } else {
                    printf("Seleccion no valida. Intentalo de nuevo.\n");
                }
            } else {
                printf("No puedes atacar o no hay guardianes enemigos para atacar.\n");
            }
            break;
		default:	printf("Selección no valida. Intentalo de nuevo.\n");
            
            break;
		}
void sacarCartaAlFinalDelTurno(Jugador* jugador, Guardian* guardianes, int cantidadGuardianes);
}
// Función para realizar una acción aleatoria durante el turno del programa
void accionAleatoria(Jugador* programa, Jugador* jugador, Guardian* guardianes, int cantidadGuardianes, RegistroHistorial* historial, int* contadorHistorial, int* numeroTurno) {
    int accion = rand() % 6; // 0: Agregar carta, 1: Atacar

    if (accion > 2) {
        // Agregar una carta a la mesa
        if (programa->numCartasEnCampo < 12 && programa->numCartasEnMano > 0) {
            int indiceCarta = rand() % programa->numCartasEnMano;
            programa->campoBatalla[programa->numCartasEnCampo] = programa->mano[indiceCarta];
            programa->numCartasEnCampo++;
            for (int i = indiceCarta; i < programa->numCartasEnMano - 1; i++) {
                programa->mano[i] = programa->mano[i + 1];
            }
            programa->numCartasEnMano--;
            printf("%s agrega una carta a la mesa.\n", programa->nombre);
            // Registra el evento en el historial
            sprintf(historial[*contadorHistorial].evento, "%s agrega una carta a la mesa.", programa->nombre);
            historial[*contadorHistorial].numeroTurno = *numeroTurno;
            (*contadorHistorial)++;
        }
    } else {
        // Atacar a un guardián del jugador
        if (programa->numCartasEnCampo > 0) {
            int indiceGuardianAtacado = rand() % jugador->numCartasEnCampo;
            int indiceCartaAtacante = rand() % programa->numCartasEnCampo;
            Guardian* atacante = &programa->campoBatalla[indiceCartaAtacante];
            Guardian* defensor = &jugador->campoBatalla[indiceGuardianAtacado];

            int danio = atacante->puntosAtaque - defensor->puntosDefensa;
            if (danio <= 0) {
                danio = 1; // Al menos 1 de daño
            }

            defensor->puntosVida -= danio;

            if (defensor->puntosVida <= 0) {
                // El guardián ha sido derrotado
                for (int i = indiceGuardianAtacado; i < jugador->numCartasEnCampo - 1; i++) {
                    jugador->campoBatalla[i] = jugador->campoBatalla[i + 1];
                }
                jugador->numCartasEnCampo--;
                printf("%s ataca a %s con %d de daño. %s ha sido derrotado.\n", programa->nombre, defensor->nombre, danio, defensor->nombre);
                
                // Registra el evento en el historial
                sprintf(historial[*contadorHistorial].evento, "%s ataca a %s con %d de daño. %s ha sido derrotado.", programa->nombre, defensor->nombre, danio, defensor->nombre);
                historial[*contadorHistorial].numeroTurno = *numeroTurno;
                (*contadorHistorial)++;
            } else {
                printf("%s ataca a %s con %d de daño. %s ahora tiene %d puntos de vida.\n", programa->nombre, defensor->nombre, danio, defensor->nombre, defensor->puntosVida);
                
                // Registra el evento en el historial
                sprintf(historial[*contadorHistorial].evento, "%s ataca a %s con %d de daño. %s ahora tiene %d puntos de vida.", programa->nombre, defensor->nombre, danio, defensor->nombre, defensor->puntosVida);
                historial[*contadorHistorial].numeroTurno = *numeroTurno;
                (*contadorHistorial)++;
            }
        }
    }
}

void sacarCartaAlFinalDelTurno(Jugador* jugador, Guardian* guardianes, int cantidadGuardianes) {
    if (jugador->numCartasEnMano < 3) {
        // Generar un índice aleatorio para seleccionar una carta del mazo de guardianes
        int indiceGuardianAleatorio = rand() % cantidadGuardianes;
        Guardian nuevaCarta = guardianes[indiceGuardianAleatorio];

        // Agregar la nueva carta a la mano del jugador
        jugador->mano[jugador->numCartasEnMano] = nuevaCarta;
        jugador->numCartasEnMano++;

        printf("%s ha sacado una nueva carta: %s (PV: %d, PA: %d, PD: %d)\n", jugador->nombre, nuevaCarta.nombre, nuevaCarta.puntosVida, nuevaCarta.puntosAtaque, nuevaCarta.puntosDefensa);
    } else {
        printf("La mano de %s está llena. No puede sacar más cartas.\n", jugador->nombre);
    }
}

void sacarCartaComputadoraAlFinalDelTurno(Jugador* programa, Guardian* guardianes, int cantidadGuardianes) {
    if (programa->numCartasEnMano < 3) {
        // Generar un índice aleatorio para seleccionar una carta del mazo de guardianes
        int indiceGuardianAleatorio = rand() % cantidadGuardianes;
        Guardian nuevaCarta = guardianes[indiceGuardianAleatorio];

        // Agregar la nueva carta a la mano de la computadora
        programa->mano[programa->numCartasEnMano] = nuevaCarta;
        programa->numCartasEnMano++;

        printf("%s ha sacado una nueva carta: %s (PV: %d, PA: %d, PD: %d)\n", programa->nombre, nuevaCarta.nombre, nuevaCarta.puntosVida, nuevaCarta.puntosAtaque, nuevaCarta.puntosDefensa);
    } else {
        printf("La mano de %s está llena. No puede sacar más cartas.\n", programa->nombre);
    }
}

// Función para iniciar el juego
void iniciarJuego(Jugador* jugadorUsuario, Jugador* jugadorPrograma, Guardian* guardianes, int cantidadGuardianes) {
    printf("El juego ha comenzado\n");
    
    int contadorHistorial = 0;
    RegistroHistorial historial[500];
    int numeroTurno = 1;

    // Revolver y repartir cartas a los jugadores
    int cartasRepartidas = 0;
    int cartasTotales = 15;
    int cartasEnMano = 3;

    while (cartasRepartidas < cartasTotales) {
        int indiceGuardianAleatorio = rand() % cantidadGuardianes;
        Guardian carta = guardianes[indiceGuardianAleatorio];

        if (cartasRepartidas < cartasEnMano) {
            if (jugadorUsuario->numCartasEnMano < 3) {
                jugadorUsuario->mano[jugadorUsuario->numCartasEnMano] = carta;
                jugadorUsuario->numCartasEnMano++;
            } else {
                printf("La mano esta llena. No se pueden agregar mas cartas.\n");
            }
        } else {
            if (jugadorPrograma->numCartasEnMano < 3) {
                jugadorPrograma->mano[jugadorPrograma->numCartasEnMano] = carta;
                jugadorPrograma->numCartasEnMano++;
            }
        }
        cartasRepartidas++;
    }

    while (1) {
        // Turno del jugador
        realizarAccionDelJugador(jugadorUsuario, jugadorPrograma, guardianes, cantidadGuardianes, historial, &contadorHistorial, &numeroTurno);
        sacarCartaAlFinalDelTurno(jugadorUsuario, guardianes, cantidadGuardianes);
        // Verifica el estado del juego (por ejemplo, si uno de los jugadores se queda sin puntos de vida)
        if (jugadorUsuario->puntosVida <= 0) {
            printf("%s ha perdido. El programa gana\n", jugadorUsuario->nombre);
            break;
        } else if (jugadorPrograma->puntosVida <= 0) {
            printf("Felicidades Has ganado.\n");
            break;
        }

        // Turno del programa
        accionAleatoria(jugadorPrograma,jugadorUsuario, guardianes, cantidadGuardianes, historial, &contadorHistorial, &numeroTurno);
        sacarCartaComputadoraAlFinalDelTurno(jugadorPrograma, guardianes, cantidadGuardianes);
        mostrarCampoDeBatalla(jugadorUsuario, jugadorPrograma);

        // Verifica el estado del juego después del turno del programa
        if (jugadorUsuario->puntosVida <= 0) {
            printf("%s ha perdido. El programa gana\n", jugadorUsuario->nombre);
            break;
        } else if (jugadorPrograma->puntosVida <= 0) {
            printf("¡Felicidades! Has ganado.\n");
            
            // Muestra el historial de la partida
            printf("\n--- Historial de la partida ---\n");
            for (int i = 0; i < contadorHistorial; i++) {
                printf("Turno %d: %s\n", historial[i].numeroTurno, historial[i].evento);
            }
            printf("\n");

            break;
        }
    }
}

int main() {
    srand(time(NULL)); // Inicializa la semilla para números aleatorios

    Guardian guardianes[60];
    int cantidadGuardianes;
    Jugador jugadorUsuario = {"Usuario", 5}; // Nombre del jugador y puntos de vida iniciales
    Jugador jugadorPrograma = {"Programa", 5}; // Nombre del programa y puntos de vida iniciales
    int contadorHistorial = 0;
    RegistroHistorial historial[500];  

    cargarGuardianesDesdeArchivo("guardianes.txt", guardianes, &cantidadGuardianes);

    int opcion;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Crear una nueva carta de guardian\n");
        printf("2. Comenzar el juego\n");
        printf("3. Para ver el historial\n");
        printf("4. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                // Crear una nueva carta de guardian
                if (jugadorUsuario.numCartasEnMano < 3) {
                    jugadorUsuario.mano[jugadorUsuario.numCartasEnMano] = crearGuardian();
                    jugadorUsuario.numCartasEnMano++;
                } else {
                    printf("La mano esta llena. No se pueden agregar mas cartas.\n");
                }
                break;
            case 2:
                // Iniciar el juego
                iniciarJuego(&jugadorUsuario, &jugadorPrograma, guardianes, cantidadGuardianes);
                break;
            case 3:
            	// Muestra el historial de la partida
                if(contadorHistorial != 0){
                	printf("\n--- Historial de la partida ---\n");
                for (int i = 0; i < contadorHistorial; i++) {
                    printf("Turno %d: %s\n", historial[i].numeroTurno, historial[i].evento);
                }
                printf("\n");
				}else{
					printf("No hay partida para dar un historial");
				}
                break;
            case 4:
                // Salir del juego
                printf("Hasta luego\n");
                return 0;
            default:
                printf("Opcion no valida. Intente de nuevo.\n");
                break;
        }
    }

    return 0;
}

