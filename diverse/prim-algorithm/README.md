# Prim Algorithm



## Echipa proiect

```
Riza Melih
Subțirică Florin Ioan
```

## Asistent laborator

```
Ouatu Andrei Cătălin
```


## Descriere algoritm

Algoritmul lui Prim se utilizează pentru a găsi arborele minim de acoperire din cadrul unui graf conex ponderat.
Rezultatul reprezintă un subgraf care conectează toate nodurile grafului inițial de cost minim total fără a forma
cicluri.

## Planifcare pe saptămâni

### Săptămâna 1

```

Adăugarea codului serial pentru Prim, crerea unui script in python pentru generarea de fisere input pentru grafuri
și unul pentru generarea de plot-uri folosite in Wiki.
Realizarea profiling-ului algoritmului serial pe masina 'haswell' din cadrul cluster-ului.
Update Wiki.

```


### Săptămâna 2

```

Implementarea paralelă a algoritmului folosind OpenMP.
Rularea pe masina 'haswell' din cadrul cluster-ului folosind aceleași input-uri ca în cazul algoritmului serial.
Obținerea timpilor de rulare, comparând execuția pe un număr variabil de thread-uri, respectiv speedup-ul și eficiența.

```


### Săptămâna 3

```

Realizare profiling pentru determinarea motivilui apariției spike-ului la rularea openMP pe 9 thread-uri.
Rularea mai multor configuratii (scheduling + chunck_size) pentru openMP și cuantificarea timpilor obținuți.
Scăderea gradului de paralelizare și analiza performațelor obținute.

```



### Săptămâna 4

```

Adaugarea implemtare MPI si hybrid (MPI + OpenMP) + verificare rezultate algoritm.

```