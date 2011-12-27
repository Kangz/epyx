#include "stdio.h"

#include "core/routine.h"

ROUTINE_DECLARE(NiceRoutine, int _i)

    ROUTINE_LOCAL_VARS
    int i;

ROUTINE_DECLARE_END



ROUTINE_INIT(NiceRoutine, int _i)
    i = _i;
ROUTINE_INIT_END

ROUTINE_MAIN(NiceRoutine)
    printf("Started a Routine\n");
    while(1){
        printf("%i\n", i++);
        ROUTINE_YIELD;
        printf("Wait please !\n");
        ROUTINE_YIELD;
    }
ROUTINE_MAIN_END


//TODO add a nicer cast
int main(){
    Epyx::Routine* f = (Epyx::Routine*) new NiceRoutine(0);
    Epyx::Routine& g = *f;
    g();
    g();
    g();
    g();
    g();
}
