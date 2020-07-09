
#define __MAX 4




typedef struct{
	int Final;
	double elem[__MAX];
}Tcola;



int cllena (Tcola *q){
	if (q->Final == __MAX-1){
        return 1;
    }else{
        return 0;
    }
}

int cvacia (Tcola *q){
	 if(q->Final == -1){
        return 1;
    }else{
        return 0;
    }
}

void ccrear (Tcola *q){
	 q->Final = -1;
}

void cponer (Tcola *q, double x){
	 q->Final++;
    q->elem[q->Final] = x;
}

void csacar (Tcola *q, double *x){
int i;

    *x = q->elem[0];
    for (i=0; i <= q->Final-1; i++){
        q->elem[i] = q->elem[i+1];
    }
    q->Final = q->Final - 1;
}
